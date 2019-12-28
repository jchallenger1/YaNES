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
    // This constructor is only if there is a file ready to go, all components
    // of the NES is already set up, only used in debugging
    // example of use in main
    explicit MainWindow(std::shared_ptr<NES> nes, QWidget *parent = nullptr);
    // No components are set up constructor, default for UI
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void virtual paintEvent(QPaintEvent*) override;
    void virtual keyPressEvent(QKeyEvent* key) override;
    void virtual keyReleaseEvent(QKeyEvent* key) override;

private:
    static inline void setPaintColour(QPainter& painter, const QColor& c);
    void paint();
    void timeTick();

    void loadFile();
    void sendMessage(const QString&, const QString& title = "Message");

    Ui::MainWindow *ui;
    std::shared_ptr<NES> nes;
    QTimer* timer;

};

#endif // MAINWINDOW_H
