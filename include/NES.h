#ifndef NES_HPP
#define NES_HPP

#include <memory>

#include "Memory.h"
#include "Cpu6502.h"
#include "Ppu.h"

class NES : public std::enable_shared_from_this<NES> {
public:
    // x, y, chroma colour
    void init(); // This function must be called right after the constructor
    Cpu6502 cpu;
    Ppu ppu;
    std::shared_ptr<NES> getPtr();

    void clear();
    void step();
    void powerUp(); // Creates the powerup state

   void addVideoData(const uint8_t& x, const uint8_t& y, const uint8_t& chroma);
   std::array<std::array<uint8_t, 256>, 240> screen{};
};

#endif // NES_HPP
