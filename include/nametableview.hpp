#ifndef NAMETABLEVIEW_HPP
#define NAMETABLEVIEW_HPP

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <iostream>
#include "ui_nametableview.h"
#include "NES.hpp"
#include "functions.hpp"

namespace Ui {
class NameTableView;
}

class NameTableView : public QWidget {
    Q_OBJECT
public:
    inline NameTableView(NES& nes, QWidget *parent = nullptr);
    inline NameTableView(NES& nes, bool shouldStepNes, QWidget *parent = nullptr);
    inline ~NameTableView() override;

    inline void timeTick();
    inline void paintEvent(QPaintEvent*) override;
    inline void paint();
    inline QColor getPalQColor(const uint16_t& address) const;
    inline QColor getColor(const uint8_t& n) const;
private:
    Ui::NameTableView *ui;
    std::shared_ptr<NES> nes;
    QTimer* timer;
};

NameTableView::NameTableView(NES& nes, QWidget *parent) : QWidget(parent), ui(new Ui::NameTableView) {
    ui->setupUi(this);
    this->nes = std::make_shared<NES>(nes);
}

NameTableView::NameTableView(NES& nes, bool shouldStepNes, QWidget *parent) : QWidget(parent), ui(new Ui::NameTableView) {
    ui->setupUi(this);
    this->nes = std::make_shared<NES>(nes);
    if (shouldStepNes) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &NameTableView::timeTick);
        timer->start(0);
    }
}

NameTableView::~NameTableView() {
    delete ui;
}

void NameTableView::timeTick() {
    static int i = 0;
    std::cerr << std::to_string(i++) << "\n";
    for (int n = 0; n != 20; ++n)
        nes->step();
    if (i % 10000 == 0)
        this->repaint();
}

void NameTableView::paintEvent(QPaintEvent *) {
    paint();
}

QColor NameTableView::getPalQColor(const uint16_t &address) const {
    uint8_t colorByte = nes->ppu.vRamRead(address);
    typename Ppu::PaletteT universalPalette = Ppu::getRGBPalette(colorByte & 0x3F);
    return apply_from_tuple(qRgb, universalPalette);
}

QColor NameTableView::getColor(const uint8_t &n) const {
    switch(n) {
        case 0: return Qt::black;
        case 1: return Qt::red;
        case 2: return Qt::yellow;
        case 3: return Qt::blue;
        default: return Qt::black;
    }
}

void NameTableView::paint() {
    QPainter painter(this);
    auto setColor = [&painter](const auto& color) {
        painter.setPen(color);
        painter.setBrush(color);
    };

    // After 0x23C0 is the attribute table
    for (uint16_t address = 0x2000; address != 0x23C0; address++) {
        uint16_t tileNum = address - 0x2000;

        uint8_t paletteID = nes->ppu.getPaletteFromNameTable(tileNum, 0x23C0);

        // Address X and Y determine where to put the tile
        // The screen is a 32x30 tile screen, so to put it on, 32*8 and 30*8 for 256x240 NES screen size
        uint8_t addressX = tileNum % 32;
        uint8_t addressY = static_cast<uint8_t>(static_cast<int>(tileNum / 32));
        uint8_t byte = nes->ppu.vRamRead(address);
        Ppu::PatternTableT tile = nes->ppu.getPatternTile(byte);
        for (uint8_t tileY = 0; tileY != 8; tileY++) {
            uint16_t line = tile[tileY];
            for (uint8_t tileX = 0; tileX != 8; tileX++) {
                uint8_t pixel = ( line >> (tileX * 2) ) & 0b11; // two bits determine the color of bit (0, 1, 2)
                setColor(getColor(pixel));

                int pixelX = addressX * 8 + tileX;
                int pixelY = addressY * 8 + tileY;


                painter.drawRect(pixelX, pixelY, 1, 1);
            }
        }
    }

}


#endif // NAMETABLEVIEW_HPP
