#include <iostream>
#include <algorithm>
#include "Disassembler6502.hpp"

#define EXECOPCODE(instrPtr, adringPtr, state) (this->*(instrPtr))((state), (adringPtr))
#define EXECADDRESSING(adringPtr, state) (this->*(adringPtr))(state)

constexpr uint16_t Disassembler6502::vectorNMI;
constexpr uint16_t Disassembler6502::vectorRESET;
constexpr uint16_t Disassembler6502::vectorIRQ;

Disassembler6502::Disassembler6502() {


    constexpr Instr illegalFunc = {&Disassembler6502::OP_ILLEGAL, &Disassembler6502::ADR_IMPLICIT};
    std::fill(opcodeTable.begin(), opcodeTable.end(), illegalFunc);

    /// ----- Storage Instructions ------
    ///
    ///
    // LDA
    opcodeTable[0xA9] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xA5] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xB5] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xAD] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ABS};
    opcodeTable[0xBD] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ABSX};
    opcodeTable[0xB9] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ABSY};
    opcodeTable[0xA1] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0xB1] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_INDRECTINDEX};
    // LDX
    opcodeTable[0xA2] = {&Disassembler6502::OP_LDX, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xA6] = {&Disassembler6502::OP_LDX, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xB6] = {&Disassembler6502::OP_LDX, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xAE] = {&Disassembler6502::OP_LDX, &Disassembler6502::ADR_ABS};
    opcodeTable[0xBE] = {&Disassembler6502::OP_LDX, &Disassembler6502::ADR_ABSX};
    // LDY
    opcodeTable[0xA0] = {&Disassembler6502::OP_LDY, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xA4] = {&Disassembler6502::OP_LDY, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xB4] = {&Disassembler6502::OP_LDY, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xAC] = {&Disassembler6502::OP_LDY, &Disassembler6502::ADR_ABS};
    opcodeTable[0xBC] = {&Disassembler6502::OP_LDY, &Disassembler6502::ADR_ABSX};
    // STA
    opcodeTable[0x85] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x95] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x8D] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ABS};
    opcodeTable[0x9D] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ABSX};
    opcodeTable[0x99] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ABSY};
    opcodeTable[0x81] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0x91] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_INDRECTINDEX};
    // STX
    opcodeTable[0x86] = {&Disassembler6502::OP_STX, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x96] = {&Disassembler6502::OP_STX, &Disassembler6502::ADR_ZEROPAGEY};
    opcodeTable[0x8E] = {&Disassembler6502::OP_STX, &Disassembler6502::ADR_ABS};
    // STY
    opcodeTable[0x84] = {&Disassembler6502::OP_STY, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x94] = {&Disassembler6502::OP_STY, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x8C] = {&Disassembler6502::OP_STY, &Disassembler6502::ADR_ABS};
    // Transfer instr
    opcodeTable[0xAA] = {&Disassembler6502::OP_TAX, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0xA8] = {&Disassembler6502::OP_TAY, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0xBA] = {&Disassembler6502::OP_TSX, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x8A] = {&Disassembler6502::OP_TXA, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x9A] = {&Disassembler6502::OP_TXS, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x98] = {&Disassembler6502::OP_TYA, &Disassembler6502::ADR_IMPLICIT};
    /// ----- Math Instructions ------
    ///
    ///
    // ADC
    opcodeTable[0x69] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0x65] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x75] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x6D] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_ABS};
    opcodeTable[0x7D] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_ABSX};
    opcodeTable[0x79] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_ABSY};
    opcodeTable[0x61] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0x71] = {&Disassembler6502::OP_ADC, &Disassembler6502::ADR_INDRECTINDEX};
    // SBC
    opcodeTable[0xE9] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xE5] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xF5] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xED] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_ABS};
    opcodeTable[0xFD] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_ABSX};
    opcodeTable[0xF9] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_ABSY};
    opcodeTable[0xE1] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0xF1] = {&Disassembler6502::OP_SBC, &Disassembler6502::ADR_INDRECTINDEX};
    // Decrementing
    opcodeTable[0xC6] = {&Disassembler6502::OP_DEC, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xD6] = {&Disassembler6502::OP_DEC, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xCE] = {&Disassembler6502::OP_DEC, &Disassembler6502::ADR_ABS};
    opcodeTable[0xDE] = {&Disassembler6502::OP_DEC, &Disassembler6502::ADR_ABSX};
    opcodeTable[0xCA] = {&Disassembler6502::OP_DEX, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x88] = {&Disassembler6502::OP_DEY, &Disassembler6502::ADR_IMPLICIT};
    // Incrementing
    opcodeTable[0xE6] = {&Disassembler6502::OP_INC, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xF6] = {&Disassembler6502::OP_INC, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xEE] = {&Disassembler6502::OP_INC, &Disassembler6502::ADR_ABS};
    opcodeTable[0xFE] = {&Disassembler6502::OP_INC, &Disassembler6502::ADR_ABSX};
    opcodeTable[0xE8] = {&Disassembler6502::OP_INX, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0xC8] = {&Disassembler6502::OP_INY, &Disassembler6502::ADR_IMPLICIT};
    /// ----- Bitwise Instructions -----
    ///
    ///
    // AND
    opcodeTable[0x29] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0x25] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x35] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x2D] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_ABS};
    opcodeTable[0x3D] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_ABSX};
    opcodeTable[0x39] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_ABSY};
    opcodeTable[0x21] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0x31] = {&Disassembler6502::OP_AND, &Disassembler6502::ADR_INDRECTINDEX};
    // OR
    opcodeTable[0x09] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0x05] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x15] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x0D] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_ABS};
    opcodeTable[0x1D] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_ABSX};
    opcodeTable[0x19] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_ABSY};
    opcodeTable[0x01] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0x11] = {&Disassembler6502::OP_ORA, &Disassembler6502::ADR_INDRECTINDEX};
    // EOR
    opcodeTable[0x49] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0x45] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x55] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x4D] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_ABS};
    opcodeTable[0x5D] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_ABSX};
    opcodeTable[0x59] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_ABSY};
    opcodeTable[0x41] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0x51] = {&Disassembler6502::OP_EOR, &Disassembler6502::ADR_INDRECTINDEX};
    // BIT
    opcodeTable[0x24] = {&Disassembler6502::OP_BIT, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x2C] = {&Disassembler6502::OP_BIT, &Disassembler6502::ADR_ABS};
    // ASL
    opcodeTable[0x0A] = {&Disassembler6502::OP_ASL, &Disassembler6502::ADR_ACCUM};
    opcodeTable[0x06] = {&Disassembler6502::OP_ASL, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x16] = {&Disassembler6502::OP_ASL, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x0E] = {&Disassembler6502::OP_ASL, &Disassembler6502::ADR_ABS};
    opcodeTable[0x1E] = {&Disassembler6502::OP_ASL, &Disassembler6502::ADR_ABSX};
    // LSR
    opcodeTable[0x4A] = {&Disassembler6502::OP_LSR, &Disassembler6502::ADR_ACCUM};
    opcodeTable[0x46] = {&Disassembler6502::OP_LSR, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x56] = {&Disassembler6502::OP_LSR, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x4E] = {&Disassembler6502::OP_LSR, &Disassembler6502::ADR_ABS};
    opcodeTable[0x5E] = {&Disassembler6502::OP_LSR, &Disassembler6502::ADR_ABSX};
    // ROL
    opcodeTable[0x2A] = {&Disassembler6502::OP_ROL, &Disassembler6502::ADR_ACCUM};
    opcodeTable[0x26] = {&Disassembler6502::OP_ROL, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x36] = {&Disassembler6502::OP_ROL, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x2E] = {&Disassembler6502::OP_ROL, &Disassembler6502::ADR_ABS};
    opcodeTable[0x3E] = {&Disassembler6502::OP_ROL, &Disassembler6502::ADR_ABSX};
    // ROR
    opcodeTable[0x6A] = {&Disassembler6502::OP_ROR, &Disassembler6502::ADR_ACCUM};
    opcodeTable[0x66] = {&Disassembler6502::OP_ROR, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x76] = {&Disassembler6502::OP_ROR, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x6E] = {&Disassembler6502::OP_ROR, &Disassembler6502::ADR_ABS};
    opcodeTable[0x7E] = {&Disassembler6502::OP_ROR, &Disassembler6502::ADR_ABSX};
    /// ----- Branch Instructions -----
    ///
    ///
    opcodeTable[0x10] = {&Disassembler6502::OP_BPL, &Disassembler6502::ADR_RELATIVE};
    opcodeTable[0x30] = {&Disassembler6502::OP_BMI, &Disassembler6502::ADR_RELATIVE};
    opcodeTable[0x50] = {&Disassembler6502::OP_BVC, &Disassembler6502::ADR_RELATIVE};
    opcodeTable[0x70] = {&Disassembler6502::OP_BVS, &Disassembler6502::ADR_RELATIVE};
    opcodeTable[0x90] = {&Disassembler6502::OP_BCC, &Disassembler6502::ADR_RELATIVE};
    opcodeTable[0xB0] = {&Disassembler6502::OP_BCS, &Disassembler6502::ADR_RELATIVE};
    opcodeTable[0xD0] = {&Disassembler6502::OP_BNE, &Disassembler6502::ADR_RELATIVE};
    opcodeTable[0xF0] = {&Disassembler6502::OP_BEQ, &Disassembler6502::ADR_RELATIVE};

    /// ----- Jump Instructions ------
    ///
    ///
    opcodeTable[0x4C] = {&Disassembler6502::OP_JMP, &Disassembler6502::ADR_ABS};
    opcodeTable[0x6C] = {&Disassembler6502::OP_JMP, &Disassembler6502::ADR_INDIRECT};
    opcodeTable[0x20] = {&Disassembler6502::OP_JSR, &Disassembler6502::ADR_ABS};
    opcodeTable[0x60] = {&Disassembler6502::OP_RTS, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x40] = {&Disassembler6502::OP_RTI, &Disassembler6502::ADR_IMPLICIT};

    /// ----- Register Instructions ------
    ///
    ///
    opcodeTable[0x18] = {&Disassembler6502::OP_CLC, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x38] = {&Disassembler6502::OP_SEC, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x58] = {&Disassembler6502::OP_CLI, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x78] = {&Disassembler6502::OP_SEI, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0xB8] = {&Disassembler6502::OP_CLV, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0xD8] = {&Disassembler6502::OP_CLD, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0xF8] = {&Disassembler6502::OP_SED, &Disassembler6502::ADR_IMPLICIT};
    // CMP
    opcodeTable[0xC9] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xC5] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xD5] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xCD] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_ABS};
    opcodeTable[0xDD] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_ABSX};
    opcodeTable[0xD9] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_ABSY};
    opcodeTable[0xC1] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0xD1] = {&Disassembler6502::OP_CMP, &Disassembler6502::ADR_INDRECTINDEX};
    // CPX
    opcodeTable[0xE0] = {&Disassembler6502::OP_CPX, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xE4] = {&Disassembler6502::OP_CPX, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xEC] = {&Disassembler6502::OP_CPX, &Disassembler6502::ADR_ABS};
    // CPY
    opcodeTable[0xC0] = {&Disassembler6502::OP_CPY, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xC4] = {&Disassembler6502::OP_CPY, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xCC] = {&Disassembler6502::OP_CPY, &Disassembler6502::ADR_ABS};
    /// ----- Stack Instructions -------
    opcodeTable[0x48] = {&Disassembler6502::OP_PHA, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x68] = {&Disassembler6502::OP_PLA, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x08] = {&Disassembler6502::OP_PHP, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x28] = {&Disassembler6502::OP_PLP, &Disassembler6502::ADR_IMPLICIT};
    /// ----- System Instructions ------
    opcodeTable[0xEA] = {&Disassembler6502::OP_NOP, &Disassembler6502::ADR_IMPLICIT};
    opcodeTable[0x00] = {&Disassembler6502::OP_BRK, &Disassembler6502::ADR_IMPLICIT};
}


