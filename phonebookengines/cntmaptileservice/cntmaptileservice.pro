#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).


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
TARGET = cntmaptileservice   

CONFIG += dll
CONFIG += hb
CONFIG += mobility
MOBILITY = publishsubscribe

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc


MOC_DIR = moc

DEFINES += CNTMAPTILESERVICEDLL

INTERNAL_PUBLIC_HEADERS +=  inc/cntmaptileservice.h        
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE 
# Input
HEADERS +=  $$INTERNAL_PUBLIC_HEADERS \
           ./inc/cntmaptiledblookuptable.h  
               
    
SOURCES += ./src/cntmaptileservice.cpp ./src/cntmaptiledblookuptable.cpp

symbian: 
{

    :BLD_INF_RULES.prj_exports += "conf/cntmaptileservice.confml APP_LAYER_CONFML(cntmaptileservice.confml)"
    :BLD_INF_RULES.prj_exports += "conf/cntmaptileservice_2002C3A8.crml APP_LAYER_CRML(cntmaptileservice_2002C3A8.crml)"
    :BLD_INF_RULES.prj_exports += "conf/2002C3A8.txt /epoc32/winscw/c/private/10202be9/2002C3A8.txt"
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x2002C3A8

    
   LIBS += -lcntmodel \      
           -ledbms  \
           -lbafl \
           -lcentralrepository \
           -leuser \
           -lefsrv
   
        myCrml.sources = ./conf/cntmaptilepublisher.qcrml
        myCrml.path = c:/resource/qt/crml
        DEPLOYMENT += myCrml

}  

deploy.path = /
headers.sources = $$INTERNAL_PUBLIC_HEADERS
headers.path = epoc32/include/app
DEPLOYMENT += exportheaders

# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$headers.path/$$basename(header)"
defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/cntmaptileservice.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/cntmaptileservice.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock
