#ifndef PPU_HPP
#define PPU_HPP

#include "GamePak.hpp"
#include <array>

class Ppu {
    friend struct GamePak;
public:
    Ppu();

    std::array<uint16_t, 8> getTile(unsigned x, unsigned y);
    uint8_t readRegister(const uint16_t& adr);
    void writeRegister(const uint16_t& adr, const uint8_t& val);
private:
    // may or may not be needed, but added for now to easily know the scrolling that was set
    uint16_t scrollPos = 0; // (0-0xFF) -> x scroll, (0x100-0xFFFF) -> y scroll

    // Internal Registers
    int16_t scanline = -1;
    uint16_t vAdr = 0;
    uint16_t vTempAdr = 0;
    uint8_t fineXScroll = 0; // only three bits
    uint8_t writeToggle = 0; // 1 bit

    struct PPUCTRL {
        uint8_t nameTable : 4; // (NN) NameTable selector (0=0x2000, 1=0x2400, 2=0x2800, 3=0x2C00)
        uint8_t increment : 1; // (I) Vram increment per read/write of PPUData (0=1,1=32|going across vs down)
        uint8_t spriteTile : 1; // (S) Sprite Pattern Table Address for 8x8 sprites
        uint8_t bkgrdTile : 1; // (B) Background pattern table address (0=0x000, 1=0x1000)
        uint8_t spriteSz : 1; // (H) Sprite Size (0=8x8, 1=8x16)
        uint8_t masterSlave : 1; // (P) PPU master/slave select
        uint8_t NMI : 1; // (V) Generate NMi @ start of vblank (0=off,1=on)
        uint8_t asByte() const noexcept;
        void fromByte(const uint8_t&) noexcept;
    };
    struct PPUMASK {
        uint8_t greyScale : 1; // (G) Produce a greyscale display (0=normal, 1=greyscale)
        uint8_t bkgrdLeftEnable : 1; // (m) show background in left 8 pixels of screen
        uint8_t spriteLeftEnable : 1; // (M) show sprites in left 8 pixels of screen
        uint8_t bkgrdEnable : 1; // (b) show background
        uint8_t spriteEnable : 1; // (s) show sprites
        // Emphasize which colour bits (RGB)
        uint8_t red : 1;
        uint8_t green : 1;
        uint8_t blue : 1;
        uint8_t asByte() const noexcept;
        void fromByte(const uint8_t&) noexcept;
    };
    struct PPUSTATUS {
        uint8_t sOverflow : 1; // (O) Sprite Overflow
        uint8_t sprite0Hit : 1; // (S)
        uint8_t vblank : 1; // (V) Vertical Blank has Started
        uint8_t asByte() const noexcept;
        void fromByte(const uint8_t&) noexcept;
    };

    // 8 Registers that are exposed to the cpu
    // Descriptions here : https://wiki.nesdev.com/w/index.php/PPU_registers
    PPUCTRL PpuCtrl;
    PPUMASK PpuMask;
    PPUSTATUS PpuStatus;
    uint8_t OamAddr = 0;
    uint8_t OamData = 0;
    uint8_t PpuScroll = 0;
    uint8_t PpuAddr = 0;
    uint8_t PpuData = 0;
    uint8_t OamDma = 0;


    std::array<uint8_t, GamePak::KB16> memory{};
    // Oam is list of 64 sprites, each having info of 4 bytes
    // Description of each byte : https://wiki.nesdev.com/w/index.php/PPU_OAM
    std::array<uint8_t, 0xFF> OAM{};
};

#endif // PPU_HPP
