#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>
#include "NES.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(std::shared_ptr<NES> nes, QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void virtual paintEvent(QPaintEvent*) override;
    void virtual keyPressEvent(QKeyEvent* key) override;
    void virtual keyReleaseEvent(QKeyEvent* key) override;

private:
    static inline void setPaintColour(QPainter& painter, const QColor& c);
    void paint();
    void timeTick();
    Ui::MainWindow *ui;
    std::shared_ptr<NES> nes;
    QTimer* timer;

};

#endif // MAINWINDOW_H
