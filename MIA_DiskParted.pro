QT -= gui
QT += core network

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        peticion.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    command/cat.h \
    command/chgrp.h \
    command/chmod.h \
    command/edit.h \
    command/fdisk.h \
    command/login.h \
    command/logout.h \
    command/loss.h \
    command/mkdir.h \
    command/mkdisk.h \
    command/mkfile.h \
    command/mkfs.h \
    command/mkgrp.h \
    command/mkusr.h \
    command/mount.h \
    command/mv.h \
    command/recovery.h \
    command/rem.h \
    command/ren.h \
    command/rep.h \
    command/rmdisk.h \
    command/rmgrp.h \
    command/rmusr.h \
    command/sync.h \
    command/unmount.h \
    fileManager/filesystem.h \
    fileManager/manager.h \
    fileManager/mpartition.h \
    interpreter.h \
    peticion.h \
    var/filename.h \
    var/globals.h \
    var/struct.h
