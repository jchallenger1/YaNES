#include "tests.hpp"
#include "State6502.hpp"
#include "Disassembler6502.hpp"

void cpuMessage() { std::cout << " --- Running Opcode Test Cases ---\n"; }

// Test if all addressing modes work also check lda function
void cpuLdaAddressingTests() {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;
    memory.write(0, 0xA9); // check immediate
    memory.write(1, 0xFA);
    dis.runCycle(state);
    ckPassFail(state.pc == 0x2 && state.a == 0xFA, "Immediate failure");

    memory.write(2, 0xA5); // check zeropage
    memory.write(3, 0x00);
    dis.runCycle(state);
    ckPassFail(state.pc == 0x4 && state.a == 0xA9, "Zero page failure");

    memory.write(4, 0xB5); // check zero page X
    memory.write(5, 0x80);
    memory.write(0x8F, 0xBB);
    state.x = 0xF;
    dis.runCycle(state);
    ckPassFail(state.pc == 0x6 && state.a == 0xBB, "Zero page X failure");

    memory.write(6, 0xAD); // check ABS
    memory.write(7, 0x10);
    memory.write(8, 0xAF);
    memory.write(0xAF10, 0xAA);
    dis.runCycle(state);
    ckPassFail(state.pc == 0x9 && state.a == 0xAA, "ABS failure");

    state.clear();
    memory.write(0, 0x9A);
    state.x = 0xFA;
    state.sp = 022;
    dis.runCycle(state);
    ckPassFail(state.sp == 0xFA && state.x == 0xFA && state.pc == 0x1, "Implicit/Implied failure");
}

// These tests are from the examples databook
// Tests are important for these instructions as they are the hardest in the entire instruction set
void cpuMathTests() {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;

    // Regular Add test
    memory.write(0, 0x69);
    memory.write(1, 211);
    state.a = 13;
    state.status.c = 1;
    dis.runCycle(state);
    ckPassErr(state.a == 225 && state.status.c == 0, "2.1 ADC failure");
    state.clear();

    // Add with carry
    memory.write(0, 0x69);
    memory.write(1, 6);
    state.a = 254;
    state.status.c = 1;
    dis.runCycle(state);
    ckPassErr(state.a == 5 && state.status.c == 1, "2.2 ADC failure");
    state.clear();

    // Add two pos with overflow
    memory.write(0, 0x69);
    memory.write(1, 2);
    state.a = 127;
    state.status.c = 0;
    dis.runCycle(state);
    ckPassErr(state.a == 129 && state.status.o == 1, "2.7 ADC failure");
    state.clear();

    // Add two in decimal mode
    memory.write(0, 0x69);
    memory.write(1, 20); // 20 in BCD is 14
    state.status.c = 0;
    state.status.d = 1;
    state.a = 121; // 79 in BCD
    dis.runCycle(state);
    // 93 in BCD (79+14=93)
    ckPassErr(state.a == 147, "2.12 Decimal ADC failure");
    state.clear();

    // Sub 2 num w/ borrow w/ positive
    state.status.c = 1;
    state.a = 5;
    memory.write(0, 0xE9);
    memory.write(1, 3);

    dis.runCycle(state);
    ckPassErr(state.a == 2 && state.status.c == 1, "2.13 SBC failure");

    // Test INC in ABS
    state.clear();
    memory.write(0, 0xEE);
    memory.write(1, 0x21);
    memory.write(2, 0xF5);
    memory.write(0xF521, 0xFF);
    dis.runCycle(state);
    ckPassErr(memory.read(0xF521) == 0 && state.status.z == 1, "INC failure");
    // Test INX
    memory.write(3, 0xE8);
    state.x = 0xFB;
    dis.runCycle(state);
    ckPassErr(state.x == 0xFB + 1, "INX failure");
    // Test DEC in zeropagex
    state.x = 10;
    memory.write(0xE2 + state.x, 0xFE);
    memory.write(4, 0xD6);
    memory.write(5, 0xE2);
    dis.runCycle(state);
    ckPassErr(memory.read(0xE2 + state.x) == 0xFE - 1, "DEC failure");
    // Test DEY
    memory.write(6, 0x88);
    state.x = 0;
    dis.runCycle(state);
    ckPassErr(state.y == 0xFF && state.status.n == 1, "DEY failure");
}

