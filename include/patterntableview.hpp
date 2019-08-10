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

void PatternTableView::paint()  {
    QPainter painter(this);
    static auto getColor = [](const uint8_t& n) -> auto {
        switch(n) {
            case 0: return Qt::black;
            case 1: return Qt::red;
            case 2: return Qt::yellow;
            default: return Qt::blue;
        }
    };
    auto setColor = [&painter](const auto& color) {
        painter.setPen(color);
        painter.setBrush(color);
    };
    setColor(Qt::black);
    // Draw Left Pattern Table
    for (uint16_t tileAddr = 0x0, tileCount = 0 ; tileAddr < 0x1000; tileAddr += 16, ++tileCount) {
        Ppu::PatternTableT tile = nes->ppu.getPatternTile(tileAddr);
        for (uint8_t y = 0; y != 8; y++) {
            uint16_t line = tile[y];
            for (uint8_t x = 0; x != 8; x++) {
                uint8_t pixel = ( line >> (x * 2) ) & 0b11; // two bits determine the color of bit (0, 1, 2)
                setColor(getColor(pixel));
                // For every 16 tiles skip to the next line
                // Every tile's width and height is 8 rows/columns long.
                int pixelX = static_cast<int>( (tileCount % 16) * 8 + x ) * resizeFactor;
                int pixelY = static_cast<int>( y + static_cast<int>(tileCount / 16) * 8) * resizeFactor;
                painter.drawRect(pixelX, pixelY, resizeFactor, resizeFactor);
            }
        }
    }
    // Draw Right Pattern Table
    for (uint16_t tileAddr = 0x1000, tileCount = 0 ; tileAddr < 0x2000; tileAddr += 16, ++tileCount) {
        Ppu::PatternTableT tile = nes->ppu.getPatternTile(tileAddr);
        for (uint8_t y = 0; y != 8; y++) {
            uint16_t line = tile[y];
            for (uint8_t x = 0; x != 8; x++) {
                uint8_t pixel = ( line >> (x * 2) ) & 0b11;
                setColor(getColor(pixel));
                int pixelX = static_cast<int>( (tileCount % 16) * 8 + x ) * resizeFactor + moveFactor;
                int pixelY = static_cast<int>( static_cast<int>(tileCount / 16) * 8 + y) * resizeFactor;
                painter.drawRect(pixelX, pixelY, resizeFactor, resizeFactor);
            }
        }
    }

}
#endif // PATTERNTABLEVIEW_HPP
