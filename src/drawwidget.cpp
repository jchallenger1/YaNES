#include "include/drawwidget.hpp"
#include "ui_drawwidget.h"

DrawWidget::DrawWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawWidget)
{
    ui->setupUi(this);
    painter = new QPainter(this);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::black);
    painter->drawRect(50,50,100,100);
}

DrawWidget::~DrawWidget(){
    delete ui;
}
