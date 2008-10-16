SOURCES += main.cpp \
window.cpp
FORMS += window.ui

HEADERS += window.h

TEMPLATE = app



QT += opengl

CONFIG -= release

CONFIG += debug


DESTDIR = ../../bin/

INCLUDEPATH += ../../common \
  ../../Box2D \
  ../../include/

LIBS += ../../lib/libflyercommon.a \
  ../../lib/libbox2d.a \
  ../../lib/libgpc.a \
  -L../../lib/ \
  -lgpc \
  -lbox2d

TARGETDEPS += ../../lib/libflyercommon.a \
  ../../lib/libgpc.a \
  ../../lib/libbox2d.a

