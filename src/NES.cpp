#include <iostream>
#include <numeric>
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

void NES::addVideoData(const PixelT& pixel) {
    //pixelsToAdd.push(pixel);
    videoRequested = true;
}

void NES::removeRequest() {
    //videoRequested = false;
}

bool NES::videoRequest() const {
    return videoRequested;
}
