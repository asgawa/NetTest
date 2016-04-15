QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    customprotocol.cpp

HEADERS  += mainwindow.h \
    define_packet.h \
    customprotocol.h

FORMS    += mainwindow.ui \
    customprotocol.ui

release:TARGET_DIR = $$PWD/release
debug:TARGET_DIR = $$PWD/debug

DESTDIR = $$TARGET_DIR
MOC_DIR = $$TARGET_DIR/.moc
OBJECTS_DIR = $$TARGET_DIR/.obj
UI_DIR = $$TARGET_DIR/.ui
RCC_DIR = $$TARGET_DIR/.rcc

release:QMAKE_LFLAGS += -static
