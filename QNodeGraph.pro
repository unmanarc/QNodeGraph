QT -= gui
QT += widgets xml

CONFIG += c++17
CONFIG -= app_bundle

TEMPLATE = lib
TARGET = qnodegraph-qt$$QT_MAJOR_VERSION

include(version.pri)

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/abstractnodewidget.cpp \
    src/arrange.cpp \
    src/graphwidget.cpp \
    src/groupwidget.cpp \
    src/itemwidget.cpp \
    src/link.cpp \
    src/xmlfunctions.cpp

HEADERS += \
    src/abstractnodewidget.h \
    src/arrange.h \
    src/graphwidget.h \
    src/groupwidget.h \
    src/itemwidget.h \
    src/link.h \
    src/xmlfunctions.h

# includes dir
QMAKE_INCDIR += $$PREFIX/include
QMAKE_INCDIR += src
INCLUDEPATH += $$PREFIX/include
INCLUDEPATH += src

# INSTALLATION:
target.path = $$PREFIX/lib64
header_files.files = $$HEADERS
header_files.path = $$PREFIX/include/$$TARGET
INSTALLS += target
INSTALLS += header_files

# PKGCONFIG
CONFIG += create_pc create_prl no_install_prl
QMAKE_PKGCONFIG_LIBDIR = $$PREFIX/lib64/
QMAKE_PKGCONFIG_INCDIR = $$PREFIX/include/$$TARGET
QMAKE_PKGCONFIG_CFLAGS = -I$$PREFIX/include/
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
#$$PREFIX/share/

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

