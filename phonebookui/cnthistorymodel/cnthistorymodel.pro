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
TARGET = cnthistorymodel
CONFIG += hb

DEFINES += dll \
    BUILD_CNTHISTORYMODEL
    
MOC_DIR = moc

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002E6EC

QT += sql

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += inc

INTERNAL_PUBLIC_HEADERS += \
    inc/cnthistorymodelglobal.h \
    inc/cnthistorymodel.h        

HEADERS += \
    $$INTERNAL_PUBLIC_HEADERS \
    inc/cnthistorymodel_p.h 

SOURCES += src/cnthistorymodel.cpp
    
LIBS += -llogsengine \
        -lQtContacts \
        -lmsghistory
                 
deploy.path = /
headers.sources = $$INTERNAL_PUBLIC_HEADERS
headers.path = epoc32/include/app
DEPLOYMENT += exportheaders

# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$headers.path/$$basename(header)"
defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/cnthistorymodel.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/cnthistorymodel.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock
