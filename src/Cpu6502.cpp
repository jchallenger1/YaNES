#include <iostream>
#include <algorithm>
#include "Cpu6502.hpp"

#define EXECOPCODE(instrPtr, adringPtr) (this->*(instrPtr))((adringPtr))
#define EXECADDRESSING(adringPtr) (this->*(adringPtr))()

constexpr uint16_t Cpu6502::vectorNMI;
constexpr uint16_t Cpu6502::vectorRESET;
constexpr uint16_t Cpu6502::vectorIRQ;

Cpu6502::Cpu6502() {
    fillOpTable();

}


void Cpu6502::fillOpTable() {
    constexpr Instr illegalFunc = {&Cpu6502::OP_ILLEGAL, &Cpu6502::ADR_IMPLICIT};
    std::fill(opcodeTable.begin(), opcodeTable.end(), illegalFunc);

    /// ----- Storage Instructions ------
    ///
    ///
    // LDA
    opcodeTable[0xA9] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0xA5] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xB5] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0xAD] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_ABS};
    opcodeTable[0xBD] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_ABSX};
    opcodeTable[0xB9] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_ABSY};
    opcodeTable[0xA1] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0xB1] = {&Cpu6502::OP_LDA, &Cpu6502::ADR_INDRECTINDEX};
    // LDX
    opcodeTable[0xA2] = {&Cpu6502::OP_LDX, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0xA6] = {&Cpu6502::OP_LDX, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xB6] = {&Cpu6502::OP_LDX, &Cpu6502::ADR_ZEROPAGEY};
    opcodeTable[0xAE] = {&Cpu6502::OP_LDX, &Cpu6502::ADR_ABS};
    opcodeTable[0xBE] = {&Cpu6502::OP_LDX, &Cpu6502::ADR_ABSY};
    // LDY
    opcodeTable[0xA0] = {&Cpu6502::OP_LDY, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0xA4] = {&Cpu6502::OP_LDY, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xB4] = {&Cpu6502::OP_LDY, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0xAC] = {&Cpu6502::OP_LDY, &Cpu6502::ADR_ABS};
    opcodeTable[0xBC] = {&Cpu6502::OP_LDY, &Cpu6502::ADR_ABSX};
    // STA
    opcodeTable[0x85] = {&Cpu6502::OP_STA, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x95] = {&Cpu6502::OP_STA, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x8D] = {&Cpu6502::OP_STA, &Cpu6502::ADR_ABS};
    opcodeTable[0x9D] = {&Cpu6502::OP_STA, &Cpu6502::ADR_ABSX};
    opcodeTable[0x99] = {&Cpu6502::OP_STA, &Cpu6502::ADR_ABSY};
    opcodeTable[0x81] = {&Cpu6502::OP_STA, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0x91] = {&Cpu6502::OP_STA, &Cpu6502::ADR_INDRECTINDEX};
    // STX
    opcodeTable[0x86] = {&Cpu6502::OP_STX, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x96] = {&Cpu6502::OP_STX, &Cpu6502::ADR_ZEROPAGEY};
    opcodeTable[0x8E] = {&Cpu6502::OP_STX, &Cpu6502::ADR_ABS};
    // STY
    opcodeTable[0x84] = {&Cpu6502::OP_STY, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x94] = {&Cpu6502::OP_STY, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x8C] = {&Cpu6502::OP_STY, &Cpu6502::ADR_ABS};
    // Transfer instr
    opcodeTable[0xAA] = {&Cpu6502::OP_TAX, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0xA8] = {&Cpu6502::OP_TAY, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0xBA] = {&Cpu6502::OP_TSX, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x8A] = {&Cpu6502::OP_TXA, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x9A] = {&Cpu6502::OP_TXS, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x98] = {&Cpu6502::OP_TYA, &Cpu6502::ADR_IMPLICIT};
    /// ----- Math Instructions ------
    ///
    ///
    // ADC
    opcodeTable[0x69] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0x65] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x75] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x6D] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_ABS};
    opcodeTable[0x7D] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_ABSX};
    opcodeTable[0x79] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_ABSY};
    opcodeTable[0x61] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0x71] = {&Cpu6502::OP_ADC, &Cpu6502::ADR_INDRECTINDEX};
    // SBC
    opcodeTable[0xE9] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0xE5] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xF5] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0xED] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_ABS};
    opcodeTable[0xFD] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_ABSX};
    opcodeTable[0xF9] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_ABSY};
    opcodeTable[0xE1] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0xF1] = {&Cpu6502::OP_SBC, &Cpu6502::ADR_INDRECTINDEX};
    // Decrementing
    opcodeTable[0xC6] = {&Cpu6502::OP_DEC, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xD6] = {&Cpu6502::OP_DEC, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0xCE] = {&Cpu6502::OP_DEC, &Cpu6502::ADR_ABS};
    opcodeTable[0xDE] = {&Cpu6502::OP_DEC, &Cpu6502::ADR_ABSX};
    opcodeTable[0xCA] = {&Cpu6502::OP_DEX, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x88] = {&Cpu6502::OP_DEY, &Cpu6502::ADR_IMPLICIT};
    // Incrementing
    opcodeTable[0xE6] = {&Cpu6502::OP_INC, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xF6] = {&Cpu6502::OP_INC, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0xEE] = {&Cpu6502::OP_INC, &Cpu6502::ADR_ABS};
    opcodeTable[0xFE] = {&Cpu6502::OP_INC, &Cpu6502::ADR_ABSX};
    opcodeTable[0xE8] = {&Cpu6502::OP_INX, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0xC8] = {&Cpu6502::OP_INY, &Cpu6502::ADR_IMPLICIT};
    /// ----- Bitwise Instructions -----
    ///
    ///
    // AND
    opcodeTable[0x29] = {&Cpu6502::OP_AND, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0x25] = {&Cpu6502::OP_AND, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x35] = {&Cpu6502::OP_AND, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x2D] = {&Cpu6502::OP_AND, &Cpu6502::ADR_ABS};
    opcodeTable[0x3D] = {&Cpu6502::OP_AND, &Cpu6502::ADR_ABSX};
    opcodeTable[0x39] = {&Cpu6502::OP_AND, &Cpu6502::ADR_ABSY};
    opcodeTable[0x21] = {&Cpu6502::OP_AND, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0x31] = {&Cpu6502::OP_AND, &Cpu6502::ADR_INDRECTINDEX};
    // OR
    opcodeTable[0x09] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0x05] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x15] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x0D] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_ABS};
    opcodeTable[0x1D] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_ABSX};
    opcodeTable[0x19] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_ABSY};
    opcodeTable[0x01] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0x11] = {&Cpu6502::OP_ORA, &Cpu6502::ADR_INDRECTINDEX};
    // EOR
    opcodeTable[0x49] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0x45] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x55] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x4D] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_ABS};
    opcodeTable[0x5D] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_ABSX};
    opcodeTable[0x59] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_ABSY};
    opcodeTable[0x41] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0x51] = {&Cpu6502::OP_EOR, &Cpu6502::ADR_INDRECTINDEX};
    // BIT
    opcodeTable[0x24] = {&Cpu6502::OP_BIT, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x2C] = {&Cpu6502::OP_BIT, &Cpu6502::ADR_ABS};
    // ASL
    opcodeTable[0x0A] = {&Cpu6502::OP_ASL, &Cpu6502::ADR_ACCUM};
    opcodeTable[0x06] = {&Cpu6502::OP_ASL, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x16] = {&Cpu6502::OP_ASL, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x0E] = {&Cpu6502::OP_ASL, &Cpu6502::ADR_ABS};
    opcodeTable[0x1E] = {&Cpu6502::OP_ASL, &Cpu6502::ADR_ABSX};
    // LSR
    opcodeTable[0x4A] = {&Cpu6502::OP_LSR, &Cpu6502::ADR_ACCUM};
    opcodeTable[0x46] = {&Cpu6502::OP_LSR, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x56] = {&Cpu6502::OP_LSR, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x4E] = {&Cpu6502::OP_LSR, &Cpu6502::ADR_ABS};
    opcodeTable[0x5E] = {&Cpu6502::OP_LSR, &Cpu6502::ADR_ABSX};
    // ROL
    opcodeTable[0x2A] = {&Cpu6502::OP_ROL, &Cpu6502::ADR_ACCUM};
    opcodeTable[0x26] = {&Cpu6502::OP_ROL, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x36] = {&Cpu6502::OP_ROL, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x2E] = {&Cpu6502::OP_ROL, &Cpu6502::ADR_ABS};
    opcodeTable[0x3E] = {&Cpu6502::OP_ROL, &Cpu6502::ADR_ABSX};
    // ROR
    opcodeTable[0x6A] = {&Cpu6502::OP_ROR, &Cpu6502::ADR_ACCUM};
    opcodeTable[0x66] = {&Cpu6502::OP_ROR, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0x76] = {&Cpu6502::OP_ROR, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0x6E] = {&Cpu6502::OP_ROR, &Cpu6502::ADR_ABS};
    opcodeTable[0x7E] = {&Cpu6502::OP_ROR, &Cpu6502::ADR_ABSX};
    /// ----- Branch Instructions -----
    ///
    ///
    opcodeTable[0x10] = {&Cpu6502::OP_BPL, &Cpu6502::ADR_RELATIVE};
    opcodeTable[0x30] = {&Cpu6502::OP_BMI, &Cpu6502::ADR_RELATIVE};
    opcodeTable[0x50] = {&Cpu6502::OP_BVC, &Cpu6502::ADR_RELATIVE};
    opcodeTable[0x70] = {&Cpu6502::OP_BVS, &Cpu6502::ADR_RELATIVE};
    opcodeTable[0x90] = {&Cpu6502::OP_BCC, &Cpu6502::ADR_RELATIVE};
    opcodeTable[0xB0] = {&Cpu6502::OP_BCS, &Cpu6502::ADR_RELATIVE};
    opcodeTable[0xD0] = {&Cpu6502::OP_BNE, &Cpu6502::ADR_RELATIVE};
    opcodeTable[0xF0] = {&Cpu6502::OP_BEQ, &Cpu6502::ADR_RELATIVE};

    /// ----- Jump Instructions ------
    ///
    ///
    opcodeTable[0x4C] = {&Cpu6502::OP_JMP, &Cpu6502::ADR_ABS};
    opcodeTable[0x6C] = {&Cpu6502::OP_JMP, &Cpu6502::ADR_INDIRECT};
    opcodeTable[0x20] = {&Cpu6502::OP_JSR, &Cpu6502::ADR_ABS};
    opcodeTable[0x60] = {&Cpu6502::OP_RTS, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x40] = {&Cpu6502::OP_RTI, &Cpu6502::ADR_IMPLICIT};

    /// ----- Register Instructions ------
    ///
    ///
    opcodeTable[0x18] = {&Cpu6502::OP_CLC, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x38] = {&Cpu6502::OP_SEC, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x58] = {&Cpu6502::OP_CLI, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x78] = {&Cpu6502::OP_SEI, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0xB8] = {&Cpu6502::OP_CLV, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0xD8] = {&Cpu6502::OP_CLD, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0xF8] = {&Cpu6502::OP_SED, &Cpu6502::ADR_IMPLICIT};
    // CMP
    opcodeTable[0xC9] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0xC5] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xD5] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_ZEROPAGEX};
    opcodeTable[0xCD] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_ABS};
    opcodeTable[0xDD] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_ABSX};
    opcodeTable[0xD9] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_ABSY};
    opcodeTable[0xC1] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_INDEXINDIRECT};
    opcodeTable[0xD1] = {&Cpu6502::OP_CMP, &Cpu6502::ADR_INDRECTINDEX};
    // CPX
    opcodeTable[0xE0] = {&Cpu6502::OP_CPX, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0xE4] = {&Cpu6502::OP_CPX, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xEC] = {&Cpu6502::OP_CPX, &Cpu6502::ADR_ABS};
    // CPY
    opcodeTable[0xC0] = {&Cpu6502::OP_CPY, &Cpu6502::ADR_IMMEDIATE};
    opcodeTable[0xC4] = {&Cpu6502::OP_CPY, &Cpu6502::ADR_ZEROPAGE};
    opcodeTable[0xCC] = {&Cpu6502::OP_CPY, &Cpu6502::ADR_ABS};
    /// ----- Stack Instructions -------
    opcodeTable[0x48] = {&Cpu6502::OP_PHA, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x68] = {&Cpu6502::OP_PLA, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x08] = {&Cpu6502::OP_PHP, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x28] = {&Cpu6502::OP_PLP, &Cpu6502::ADR_IMPLICIT};
    /// ----- System Instructions ------
    opcodeTable[0xEA] = {&Cpu6502::OP_NOP, &Cpu6502::ADR_IMPLICIT};
    opcodeTable[0x00] = {&Cpu6502::OP_BRK, &Cpu6502::ADR_IMPLICIT};
}


