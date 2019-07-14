#include <iostream>
#include "Disassembler6502.hpp"

#define EXECOPCODE(instrPtr, adringPtr, state) (this->*(instrPtr))((state), (adringPtr))
#define EXECADDRESSING(adringPtr, state) (this->*(adringPtr))(state)

Disassembler6502::Disassembler6502() {

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
    /// ----- Stack Instructions -------
    /// ----- System Instructions ------
}


void Disassembler6502::runCycle(State6502& state) {
    uint8_t opcode = state.memory.read(state.pc);
    Instr instruction = opcodeTable[opcode];
    EXECOPCODE(instruction.instr, instruction.addr, state);
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
    return isPositive ? state.pc + offset : state.pc - offset;
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

inline void Disassembler6502::ST(State6502& state, AddressingPtr& adr, uint8_t& reg) const {
    uint16_t address = EXECADDRESSING(adr, state);
    state.memory.write(address, reg);
}
///
///
/// ---------------- Opcode Functions ----------------
///
///


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

/// ---- Math Instructions ----


// Add with carry from memory (A + M + C -> A)
// https://stackoverflow.com/questions/29193303/6502-emulation-proper-way-to-implement-adc-and-sbc  and
// https://github.com/gianlucag/mos6502/blob/master/mos6502.cpp in ADC and SBC
void Disassembler6502::OP_ADC(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    uint16_t sum = state.a + byte + state.status.c;

    if (state.status.d) {
        if ( (state.a & 0xF) + (byte & 0xF) + state.status.c > 9)
            sum += 6;
        setNegative(state, sum);
        state.status.o = ((state.a ^ sum) & (byte ^ sum) & 0x80) == 0x80;
        if (sum > 0x99)
            sum += 96;
        state.status.c = sum > 0x99;
    }
    else {
        setNegative(state, sum);
        state.status.o = ((state.a ^ sum) & (byte ^ sum) & 0x80) == 0x80;
        state.status.c = sum > 0xFF;
    }

    state.a = sum & 0xFF;
}

// Subtract memory from a (A - M - C -> A)
// Same sources used for ADC
void Disassembler6502::OP_SBC(State6502& state, AddressingPtr& adr) {
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    uint16_t sum = state.a - byte - state.status.c;
    setNegative(state, sum);
    setZero(state, sum);
    state.status.o = ((state.a ^ sum) & (byte ^ sum) & 0x80) == 0x80;

    if (state.status.d) {
        if ((state.a & 0x0F) - state.status.c < (byte & 0x0F))
            sum -= 6;
        if (sum >= 0x99)
            sum -= 0x60;
    }

    state.status.c = sum < 0x100;
    state.a = sum & 0xFF;
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
    setNegative(state, sum);
    state.status.o = (sum & 0x40) >> 6;
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
    uint8_t byte = state.memory.read(EXECADDRESSING(adr, state));
    uint8_t sum = state.a + (~byte + 1);
    setZero(state, sum);
    setNegative(state, sum);
    state.status.c = byte <= state.a;

}


