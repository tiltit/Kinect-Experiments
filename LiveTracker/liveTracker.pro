CONFIG += qt debug
SOURCES += main.cpp MainDialog.cpp TrackWidget.cpp colorwheel.cpp MyFreenect.cpp FontRender.cpp
HEADERS += MainDialog.h TrackWidget.h colorwheel.h MyFreenect.hpp FontRender.h

CFLAGS += -fPIC -g -Wall `pkg-config --cflags opencv cvblob`
LIBS += -lGLU

INCLUDEPATH += /usr/include/opencv2 /usr/local/include/libfreenect/
LIBS += `pkg-config --libs opencv cvblob`
LIBS += -lfreenect -L/usr/local/lib64

LIBS += -lm

QT += opengl
QT += xml

TARGET = live-tracker
