#include <memory>
#include <iostream>
#include <cmath>
#include <bitset>
#include <QTimer>
#include <QPainter>
#include "include/debugview.h"
#include "ui_debugview.h"
#include "include/drawwidget.hpp"
#include "NES.hpp"

DebugView::DebugView(QWidget *parent) : QWidget(parent),  ui(new Ui::DebugView) {
    init();
}

DebugView::DebugView(NES& nes, QWidget* parent) : QWidget (parent), ui(new Ui::DebugView) {
    this->nes = std::make_shared<NES>(nes);
    init();
}

void DebugView::init() {
    ui->setupUi(this);
    // Remove predefault widgets in stack and put in the one we want
    DrawWidget* drawW = new DrawWidget();
    ui->stackedWidget->removeWidget(ui->page);
    ui->stackedWidget->removeWidget(ui->page_2);
    ui->page->deleteLater();
    ui->page_2->deleteLater();
    ui->stackedWidget->addWidget(drawW);
    // Timer
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DebugView::timeClick);
    timer->start(0);
}

void DebugView::timeClick() {
    //nes->step();
}

void DebugView::paintEvent(QPaintEvent *) {
    paint();
}

typename DebugView::tileT DebugView::getTile(const unsigned& tileAddress) const {
    tileT tile{};
    // Create a line of a tile
    // For every bit position of left and right, set it to the two bits equivalent position
    // in a 16 bit field, ex if left -> 0 , right -> 1, then bitPos(line) = 10 in the u16 type
    auto createLine = [](const uint8_t& left, const uint8_t& right) -> uint16_t {
            uint16_t line = 0;
            for (short i = 0; i != 8; i++) {
                const uint16_t pow2 = static_cast<uint16_t>(std::pow(2,i));
                line |= (static_cast<uint16_t>(right) & pow2) << (i + 1);
                line |= (static_cast<uint16_t>(left) & pow2) << i;
            }
            return line;
    };

    for (unsigned i = 0; i != 8; i++) {
        tile[i % 8] = createLine(nes->ppu.memory[tileAddress + i], nes->ppu.memory[tileAddress + i + 8]);
    }
    return tile;

}

void DebugView::stdDrawTile(const unsigned int &tileAddress) const {
    tileT tile = getTile(tileAddress);
    for (auto begin = tile.cbegin(); begin != tile.cend(); begin++) {
        std::bitset<16> p(*begin);
        std::cout << p << "\n";
    }
}

void DebugView::paint() {
    QPainter painter(this);
    auto getColor = [](const uint8_t& n) -> auto {
        switch(n) {
            case 0: return Qt::blue;
            case 1: return Qt::red;
            case 2: return Qt::yellow;
            default: return Qt::black;
        }
    };
    auto setColor = [&painter](const auto& color) {
        painter.setPen(color);
        painter.setBrush(color);
    };

    constexpr uint8_t resizeFac = 2;

    // Draw Left Pattern Table
    for (unsigned tileAddr = 0x1000, tileCount = 0 ; tileAddr < 0x2000; tileAddr += 16, ++tileCount) {
        tileT tile = getTile(tileAddr);
        for (uint8_t y = 0; y != 8; y++) {
            uint16_t line = tile[y];
            for (uint8_t x = 0; x != 8; x++) {
                uint8_t pixel = ( line >> (x * 2) ) & 0b11;
                setColor(getColor(pixel));
                int pixelX = static_cast<int>( (tileCount % 16) * 8 + x ) * resizeFac;
                int pixelY = static_cast<int>( y + static_cast<int>(tileCount / 16) * 8) * resizeFac;
                painter.drawRect(pixelX, pixelY, resizeFac, resizeFac);
            }
        }
    }
    std::cerr << "finish paint\n";
}

DebugView::~DebugView() {
    delete ui;
}
