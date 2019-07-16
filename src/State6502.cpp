#include "State6502.hpp"



void State6502::clear() {
    a = x = y = 0;
    sp = 0;
    pc = 0;
    cycleCount = 0;
    status.clear();
    memory.clear();
}



State6502::Status::Status() {
    reset();
}

void State6502::Status::clear() noexcept {
    c = z = i = d = b = pad = o = n = 0;
}

void State6502::Status::reset() noexcept {
    i = z = 1;
    n = o = b = d = c = 0;
}

uint8_t State6502::Status::asByte() const noexcept {
    uint8_t byte = 0x20; // set bit 5, always on
    byte |= c;
    byte |= z << 1;
    byte |= i << 2;
    byte |= d << 3;
    byte |= b << 4;
    // 5 is padding
    byte |= o << 6;
    byte |= n << 7;
    return byte;
}

void State6502::Status::fromByte(const uint8_t& byte) noexcept {
    c = byte & 1;
    z = (byte & 2) >> 1;
    i = (byte & 4) >> 2;
    d = (byte & 0xF) >> 3;
    o = (byte & 0x40) >> 6;
    n = (byte & 0x80) >> 7;
}
