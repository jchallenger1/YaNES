TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../src/Cpu6502.cpp \
        ../src/Memory.cpp \
        ../src/GamePak.cpp \
        ../src/Ppu.cpp \
        nescputests.cpp \
        optests.cpp \
        mastertestsuite.cpp \
        testenv.cpp


INCLUDEPATH += ../include/

HEADERS += \
    ../include/Cpu6502.hpp \
    ../include/Memory.hpp \
    ../include/GamePak.hpp \
    ../include/Ppu.hpp \
    tests.hpp
