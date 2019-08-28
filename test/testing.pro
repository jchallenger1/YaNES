TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        ../src/Cpu6502.cpp \
        ../src/Memory.cpp \
        ../src/GamePak.cpp \
        ../src/Ppu.cpp \
        ../src/NES.cpp \
        nescputests.cpp \
        optests.cpp \
        mastertestsuite.cpp \
        ppuregistertests.cpp \
        testenv.cpp


INCLUDEPATH += ../include/

HEADERS += \
    ../include/Cpu6502.hpp \
    ../include/Memory.hpp \
    ../include/GamePak.hpp \
    ../include/Ppu.hpp \
    ../include/NES.hpp \
    tests.hpp

# Include Boost Program Options linking

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/release/ -lboost_program_options
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/debug/ -lboost_program_options
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lboost_program_options

INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