void Cpu6502::runCycle(const uint64_t& num) {
    for (uint64_t i = num; i != 0; --i) {
        uint8_t opcode = memory.read(pc);
        Instr instruction = opcodeTable[opcode];
        EXECOPCODE(instruction.instr, instruction.addr);
        ++instrCount;
    }
}

// A vector is a 'vector pointer' that consists of two parts a low and a high
// Both parts cretae a program counter high and low value to where the pc should point
//http://users.telenet.be/kim1-6502/6502/proman.html#90
//https://www.pagetable.com/?p=410

// Non Maskable Interrupt: an interrupt that cannot be ignored
// Interrupts push the pc and the status to the stack and disables interrupts
void Cpu6502::signalNMI() {
    status.b = 0;
    generateInterrupt(vectorNMI);
}

// Reset Signal: An interrupt that sends the pc to the reset vector
// note that no stack operations are done
void Cpu6502::signalRESET() {
    // Assumption that this also resets the state as well
    pc =  static_cast<uint16_t>( (static_cast<uint16_t>(memory[vectorRESET + 1]) << 8) | memory[vectorRESET] );
    status.reset();
    sp = a = x = y = 0;
}

// Interrupt Request:
void Cpu6502::signalIRQ() {
    if (status.i == 0) {// allow interrupt
        status.b = 0;
        generateInterrupt(vectorIRQ);
    }
}

