#ifndef NES_HPP
#define NES_HPP

#include <memory>

#include "Memory.hpp"
#include "Cpu6502.hpp"
#include "Ppu.hpp"

class NES {
public:
    // note that unique ptrs may have to be used later
    // Otherwise when the original location of the cpu's scope is destroyed so is the reference and creates a dangling reference
    // For now be safe with this class.
    NES(Cpu6502& cpu, Ppu& ppu);
    Cpu6502& cpu;
    Ppu& ppu;

    void clear();
};

#endif // NES_HPP
