#ifndef DEBUGVIEW_H
#define DEBUGVIEW_H

#include <QWidget>
#include <QTimer>
#include <memory>
#include "NES.hpp"

namespace Ui {
class DebugView;
}

class DebugView final: public QWidget {
    Q_OBJECT
public:
    explicit DebugView(QWidget *parent = nullptr);
    DebugView(NES& nes, QWidget* parent = nullptr);
    ~DebugView() override;

private:
    using tileT = std::array<uint16_t, 8>;
    void timeClick();
    void init();
    void paintEvent(QPaintEvent*) override;
    void paint();

    std::array<uint16_t, 8> getTile(const unsigned& tileNum) const;

    std::shared_ptr<NES> nes;
    Ui::DebugView *ui;
    QTimer* timer;
    QPainter* painter;
};

#endif // DEBUGVIEW_H
