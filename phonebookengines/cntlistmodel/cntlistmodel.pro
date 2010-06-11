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
TARGET = cntlistmodel

DEFINES += dll \
    BUILD_CNTLISTMODEL 
    
MOC_DIR = moc

CONFIG += hb

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x20026FC3

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += inc
INCLUDEPATH += ../inc


INTERNAL_PUBLIC_HEADERS += \
        inc/cntlistmodelglobal.h \
        inc/cntlistmodel.h    

HEADERS += $$INTERNAL_PUBLIC_HEADERS \
           inc/cntlistmodel_p.h \
           inc/cntcache.h \
           inc/cntcache_p.h \
           inc/cntinfoprovider.h \
           inc/cntdefaultinfoprovider.h

SOURCES += src/cntlistmodel.cpp \
           src/cntcache.cpp \
           src/cntcache_p.cpp \
           src/cntdefaultinfoprovider.cpp
    
LIBS += -lQtContacts \
        -lhbcore \
        -lthumbnailmanagerqt

deploy.path = /
headers.sources = $$INTERNAL_PUBLIC_HEADERS
headers.path = epoc32/include/app #change this to internal folder
DEPLOYMENT += exportheaders

# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$headers.path/$$basename(header)"



