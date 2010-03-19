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

QT += testlib sql xml
CONFIG += hb
HB += hbcore hbwidgets hbtools hbfeedback
DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_NO_WARNING_OUTPUT
DEFINES += PBK_UNIT_TEST
RESOURCES += ../../resources/pbkcommonui.qrc

MOC_DIR = moc

INCLUDEPATH += .
INCLUDEPATH += stubs       #must be before orbit includes
INCLUDEPATH += ../../
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../../inc
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
HEADERS += inc/testrunner.h
HEADERS += inc/t_cntactions.h
HEADERS += inc/t_cntaddresseditorview.h
HEADERS += inc/t_cntbasedetaileditorview.h
HEADERS += inc/t_cntbaselistview.h
HEADERS += inc/t_cntbaseselectionview.h
HEADERS += inc/t_cntbaseview.h
HEADERS += inc/t_cntcollectionlistmodel.h
HEADERS += inc/t_cntcollectionview.h
HEADERS += inc/t_cntcommands.h
HEADERS += inc/t_cntcompanyeditorview.h
HEADERS += inc/t_cntcontactcarddatacontainer.h
HEADERS += inc/t_cntcontactcarddetailitem.h
HEADERS += inc/t_cntcontactcardheadingitem.h
HEADERS += inc/t_cntcontactcardview.h
HEADERS += inc/t_cntdateeditorview.h
HEADERS += inc/t_cntdetailpopup.h
HEADERS += inc/t_cnteditordatamodelitem.h
HEADERS += inc/t_cnteditordataviewitem.h
HEADERS += inc/t_cnteditviewdetailitem.h
HEADERS += inc/t_cnteditviewheadingitem.h
HEADERS += inc/t_cntemaileditorview.h
HEADERS += inc/t_cntfamilydetaileditorview.h
HEADERS += inc/t_cntfavoritesselectionview.h
HEADERS += inc/t_cntfavoritesview.h
HEADERS += inc/t_cntgroupeditorview.h
HEADERS += inc/t_cntgroupactionsview.h
HEADERS += inc/t_cntgroupmemberview.h
HEADERS += inc/t_cntimageeditorview.h
HEADERS += inc/t_cntmainwindow.h
HEADERS += inc/t_cntmodelprovider.h
HEADERS += inc/t_cntmycardselectionview.h
HEADERS += inc/t_cntmycardview.h
HEADERS += inc/t_cntnameseditorview.h
HEADERS += inc/t_cntnamesview.h
HEADERS += inc/t_cntnoteeditorview.h
HEADERS += inc/t_cntonlineaccounteditorview.h
HEADERS += inc/t_cntphonenumbereditorview.h
HEADERS += inc/t_cntsnapshotwidget.h
HEADERS += inc/t_cnturleditorview.h
HEADERS += inc/t_cntviewmanager.h
HEADERS += inc/t_cntviewparameters.h
HEADERS += inc/t_cntcommhistoryview.h
HEADERS += inc/t_cntgroupselectionpopup.h
HEADERS += ../../inc/cntactions.h \
    ../../inc/cntbaselistview.h \
    ../../inc/cntbaseview.h \
    ../../inc/cntbasedetaileditorview.h \
    ../../inc/cntbaseselectionview.h \
    ../../inc/cntcollectionview.h \
    ../../inc/cntcollectionlistmodel.h \
    ../../inc/cntcommands.h \
    ../../inc/cntdetailpopup.h \
    ../../inc/cnteditordataviewitem.h \
    ../../inc/cnteditordatamodelitem.h \
    ../../inc/cntemaileditorview.h \
    ../../inc/cntnameseditorview.h \
    ../../inc/cnturleditorview.h \
    ../../inc/cntcompanyeditorview.h \
    ../../inc/cntphonenumbereditorview.h \
    ../../inc/cntaddresseditorview.h \
    ../../inc/cntonlineaccounteditorview.h \
    ../../inc/cntnoteeditorview.h \
    ../../inc/cntfamilydetaileditorview.h \
    ../../inc/cntdateeditorview.h \
    ../../inc/cntimageeditorview.h \
    ../../inc/cntfavoritesselectionview.h \
    ../../inc/cntfavoritesview.h \
    ../../inc/cntmainwindow.h \
    ../../inc/cntmodelprovider.h \
    ../../inc/cntnamesview.h \
    ../../inc/cntviewmanager.h \
    ../../inc/cntviewparameters.h \
    ../../inc/cntcontactcardheadingitem.h \
    ../../inc/cntcontactcarddatacontainer.h \
    ../../inc/cntcontactcardview.h \
    ../../inc/cntcontactcarddetailitem.h \
    ../../inc/cntdocumentloader.h \
    ../../inc/cnteditview.h \
    ../../inc/cnteditviewdetailitem.h \
    ../../inc/cnteditviewheadingitem.h \
    ../../inc/cntstringmapper.h \
    ../../inc/cntmycardselectionview.h \
    ../../inc/cntmycardview.h \
    ../../inc/cntgroupeditorview.h \
    ../../inc/cntgroupmemberview.h \
    ../../inc/cntgroupactionsview.h \
    ../../inc/cntsnapshotwidget.h \
    ../../inc/cnthistoryview.h \
    ../../inc/cnthistoryviewitem.h \
    ../../inc/cnthistoryviewitemwidget.h \
    ../../inc/cntgroupselectionpopup.h

