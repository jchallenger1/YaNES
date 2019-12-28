#include <iostream>
#include <tuple>
#include <algorithm>
#include "NES.h"
#include "functions.hpp" // toHex()

// Must be called right after the constructor, cannot be in the constructor
// because at that point this is not a shared pointer and will throw
// Sets handles to the ppu and cpu
void NES::init() {
    ppu.setNESHandle(getPtr());
    cpu.memory.setNESHandle(getPtr());
    gamepak.setNESHandle(getPtr());
}

// For this function to be used, the this pointer must
// already be a shared pointer, otherwise bad_weak_ptr will be thrown
std::shared_ptr<NES> NES::getPtr() {
    return shared_from_this();
}

void NES::load(const std::string& fname) {
    gamepak.load(fname);
    // Get the basename of the file
    auto slash = std::find(fname.crbegin(), fname.crend(), '/');
    if (slash == fname.crend()) {
        fileName = fname;
    }
    else {
        fileName = std::string(slash.base(), fname.cend());
    }
}

void NES::clear() {
    ppu.clear();
    cpu.clear();
}


void NES::step() {
    cpu.runCycle();
    // Ppu runs 3x as fast as cpu
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
