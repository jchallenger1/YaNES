#ifndef CPU6502_HPP
#define CPU6502_HPP

#include "Memory.hpp"
#include <cstdint>
#include <array>




class Cpu6502 {
    friend struct Tests;
    friend class NES;
    // Status registers
    struct Status {
        Status();
        uint8_t c : 1; // Carry flag
        uint8_t z : 1; // Zero flag
        uint8_t i : 1; // Interrupt Disable
        uint8_t d : 1; // Decimal Mode
        uint8_t b : 1; // Break Command, the flag is to identify from where an interrupt is coming from
        // Either as an instruction PHP/BRK or a signal IRQ/NMI, otherwise not used, https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
        uint8_t pad : 1; // padding, non status
        uint8_t o : 1; // Overflow Flag
        uint8_t n : 1; // Negative Flag

        void clear() noexcept;
        void reset() noexcept;
        uint8_t asByte() const noexcept;
        void fromByte(const uint8_t&) noexcept;
    };
    // First is the particular instruction
    // Second is the addressing mode w/ the instruction
    using AddressingPtr = uint16_t (Cpu6502::*)();
    using InstrFuncPtr = void (Cpu6502::*)(AddressingPtr&);
public:
    Cpu6502();
    Status status;

    Memory memory;

    void runCycle(const uint64_t& num = 1);

    void signalNMI();
    void signalRESET();
    void signalIRQ();

    void clear();


private:

    // Registers
    uint8_t a = 0; // Accumulator
    uint8_t x = 0; // Index Register X
    uint8_t y = 0; // Index Register Y

    uint8_t sp = 0; // Stack Pointer
    uint16_t pc = 0; // Program Counter

    uint64_t cycleCount = 0;
    uint64_t instrCount = 0;

    // Allow Decimal mode of Cpu, uneeded for NES
    bool cpuAllowDec = false;

    void fillOpTable();

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

    // General Interrupt Function
    inline void generateInterrupt(const uint16_t& vector);

    /// -- Flagging Operations --
    inline void setZero(const uint16_t&) noexcept;
    inline void setNegative(const uint16_t&)noexcept;

    /// -- General CPU functions --
    inline void LD(AddressingPtr&, uint8_t& reg); // Load
    inline void ST(AddressingPtr&, uint8_t& reg); // Store
    inline void TR(AddressingPtr&, const uint8_t& src, uint8_t& dst); // Transfer
    inline void INC(uint8_t& reg);
    inline void DEC(uint8_t& reg);
    inline void CMP(AddressingPtr&, const uint8_t& reg);
    inline void PUSH(const uint8_t& val);
    inline uint8_t POP();

    /// -- Addressing Mode Instructions --
    uint16_t ADR_IMPLICIT() noexcept;
    uint16_t ADR_ACCUM();
    uint16_t ADR_IMMEDIATE() noexcept;
    uint16_t ADR_ZEROPAGE();
    uint16_t ADR_ZEROPAGEX();
    uint16_t ADR_ZEROPAGEY();
    uint16_t ADR_RELATIVE();
    uint16_t ADR_ABS();
    uint16_t ADR_ABSX();
    uint16_t ADR_ABSY();
    uint16_t ADR_INDIRECT();
    uint16_t ADR_INDEXINDIRECT();
    uint16_t ADR_INDRECTINDEX();

    /// -- Opcode Instructions --
    [[ noreturn ]]
    void OP_ILLEGAL(AddressingPtr&);
    // Storage Instructions
    void OP_LDA(AddressingPtr&);
    void OP_LDX(AddressingPtr&);
    void OP_LDY(AddressingPtr&);
    void OP_STA(AddressingPtr&);
    void OP_STX(AddressingPtr&);
    void OP_STY(AddressingPtr&);
    void OP_TAX(AddressingPtr&);
    void OP_TAY(AddressingPtr&);
    void OP_TSX( AddressingPtr&);
    void OP_TXA(AddressingPtr&);
    void OP_TXS(AddressingPtr&);
    void OP_TYA( AddressingPtr&);

    // Math Instructions
    void OP_ADC(AddressingPtr&);
    void OP_SBC(AddressingPtr&);
    void OP_DEC(AddressingPtr&);
    void OP_DEX( AddressingPtr&);
    void OP_DEY(AddressingPtr&);
    void OP_INC( AddressingPtr&);
    void OP_INX(AddressingPtr&);
    void OP_INY(AddressingPtr&);

    // Bitwise Instructions
    void OP_AND(AddressingPtr&);
    void OP_ORA(AddressingPtr&);
    void OP_EOR(AddressingPtr&);
    void OP_BIT(AddressingPtr&);
    void OP_ASL(AddressingPtr&);
    void OP_LSR(AddressingPtr&);
    void OP_ROL(AddressingPtr&);
    void OP_ROR(AddressingPtr&);

    // Branch Instructions
    void OP_BMI(AddressingPtr&);
    void OP_BPL(AddressingPtr&);
    void OP_BCC(AddressingPtr&);
    void OP_BCS(AddressingPtr&);
    void OP_BEQ(AddressingPtr&);
    void OP_BNE(AddressingPtr&);
    void OP_BVS(AddressingPtr&);
    void OP_BVC(AddressingPtr&);

    // Jump Instructions
    void OP_JMP(AddressingPtr&);
    void OP_JSR(AddressingPtr&);
    void OP_RTS(AddressingPtr&);
    void OP_RTI(AddressingPtr&);

    // Register Instrucitons
    void OP_SEC(AddressingPtr&);
    void OP_CLC(AddressingPtr&);
    void OP_SEI(AddressingPtr&);
    void OP_CLI(AddressingPtr&);
    void OP_SED(AddressingPtr&);
    void OP_CLD(AddressingPtr&);
    void OP_CLV(AddressingPtr&);
    void OP_CMP(AddressingPtr&);
    void OP_CPX(AddressingPtr&);
    void OP_CPY(AddressingPtr&);

    // Stack
    void OP_PHA(AddressingPtr&);
    void OP_PHP(AddressingPtr&);
    void OP_PLA(AddressingPtr&);
    void OP_PLP(AddressingPtr&);

    // System
    void OP_NOP(AddressingPtr&);
    void OP_BRK(AddressingPtr&);

};


#endif // CPU6502_HPP
