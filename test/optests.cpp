#include "tests.hpp"
#include "Cpu6502.hpp"

void Tests::cpuMessage() { std::cout << " --- Running Opcode Test Cases ---\n"; }

// Test if all addressing modes work also check lda function
void Tests::cpuLdaAddressingTests() {
    Cpu6502 cpu;
    auto& memory = cpu.memory;
    memory.write(0, 0xA9); // check immediate
    memory.write(1, 0xFA);
    cpu.runCycle();
    ckPassFail(cpu.pc == 0x2 && cpu.a == 0xFA, "Immediate failure");

    memory.write(2, 0xA5); // check zeropage
    memory.write(3, 0x00);
    cpu.runCycle();
    ckPassFail(cpu.pc == 0x4 && cpu.a == 0xA9, "Zero page failure");

    memory.write(4, 0xB5); // check zero page X
    memory.write(5, 0x80);
    memory.write(0x8F, 0xBB);
    cpu.x = 0xF;
    cpu.runCycle();
    ckPassFail(cpu.pc == 0x6 && cpu.a == 0xBB, "Zero page X failure");

    memory.write(6, 0xAD); // check ABS
    memory.write(7, 0x10);
    memory.write(8, 0xAF);
    memory.write(0xAF10, 0xAA);
    cpu.runCycle();
    ckPassFail(cpu.pc == 0x9 && cpu.a == 0xAA, "ABS failure");

    cpu.clear();
    memory.write(0, 0x9A);
    cpu.x = 0xFA;
    cpu.sp = 022;
    cpu.runCycle();
    ckPassFail(cpu.sp == 0xFA && cpu.x == 0xFA && cpu.pc == 0x1, "Implicit/Implied failure");
}

// These tests are from the examples databook
// Tests are important for these instructions as they are the hardest in the entire instruction set
void Tests::cpuMathTests() {
    Cpu6502 cpu;
    auto& memory = cpu.memory;

    // Regular Add test
    memory.write(0, 0x69);
    memory.write(1, 211);
    cpu.a = 13;
    cpu.status.c = 1;
    cpu.runCycle();
    ckPassErr(cpu.a == 225 && cpu.status.c == 0, "2.1 ADC failure");
    cpu.clear();

    // Add with carry
    memory.write(0, 0x69);
    memory.write(1, 6);
    cpu.a = 254;
    cpu.status.c = 1;
    cpu.runCycle();
    ckPassErr(cpu.a == 5 && cpu.status.c == 1, "2.2 ADC failure");
    cpu.clear();

    // Add two pos with overflow
    memory.write(0, 0x69);
    memory.write(1, 2);
    cpu.a = 127;
    cpu.status.c = 0;
    cpu.runCycle();
    ckPassErr(cpu.a == 129 && cpu.status.o == 1, "2.7 ADC failure");
    cpu.clear();

    // Add two in decimal mode
    memory.write(0, 0x69);
    memory.write(1, 20); // 20 in BCD is 14
    cpu.status.c = 0;
    cpu.status.d = 1;
    cpu.a = 121; // 79 in BCD
    cpu.runCycle();
    // 93 in BCD (79+14=93)
    ckPassErr(cpu.a == 147, "2.12 Decimal ADC failure");
    cpu.clear();

    // Sub 2 num w/ borrow w/ positive
    cpu.status.c = 1;
    cpu.a = 5;
    memory.write(0, 0xE9);
    memory.write(1, 3);

    cpu.runCycle();
    ckPassErr(cpu.a == 2 && cpu.status.c == 1, "2.13 SBC failure");

    // Test INC in ABS
    cpu.clear();
    memory.write(0, 0xEE);
    memory.write(1, 0x21);
    memory.write(2, 0xF5);
    memory.write(0xF521, 0xFF);
    cpu.runCycle();
    ckPassErr(memory.read(0xF521) == 0 && cpu.status.z == 1, "INC failure");
    // Test INX
    memory.write(3, 0xE8);
    cpu.x = 0xFB;
    cpu.runCycle();
    ckPassErr(cpu.x == 0xFB + 1, "INX failure");
    // Test DEC in zeropagex
    cpu.x = 10;
    memory.write(0xE2 + cpu.x, 0xFE);
    memory.write(4, 0xD6);
    memory.write(5, 0xE2);
    cpu.runCycle();
    ckPassErr(memory.read(0xE2 + cpu.x) == 0xFE - 1, "DEC failure");
    // Test DEY
    memory.write(6, 0x88);
    cpu.x = 0;
    cpu.runCycle();
    ckPassErr(cpu.y == 0xFF && cpu.status.n == 1, "DEY failure");
}

