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
 body.cpp \
 joint.cpp \
 mounting.cpp \
 environment.cpp \
 surface.cpp \
 wing.cpp \
 controlsurface.cpp \
 wheelbrake.cpp \
 autopilot.cpp \
 machine.cpp \
 b2dqt.cpp \
 bullet.cpp \
 gun.cpp \
 spotlight.cpp


HEADERS = worldwidget.h Box2D.h \
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
 body.h \
 joint.h \
 mounting.h \
 environment.h \
 surface.h \
 b2dqt.h \
 wing.h \
 controlsurface.h \
 wheelbrake.h \
 autopilot.h \
 machine.h \
 bullet.h \
 gun.h \
 spotlight.h

FORMS += mainwindow.ui

LIBS += -L. \
  -lbox2d

CONFIG -= release

CONFIG += debug \
 opengl
QT += opengl

HEADERS -= object.h

#DEFINES += FLYER_NO_OPENGL
