TEMPLATE = app
DESTDIR = ../bin

HEADERS += editor.h \
editorpoint.h \
editorpolygon.h \
editableitem.h \
editorcircle.h \
bodywrapper.h \
editablewrapper.h \
 shapewrapper.h \
 editorgraphicsview.h
SOURCES += editor.cpp \
main.cpp \
editorpoint.cpp \
editorpolygon.cpp \
editableitem.cpp \
editorcircle.cpp \
bodywrapper.cpp \
editablewrapper.cpp \
 shapewrapper.cpp \
 editorgraphicsview.cpp
FORMS += editor.ui
RESOURCES =resources/resources.qrc

CONFIG += rtti



CONFIG += release

LIBS += -L../lib/ \
  -lflyercommon \
  -lbox2d \
  -lQPropertyEditor

INCLUDEPATH += ../common \
  ../flyer \
  ../include \
  ../QPropertyEditor

TARGETDEPS += ../lib/libflyercommon.a \
  ../lib/libbox2d.a

