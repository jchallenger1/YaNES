#ifndef PATTERNTABLEVIEW_HPP
#define PATTERNTABLEVIEW_HPP

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <memory>
#include "ui_patterntableview.h"
#include "NES.hpp"

namespace Ui {
class PatternTableView;
}

class PatternTableView : public QWidget {
    Q_OBJECT
public:
    inline explicit PatternTableView(NES& nes, QWidget *parent = nullptr);
    inline virtual ~PatternTableView() override;

    inline void paintEvent(QPaintEvent*) override;
    inline void paint();
    static inline constexpr auto getColor(const uint8_t&);
    inline void setColor(QPainter& painter, const QColor& color);

    inline void drawLPatternTable(QPainter& painter);
    inline void drawRPatternTable(QPainter& painter);
private:
    Ui::PatternTableView *ui;
    std::shared_ptr<NES> nes;
    uint8_t resizeFactor = 2;
    uint16_t moveFactor = 400;
};

PatternTableView::PatternTableView(NES& nes, QWidget *parent) : QWidget(parent), ui(new Ui::PatternTableView) {
    ui->setupUi(this);
    this->nes = std::make_shared<NES>(nes);
}

PatternTableView::~PatternTableView() {
    delete ui;
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

void PatternTableView::drawLPatternTable(QPainter& painter) {
    for (uint16_t tileAddr = 0x0, tileCount = 0 ; tileAddr < 0x1000; tileAddr += 16, ++tileCount) {
        Ppu::PatternTableT tile = nes->ppu.getPatternTile(tileAddr);
        for (uint8_t y = 0; y != 8; y++) {
            uint16_t line = tile[y];
            for (uint8_t x = 0; x != 8; x++) {
                uint8_t pixel = ( line >> (x * 2) ) & 0b11; // two bits determine the color of bit (0, 1, 2)
                setColor(painter, getColor(pixel));
                // For every 16 tiles skip to the next line
                // Every tile's width and height is 8 rows/columns long.
                int pixelX = static_cast<int>( (tileCount % 16) * 8 + x ) * resizeFactor;
                int pixelY = static_cast<int>( y + static_cast<int>(tileCount / 16) * 8) * resizeFactor;
                painter.drawRect(pixelX, pixelY, resizeFactor, resizeFactor);
            }
        }
    }
}

void PatternTableView::drawRPatternTable(QPainter& painter) {
    for (uint16_t tileAddr = 0x1000, tileCount = 0 ; tileAddr < 0x2000; tileAddr += 16, ++tileCount) {
        Ppu::PatternTableT tile = nes->ppu.getPatternTile(tileAddr);
        for (uint8_t y = 0; y != 8; y++) {
            uint16_t line = tile[y];
            for (uint8_t x = 0; x != 8; x++) {
                uint8_t pixel = ( line >> (x * 2) ) & 0b11;
                setColor(painter, getColor(pixel));
                int pixelX = static_cast<int>( (tileCount % 16) * 8 + x ) * resizeFactor + moveFactor;
                int pixelY = static_cast<int>( static_cast<int>(tileCount / 16) * 8 + y) * resizeFactor;
                painter.drawRect(pixelX, pixelY, resizeFactor, resizeFactor);
            }
        }
    }
}

void PatternTableView::paint()  {
    QPainter painter(this);
    drawLPatternTable(painter);
    drawRPatternTable(painter);

}
#endif // PATTERNTABLEVIEW_HPP
