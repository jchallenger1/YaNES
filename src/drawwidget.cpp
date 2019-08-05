#include "include/drawwidget.hpp"
#include "ui_drawwidget.h"

DrawWidget::DrawWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DrawWidget)
{
    ui->setupUi(this);
    painter = new QPainter(this);
}

DrawWidget::~DrawWidget(){
    delete ui;
}