// Tests cpu's and/or/xor, also tests shifting ASL/LSR and rotating ROL/ROR
void cpuBitwiseTests() {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;
    memory.write(0, 0x29);
    memory.write(1, 0b11001111);
    state.a = 0b11110111;
    dis.runCycle(state);
    ckPassErr(state.a == 0b11000111, "AND 2.19 failure");

    memory.write(2, 0x09);
    memory.write(3, 0b11101111);
    state.a = 0x8;
    dis.runCycle(state);
    ckPassErr(state.a == 0b11101111, "OR 2.20 failure");

    memory.write(4, 0x49);
    memory.write(5, 0b10101111);
    state.a = 0xFF;
    dis.runCycle(state);
    ckPassErr(state.a == 0b01010000, "XOR 2.21 failure");

    state.clear();
    state.a = 0xFF; // test ASL ACC
    state.memory.write(0, 0x0A);
    dis.runCycle(state);
    ckPassErr(state.a == 0xFE && state.status.c == 1, "ASL ACC failure");

    state.status.clear();
    memory.write(1, 0x4E);
    memory.write(2, 0x23);
    memory.write(3, 0x5F);
    memory.write(0x5F23, 1);
    dis.runCycle(state);
    ckPassErr(memory.read(0x5F23) == 0 && state.status.z == 1 && state.status.c == 1 && state.status.n == 0, "LSR ABS failure");

    state.status.clear();
    state.status.c = 1;
    memory.write(4, 0x26);
    memory.write(5, 0xFA);
    memory.write(0xFA, 0b01111111);
    dis.runCycle(state);
    ckPassErr(memory.read(0xFA) == 0xFF && state.status.c == 0 && state.status.z == 0 && state.status.n == 1, "ROL ZeroPage failure");

    state.status.clear();
    state.a = 0x7F;
    memory.write(6, 0x6A);
    dis.runCycle(state);
    ckPassErr(state.a == 0x3F && state.status.c == 1 && state.status.z == 0 && state.status.n == 0 && state.pc == 7, "ROR ACC failure");
}

// Tests cpu's clearing and setting status
void cpuStatusTests() {

    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;

    memory.write(0, 0x38);
    dis.runCycle(state);
    ckPassErr(state.status.c == 1, "SEC failure");

    memory.write(1, 0x18);
    dis.runCycle(state);
    ckPassErr(state.status.c == 0, "CLC failure");


    memory.write(2, 0x78);
    dis.runCycle(state);
    ckPassErr(state.status.i == 1, "SEI failure");

    memory.write(3, 0x58);
    dis.runCycle(state);
    ckPassErr(state.status.i == 0, "CLI failure");

    memory.write(4, 0xF8);
    dis.runCycle(state);
    ckPassErr(state.status.d == 1, "SED failure");

    memory.write(5, 0xD8);
    dis.runCycle(state);
    ckPassErr(state.status.d == 0, "CLD failure");

    state.status.o = 1;
    memory.write(6, 0xB8);
    dis.runCycle(state);
    ckPassErr(state.status.o == 0, "CLV failure");
}


// Tests Cpu's jumping and branching functions
void cpuJumpBranchTests() {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;
    memory.write(0, 0x4C);
    memory.write(1, 0x56);
    memory.write(2, 0xC9);
    dis.runCycle(state);
    ckPassErr(state.pc == 0xC956, "ABS JMP failure");
    state.clear();

    memory[0xFA23] = 0x65;
    memory[0xFA23 + 1] = 0xB3;
    memory[0] = 0x6C;
    memory[1] = 0x23;
    memory[2] = 0xFA;
    dis.runCycle(state);
    ckPassErr(state.pc == 0xB365, "INDR JMP failure");
    state.clear();

    memory.write(0x8000, 0x40);
    memory.write(0x80FF, 0x80);
    memory.write(0x8100, 0x50);
    memory.write(0, 0x6C);
    memory.write(1, 0xFF);
    memory.write(2, 0x80);
    dis.runCycle(state);
    ckPassErr(state.pc == 0x4080, "INDR JMP page boundary failure");

    state.clear();
    state.pc = 10;
    memory.write(10, 0x10);
    memory.write(11, 0x56);
    dis.runCycle(state);
    ckPassFail(state.pc == 10 + 0x56 + 2, "Branch/Relative mode failure, can't test next one");

    memory.write(10 + 0x56 + 2, 0x70);
    memory.write(10 + 0x56 + 3, 0xFF);
    dis.runCycle(state);
    ckPassErr(state.pc == 10 + 0x56 + 4, "branch/relative mode no branching failure");
}

