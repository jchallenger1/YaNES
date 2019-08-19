#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NES.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(NES& nes, QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void paintEvent(QPaintEvent*) override;
    void paint();
    void timeTick();
    Ui::MainWindow *ui;
    std::shared_ptr<NES> nes;
    QTimer* timer;

};

#endif // MAINWINDOW_H
