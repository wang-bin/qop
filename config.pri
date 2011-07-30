CONFIG		+= #ezx#static ezx
CONFIG += profile
#profiling, -pg is not supported for msvc
debug:!*msvc*:profile {
	QMAKE_CXXFLAGS_DEBUG += -pg
	QMAKE_LFLAGS_DEBUG += -pg
}

#$$[TARGET_PLATFORM]
#$$[QT_ARCH] #windows symbian windowsce arm
PLATFORM_EXT =
ARCH_EXT =
TOOLCHAIN_EXT =
unix {
  PLATFORM_EXT = _unix
  *linux*: PLATFORM_EXT = _linux
  maemo*: PLATFORM_EXT = _maemo
} else:win32 {
  PLATFORM_EXT = _win32
} else:macx {
  PLATFORM_EXT = _macx
}

ezx {
  PLATFORM_EXT = _ezx
  DEFINES        += CONFIG_EZX
  QMAKE_CXXFLAGS.ARMCC +=
}

#*arm*: ARCH_EXT = $${ARCH_EXT}_arm
#isEqual(QT_ARCH, arm) {
contains(QT_ARCH, arm.*) {
  ARCH_EXT = $${ARCH_EXT}_$${QT_ARCH}
}
*64:   ARCH_EXT = $${ARCH_EXT}_x64

*llvm*: TOOLCHAIN_EXT = _llvm
#*msvc*:

DESTDIR	= bin
TARGET	= $${TARGET}$${PLATFORM_EXT}$${ARCH_EXT}$${TOOLCHAIN_EXT}
OBJECTS_DIR = .obj/$${PLATFORM_EXT}$${ARCH_EXT}$${TOOLCHAIN_EXT}
MOC_DIR = .moc/$$[QT_VERSION]
RCC_DIR = .rcc/$$[QT_VERSION]
UI_DIR  = .ui/$$[QT_VERSION]
