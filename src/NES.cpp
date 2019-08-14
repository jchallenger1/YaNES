#include <iostream>
#include <sstream>
#include <numeric>
#include <queue>
#include "NES.hpp"
#include "functions.hpp" // toHex()

NES::NES(Cpu6502& cpu, Ppu& ppu) : cpu(cpu), ppu(ppu) {
    ppu.setNESHandle(*this);
    cpu.memory.setNESHandle(*this);
}


void NES::clear() {
    ppu.clear();
    cpu.clear();
}


void NES::step() {
    static std::queue<std::string> queue;
    constexpr uint8_t stepMemory = 15;
    try {
        cpu.runCycle();
        for (int i = 0; i != 3; i++)
            ppu.runCycle();

        if (queue.size() >= stepMemory)
            queue.pop();

        //std::cerr << "opcode " + toHex(cpu.memory[cpu.pc]) + " pc " + toHex(cpu.pc) << "\n";
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
