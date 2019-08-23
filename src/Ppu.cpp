#include "Ppu.h"
#include "NES.h"
#include <bitset>
#include <iostream>
#include <memory>
#include <cmath>

#define UNUSED(x) (void)(x)

#define mT(...) std::make_tuple<uint8_t, uint8_t, uint8_t>(__VA_ARGS__) // Quick make tuple without the large syntax of uint8_t's...

// Using 2C02 Palette Configuration : https://wiki.nesdev.com/w/index.php/PPU_palettes#2C02
const std::array<const Ppu::PaletteT, 0x40 > Ppu::RGBPaletteTable = {
    //     X0                X1              X2               X3               X4              X5               X6                X7
/*0X:*/mT(84,84,84),    mT(0,30,116),    mT(8,16,144),    mT(48,0,136),    mT(68,0,100),    mT(92,0,48),      mT(84,4,0),      mT(60,24,0),
    //     X8               X9               XA               XB               XC              XD               XE                XF
       mT(32,42,0),     mT(8,58,0),      mT(0,64,0),      mT(0,60,0),      mT(0,50,60),     mT(0,0,0),        mT(0,0,0),       mT(0,0,0),
/*1X:*/
       mT(152,150,152), mT(8,76,197),    mT(48,50,236),   mT(92,30,228),   mT(136,20,176),  mT(160,20,100),   mT(152,34,32),   mT(120,60,0),
       mT(84, 90, 0),   mT(40,114,0),    mT(8,124,0),     mT(0,118,40),    mT(0,102,120),   mT(0,0,0),        mT(0,0,0),       mT(0,0,0),
/*2X:*/
       mT(236,238,236), mT(76,154,236),  mT(120,124,236), mT(176,98,236),  mT(228,84,236),  mT(236, 88, 180), mT(236,106,100), mT(212,136,32),
       mT(160,170,0),   mT(116,196,0),   mT(76,208,32),   mT(56,204,108),  mT(57,180,204),  mT(60,60,60),     mT(0,0,0),       mT(0,0,0),
/*3X:*/
       mT(236,238,236), mT(168,204,236), mT(188,188,236), mT(212,178,236), mT(236,174,236), mT(236,174,212),  mT(236,180,176), mT(228,196,144),
       mT(204,210,120), mT(180,222,120), mT(168,226,144), mT(152,226,180), mT(160,214,228), mT(160,162,160),  mT(0,0,0),       mT(0,0,0)
};

#undef mT

Ppu::Ppu() {
    clear();
}

Ppu::Ppu(std::shared_ptr<NES> nes) {
    setNESHandle(nes);
    clear();
}

void Ppu::setNESHandle(std::shared_ptr<NES> nes) & {
    this->nes = nes;
}

constexpr inline bool inRange(const uint16_t& min, const uint16_t& max, const uint16_t& val) {
    return val <= max && val >= min;
}
    // Create a line of a tile
    // For every bit position of left and right, set it to the two bits equivalent position
    // in a 16 bit field, ex if left -> 0 , right -> 1, then bitPos(line) = 10 in the u16 type
    // But note that setting left to right immeditely to 0 and 1 reverses the line, instead set to bits 16 and 15
uint16_t Ppu::createLine(const uint8_t &left, const uint8_t &right) {
    uint16_t line = 0;
    for (short bitPos = 0, topBitLoc = 0; bitPos != 8; bitPos++, topBitLoc+=2) {
        const uint16_t pow2 = static_cast<uint16_t>(std::pow(2, bitPos)); // select which bit to choose
        // Take the bit and move to 1's position, then move it to the line's bits bottom down
        line |= ( (static_cast<uint16_t>(right) & pow2) >> bitPos) << (15 - topBitLoc);
        line |= ( (static_cast<uint16_t>(left) & pow2) >> bitPos) << (15 - topBitLoc - 1);
    }
    return line;
}

// Get both bit planes starting at tileAddress and make into patterntableT
// This structure is traversable via example of stdDrawPatternTile
Ppu::PatternTableT Ppu::getPatternTile(const uint16_t& tileAddress) const {
    if (tileAddress >= 0x2000 - 0xF) throw std::runtime_error("Given tile address it not a pattern table address");

    PatternTableT tile{};

    for (unsigned i = 0; i != 8; i++) {
        tile[i % 8] = createLine(nes->ppu.memory[tileAddress + i], nes->ppu.memory[tileAddress + i + 8]);
    }
    return tile;
}

