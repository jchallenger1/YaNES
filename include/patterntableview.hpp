#ifndef PATTERNTABLEVIEW_HPP
#define PATTERNTABLEVIEW_HPP

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <memory>
#include "ui_patterntableview.h"
#include "NES.hpp"
#include "Ppu.hpp"

// from https://cpppatterns.com/patterns/apply-tuple-to-function.html
// Website also includes an explantation
// C++17 also includes std::apply, but assuming c++14 here.
template<typename F, typename Tuple, size_t ...S >
decltype(auto) apply_tuple_impl(F&& fn, Tuple&& t, std::index_sequence<S...>) {
    return std::forward<F>(fn)(std::get<S>(std::forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
decltype(auto) apply_from_tuple(F&& fn, Tuple&& t) {
    constexpr std::size_t tSize = std::tuple_size<
                typename std::remove_reference<Tuple>::type
            >::value;
    return apply_tuple_impl(std::forward<F>(fn), std::forward<Tuple>(t),
                            std::make_index_sequence<tSize>());
}

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

    inline void drawPatternTable(QPainter& painter, const uint16_t& startAdr, const int& originX, const int& originY);
    inline QColor getPalQColor(const uint16_t& address) const;
private:
    Ui::PatternTableView *ui;
    std::shared_ptr<NES> nes;
    uint8_t resizeFactor = 2;
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
    typename Ppu::PaletteT universalPalette = Ppu::getRGBPalette(colorByte);
    return apply_from_tuple(qRgb, universalPalette);

}

void PatternTableView::paint()  {
    QPainter painter(this);
    drawPatternTable(painter, 0x0, 0, 0);
    drawPatternTable(painter, 0x1000, 400, 0);
    setColor(painter, getPalQColor(0x3F00));

}
#endif // PATTERNTABLEVIEW_HPP
