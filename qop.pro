TEMPLATE	= app
TARGET		= qop
INCLUDEPATH	+= src
LIBS		+= #-Llib -lz
FORMS	=

include(config.pri)
include(i18n.pri)

HEADERS		+= src/QOutParser.h \
		src/gui/ezprogressdialog.h \
		src/gui/ezprogressdialog_p.h \
		src/qcounterthread.h \
		src/qarchive/arcreader.h \
		src/option.h \
		src/Types.h \
		src/qarchive/qarchive.h \
		src/qarchive/tar/qtar.h \
		src/qarchive/tar/TarItem.h \
		src/qarchive/tar/TarHeader.h \
		src/qarchive/zip/ZipHeader.h \
		src/qarchive/gzip/GzipHeader.h \
		src/qarchive/gzip/GzipItem.h \
		src/qop.h \
		src/commandparser.h \
		src/version.h \
		src/qarchive/qarchive_p.h \
		src/msgdef.h \
		src/qtcompat.h \
		src/utils/util.h \
		src/utils/strutil.h \
		src/utils/convert.h \
		src/utils/qt_util.h

SOURCES		+= src/QOutParser.cpp \
		src/gui/ezprogressdialog.cpp \
		src/main.cpp \
		src/qcounterthread.cpp \
		src/qarchive/arcreader.cpp \
		src/option.cpp \
		src/qarchive/qarchive.cpp \
		src/qarchive/tar/qtar.cpp \
		src/qarchive/zip/ZipHeader.cpp \
		src/qarchive/gzip/GzipItem.cpp \
		src/qop.cpp \
		src/commandparser.cpp \
		src/utils/util.cpp \
		src/utils/strutil.cpp \
		src/utils/convert.cpp \
		src/utils/qt_util.cpp


greaterThan(QT_MAJOR_VERSION, 3) {
	HEADERS += src/zprocess.h src/zprocess_p.h
	SOURCES += src/zprocess.cpp
} else {
	#DEFINES += QT_NO_PROCESS
}

!*g++* {
	HEADERS += src/getopt.h
	SOURCES += src/getopt.cpp
}


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
		config.in \
		qtc_packaging/debian_harmattan/rules \
		qtc_packaging/debian_harmattan/README \
		qtc_packaging/debian_harmattan/copyright \
		qtc_packaging/debian_harmattan/control \
		qtc_packaging/debian_harmattan/compat \
		qtc_packaging/debian_harmattan/changelog \
				README

unix:!symbian {
	maemo* {
		target.path = /opt/usr/bin
		DEFINES		 += CONFIG_MAEMO
	} else {
		target.path = /usr/local/bin
	}
	INSTALLS += target
}


unix:maemo* {
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

DISTFILES = $${HEADERS} $${SOURCES} $${TRANSLATIONS} $${OTHER_FILES}

unix:!symbian:!maemo5 {
	target.path = /opt/qop/bin
	INSTALLS += target
}


