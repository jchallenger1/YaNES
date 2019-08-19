#include <memory>
#include <iostream>
#include <cmath>
#include <bitset>
#include <QTimer>
#include <QPainter>
#include "include/debugview.h"
#include "ui_debugview.h"
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
    pTableView = new PatternTableView(*nes, true);
    //nTableView = new NameTableView(*nes, true);
    ui->stackedWidget->removeWidget(ui->page);
    ui->stackedWidget->removeWidget(ui->page_2);
    ui->page->deleteLater();
    ui->page_2->deleteLater();
    ui->stackedWidget->addWidget(pTableView);
    ui->stackedWidget->addWidget(nTableView);
    ui->stackedWidget->setCurrentIndex(1);
}

DebugView::~DebugView() {
    delete ui;
}
