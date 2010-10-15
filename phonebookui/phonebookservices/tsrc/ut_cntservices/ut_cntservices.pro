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

QT += testlib sql xml core
CONFIG += hb symbian_test
HB += hbfeedback
DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
DEFINES += PBK_UNIT_TEST

MOC_DIR = moc

INCLUDEPATH += ../../../../../inc
INCLUDEPATH += ../../../../../phonebookui/phonebookapp/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 

INCLUDEPATH +=  ../../../cntcommonui/core \
                ../../../cntcommonui/engine_inc \
                ../../../inc
INCLUDEPATH +=  ../../../../phonebookengines/cntimageutility/inc


# contacts/inc/
HEADERS += ../../../../inc

#=======================================================================
# phonebookservices being unit tested

HEADERS += ../../inc/cntabstractserviceprovider.h
HEADERS += ../../inc/cntservices.h
SOURCES += ../../src/cntservices.cpp


#=======================================================================
# test utilities

HEADERS += ../../../cntcommonui/tsrc/testutility/testrunner.h
SOURCES += ../../../cntcommonui/tsrc/testutility/testrunner.cpp

HEADERS += ../../../cntcommonui/tsrc/testutility/testengine.h
SOURCES += ../../../cntcommonui/tsrc/testutility/testengine.cpp    

HEADERS += ../../../cntcommonui/tsrc/testutility/testviewmanager.h
SOURCES += ../../../cntcommonui/tsrc/testutility/testviewmanager.cpp

#HEADERS += ../../../cntcommonui/tsrc/testutility/hbstubs_helper.h
#SOURCES += ../../../cntcommonui/tsrc/testutility/hbstubs.cpp

#SOURCES += ../../../cntcommonui/tsrc/testutility/qthighway_stub.cpp
#qthighway_stub_helper.h

#=======================================================================
# Dependencies from cntcommonui which are needed

HEADERS += ../../../cntcommonui/core/cntextensionmanager.h
SOURCES += ../../../cntcommonui/core/cntextensionmanager.cpp

HEADERS += ../../../cntcommonui/core/cntthumbnailmanager.h
SOURCES += ../../../cntcommonui/core/cntthumbnailmanager.cpp

#=======================================================================

# Tester sources
HEADERS += ./*.h
SOURCES += ./*.cpp



# capability
TARGET.CAPABILITY = ALL -TCB
    
TRANSLATIONS = contacts.ts

LIBS += -lhbcore \
        -lqtcontacts \
        -lcntlistmodel \
        -lcntcommonui \
        -lqtversit \
        -lcntimageutility \
	    -lxqkeycapture \
        -lxqservice \
        -lxqserviceutil
symbian:MMP_RULES += SMPSAFE