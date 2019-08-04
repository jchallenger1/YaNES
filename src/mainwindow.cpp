#include <QPainter>
#include <QWidget>
#include <iostream>
#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {
    ui->setupUi(this);
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
