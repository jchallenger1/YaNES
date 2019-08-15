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
    uint8_t hTableLevel = static_cast<uint8_t>( (nameTableRelativeAdr % 32) / 4);
    uint8_t vTableLevel = static_cast<uint8_t>( nameTableRelativeAdr / 64);
    uint16_t adr = hTableLevel + vTableLevel * 8 + atrTableStart;
    return adr;
}

uint8_t getShift(const uint16_t& nameTableRelativeAdr, const uint16_t& atrTableStart) {
    uint8_t divBy4 = static_cast<uint8_t>( (nameTableRelativeAdr % 32) / 4);
    uint8_t tableLevel = static_cast<uint8_t>( nameTableRelativeAdr / 64);
    uint16_t adr = divBy4 + tableLevel * 8 + atrTableStart;
    uint8_t byte;
    // uint8_t byte = memory.read(adr);
    switch(nameTableRelativeAdr) {
        case 0: case 1:
            return 0;
        case 2: case 3:
            return 2;
        default:
            throw std::runtime_error("");
    }

}

void Tests::testenv() {
    Memory memory;
    Ppu ppu;
    GamePak::load(memory, ppu, "/home/challenger/Desktop/Donkey Kong (World) (Rev A).nes");

    std::cerr<< getAtrAddress(0x2005 - 0x2000, 0) << std::endl;
}
