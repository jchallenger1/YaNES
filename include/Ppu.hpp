#ifndef PPU_HPP
#define PPU_HPP

#include "GamePak.hpp"
#include <array>

class Ppu {
    friend struct GamePak;
public:
    Ppu();

    std::array<uint16_t, 8> getTile(unsigned x, unsigned y);

private:


    // 8 Registers that are exposed to the cpu
    // Descriptions here : https://wiki.nesdev.com/w/index.php/PPU_registers
    uint8_t PPUCTRL = 0;
    uint8_t PPUMASK = 0;
    uint8_t PPUSTATUS = 0;
    uint8_t OAMADDR = 0;
    uint8_t OAMDATA = 0;
    uint8_t PPUSCROLL = 0;
    uint8_t PPUADDR = 0;
    uint8_t PPUDATA = 0;
    uint8_t OAMDMA = 0;


    std::array<uint8_t, GamePak::KB16> memory{};
    // Oam is list of 64 sprites, each having info of 4 bytes
    // Description of each byte : https://wiki.nesdev.com/w/index.php/PPU_OAM
    std::array<uint8_t, 0xFF> OAM{};
};

#endif // PPU_HPP