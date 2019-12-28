#include <QApplication>
#include <QException>
#include "mainwindow.h"
#include "Cpu6502.h"
#include "Ppu.h"
#include "NES.h"
#include "GamePak.h"

int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    std::shared_ptr<NES> nes = std::make_shared<NES>();
    nes->init();
    GamePak::load(nes->cpu.memory, nes->ppu, "../YaNES/rsc/roms/Donkey Kong (World) (Rev A).nes");
    nes->powerUp();
    //PatternTableView pattern(nes->getPtr(), true);
    //pattern.show();
    MainWindow main(nes->getPtr());
    //NameTableView name(nes->getPtr(), true);
    main.show();
    //name.show();
    return a.exec();

}
