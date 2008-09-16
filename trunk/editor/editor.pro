TEMPLATE = app

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

CONFIG += rtti \
 debug



CONFIG -= release

LIBS += -L../lib/ \
  -lflyercommon \
  -lbox2d \
  -lQPropertyEditord

INCLUDEPATH += ../common \
  ../flyer \
  ../include

TARGETDEPS += ../lib/libflyercommon.a \
  ../lib/libbox2d.a

