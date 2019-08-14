#ifndef PATTERNTABLEVIEW_HPP
#define PATTERNTABLEVIEW_HPP

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QTimer>
#include <memory>
#include <iostream>
#include "ui_patterntableview.h"
#include "NES.hpp"
#include "Ppu.hpp"
#include "functions.hpp" // apply_from_tuple()

namespace Ui {
class PatternTableView;
}

class PatternTableView : public QWidget {
    Q_OBJECT
public:
    inline explicit PatternTableView(NES& nes, bool stepNES, QWidget *parent = nullptr);
    inline virtual ~PatternTableView() override;

    inline void paintEvent(QPaintEvent*) override;

    static inline constexpr auto getColor(const uint8_t&);
    static inline void setColor(QPainter& painter, const QColor& color);

private:
    inline void paint();
    inline void stepNES();

    inline void drawPatternTable(QPainter& painter, const uint16_t& startAdr, const int& originX, const int& originY);
    inline QColor getPalQColor(const uint16_t& address) const;
    inline void drawPalette(QPainter& painter, const uint16_t& startAdr, const int& originX, const int& originY, const bool& isGroup = true);

    Ui::PatternTableView *ui;
    std::shared_ptr<NES> nes;
    QTimer* timer;
    uint8_t resizeFactor = 2;
};

PatternTableView::PatternTableView(NES& nes, bool stepNES, QWidget *parent) : QWidget(parent), ui(new Ui::PatternTableView) {
    ui->setupUi(this);
    this->nes = std::make_shared<NES>(nes);
    if (stepNES) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &PatternTableView::stepNES);
        timer->start(0);
    }
}

PatternTableView::~PatternTableView() {
    delete ui;
}

void PatternTableView::stepNES() {
    for (int i = 0; i != 25; i++)
        nes->step();
    static int i = 0;
    if (i++ % 10000 == 0)
        repaint();
}

void PatternTableView::paintEvent(QPaintEvent *) {
    paint();
}

constexpr auto PatternTableView::getColor(const uint8_t& n) {
    switch(n) {
        case 0: return Qt::black;
        case 1: return Qt::red;
        case 2: return Qt::yellow;
        default: return Qt::blue;
    }
}

void PatternTableView::setColor(QPainter& painter, const QColor& color) {
    painter.setPen(color);
    painter.setBrush(color);
}

inline void PatternTableView::drawPatternTable(QPainter& painter, const uint16_t& startAdr, const int& originX, const int& originY) {
    for (uint16_t tileAddr = startAdr, tileCount = 0 ; tileAddr < startAdr + 0x1000; tileAddr += 16, ++tileCount) {
        Ppu::PatternTableT tile = nes->ppu.getPatternTile(tileAddr);
        for (uint8_t y = 0; y != 8; y++) {
            uint16_t line = tile[y];
            for (uint8_t x = 0; x != 8; x++) {
                uint8_t pixel = ( line >> (x * 2) ) & 0b11; // two bits determine the color of bit (0, 1, 2)
                setColor(painter, getColor(pixel));
                // For every 16 tiles skip to the next line
                // Every tile's width and height is 8 rows/columns long.
                int pixelX = static_cast<int>( (tileCount % 16) * 8 + x ) * resizeFactor + originX;
                int pixelY = static_cast<int>( y + static_cast<int>(tileCount / 16) * 8) * resizeFactor + originY;
                painter.drawRect(pixelX, pixelY, resizeFactor, resizeFactor);
            }
        }
    }
}

QColor PatternTableView::getPalQColor(const uint16_t &address) const {
    uint8_t colorByte = nes->ppu.vRamRead(address);
    typename Ppu::PaletteT universalPalette = Ppu::getRGBPalette(colorByte & 0x3F);
    return apply_from_tuple(qRgb, universalPalette);
}

inline void PatternTableView::drawPalette(QPainter& painter, const uint16_t& startAdr, const int& originX, const int& originY, const bool& isGroup) {
    for (uint16_t colorAdr = startAdr, times = 0; times != (isGroup ? 3 : 1); ++colorAdr, ++times ) {
        setColor(painter, getPalQColor(colorAdr));
        int pixelX = originX + 16 * times;
        int pixelY = originY;
        painter.drawRect(pixelX, pixelY, 16, 16);
    }
}

void PatternTableView::paint()  {
    QPainter painter(this);
    drawPatternTable(painter, 0x0, 0, 0); // Left Pattern Table
    drawPatternTable(painter, 0x1000, 400, 0); // Right Pattern Table

    static constexpr int paletteY = 325;

    // Background Palette Tables
    drawPalette(painter, 0x3F00, 0, paletteY, false); // Universal Background Color
    drawPalette(painter, 0x3F01, 40, paletteY); // Background palette 0
    drawPalette(painter, 0x3f05, 100, paletteY); // Background palette 1
    drawPalette(painter, 0x3f09, 170, paletteY); // ...
    drawPalette(painter, 0x3f0d, 240, paletteY);

    // Sprite Palette Tables
    drawPalette(painter, 0x3F11, 400, paletteY); // Sprite Palette 0
    drawPalette(painter, 0x3F15, 470, paletteY); // Sprite Palette 1
    drawPalette(painter, 0x3F19, 540, paletteY); // ...
    drawPalette(painter, 0x3F1D, 600, paletteY);



}
#endif // PATTERNTABLEVIEW_HPP