Ppu::PatternTableT Ppu::getPatternTile(const uint8_t& tileID, bool isLeft) const {
    if (isLeft)
        return getPatternTile(tileID * 16);
    return getPatternTile(0x1000 + tileID * 16);
}

void Ppu::stdDrawPatternTile(const uint16_t& tileAddress) const {
    PatternTableT tile = getPatternTile(tileAddress);
    for (uint8_t y = 0; y != 8; y++) {
        uint16_t line = tile[y];
        for (uint8_t x = 0; x != 8; x++) {
            uint8_t pixel = ( line >> (x * 2) ) & 0b11;
            std::cout << static_cast<int>(pixel);
        }
        std::cout << "\n";
    }
    std::cout << std::endl;

}

Ppu::PaletteT Ppu::getRGBPalette(const uint8_t &paletteNum) {
    if (paletteNum > 0x40) {
        std::cerr << " In " << __FILE__ << " Palette Number is out of range of the table";
        throw std::out_of_range("Palette Number is out of range");
    }
    return RGBPaletteTable[paletteNum];
}

// https://wiki.nesdev.com/w/index.php/PPU_attribute_tables
// Get the Attribute Table Address from a nametable address
// NameTableAdr must NOT be the actual address, but the relative one to the name table source
// Ex if nametable Address = 0x2005, then relative address is 0x5, Similarly 0x3325 -> 0x125
// atrTableStart must be the origin location attribute table to the particular nametable
// Ex if nameTable Adr = 0x2000, then atrTableStart = 0x23C0
uint16_t Ppu::getAtrAddress(const uint16_t& nameTableRelativeAdr, const uint16_t& atrTableStart) const {
    // TODO : Make it such that relative addresses and the attribute table starts are unneeded for the function
    // Do when donkey kong works
    uint8_t hTableLevel = static_cast<uint8_t>( (nameTableRelativeAdr % 32) / 4); // Get horizontal table location, there is 32 bytes in a partial vertical line, and 4 numbers routes to one (0,1,2,3->0)
    uint8_t vTableLevel = static_cast<uint8_t>( nameTableRelativeAdr / 128); // Get vertical, The next vertical tile occurs every 64 bytes
    uint16_t adr = atrTableStart + hTableLevel + vTableLevel * 8;
    return adr;
}

// Function gets the shift from a attribute table needed for the correct palette selection
uint8_t Ppu::getShift(const uint16_t& nameTableRelativeAdr) const {
    // Compress all vertical tiles into one vertical tile (%64), if the address is divisable by 32 then it's in the upper half of tile, otherwise in lower half
    if (static_cast<int>( (nameTableRelativeAdr % 64) / 32) == 0) {
        switch(nameTableRelativeAdr % 4){ // Top Half
            case 0: case 1:
                return 0;
            case 2: case 3:
                return 2;
        }
    }
    else {
        switch(nameTableRelativeAdr % 4) { // Lower half
            case 0: case 1:
                return 4;
            case 2: case 3:
                return 6;
        }
    }
    std::cerr << "In " << __FUNCTION__ << " in " << __FILE__ << " Reached end of function without a shift, Relative Address : " << nameTableRelativeAdr << "\n";
    throw std::runtime_error("Could not get a shift from name table address");
}

// https://wiki.nesdev.com/w/index.php/PPU_attribute_tables
// Gets Palette selection from a nametable address
uint8_t Ppu::getPaletteFromNameTable(const uint16_t& nameTableRelativeAdr, const uint16_t& atrTableStart) const {
    uint8_t byte = vRamRead( getAtrAddress(nameTableRelativeAdr, atrTableStart) );

    switch(getShift(nameTableRelativeAdr)) {
        case 0:
            return byte & 0x3;
        case 2:
            return (byte & 0xC) >> 2;
        case 4:
            return (byte & 0x30) >> 4;
        case 6:
            return (byte & 0xC0) >> 6;
    }
    std::cerr << "In " << __FUNCTION__ << " in " << __FILE__ << " Reached end of function without a palett, Relative Address : " << nameTableRelativeAdr << "\n";
    throw std::runtime_error("Could not get a shift from name table address");
}


