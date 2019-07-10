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
    memory.write(0x10AF, 0xAA);
    dis.runCycle(state);
    passed = state.pc == 0x9 && state.a == 0xAA;
    if (!passed)
        BOOST_FAIL("ABS failure");
}
