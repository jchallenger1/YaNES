#ifndef NES_HPP
#define NES_HPP

#include <memory>

#include "Memory.h"
#include "Cpu6502.h"
#include "Ppu.h"

// This class acts as the main bus that connects everything
// It communicates with the cpu and ppu and allows interaction between the two

class NES : public std::enable_shared_from_this<NES> {
public:
    void init(); // This function must be called right after the constructor
    Cpu6502 cpu;
    Ppu ppu;
    std::shared_ptr<NES> getPtr();

    void clear();
    void step();
    void powerUp(); // Creates the powerup state

    // adds a chroma colour to the screen
    void addVideoData(const uint8_t& x, const uint8_t& y, const uint8_t& chroma);
    std::array<std::array<uint8_t, 256>, 240> screen{};
};

#endif // NES_HPP