Ppu::ColorSetT Ppu::getColorSetFromAdr(const uint16_t& paletteAdr) const {
    // Assuming always background palette for now
    if (!inRange(0x3F00, 0x3F1F, paletteAdr)) {
        std::cerr << "Palette Address is not a background or sprite palette address\n";
        throw std::runtime_error("Palette Address is invalid");
    }
    else if (paletteAdr == 0x3F00) { // universal only
        uint8_t u = vRamRead(0x3F00);
        return std::make_tuple(u, u, u, u);
    }

    ColorSetT set;
    std::get<0>(set) = vRamRead(0x3F00); // Universal
    // Next three palette colours
    std::get<1>(set) = vRamRead(paletteAdr);
    std::get<2>(set) = vRamRead(paletteAdr + 1);
    std::get<3>(set) = vRamRead(paletteAdr + 2);
    return set;
}





void Ppu::vRamWrite(const uint16_t& adr, const uint8_t& val) {
    if (inRange(0x3000, 0x3EFF, adr))
        memory[0x2000 + adr % 1000] = val;
    else if (inRange(0x3F20, 0x3FFF, adr))
        memory[0x3F00 + adr % 0x20] = val;
    else
        memory[adr] = val;
}

uint8_t Ppu::vRamRead(const uint16_t& adr) const {
    if (inRange(0x3000, 0x3EFF, adr))
        return memory[0x2000 + adr % 3000];
    else if (inRange(0x3F20, 0x3FFF, adr))
        return memory[0x3F00 + adr % 0x20];
    else
        return memory[adr];
}

// https://wiki.nesdev.com/w/index.php/PPU_registers
// https://wiki.nesdev.com/w/index.php/PPU_scrolling
uint8_t Ppu::readRegister(const uint16_t& adr) {
    switch(adr) {
        case 0x2002: { // Status < read
            uint8_t stat = PpuStatus.asByte();
            PpuStatus.sOverflow = 0;
            writeToggle = 0;
            /* TODO :
             * Set at dot 1 of line 241 (the line *after* the post-render
             * line); cleared after reading $2002 and at dot 1 of the
             * pre-render line.
             */
            return stat;
        }
        case 0x2004: // OAM data <> Read/Write
            return OAM[OamAddr];
        case 0x2007: // Data <> Read/Write
            return vRamRead(vAdr);
        default:
            std::cerr << "Error Read to address " << std::hex << "0x" << adr << " was detected\n";
            throw std::runtime_error("Attempted read to non PPU register or to a writeonly register of (dec) " + std::to_string(adr));
    }
}

// Same sources as readRegister
void Ppu::writeRegister(const uint16_t& adr, const uint8_t& val) {
    switch(adr) {
        case 0x2000: { // Controller > Write
            PpuCtrl.fromByte(val);
            // t: ...xx.. ........ = d: ......xx
            uint8_t bits2 = val & 0b11;
            vTempAdr &= ~0xC00; // clear the spot where these bits will go
            vTempAdr |= static_cast<uint16_t>(bits2) << 10; // move the bits into bit 10, 11
            break;
        }
        case 0x2001: // Mask > Write
            PpuMask.fromByte(val);
            break;
        case 0x2003: // OAM address > Write
            OamAddr = val;
            break;
        case 0x2004: // OAM data <> Read/Write
            OAM[OamAddr++] = val;
            break;
        case 0x2005: // Scroll >> write x2
            if (writeToggle == 0) { // first write is X
                vTempAdr &= ~0x1F; // clear and set the first 5 bits of tempVram to the byte
                vTempAdr |= val >> 3;
                fineXScroll = val & 0b111;
                writeToggle = 1;
                scrollPos = (~0x00FF & scrollPos) | val;
            }
            else { // second write is Y
                vTempAdr &= ~0x73E0; // fill in order of CBA..HG FED...., d=HGFEDCBA
                vTempAdr |= static_cast<uint16_t>(val & 0b111) << 12; // fil CBA
                vTempAdr |= static_cast<uint16_t>(val & 0xC0) << 2; // fill HG
                vTempAdr |= static_cast<uint16_t>(val & 0x38) << 2; // fill FED
                writeToggle = 0;
                scrollPos = static_cast<uint16_t>( (~0xFF00 & scrollPos) | static_cast<uint16_t>(val) << 8 );
            }
            break;
        case 0x2006: // Address >> write x2
            if (writeToggle == 0) { // load high byte of address
                vTempAdr = static_cast<uint16_t>( (vTempAdr & ~0x3F00) | (static_cast<uint16_t>(val & 0x3F) << 8) );
                writeToggle = 1;
            }
            else {
                vTempAdr = (vTempAdr & ~0xFF) | val;
                vAdr = vTempAdr;
                writeToggle = 0;
            }
            break;
        case 0x2007: // Data <> Read/Write
            // Note that the scrolling details are not implemented
            vRamWrite(vAdr, val);
            vAdr += PpuCtrl.increment == 0 ? 1 : 32;
            break;
        case 0x4014: {// OAM DMA > Write
                // Read/Write from cpu's XX00-XXFF, XX=val, to OAM
                uint16_t start = static_cast<uint16_t>(static_cast<uint16_t>(val) << 8),
                        end = start | 0xFF;
                while (start != end) {
                    OAM[OamAddr++] = nes->cpu.memory.read(start++);
                }
                break;
            }
        default:
            std::cerr << "Error Write to address " << std::hex << "0x" << adr << " was detected\n";
            throw std::runtime_error("Attempted write to non PPU register or to a readonly register of (dec) " + std::to_string(adr));
    }
}

