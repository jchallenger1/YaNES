#ifndef NAMETABLEVIEW_HPP
#define NAMETABLEVIEW_HPP

#include <QWidget>
#include <QTimer>
#include "ui_nametableview.h"
#include "NES.hpp"

namespace Ui {
class NameTableView;
}

class NameTableView : public QWidget {
    Q_OBJECT
public:
    inline NameTableView(NES& nes, QWidget *parent = nullptr);
    inline NameTableView(NES& nes, bool shouldStepNes, QWidget *parent = nullptr);
    inline ~NameTableView();

    inline void timeTick();
private:
    Ui::NameTableView *ui;
    std::shared_ptr<NES> nes;
    QTimer* timer;
};

NameTableView::NameTableView(NES& nes, QWidget *parent) : QWidget(parent), ui(new Ui::NameTableView) {
    ui->setupUi(this);
    this->nes = std::make_shared<NES>(nes);
}

NameTableView::NameTableView(NES& nes, bool shouldStepNes, QWidget *parent) : QWidget(parent), ui(new Ui::NameTableView) {
    ui->setupUi(this);
    this->nes = std::make_shared<NES>(nes);
    if (shouldStepNes) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &NameTableView::timeTick);
        timer->start(0);
    }
}

NameTableView::~NameTableView() {
    delete ui;
}

void NameTableView::timeTick() {
    nes->step();
}


#endif // NAMETABLEVIEW_HPP
