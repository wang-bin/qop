TEMPLATE	= app
CONFIG		= qt warn_on release #for EZX
#CONFIG		+= static
DESTDIR	= bin
MOC_DIR		= .moc
OBJECTS_DIR	= .obj
DEFINES		+= QT_THREAD_SUPPORT
HEADERS		= QOutParser.h \
		  gui/ezprogressdialog.h \
		  gui/ezprogressdialog_p.h \
		  global.h \
		  qcounterthread.h \
		  util.h \
    qarchive/arcreader.h \
    option.h \
    qany.h \
	Types.h \
    qarchive/qarchive.h \
    qarchive/tar/qtar.h \
    qarchive/tar/TarItem.h \
    qarchive/tar/TarHeader.h \
    algorithm/zlib_alg.h \
	qarchive/zip/ZipHeader.h \
    qarchive/gzip/GzipHeader.h \
	qarchive/gzip/GzipItem.h \
	Compress/BitlDecoder.h

SOURCES		= QOutParser.cpp \
		  gui/ezprogressdialog.cpp \
		  main.cpp \
		  qcounterthread.cpp \
    qarchive/arcreader.cpp \
    util.cpp \
    option.cpp \
    qarchive/qarchive.cpp \
    qarchive/tar/qtar.cpp \
    algorithm/zlib_alg.cpp \
    qarchive/zip/ZipHeader.cpp \
	qarchive/gzip/GzipItem.cpp \
	Compress/BitlDecoder.cpp

INCLUDEPATH	+= .
LIBS	+= -Llib -lz
TRANSLATIONS += qop_zh-cn.ts

OTHER_FILES += \
    doc/TODO.txt \
    doc/history

win32 {
	QT += testlib
}
