#ifndef PPU_HPP
#define PPU_HPP

#include "GamePak.hpp"
#include <array>

class Ppu {
    friend struct GamePak;
public:
    Ppu();

private:
    // Left and Right Pattern Tables both 4kb
    std::array<uint8_t, GamePak::KB4> leftPatternT{};
    std::array<uint8_t, GamePak::KB4> rightPatternT{};
};

#endif // PPU_HPP
