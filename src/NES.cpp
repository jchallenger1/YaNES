#include "NES.hpp"

NES::NES(Cpu6502& cpu, Ppu& ppu) : cpu(cpu), ppu(ppu) {
    ppu.setNESHandle(*this);
    cpu.memory.setNESHandle(*this);
}


void NES::clear() {
    ppu.clear();
    cpu.clear();
}

void NES::step() {
    cpu.runCycle();
    //ppu.runCycle();
    //ppu.runCycle();
    //ppu.runCycle();
}
