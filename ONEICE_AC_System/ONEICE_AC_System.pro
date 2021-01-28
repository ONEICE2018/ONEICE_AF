#-------------------------------------------------
#
# Project created by QtCreator 2020-02-21T15:39:59
#
#-------------------------------------------------
QT       += core gui
QT       += serialport
QT       += network
QT       += widgets printsupport
QT       += websockets
QT       += core gui printsupport
QT       += multimedia
#excel相关
QT  +=  core  gui  axcontainer
 win32 {
    INCLUDEPATH += "C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Include"
    LIBS += "C:\Program Files (x86)\IVI Foundation\VISA\WinNT\lib\msc\visa32.lib"
}
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ONEICE_AC_System
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    qextserialbase.cpp \
    qextserialenumerator.cpp \
    qextserialport.cpp \
    win_qextserialport.cpp \
    powermeter.cpp \
    powersource.cpp \
    appropriatenetworkanalyzer.cpp \
    spectrograph.cpp \
    datarecord_base.cpp \
    datacenter.cpp \
    curve_fitting.cpp \
    axistag.cpp \
    qcustomplot.cpp \
    con2stm.cpp \
    chioced_dataitem.cpp \
    onelistwidgetitem.cpp \
    onelistwidget.cpp \
    onedragtimer.cpp \
    cf_lineclassdialog.cpp \
    onefilemanage.cpp \
    onecurve.cpp

HEADERS += \
        mainwindow.h \
    qextserialbase.h \
    qextserialenumerator.h \
    qextserialport.h \
    win_qextserialport.h \
    powermeter.h \
    datarecord_base.h \
    curve_fitting.h \
    axistag.h \
    qcustomplot.h \
    con2stm.h \
    chioced_dataitem.h \
    onelistwidgetitem.h \
    onelistwidget.h \
    onedragtimer.h \
    cf_lineclassdialog.h \
    onefilemanage.h \
    onecurve.h

FORMS += \
        mainwindow.ui \
    datarecord_base.ui \
    curve_fitting.ui \
    con2stm.ui \
    chioced_dataitem.ui \
    cf_lineclassdialog.ui \
    onecurve.ui

RESOURCES += \
    ac_source.qrc
