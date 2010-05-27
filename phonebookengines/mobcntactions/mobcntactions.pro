#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: 
#

TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(mobcntactionsplugin)
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS += \
    inc/mobcntactionfactory.h \
    inc/mobcntaction.h \
    inc/mobcntmessageaction.h \
    inc/mobcntvideocallaction.h \
    inc/mobcntcallaction.h \
    inc/mobcntemailaction.h \
    inc/mobcntbrowseraction.h
    
SOURCES += \
    src/mobcntactionfactory.cpp \
    src/mobcntaction.cpp \
    src/mobcntmessageaction.cpp \
    src/mobcntvideocallaction.cpp \
    src/mobcntcallaction.cpp \
    src/mobcntemailaction.cpp \
    src/mobcntbrowseraction.cpp

MOC_DIR = moc

symbian:
 { 
    load(data_caging_paths)
    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.UID3 = 0x20027012
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    LIBS += -lQtContacts \
        -lxqservice
    target.path = /sys/bin
    INSTALLS += target
    symbianplugin.sources = $${TARGET}.dll
    symbianplugin.path = /resource/qt/plugins/contacts
    DEPLOYMENT += symbianplugin
}