// Both these functions address finding comes from here:
// https://wiki.nesdev.com/w/index.php/PPU_scrolling#Tile_and_attribute_fetching

void Ppu::fetchNameTableByte() {
    uint16_t tileAddress = 0x2000 | (vAdr & 0x0FFF);
    nameTableLatch = vRamRead(tileAddress);
}

void Ppu::fetchAttrTableByte() {
    uint16_t attrAddress = 0x23C0 | (vAdr & 0x0C00) | ((vAdr >> 4) & 0x38) | ((vAdr >> 2) & 0x07);
    uint8_t shift = ( (vAdr >> 4) & 4) | (vAdr & 2);
    attrTableLatch = vRamRead(attrAddress) >> shift;
}

// Refer to https://wiki.nesdev.com/w/index.php/PPU_pattern_tables on left/right bit planes

void Ppu::fetchTableLowByte() {
    // TODO : Scroll Y from first 3 bits in vAdr to use in scrolling
    uint16_t address = PpuCtrl.bkgrdTile * 0x1000 + // Which pattern table to use
            + nameTableLatch * 16; // which specific 8x8 CHR to use in the table, * 16 because both left and right tables are 8 bytes, 16 bytes to skip to next one.
    patternTableLowLatch = vRamRead(address);
}
void Ppu::fetchTableHighByte() {
    uint16_t address = PpuCtrl.bkgrdTile * 0x1000 + nameTableLatch * 16;
    patternTableHighLatch = vRamRead(address + 8); // high pattern table is 8 bytes after the low
}

// https://forums.nesdev.com/viewtopic.php?t=10348
uint8_t Ppu::bGPixel() {
    // Recall that fineX determines which bit to use
    uint8_t a = attrShiftHigh >> (7 - fineXScroll) & 0x1 ; // high palette bit
    uint8_t b = attrShiftLow >> (7 - fineXScroll) & 0x1; // low palette bit
    uint8_t c = (bkShiftHigh >> (15 - fineXScroll) ) & 0x1; // bg high bit
    uint8_t d = (bkShiftLow >> (15 - fineXScroll) ) & 0x1; // bg low bit
    return static_cast<uint8_t>(  (a << 3) | (b << 2) | (c << 1) | d  );
}

void Ppu::clear() {
    PpuCtrl.clear();
    PpuMask.clear();
    PpuStatus.clear();
    std::fill(memory.begin(), memory.end(), 0);
    std::fill(OAM.begin(), OAM.end(), 0);
    OamAddr = scrollPos = 0;
    scanline = vAdr = vTempAdr = fineXScroll = writeToggle = 0;
}

// Vblanking Functions
// TODO : When vblank is set and cleared, there is a delay in timing.

// Sets VBlank
void Ppu::setVBlank() {
    PpuStatus.vblank = 1;
    nes->cpu.signalNMI();
}

// Clear vblank, 0 sprite and overflow flags
void Ppu::clearVBlank() {
    PpuStatus.clear();
}

void Ppu::renderPixel() {
    uint16_t x = cycle - 1;
    uint16_t y = scanline;

    uint8_t backgroundPix = bGPixel();
    PaletteT palette = getRGBPalette( (backgroundPix & 0xC) >> 2 );
    nes->addVideoData(std::make_tuple(x, y, palette));
    nes->videoRequested = true;
}

