#ifndef GAMEPAK_HPP
#define GAMEPAK_HPP

#include <cstdint>
#include <string>

class Memory;
class Ppu;

struct GamePak {
    static constexpr uint16_t KB4 = 4096;
    static constexpr uint16_t KB8 = 8192;
    static constexpr uint16_t KB16 = 16384;
    uint8_t PRG_ROM_sz; // Program Read only memory in 16kb size
    uint8_t CHR_ROM_sz; // Character Read only memory in 8kb size
    uint8_t mapper; // Mapper number
    uint8_t flags7, flags8, flags9, flags10; // flags used in header, currently unused in this project
    static GamePak load(Memory& memory, Ppu& ppu, const std::string& fname);
    static GamePak load(Memory& memory, const std::string& fname);
};

#endif // GAMEPAK_HPP
