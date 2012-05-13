TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = libqop qop tst_zprocess \
    test/tst_zprocess_gui

libqop.file = src/libqop.pro
qop.file = src/qop.pro
qop.depends += libqop

tst_zprocess.file = test/tst_zprocess.pro
tst_zprocess.depends += libqop


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


