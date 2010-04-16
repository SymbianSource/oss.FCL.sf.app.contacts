#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
TARGET = pbkcommonui

CONFIG += dll
CONFIG += hb
HB += hbfeedback
QT += sql

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE
INCLUDEPATH += $$MW_LAYER_SYSTEMINCLUDE

MOC_DIR = moc

DEFINES += QT_DLL BUILD_QTPBK

symbian: {
        # Skip the UID2/3 thing
        TARGET.UID3 = 0x20026792
        TARGET.EPOCSTACKSIZE = 0x5000
        TARGET.EPOCHEAPSIZE = "0x20000 0x2000000"
}

# Input
HEADERS += \
	inc/cntviewnavigator.h \
	inc/cntabstractviewmanager.h \
	inc/cntabstractview.h \
	inc/cntabstractviewfactory.h \
	inc/cntdefaultviewmanager.h \
	inc/cntactionmenubuilder.h \
	inc/cntaction.h \
	inc/cntactions.h \
    inc/cntbaselistview.h \
    inc/cntbaseview.h \
    inc/cntbasedetaileditorview.h \
    inc/cntbaseselectionview.h \
    inc/cntcollectionview.h \
    inc/cntcollectionlistmodel.h \
    inc/cntcommands.h \
    inc/cntdetailpopup.h \
    inc/cnteditordataviewitem.h \
    inc/cnteditordatamodelitem.h \
    inc/cntonlineaccounteditorview.h \
    inc/cntimageeditorview.h \
    inc/cntfavoritesmemberview.h \
    inc/cntfavoritesview.h \
    inc/cntmainwindow.h \
    inc/cntmodelprovider.h \
    inc/cntnamesview.h \
    inc/cntnamesview_p.h \
    inc/cntviewmanager.h \
    inc/cntviewparameters.h \
    inc/cntcontactcardheadingitem.h \
    inc/cntcontactcarddatacontainer.h \
    inc/cntcontactcardview.h \
    inc/cntcontactcarddetailitem.h \
    inc/cntdocumentloader.h \
    inc/cnteditview.h \
    inc/cnteditviewdetailitem.h \
    inc/cnteditviewheadingitem.h \
    inc/cntstringmapper.h \
    inc/cntmycardselectionview.h \
    inc/cntmycardview.h \
    inc/cntgroupeditorview.h \
    inc/cntgroupmemberview.h \
    inc/cntgroupactionsview.h \
    inc/cntsnapshotwidget.h \
    inc/cnthistoryview.h \
    inc/cnthistoryviewitem.h \
    inc/cnthistoryviewitemwidget.h \
    inc/cntgroupselectionpopup.h \
    inc/cntgroupdeletepopupmodel.h \
    inc/cntdetailconst.h \
    inc/cntdetaileditormodel.h \
    inc/cnteditorfactory.h \
    inc/cntdetaileditor.h \
    inc/cntphonenumberviewitem.h \
    inc/cntphonenumbermodel.h \
    inc/cntdetailmodelitem.h \
    inc/cntemaileditormodel.h \
    inc/cntemaileditorviewitem.h \
    inc/cntaddressmodel.h \
    inc/cntaddressviewitem.h \
    inc/cnturleditormodel.h \
    inc/cnturleditorviewitem.h \
    inc/cntnameeditormodel.h \
    inc/cntnoteeditormodel.h \
    inc/cntnoteeditorviewitem.h \
    inc/cntdateeditormodel.h \
    inc/cntdateeditorviewitem.h \
    inc/cntcompanyeditormodel.h \
    inc/cntfamilyeditormodel.h \
    inc/cntdetailviewitem.h \
    inc/cntgroupdeletepopup.h 
    
SOURCES += \
	src/cntviewnavigator.cpp \
	src/cntdefaultviewfactory.cpp \
	src/cntdefaultviewmanager.cpp \
	src/cntactionmenubuilder.cpp \
	src/cntaction.cpp \
	src/cntactions.cpp \
    src/cntbaselistview.cpp \
    src/cntbaseview.cpp \
    src/cntbasedetaileditorview.cpp \
    src/cntbaseselectionview.cpp \
    src/cntcollectionview.cpp \
    src/cntcollectionlistmodel.cpp \
    src/cntcommands.cpp \
    src/cntdetailpopup.cpp \
    src/cnteditordataviewitem.cpp \
    src/cnteditordatamodelitem.cpp \
    src/cntonlineaccounteditorview.cpp \
    src/cntimageeditorview.cpp \
    src/cntfavoritesmemberview.cpp \
    src/cntfavoritesview.cpp \
    src/cntmainwindow.cpp \
    src/cntmodelprovider.cpp \
    src/cntnamesview.cpp \
    src/cntnamesview_p.cpp \
    src/cntviewmanager.cpp \
    src/cntviewparameters.cpp \
    src/cntcontactcarddetailitem.cpp \
    src/cntcontactcarddatacontainer.cpp \
    src/cntcontactcardview.cpp \
    src/cntcontactcardheadingitem.cpp \
    src/cntdocumentloader.cpp \
    src/cnteditview.cpp \
    src/cnteditviewdetailitem.cpp \
    src/cnteditviewheadingitem.cpp \
    src/cntmycardselectionview.cpp \
    src/cntmycardview.cpp \
    src/cntgroupeditorview.cpp \
    src/cntgroupmemberview.cpp\
    src/cntgroupactionsview.cpp \
    src/cntsnapshotwidget.cpp \
    src/cnthistoryview.cpp \
    src/cnthistoryviewitem.cpp \
    src/cnthistoryviewitemwidget.cpp \
    src/cntgroupselectionpopup.cpp \
    src/cntgroupdeletepopupmodel.cpp \
    src/cntaddressmodel.cpp \
    src/cntaddressviewitem.cpp \
    src/cntcompanyeditormodel.cpp \
    src/cntdateeditormodel.cpp \
    src/cntdateeditorviewitem.cpp \
    src/cntdetaileditor.cpp \
    src/cntdetailmodelitem.cpp \
    src/cnteditorfactory.cpp \
    src/cntemaileditormodel.cpp \
    src/cntemaileditorviewitem.cpp \
    src/cntfamilyeditormodel.cpp \
    src/cntnameeditormodel.cpp \
    src/cntnoteeditormodel.cpp \
    src/cntnoteeditorviewitem.cpp \
    src/cntphonenumbermodel.cpp \
    src/cntphonenumberviewitem.cpp \
    src/cnturleditormodel.cpp \
    src/cnturleditorviewitem.cpp \
    src/cntgroupdeletepopup.cpp
    
RESOURCES += resources\pbkcommonui.qrc

LIBS+= -lhbcore \
       -lxqservice \
       -lQtContacts \
       -lmobcntmodel \
       -lthumbnailmanagerqt \
       -lmobhistorymodel \
       -lcntmaptileservice

# capability
TARGET.CAPABILITY = CAP_GENERAL_DLL


