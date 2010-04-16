######################################################################
#
# Mobility API project - plugins
#
######################################################################

include(../pathconfig.pri)
include($$QT_MOBILITY_BUILD_TREE/config.pri)

TEMPLATE = subdirs

contains(mobility_modules,contacts): SUBDIRS += contacts
contains(mobility_modules,multimedia): SUBDIRS += multimedia
contains(mobility_modules,sensors): SUBDIRS += sensors