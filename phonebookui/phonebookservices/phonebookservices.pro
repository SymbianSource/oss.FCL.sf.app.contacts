# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
# Initial Contributors:
# Nokia Corporation - initial contribution.
# Contributors:
# Description:

TEMPLATE = app
TARGET = phonebookservices
CONFIG += hb
CONFIG += service

DEPENDPATH += . \
    src

INCLUDEPATH +=  ../pbkcommonui/inc \
                ../inc
                
INCLUDEPATH += ../../phonebookengines/cntimageutility/inc

INCLUDEPATH += . \
    inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

MOC_DIR = moc

# Input
HEADERS += inc/cntservicemainwindow.h \
           inc/cntserviceviewmanager.h \
           inc/cntservicehandler.h \
           inc/cntservicecontactfetchview.h \
           inc/cntservicecontactselectionview.h \
           inc/cntserviceeditview.h \
           inc/cntservicecontactcardview.h \
           inc/cntservicesubeditview.h \
           inc/cntserviceassigncontactcardview.h \
           inc/cntserviceviewfactory.h 

SOURCES += src/main.cpp \
           src/cntservicemainwindow.cpp \
           src/cntserviceviewmanager.cpp \
           src/cntservicehandler.cpp \
           src/cntservicecontactfetchview.cpp \
           src/cntservicecontactselectionview.cpp \
           src/cntserviceeditview.cpp \
           src/cntservicecontactcardview.cpp \
           src/cntservicesubeditview.cpp \
           src/cntserviceassigncontactcardview.cpp \
           src/cntserviceviewfactory.cpp

# capability
TARGET.CAPABILITY = CAP_APPLICATION

TRANSLATIONS = contacts.ts

TARGET.UID3 = 0x2002429B

LIBS += -lxqservice \
        -lxqserviceutil \
        -lhbcore \
        -lqtcontacts \
        -lmobcntmodel \
        -lpbkcommonui \
        -lqtversit \
        -lcntimageutility

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden
