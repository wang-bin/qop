
OTHER_FILES += README

TEMPLATE	= app
TARGET		= qop
INCLUDEPATH	+= src
LIBS		+= #-Llib -lz
FORMS	=

#include(../config.pri)
#include(../i18n.pri)
include(libqop.pri)

HEADERS		+= option.h
SOURCES		+= main.cpp option.cpp


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
	sources.path = /opt/usr/$${TARGET}

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
