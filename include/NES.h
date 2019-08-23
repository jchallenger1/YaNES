#ifndef NES_HPP
#define NES_HPP

#include <memory>

#include "Memory.h"
#include "Cpu6502.h"
#include "Ppu.h"

class NES : public std::enable_shared_from_this<NES> {
public:
    // x, y, chroma colour
    using PixelT = std::tuple<uint8_t, uint8_t, uint8_t>;
    void init(); // This function must be called right after the constructor
    Cpu6502 cpu;
    Ppu ppu;
    std::shared_ptr<NES> getPtr();

    void clear();
    void step();
    void powerUp(); // Creates the powerup state

   void addVideoData(const PixelT& pixel);
   void removeRequest();
   bool videoRequest() const;
   bool videoRequested = false;
};

#endif // NES_HPP
