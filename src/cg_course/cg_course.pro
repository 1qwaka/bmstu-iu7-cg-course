QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    intersectvisitor.cpp \
    lens.cpp \
    light.cpp \
    main.cpp \
    mainwindow.cpp \
    material.cpp \
    measureworker.cpp \
    object.cpp \
    objectadduicontroller.cpp \
    objectdeleteuicontroller.cpp \
    polyobject.cpp \
    predefinedscenes.cpp \
    renderworker.cpp \
    scene.cpp \
    sphere.cpp \
    state.cpp \
    tracer.cpp \
    uiutils.cpp \
    vars.cpp \
    visitor.cpp

HEADERS += \
    awesomemacros.h \
    measureworker.h \
    objectnaming.h \
    camera.h \
    colorop.h \
    intersectvisitor.h \
    lens.h \
    light.h \
    mainwindow.h \
    material.h \
    object.h \
    objectadduicontroller.h \
    objectdeleteuicontroller.h \
    polyobject.h \
    predefinedscenes.h \
    renderworker.h \
    scene.h \
    sphere.h \
    state.h \
    tracer.h \
    tri.h \
    uiutils.h \
    vars.h \
    visitor.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

RESOURCES += \
    res.qrc
