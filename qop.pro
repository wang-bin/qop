TEMPLATE	= app
#CONFIG		= qt warn_on release #for EZX
CONFIG		+= #ezx#static ezx
CONFIG		+= profile
DESTDIR		= bin
MOC_DIR		= .moc
OBJECTS_DIR	= .obj
DEFINES		+= QT_THREAD_SUPPORT

#profiling, -pg is not supported for msvc
debug:profile {
	QMAKE_CXXFLAGS_DEBUG += -pg
	QMAKE_LFLAGS_DEBUG += -pg
}

HEADERS		= src/QOutParser.h \
		src/gui/ezprogressdialog.h \
		src/gui/ezprogressdialog_p.h \
		src/global.h \
		src/qcounterthread.h \
		src/util.h \
		src/qarchive/arcreader.h \
		src/option.h \
		src/qany.h \
		src/Types.h \
		src/qarchive/qarchive.h \
		src/qarchive/tar/qtar.h \
		src/qarchive/tar/TarItem.h \
		src/qarchive/tar/TarHeader.h \
		src/qarchive/zip/ZipHeader.h \
		src/qarchive/gzip/GzipHeader.h \
		src/qarchive/gzip/GzipItem.h \
		src/Compress/BitlDecoder.h \
		src/qop.h \
		src/commandparser.h \
		src/version.h \
		src/qarchive/qarchive_p.h \
		src/msgdef.h

SOURCES		= src/QOutParser.cpp \
		src/gui/ezprogressdialog.cpp \
		src/main.cpp \
		src/qcounterthread.cpp \
		src/qarchive/arcreader.cpp \
		src/util.cpp \
		src/option.cpp \
		src/qarchive/qarchive.cpp \
		src/qarchive/tar/qtar.cpp \
		src/qarchive/zip/ZipHeader.cpp \
		src/qarchive/gzip/GzipItem.cpp \
		src/Compress/BitlDecoder.cpp \
		src/qop.cpp \
		src/commandparser.cpp

win32 {
	HEADERS += src/getopt.h
	SOURCES += src/getopt.cpp
}

INCLUDEPATH	+= src
LIBS		+= #-Llib -lz
TRANSLATIONS+= i18n/qop_zh-cn.ts

OTHER_FILES	+= \
		doc/TODO.txt \
		doc/history \
		doc/help-zh_CN.txt \
		doc/help-en_US.txt \
		test/zip-qop.sh \
		test/unzip-qop.sh \
		test/unrar-zip.sh \
		test/tar-qop.sh \
		test/qop-zip.sh \
		test/qop-unzip.sh \
		test/qop-unrar.sh \
		test/qop-tar.sh \
		test/7z-qop.sh \
		test/.findqop.sh \
		configure \
		config.in

FORMS	=

CONFIG(ezx) {
	 TARGET =	qop-ezx
	 MOC_DIR	= .moc/ezx
	 OBJECTS_DIR	= .obj/ezx
	 DEFINES        += CONFIG_EZX
	 QMAKE_CXXFLAGS.ARMCC +=
 } else {
	 TARGET = qop
	 unix: {
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
		#DESTDIR		= package
		MOC_DIR		= .moc/maemo5
		OBJECTS_DIR	= .obj/maemo5
		DEFINES         += CONFIG_MAEMO
		target.path = /opt/usr/bin
	} else {
		target.path = /usr/local/bin
	}
	INSTALLS += target
}


unix:maemo5 {
	isEmpty(PREFIX) {
	PREFIX = /opt/usr
	}
	BINDIR = $$PREFIX/bin
	DATADIR =$$PREFIX/share

	DEFINES += DATADIR=\\\"$$DATADIR\\\" PKGDATADIR=\\\"$$PKGDATADIR\\\"
# install
	INSTALLS += target desktop service iconxpm icon26 icon48 icon64 sources

	target.files = bin/$$TARGET i18n
	target.path = $$BINDIR

	desktop.path  = /usr/share/applications/hildon
	desktop.files  = data/$${TARGET}.desktop

	service.path  = /usr/share/dbus-1/services
	service.files  = data/$${TARGET}.service

	icon64.path  = $$DATADIR/icons/hicolor/64x64/apps
	icon64.files  = data/64x64/$${TARGET}.png

	sources.files = $$HEADERS $$SOURCES $$FORMS $$RESOURCES $${TARGET}.pro *.ico *.icns *.rc *.plist
	sources.path = /opt/usr/src/$${TARGET}

#
# Targets for debian source and binary package creation
#
	debian-src.commands = dpkg-buildpackage -S -r -us -uc -d
	debian-bin.commands = dpkg-buildpackage -b -r -uc -d
	debian-all.depends = debian-src debian-bin

#
# Clean all but Makefile
#
	compiler_clean.commands = -$(DEL_FILE) $(TARGET)

	QMAKE_EXTRA_TARGETS += debian-all debian-src debian-bin compiler_clean
}