// Generates an interrupt by pushing the pc and stack and pointing pc to the new vector
inline void Cpu6502::generateInterrupt(const uint16_t& vector) {
    PUSH((pc & 0xFF00) >> 8);
    PUSH(pc & 0xFF);
    PUSH(status.asByte());
    status.i = 1;
    pc =  static_cast<uint16_t>( (static_cast<uint16_t>(memory[vector + 1]) << 8) | memory[vector] );
}

///
///
/// \ ---------------- Flagging Operations ----------------
///
///



// zero flag is set if value is equal to zero.
inline void Cpu6502::setZero(const uint16_t& val) noexcept {
    status.z = val == 0;
}

// Negative flag is set if value's bit 7 is set
inline void Cpu6502::setNegative(const uint16_t& val) noexcept {
    status.n = (val >> 7) & 1;
}



///
///
/// \ ---------------- Addressing Operations ----------------
///
///



// Immediate: The data to be obtained is simply the next byte
uint16_t Cpu6502::ADR_IMMEDIATE() noexcept {
    uint16_t address = pc + 1;
    pc += 2;
    return address;
}

// ZeroPage: The data is in the location of the address of the next byte
// Limits the address from 0-256
uint16_t Cpu6502::ADR_ZEROPAGE() {
    uint8_t address = memory.read(pc + 1);
    pc += 2;
    return address;

}
// ZeroPageX: Similar to ZeroPage, but address is added with register X
// Number will wrap around if address >= 256
uint16_t Cpu6502::ADR_ZEROPAGEX() {
    uint8_t address = (memory.read(pc + 1) + x) % 256;
    pc += 2;
    return address;
}

