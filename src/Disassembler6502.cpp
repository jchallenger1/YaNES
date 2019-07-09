#include "Disassembler6502.hpp"

//#define EXECOPCODE(instrPtr, adringPtr, state) (this.*(instrPtr))(state, adringPtr)
#define EXECOPCODE(instrPtr, adringPtr, state) (this->*(instrPtr))((state), (adringPtr))


Disassembler6502::Disassembler6502() {

}


void Disassembler6502::runCycle(State6502& state) {
    Instr instruction = opcodeTable[state.pc];
    EXECOPCODE(instruction.first, instruction.second, state);
}


uint16_t Disassembler6502::ADR_IMMEDIATE(State6502&) {

}


void Disassembler6502::OP_AND(State6502&, addressingPtr&) {

}
