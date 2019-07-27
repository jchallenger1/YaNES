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

uint8_t Ppu::readRegister(const uint16_t& adr) {
    return 0; UNUSED(adr);
}

void Ppu::writeRegister(const uint16_t& adr, const uint8_t& val) {
    UNUSED(adr); UNUSED(val);
}











uint8_t Ppu::PPUCTRL::asByte() const noexcept {
    uint8_t byte = 0;
    byte |= nameTable;
    byte |= increment << 2;
    byte |= spriteTile << 3;
    byte |= bkgrdTile << 4;
    byte |= spriteSz << 5;
    byte |= masterSlave << 6;
    byte |= NMI << 7;
    return byte;
}

void Ppu::PPUCTRL::fromByte(const uint8_t& byte) noexcept {
    nameTable = byte & 0b11;
    increment = (byte & 0b100) >> 2;
    spriteTile = (byte & 0x8) >> 3;
    bkgrdTile = (byte & 0x10) >> 4;
    spriteSz = (byte & 0x20) >> 5;
    masterSlave = (byte & 0x40) >> 6;
    NMI = (byte & 0x80) >> 7;
}

uint8_t Ppu::PPUMASK::asByte() const noexcept {
    uint8_t byte = 0;
    byte |= greyScale;
    byte |= bkgrdLeftEnable << 1;
    byte |= spriteLeftEnable << 2;
    byte |= bkgrdEnable << 3;
    byte |= spriteEnable << 4;
    byte |= red << 5;
    byte |= green << 6;
    byte |= blue << 7;
    return byte;
}

void Ppu::PPUMASK::fromByte(const uint8_t& byte) noexcept {
    greyScale = byte & 1;
    bkgrdLeftEnable = (byte & 2) >> 1;
    spriteLeftEnable = (byte & 0b100) >> 2;
    bkgrdEnable = (byte & 0x8) >> 3;
    spriteEnable = (byte & 0x10) >> 4;
    red = (byte & 0x20) >> 5;
    green = (byte & 0x40) >> 6;
    blue = (byte & 0x80) >> 7;
}

uint8_t Ppu::PPUSTATUS::asByte() const noexcept {
    uint8_t byte = 0;
    byte |= sOverflow << 5;
    byte |= sprite0Hit << 6;
    byte |= vblank << 7;
    return byte;
}

void Ppu::PPUSTATUS::fromByte(const uint8_t& byte) noexcept {
    sOverflow = (byte & 0x20) >> 5;
    sprite0Hit = (byte & 0x40) >> 6;
    vblank = (byte & 0x80) >> 7;
}
