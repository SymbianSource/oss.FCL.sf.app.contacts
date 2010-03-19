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
CONFIG += hb

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../symbianos_stub
INCLUDEPATH += ../hbstubs
INCLUDEPATH += ../stubs
INCLUDEPATH += /orbit/include # This must be included after the HB stubs
INCLUDEPATH += /epoc32/include
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += /epoc32/include/ecom
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../logssymbianos/inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += ../../../tsrc/qtestutils/inc
#INCLUDEPATH += ../../logssymbianos/tsrc/stubs

DEFINES += QT_NO_DEBUG_OUTPUT

# Input
HEADERS += inc/ut_logsmodel.h
HEADERS += inc/ut_logsdetailsmodel.h
HEADERS += inc/ut_logsmatchesmodel.h
HEADERS += inc/ut_logsfilter.h
HEADERS += inc/ut_logscustomfilter.h
HEADERS += inc/ut_logscall.h
HEADERS += inc/ut_logscontact.h
HEADERS += inc/ut_logsmessage.h
HEADERS += inc/ut_logsevent.h
HEADERS += inc/ut_logseventdata.h
HEADERS += inc/ut_logsthumbnailmanager.h
HEADERS += inc/ut_logscommondata.h
HEADERS += ../../../../contacts_plat/logs_engine_api/inc/logsabstractmodel.h
HEADERS += ../../../../contacts_plat/logs_engine_api/inc/logsmodel.h
HEADERS += ../../inc/logsdetailsmodel.h
HEADERS += ../../inc/logsmatchesmodel.h
HEADERS += ../../../../contacts_plat/logs_engine_api/inc/logsfilter.h
HEADERS += ../../../../contacts_plat/logs_engine_api/inc/logscustomfilter.h
HEADERS += ../../inc/logscall.h
HEADERS += ../../inc/logscontact.h
HEADERS += ../../inc/logsmessage.h
HEADERS += ../../../../contacts_plat/logs_engine_api/inc/logsevent.h
HEADERS += ../../inc/logseventdata.h
HEADERS += ../../inc/logsthumbnailmanager.h
HEADERS += ../../inc/logscommondata.h
HEADERS += ../../logssymbianos/inc/logseventparser.h
HEADERS += ../../logssymbianos/inc/logseventdataparser.h
HEADERS += ../../logssymbianos/inc/logsdbconnector.h
HEADERS += ../../logssymbianos/inc/logsremove.h
HEADERS += ../stubs/logscntfinder.h

SOURCES += src/main.cpp
SOURCES += src/ut_logsmodel.cpp 
SOURCES += src/ut_logsdetailsmodel.cpp
SOURCES += src/ut_logsmatchesmodel.cpp 
SOURCES += src/ut_logsfilter.cpp
SOURCES += src/ut_logscustomfilter.cpp
SOURCES += src/ut_logscall.cpp
SOURCES += src/ut_logscontact.cpp
SOURCES += src/ut_logsmessage.cpp
SOURCES += src/ut_logsevent.cpp
SOURCES += src/ut_logseventdata.cpp
SOURCES += src/ut_logsthumbnailmanager.cpp
SOURCES += src/ut_logscommondata.cpp
SOURCES += ../../src/logsabstractmodel.cpp
SOURCES += ../../src/logsmodel.cpp
SOURCES += ../../src/logsdetailsmodel.cpp
SOURCES += ../../src/logsmatchesmodel.cpp
SOURCES += ../../src/logsfilter.cpp
SOURCES += ../../src/logscustomfilter.cpp
SOURCES += ../../src/logscall.cpp
SOURCES += ../../src/logscontact.cpp
SOURCES += ../../src/logsmessage.cpp
SOURCES += ../../src/logsevent.cpp
SOURCES += ../../src/logseventdata.cpp
SOURCES += ../../src/logsthumbnailmanager.cpp
SOURCES += ../../src/logscommondata.cpp
SOURCES += ../hbstubs/hbstubs.cpp
SOURCES += ../hbstubs/qiconstubs.cpp
SOURCES += ../symbianos_stub/logsdbconnector_stub.cpp
SOURCES += ../symbianos_stub/logseventparser_stub.cpp
SOURCES += ../symbianos_stub/logseventdataparser_stub.cpp
SOURCES += ../../../tsrc/qtestutils/src/testresultxmlparser.cpp
SOURCES += ../stubs/qthighway_stub.cpp
SOURCES += ../stubs/logscntfinder_stub.cpp
SOURCES += ../../logssymbianos/tsrc/stubs/logclient_stubs.cpp
SOURCES += ../../logssymbianos/tsrc/stubs/qtcontacts_stubs.cpp

symbian: {
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0xEfa329b2
    TARGET.CAPABILITY = ALL -TCB
    TARGET.EPOCALLOWDLLDATA = 1
    LIBS += -lxqservice -lqtcontacts -lthumbnailmanagerqt
}