// Tests Cpu's compare function w/accumulator, also tests BIT compare
void cpuCompareTests() {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;

    memory.write(0, 0xC9);
    memory.write(1, 0x62);
    state.a = 0xF2;
    dis.runCycle(state);
    ckPassErr(state.status.z == 0 && state.status.n == 1 && state.status.c == 1, "CMP a>m failure");

    memory.write(2, 0xC9);
    memory.write(3, 0xC4);
    state.a = 0xC4;
    dis.runCycle(state);
    ckPassErr(state.status.z == 1 && state.status.n == 0 && state.status.c == 1, "CMP a=m failure");

    memory.write(4, 0xC9);
    memory.write(5, 0xF8);
    state.a = 0xC8;
    dis.runCycle(state);
    ckPassErr(state.status.z == 0 && state.status.n == 1 && state.status.c == 0, "CMP a<m failure");

    // bit tests
    state.clear();
    memory.write(0, 0x24);
    memory.write(1, 128);
    state.a = 127;
    dis.runCycle(state);
    ckPassErr(state.status.z == 1 && state.status.c == 0 && state.status.o == 0, "BIT failure 1");

    memory.write(2, 0x24);
    memory.write(0xA1, 0xC9);
    memory.write(3, 0xA1); // zero page addressing
    state.a = 0xF5;
    dis.runCycle(state);
    ckPassErr(state.status.z == 0 && state.status.n == 1 && state.status.o == 1, "BIT failure 2");
}

// Tests Stack's pulling and pushing PHP/PHA/PLA/PLP, also calling and returning
void cpuStackTests() {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;

    state.sp = 0x56;
    // General stack tests
    state.status.clear();
    state.status.z = state.status.n = state.status.o = 1;
    // write a 3 times, write status once
    state.a = 0xA4;
    memory.write(0, 0x48);
    dis.runCycle(state);
    state.a = 0xF5;
    memory.write(1, 0x48);
    memory.write(2, 0x08);
    dis.runN(state, 2);
    state.a = 0x78;
    memory.write(3, 0x48);
    dis.runCycle(state);
    ckPassFail(state.sp == 0x56 - 4, "PHP/PHA failure, cannot continue");
    // Pop all of them off
    state.a = 0;
    state.status.clear();

    memory.write(4, 0x68);
    dis.runCycle(state);
    ckPassFail(state.a == 0x78, "PLA failure 3, cannot continue");

    memory.write(5, 0x28);
    dis.runCycle(state);
    ckPassFail(state.status.z && state.status.n && state.status.o && !state.status.c && !state.status.i, "PLP failure, cannot continue");

    memory.write(6, 0x68);
    dis.runCycle(state);
    ckPassFail(state.a == 0xF5, "PLA failure 2");

    memory.write(7, 0x68);
    dis.runCycle(state);
    ckPassFail(state.a == 0xA4, "PLA failure 1");
    ckPassErr(state.sp == 0x56, "sp failure in stack");


    // Function stack tests
    state.clear();


    state.sp = 0x10;
    state.pc = 0x10;
    state.memory.write(0x10, 0x20);
    state.memory.write(0x11, 0xF5);
    state.memory.write(0x12, 0xC4);
    dis.runCycle(state);
    ckPassFail(state.sp == 0x10 - 2 && state.pc == 0xC4F5, "JSR failure, cannot continue");

    state.memory[0xC4F5] = 0xA9;
    state.memory[0xC4F5 + 1] = 0xEA;
    dis.runCycle(state);
    ckPassErr(state.a == 0xEA, "partial failure in jumped address");

    state.memory[0xC4F5 + 2] = 0x60;
    dis.runCycle(state);
    ckPassFail(state.sp == 0x10 && state.pc == 0x13, "RST failure");

    state.memory.write(0x13, 0xA9);
    state.memory.write(0x14, 0x55);
    dis.runCycle(state);
    ckPassErr(state.a == 0x55, "partial failure in returned address");

}
