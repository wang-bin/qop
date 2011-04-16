TEMPLATE	= app
#CONFIG		= qt warn_on release #for EZX
#CONFIG		+= static #ezx
DESTDIR		= bin
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
	Compress/BitlDecoder.h \
    qop.h \
    commandparser.h

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
	Compress/BitlDecoder.cpp \
    qop.cpp \
    commandparser.cpp

INCLUDEPATH	+= .
LIBS	+= -Llib -lz
TRANSLATIONS += qop_zh-cn.ts

OTHER_FILES += \
    doc/TODO.txt \
    doc/history \
    doc/help-zh_CN.txt \
    doc/help-en_US.txt


INTERFACES	=

CONFIG(ezx) {
	 TARGET =	qop-ezx
	 MOC_DIR	= .moc/ezx
	 OBJECTS_DIR	= .obj/ezx
	 DEFINES        += CONFIG_EZX
	 QMAKE_CXXFLAGS.ARMCC +=
 } else {
	 TARGET = qop
	 unix:  {
			MOC_DIR		= .moc/unix
			OBJECTS_DIR	= .obj/unix
	 }
	 win32  {
			MOC_DIR		= .moc/win32
			OBJECTS_DIR	= .obj/win32
	 }
 }

unix:!symbian {
	maemo5 {
		TARGET = qop-maemo5
		MOC_DIR		= .moc/maemo5
		OBJECTS_DIR	= .obj/maemo5
		DEFINES         += CONFIG_MAEMO
		target.path = /opt/usr/bin
	} else {
		target.path = /usr/local/bin
	}
	INSTALLS += target
}

message(Qt version: $$[QT_VERSION])
message(Qt is installed in $$[QT_INSTALL_PREFIX])
message(Qt resources can be found in the following locations:)
message(Documentation: $$[QT_INSTALL_DOCS])
message(Header files: $$[QT_INSTALL_HEADERS])
message(Libraries: $$[QT_INSTALL_LIBS])
message(Binary files (executables): $$[QT_INSTALL_BINS])
message(Plugins: $$[QT_INSTALL_PLUGINS])
message(Data files: $$[QT_INSTALL_DATA])
message(Translation files: $$[QT_INSTALL_TRANSLATIONS])
message(Settings: $$[QT_INSTALL_SETTINGS])
message(Examples: $$[QT_INSTALL_EXAMPLES])
message(Demonstrations: $$[QT_INSTALL_DEMOS])
