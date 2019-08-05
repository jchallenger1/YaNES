#include <memory>
#include <iostream>
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
    // Painter
    painter = new QPainter(this);
}

void DebugView::timeClick() {
    nes->step();
}

void DebugView::paintEvent(QPaintEvent *) {
    paint();
}

typename DebugView::tileT DebugView::getTile(const unsigned& tileNum) const {
    tileT tile{};
    auto createLine = [](const uint8_t& left, const uint8_t& right) -> uint16_t {

    };

}

void DebugView::paint() {
    DrawWidget* d = dynamic_cast<DrawWidget*>(ui->stackedWidget->currentWidget());
    QPainter *& painter = d->painter;
    // Draw Left Pattern Table
    std::array<uint8_t, GamePak::KB16>& vRam = nes->ppu.memory;
    for (unsigned i = 0 ; i < 0x1000; i += 16) {
        auto tile = getTile(i);
    }
}

DebugView::~DebugView() {
    delete ui;
}
