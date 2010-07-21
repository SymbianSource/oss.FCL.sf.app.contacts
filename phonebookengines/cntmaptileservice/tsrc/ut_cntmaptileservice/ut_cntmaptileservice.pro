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
QT += testlib sql xml

CONFIG += hb
CONFIG += mobility
MOBILITY = publishsubscribe

DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
DEFINES += CNTMAPTILESERVICEDLL
DEFINES += CNTMAPTILESERVICE_UNIT_TEST

INCLUDEPATH += inc
INCLUDEPATH += ../inc
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

symbian:
{
    :BLD_INF_RULES.prj_exports += "../../../cntmaptileservice/conf/cntmaptileservice.confml APP_LAYER_CONFML(cntmaptileservice.confml)"
    :BLD_INF_RULES.prj_exports += "../../../cntmaptileservice/conf/cntmaptileservice_2002C3A8.crml APP_LAYER_CRML(cntmaptileservice_2002C3A8.crml)"
    :BLD_INF_RULES.prj_exports += "../../../cntmaptileservice/conf/2002C3A8.txt /epoc32/winscw/c/private/10202be9/2002C3A8.txt"
 
    :BLD_INF_RULES.prj_exports += "data/preferredaddressmap.png /epoc32/winscw/c/maptiletest/preferredaddressmap.png"
    :BLD_INF_RULES.prj_exports += "data/workaddressmap.png /epoc32/winscw/c/maptiletest/workaddressmap.png"
    :BLD_INF_RULES.prj_exports += "data/homeaddressmap.png /epoc32/winscw/c/maptiletest/homeaddressmap.png"
}

SOURCES += src/testrunner.cpp
SOURCES += src/main.cpp
SOURCES += src/ut_cntmaptileservice.cpp

SOURCES += ../../src/cntmaptileservice.cpp
SOURCES += ../../src/cntmaptiledblookuptable.cpp

# Input
HEADERS += inc/testrunner.h
HEADERS += inc/ut_cntmaptileservice.h

HEADERS += ../../inc/cntmaptileservice.h
HEADERS += ../../inc/cntmaptiledblookuptable.h


LIBS += -lQtContacts \
        -lcntmodel \      
        -ledbms  \
        -lbafl \
        -lcentralrepository \
        -leuser

myCrml.sources = ../../../cntmaptileservice/conf/cntmaptilepublisher.qcrml
        myCrml.path = c:/resource/qt/crml
        DEPLOYMENT += myCrml


TARGET.CAPABILITY = ALL -TCB    

