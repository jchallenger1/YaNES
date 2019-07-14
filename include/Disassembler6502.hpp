#ifndef DISASSEMBLER6502_HPP
#define DISASSEMBLER6502_HPP

#include "State6502.hpp"
#include <utility>
#include <array>

class Disassembler6502 {
public:
    // First is the particular instruction
    // Second is the addressing mode w/ the instruction
    using AddressingPtr = uint16_t (Disassembler6502::*)(State6502&);
    using InstrFuncPtr = void (Disassembler6502::*)(State6502&, AddressingPtr&);

    Disassembler6502();
    
    void runCycle(State6502&);
private:

    struct Instr {
        InstrFuncPtr instr;
        AddressingPtr addr;
    };

    std::array<Instr, 0xFF> opcodeTable;
    bool canBranch; // used by branching instructions

    /// -- Opcode Instructions --
    // Storage Instructions
    void OP_LDA(State6502&, AddressingPtr&);
    void OP_STA(State6502&, AddressingPtr&);
    // Math Instructions
    void OP_ADC(State6502&, AddressingPtr&);
    void OP_SBC(State6502&, AddressingPtr&);
    // Bitwise Instructions
    void OP_AND(State6502&, AddressingPtr&);
    void OP_ORA(State6502&, AddressingPtr&);
    void OP_EOR(State6502&, AddressingPtr&);
    void OP_BIT(State6502&, AddressingPtr&);
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
    // Register Instrucitons
    void OP_SEC(State6502&, AddressingPtr&);
    void OP_CLC(State6502&, AddressingPtr&);
    void OP_SEI(State6502&, AddressingPtr&);
    void OP_CLI(State6502&, AddressingPtr&);
    void OP_SED(State6502&, AddressingPtr&);
    void OP_CLD(State6502&, AddressingPtr&);
    void OP_CLV(State6502&, AddressingPtr&);
    void OP_CMP(State6502&, AddressingPtr&);

    /// -- Addressing Mode Instructions --
    uint16_t ADR_IMPLICIT(State6502&);
    uint16_t ADR_ACCUM(State6502&);
    uint16_t ADR_IMMEDIATE(State6502&);
    uint16_t ADR_ZEROPAGE(State6502&);
    uint16_t ADR_ZEROPAGEX(State6502&);
    uint16_t ADR_ZEROPAGEY(State6502&);
    uint16_t ADR_RELATIVE(State6502&);
    uint16_t ADR_ABS(State6502&);
    uint16_t ADR_ABSX(State6502&);
    uint16_t ADR_ABSY(State6502&);
    uint16_t ADR_INDIRECT(State6502&);
    uint16_t ADR_INDEXINDIRECT(State6502&);
    uint16_t ADR_INDRECTINDEX(State6502&);

    /// -- Flagging Operations --
    inline void setZero(State6502&, const uint16_t&) const;
    inline void setNegative(State6502&, const uint16_t&) const;

    /// -- General CPU functions --
    inline void LDA(State6502&, AddressingPtr& adr, uint8_t& reg);


};

#endif // DISASSEMBLER6502_HPP
