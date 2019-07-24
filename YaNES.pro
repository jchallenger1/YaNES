TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/Disassembler6502.cpp \
        src/GamePak.cpp \
        src/Memory.cpp \
        src/Ppu.cpp \
        src/State6502.cpp

INCLUDEPATH += include/

HEADERS += \
    include/Disassembler6502.hpp \
    include/GamePak.hpp \
    include/Memory.hpp \
    include/Ppu.hpp \
    include/State6502.hpp
