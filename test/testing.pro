TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../src/Disassembler6502.cpp \
        ../src/Memory.cpp \
        ../src/State6502.cpp \
        ../src/GamePak.cpp \
        nescputests.cpp \
        optests.cpp \
        mastertestsuite.cpp \
        testenv.cpp


INCLUDEPATH += ../include/

HEADERS += \
    ../include/Disassembler6502.hpp \
    ../include/Memory.hpp \
    ../include/State6502.hpp \
    ../include/GamePak.hpp \
    tests.hpp
