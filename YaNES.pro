TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/Disassembler6502.cpp \
        src/State6502.cpp \
        src/main.cpp \
        src/memory.cpp



INCLUDEPATH += include/

HEADERS += \
    include/Disassembler6502.hpp \
    include/State6502.hpp \
    include/memory.hpp
