TEMPLATE = app
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += c++14

SOURCES += \
        src/Cpu6502.cpp \
        src/GamePak.cpp \
        src/Memory.cpp \
        src/NES.cpp \
        src/Ppu.cpp \
        src/debugview.cpp \
        src/main.cpp \
        src/mainwindow.cpp

INCLUDEPATH += include/

HEADERS += \
    include/Cpu6502.h \
    include/GamePak.h \
    include/Memory.h \
    include/NES.h \
    include/Ppu.h \
    include/debugview.h \
    include/functions.hpp \
    include/mainwindow.h \
    include/nametableview.hpp \
    include/patterntableview.hpp

FORMS += \
    forms/debugview.ui \
    forms/mainwindow.ui \
    forms/nametableview.ui \
    forms/patterntableview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


