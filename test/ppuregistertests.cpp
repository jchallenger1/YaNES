#include "tests.hpp"
#include "NES.hpp"
#include "Cpu6502.hpp"
#include "Ppu.hpp"
#include "Memory.hpp"


void Tests::ppuRegisterTests() {
    Cpu6502 cpu;
    Ppu ppu;
    NES nes(cpu, ppu);

    // Check write to 0x2000
    cpu.a = 0xAB;
    cpu.memory[0] = 0x8D; // STA ABS
    cpu.memory[1] = 0;
    cpu.memory[2] = 0x20;
    cpu.runCycle();
    ckPassErr(ppu.PpuCtrl.asByte() == cpu.a, "Writing A to register Ppu 0x2000 fail");

    // Check write to 0x2001
    cpu.a = 0xAE;
    cpu.memory[3] = 0x8D;
    cpu.memory[4] = 1;
    cpu.memory[5] = 0x20;
    cpu.runCycle();
    ckPassErr(ppu.PpuMask.asByte() == cpu.a, "Writing A to register Ppu 0x2001 fail");

    // Check read from 0x2002
    ppu.PpuStatus.vblank = ppu.PpuStatus.sOverflow = ppu.PpuStatus.sprite0Hit = 1;
    cpu.a = 0;
    cpu.memory[6] = 0xAD; // LDA ABS
    cpu.memory[7] = 2;
    cpu.memory[8] = 0x20;
    cpu.runCycle();
    ckPassErr(ppu.PpuStatus.asByte() == cpu.a && cpu.a == 0xE0, "Reading from Ppu Register 0x2002 fail");


}
