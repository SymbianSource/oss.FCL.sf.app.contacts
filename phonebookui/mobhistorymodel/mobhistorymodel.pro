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


TEMPLATE = lib
TARGET = mobhistorymodel
CONFIG += hb

DEFINES += dll \
    BUILD_MOBHISTORYMODEL
    
MOC_DIR = moc

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1

QT += sql

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += inc
INCLUDEPATH += ../inc
INCLUDEPATH += ../../logsui/logsengine/inc
#INCLUDEPATH += /sf/app/messaging/msg_plat/msg_conversation_model_api/inc


INTERNAL_PUBLIC_HEADERS += \
    inc/mobhistorymodelglobal.h \
    inc/mobhistorymodel.h        

HEADERS += \
    $$INTERNAL_PUBLIC_HEADERS \
    inc/mobhistorymodel_p.h 

SOURCES += src/mobhistorymodel.cpp
    
LIBS += -llogsengine \
        -lQtContacts
                 
deploy.path = /
headers.sources = $$INTERNAL_PUBLIC_HEADERS
headers.path = epoc32/include/app
DEPLOYMENT += exportheaders

# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$headers.path/$$basename(header)"
