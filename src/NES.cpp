#include <iostream>
#include <sstream>
#include <queue>
#include "NES.hpp"

NES::NES(Cpu6502& cpu, Ppu& ppu) : cpu(cpu), ppu(ppu) {
    ppu.setNESHandle(*this);
    cpu.memory.setNESHandle(*this);
}


void NES::clear() {
    ppu.clear();
    cpu.clear();
}

template<typename T>
std::string toHex(T&& num) {
    static_assert (std::is_integral<typename std::decay<T>::type>::value, "must be an integral type");
    std::stringstream stream;
    stream << "0x" << std::hex << static_cast<int>(std::forward<T>(num));
    return stream.str();
}

void NES::step() {
    static std::queue<std::string> queue;
    constexpr uint8_t stepMemory = 15;
    try {
        cpu.runCycle();
        /* for (int i = 0; i != 3; i++)
         *    ppu.runCycle();
         */
        if (queue.size() >= stepMemory)
            queue.pop();
        queue.emplace("opcode " + toHex(cpu.memory[cpu.pc]) + " pc " + toHex(cpu.pc));
    } catch (const std::exception& e) {
        std::cerr << "\n" << e.what() << std::endl;
        std::cerr << "Last " << std::to_string(stepMemory) << " : \n";
        while (!queue.empty()) {
            std::cerr << queue.front() << "\n";
            queue.pop();
        }
        std::cerr << std::endl;
        std::abort();
    }
}

void NES::powerUp() {
    cpu.signalRESET();
}
