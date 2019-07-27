#ifndef NES_HPP
#define NES_HPP

#include "Memory.hpp"
#include "Cpu6502.hpp"
#include "Ppu.hpp"

class NES {
public:
    Cpu6502 cpu;
    Ppu ppu;
    NES();
};

#endif // NES_HPP
