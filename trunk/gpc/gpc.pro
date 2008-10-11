TARGET = gpc
SOURCES += gpc.c

HEADERS += gpc.h

TEMPLATE = lib
DESTDIR = ../lib
CONFIG += staticlib \
debug \
warn_on

CONFIG -= release \
exceptions \
stl \
qt \
thread
