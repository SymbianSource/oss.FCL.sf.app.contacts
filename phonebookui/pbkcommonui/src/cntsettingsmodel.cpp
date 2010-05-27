/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/
#include "cntsettingsview.h"
#include <hbdataformmodelitem.h>
#include <hbglobal.h>

CntDefaultSettingsModel::CntDefaultSettingsModel()
{
    QStringList orderList;
    orderList.append( hbTrId("txt_phob_setlabel_name_display_order_val_last_name") );
    orderList.append( hbTrId("txt_phob_setlabel_name_display_order_val_last_name_separator") );
    orderList.append( hbTrId("txt_phob_setlabel_name_display_order_val_first_nam") );
    /*
    QStringList showList;
    showList.append(hbTrId("txt_phob_setlabel_show_val_status_update"));
    showList.append(hbTrId("txt_phob_setlabel_show_val_name_and_phonenumber"));
    showList.append(hbTrId("txt_phob_setlabel_show_val_name_only"));
    */
    mOrder = new HbDataFormModelItem(HbDataFormModelItem::ComboBoxItem,  hbTrId("txt_phob_setlabel_name_display_order"));
    mOrder->setContentWidgetData( "items", orderList );

    /*
    HbDataFormModelItem* show = new HbDataFormModelItem( HbDataFormModelItem::ComboBoxItem, hbTrId("txt_phob_setlabel_show") );
    show->setContentWidgetData( "items", showList );
    */
    
    appendDataFormItem( mOrder, invisibleRootItem() );
    /* appendDataFormItem( show, invisibleRootItem() );
     */
}

CntDefaultSettingsModel::~CntDefaultSettingsModel()
{
    
}

void CntDefaultSettingsModel::saveSettings()
{
    QString selected = mOrder->contentWidgetData( "currentText" ).toString();
    
}

void CntDefaultSettingsModel::loadSettings()
{
    mOrder->setContentWidgetData("currentText", hbTrId("txt_phob_setlabel_name_display_order_val_last_name_separator") );
}
// End of File