// ZeroPageY: Same as X, but add Y instead
uint16_t Cpu6502::ADR_ZEROPAGEY() {
    uint8_t address = (memory.read(pc + 1) + y) % 256;
    pc += 2;
    return address;
}

// Absolute: A full 16 bit address is used to identify target location
// Note that this system uses little endian architecture
// lowest bits @ 0, highest @ 1
uint16_t Cpu6502::ADR_ABS() {
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(memory.read(pc + 2)) << 8) | memory.read(pc + 1) );
    pc += 3;
    return address;
}

// AbsoluteX: Similar to Absolute, but address is added with register X
// Assumption that no wrapping occurs
uint16_t Cpu6502::ADR_ABSX() {
    uint16_t address = static_cast<uint16_t>( ((static_cast<uint16_t>(memory.read(pc + 2)) << 8) | memory.read(pc + 1)) + x );
    pc += 3;
    return address;
}

// AbsoluteX: Similar to Absolute, but address is added with register Y
// Assumption that no wrapping occurs
uint16_t Cpu6502::ADR_ABSY() {
    uint16_t address = static_cast<uint16_t>( ((static_cast<uint16_t>(memory.read(pc + 2)) << 8) | memory.read(pc + 1)) + y );
    pc += 3;
    return address;
}

// IndirectIndexed/Indirect,Y: Get a full 16bit address from zero page(0-255) memory
// The next byte refers to a location in memory within range 0-255, p for simplicity
// p and p+1 is a full 16 bit location address, the address is then added with register Y to get the final address
// The byte is then that full location
// Wrapping does occur here
uint16_t Cpu6502::ADR_INDRECTINDEX() {
    uint8_t p = memory.read(pc + 1);
    uint16_t address = 0;
    if (p == 0xFF) // wrapping occurs, write from 0 (where it wraps) for high bytes
        address = static_cast<uint16_t>( (static_cast<uint16_t>(memory.read(0)) << 8) | memory.read(p) );
    else
        address = static_cast<uint16_t>( (static_cast<uint16_t>(memory.read(p + 1)) << 8) | memory.read(p) );
    address += y;
    pc += 2;
    return address;
}