void Disassembler6502::runCycle(State6502& state) {
    uint8_t opcode = state.memory.read(state.pc);
    Instr instruction = opcodeTable[opcode];
    EXECOPCODE(instruction.instr, instruction.addr, state);
}

void Disassembler6502::runN(State6502& state, const int& num) {
    for (int i = 0; i != num; i++)
        runCycle(state);
}

// A vector is a 'vector pointer' that consists of two parts a low and a high
// Both parts cretae a program counter high and low value to where the pc should point
//http://users.telenet.be/kim1-6502/6502/proman.html#90
//https://www.pagetable.com/?p=410

// Non Maskable Interrupt: an interrupt that cannot be ignored
// Interrupts push the pc and the status to the stack and disables interrupts
void Disassembler6502::signalNMI(State6502& state) {
    state.status.b = 0;
    generateInterrupt(state, vectorNMI);
}

// Reset Signal: An interrupt that sends the pc to the reset vector
// note that no stack operations are done
void Disassembler6502::signalRESET(State6502& state) {
    // Assumption that this also resets the state as well
    state.pc =  static_cast<uint16_t>( (static_cast<uint16_t>(state.memory[vectorRESET + 1]) << 8) | state.memory[vectorRESET] );
    state.status.reset();
    state.sp = state.a = state.x = state.y = 0;
}

