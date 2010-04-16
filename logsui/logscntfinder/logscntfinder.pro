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

TEMPLATE = lib

TARGET = logscntfinder
CONFIG += dll
CONFIG += hb

INCLUDEPATH += ./
INCLUDEPATH += ./inc
INCLUDEPATH += ../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE


# Input
HEADERS += inc/logscntfinder.h
HEADERS += inc/logspredictivetranslator.h

# Input
SOURCES += src/logscntfinder.cpp
SOURCES += src/logspredictivetranslator.cpp

# stub Input. comment out from real lib
#SOURCES += tsrc/stubs/qtcontacts_stubs.cpp

DEFINES += LOGSCNTFINDER_LIB

libFiles.sources = logscntfinder.dll
libFiles.path = "!:/sys/bin"
DEPLOYMENT += libFiles

symbian: {
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x10282CE3

    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lflogger -lqtcontacts
    defFiles = "$${LITERAL_HASH}ifdef WINS" \
        "DEFFILE bwins/logscntfinder.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE eabi/logscntfinder.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defFiles
}