// IndexedIndirect/Indirect,X:
// Similar to above, but X is not added to the full address, rather it is added to p to specifiy where the low and high bits are
// Instead of p and p+1, it is p+x and p+x+1
// Wrapping does occur here
uint16_t Cpu6502::ADR_INDEXINDIRECT() {
    uint8_t p = memory.read(pc + 1);
    p += x;
    uint16_t address = 0;
    if (p == 0xFF)
        address = static_cast<uint16_t>( (static_cast<uint16_t>(memory.read(0)) << 8) | memory.read(p));
    else
        address = static_cast<uint16_t>( (static_cast<uint16_t>(memory.read(p + 1)) << 8) | memory.read(p) );
    pc += 2;
    return address;
}

// Implicit/Implied: No address is returned, its needed address is implied via the instruction
uint16_t Cpu6502::ADR_IMPLICIT() noexcept {
    ++pc;
    return 0;
}

// Indirect: Only the JMP instruction uses this
// The next 16 bytes is a pointer to the real address to where it should jump
// Note that if the lowest bits are at the end of the page boundary, then it should
// wrap around back.
// EX jmp 0xC1FF, should wrap to 0xC100 ONLY if low bits is 0xFF
uint16_t Cpu6502::ADR_INDIRECT() {
    uint8_t lowByte = memory.read(pc + 1);
    uint8_t highByte = memory.read(pc + 2);

    uint8_t adrlByte = 0, adrhByte = 0;
    adrlByte = memory.read(static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte) );

    if (lowByte == 0xFF) { // wraps to higbyte only, lowbits are all 0
        adrhByte = memory.read( static_cast<uint16_t>(static_cast<uint16_t>(highByte) << 8) );
        std::cerr << "jmp indirect zero page boundary taken\n";
    }
    else
        adrhByte = memory.read(static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte) + 1);
    pc += 3;
    return static_cast<uint16_t>( (static_cast<uint16_t>(adrhByte) << 8) | adrlByte);
}



// Relative : Similar to immediate however the byte is a signed number rather than unsigned
// Relative is only used by branching operations
// The byte determines from where the pc should move in the range of -128 to +127
uint16_t Cpu6502::ADR_RELATIVE() {

    if (!canBranch) return pc + 2;

    uint8_t byte = memory.read(pc + 1);
    bool isPositive = (0x80 & byte) >> 7 == 0;
    uint8_t offset = (~0x80) & byte;
    return (isPositive ? pc + offset : pc - offset) + 2;
}

// Accumulator : Same as implied, but its always accumulator(a) register.
uint16_t Cpu6502::ADR_ACCUM() {
    ++pc;
    return 0;
}

///
///
/// ---------------- General CPU Functions ----------------
///
///

// Load register reg from memory
inline void Cpu6502::LD(AddressingPtr& adr, uint8_t& reg) {
    uint16_t address = EXECADDRESSING(adr);
    reg = memory.read(address);
    setZero(reg);
    setNegative(reg);
}

// Store a register into memory
inline void Cpu6502::ST(AddressingPtr& adr, uint8_t& reg) {
    uint16_t address = EXECADDRESSING(adr);
    memory.write(address, reg);
}

// Transfer a regular or special register to another
inline void Cpu6502::TR(AddressingPtr& adr, const uint8_t& src, uint8_t& dst) {
    EXECADDRESSING(adr);
    setNegative(src);
    setZero(src);
    dst = src;
}

// Increment a register or memory byte
// Addressing is put to the opcode function
inline void Cpu6502::INC(uint8_t& reg) {
    ++reg;
    setNegative(reg);
    setZero(reg);
}

// Decrement a register or memory byte
// Addressing is put to the opcode function
inline void Cpu6502::DEC(uint8_t& reg) {
    --reg;
    setNegative(reg);
    setZero(reg);
}

inline void Cpu6502::CMP(AddressingPtr& adr, const uint8_t& reg) {
    uint8_t byte = memory.read(EXECADDRESSING(adr));
    uint8_t sum = reg + (~byte + 1);
    setZero(sum);
    setNegative(sum);
    status.c = byte <= reg;
}

