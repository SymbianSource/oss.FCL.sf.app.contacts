include(../pathconfig.pri)
include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs
SUBDIRS += auto

contains(mobility_modules,serviceframework): SUBDIRS += testservice2 sampleserviceplugin sampleserviceplugin2
contains(mobility_modules,bearer) {
    contains(QT_CONFIG,dbus) {
        !symbian:!mac:!maemo6:!maemo5:unix:SUBDIRS += networkmanager
    }
    symbian:SUBDIRS += bearerex
}

symbian {
    contains(mobility_modules,messaging): SUBDIRS += messagingex
    contains(mobility_modules,publishsubscribe): SUBDIRS += publishsubscribeex
}
