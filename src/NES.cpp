#include <iostream>
#include <numeric>
#include <queue>
#include "NES.h"
#include "functions.hpp" // toHex()

NES::NES() : thisPtr(this){
    cpu.memory.setNESHandle(thisPtr);
    ppu.setNESHandle(thisPtr);
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
    pixelsToAdd.push(pixel);
    videoRequested = true;
}

void NES::removeRequest() {
    //videoRequested = false;
}

bool NES::videoRequest() const {
    return videoRequested;
}
