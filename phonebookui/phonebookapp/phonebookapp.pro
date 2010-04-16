#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).

#
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
#



TEMPLATE = app
TARGET = phonebook

ICON = resources/phonebook.svg

#DEFINES += BUILD_QTCONTACTS

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
INCLUDEPATH += ../pbkcommonui/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

MOC_DIR = moc

CONFIG += hb
CONFIG += service

LIBS += -lhbcore \
        -lpbkcommonui \
        -lxqservice \
        -lxqserviceutil
        
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable

# Input
HEADERS += inc/cntinterface.h
HEADERS += inc/cntappservicehandler.h

SOURCES += src/main.cpp
SOURCES += src/cntappservicehandler.cpp

# capability
TARGET.CAPABILITY = ALL \
    -TCB

TRANSLATIONS = contacts.ts
    
symbian: {
        # Skip the UID2/3 thing
        TARGET.UID3 = 0x20022EF9
        TARGET.EPOCSTACKSIZE = 0x14000
        TARGET.EPOCHEAPSIZE = 0x1000 0xA00000 

}