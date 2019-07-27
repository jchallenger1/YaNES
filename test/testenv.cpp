#include "tests.hpp"
#include <iostream>

#include "Memory.hpp"
#include "Ppu.hpp"
#include "GamePak.hpp"

void testenv() {
    Memory memory;
    Ppu ppu;
    GamePak::load(memory, ppu, "/home/challenger/Desktop/Donkey Kong (World) (Rev A).nes");
    ppu.getTile(0,0);

}
