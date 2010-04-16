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

DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
DEFINES += MOBCNTMODEL_NO_EXPORT
    
MOC_DIR = moc

QT += testlib xml

CONFIG += hb

TARGET.CAPABILITY = ALL \
    -TCB

INCLUDEPATH += .
INCLUDEPATH += ../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
    
HEADERS += inc/testrunner.h
HEADERS += inc/ut_mobcnticonmanager.h
HEADERS += inc/ut_mobcntmodel.h
HEADERS += ../../inc/mobcntmodelglobal.h \
           ../../inc/mobcntmodel.h \
           ../../inc/mobcntmodel_p.h \
           ../../inc/mobcnticonmanager.h

SOURCES += src/testrunner.cpp
SOURCES += src/main.cpp
SOURCES += src/ut_mobcnticonmanager.cpp
SOURCES += src/ut_mobcntmodel.cpp
SOURCES += ../../src/mobcntmodel.cpp \
           ../../src/mobcnticonmanager.cpp

RESOURCES += ../../resources/mobcntmodel.qrc

BLD_INF_RULES.prj_exports += "image1.png /epoc32/winscw/c/data/images/"
BLD_INF_RULES.prj_exports += "image2.png /epoc32/winscw/c/data/images/"
    
LIBS += -lQtContacts \
        -lhbcore \
        -lthumbnailmanagerqt

