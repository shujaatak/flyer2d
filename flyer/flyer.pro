TEMPLATE = app
TARGET = flyer

SOURCES = main.cpp worldwidget.cpp \
 mainwindow.cpp \
 plane.cpp \
 world.cpp \
 worldobject.cpp \
 ground.cpp \
 airfield.cpp \
 landinglight.cpp \
 damagemanager.cpp \
 system.cpp \
 engine.cpp \
 joint.cpp \
 mounting.cpp \
 environment.cpp \
 surface.cpp \
 wing.cpp \
 controlsurface.cpp \
 wheelbrake.cpp \
 autopilot.cpp \
 machine.cpp \
 bullet.cpp \
 gun.cpp \
 spotlight.cpp \
 objects/planebumblebee.cpp \
 statuswindow.cpp \
 objects/antiairbattery.cpp \
 antiairgunoperator.cpp \
 shrapnel.cpp \
 explosion.cpp \
 objects/ironbomb.cpp \
 contactfuse.cpp \
 cloud.cpp \
 building.cpp

HEADERS = worldwidget.h \
 mainwindow.h \
 plane.h \
 world.h \
 worldobject.h \
 ground.h \
 airfield.h \
 landinglight.h \
 damagemanager.h \
 system.h \
 engine.h \
 joint.h \
 mounting.h \
 environment.h \
 surface.h \
 wing.h \
 controlsurface.h \
 wheelbrake.h \
 autopilot.h \
 machine.h \
 bullet.h \
 gun.h \
 spotlight.h \
 objects/planebumblebee.h \
 statuswindow.h \
 objects/antiairbattery.h \
 antiairgunoperator.h \
 shrapnel.h \
 explosion.h \
 objects/ironbomb.h \
 contactfuse.h \
 cloud.h \
 building.h \
 \
 renderingoptions.h
FORMS += mainwindow.ui \
 statuswindow.ui


CONFIG -= release

CONFIG += debug \
 opengl
QT += opengl

HEADERS -= object.h \
 renderingoptions.h

#DEFINES += FLYER_NO_OPENGL

LIBS += -L../lib/ \
  -L. \
  -lbox2d -lflyercommon

POS_TARGETDESP = ../lib/libflyercommon.a
INCLUDEPATH += ../common \
  objects/ \
  ../include

TARGETDEPS += ../lib/libflyercommon.a \
  ../lib/libbox2d.a

SOURCES -= renderingoptions.cpp

