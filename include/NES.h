#ifndef NES_HPP
#define NES_HPP

#include <memory>
#include <queue>

#include "Memory.h"
#include "Cpu6502.h"
#include "Ppu.h"

class NES {
public:
    using PixelT = std::tuple<uint8_t, uint8_t, Ppu::PaletteT>;
    // note that unique ptrs may have to be used later
    // Otherwise when the original location of the cpu's scope is destroyed so is the reference and creates a dangling reference
    // For now be safe with this class.
    NES();
    Cpu6502 cpu;
    Ppu ppu;
    std::shared_ptr<NES> thisPtr;

    void clear();
    void step();
    void powerUp(); // Creates the powerup state

   void addVideoData(const PixelT& pixel);
   void removeRequest();
   bool videoRequest() const;
   std::queue<PixelT> pixelsToAdd;
   bool videoRequested = false;
};

#endif // NES_HPP