// Interrupt Request:
void Disassembler6502::signalIRQ(State6502& state) {
    if (state.status.i == 0) {// allow interrupt
        state.status.b = 0;
        generateInterrupt(state, vectorIRQ);
    }
}

// Generates an interrupt by pushing the pc and stack and pointing pc to the new vector
inline void Disassembler6502::generateInterrupt(State6502& state, const uint16_t& vector) const {
    PUSH(state, (state.pc & 0xFF00) >> 8);
    PUSH(state, state.pc & 0xFF);
    PUSH(state, state.status.asByte());
    state.status.i = 1;
    state.pc =  static_cast<uint16_t>( (static_cast<uint16_t>(state.memory[vector + 1]) << 8) | state.memory[vector] );
}

///
///
/// \ ---------------- Flagging Operations ----------------
///
///



// zero flag is set if value is equal to zero.
inline void Disassembler6502::setZero(State6502& state, const uint16_t& val) const noexcept {
    state.status.z = val == 0;
}

// Negative flag is set if value's bit 7 is set
inline void Disassembler6502::setNegative(State6502& state, const uint16_t& val) const noexcept {
    state.status.n = (val >> 7) & 1;
}



///
///
/// \ ---------------- Addressing Operations ----------------
///
///



// Immediate: The data to be obtained is simply the next byte
uint16_t Disassembler6502::ADR_IMMEDIATE(State6502& state) const noexcept {
    uint16_t address = state.pc + 1;
    state.pc += 2;
    return address;
}

