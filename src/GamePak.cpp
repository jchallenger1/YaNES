#include "GamePak.h"
#include "Memory.h"
#include "Ppu.h"
#include <string>
#include <fstream>
#include <iostream>

#include "functions.hpp"
#include "NES.h"

GamePak::GamePak(std::shared_ptr<NES> nesptr) {
    nes = nesptr;
}

void GamePak::setNESHandle(std::shared_ptr<NES> nes) & {
    this->nes = nes;
}

void GamePak::load(const std::string& fname) {
    if (!nes) {
        throw std::runtime_error("Gamepak must have a NES handle before loading a file.");
    }

    GamePak gamepak = load(nes->cpu.memory, nes->ppu, fname);
    std::swap(gamepak, *this);
}


GamePak GamePak::load(Memory& memory, Ppu& ppu, const std::string& fname) {
    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "File not found" << std::endl;
        throw std::runtime_error("File not found, given path:" + fname);
    }
    GamePak pak = cpuLoad(memory, ifs);
    // Remaining bytes are CHR_ROM
    uint8_t byte;
    auto read = [&ifs, &byte]() -> uint8_t {
            ifs.read(reinterpret_cast<char*>(&byte), sizeof (uint8_t) );
            return byte;
    };
    // Load the bytes of CHROM one to one onto the PPU memory bus
    for (unsigned index = 0; index != memsize::KB16; index++) {
        ppu.memory.at(index) = read();
    }

    return pak;
}


// This loading scheme is used only by the
GamePak GamePak::load(Memory& memory, const std::string& fname) {
    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "File not found" << std::endl;
        throw std::runtime_error("File not found, given path:" + fname);
    }
    return cpuLoad(memory, ifs);
}



// Function is NOT apart of Gamepak itself
// Refer to https://wiki.nesdev.com/w/index.php/CPU_memory_map and
//  https://wiki.nesdev.com/w/index.php/INES
GamePak GamePak::cpuLoad(Memory& memory, std::ifstream& ifs) {
    GamePak gamepak;

    // Get file size, might be helpful for later in the project
    long sizeOfFile = ifs.tellg();
    ifs.seekg( ifs.beg );
    UNUSED(sizeOfFile);

    // Read function to grab a single byte from the file
    uint8_t byte;
    auto read = [&ifs, &byte]() -> uint8_t {
            ifs.read(reinterpret_cast<char*>(&byte), sizeof (uint8_t) );
            return byte;
    };
    // Check if the iNES header contains the NES bytes to check if its a correct file
    bool isNESFile = read() == 'N' && read() == 'E' && read() == 'S' && read() == 0x1A;
    if (!isNESFile) {
        std::cerr << "Given file is not an NES 2.0 file" << std::endl;
        throw std::runtime_error("Unsupported file type");
    }
    // Get flags

    gamepak.PRG_ROM_sz = read(); // in 16kb units
    gamepak.CHR_ROM_sz = read(); // in 8 kb units
    uint8_t flags6 = read();
    uint8_t mapperNum = (flags6 & 0xF0) >> 4;
    gamepak.mirror = static_cast<GamePak::MIRRORT>(flags6 & 1);

    // these flags will be used later on with other mappers
    gamepak.flags7 = read();
    gamepak.flags8 = read();
    gamepak.flags9 = read();
    gamepak.flags10 = read();
    // skip padding of header
    for (short i = 0; i != 5; i++)
        read();

    // Only supporting NROM
    if (mapperNum != 0) {
        std::cerr << "Non NROM not supported" << std::endl;
        throw std::runtime_error("Unsupported Mapper type");
    }

    long startOfPROM = ifs.tellg();

    // Write PRG ROM @ 0x8000
    for (uint16_t index = 0x8000, times = 0; times != gamepak.PRG_ROM_sz * memsize::KB16; times++, index++) {
        memory[index] = read();
    }
    // In NROM 128, after 0xBFFF is just a mirror of the rom of 0x8000 - 0xBFFF
    // For now most mirrors will actually write to memory
    ifs.seekg(startOfPROM);
    for (uint16_t index = 0xC000, times = 0; times != gamepak.PRG_ROM_sz * memsize::KB16; times++, index++) {
        memory[index] = read();
    }

    return gamepak;
}



