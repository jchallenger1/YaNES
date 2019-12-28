#include <QApplication>
#include <QException>
#include "mainwindow.h"
#include "Cpu6502.h"
#include "Ppu.h"
#include "NES.h"
#include "GamePak.h"

// This creates a static application directly mounted to where nes is loaded
// Should only be used for debugging purposes
/*
int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    std::shared_ptr<NES> nes = std::make_shared<NES>();
    nes->init();
    nes->load("../YaNES/rsc/roms/Donkey Kong (World) (Rev A).nes");
    nes->powerUp();

    MainWindow main(nes->getPtr());
    main.show();
    return a.exec();

}
*/

// UI interface of main
int main(int argc, char *argv[]) {

    QApplication a(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();

}

