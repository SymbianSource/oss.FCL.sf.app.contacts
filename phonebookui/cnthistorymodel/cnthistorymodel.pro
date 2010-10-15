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

DEFINES += dll
DEFINES += BUILD_CNTHISTORYMODEL
    
MOC_DIR = moc

TARGET.CAPABILITY = CAP_GENERAL_DLL
TARGET.EPOCALLOWDLLDATA = 1
TARGET.UID3 = 0x2002E6EC

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../inc

HEADERS += \
    cnthistorymodelglobal.h \
    cnthistorymodelconsts.h \
    cnthistorymodel.h \
    cnthistorymodel_p.h 

SOURCES += \
    cnthistorymodel_p.cpp \
    cnthistorymodel.cpp
    
LIBS += -llogsengine \
        -lQtContacts \
        -lmsghistory

defBlock = \      
    "$${LITERAL_HASH}if defined(EABI)" \
        "DEFFILE  ../eabi/cnthistorymodel.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/cnthistorymodel.def" \
    "$${LITERAL_HASH}endif"

MMP_RULES += defBlock
symbian:MMP_RULES += SMPSAFE