// Tests cpu's and/or/xor, also tests shifting ASL/LSR and rotating ROL/ROR
void Tests::cpuBitwiseTests() {
    Cpu6502 cpu;
    auto& memory = cpu.memory;
    memory.write(0, 0x29);
    memory.write(1, 0b11001111);
    cpu.a = 0b11110111;
    cpu.runCycle();
    ckPassErr(cpu.a == 0b11000111, "AND 2.19 failure");

    memory.write(2, 0x09);
    memory.write(3, 0b11101111);
    cpu.a = 0x8;
    cpu.runCycle();
    ckPassErr(cpu.a == 0b11101111, "OR 2.20 failure");

    memory.write(4, 0x49);
    memory.write(5, 0b10101111);
    cpu.a = 0xFF;
    cpu.runCycle();
    ckPassErr(cpu.a == 0b01010000, "XOR 2.21 failure");

    cpu.clear();
    cpu.a = 0xFF; // test ASL ACC
    cpu.memory.write(0, 0x0A);
    cpu.runCycle();
    ckPassErr(cpu.a == 0xFE && cpu.status.c == 1, "ASL ACC failure");

    cpu.status.clear();
    memory.write(1, 0x4E);
    memory.write(2, 0x23);
    memory.write(3, 0x5F);
    memory.write(0x5F23, 1);
    cpu.runCycle();
    ckPassErr(memory.read(0x5F23) == 0 && cpu.status.z == 1 && cpu.status.c == 1 && cpu.status.n == 0, "LSR ABS failure");

    cpu.status.clear();
    cpu.status.c = 1;
    memory.write(4, 0x26);
    memory.write(5, 0xFA);
    memory.write(0xFA, 0b01111111);
    cpu.runCycle();
    ckPassErr(memory.read(0xFA) == 0xFF && cpu.status.c == 0 && cpu.status.z == 0 && cpu.status.n == 1, "ROL ZeroPage failure");

    cpu.status.clear();
    cpu.a = 0x7F;
    memory.write(6, 0x6A);
    cpu.runCycle();
    ckPassErr(cpu.a == 0x3F && cpu.status.c == 1 && cpu.status.z == 0 && cpu.status.n == 0 && cpu.pc == 7, "ROR ACC failure");
}

// Tests cpu's clearing and setting status
void Tests::cpuStatusTests() {
    Cpu6502 cpu;
    auto& memory = cpu.memory;

    memory.write(0, 0x38);
    cpu.runCycle();
    ckPassErr(cpu.status.c == 1, "SEC failure");

    memory.write(1, 0x18);
    cpu.runCycle();
    ckPassErr(cpu.status.c == 0, "CLC failure");


    memory.write(2, 0x78);
    cpu.runCycle();
    ckPassErr(cpu.status.i == 1, "SEI failure");

    memory.write(3, 0x58);
    cpu.runCycle();
    ckPassErr(cpu.status.i == 0, "CLI failure");

    memory.write(4, 0xF8);
    cpu.runCycle();
    ckPassErr(cpu.status.d == 1, "SED failure");

    memory.write(5, 0xD8);
    cpu.runCycle();
    ckPassErr(cpu.status.d == 0, "CLD failure");

    cpu.status.o = 1;
    memory.write(6, 0xB8);
    cpu.runCycle();
    ckPassErr(cpu.status.o == 0, "CLV failure");
}


// Tests Cpu's jumping and branching functions
void Tests::cpuJumpBranchTests() {
    Cpu6502 cpu;
    auto& memory = cpu.memory;
    memory.write(0, 0x4C);
    memory.write(1, 0x56);
    memory.write(2, 0xC9);
    cpu.runCycle();
    ckPassErr(cpu.pc == 0xC956, "ABS JMP failure");
    cpu.clear();

    memory[0xFA23] = 0x65;
    memory[0xFA23 + 1] = 0xB3;
    memory[0] = 0x6C;
    memory[1] = 0x23;
    memory[2] = 0xFA;
    cpu.runCycle();
    ckPassErr(cpu.pc == 0xB365, "INDR JMP failure");
    cpu.clear();

    memory.write(0x8000, 0x40);
    memory.write(0x80FF, 0x80);
    memory.write(0x8100, 0x50);
    memory.write(0, 0x6C);
    memory.write(1, 0xFF);
    memory.write(2, 0x80);
    cpu.runCycle();
    ckPassErr(cpu.pc == 0x4080, "INDR JMP page boundary failure");

    cpu.clear();
    cpu.pc = 10;
    memory.write(10, 0x10);
    memory.write(11, 0x56);
    cpu.runCycle();
    ckPassFail(cpu.pc == 10 + 0x56 + 2, "Branch/Relative mode failure, can't test next one");

    memory.write(10 + 0x56 + 2, 0x70);
    memory.write(10 + 0x56 + 3, 0xFF);
    cpu.runCycle();
    ckPassErr(cpu.pc == 10 + 0x56 + 4, "branch/relative mode no branching failure");
}

