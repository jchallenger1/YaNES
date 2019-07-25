#include "Ppu.hpp"
#include <bitset>
#include <iostream>

#define UNUSED(x) (void)(x)

Ppu::Ppu() {

}

inline uint8_t getBit(const uint16_t& val, const uint8_t& bitNum) {
    return static_cast<uint8_t>((val & (1 << bitNum)) >> bitNum);
}

inline void setBit(uint16_t& val, const uint8_t& bitNum) {
    val |= (1 << bitNum);
}

uint16_t createLine(uint8_t left, uint8_t right) {
    uint16_t line = 0;
    for (uint8_t i = 0; i != 8; i++) {
        if (!getBit(left, i) && !getBit(right, i))
            setBit(line, 2 * i);
        if (getBit(left, i) && getBit(right, i)) {
            setBit(line, 2 * i);
            setBit(line, 2 * i + 1);
        }
        if (getBit(left, i) == 1 && !getBit(right,i))
            setBit(line, 2 * i);
        else
            setBit(line, 2 * i + 1);
    }
    return line;
}

void printTile(std::array<uint16_t, 8>& tile) {
    for (auto begin = tile.cbegin(); begin != tile.cend(); begin++) {
        std::bitset<16> p(*begin);
        std::cout << p << "\n";
    }
    std::cout << std::endl;
}

std::array<uint16_t, 8> Ppu::getTile(unsigned x, unsigned y) {
    std::array<uint16_t, 8> tile{};
    UNUSED(x); UNUSED(y);
    for (unsigned i = 0; i != 8; i++) {
        tile[i] = createLine(memory[0x1000 + i], memory[0x1000 + i + 8]);
    }
    printTile(tile);
    return tile;
}
