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

TEMPLATE = app
TARGET = 

QT += sql
QT += testlib
CONFIG  += qtestlib hb
DEFINES += PBK_UNIT_TEST \
           CNTHISTORYMODEL_NO_EXPORT

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
	
# Input
HEADERS += mt_cnthistorymodel.h \
           stub_classes.h \
           ../../inc/cnthistorymodelglobal.h \
           ../../inc/cnthistorymodel_p.h \
           ../../inc/cnthistorymodel.h
           
SOURCES += stub_classes.cpp \
           ../../src/cnthistorymodel.cpp \
           mt_cnthistorymodel.cpp
           
LIBS += -lQtContacts \
        -lmsghistory

TARGET.CAPABILITY = ALL -TCB
