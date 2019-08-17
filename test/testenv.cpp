#include "tests.hpp"
#include <iostream>

#include "Memory.hpp"
#include "Ppu.hpp"
#include "GamePak.hpp"

// https://wiki.nesdev.com/w/index.php/PPU_attribute_tables
// Get the Attribute Table Address from a nametable address
// NameTableAdr must NOT be the actual address, but the relative one to the name table source
// Ex if nametable Address = 0x2005, then relative address is 0x5, Similarly 0x3325 -> 0x125
// atrTableStart must be the origin location attribute table to the particular nametable
// Ex if nameTable Adr = 0x2000, then atrTableStart = 0x23C0
uint16_t getAtrAddress(const uint16_t& nameTableRelativeAdr, const uint16_t& atrTableStart) {
    // TODO : Make it such that relative addresses and the attribute table starts are unneeded for the function
    // Do when donkey kong works
    uint8_t hTableLevel = static_cast<uint8_t>( (nameTableRelativeAdr % 32) / 4); // Get horizontal table location, there is 32 bytes in a partial vertical line, and 4 numbers routes to one (0,1,2,3->0)
    uint8_t vTableLevel = static_cast<uint8_t>( nameTableRelativeAdr / 64); // Get vertical, The next vertical tile occurs every 64 bytes
    uint16_t adr = atrTableStart + hTableLevel + vTableLevel * 8;
    return adr;
}

// Function gets the shift from a attribute table needed for the correct palette selection
uint8_t getShift(const uint16_t& nameTableRelativeAdr) {
    // Compress all vertical tiles into one vertical tile (%64), if the address is divisable by 32 then it's in the upper half of tile, otherwise in lower half
    if (static_cast<int>( (nameTableRelativeAdr % 64) / 32) == 0) {
        switch(nameTableRelativeAdr % 4){ // Top Half
            case 0: case 1:
                return 0;
            case 2: case 3:
                return 2;
        }
    }
    else {
        switch(nameTableRelativeAdr % 4) { // Lower half
            case 0: case 1:
                return 4;
            case 2: case 3:
                return 6;
        }
    }
    std::cerr << "In " << __FUNCTION__ << " in " << __FILE__ << " Reached end of function without a shift, Relative Address : " << nameTableRelativeAdr << "\n";
    throw std::runtime_error("Could not get a shift from name table address");
}

// https://wiki.nesdev.com/w/index.php/PPU_attribute_tables
// Gets Palette selection from a nametable address
uint8_t getPaletteFromNameTable(const uint16_t& nameTableRelativeAdr, const uint16_t& atrTableStart) {
    uint8_t byte = /*vRamRead*/(getAtrAddress(nameTableRelativeAdr, atrTableStart));

    switch(getShift(nameTableRelativeAdr)) {
        case 0:
            return byte & 0x3;
        case 2:
            return byte & 0xC;
        case 4:
            return byte & 0x30;
        case 6:
            return byte & 0xC0;
    }

    std::cerr << "In " << __FUNCTION__ << " in " << __FILE__ << " Reached end of function without a palett, Relative Address : " << nameTableRelativeAdr << "\n";
    throw std::runtime_error("Could not get a shift from name table address");
}

void Tests::testenv() {
    Memory memory;
    Ppu ppu;
    GamePak::load(memory, ppu, "/home/challenger/Desktop/Donkey Kong (World) (Rev A).nes");

    std::cerr<< (int)getShift(128+2+32, 0) << std::endl;
}
