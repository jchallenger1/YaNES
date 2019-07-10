#include "Disassembler6502.hpp"

#define EXECOPCODE(instrPtr, adringPtr, state) (this->*(instrPtr))((state), (adringPtr))
#define EXECADDRESSING(adringPtr, state) (this->*(adringPtr))(state)

Disassembler6502::Disassembler6502() {

    // LDA
    opcodeTable[0xA9] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_IMMEDIATE};
    opcodeTable[0xA5] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0xB5] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0xAD] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ABS};
    opcodeTable[0xBD] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ABSX};
    opcodeTable[0xB9] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_ABSY};
    opcodeTable[0xA1] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0xB1] = {&Disassembler6502::OP_LDA, &Disassembler6502::ADR_INDRECTINDEX};
    // STA
    opcodeTable[0x85] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ZEROPAGE};
    opcodeTable[0x95] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ZEROPAGEX};
    opcodeTable[0x8D] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ABS};
    opcodeTable[0x9D] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ABSX};
    opcodeTable[0x99] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_ABSY};
    opcodeTable[0x81] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_INDEXINDIRECT};
    opcodeTable[0x91] = {&Disassembler6502::OP_STA, &Disassembler6502::ADR_INDRECTINDEX};
}


void Disassembler6502::runCycle(State6502& state) {
    uint8_t opcode = state.memory.read(state.pc);
    Instr instruction = opcodeTable[opcode];
    EXECOPCODE(instruction.instr, instruction.addr, state);
}

///
/// \ Flagging Operations
///
///

// zero flag is set if value is equal to zero.
inline void Disassembler6502::setZero(State6502& state, const uint16_t& val) const {
    state.status.z = val == 0;
}

// Negative flag is set if value's bit 7 is set
inline void Disassembler6502::setNegative(State6502& state, const uint16_t& val) const {
    state.status.n = (val >> 7) & 1;
}

///
/// \ Addressing Operations
///

// Immediate: The data to be obtained is simply the next byte
uint16_t Disassembler6502::ADR_IMMEDIATE(State6502& state) {
    state.pc += 2;
    return state.pc + 1;
}

// ZeroPage: The data is in the location of the address of the next byte
// Limits the address from 0-256
uint16_t Disassembler6502::ADR_ZEROPAGE(State6502& state) {
    uint8_t address = state.memory.read(state.pc + 1);
    state.pc += 2;
    return address;

}
// ZeroPageX: Similar to ZeroPage, but address is added with register X
// Number will wrap around if address >= 256
uint16_t Disassembler6502::ADR_ZEROPAGEX(State6502& state) {
    uint8_t address = (state.memory.read(state.pc + 1) + state.x) % 256;
    state.pc += 2;
    return address;
}

// ZeroPageY: Same as X, but add Y instead
uint16_t Disassembler6502::ADR_ZEROPAGEY(State6502& state) {
    uint8_t address = (state.memory.read(state.pc + 1) + state.y) % 256;
    state.pc += 2;
    return address;
}

// Absolute: A full 16 bit address is used to identify target location
// Note that this system uses little endian architecture
// lowest bits @ 0, highest @ 1
uint16_t Disassembler6502::ADR_ABS(State6502& state) {
    uint16_t address = (static_cast<uint16_t>(state.memory.read(state.pc + 2)) << 8) & state.memory.read(state.pc + 1);
    state.pc += 3;
    return address;
}

// AbsoluteX: Similar to Absolute, but address is added with register X
// Assumption that no wrapping occurs
uint16_t Disassembler6502::ADR_ABSX(State6502& state) {
    uint16_t address = ((static_cast<uint16_t>(state.memory.read(state.pc + 2)) << 8) & state.memory.read(state.pc + 1)) + state.x;
    state.pc += 3;
    return address;
}

// AbsoluteX: Similar to Absolute, but address is added with register Y
// Assumption that no wrapping occurs
uint16_t Disassembler6502::ADR_ABSY(State6502& state) {
    uint16_t address = ((static_cast<uint16_t>(state.memory.read(state.pc + 2)) << 8) & state.memory.read(state.pc + 1)) + state.y;
    state.pc += 3;
    return address;
}

// IndirectIndexed/Indirect,Y: Get a full 16bit address from zero page(0-255) memory
// The next byte refers to a location in memory within range 0-255, p for simplicity
// p and p+1 is a full 16 bit location address, the address is then added with register Y to get the final address
// The byte is then that full location
uint16_t Disassembler6502::ADR_INDRECTINDEX(State6502& state) {
    uint8_t p = state.memory.read(state.pc + 1);
    uint16_t address = (static_cast<uint16_t>(state.memory.read(p + 1)) << 8) & state.memory.read(p);
    address += state.y;
    state.pc += 2;
    return address;
}

// IndexedIndirect/Indirect,X:
// Similar to above, but X is not added to the full address, rather it is added to p to specifiy where the low and high bits are
// Instead of p and p+1, it is p+x and p+x+1
uint16_t Disassembler6502::ADR_INDEXINDIRECT(State6502& state) {
    uint8_t p = state.memory.read(state.pc + 1);
    uint16_t address = (static_cast<uint16_t>(state.memory.read(p + state.x + 1)) << 8) & state.memory.read(p + state.x);
    state.pc += 2;
    return address;
}


// Load accumulator from memory
void Disassembler6502::OP_LDA(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    state.a = state.memory.read(address);
    setZero(state, state.a);
    setNegative(state, state.a);
}

// Store accumulator in memory
void Disassembler6502::OP_STA(State6502& state, AddressingPtr& adr) {
    uint16_t address = EXECADDRESSING(adr, state);
    state.memory.write(address, state.a);
}

void Disassembler6502::OP_AND(State6502&, AddressingPtr&) {

}