inline void Cpu6502::PUSH(const uint8_t& val) {
    memory.write(0x100 + sp, val);
    --sp;
}

inline uint8_t Cpu6502::POP() {
    ++sp;
    uint8_t val = memory.read(0x100 + sp);
    return val;
}



///
///
/// ---------------- Opcode Functions ----------------
///
///


void Cpu6502::OP_ILLEGAL(AddressingPtr& adr) {
    std::cerr << std::hex << "opcode " << static_cast<int>(memory.read(pc))
              << " is ILLEGAL at address " << static_cast<int>(pc) << std::dec;
    EXECADDRESSING(adr);
}

/// ---- Storage Instructions ----

// Load accumulator from memory
void Cpu6502::OP_LDA(AddressingPtr& adr) {
    LD(adr, a);
}
void Cpu6502::OP_LDX(AddressingPtr& adr) {
    LD(adr, x);
}
void Cpu6502::OP_LDY(AddressingPtr& adr) {
    LD(adr, y);
}

// Store accumulator in memory
void Cpu6502::OP_STA(AddressingPtr& adr) {
    ST(adr, a);
}
void Cpu6502::OP_STX(AddressingPtr& adr) {
    ST(adr, x);
}
void Cpu6502::OP_STY(AddressingPtr& adr) {
    ST(adr, y);
}

// Transfer register to another register
// in form TQP, T: Transfer opcode, Q: src, P: dst

void Cpu6502::OP_TAX(AddressingPtr& adr) {
    TR(adr, a, x);
}
void Cpu6502::OP_TAY(AddressingPtr& adr) {
    TR(adr, a, y);
}
void Cpu6502::OP_TSX(AddressingPtr& adr) {
    TR(adr, sp, x);
}
void Cpu6502::OP_TXA(AddressingPtr& adr) {
    TR(adr, x, a);
}
void Cpu6502::OP_TXS(AddressingPtr& adr) {
    // TXS does not modify processor state
    Status s = status;
    TR(adr, x, sp);
    std::swap(s, status);
}
void Cpu6502::OP_TYA(AddressingPtr& adr) {
    TR(adr, y, a);
}

/// ---- Math Instructions ----



// Add with carry from memory (A + M + C -> A)
// https://stackoverflow.com/questions/29193303/6502-emulation-proper-way-to-implement-adc-and-sbc  and
// https://github.com/gianlucag/mos6502/blob/master/mos6502.cpp in ADC and SBC
void Cpu6502::OP_ADC(AddressingPtr& adr) {
    uint8_t byte = memory.read(EXECADDRESSING(adr));
    uint16_t sum = a + byte + status.c;
    if (status.d && cpuAllowDec) {
        if ( (a & 0xF) + (byte & 0xF) + status.c > 9)
            sum += 6;
        setNegative(sum);
        status.o = ((a ^ sum) & (byte ^ sum) & 0x80) == 0x80;
        if (sum > 0x99)
            sum += 96;
        status.c = sum > 0x99;
    }
    else {
        setNegative(sum);
        status.o = ((a ^ sum) & (byte ^ sum) & 0x80) == 0x80;
        status.c = sum > 0xFF;
    }

    a = sum & 0xFF;
    setZero(a);
}

// Subtract memory from a (A - M - (1-C) -> A)
// Same sources used for ADC
void Cpu6502::OP_SBC(AddressingPtr& adr) {
    uint8_t byte = memory.read(EXECADDRESSING(adr));
    uint16_t sum = a - byte - (1 - status.c);
    setNegative(sum);
    setZero(sum);
    status.o = ((a ^ sum) & 0x80) && ((a ^ byte) & 0x80);

    if (status.d && cpuAllowDec) {
        if ((a & 0x0F) - status.c < (byte & 0x0F))
            sum -= 6;
        if (sum >= 0x99)
            sum -= 0x60;
    }

    status.c = sum < 0x100;
    a = sum & 0xFF;
}

void Cpu6502::OP_DEC(AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr);
    uint8_t byte = memory.read(address);
    DEC(byte);
    memory.write(address, byte);
}
void Cpu6502::OP_DEX(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    DEC(x);
}
void Cpu6502::OP_DEY(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    DEC(y);
}


void Cpu6502::OP_INC(AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr);
    uint8_t byte = memory.read(address);
    INC(byte);
    memory.write(address, byte);
}
void Cpu6502::OP_INX(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    INC(x);
}
void Cpu6502::OP_INY(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    INC(y);
}



/// ----- Bitwise Instructions



