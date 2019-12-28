#include <QPainter>
#include <QWidget>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include <QTimer>
#include <QString>
#include <iostream>
#include <memory>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "functions.hpp" // apply_from_tuple

std::ostream& operator<<(std::ostream&, const QString&); // helper for << operator for qstrings

// UI interface constructor
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    nes = std::make_shared<NES>();
    // Other tables must be intantiated after nes has been created
    nameTableViewer = new NameTableView(nes, false);
    patternTableViewer = new PatternTableView(nes, false);

    nameTableViewer->hide();
    patternTableViewer->hide();

    // connect everything but don't run it
    this->timer = new QTimer(this);

    // Timer for stepping the nes
    connect(this->timer, &QTimer::timeout, this, &MainWindow::timeTick);
    // File menubar
    connect(ui->actionOpen_iNES_file, &QAction::triggered, this, &MainWindow::loadFile);
    // Debug menubar
    connect(ui->actionNametable_Viewer, &QAction::triggered, this, [&](){
        nameTableViewer->show();
    });
    connect(ui->actionPatternTable_Viewer, &QAction::triggered, this, [&](){
        patternTableViewer->show();
    });

}

// File is already good to go, only for debugging purposes
MainWindow::MainWindow(std::shared_ptr<NES> nes, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    {

    ui->setupUi(this);
    this->nes = nes;
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &MainWindow::timeTick);
    timer->start(0);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent*) {
    paint();
}

void MainWindow::keyPressEvent(QKeyEvent *key) {
    UNUSED(key);
}

void MainWindow::keyReleaseEvent(QKeyEvent *key) {
    UNUSED(key);
}

void MainWindow::setPaintColour(QPainter& painter, const QColor& c) {
    painter.setPen(c);
    painter.setBrush(c);
}

void MainWindow::paint() {
    QPainter painter(this);

    for (uint8_t y = 0; y != 240; y++) {
        for (uint8_t x = 0; x != 255; x++) {
            auto rgb = Ppu::getRGBPalette(nes->screen[y][x] & 0x3F);
            QColor colour = apply_from_tuple(qRgb, rgb);
            setPaintColour(painter, colour);
            painter.drawRect(x, y, 1, 1);
        }
    }

}

void MainWindow::timeTick() {
    for (int i = 0; i != 15; ++i) nes->step();

    if (nes->ppu.completeFrame) {
        nes->ppu.completeFrame = false;
        repaint();
    }
}

void MainWindow::loadFile() {
    // Set a file dialog to select which file
    QFileDialog dialog(this);
    dialog.setOption(QFileDialog::DontUseNativeDialog, true); // qt error if false
    dialog.setFileMode(QFileDialog::AnyFile);
    QString filename;

    // make this call blocking
    if (dialog.exec()) {

        QStringList list = dialog.selectedFiles();
        // Make sure only one file was selected
        if (list.count() > 1) {
            sendMessage("Only select one file");
            std::cerr << "Too many files selected\n";
            return;
        }
        else if (list.count() == 0) {
            std::cerr << "No file selected\n";
            return;
        }
        // set to filename
        filename = list.at(0);
    }
    else {
        std::cerr << "Failure to send file dialog to the user\n";
        std::cerr << "This is most likely an error with QT\n";
        return;
    }
    // At this point we have a valid file, now load it into the NES
    // Important!
    // Make sure the timer is not trying to run or display anything while we
    // are trying to load the nes contents
    bool active = timer->isActive();

    try {
        if (active) {
            timer->stop();
        }
        // Load the componenets
        nes->init();
        nes->load(filename.toStdString());
        // Set components to power up state
        nes->powerUp();
        // Start displaying
        timer->start(0);
    } catch(const std::exception& e) {
        // Error has occured, tell the user
        std::cerr << e.what() << "\n";
        QString message = "A fatal error has occured!\n";
        message += e.what();
        sendMessage(message);
        // If the last rom was a working rom, start it again
        if (active) {
            timer->start(0);
        }

    }
}

// Function sends a message to the user in a UI box
void MainWindow::sendMessage(const QString& message, const QString& title) {
    QMessageBox msgBox;
    msgBox.setWindowTitle(title);
    msgBox.setText(message);
    msgBox.exec();
}

// Quick helper function for sending qstrings to stdout
std::ostream& operator<<(std::ostream& os, const QString& QTs) {
    os << QTs.toStdString();
    return os;
}
