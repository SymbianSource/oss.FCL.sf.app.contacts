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
#

TEMPLATE = app
TARGET = 

QT += testlib xml
CONFIG  += qtestlib
CONFIG  += hb
HB = hbcore hbinput 

DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += ../../
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../../tsrc/qtestutils/inc

DEFINES += QT_NO_DEBUG_OUTPUT

# Input
HEADERS += inc/ut_logscntfinder.h
HEADERS += ../../inc/logscntfinder.h
HEADERS += inc/ut_logspredictivetranslator.h
HEADERS += ../../inc/logspredictivetranslator.h
HEADERS += inc/ut_logscntentry.h

SOURCES += src/main.cpp
SOURCES += src/ut_logscntfinder.cpp 
SOURCES += src/ut_logspredictivetranslator.cpp
SOURCES += src/ut_logscntentry.cpp
SOURCES += ../../src/logscntfinder.cpp
SOURCES += ../../src/logspredictivetranslator.cpp
SOURCES += ../../../tsrc/qtestutils/src/testresultxmlparser.cpp
SOURCES += ../stubs/qtcontacts_stubs.cpp

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEfa329b2
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lflogger -lqtcontacts
    
}
