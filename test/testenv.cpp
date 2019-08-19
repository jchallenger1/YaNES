#include "tests.hpp"
#include <iostream>

#include "Memory.h"
#include "Ppu.h"
#include "Cpu6502.h"
#include "GamePak.h"
#include "NES.h"

void Tests::testenv() {
    Memory memory;
    Ppu ppu;
    Cpu6502 cpu;
    NES nes(cpu, ppu);
    GamePak::load(memory, ppu, "/home/challenger/Desktop/Donkey Kong (World) (Rev A).nes");
    ppu.stdDrawPatternTile(0x1000 + 16 * 0xF);
}
