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
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../phonebookengines/cntimageutility/inc
INCLUDEPATH += ../../phonebookengines/cntsimutility/inc
INCLUDEPATH += ../phonebookapp/inc
INCLUDEPATH += ../cnthistorymodel/inc

INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

MOC_DIR = moc

DEFINES += QT_DLL BUILD_QTPBK

symbian: {
        # Skip the UID2/3 thing
        TARGET.UID3 = 0x20026792
        TARGET.EPOCSTACKSIZE = 0x5000
        TARGET.EPOCHEAPSIZE = "0x20000 0x2000000"

        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_services_api/inc/cntservicescontact.h APP_LAYER_PLATFORM_EXPORT_PATH(cntservicescontact.h)"

        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntviewparams.h APP_LAYER_PLATFORM_EXPORT_PATH(cntviewparams.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntabstractview.h APP_LAYER_PLATFORM_EXPORT_PATH(cntabstractview.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntabstractviewmanager.h APP_LAYER_PLATFORM_EXPORT_PATH(cntabstractviewmanager.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntabstractengine.h APP_LAYER_PLATFORM_EXPORT_PATH(cntabstractengine.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_api/inc/cntuids.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuids.h)"

        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuiextensionfactory.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuiextensionfactory.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuigroupsupplier.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuigroupsupplier.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuisocialextension.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuisocialextension.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuiextensiongroup.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuiextensiongroup.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntextensiongroupcallback.h APP_LAYER_PLATFORM_EXPORT_PATH(cntextensiongroupcallback.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntviewsupplier.h APP_LAYER_PLATFORM_EXPORT_PATH(cntviewsupplier.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cnteditviewitemsupplier.h APP_LAYER_PLATFORM_EXPORT_PATH(cnteditviewitemsupplier.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cnteditviewitem.h APP_LAYER_PLATFORM_EXPORT_PATH(cnteditviewitem.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuiactionextension.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuiactionextension.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cnteditviewitemcallback.h APP_LAYER_PLATFORM_EXPORT_PATH(cnteditviewitemcallback.h)"
        :BLD_INF_RULES.prj_exports += "../../contacts_plat/contacts_ui_extensions_api/inc/cntuicontactcardextension.h APP_LAYER_PLATFORM_EXPORT_PATH(cntuicontactcardextension.h)"
}

# Input
HEADERS += \
	inc/cntviewnavigator.h \
	inc/cntabstractviewfactory.h \
	inc/cntdefaultviewmanager.h \
	inc/cntactionmenubuilder.h \
	inc/cntactionlauncher.h \
    inc/cntbaseselectionview.h \
    inc/cntcollectionview.h \
    inc/cntcollectionlistmodel.h \
    inc/cntcollectionlistmodel_p.h \
    inc/cntcollectionlistmodelworker.h \
    inc/cntdetailpopup.h \
    inc/cntimageeditorview.h \
    inc/cntfavoritesmemberview.h \
    inc/cntfavoritesview.h \
    inc/cntfavourite.h \
    inc/cntmainwindow.h \
    inc/cntnamesview.h \
    inc/cntnamesview_p.h \
    inc/cntcontactcardheadingitem.h \
    inc/cntcontactcarddatacontainer.h \
    inc/cntcontactcarddataitem.h \
    inc/cntcontactcardcontextmenu.h \
    inc/cntcontactcardview.h \
    inc/cntcontactcardview_p.h \
    inc/cntcontactcarddetailitem.h \
    inc/cntdocumentloader.h \
    inc/cnteditview.h \
    inc/cnteditview_p.h \
    inc/cnteditviewlistmodel.h \
    inc/cnteditviewdetailitem.h \
    inc/cnteditviewheadingitem.h \
    inc/cntstringmapper.h \
    inc/cntmycardview.h \
    inc/cntgroupmemberview.h \
    inc/cntgroupmemberview_p.h \
    inc/cntgroupactionsview.h \
    inc/cntgroupeditormodel.h \
    inc/cnthistoryview.h \
    inc/cnthistoryviewitem.h \
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
    inc/cntgroupdeletepopup.h \
    inc/cntextensionmanager.h \
    inc/cntimagelabel.h \
    inc/cntfetchcontactpopup.h \
    inc/cntfetchmarkall.h \
    inc/cntimportsview.h \
    inc/cntsettingsview.h \
    inc/cntpresencelistener.h \
    inc/cntdetailorderinghelper.h \
    inc/cntsettingsmodel.h \
    inc/cntcommondetailviewitem.h \
    ../../inc/cntdebug.h \
    inc/cntactionpopup_p.h \
    inc/cntactionpopup.h \
    inc/cntsavemanager.h \
    inc/cntkeygrabber.h \
    engine_inc/cntdefaultengine.h \
    engine_inc/cntthumbnailmanager.h
    
    
SOURCES += \
	src/cntviewnavigator.cpp \
	src/cntdefaultviewfactory.cpp \
	src/cntdefaultviewmanager.cpp \
	src/cntactionmenubuilder.cpp \
	src/cntactionlauncher.cpp \
    src/cntbaseselectionview.cpp \
    src/cntcollectionview.cpp \
    src/cntcollectionlistmodel.cpp \
    src/cntcollectionlistmodelworker.cpp \
    src/cntdetailpopup.cpp \
    src/cntimageeditorview.cpp \
    src/cntfavoritesmemberview.cpp \
    src/cntfavoritesview.cpp \
    src/cntfavourite.cpp \
    src/cntmainwindow.cpp \
    src/cntnamesview.cpp \
    src/cntnamesview_p.cpp \
    src/cntcontactcarddetailitem.cpp \
    src/cntcontactcarddatacontainer.cpp \
    src/cntcontactcarddataitem.cpp \
    src/cntcontactcardview.cpp \
    src/cntcontactcardview_p.cpp \
    src/cntcontactcardheadingitem.cpp \
    src/cntcontactcardcontextmenu.cpp \
    src/cntdocumentloader.cpp \
    src/cnteditview.cpp \
    src/cnteditview_p.cpp \
    src/cnteditviewlistmodel.cpp \
    src/cnteditviewitembuilder.cpp \
    src/cnteditviewdetailitem.cpp \
    src/cnteditviewheadingitem.cpp \
    src/cnteditviewseparator.cpp \
    src/cntmycardview.cpp \
    src/cntgroupmemberview.cpp\
    src/cntgroupmemberview_p.cpp\
    src/cntgroupactionsview.cpp \
    src/cntgroupeditormodel.cpp \
    src/cnthistoryview.cpp \
    src/cnthistoryviewitem.cpp \
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
    src/cntgroupdeletepopup.cpp \
    src/cntextensionmanager.cpp \
    src/cntimagelabel.cpp \
    src/cntimportsview.cpp \
    src/cntfetchcontactpopup.cpp \
    src/cntfetchmarkall.cpp \
    src/cntsettingsview.cpp \
    src/cntpresencelistener.cpp \
    src/cntdetailorderinghelper.cpp \
    src/cntsettingsmodel.cpp \
    src/cntcommondetailviewitem.cpp \
    src/cntactionpopup_p.cpp \
    src/cntactionpopup.cpp \
    src/cntsavemanager.cpp \
    src/cntkeygrabber.cpp \
    engine_src/cntthumbnailmanager.cpp \
    engine_src/cntdefaultengine.cpp

DOCML += resources/contacts_contactcard.docml \
         resources/contacts_ev.docml \
         resources/contacts_mc.docml \
         resources/contacts_if.docml \
         resources/contacts_list.docml \
         resources/contacts_namelist.docml \
         resources/contacts_groupactions.docml \
         resources/contacts_history.docml \
         resources/contacts_editor.docml \
         resources/contacts_sim.docml \
         resources/contacts_favorite.docml \
         resources/contacts_favmember.docml \
         resources/contacts_collections.docml \
         resources/contacts_groupmembers.docml \
         resources/contacts_detail_editor.docml \
         resources/contacts_fetchdialog.docml \
         resources/contacts_settings.docml
    
RESOURCES += resources/pbkcommonui.qrc

LIBS+= -lhbcore \
       -lxqservice \
       -lqtcontacts \
       -lcntlistmodel \
       -lthumbnailmanagerqt \
       -lcnthistorymodel \
       -lmaptileservice \
       -lqtversit \
       -lcntimageutility \
       -lcntsimutility \
       -lshareui \
       -lpresencecacheqt \
       -lxqsettingsmanager

# capability
TARGET.CAPABILITY = CAP_GENERAL_DLL
defBlock = \      
	"$${LITERAL_HASH}if defined(EABI)" \
		"DEFFILE  ../eabi/pbkcommonui.def" \
    "$${LITERAL_HASH}else" \
        "DEFFILE  ../bwins/pbkcommonui.def" \
	"$${LITERAL_HASH}endif"
MMP_RULES += defBlock

