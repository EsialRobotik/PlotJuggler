#-------------------------------------------------
#
# Project created by QtCreator 2016-04-02T12:24:58
#
#-------------------------------------------------

TARGET = $$qtLibraryTarget(DataLoadCSV)

TEMPLATE = lib
CONFIG  += plugin

QT      += gui widgets


SOURCES += dataload_csv.cpp \
        selectxaxisdialog.cpp

HEADERS += dataload_csv.h\
        selectxaxisdialog.h

FORMS  +=  selectxaxisdialog.ui

INCLUDEPATH += $$PWD/../../common \
               $$PWD/../../qwt/src

unix {
    target.path = /usr/lib
    INSTALLS += target
}

DESTDIR = $$OUT_PWD/../../bin/plugins

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/ -lcommon
else:unix: LIBS += -L$$OUT_PWD/../../common/ -lcommon

INCLUDEPATH += $$PWD/../../common
DEPENDPATH += $$PWD/../../common

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcommon.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/libcommon.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/common.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../lib/common.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../common/libcommon.a
