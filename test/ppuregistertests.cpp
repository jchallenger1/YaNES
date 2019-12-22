#include "tests.hpp"
#include "NES.h"
#include "Cpu6502.h"
#include "Ppu.h"
#include "Memory.h"


void Tests::ppuRegisterTests() {
    std::shared_ptr<NES> nes = std::make_shared<NES>();
    nes->init();
    Cpu6502& cpu = nes->cpu;
    Ppu& ppu = nes->ppu;

    // Check write to 0x2000
    cpu.a = 0xAB;
    cpu.memory[0] = 0x8D; // STA ABS
    cpu.memory[1] = 0;
    cpu.memory[2] = 0x20;
    cpu.runCycle();
    ckPassErr(ppu.PpuCtrl == cpu.a && cpu.a == 0xAB, "Writing A to register Ppu 0x2000 fail");

    // Check write to 0x2001
    cpu.a = 0xAE;
    cpu.memory[3] = 0x8D;
    cpu.memory[4] = 1;
    cpu.memory[5] = 0x20;
    cpu.runCycle();
    ckPassErr(ppu.PpuMask == cpu.a && cpu.a == 0xAE, "Writing A to register Ppu 0x2001 fail");

    // Check read from 0x2002
    ppu.PpuStatus.vblank = ppu.PpuStatus.sOverflow = ppu.PpuStatus.sprite0Hit = 1;
    cpu.a = 0;
    cpu.memory[6] = 0xAD; // LDA ABS
    cpu.memory[7] = 2;
    cpu.memory[8] = 0x20;
    cpu.runCycle();
    // Note that sOverflow is reset on a read
    ckPassErr(ppu.PpuStatus == 0xC0 && cpu.a == 0xE0, "Reading from Ppu Register 0x2002 fail");

    // Check write to 0x2003
    cpu.a = 0xAB;
    cpu.memory[9] = 0x8D; // STA ABS
    cpu.memory[10] = 3;
    cpu.memory[11] = 0x20;
    cpu.runCycle();
    ckPassFail(ppu.OamAddr == cpu.a, "OAM write fail, can't check 0x2004 read/writes");

    // Check write to 0x2004
    cpu.a = 0xBF;
    cpu.memory[12] = 0x8D; // STA ABS
    cpu.memory[13] = 4;
    cpu.memory[14] = 0x20;
    cpu.runCycle(); // OAM(AB) now contains BF.
    // Check read from 0x2004, note that writes increments the address
    cpu.a = 0xAB;
    cpu.memory[15] = 0x8D; // Go back to AB address
    cpu.memory[16] = 3;
    cpu.memory[17] = 0x20;
    cpu.runCycle();

    cpu.a = 0;
    cpu.memory[18] = 0xAD; // LDA ABS
    cpu.memory[19] = 4;
    cpu.memory[20] = 0x20;
    cpu.runCycle();
    ckPassErr(cpu.a == 0xBF, "OAM Data Read/Write failure");

    nes->clear();
    // Check Double write 0x2006
    cpu.a = 0x21; // high byte
    cpu.memory[0] = 0x8D;
    cpu.memory[1] = 6;
    cpu.memory[2] = 0x20;
    cpu.runCycle();

    cpu.a = 0x08;
    cpu.memory[3] = 0x8D;
    cpu.memory[4] = 6;
    cpu.memory[5] = 0x20;
    cpu.runCycle();

    ckPassFail(ppu.vAdr == 0x2108 && ppu.writeToggle == 0, "Address x2 Write failure, can't continue");
    // Check write to 2007

    cpu.a = 0xFD;
    cpu.memory[6] = 0x8D;
    cpu.memory[7] = 7;
    cpu.memory[8] = 0x20;
    cpu.runCycle();

    ckPassErr(ppu.memory[0x2108] == 0xFD, "Data 0x2007 ppu register failure");

    nes->clear();

    // Check write to 0x4014
    std::fill(cpu.memory.memory.begin() + 0x8000, cpu.memory.memory.begin() + 0x80FF, 0x50);
    cpu.a = 0x80;
    cpu.memory[0] = 0x8D; // STA ABS
    cpu.memory[1] = 0x14;
    cpu.memory[2] = 0x40;
    cpu.runCycle();
    auto it = std::find_if(ppu.OAM.cbegin(), ppu.OAM.cend(), [](const auto& val) {
        return val != 0x50;
     });
    ckPassErr(it == ppu.OAM.cend(), "OAM DMA failure");

}
