#define BOOST_TEST_MODULE OpcodeTests
#include <boost/test/included/unit_test.hpp>

#include "State6502.hpp"
#include "Disassembler6502.hpp"

// Test if all addressing modes work also check lda function
BOOST_AUTO_TEST_CASE( LDA_ADDRESSING_TESTS ) {

    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;
    memory.write(0, 0xA9); // check immediate
    memory.write(1, 0xFA);
    dis.runCycle(state);
    bool passed = state.pc == 0x2 && state.a == 0xFA;
    if (!passed)
        BOOST_FAIL("Immediate failure");


    memory.write(2, 0xA5); // check zeropage
    memory.write(3, 0x00);
    dis.runCycle(state);
    passed = state.pc == 0x4 && state.a == 0xA9;
    if (!passed)
        BOOST_FAIL("Zero page failure");
    memory.write(4, 0xB5); // check zero page X
    memory.write(5, 0x80);
    memory.write(0x8F, 0xBB);
    state.x = 0xF;
    dis.runCycle(state);
    passed = state.pc == 0x6 && state.a == 0xBB;
    if (!passed)
        BOOST_FAIL("Zero page X failure");
    memory.write(6, 0xAD); // check ABS
    memory.write(7, 0x10);
    memory.write(8, 0xAF);
    memory.write(0xAF10, 0xAA);
    dis.runCycle(state);
    passed = state.pc == 0x9 && state.a == 0xAA;
    if (!passed)
        BOOST_FAIL("ABS failure");
}

// These tests are from the examples databook
// Tests are important for these instructions as they are the hardest in the entire instruction set
BOOST_AUTO_TEST_CASE( ADC_SBC ) {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;

    // Regular Add test
    memory.write(0, 0x69);
    memory.write(1, 211);
    state.a = 13;
    state.status.c = 1;
    dis.runCycle(state);
    bool passed = state.a == 225 && state.status.c == 0;
    if (!passed)
        BOOST_ERROR("2.1 ADC failure");
    state.clear();

    // Add with carry
    memory.write(0, 0x69);
    memory.write(1, 6);
    state.a = 254;
    state.status.c = 1;
    dis.runCycle(state);
    passed = state.a == 5 && state.status.c == 1;
    if (!passed)
        BOOST_ERROR("2.2 ADC failure");
    state.clear();

    // Add two pos with overflow
    memory.write(0, 0x69);
    memory.write(1, 2);
    state.a = 127;
    state.status.c = 0;
    dis.runCycle(state);
    passed = state.a == 129 && state.status.o == 1; // 129 is really -127
    if (!passed)
        BOOST_ERROR("2.7 ADC failure");
    state.clear();

    // Add two in decimal mode
    memory.write(0, 0x69);
    memory.write(1, 20); // 20 in BCD is 14
    state.status.c = 0;
    state.status.d = 1;
    state.a = 121; // 79 in BCD
    dis.runCycle(state);
    passed = state.a == 147; // 93 in BCD (79+14=93)
    if (!passed)
        BOOST_ERROR("2.12 Decimal ADC failure");
    state.clear();

    // Sub 2 num w/ borrow w/ positive
    memory.write(0, 0xE9);
    memory.write(1, 3);
    state.a = 5;
    dis.runCycle(state);
    passed = state.a == 2 && state.status.c == 1;
    if (!passed)
        BOOST_ERROR("2.13 SBC failure");
}

BOOST_AUTO_TEST_CASE( AND_OR_XOR ) {

    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;
    memory.write(0, 0x29);
    memory.write(1, 0b11001111);
    state.a = 0b11110111;
    dis.runCycle(state);
    bool passed = state.a == 0b11000111;
    if (!passed)
        BOOST_ERROR("AND 2.19 failure");
    memory.write(2, 0x09);
    memory.write(3, 0b11101111);
    state.a = 0x8;
    dis.runCycle(state);
    passed = state.a == 0b11101111;
    if (!passed)
        BOOST_ERROR("OR 2.20 failure");
    memory.write(4, 0x49);
    memory.write(5, 0b10101111);
    state.a = 0xFF;
    dis.runCycle(state);
    passed = state.a == 0b01010000;
    if (!passed)
        BOOST_ERROR("XOR 2.21 failure");
}

BOOST_AUTO_TEST_CASE( PROCESSOR_STATUS ) {

    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;

    memory.write(0, 0x38);
    dis.runCycle(state);
    bool passed = state.status.c == 1;
    if (!passed)
        BOOST_ERROR("SEC failure");
    memory.write(1, 0x18);
    dis.runCycle(state);
    passed = state.status.c == 0;
    if (!passed)
        BOOST_ERROR("CLC failure");


    memory.write(2, 0x78);
    dis.runCycle(state);
    passed = state.status.i == 1;
    if (!passed)
        BOOST_ERROR("SEI failure");
    memory.write(3, 0x58);
    dis.runCycle(state);
    passed = state.status.i == 0;
    if (!passed)
        BOOST_ERROR("CLI failure");

    memory.write(4, 0xF8);
    dis.runCycle(state);
    passed = state.status.d == 1;
    if (!passed)
        BOOST_ERROR("SED failure");
    memory.write(5, 0xD8);
    dis.runCycle(state);
    passed = state.status.d == 0;
    if (!passed)
        BOOST_ERROR("CLD failure");

    state.status.o = 1;
    memory.write(6, 0xB8);
    dis.runCycle(state);
    passed = state.status.o == 0;
    if (!passed)
        BOOST_ERROR("CLV failure");

}


BOOST_AUTO_TEST_CASE (JMP_BRANCH ) {
    State6502 state;
    Disassembler6502 dis;
    auto& memory = state.memory;
    memory.write(0, 0x4C);
    memory.write(1, 0x56);
    memory.write(2, 0xC9);
    dis.runCycle(state);
    bool passed = state.pc == 0xC956;
    if (!passed)
        BOOST_ERROR("ABS JMP failure");
    state.clear();

    memory.write(0xFA23, 0x65);
    memory.write(0xFA23 + 1, 0xB3);
    memory.write(0, 0x6C);
    memory.write(1, 0x23);
    memory.write(2, 0xFA);
    dis.runCycle(state);
    passed = state.pc == 0xB365;
    if (!passed)
        BOOST_ERROR("INDR JMP failure");
    state.clear();

    memory.write(0x3000, 0x40);
    memory.write(0x30FF, 0x80);
    memory.write(0x3100, 0x50);
    memory.write(0, 0x6C);
    memory.write(1, 0xFF);
    memory.write(2, 0x30);
    dis.runCycle(state);
    passed = state.pc == 0x4080;
    if (!passed)
        BOOST_ERROR("INDR JMP page boundary failure");

    state.clear();
    state.pc = 10;
    memory.write(10, 0x10);
    memory.write(11, 0x56);
    dis.runCycle(state);
    passed = state.pc == 10 + 0x56;
    if (!passed)
        BOOST_FAIL("Branch/Relative mode failure, can't test next one");
    memory.write(10 + 0x56, 0x70);
    memory.write(10 + 0x56 + 1, 0xFF);
    dis.runCycle(state);
    passed = state.pc == 10 + 0x56 + 2;
    if (!passed)
        BOOST_ERROR("branch/relative mode no branching failure");

}
