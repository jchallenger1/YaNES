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

    std::array<uint8_t, GamePak::KB16> memory{};
    std::array<uint8_t, 0xFF> OAM{};
};

#endif // PPU_HPP
