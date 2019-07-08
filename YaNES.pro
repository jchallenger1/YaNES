TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        src/State6502.cpp \
        src/main.cpp \
        src/memory.cpp



INCLUDEPATH += include/

HEADERS += \
    include/State6502.hpp \
    include/memory.hpp