// ZeroPage: The data is in the location of the address of the next byte
// Limits the address from 0-256
uint16_t Disassembler6502::ADR_ZEROPAGE(State6502& state) const {
    uint8_t address = state.memory.read(state.pc + 1);
    state.pc += 2;
    return address;

}
// ZeroPageX: Similar to ZeroPage, but address is added with register X
// Number will wrap around if address >= 256
uint16_t Disassembler6502::ADR_ZEROPAGEX(State6502& state) const {
    uint8_t address = (state.memory.read(state.pc + 1) + state.x) % 256;
    state.pc += 2;
    return address;
}

// ZeroPageY: Same as X, but add Y instead
uint16_t Disassembler6502::ADR_ZEROPAGEY(State6502& state) const {
    uint8_t address = (state.memory.read(state.pc + 1) + state.y) % 256;
    state.pc += 2;
    return address;
}

// Absolute: A full 16 bit address is used to identify target location
// Note that this system uses little endian architecture
// lowest bits @ 0, highest @ 1
uint16_t Disassembler6502::ADR_ABS(State6502& state) const {
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(state.memory.read(state.pc + 2)) << 8) | state.memory.read(state.pc + 1) );
    state.pc += 3;
    return address;
}

// AbsoluteX: Similar to Absolute, but address is added with register X
// Assumption that no wrapping occurs
uint16_t Disassembler6502::ADR_ABSX(State6502& state) const {
    uint16_t address = static_cast<uint16_t>( ((static_cast<uint16_t>(state.memory.read(state.pc + 2)) << 8) | state.memory.read(state.pc + 1)) + state.x );
    state.pc += 3;
    return address;
}

// AbsoluteX: Similar to Absolute, but address is added with register Y
// Assumption that no wrapping occurs
uint16_t Disassembler6502::ADR_ABSY(State6502& state) const {
    uint16_t address = static_cast<uint16_t>( ((static_cast<uint16_t>(state.memory.read(state.pc + 2)) << 8) | state.memory.read(state.pc + 1)) + state.y );
    state.pc += 3;
    return address;
}