// Binary AND w/ accumulator ( A & M -> A)
void Cpu6502::OP_AND(AddressingPtr& adr) {
    uint8_t byte = memory.read(EXECADDRESSING(adr));
    a &= byte;
    setZero(a);
    setNegative(a);
}

// Binary OR w/ accumulator ( A | M -> A)
void Cpu6502::OP_ORA(AddressingPtr& adr) {
    uint8_t byte = memory.read(EXECADDRESSING(adr));
    a |= byte;
    setZero(a);
    setNegative(a);
}

// Binary XOR w/ accumulator ( A ^ M -> A)
void Cpu6502::OP_EOR(AddressingPtr& adr) {
    uint8_t byte = memory.read(EXECADDRESSING(adr));
    a ^= byte;
    setZero(a);
    setNegative(a);
}

// Test bits in memory with accumulator by using binary AND
// -> A & M, no registers modified
void Cpu6502::OP_BIT(AddressingPtr& adr) {
    uint8_t byte = memory.read(EXECADDRESSING(adr));
    uint8_t sum = byte & a;
    setZero(sum);
    setNegative(byte);
    status.o = (byte & 0x40) >> 6;
}

// Arithmetric Shift Left
// Shift all bits left by 1, bit 0 is always 0
// The original 7 bit is shifted into the carry status flag
void Cpu6502::OP_ASL(AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr);
    uint8_t byte = adr == &Cpu6502::ADR_ACCUM ? a : memory.read(address);
    status.c = (byte & 0x80) >> 7;
    byte <<= 1;
    setZero(byte);
    setNegative(byte);
    if (adr == &Cpu6502::ADR_ACCUM)
        a = byte;
    else
        memory.write(address, byte);
}

// Logical Shift Right
// Shift all bits right one position bit 7 is always 0, original 0 bit shifted into carry
void Cpu6502::OP_LSR(AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr);
    uint8_t byte = adr == &Cpu6502::ADR_ACCUM ? a : memory.read(address);
    status.c = byte & 1;
    byte >>= 1;
    setZero(byte);
    setNegative(byte);
    if (adr == &Cpu6502::ADR_ACCUM)
        a = byte;
    else
        memory.write(address, byte);

}

// Rotate Left
// Shift all bits left by 1
// bit 0 is the carry flag and original bit 7 is now carry
void Cpu6502::OP_ROL(AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr);
    uint8_t byte = adr == &Cpu6502::ADR_ACCUM ? a : memory.read(address);
    uint8_t bit7 = (byte & 0x80) >> 7;
    byte <<= 1;
    byte |= status.c;
    status.c = bit7;
    setZero(byte);
    setNegative(byte);
    if (adr == &Cpu6502::ADR_ACCUM)
        a = byte;
    else
        memory.write(address, byte);
}

// Rotate Right
// shfit right, carry bit goes into bit 7, original bit 0 is now carry
void Cpu6502::OP_ROR(AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr);
    uint8_t byte = adr == &Cpu6502::ADR_ACCUM ? a : memory.read(address);
    uint8_t bit0 = byte & 1;
    byte >>= 1;
    byte |= status.c << 7;
    status.c = bit0;
    setZero(byte);
    setNegative(byte);
    if (adr == &Cpu6502::ADR_ACCUM)
        a = byte;
    else
        memory.write(address, byte);
}

/// ----- Branching Instructions



// The next BXX instructions:
// Branch/Jump if condition, specified in canBranch otherwise skip

void Cpu6502::OP_BMI(AddressingPtr& adr) {
    canBranch = status.n == 1;
    pc = EXECADDRESSING(adr);
}

void Cpu6502::OP_BPL(AddressingPtr& adr) {
    canBranch = status.n == 0;
    pc = EXECADDRESSING(adr);
}

void Cpu6502::OP_BCC(AddressingPtr& adr) {
    canBranch = status.c == 0;
    pc = EXECADDRESSING(adr);
}

void Cpu6502::OP_BCS(AddressingPtr& adr) {
    canBranch = status.c == 1;
    pc = EXECADDRESSING(adr);
}

void Cpu6502::OP_BEQ(AddressingPtr& adr) {
    canBranch = status.z == 1;
    pc = EXECADDRESSING(adr);
}

void Cpu6502::OP_BNE(AddressingPtr& adr) {
    canBranch = status.z == 0;
    pc = EXECADDRESSING(adr);
}

void Cpu6502::OP_BVS(AddressingPtr& adr) {
    canBranch = status.o == 1;
    pc = EXECADDRESSING(adr);
}