void Ppu::runCycle() {
    std::cerr << nes.use_count();
    static uint8_t atrLatchLow = 0;
    static uint8_t atrLatchHigh = 0;

    if (inRange(1, 256, cycle)) { // Data for the current scanline, note that the first 2 tiles are already filled
        renderPixel();
        bkShiftLow <<= 1;
        bkShiftHigh <<= 1;
        attrShiftLow <<= 1;
        attrShiftHigh <<= 1;
        attrShiftLow |= atrLatchLow;
        attrShiftHigh |= atrLatchHigh << 1;
        switch (cycle % 8) {
            case 1:
                fetchNameTableByte();
                // Get new data into shift registers, occurs every 8 cycles
                bkShiftLow |= patternTableLowLatch;
                bkShiftHigh |= patternTableHighLatch;

                atrLatchLow = attrTableLatch & 1;
                atrLatchHigh = (attrShiftHigh >> 1) & 1;
                break;
            case 3:
                fetchAttrTableByte();
                break;
            case 5:
                fetchTableLowByte();
                break;
            case 7:
                fetchTableHighByte();
                break;
            case 0:
                break;
        }
    }

    // Vblanking and cycling logic
    if (scanline == 241 && cycle == 1) {
        setVBlank();
    }
    if (scanline == 261 && cycle == 1) {
        clearVBlank();
    }

    if (scanline == 261) scanline = 0;
    if (cycle == 340) {
        cycle = 0;
        ++scanline;
    }
    ++cycle;

}









uint8_t Ppu::PPUCTRL::asByte() const noexcept {
    uint8_t byte = 0;
    byte |= nameTable;
    byte |= increment << 2;
    byte |= spriteTile << 3;
    byte |= bkgrdTile << 4;
    byte |= spriteSz << 5;
    byte |= masterSlave << 6;
    byte |= NMI << 7;
    return byte;
}

void Ppu::PPUCTRL::fromByte(const uint8_t& byte) noexcept {
    nameTable = byte & 0b11;
    increment = (byte & 0b100) >> 2;
    spriteTile = (byte & 0x8) >> 3;
    bkgrdTile = (byte & 0x10) >> 4;
    spriteSz = (byte & 0x20) >> 5;
    masterSlave = (byte & 0x40) >> 6;
    NMI = (byte & 0x80) >> 7;
}

void Ppu::PPUCTRL::clear() noexcept {
    nameTable = increment = spriteTile = bkgrdTile = spriteSz = masterSlave = NMI = 0;
}

uint8_t Ppu::PPUMASK::asByte() const noexcept {
    uint8_t byte = 0;
    byte |= greyScale;
    byte |= bkgrdLeftEnable << 1;
    byte |= spriteLeftEnable << 2;
    byte |= bkgrdEnable << 3;
    byte |= spriteEnable << 4;
    byte |= red << 5;
    byte |= green << 6;
    byte |= blue << 7;
    return byte;
}

void Ppu::PPUMASK::fromByte(const uint8_t& byte) noexcept {
    greyScale = byte & 1;
    bkgrdLeftEnable = (byte & 2) >> 1;
    spriteLeftEnable = (byte & 0b100) >> 2;
    bkgrdEnable = (byte & 0x8) >> 3;
    spriteEnable = (byte & 0x10) >> 4;
    red = (byte & 0x20) >> 5;
    green = (byte & 0x40) >> 6;
    blue = (byte & 0x80) >> 7;
}

void Ppu::PPUMASK::clear() noexcept {
    greyScale = bkgrdLeftEnable = spriteLeftEnable = bkgrdEnable = spriteEnable = red = green = blue = 0;
}

uint8_t Ppu::PPUSTATUS::asByte() const noexcept {
    uint8_t byte = 0;
    byte |= sOverflow << 5;
    byte |= sprite0Hit << 6;
    byte |= vblank << 7;
    return byte;
}

void Ppu::PPUSTATUS::fromByte(const uint8_t& byte) noexcept {
    sOverflow = (byte & 0x20) >> 5;
    sprite0Hit = (byte & 0x40) >> 6;
    vblank = (byte & 0x80) >> 7;
}

void Ppu::PPUSTATUS::clear() noexcept {
    sOverflow = sprite0Hit = vblank = 0;
}