// IndirectIndexed/Indirect,Y: Get a full 16bit address from zero page(0-255) memory
// The next byte refers to a location in memory within range 0-255, p for simplicity
// p and p+1 is a full 16 bit location address, the address is then added with register Y to get the final address
// The byte is then that full location
uint16_t Disassembler6502::ADR_INDRECTINDEX(State6502& state) const {
    uint8_t p = state.memory.read(state.pc + 1);
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(state.memory.read(p + 1)) << 8) | state.memory.read(p) );
    address += state.y;
    state.pc += 2;
    return address;
}

// IndexedIndirect/Indirect,X:
// Similar to above, but X is not added to the full address, rather it is added to p to specifiy where the low and high bits are
// Instead of p and p+1, it is p+x and p+x+1
uint16_t Disassembler6502::ADR_INDEXINDIRECT(State6502& state) const {
    uint8_t p = state.memory.read(state.pc + 1);
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(state.memory.read(p + state.x + 1)) << 8) | state.memory.read(p + state.x) );
    state.pc += 2;
    return address;
}

// Implicit/Implied: No address is returned, its needed address is implied via the instruction
uint16_t Disassembler6502::ADR_IMPLICIT(State6502& state) const noexcept {
    ++state.pc;
    return 0;
}

// Indirect: Only the JMP instruction uses this
// The next 16 bytes is a pointer to the real address to where it should jump
// Note that if the lowest bits are at the end of the page boundary, then it should
// wrap around back.
// EX jmp 0xC1FF, should wrap to 0xC100 ONLY if low bits is 0xFF
uint16_t Disassembler6502::ADR_INDIRECT(State6502& state) const {
    uint8_t lowByte = state.memory.read(state.pc + 1);
    uint8_t highByte = state.memory.read(state.pc + 2);

    uint8_t adrlByte = 0, adrhByte = 0;
    adrlByte = state.memory.read(static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte) );

    if (lowByte == 0xFF) { // wraps to higbyte only, lowbits are all 0
        adrhByte = state.memory.read( static_cast<uint16_t>(static_cast<uint16_t>(highByte) << 8) );
        std::cerr << "jmp indirect zero page boundary taken\n";
    }
    else
        adrhByte = state.memory.read(static_cast<uint16_t>( (static_cast<uint16_t>(highByte) << 8) | lowByte) + 1);
    state.pc += 3;
    return static_cast<uint16_t>( (static_cast<uint16_t>(adrhByte) << 8) | adrlByte);
}



// Relative : Similar to immediate however the byte is a signed number rather than unsigned
// Relative is only used by branching operations
// The byte determines from where the pc should move in the range of -128 to +127
uint16_t Disassembler6502::ADR_RELATIVE(State6502& state) const {

    if (!canBranch) return state.pc + 2;

    uint8_t byte = state.memory.read(state.pc + 1);
    bool isPositive = (0x80 & byte) >> 7 == 0;
    uint8_t offset = (~0x80) & byte;
    return (isPositive ? state.pc + offset : state.pc - offset) + 2;
}

// Accumulator : Same as implied, but its always accumulator(a) register.
uint16_t Disassembler6502::ADR_ACCUM(State6502& state) const {
    ++state.pc;
    return 0;
}

///
///
/// ---------------- General CPU Functions ----------------
///
///

// Load register reg from memory
inline void Disassembler6502::LD(State6502& state, AddressingPtr& adr, uint8_t& reg) const {
    uint16_t address = EXECADDRESSING(adr, state);
    reg = state.memory.read(address);
    setZero(state, reg);
    setNegative(state, reg);
}

// Store a register into memory
inline void Disassembler6502::ST(State6502& state, AddressingPtr& adr, uint8_t& reg) const {
    uint16_t address = EXECADDRESSING(adr, state);
    state.memory.write(address, reg);
}

// Transfer a regular or special register to another
inline void Disassembler6502::TR(State6502& state, AddressingPtr& adr, const uint8_t& src, uint8_t& dst) const {
    EXECADDRESSING(adr, state);
    setNegative(state, src);
    setZero(state, src);
    dst = src;
}

// Increment a register or memory byte
// Addressing is put to the opcode function
inline void Disassembler6502::INC(State6502& state, uint8_t& reg) const {
    ++reg;
    setNegative(state, reg);
    setZero(state, reg);
}

