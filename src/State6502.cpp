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
