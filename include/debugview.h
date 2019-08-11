#ifndef DEBUGVIEW_H
#define DEBUGVIEW_H

#include <QWidget>
#include <QTimer>
#include <memory>
#include "NES.hpp"
#include "patterntableview.hpp"
#include "nametableview.hpp"

namespace Ui {
class DebugView;
}

class DebugView final: public QWidget {
    Q_OBJECT
public:
    explicit DebugView(QWidget *parent = nullptr);
    DebugView(NES& nes, QWidget* parent = nullptr);
    virtual ~DebugView() override;

private:
    void init();

    std::shared_ptr<NES> nes;
    Ui::DebugView *ui;
    QTimer* timer;
    PatternTableView* pTableView;
    NameTableView* nTableView;
};

#endif // DEBUGVIEW_H
