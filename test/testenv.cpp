#include "tests.hpp"
#include <iostream>

#include "Memory.hpp"
#include "Ppu.hpp"
#include "GamePak.hpp"

void Tests::testenv() {
    Memory memory;
    Ppu ppu;
    GamePak::load(memory, ppu, "/home/challenger/Desktop/Donkey Kong (World) (Rev A).nes");
}
