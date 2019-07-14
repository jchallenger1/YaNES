#include "State6502.hpp"



void State6502::clear() {
    a = x = y = 0;
    sp = 0;
    pc = 0;
    cycleCount = 0;
    status.clear();
    memory.clear();
}


void State6502::Status::clear() noexcept {
    c = z = i = d = b = pad = o = n = 0;
}

uint8_t State6502::Status::asByte() const noexcept {
    uint8_t byte = 0;
    byte |= c;
    byte |= z << 1;
    byte |= i << 2;
    byte |= d << 3;
    // 4 & 5 are the B(not break) flag that has no CPU effect. https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
    byte |= o << 6;
    byte |= n << 7;
    return byte;
}
