#ifndef DISASSEMBLER6502_HPP
#define DISASSEMBLER6502_HPP

#include "State6502.hpp"
#include <utility>
#include <array>

class Disassembler6502 {
public:
    // First is the particular instruction
    // Second is the addressing mode w/ the instruction
    using AddressingPtr = uint16_t (Disassembler6502::*)(State6502&) const;
    using InstrFuncPtr = void (Disassembler6502::*)(State6502&, AddressingPtr&);

    Disassembler6502();
    
    void runCycle(State6502&);
    void runN(State6502&, const int& num);

    void signalNMI(State6502&);
    void signalRESET(State6502&);
    void signalIRQ(State6502&);
private:
    // Vectors are vector pointers pointing to an address where the pc should be
    // Each variable is where the signal's vector points to, the value is the low byte of the address
    constexpr static uint16_t vectorNMI = 0xFFFA;
    constexpr static uint16_t vectorRESET = 0xFFFC;
    constexpr static uint16_t vectorIRQ = 0xFFFE; // note that IRQ and BRK use the same vector

    struct Instr {
        InstrFuncPtr instr;
        AddressingPtr addr;
    };

    std::array<Instr, 0xFF> opcodeTable;
    bool canBranch; // used by branching instructions

    /// -- Opcode Instructions --
    // Storage Instructions
    void OP_LDA(State6502&, AddressingPtr&);
    void OP_LDX(State6502&, AddressingPtr&);
    void OP_LDY(State6502&, AddressingPtr&);
    void OP_STA(State6502&, AddressingPtr&);
    void OP_STX(State6502&, AddressingPtr&);
    void OP_STY(State6502&, AddressingPtr&);
    void OP_TAX(State6502&, AddressingPtr&);
    void OP_TAY(State6502&, AddressingPtr&);
    void OP_TSX(State6502&, AddressingPtr&);
    void OP_TXA(State6502&, AddressingPtr&);
    void OP_TXS(State6502&, AddressingPtr&);
    void OP_TYA(State6502&, AddressingPtr&);
    // Math Instructions
    void OP_ADC(State6502&, AddressingPtr&);
    void OP_SBC(State6502&, AddressingPtr&);
    void OP_DEC(State6502&, AddressingPtr&);
    void OP_DEX(State6502&, AddressingPtr&);
    void OP_DEY(State6502&, AddressingPtr&);
    void OP_INC(State6502&, AddressingPtr&);
    void OP_INX(State6502&, AddressingPtr&);
    void OP_INY(State6502&, AddressingPtr&);
    // Bitwise Instructions
    void OP_AND(State6502&, AddressingPtr&);
    void OP_ORA(State6502&, AddressingPtr&);
    void OP_EOR(State6502&, AddressingPtr&);
    void OP_BIT(State6502&, AddressingPtr&);
    void OP_ASL(State6502&, AddressingPtr&);
    void OP_LSR(State6502&, AddressingPtr&);
    void OP_ROL(State6502&, AddressingPtr&);
    void OP_ROR(State6502&, AddressingPtr&);
    // Branch Instructions
    void OP_BMI(State6502&, AddressingPtr&);
    void OP_BPL(State6502&, AddressingPtr&);
    void OP_BCC(State6502&, AddressingPtr&);
    void OP_BCS(State6502&, AddressingPtr&);
    void OP_BEQ(State6502&, AddressingPtr&);
    void OP_BNE(State6502&, AddressingPtr&);
    void OP_BVS(State6502&, AddressingPtr&);
    void OP_BVC(State6502&, AddressingPtr&);
    // Jump Instructions
    void OP_JMP(State6502&, AddressingPtr&);
    void OP_JSR(State6502&, AddressingPtr&);
    void OP_RTS(State6502&, AddressingPtr&);
    void OP_RTI(State6502&, AddressingPtr&);
    // Register Instrucitons
    void OP_SEC(State6502&, AddressingPtr&);
    void OP_CLC(State6502&, AddressingPtr&);
    void OP_SEI(State6502&, AddressingPtr&);
    void OP_CLI(State6502&, AddressingPtr&);
    void OP_SED(State6502&, AddressingPtr&);
    void OP_CLD(State6502&, AddressingPtr&);
    void OP_CLV(State6502&, AddressingPtr&);
    void OP_CMP(State6502&, AddressingPtr&);
    void OP_CPX(State6502&, AddressingPtr&);
    void OP_CPY(State6502&, AddressingPtr&);
    // Stack
    void OP_PHA(State6502&, AddressingPtr&);
    void OP_PHP(State6502&, AddressingPtr&);
    void OP_PLA(State6502&, AddressingPtr&);
    void OP_PLP(State6502&, AddressingPtr&);
    // System
    void OP_NOP(State6502&, AddressingPtr&);
    void OP_BRK(State6502&, AddressingPtr&);

    /// -- Addressing Mode Instructions --
    uint16_t ADR_IMPLICIT(State6502&) const noexcept;
    uint16_t ADR_ACCUM(State6502&) const;
    uint16_t ADR_IMMEDIATE(State6502&) const noexcept;
    uint16_t ADR_ZEROPAGE(State6502&) const;
    uint16_t ADR_ZEROPAGEX(State6502&) const;
    uint16_t ADR_ZEROPAGEY(State6502&) const;
    uint16_t ADR_RELATIVE(State6502&) const;
    uint16_t ADR_ABS(State6502&) const;
    uint16_t ADR_ABSX(State6502&) const;
    uint16_t ADR_ABSY(State6502&) const;
    uint16_t ADR_INDIRECT(State6502&) const;
    uint16_t ADR_INDEXINDIRECT(State6502&) const;
    uint16_t ADR_INDRECTINDEX(State6502&) const;

    /// -- Flagging Operations --
    inline void setZero(State6502&, const uint16_t&) const noexcept;
    inline void setNegative(State6502&, const uint16_t&) const noexcept;

    /// -- General CPU functions --
    inline void LD(State6502&, AddressingPtr&, uint8_t& reg) const; // Load
    inline void ST(State6502&, AddressingPtr&, uint8_t& reg) const; // Store
    inline void TR(State6502&, AddressingPtr&, const uint8_t& src, uint8_t& dst) const; // Transfer
    inline void INC(State6502&, uint8_t& reg) const;
    inline void DEC(State6502&, uint8_t& reg) const;
    inline void CMP(State6502&, AddressingPtr&, const uint8_t& reg) const;
    inline void PUSH(State6502&, const uint8_t& val) const;
    inline uint8_t POP(State6502&) const;

    inline void generateInterrupt(State6502&, const uint16_t& vector) const;

};

#endif // DISASSEMBLER6502_HPP
