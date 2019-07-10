#include "Memory.hpp"



inline bool inRange(const uint8_t& min, const uint8_t& max, const uint8_t& val) {
    return val <= max && val >= min;
}

Memory::Memory() {
    ;
}

// These two functions will be much more in depth due to mirrors later.
uint8_t Memory::read(const uint16_t& adr) const {
    return memory[adr];
}

void Memory::write(const uint16_t& adr, const uint8_t& val) {
    memory[adr] = val;
}

void Memory::clear() {
    memory.fill(0);
}
