#include <QPainter>
#include <QWidget>
#include <QTimer>
#include <iostream>
#include <memory>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "functions.hpp" // apply_from_tuple



MainWindow::MainWindow(std::shared_ptr<NES> nes, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
    ui->setupUi(this);
    this->nes = nes;
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &MainWindow::timeTick);
    timer->start(0);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent*) {
    paint();
}

void MainWindow::setPaintColour(QPainter& painter, const QColor& c) {
    painter.setPen(c);
    painter.setBrush(c);
}

void MainWindow::paint() {
    QPainter painter(this);
    for (uint8_t y = 0; y != 240; y++) {
        for (uint8_t x = 0; x != 255; x++) {
            auto rgb = Ppu::getRGBPalette(nes->screen[x][y] & 0x3F);
            QColor colour = apply_from_tuple(qRgb, rgb);
            setPaintColour(painter, colour);
            painter.drawRect(x, y, 1, 1);
        }
    }
}

void MainWindow::timeTick() {
    static int i = 0;
    nes->step();

    if (i % 1000 == 0)
        this->repaint();

    /*
    if (nes->videoRequested) {
        this->repaint();
    }
    */
    ++i;
}
