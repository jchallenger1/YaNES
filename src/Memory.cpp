#include "Memory.hpp"

#include <fstream>
#include <iostream>

#define UNUSED(x) (void)(x)

inline bool inRange(const uint16_t& min, const uint16_t& max, const uint16_t& val) {
    return val <= max && val >= min;
}

Memory::Memory() {
    ;
}


uint8_t Memory::read(const uint16_t& adr) const {
    if (inRange(0x0800, 0x0FFF, adr)) // ram mirror, repeats every 0x0800
        return memory[adr % 0x0800];
    else if (inRange(0x2008, 0x3FFF, adr)) // nes ppu register mirrors, repeats every 0x8
        return memory[0x2000 + adr % 8];
    else
        return memory[adr];
}

void Memory::write(const uint16_t& adr, const uint8_t& val) {
    if (inRange(0x0800, 0x0FFF, adr)) // ram mirror, repeats every 0x0800
        memory[adr % 0x0800] = val;
    else if (inRange(0x2008, 0x3FFF, adr)) // nes ppu register mirrors, repeats every 0x8
        memory[0x2000 + adr % 8] = val;
    else
        memory[adr] = val;
}

void Memory::clear() {
    memory.fill(0);
}


uint8_t& Memory::operator[](const size_t& index) {
    return memory[index];
}

const uint8_t& Memory::operator[](const size_t& index) const {
    return memory[index];
}

 // Refer to https://wiki.nesdev.com/w/index.php/CPU_memory_map and
//  https://wiki.nesdev.com/w/index.php/INES
void Memory::fromFile(const std::string& fname) {
    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "File not found" << std::endl;
        throw std::runtime_error("File not found, given path:" + fname);
    }

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

    uint8_t PRG_ROM = read(); // in 16kb units
    uint8_t CHR_ROM = read(); // in 8 kb units
    UNUSED(CHR_ROM);
    uint8_t flags6 = read();
    uint8_t mapperNum = (flags6 & 0xF0) >> 4;

    if (mapperNum != 0) {
        std::cerr << "Non NROM not supported" << std::endl;
        throw std::runtime_error("Unsupported Mapper type");
    }

    // these flags will be used later on with other mappers
    uint8_t flags7 = read();
    uint8_t flags8 = read();
    uint8_t flags9 = read();
    uint8_t flags10 = read();
    UNUSED(flags7); UNUSED(flags8); UNUSED(flags9); UNUSED(flags10);

    for (short i = 0; i != 5; i++) // skip padding of header
        read();

    long startOfPROM = ifs.tellg();

    // Write PRG ROM @ 0x8000
    for (uint16_t index = 0x8000, times = 0; times != PRG_ROM * 16384; times++, index++) {
        memory[index] = read();
    }
    // In NROM 128, after 0xBFFF is just a mirror of the rom of 0x8000 - 0xBFFF
    // For now most mirrors will actually write to memory
    ifs.seekg(startOfPROM);
    for (uint16_t index = 0xC000, times = 0; times != PRG_ROM * 16384; times++, index++) {
        memory[index] = read();
    }
}
