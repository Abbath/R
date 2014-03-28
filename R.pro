#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:04:27
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(opengl) {
    QT += opengl
}
TARGET = R
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    imagearea.cpp \
    mainwidget.cpp \
    processor.cpp

HEADERS  += mainwindow.hpp \
    imagearea.hpp \
    mainwidget.hpp \
    processor.hpp

FORMS    += mainwindow.ui \
    imagearea.ui
unix{
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -L"/usr/local/qwt-6.1.0/lib" -lqwt
INCLUDEPATH += "/usr/local/qwt-6.1.0/include"
}

win32{
LIBS += -lopencv_core246 -lopencv_highgui246 -lopencv_imgproc246 -lqwt -L"C:\opencv-build\bin"
INCLUDEPATH += "C:\opencv\build\include"
INCLUDEPATH += "C:\Users\Dan\Downloads\qwt-6.1.0\qwt-6.1.0\qwt"
LIBS += -L"C:\Users\Dan\Downloads\qwt-6.1.0\qwt-6.1.0\lib"
}
QMAKE_CXXFLAGS += -std=c++0x

RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl
