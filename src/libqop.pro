TEMPLATE = lib

QT += core gui 

CONFIG *= qop-buildlib

!include(libqop.pri): error(could not find libqop.pri)

#src
unix: SOURCES += 
else:win32: SOURCES += 


greaterThan(QT_MAJOR_VERSION, 3) {
        HEADERS += zprocess.h zprocess_p.h
        SOURCES += zprocess.cpp
} else {
        #DEFINES += QT_NO_PROCESS
}

!*g++* {
        HEADERS += getopt.h
        SOURCES += getopt.cpp
}


HEADERS		+= qop_global.h QOutParser.h \
                gui/ezprogressdialog.h \
                gui/ezprogressdialog_p.h \
                qcounterthread.h \
                qarchive/arcreader.h \
                Types.h \
                qarchive/qarchive.h \
                qarchive/tar/qtar.h \
                qarchive/tar/TarItem.h \
                qarchive/tar/TarHeader.h \
                qarchive/zip/ZipHeader.h \
                qarchive/gzip/GzipHeader.h \
                qarchive/gzip/GzipItem.h \
                qop.h \
                commandparser.h \
                version.h \
                qarchive/qarchive_p.h \
                msgdef.h \
                qtcompat.h \
                utils/util.h \
                utils/strutil.h \
                utils/convert.h \
                utils/qt_util.h

SOURCES		+= QOutParser.cpp \
                gui/ezprogressdialog.cpp \
                qcounterthread.cpp \
                qarchive/arcreader.cpp \
                qarchive/qarchive.cpp \
                qarchive/tar/qtar.cpp \
                qarchive/zip/ZipHeader.cpp \
                qarchive/gzip/GzipItem.cpp \
                qop.cpp \
                commandparser.cpp \
                utils/util.cpp \
                utils/strutil.cpp \
                utils/convert.cpp \
                utils/qt_util.cpp



