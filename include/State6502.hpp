#ifndef STATE6502_HPP
#define STATE6502_HPP

#include <cstdint>

struct State6502 {
    // Registers
    uint8_t a = 0; // Accumulator
    uint8_t x = 0; // Index Register X
    uint8_t y = 0; // Index Register Y

    uint8_t sp = 0; // Stack Pointer
    uint16_t pc = 0; // Program Counter

    // Struct bitfield for status registers
    struct Status {
        uint8_t c : 1; // Carry flag
        uint8_t z : 1; // Zero flag
        uint8_t i : 1; // Interrupt Disable
        uint8_t d : 1; // Decimal Mode
        uint8_t b : 1; // Break Command
        uint8_t pad : 1; // padding, non status
        uint8_t o : 1; // Overflow Flag
        uint8_t n : 1; // Negative Flag
    };

};



#endif
