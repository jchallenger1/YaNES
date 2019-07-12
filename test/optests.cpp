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
