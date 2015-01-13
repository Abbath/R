#-------------------------------------------------
#
# Project created by QtCreator 2013-08-01T23:04:27
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
qtHaveModule(opengl) {
    QT += opengl
}
TARGET = R
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
#    mainwidget.cpp \
    imagearea.cpp \
    results.cpp \
    imageconverter.cpp \
    imageprocessor.cpp \
    lightdetector.cpp \
    capturewrapper.cpp \
    videoprocessor.cpp \
    imagestorage.cpp \
    streamprocessor.cpp

HEADERS  += mainwindow.hpp \
#    mainwidget.hpp \
    imagearea.hpp \
    results.hpp \
    imageconverter.hpp \
    imageprocessor.hpp \
    lightdetector.hpp \
    capturewrapper.hpp \
    videoprocessor.hpp \
    imagestorage.hpp \
    streamprocessor.hpp

FORMS    += mainwindow.ui \
    imagearea.ui

unix{
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -L"/usr/local/qwt-6.1.0/lib" -lqwt
INCLUDEPATH += "/usr/local/qwt-6.1.0/include"
QMAKE_CXXFLAGS += -std=c++0x -Wall -Wextra
}

win32{
INCLUDEPATH += "D:\opencv\build\include"
INCLUDEPATH += "D:\qwt-6.1.0\qwt-6.1.0\qwt"
}

RESOURCES += \
    shaders.qrc \
    icons.qrc

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl

win32:CONFIG(release, debug|release): LIBS += -L"C:/Qwt-6.1.0/lib/" -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Qwt-6.1.0/lib/" -lqwtd

win32:CONFIG(release, debug|release): LIBS += -L"D:/opencv/build/x86/vc12/lib" -lopencv_core249
else:win32:CONFIG(debug, debug|release): LIBS += -L"D:/opencv/build/x86/vc12/lib" -lopencv_core249d

win32:CONFIG(release, debug|release): LIBS += -L"D:/opencv/build/x86/vc12/lib" -lopencv_imgproc249
else:win32:CONFIG(debug, debug|release): LIBS += -L"D:/opencv/build/x86/vc12/lib" -lopencv_imgproc249d

win32:CONFIG(release, debug|release): LIBS += -L"D:/opencv/build/x86/vc12/lib" -lopencv_highgui249
else:win32:CONFIG(debug, debug|release): LIBS += -L"D:/opencv/build/x86/vc12/lib" -lopencv_highgui249d

