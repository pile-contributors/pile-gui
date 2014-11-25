#-------------------------------------------------
#
# Project created by QtCreator 2014-11-07T17:13:35
#
#-------------------------------------------------

QT       += core gui network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pilesgui
TEMPLATE = app

debug {
  DEFINES += PILESGUI_DEBUG
  DEFINES -= PILESGUI_RELEASE
}
release {
  DEFINES -= PILESGUI_DEBUG
  DEFINES += PILESGUI_RELEASE
}
win32 {
  DEFINES += PILESGUI_WINDOWS
}
macx {
  DEFINES += PILESGUI_MAC
}
unix {
  DEFINES += PILESGUI_UNIX
}

SOURCES += main.cpp\
    mainwindow.cpp \
    pileslist.cpp \
    downloader.cpp \
    pilesgui.cpp \
    logic/pile.cpp \
    logic/pileprovider.cpp \
    logic/pilecontainer.cpp \
    settingsdlg.cpp \
    newpiledlg.cpp \
    support/replacer.cpp \
    support/gitwrapper.cpp \
    support/cmakewrapper.cpp \
    custcmddlg.cpp \
    copyreplacedlg.cpp \
    support/programrunner.cpp \
    support/waiter.cpp \
    snippetsdlg.cpp \
    snippets/snipmodel.cpp \
    snippets/snipitem.cpp \
    envdlg.cpp \
    editors/codeeditor.cc \
    support/fsmodelenhanced.cpp

HEADERS  += mainwindow.h \
    setting_names.h \
    pileslist.h \
    downloader.h \
    pilesgui.h \
    logic/pile.h \
    logic/pileprovider.h \
    logic/pilecontainer.h \
    settingsdlg.h \
    newpiledlg.h \
    support/replacer.h \
    config.h \
    support/gitwrapper.h \
    support/cmakewrapper.h \
    custcmddlg.h \
    copyreplacedlg.h \
    support/programrunner.h \
    support/waiter.h \
    snippetsdlg.h \
    snippets/snipmodel.h \
    snippets/snipitem.h \
    envdlg.h \
    editors/codeeditor.h \
    support/fsmodelenhanced.h

FORMS    += mainwindow.ui \
    pileslist.ui \
    settingsdlg.ui \
    newpiledlg.ui \
    custcmddlg.ui \
    copyreplacedlg.ui \
    snippetsdlg.ui \
    envdlg.ui

RESOURCES += \
    appres.qrc
