TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/Cpu6502.cpp \
        src/GamePak.cpp \
        src/Memory.cpp \
        src/Ppu.cpp

INCLUDEPATH += include/

HEADERS += \
    include/Cpu6502.hpp \
    include/GamePak.hpp \
    include/Memory.hpp \
    include/Ppu.hpp