SOURCES += src/testrunner.cpp
SOURCES += src/main.cpp
SOURCES += src/t_cntactions.cpp
SOURCES += src/t_cntaddresseditorview.cpp
SOURCES += src/t_cntbasedetaileditorview.cpp
SOURCES += src/t_cntbaselistview.cpp
SOURCES += src/t_cntbaseselectionview.cpp
SOURCES += src/t_cntbaseview.cpp
SOURCES += src/t_cntcollectionlistmodel.cpp
SOURCES += src/t_cntcollectionview.cpp
SOURCES += src/t_cntcommands.cpp
SOURCES += src/t_cntcompanyeditorview.cpp
SOURCES += src/t_cntcontactcarddatacontainer.cpp
SOURCES += src/t_cntcontactcarddetailitem.cpp
SOURCES += src/t_cntcontactcardheadingitem.cpp
SOURCES += src/t_cntcontactcardview.cpp
SOURCES += src/t_cntdateeditorview.cpp
SOURCES += src/t_cntdetailpopup.cpp
SOURCES += src/t_cnteditordatamodelitem.cpp
SOURCES += src/t_cnteditordataviewitem.cpp
SOURCES += src/t_cnteditviewdetailitem.cpp
SOURCES += src/t_cnteditviewheadingitem.cpp
SOURCES += src/t_cntemaileditorview.cpp
SOURCES += src/t_cntfamilydetaileditorview.cpp
SOURCES += src/t_cntfavoritesselectionview.cpp
SOURCES += src/t_cntfavoritesview.cpp
SOURCES += src/t_cntgroupactionsview.cpp
SOURCES += src/t_cntgroupeditorview.cpp
SOURCES += src/t_cntgroupmemberview.cpp
SOURCES += src/t_cntimageeditorview.cpp
SOURCES += src/t_cntmainwindow.cpp
SOURCES += src/t_cntmodelprovider.cpp
SOURCES += src/t_cntmycardselectionview.cpp
SOURCES += src/t_cntmycardview.cpp
SOURCES += src/t_cntnameseditorview.cpp
SOURCES += src/t_cntnamesview.cpp
SOURCES += src/t_cntnoteeditorview.cpp
SOURCES += src/t_cntonlineaccounteditorview.cpp
SOURCES += src/t_cntphonenumbereditorview.cpp
SOURCES += src/t_cntsnapshotwidget.cpp
SOURCES += src/t_cnturleditorview.cpp
SOURCES += src/t_cntviewmanager.cpp
SOURCES += src/t_cntviewparameters.cpp
SOURCES += src/t_cntcommhistoryview.cpp
SOURCES += src/t_cntgroupselectionpopup.cpp
SOURCES += ../../src/cntactions.cpp \
    ../../src/cntbaselistview.cpp \
    ../../src/cntbaseview.cpp \
    ../../src/cntbasedetaileditorview.cpp \
    ../../src/cntbaseselectionview.cpp \
    ../../src/cntcollectionview.cpp \
    ../../src/cntcollectionlistmodel.cpp \
    ../../src/cntcommands.cpp \
    ../../src/cntdetailpopup.cpp \
    ../../src/cnteditordataviewitem.cpp \
    ../../src/cnteditordatamodelitem.cpp \
    ../../src/cntemaileditorview.cpp \
    ../../src/cntnameseditorview.cpp \
    ../../src/cnturleditorview.cpp \
    ../../src/cntcompanyeditorview.cpp \
    ../../src/cntphonenumbereditorview.cpp \
    ../../src/cntaddresseditorview.cpp \
    ../../src/cntonlineaccounteditorview.cpp \
    ../../src/cntnoteeditorview.cpp \
    ../../src/cntfamilydetaileditorview.cpp \
    ../../src/cntdateeditorview.cpp \
    ../../src/cntimageeditorview.cpp \
    ../../src/cntfavoritesselectionview.cpp \
    ../../src/cntfavoritesview.cpp \
    ../../src/cntmainwindow.cpp \
    ../../src/cntmodelprovider.cpp \
    ../../src/cntnamesview.cpp \
    ../../src/cntviewmanager.cpp \
    ../../src/cntviewparameters.cpp \
    ../../src/cntcontactcarddetailitem.cpp \
    ../../src/cntcontactcarddatacontainer.cpp \
    ../../src/cntcontactcardview.cpp \
    ../../src/cntcontactcardheadingitem.cpp \
    ../../src/cntdocumentloader.cpp \
    ../../src/cnteditview.cpp \
    ../../src/cnteditviewdetailitem.cpp \
    ../../src/cnteditviewheadingitem.cpp \
    ../../src/cntmycardselectionview.cpp \
    ../../src/cntmycardview.cpp \
    ../../src/cntgroupeditorview.cpp \
    ../../src/cntgroupmemberview.cpp\
    ../../src/cntgroupactionsview.cpp \
    ../../src/cntsnapshotwidget.cpp \
    ../../src/cnthistoryview.cpp \
    ../../src/cnthistoryviewitem.cpp \
    ../../src/cnthistoryviewitemwidget.cpp \
    ../../src/cntgroupselectionpopup.cpp
SOURCES += ./stubs/hbstubs.cpp
SOURCES += ./stubs/qthighway_stub.cpp

# capability
TARGET.CAPABILITY = ALL \
    -TCB

LIBS+= -lxqservice \
       -lQtContacts \
       -lmobcntmodel \
       -lthumbnailmanagerqt \
       -lmobhistorymodel

