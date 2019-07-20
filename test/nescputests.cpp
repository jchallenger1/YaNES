#include "tests.hpp"
#include "Disassembler6502.hpp"
#include "State6502.hpp"
#include "Memory.hpp"

#include <fstream>
#include <numeric>
#include <iostream>
#include <sstream>
#include <tuple>
// 0->PC, 1->A, 2->X, 3->Y, 4->P, 5->SP, 6->Instruction Description
using TupleState = std::tuple<uint16_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, std::string>;

TupleState getTestState(const std::string& line) {
    std::istringstream strStream(line);
    std::string word;
    TupleState tState;
    strStream >> word; // first is always the pc
    std::get<0>(tState) = static_cast<uint16_t>(std::stoul(word.c_str(), nullptr, 16));
    word.clear();

    std::string instrDesc;
    // Instruction Description varies each line
    // Keep adding to a str until a register appears
    // Note that the instruction returned in the error is the NEXT instruction that would be preformed
    // Meaning the real error is the one before it.
    while (strStream >> word) {
        if (std::find(word.cbegin(), word.cend(), ':') != word.cend()) {// now a register
            word.erase(0,2);
            std::get<1>(tState) = static_cast<uint8_t>(std::stoul(word.c_str(), nullptr, 16));
            break;
        }
        else {
            instrDesc += " " + word;
        }
    }

    // For now do not use a loop
    // Later use varadic templates w/Integer Sequence to do this in a loop
    strStream >> word;
    word.erase(0, 2);
    std::get<2>(tState) = static_cast<uint8_t>(std::stoul(word.c_str(), nullptr, 16)); // x
    strStream >> word;
    word.erase(0, 2);
    std::get<3>(tState) = static_cast<uint8_t>(std::stoul(word.c_str(), nullptr, 16)); // y
    strStream >> word;
    word.erase(0, 2);
    std::get<4>(tState) = static_cast<uint8_t>(std::stoul(word.c_str(), nullptr, 16)); // p
    strStream >> word;
    word.erase(0, 3);
    std::get<5>(tState) = static_cast<uint8_t>(std::stoul(word.c_str(), nullptr, 16)); // sp
    std::get<6>(tState) = instrDesc;

    return tState;
}


void nesCpuTest() {
    Memory memory;
    memory.fromFile("../rsc/tests/nestest.nes");

    std::ifstream ifsLog("../rsc/tests/nestest.log", std::ios_base::in);
    ckPassFail(ifsLog.good(), "Could not open log file to compare testsing");

    Disassembler6502 dis;
    State6502 state;
    state.memory = memory;
    state.pc = 0xC000;
    state.sp = 0xFD;
    state.a = state.x = state.y = 0;
    state.status.reset();

    std::string cycleResults;
    int i = 0;

    while( std::getline(ifsLog, cycleResults) ) {
        if (i == 8) {
            std::cout << "";
        }
        std::string instrDesc;
        uint8_t a, x, y, p, sp;
        uint16_t pc;
        std::tie(pc, a, x, y, p, sp, instrDesc) = getTestState(cycleResults);
        ckPassFail(state.a == a, "(" + std::to_string(i) + ") Accumulator Register failure detected at " + instrDesc);
        ckPassFail(state.x == x || state.y == y, "(" + std::to_string(i) + ") X,Y Register failure detected at " + instrDesc);
        ckPassFail(state.status.asByte() == p, "(" + std::to_string(i) + ") Status failure detected at " + instrDesc);
        ckPassFail(state.sp == sp, "(" + std::to_string(i) +  ") Stack pointer failure detected at " + instrDesc);
        ckPassFail(state.pc == pc, "(" + std::to_string(i) + ") Program Counter failure detected at " + instrDesc);

        if (state.memory.read(0x02) != 0 || state.memory.read(0x03) != 0) {
            ckPassErr(false, "Cpu has triggered an error at " + instrDesc);
        }
        dis.runCycle(state);
        ++i;
    }

}
