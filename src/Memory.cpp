#include "Memory.h"
#include "NES.h"
#include "functions.hpp"

#include <fstream>
#include <iostream>
#include <memory>

Memory::Memory() {
    static constexpr bool warn = false;
    if (warn)
        std::cerr << "Warning, Memory class does not have a NES handle\n";
}

Memory::Memory(std::shared_ptr<NES> nes) {
    setNESHandle(nes);
}

void Memory::setNESHandle(std::shared_ptr<NES> nes) {
    this->nes = nes;
}

uint8_t Memory::read(const uint16_t& adr) const {
    if (inRange(0x0000, 0x1FFF, adr)) // ram mirror, repeats every 0x0800
        return memory[adr % 0x0800];
    else if (inRange(0x2000, 0x3FFF, adr)) // nes ppu register mirrors, repeats every 0x8
        return nes->ppu.readRegister(0x2000 + adr % 8);
    else if (adr == 0x4014)
        return nes->ppu.readRegister(adr);
    else if (inRange(0x8000, 0xFFFF, adr)) {
        // NROM differs in if its a NROM-128 or NROM-256
        // if NROM-128 its a mirror of 0x8000-0xBFFF
        if (nes->gamepak.PRG_ROM_sz == 1){ // NROM-128
            return memory[0x8000 + adr % 0x4000];
        }
        else
            return memory[adr];
    }
    else
        return memory[adr];
}

void Memory::write(const uint16_t& adr, const uint8_t& val) {
    if (inRange(0x0000, 0x1FFF, adr)) // ram mirror, repeats every 0x0800
        memory[adr % 0x0800] = val;
    else if (inRange(0x2000, 0x3FFF, adr)) // nes ppu register mirrors, repeats every 0x8
        nes->ppu.writeRegister(0x2000 + adr % 8, val);
    else if (adr == 0x4014)
        return nes->ppu.writeRegister(adr, val);
    else if (inRange(0x8000, 0xFFFF, adr)) {
        if (nes->gamepak.PRG_ROM_sz == 1){ // NROM-128
            memory[0x8000 + adr % 0x4000] = val;
        }
        else
            memory[adr] = val;
    }
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