void Cpu6502::OP_BVC(AddressingPtr& adr) {
    canBranch = status.o == 0;
    pc = EXECADDRESSING(adr);
}


/// ----- Jump Instructions


// Jump to a new location
void Cpu6502::OP_JMP(AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr);
    pc = address;
}

// Jumps to subroutine:
// pushes address - 1 of the next operation before transfering pc
void Cpu6502::OP_JSR(AddressingPtr& adr) {
    uint16_t transferAdr = EXECADDRESSING(adr);
    uint16_t address = pc - 1;
    PUSH((address & 0xFF00) >> 8); // push high bits FIRST
    PUSH((address & 0xFF)); // push low bits LAST (sp now points to low bits)
    pc = transferAdr;
}

// Return from Subroutine:
// sets pc to the popped stack's address + 1
void Cpu6502::OP_RTS(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    uint8_t low = POP(), high = POP();
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(high) << 8) | low );
    pc = address + 1;
}

// Return from Interrupt:
// get flags then pc from the stack, the pc is actual address, not address -1
void Cpu6502::OP_RTI(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.fromByte(POP());
    uint8_t low = POP(), high = POP();
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(high) << 8) | low );
    pc = address;
}


/// ------- Register Instructions

 // Set Carry bit
void Cpu6502::OP_SEC(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.c = 1;
}

// Clear Carry bit
void Cpu6502::OP_CLC(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.c = 0;
}

// Set interrupt
void Cpu6502::OP_SEI(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.i = 1;
}

// Clear Interrupt
void Cpu6502::OP_CLI(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.i = 0;
}

// Set Decimal
void Cpu6502::OP_SED(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.d = 1;
}

// Clear Decimal
void Cpu6502::OP_CLD(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.d = 0;
}

// Clear Overflow
void Cpu6502::OP_CLV(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.o = 0;
}

// Compare a byte with the accumulator by subtracting it from the accumulator
// Effectively A - M, note that it does not modify any registers only status flags
void Cpu6502::OP_CMP(AddressingPtr& adr) {
    CMP(adr, a);
}
// comp with x register
void Cpu6502::OP_CPX(AddressingPtr& adr) {
    CMP(adr, x);
}
// comp with y register
void Cpu6502::OP_CPY(AddressingPtr& adr) {
    CMP(adr, y);
}


/// --- Stack Instructions
// Stack works from top to bottom, usually from first page 0x100 - 0x1FF
// All stack operations are implied
// Stack pointer always points to the next available byte to be pushed

// Push A to the stack
void Cpu6502::OP_PHA(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    PUSH(a);
}

// Push Processor Status onto Stack
void Cpu6502::OP_PHP(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.b = 1;
    PUSH(status.asByte());
    status.b = 0;
}

// Pull A from Stack
void Cpu6502::OP_PLA(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    a = POP();
    setZero(a);
    setNegative(a);
}

// Pull Processor Status from Stack
void Cpu6502::OP_PLP(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.fromByte(POP());
}


/// --- System Instructions


void Cpu6502::OP_NOP(AddressingPtr& adr) {
    EXECADDRESSING(adr);
}

void Cpu6502::OP_BRK(AddressingPtr& adr) {
    EXECADDRESSING(adr);
    status.b = 1;
    generateInterrupt(vectorIRQ); // BRK uses same as IRQ
}




void Cpu6502::clear() {
    a = x = y = 0;
    sp = 0;
    pc = 0;
    cycleCount = 0;
    status.clear();
    memory.clear();
}



Cpu6502::Status::Status() {
    reset();
}

void Cpu6502::Status::clear() noexcept {
    c = z = i = d = b = pad = o = n = 0;
}

void Cpu6502::Status::reset() noexcept {
    i = 1;
    n = o = d = c = z = b = 0;
}

uint8_t Cpu6502::Status::asByte() const noexcept {
    uint8_t byte = 0x20;
    byte |= c;
    byte |= z << 1;
    byte |= i << 2;
    byte |= d << 3;
    byte |= b << 4;
    // 5 is padding
    byte |= o << 6;
    byte |= n << 7;
    return byte;
}

void Cpu6502::Status::fromByte(const uint8_t& byte) noexcept {
    c = byte & 1;
    z = (byte & 2) >> 1;
    i = (byte & 4) >> 2;
    d = (byte & 0xF) >> 3;
    o = (byte & 0x40) >> 6;
    n = (byte & 0x80) >> 7;
}





