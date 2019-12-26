#include <iostream>
#include <numeric>
#include <tuple>
#include <queue>
#include "NES.h"
#include "functions.hpp" // toHex()

void NES::init() {
    ppu.setNESHandle(getPtr());
    cpu.memory.setNESHandle(getPtr());
}

std::shared_ptr<NES> NES::getPtr() {
    return shared_from_this();
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

void NES::addVideoData(const uint8_t& x, const uint8_t& y, const uint8_t& chroma) {
    screen[y][x] = chroma;
}
