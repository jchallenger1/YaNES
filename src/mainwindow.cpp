#include <QPainter>
#include <QWidget>
#include <QTimer>
#include <iostream>
#include <memory>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "functions.hpp" // apply_from_tuple



MainWindow::MainWindow(NES& nes, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
    ui->setupUi(this);
    this->nes = std::make_shared<NES>(nes);
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
    QPainter painter;
    while(!nes->pixelsToAdd.empty()) {
        std::cerr << "draw";
        NES::PixelT pixel = nes->pixelsToAdd.front();

        QColor colour = apply_from_tuple(qRgb, std::get<2>(pixel));
        setPaintColour(painter, colour);
        painter.drawRect(std::get<0>(pixel), std::get<1>(pixel), 1, 1);

        nes->pixelsToAdd.pop();
    }

}

void MainWindow::timeTick() {
    nes->step();
    if (nes->videoRequested) {
        std::cerr << "req";
        this->repaint();
    }
}
