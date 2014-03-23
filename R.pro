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
    converter.cpp \
    mainwidget.cpp

HEADERS  += mainwindow.hpp \
    imagearea.hpp \
    converter.hpp \
    helpers.hpp \
    mainwidget.hpp

FORMS    += mainwindow.ui \
    imagearea.ui

LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -L"/usr/local/qwt-6.1.0/lib" -lqwt
INCLUDEPATH += "/usr/local/qwt-6.1.0/include"

QMAKE_CXXFLAGS += -std=c++0x

RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl
