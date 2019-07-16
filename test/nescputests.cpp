#include "tests.hpp"
#include "Disassembler6502.hpp"
#include "State6502.hpp"
#include "Memory.hpp"

#include <fstream>
#include <numeric>
#include <iostream>




void nesCpuTest() {
    Memory memory;
    memory.fromFile("../rsc/tests/nestest.nes");

}