// Decrement a register or memory byte
// Addressing is put to the opcode function
inline void Disassembler6502::DEC(State6502& state, uint8_t& reg) const {
    --reg;
    setNegative(state, reg);
    setZero(state, reg);
}

inline void Disassembler6502::CMP(State6502& state, AddressingPtr& adr, const uint8_t& reg) const {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    uint8_t sum = reg + (~byte + 1);
    setZero(state, sum);
    setNegative(state, sum);
    state.status.c = byte <= reg;
}

inline void Disassembler6502::PUSH(State6502& state, const uint8_t& val) const {
    state.memory.write(0x100 + state.sp, val);
    --state.sp;
}

inline uint8_t Disassembler6502::POP(State6502& state) const {
    ++state.sp;
    uint8_t val = state.memory.read(0x100 + state.sp);
    return val;
}



///
///
/// ---------------- Opcode Functions ----------------
///
///


void Disassembler6502::OP_ILLEGAL(State6502& state, AddressingPtr& adr) {
    std::cerr << std::hex << "opcode " << static_cast<int>(state.memory.read(state.pc))
              << " is ILLEGAL at address " << static_cast<int>(state.pc) << std::dec;
    EXECADDRESSING(adr, state);
}

/// ---- Storage Instructions ----

// Load accumulator from memory
void Disassembler6502::OP_LDA(State6502& state, AddressingPtr& adr) {
    LD(state, adr, state.a);
}
void Disassembler6502::OP_LDX(State6502& state, AddressingPtr& adr) {
    LD(state, adr, state.x);
}
void Disassembler6502::OP_LDY(State6502& state, AddressingPtr& adr) {
    LD(state, adr, state.y);
}

// Store accumulator in memory
void Disassembler6502::OP_STA(State6502& state, AddressingPtr& adr) {
    ST(state, adr, state.a);
}
void Disassembler6502::OP_STX(State6502& state, AddressingPtr& adr) {
    ST(state, adr, state.x);
}
void Disassembler6502::OP_STY(State6502& state, AddressingPtr& adr) {
    ST(state, adr, state.y);
}

// Transfer register to another register
// in form TQP, T: Transfer opcode, Q: src, P: dst

void Disassembler6502::OP_TAX(State6502& state, AddressingPtr& adr) {
    TR(state, adr, state.a, state.x);
}
void Disassembler6502::OP_TAY(State6502& state, AddressingPtr& adr) {
    TR(state, adr, state.a, state.y);
}
void Disassembler6502::OP_TSX(State6502& state, AddressingPtr& adr) {
    TR(state, adr, state.sp, state.x);
}
void Disassembler6502::OP_TXA(State6502& state, AddressingPtr& adr) {
    TR(state, adr, state.x, state.a);
}
void Disassembler6502::OP_TXS(State6502& state, AddressingPtr& adr) {
    // TXS does not modify processor state
    State6502::Status s = state.status;
    TR(state, adr, state.x, state.sp);
    std::swap(s, state.status);
}
void Disassembler6502::OP_TYA(State6502& state, AddressingPtr& adr) {
    TR(state, adr, state.y, state.a);
}

/// ---- Math Instructions ----



// Add with carry from memory (A + M + C -> A)
// https://stackoverflow.com/questions/29193303/6502-emulation-proper-way-to-implement-adc-and-sbc  and
// https://github.com/gianlucag/mos6502/blob/master/mos6502.cpp in ADC and SBC
void Disassembler6502::OP_ADC(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    uint16_t sum = state.a + byte + state.status.c;
#ifdef ALLOW_DEC
    if (state.status.d) {
        if ( (state.a & 0xF) + (byte & 0xF) + state.status.c > 9)
            sum += 6;
        setNegative(state, sum);
        state.status.o = ((state.a ^ sum) & (byte ^ sum) & 0x80) == 0x80;
        if (sum > 0x99)
            sum += 96;
        state.status.c = sum > 0x99;
    }
    else
#endif
    {
        setNegative(state, sum);
        state.status.o = ((state.a ^ sum) & (byte ^ sum) & 0x80) == 0x80;
        state.status.c = sum > 0xFF;
    }

    state.a = sum & 0xFF;
    setZero(state, state.a);
}

