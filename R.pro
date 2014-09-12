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
    imagearea.cpp \
#    mainwidget.cpp \
    processor.cpp

HEADERS  += mainwindow.hpp \
    imagearea.hpp \
#    mainwidget.hpp \
    processor.hpp

FORMS    += mainwindow.ui \
    imagearea.ui
unix{
LIBS += -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_photo -L"/usr/local/qwt-6.1.0/lib" -lqwt
INCLUDEPATH += "/usr/local/qwt-6.1.0/include"
QMAKE_CXXFLAGS += -std=c++0x
}

win32{
INCLUDEPATH += "D:\opencv\build\include"
INCLUDEPATH += "D:\qwt-6.1.0\qwt-6.1.0\qwt"
}


RESOURCES += \
    shaders.qrc

OTHER_FILES += \
    vshader.glsl \
    fshader.glsl

win32:CONFIG(release, debug|release): LIBS += -L"C:/Qwt-6.1.0/lib/" -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L"C:/Qwt-6.1.0/lib/" -lqwtd


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_core249
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_core249d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_highgui249
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_highgui249d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_imgproc249
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_imgproc249d

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_photo249
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../opencv/build/x86/vc12/lib/ -lopencv_photo249d
