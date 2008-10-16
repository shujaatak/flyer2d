SOURCES += main.cpp \
window.cpp
FORMS += window.ui

HEADERS += window.h

TEMPLATE = app

CONFIG -= release

CONFIG += debug

QT += opengl

INCLUDEPATH += ../../common \
  ../../include/

DESTDIR = ../../bin/

LIBS += ../../lib/libflyercommon.a \
  -L../../lib/ \
  -lbox2d \
  -lgpc

TARGETDEPS += ../../lib/libflyercommon.a