// Subtract memory from a (A - M - (1-C) -> A)
// Same sources used for ADC
void Disassembler6502::OP_SBC(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    uint16_t sum = state.a - byte - (1 - state.status.c);
    setNegative(state, sum);
    setZero(state, sum);
    state.status.o = ((state.a ^ sum) & 0x80) && ((state.a ^ byte) & 0x80);
#ifdef ALLOW_DEC
    if (state.status.d) {
        if ((state.a & 0x0F) - state.status.c < (byte & 0x0F))
            sum -= 6;
        if (sum >= 0x99)
            sum -= 0x60;
    }
#endif
    state.status.c = sum < 0x100;
    state.a = sum & 0xFF;
}

void Disassembler6502::OP_DEC(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    uint8_t byte = state.memory.read(address);
    DEC(state, byte);
    state.memory.write(address, byte);
}
void Disassembler6502::OP_DEX(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    DEC(state, state.x);
}
void Disassembler6502::OP_DEY(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    DEC(state, state.y);
}


void Disassembler6502::OP_INC(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    uint8_t byte = state.memory.read(address);
    INC(state, byte);
    state.memory.write(address, byte);
}
void Disassembler6502::OP_INX(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    INC(state, state.x);
}
void Disassembler6502::OP_INY(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    INC(state, state.y);
}



/// ----- Bitwise Instructions



// Binary AND w/ accumulator ( A & M -> A)
void Disassembler6502::OP_AND(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    state.a &= byte;
    setZero(state, state.a);
    setNegative(state, state.a);
}

// Binary OR w/ accumulator ( A | M -> A)
void Disassembler6502::OP_ORA(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    state.a |= byte;
    setZero(state, state.a);
    setNegative(state, state.a);
}

// Binary XOR w/ accumulator ( A ^ M -> A)
void Disassembler6502::OP_EOR(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    state.a ^= byte;
    setZero(state, state.a);
    setNegative(state, state.a);
}

// Test bits in memory with accumulator by using binary AND
// -> A & M, no registers modified
void Disassembler6502::OP_BIT(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    uint8_t sum = byte & state.a;
    setZero(state, sum);
    setNegative(state, byte);
    state.status.o = (byte & 0x40) >> 6;
}

// Arithmetric Shift Left
// Shift all bits left by 1, bit 0 is always 0
// The original 7 bit is shifted into the carry status flag
void Disassembler6502::OP_ASL(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    uint8_t byte = adr == &Disassembler6502::ADR_ACCUM ? state.a : state.memory.read(address);
    state.status.c = (byte & 0x80) >> 7;
    byte <<= 1;
    setZero(state, byte);
    setNegative(state, byte);
    if (adr == &Disassembler6502::ADR_ACCUM)
        state.a = byte;
    else
        state.memory.write(address, byte);
}

// Logical Shift Right
// Shift all bits right one position bit 7 is always 0, original 0 bit shifted into carry
void Disassembler6502::OP_LSR(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    uint8_t byte = adr == &Disassembler6502::ADR_ACCUM ? state.a : state.memory.read(address);
    state.status.c = byte & 1;
    byte >>= 1;
    setZero(state, byte);
    setNegative(state, byte);
    if (adr == &Disassembler6502::ADR_ACCUM)
        state.a = byte;
    else
        state.memory.write(address, byte);

}

// Rotate Left
// Shift all bits left by 1
// bit 0 is the carry flag and original bit 7 is now carry
void Disassembler6502::OP_ROL(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    uint8_t byte = adr == &Disassembler6502::ADR_ACCUM ? state.a : state.memory.read(address);
    uint8_t bit7 = (byte & 0x80) >> 7;
    byte <<= 1;
    byte |= state.status.c;
    state.status.c = bit7;
    setZero(state, byte);
    setNegative(state, byte);
    if (adr == &Disassembler6502::ADR_ACCUM)
        state.a = byte;
    else
        state.memory.write(address, byte);
}

// Rotate Right
// shfit right, carry bit goes into bit 7, original bit 0 is now carry
void Disassembler6502::OP_ROR(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    uint8_t byte = adr == &Disassembler6502::ADR_ACCUM ? state.a : state.memory.read(address);
    uint8_t bit0 = byte & 1;
    byte >>= 1;
    byte |= state.status.c << 7;
    state.status.c = bit0;
    setZero(state, byte);
    setNegative(state, byte);
    if (adr == &Disassembler6502::ADR_ACCUM)
        state.a = byte;
    else
        state.memory.write(address, byte);
}

