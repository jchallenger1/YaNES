#include <QApplication>
#include "mainwindow.h"
#include "debugview.h"
#include "Cpu6502.h"
#include "Ppu.h"
#include "NES.h"
#include "GamePak.h"

int main(int argc, char *argv[]) {
    Ppu ppu;
    Cpu6502 cpu;
    GamePak::load(cpu.memory, ppu, "../YaNES/rsc/roms/Donkey Kong (World) (Rev A).nes");

    NES nes(cpu, ppu);
    nes.powerUp();
    QApplication a(argc, argv);
    //MainWindow w;
    //w.show();
    //DebugView d(nes);
    //d.show();
    NameTableView view(nes, true);
    view.show();
    //PatternTableView pview(nes, true);
   // pview.show();
    return a.exec();
}
