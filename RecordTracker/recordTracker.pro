CONFIG += qt debug
SOURCES += main.cpp MainWindow.cpp glView.cpp colorwheel.cpp FontRender.cpp KinectRecord.cpp split.cpp
HEADERS += ui_mainwindow.h MainWindow.h glView.h colorwheel.h FontRender.h KinectRecord.h split.h

INCLUDEPATH += /usr/include/opencv2
CFLAGS += -fPIC -g -Wall `pkg-config --cflags opencv cvblob`
LIBS += -lGLU -lboost_filesystem -lboost_system

LIBS += `pkg-config --libs opencv cvblob`

LIBS += -lm

QT += opengl
QT += xml

TARGET = record-tracker
