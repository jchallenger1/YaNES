#include "Ppu.h"
#include "NES.h"
#include <bitset>
#include <iostream>
#include <memory>
#include <cmath>
#include "functions.hpp" // apply_from_tuple inRange

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


// Refer to https://wiki.nesdev.com/w/index.php/PPU_pattern_tables on left/right bit planes
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
    // each bit plane is +8 bytes from the first left bitplane
    for (unsigned i = 0; i != 8; i++) {
        tile[i % 8] = createLine(nes->ppu.memory[tileAddress + i], nes->ppu.memory[tileAddress + i + 8]);
    }
    return tile;
}

Ppu::PatternTableT Ppu::getPatternTile(const uint8_t& tileID, bool isLeft) const {
    // move to the address of the pattern tile
    if (isLeft)
        return getPatternTile(tileID * 16);
    return getPatternTile(0x1000 + tileID * 16);
}

void Ppu::stdDrawPatternTile(const uint16_t& tileAddress) const {
    PatternTableT tile = getPatternTile(tileAddress);
    // Traverse in a 8x8 grid
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
        //return std::make_tuple(vRamRead(0x3F00), vRamRead(0x3F01), vRamRead(0x3F02), vRamRead(0x3F03));

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

// Gets a chroma colour from a palette given the id and the pixel
// id and pixel must be in the range (0-3) 2 bits.
uint8_t Ppu::getChromaFromPaletteRam(const uint8_t& paletteID, const uint8_t& pixel) const {
    uint16_t paletteStart = 0x3F00 + paletteID * 4; // each palette is 4 bytes long
    paletteStart += pixel; // index into the palette to get the chroma
    return vRamRead(paletteStart);
}


void Ppu::vRamWrite(const uint16_t& adr, const uint8_t& val) {
    if (inRange(0x2000, 0x3EFF, adr)) {
        // This is hard coded for now, for NROM, the mapper on flag 6
        // indicates if its scrolling horizontally or vertically.
        // Since right now we are only worried about donkey koney, it always has horizontal mirroring
        // Horizontal mirroring : nametables 1, 3 route to instead 0 and 2

        // Note that 0x3000 - 0x3EFF are mirror of 0x2000-0x2EFF, so ignore the most sig 8 bits
        uint16_t cutAdr = adr & 0xFFF;
        if (inRange(0x400, 0x7FF, cutAdr)) { // nametable 1
            memory[0x2000 + cutAdr % 0x400] = val;
        }
        else if (inRange(0xC00, 0xFFF, cutAdr)) {// nametable 3
            memory[0x2800 + cutAdr % 0x400] = val;
        }
        else
            memory[0x2000 + cutAdr] = val;
    }
    else if (inRange(0x3F20, 0x3FFF, adr))
        memory[0x3F00 + adr % 0x20] = val;
    else
        memory[adr] = val;
}

uint8_t Ppu::vRamRead(const uint16_t& adr) const {
    if (inRange(0x3F20, 0x3FFF, adr))
        return memory[0x3F00 + adr % 0x20];
    else if (inRange(0x2000, 0x3EFF, adr)) {
        // only horizontal mirroring
        uint16_t cutAdr = adr & 0xFFF;
        if (inRange(0x400, 0x7FF, cutAdr)) { // nametable 1
            return memory[0x2000 + cutAdr % 0x400];
        }
        else if (inRange(0xC00, 0xFFF, cutAdr)) {// nametable 3
            return memory[0x2800 + cutAdr % 0x400];
        }
        else
            return memory[0x2000 + cutAdr];
    }
    else
        return memory[adr];
}

// https://wiki.nesdev.com/w/index.php/PPU_registers
// https://wiki.nesdev.com/w/index.php/PPU_scrolling
uint8_t Ppu::readRegister(const uint16_t& adr) {
    switch(adr) {
        case 0x2002: { // Status < read
            uint8_t stat = PpuStatus;
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
        case 0x2007: { // Data <> Read/Write
            uint8_t byte = vRamRead(vAdr);
            vAdr += PpuCtrl.increment == 0 ? 1 : 32;
            return byte;
        }
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
            }
            else { // second write is Y
                vTempAdr &= ~0x73E0; // fill in order of CBA..HG FED...., d=HGFEDCBA
                vTempAdr |= static_cast<uint16_t>(val & 0b111) << 12; // fil CBA
                vTempAdr |= static_cast<uint16_t>(val & 0xC0) << 2; // fill HG
                vTempAdr |= static_cast<uint16_t>(val & 0x38) << 2; // fill FED
                writeToggle = 0;
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


// Increment coarse X of vAdr
// Both these functions are taken from nesdev:
// https://wiki.nesdev.com/w/index.php/PPU_scrolling
void Ppu::coraseXIncr() {
    if (!PpuMask.bkgrdEnable && !PpuMask.spriteEnable) return;

    if ((vAdr & 0x001F) == 31) { // if coarse X == 31
        vAdr &= ~0x001F;         // coarse X = 0
        vAdr ^= 0x0400;         // switch horizontal nametable
    }
    else {
        vAdr += 1;                // increment coarse X
    }
}

// Increment coarse Y of vAdr
void Ppu::coraseYIncr() {
    if (!PpuMask.bkgrdEnable && !PpuMask.spriteEnable) return;

    if ((vAdr & 0x7000) != 0x7000) {        // if fine Y < 7
        vAdr += 0x1000;                      // increment fine Y
    }
    else {
        vAdr &= ~0x7000;                     // fine Y = 0
        int y = (vAdr & 0x03E0) >> 5;        // let y = coarse Y
        if (y == 29) {
            y = 0;                          // coarse Y = 0
            vAdr ^= 0x0800;                    // switch vertical nametable
        }
        else if (y == 31) {
            y = 0;                          // coarse Y = 0, nametable not switched
        }
        else {
            y += 1;                         // increment coarse Y
        }
        vAdr = static_cast<uint16_t>( (vAdr & ~0x03E0) | (y << 5) );     // put coarse Y back into v
    }
}

// These transfers simply move the temporary vAdr into vAdr assossiated with X or Y

// Move all parts X parts of vTemp into vAdr
void Ppu::transferX() {
    if (!PpuMask.bkgrdEnable && !PpuMask.spriteEnable) return;

    // set the nameTable x of temp to the vram
    vAdr = (vAdr & ~0x0400) | (vTempAdr & 0x0400);
    // set the coarse x as well
    vAdr = (vAdr & ~0x001F) | (vTempAdr & 0x001F);

}

// Move all parts of Y of vTemp to vAdr
void Ppu::transferY() {
    if (!PpuMask.bkgrdEnable && !PpuMask.spriteEnable) return;
    // set fineY
    vAdr = (vAdr & ~0x7000) | (vTempAdr & 0x7000);
    // set nameTable y
    vAdr = (vAdr & ~0x0800) | (vTempAdr & 0x0800);
    // set coarse Y
    vAdr = (vAdr & ~0x03E0) | (vTempAdr & 0x3E0);

}

// Returns the Fine Y of vAdr
inline uint8_t Ppu::getFineY() const noexcept {
    return (vAdr >> 12) & 0x07;
}


// Fetch the NameTable byte, NT byte is from VRAM, the byte is the id of the pattern
// table to be used to grab the low and high tile background address
// Essentialy a tile to be represented
void Ppu::fetchNameTableByte() {
    // The 0x0FFF mask of VAdr returns coarse X and coarse Y scroll into the nametable
    // The upper two bits contains which nametable, each a multiple of 1024; the size of a nametable
    // Combined this is just the (y * width) + x of a nametable -> (coarse Y scroll) + coarse X scroll
    // Since coarse Y is already in the upper bits of 32, there is no reason to split it
    nameTableLatch = vRamRead(0x2000 | (vAdr & 0x0FFF));
}

// Fetch the attribute table byte
void Ppu::fetchAttrTableByte() {
    // Recall the attribute table at the end of a nametable
    // divide coarse x and coarse y by 4(bit shift by 2) to get which attribute table to use at the end of the nametable
    uint16_t finalCoarse = 0;
    uint8_t coarseX = vAdr & 0x1F;
    uint8_t coarseY = (vAdr & 0x3E0) >> 5;

    finalCoarse = coarseX >> 2; // coarse x
    finalCoarse |= (coarseY >> 2) << 3; // coarse y
    finalCoarse |= vAdr & 0x300; // nametable select
    finalCoarse |= 0x23C0; // where the attribute tables are located

    // At this point there is a further spliting into the byte
    // each 2 bits represent a tile
    uint8_t tileAttr = vRamRead(finalCoarse);
    if (coarseY & 0x02) tileAttr >>= 4; // In the top half
    if (coarseX & 0x02) tileAttr >>= 2; // In the left half
    // only the bottom 2 bits are considered
    tileAttr &= 0x03;

    attrTableLatch = tileAttr;
}



// Fetch the lower background tile byte
// It's location is known from fetching the nametable byte of which the id is given
void Ppu::fetchPatternLowByte() {
    // The control register determines which pattern table will be used
    uint16_t patternSelect = static_cast<uint16_t>(PpuCtrl.bkgrdTile << 12);
    // The fetch from the nametable tells the ID of tile that will be used in range 0-0xFF
    // Each tile is 16 bytes long, so we must skip to the id we actually want
    uint16_t patternLoc = nameTableLatch * 16;
    // The fine y portion of vAdr tells which line of the tile it needs (0-7)
    uint8_t y = getFineY();

    patternTableLowLatch = vRamRead(patternSelect + patternLoc + y);

}

// Fetch the higher background tile byte
void Ppu::fetchPatternHighByte() {
    // Same code and idea for lower byte, but +8 for the higher bit plane line
    patternTableHighLatch = vRamRead( static_cast<uint16_t>( (PpuCtrl.bkgrdTile << 12) + nameTableLatch * 16 + getFineY() + 8) );
}

// Shift all the shift registers by 1 to the left
// Done for each working cycle to load in the next pixel to be manipulated/selected by fineX
void Ppu::shiftRegisters() noexcept {
    if (!PpuMask.bkgrdEnable) return;

    attrShiftLow <<= 1;
    attrShiftHigh <<= 1;
    bkShiftLow <<= 1;
    bkShiftHigh <<= 1;
}

// Every 8 cycles take the 8 bit latches and store them into the shift registers
// Since every 8 cycles they are shifted 8 times, the last 8 bits will be 0
void Ppu::updateShifters() noexcept {
    bkShiftLow = (bkShiftLow & 0xFF00) | patternTableLowLatch;
    bkShiftHigh = (bkShiftHigh & 0xFF00) | patternTableHighLatch;

    // For attribute tables, one major restriction on NES is that a tile/line can only refer to one palette
    // The attrLatch contains 3 bits of which id of palette to use.
    // When they are grouped together to form a pixel (bl, bh, al, ah) al and ah must be consistent throughout the entire 8 bits
    // So they are either a 0 or a 1 to tell which of the bits remains consistent
    attrShiftLow = (attrShiftLow & 0xFF00) | ((attrTableLatch & 0b01) ? 0xFF : 0); // 0'th bit is consistent 1-> all 1s, 0-> all 0's
    attrShiftHigh = (attrShiftHigh & 0xFF00) | ((attrTableLatch & 0b10) ? 0xFF : 0); // same thing but for 1'th bit
}

// https://forums.nesdev.com/viewtopic.php?t=10348
void Ppu::renderPixel() {
    // Definately rewrite how to do this later, it is very ugly.
    if (!PpuMask.bkgrdEnable) return;
    // now to display the pixel!
    uint8_t pixel = 0; // pixel contains which color of the palette (0-3)
    uint8_t paletteID = 0; // contains the id of palette
    {
        // now from each shift register, it makes sense to get only the highest/msb of each shift register (0x8000/15th bit)
        // however fineXscroll determines it
        uint16_t mask = 0x8000 >> fineXScroll;

        // Get each individual bit from the shift registers
        uint8_t p0=0, p1=0, b0=0, b1=0;
        p0 = (mask & bkShiftLow) > 0;
        p1 = (mask & bkShiftHigh) > 0;
        b0 = (mask & attrShiftLow) > 0;
        b1 = (mask & attrShiftHigh) > 0;
        // now combine them
        pixel = static_cast<uint8_t>( (p1 << 1) | p0);
        paletteID = static_cast<uint8_t>( (b1 << 1) | b0);
    }

    uint8_t x = static_cast<uint8_t>(cycle - 1);
    uint8_t y = static_cast<uint8_t>(scanline);

    uint8_t chroma = getChromaFromPaletteRam(paletteID, pixel);
    nes->addVideoData(x, y, chroma);
}

void Ppu::clear() {
    PpuCtrl.clear();
    PpuMask.clear();
    PpuStatus.clear();
    std::fill(memory.begin(), memory.end(), 0);
    std::fill(OAM.begin(), OAM.end(), 0);
    OamAddr = 0;
    scanline = vAdr = vTempAdr = fineXScroll = writeToggle = 0;
}

// Sets VBlank
void Ppu::setVBlank() {
    PpuStatus.vblank = 1;
    if (PpuCtrl.NMI)
        nes->cpu.signalNMI();
}

// Clear vblank, 0 sprite and overflow flags
void Ppu::clearVBlank() {
    PpuStatus.clear();
}

void Ppu::runCycle() {
    // The visible scanline
    if (scanline >= -1 && scanline < 240) {
        // Odd Frame skip
        if (scanline == 0 && cycle == 0) {
            cycle = 1;
        }

        // Start of the rendering frame, clear the flag so the cpu can't do work to the ppu
        if (scanline == -1 && cycle == 1) {
            clearVBlank();
            completeFrame = false;
        }

        // At each cycle here the ppu is getting data ready for the NEXT 8 pixels
        // 2-258 is the actual visual location on screen
        // 321-338 is for the next scanline after this one
        if (inRange(2, 257, cycle) || inRange(321, 337, cycle)) {

            // Draw and render a pixel on the visible scanline
            if (inRange(2, 258, cycle)) renderPixel();
            // left shift the shift registers
            shiftRegisters();

            switch((cycle - 1) % 8) {
                case 0:
                    updateShifters();
                    fetchNameTableByte();
                    break;
                case 2:
                    fetchAttrTableByte();
                    break;
                case 4:
                    fetchPatternLowByte();
                    break;
                case 6:
                    fetchPatternHighByte();
                    break;
                case 7:
                    coraseXIncr(); // done with current tile, incremenet the tiling address to go to the next tile
                    break;
            }
        }
        // done with current line of each tile +1 to the y direction for every next tile
        // basically move on to the next row of each tile
        if (cycle == 256) {
            coraseYIncr();
        }

        // fix/reset x coordinate to the start of the scanline
        if (cycle == 257) {
            transferX();
        }

        // fix/reset y coordinate to the start aswell, but it must be done
        // outside of the visible scanline
        if (scanline == -1 && inRange(280, 304, cycle)) {
            transferY();
        }
    }

    // end of the visible frame, set vBlank to true such that cpu can now do work
    if (scanline == 241 && cycle == 1) {
        setVBlank();
    }

    ++cycle;

    // end of hblank, go down one
    if (cycle > 341) {
        cycle = 0;
        ++scanline;
        // vblank triggered
        if (scanline >= 261) {
            scanline = -1; // -1 for a pre render scanline to render the next 8 pixels
            completeFrame = true;
        }
    }
}








// --------- PPU CONTROL ---------------

Inner::PPUCTRL::operator uint8_t() const noexcept {
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

void Inner::PPUCTRL::fromByte(const uint8_t& byte) noexcept {
    nameTable = byte & 0b11;
    increment = (byte & 0b100) >> 2;
    spriteTile = (byte & 0x8) >> 3;
    bkgrdTile = (byte & 0x10) >> 4;
    spriteSz = (byte & 0x20) >> 5;
    masterSlave = (byte & 0x40) >> 6;
    NMI = (byte & 0x80) >> 7;
}

void Inner::PPUCTRL::clear() noexcept {
    nameTable = increment = spriteTile = bkgrdTile = spriteSz = masterSlave = NMI = 0;
}

// ------------ PPU MASK ------------

Inner::PPUMASK::operator uint8_t() const noexcept {
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

void Inner::PPUMASK::fromByte(const uint8_t& byte) noexcept {
    greyScale = byte & 1;
    bkgrdLeftEnable = (byte & 2) >> 1;
    spriteLeftEnable = (byte & 0b100) >> 2;
    bkgrdEnable = (byte & 0x8) >> 3;
    spriteEnable = (byte & 0x10) >> 4;
    red = (byte & 0x20) >> 5;
    green = (byte & 0x40) >> 6;
    blue = (byte & 0x80) >> 7;
}

void Inner::PPUMASK::clear() noexcept {
    greyScale = bkgrdLeftEnable = spriteLeftEnable = bkgrdEnable = spriteEnable = red = green = blue = 0;
}

// ------------ PPU STATUS -----------

Inner::PPUSTATUS::operator uint8_t() const noexcept {
    uint8_t byte = 0;
    byte |= sOverflow << 5;
    byte |= sprite0Hit << 6;
    byte |= vblank << 7;
    return byte;
}

void Inner::PPUSTATUS::fromByte(const uint8_t& byte) noexcept {
    sOverflow = (byte & 0x20) >> 5;
    sprite0Hit = (byte & 0x40) >> 6;
    vblank = (byte & 0x80) >> 7;
}

void Inner::PPUSTATUS::clear() noexcept {
    sOverflow = sprite0Hit = vblank = 0;
}
