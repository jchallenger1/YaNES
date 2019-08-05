#ifndef DRAWWIDGET_HPP
#define DRAWWIDGET_HPP

#include <QWidget>
#include <QPainter>

namespace Ui {
class DrawWidget;
}

class DrawWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrawWidget(QWidget *parent = nullptr);
    ~DrawWidget();
    QPainter* painter;
private:
    Ui::DrawWidget *ui;
};

#endif // DRAWWIDGET_HPP
