#include "GamePak.hpp"
#include "Memory.hpp"
#include "Ppu.hpp"
#include <string>
#include <fstream>
#include <iostream>

#define UNUSED(x) (void)(x)

// Refer to https://wiki.nesdev.com/w/index.php/CPU_memory_map and
//  https://wiki.nesdev.com/w/index.php/INES
GamePak cpuLoad(Memory& memory, std::ifstream& ifs) {
    GamePak gamepak;

    long sizeOfFile = ifs.tellg();
    ifs.seekg( ifs.beg );
    UNUSED(sizeOfFile);

    uint8_t byte;
    auto read = [&ifs, &byte]() -> uint8_t {
            ifs.read(reinterpret_cast<char*>(&byte), sizeof (uint8_t) );
            return byte;
    };
    bool isNESFile = read() == 'N' && read() == 'E' && read() == 'S' && read() == 0x1A;
    if (!isNESFile) {
        std::cerr << "Given file is not an NES 2.0 file" << std::endl;
        throw std::runtime_error("Unsupported file type");
    }

    gamepak.PRG_ROM_sz = read(); // in 16kb units
    gamepak.CHR_ROM_sz = read(); // in 8 kb units
    uint8_t flags6 = read();
    uint8_t mapperNum = (flags6 & 0xF0) >> 4;


    // these flags will be used later on with other mappers
    gamepak.flags7 = read();
    gamepak.flags8 = read();
    gamepak.flags9 = read();
    gamepak.flags10 = read();

    for (short i = 0; i != 5; i++) // skip padding of header
        read();

    if (mapperNum != 0) {
        std::cerr << "Non NROM not supported" << std::endl;
        throw std::runtime_error("Unsupported Mapper type");
    }

    long startOfPROM = ifs.tellg();

    // Write PRG ROM @ 0x8000
    for (uint16_t index = 0x8000, times = 0; times != gamepak.PRG_ROM_sz * 16384; times++, index++) {
        memory[index] = read();
    }
    // In NROM 128, after 0xBFFF is just a mirror of the rom of 0x8000 - 0xBFFF
    // For now most mirrors will actually write to memory
    ifs.seekg(startOfPROM);
    for (uint16_t index = 0xC000, times = 0; times != gamepak.PRG_ROM_sz * 16384; times++, index++) {
        memory[index] = read();
    }

    return gamepak;
}

GamePak GamePak::load(Memory& memory, Ppu& ppu, const std::string& fname) {
    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "File not found" << std::endl;
        throw std::runtime_error("File not found, given path:" + fname);
    }
    GamePak pak = cpuLoad(memory, ifs);
    UNUSED(ppu);
    return pak;
}

GamePak GamePak::load(Memory& memory, const std::string& fname) {
    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "File not found" << std::endl;
        throw std::runtime_error("File not found, given path:" + fname);
    }
    return cpuLoad(memory, ifs);
}
