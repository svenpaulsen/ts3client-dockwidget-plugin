#-------------------------------------------------
#
# TeamSpeak 3 Client Dock Widget Plugin
#
#-------------------------------------------------

QT     += widgets
CONFIG += c++14
TARGET   = dockwidget_plugin
VERSION  = _VERSION_
TEMPLATE = lib

CONFIG += skip_target_version_ext

CONFIG(32bit) {
    TARGET = dockwidget_plugin_win32
}
CONFIG(64bit) {
    TARGET = dockwidget_plugin_win64
}

INCLUDEPATH += src
INCLUDEPATH += includes/pluginsdk/include

DEFINES += "PLUGIN_VER=\\\"$${VERSION}\\\""

SOURCES += src/plugin.cpp \
    src/widgethelper.cpp \
    src/shared.cpp
HEADERS += src/plugin.h \
    src/widgethelper.h \
    src/shared.h \
    src/singleton.h

