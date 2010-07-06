#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: Homescreen Friend widget plugin 
#

TEMPLATE = lib
CONFIG += plugin hb mobility
MOBILITY = serviceframework contacts

TRANSLATIONS = common.ts
TRANSLATIONS += contactwidgethsplugin.ts

win32: PLUGIN_SUBDIR = /hsresources/import/widgetregistry/2002C353
symbian: PLUGIN_SUBDIR = /private/20022F35/import/widgetregistry/2002C353

symbian: LIBS += -lxqservice -lthumbnailmanagerqt

RESOURCES += ./contactwidgeths/contactwidgeths.qrc

DEPENDPATH += ./inc \
              ./src

INCLUDEPATH += ./inc

#Include MOC_DIR on symbian to the default include path
symbian {
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    INCLUDEPATH += $$MOC_DIR
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA=1
}

symbian: plugin { # copy qtstub and manifest

    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_SUBDIR

    DEPLOYMENT += pluginstub

    qtplugins.path = $$PLUGIN_SUBDIR
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin

    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin z:$$qtplugins.path/$$basename(qtplugin)"

    # copy manifest
    BLD_INF_RULES.prj_exports += "./resource/contactwidgethsplugin.manifest z:$$PLUGIN_SUBDIR/contactwidgethsplugin.manifest"
    # xml files for emulator, sisx and hw rom
    BLD_INF_RULES.prj_exports += "./resource/contactwidgethsplugin.xml z:$$PLUGIN_SUBDIR/contactwidgethsplugin.xml"
    # copy resources graphics png, svg
    BLD_INF_RULES.prj_exports += "./resource/qtg_large_friend.svg z:$$PLUGIN_SUBDIR/qtg_large_friend.svg"

    # copy iby file
    BLD_INF_RULES.prj_exports += "./rom/contactwidgeths.iby CORE_APP_LAYER_IBY_EXPORT_PATH( contactwidgeths.iby )"
}

symbian {
    TARGET.UID3 = 0x2002C353
    ICON = qtg_large_friend.svg
    # themable icon for application library
    SKINICON = qtg_large_friend
}

include(contactwidgethsplugin.pri)
