#include <iostream>
#include <sstream>
#include <numeric>
#include <queue>
#include "NES.h"
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
    cpu.runCycle();
    for (int var = 0; var < 3; ++var) {
        ppu.runCycle();
    }
}

void NES::powerUp() {
    cpu.signalRESET();
}
