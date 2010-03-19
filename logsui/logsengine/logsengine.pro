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

TARGET = logsengine
CONFIG += hb dll svg
RESOURCES += logsengine.qrc

INCLUDEPATH += ./
INCLUDEPATH += ./inc
INCLUDEPATH += ./logssymbianos
INCLUDEPATH += ./logssymbianos/inc
INCLUDEPATH += ../inc
INCLUDEPATH += ../logscntfinder/inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE


# Input
HEADERS += ../../contacts_plat/logs_engine_api/inc/logsabstractmodel.h
HEADERS += ../../contacts_plat/logs_engine_api/inc/logsmodel.h
HEADERS += ../../contacts_plat/logs_engine_api/inc/logsevent.h
HEADERS += ../../contacts_plat/logs_engine_api/inc/logsfilter.h
HEADERS += ../../contacts_plat/logs_engine_api/inc/logscustomfilter.h
HEADERS += inc/logsdetailsmodel.h
HEADERS += inc/logsmatchesmodel.h
HEADERS += inc/logsengdefs.h
HEADERS += inc/logscall.h
HEADERS += inc/logscontact.h
HEADERS += inc/logsmessage.h
HEADERS += inc/logseventdata.h
HEADERS += inc/logsthumbnailmanager.h
HEADERS += inc/logscommondata.h
HEADERS += logssymbianos/inc/logsdbconnector.h
HEADERS += logssymbianos/inc/logsreader.h
HEADERS += logssymbianos/inc/logsreaderstates.h
HEADERS += logssymbianos/inc/logsreaderstatecontext.h
HEADERS += logssymbianos/inc/logseventparser.h
HEADERS += logssymbianos/inc/logseventdataparser.h
HEADERS += logssymbianos/inc/logsremove.h

SOURCES += src/logsfilter.cpp
SOURCES += src/logsabstractmodel.cpp
SOURCES += src/logsmodel.cpp
SOURCES += src/logsdetailsmodel.cpp
SOURCES += src/logsmatchesmodel.cpp
SOURCES += src/logscall.cpp
SOURCES += src/logscontact.cpp
SOURCES += src/logsmessage.cpp
SOURCES += src/logsevent.cpp
SOURCES += src/logseventdata.cpp
SOURCES += src/logscustomfilter.cpp
SOURCES += src/logsthumbnailmanager.cpp
SOURCES += src/logscommondata.cpp
SOURCES += logssymbianos/src/logsdbconnector.cpp
SOURCES += logssymbianos/src/logsreader.cpp
SOURCES += logssymbianos/src/logsreaderstates.cpp
SOURCES += logssymbianos/src/logseventparser.cpp
SOURCES += logssymbianos/src/logseventdataparser.cpp
SOURCES += logssymbianos/src/logsremove.cpp

DEFINES += LOGSENGINE_LIB

libFiles.sources = logsengine.dll
libFiles.path = "!:/sys/bin"
DEPLOYMENT += libFiles

symbian: {
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x10282CE2

    TARGET.CAPABILITY = CAP_GENERAL_DLL
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lflogger -llogcli -llogwrap -lefsrv -lxqservice  -lqtcontacts -llogscntfinder -lthumbnailmanagerqt -lcentralrepository
    
    defFiles = "$${LITERAL_HASH}ifdef WINS" \
        "DEFFILE bwins/logsengine.def" \
        "$${LITERAL_HASH}else" \
        "DEFFILE eabi/logsengine.def" \
        "$${LITERAL_HASH}endif"
    MMP_RULES += defFiles
}
