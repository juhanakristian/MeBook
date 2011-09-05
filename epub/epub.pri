QT += xml
INCLUDEPATH += $$PWD
HEADERS += $$PWD/ziphandler.h \
           $$PWD/epub.h \
           $$PWD/opfhandler.h \
           $$PWD/ncxhandler.h

SOURCES += $$PWD/ziphandler.cpp \
           $$PWD/epub.cpp \
           $$PWD/opfhandler.cpp \
           $$PWD/ncxhandler.cpp

#minizip
INCLUDEPATH += $$PWD/minizip
HEADERS += unzip.h \
           ioapi.h

DEPENDPATH += $$PWD/minizip
SOURCES += unzip.c \
           ioapi.c