/// ----- Branching Instructions



// The next BXX instructions:
// Branch/Jump if condition, specified in canBranch otherwise skip

void Disassembler6502::OP_BMI(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.n == 1;
    state.pc = EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BPL(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.n == 0;
    state.pc = EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BCC(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.c == 0;
    state.pc = EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BCS(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.c == 1;
    state.pc = EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BEQ(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.z == 1;
    state.pc = EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BNE(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.z == 0;
    state.pc = EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BVS(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.o == 1;
    state.pc = EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BVC(State6502& state, AddressingPtr& adr) {
    canBranch = state.status.o == 0;
    state.pc = EXECADDRESSING(adr, state);
}


/// ----- Jump Instructions


// Jump to a new location
void Disassembler6502::OP_JMP(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    state.pc = address;
}

// Jumps to subroutine:
// pushes address - 1 of the next operation before transfering pc
void Disassembler6502::OP_JSR(State6502& state, AddressingPtr& adr) {
    uint16_t transferAdr = EXECADDRESSING(adr, state);
    uint16_t address = state.pc - 1;
    PUSH(state, (address & 0xFF00) >> 8); // push high bits FIRST
    PUSH(state, (address & 0xFF)); // push low bits LAST (sp now points to low bits)
    state.pc = transferAdr;
}

// Return from Subroutine:
// sets pc to the popped stack's address + 1
void Disassembler6502::OP_RTS(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    uint8_t low = POP(state), high = POP(state);
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(high) << 8) | low );
    state.pc = address + 1;
}

// Return from Interrupt:
// get flags then pc from the stack, the pc is actual address, not address -1
void Disassembler6502::OP_RTI(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.fromByte(POP(state));
    uint8_t low = POP(state), high = POP(state);
    uint16_t address = static_cast<uint16_t>( (static_cast<uint16_t>(high) << 8) | low );
    state.pc = address;
}


/// ------- Register Instructions

 // Set Carry bit
void Disassembler6502::OP_SEC(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.c = 1;
}

// Clear Carry bit
void Disassembler6502::OP_CLC(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.c = 0;
}

// Set interrupt
void Disassembler6502::OP_SEI(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.i = 1;
}

// Clear Interrupt
void Disassembler6502::OP_CLI(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.i = 0;
}

// Set Decimal
void Disassembler6502::OP_SED(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.d = 1;
}

// Clear Decimal
void Disassembler6502::OP_CLD(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.d = 0;
}

// Clear Overflow
void Disassembler6502::OP_CLV(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.o = 0;
}

// Compare a byte with the accumulator by subtracting it from the accumulator
// Effectively A - M, note that it does not modify any registers only status flags
void Disassembler6502::OP_CMP(State6502& state, AddressingPtr& adr) {
    CMP(state, adr, state.a);
}
// comp with x register
void Disassembler6502::OP_CPX(State6502& state, AddressingPtr& adr) {
    CMP(state, adr, state.x);
}
// comp with y register
void Disassembler6502::OP_CPY(State6502& state, AddressingPtr& adr) {
    CMP(state, adr, state.y);
}


/// --- Stack Instructions
// Stack works from top to bottom, usually from first page 0x100 - 0x1FF
// All stack operations are implied
// Stack pointer always points to the next available byte to be pushed

// Push A to the stack
void Disassembler6502::OP_PHA(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    PUSH(state, state.a);
}

// Push Processor Status onto Stack
void Disassembler6502::OP_PHP(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.b = 1;
    PUSH(state, state.status.asByte());
    state.status.b = 0;
}

// Pull A from Stack
void Disassembler6502::OP_PLA(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.a = POP(state);
    setZero(state, state.a);
    setNegative(state, state.a);
}

// Pull Processor Status from Stack
void Disassembler6502::OP_PLP(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.fromByte(POP(state));
}


/// --- System Instructions


void Disassembler6502::OP_NOP(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
}

void Disassembler6502::OP_BRK(State6502& state, AddressingPtr& adr) {
    EXECADDRESSING(adr, state);
    state.status.b = 1;
    generateInterrupt(state, vectorIRQ); // BRK uses same as IRQ
}

