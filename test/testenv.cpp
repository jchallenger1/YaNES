#include "tests.hpp"
#include <iostream>

#include "Memory.hpp"
#include "Ppu.hpp"
#include "Cpu6502.hpp"
#include "GamePak.hpp"
#include "NES.hpp"

void Tests::testenv() {
    Memory memory;
    Ppu ppu;
    Cpu6502 cpu;
    NES nes(cpu, ppu);
    GamePak::load(memory, ppu, "/home/challenger/Desktop/Donkey Kong (World) (Rev A).nes");
    ppu.stdDrawPatternTile(0x1000 + 16 * 0xF);
}
