#include <QPainter>
#include <QWidget>
#include <QTimer>
#include <iostream>
#include <memory>
#include "mainwindow.h"
#include "ui_mainwindow.h"



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

void MainWindow::paint() {
    QPainter* painter = new QPainter(this);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::black);
    painter->drawRect(20,20,100,100);
}

void MainWindow::timeTick() {

    static int i = 0;

    nes->step();


    if (i % 100 == 0) this->repaint();

    i++;
}
