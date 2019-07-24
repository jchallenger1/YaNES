#include "tests.hpp"
#include <iostream>

#include "Disassembler6502.hpp"
#include "Memory.hpp"
#include "State6502.hpp"
#include "Ppu.hpp"
#include "GamePak.hpp"

void testenv() {
    Memory memory;
    Ppu ppu;
    GamePak::load(memory, ppu, "/home/challenger/Desktop/Donkey Kong (World) (Rev A).nes");
    ppu.getTile(0,0);

}