// Tests Cpu's compare function w/accumulator, also tests BIT compare
void Tests::cpuCompareTests() {
    Cpu6502 cpu;
    auto& memory = cpu.memory;

    memory.write(0, 0xC9);
    memory.write(1, 0x62);
    cpu.a = 0xF2;
    cpu.runCycle();
    ckPassErr(cpu.status.z == 0 && cpu.status.n == 1 && cpu.status.c == 1, "CMP a>m failure");

    memory.write(2, 0xC9);
    memory.write(3, 0xC4);
    cpu.a = 0xC4;
    cpu.runCycle();
    ckPassErr(cpu.status.z == 1 && cpu.status.n == 0 && cpu.status.c == 1, "CMP a=m failure");

    memory.write(4, 0xC9);
    memory.write(5, 0xF8);
    cpu.a = 0xC8;
    cpu.runCycle();
    ckPassErr(cpu.status.z == 0 && cpu.status.n == 1 && cpu.status.c == 0, "CMP a<m failure");

    // bit tests
    cpu.clear();
    memory.write(0, 0x24);
    memory.write(1, 128);
    cpu.a = 127;
    cpu.runCycle();
    ckPassErr(cpu.status.z == 1 && cpu.status.c == 0 && cpu.status.o == 0, "BIT failure 1");

    memory.write(2, 0x24);
    memory.write(0xA1, 0xC9);
    memory.write(3, 0xA1); // zero page addressing
    cpu.a = 0xF5;
    cpu.runCycle();
    ckPassErr(cpu.status.z == 0 && cpu.status.n == 1 && cpu.status.o == 1, "BIT failure 2");
}

// Tests Stack's pulling and pushing PHP/PHA/PLA/PLP, also calling and returning
void Tests::cpuStackTests() {
    Cpu6502 cpu;
    auto& memory = cpu.memory;

    cpu.sp = 0x56;
    // General stack tests
    cpu.status.clear();
    cpu.status.z = cpu.status.n = cpu.status.o = 1;
    // write a 3 times, write status once
    cpu.a = 0xA4;
    memory.write(0, 0x48);
    cpu.runCycle();
    cpu.a = 0xF5;
    memory.write(1, 0x48);
    memory.write(2, 0x08);
    cpu.runCycle(2);
    cpu.a = 0x78;
    memory.write(3, 0x48);
    cpu.runCycle();
    ckPassFail(cpu.sp == 0x56 - 4, "PHP/PHA failure, cannot continue");
    // Pop all of them off
    cpu.a = 0;
    cpu.status.clear();

    memory.write(4, 0x68);
    cpu.runCycle();
    ckPassFail(cpu.a == 0x78, "PLA failure 3, cannot continue");

    memory.write(5, 0x28);
    cpu.runCycle();
    ckPassFail(cpu.status.z && cpu.status.n && cpu.status.o && !cpu.status.c && !cpu.status.i, "PLP failure, cannot continue");

    memory.write(6, 0x68);
    cpu.runCycle();
    ckPassFail(cpu.a == 0xF5, "PLA failure 2");

    memory.write(7, 0x68);
    cpu.runCycle();
    ckPassFail(cpu.a == 0xA4, "PLA failure 1");
    ckPassErr(cpu.sp == 0x56, "sp failure in stack");


    // Function stack tests
    cpu.clear();


    cpu.sp = 0x10;
    cpu.pc = 0x10;
    cpu.memory.write(0x10, 0x20);
    cpu.memory.write(0x11, 0xF5);
    cpu.memory.write(0x12, 0xC4);
    cpu.runCycle();
    ckPassFail(cpu.sp == 0x10 - 2 && cpu.pc == 0xC4F5, "JSR failure, cannot continue");

    cpu.memory[0xC4F5] = 0xA9;
    cpu.memory[0xC4F5 + 1] = 0xEA;
    cpu.runCycle();
    ckPassErr(cpu.a == 0xEA, "partial failure in jumped address");

    cpu.memory[0xC4F5 + 2] = 0x60;
    cpu.runCycle();
    ckPassFail(cpu.sp == 0x10 && cpu.pc == 0x13, "RST failure");

    cpu.memory.write(0x13, 0xA9);
    cpu.memory.write(0x14, 0x55);
    cpu.runCycle();
    ckPassErr(cpu.a == 0x55, "partial failure in returned address");

}
