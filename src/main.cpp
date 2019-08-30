#include <QApplication>
#include <QException>
#include "mainwindow.h"
#include "debugview.h"
#include "Cpu6502.h"
#include "Ppu.h"
#include "NES.h"
#include "GamePak.h"

int main(int argc, char *argv[]) {
    try {
        QApplication a(argc, argv);

        std::shared_ptr<NES> nes = std::make_shared<NES>();
        nes->init();
        GamePak::load(nes->cpu.memory, nes->ppu, "../YaNES/rsc/roms/Donkey Kong (World) (Rev A).nes");
        nes->powerUp();

        MainWindow main(nes->getPtr());
        NameTableView name(nes->getPtr(), false);
        main.show();
        name.show();
        return a.exec();
    } catch (const QException& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    catch (...) {
        std::cerr << "Fatal Error caught out of main" << std::endl;
        return -1;
    }
}
