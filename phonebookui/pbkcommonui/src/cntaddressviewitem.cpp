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
#include "cntaddressviewitem.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"

#include <qcontactdetail.h>
#include <qcontactaddress.h>
#include <hbdataform.h>
#include <hbdataformmodel.h>
#include <hbdataformmodelitem.h>
#include <hbstyleloader.h>
#include <hbpushbutton.h>
#include <qgraphicslinearlayout.h>
#include <hbwidget.h>
#include <hbtextitem.h>

#include "qlocationpickeritem.h"
#include <xqaiwrequest.h>
#include <xqservicerequest.h>
#include <xqappmgr.h>
CntAddressViewItem::CntAddressViewItem(QGraphicsItem* aParent) :
    /*CntDetailViewItem(aParent),*/
    HbDataFormViewItem(aParent)
{
}

CntAddressViewItem::~CntAddressViewItem()
{
}

HbAbstractViewItem* CntAddressViewItem::createItem()
{
    return new CntAddressViewItem(*this);
}

bool CntAddressViewItem::canSetModelIndex( const QModelIndex &index ) const 
{ 
    HbDataFormModelItem::DataItemType itemType = 
        static_cast<HbDataFormModelItem::DataItemType>( 
        index.data(HbDataFormModelItem::ItemTypeRole).toInt() ); 

    if( itemType == HbDataFormModelItem::CustomItemBase )       
    {  
        return true; 
    } 
    else 
    { 
        return false; 
    } 

}

HbWidget* CntAddressViewItem::createCustomWidget()
{
    HbDataFormModelItem::DataItemType itemType = static_cast<HbDataFormModelItem::DataItemType>( 
              modelIndex().data(HbDataFormModelItem::ItemTypeRole).toInt());
    
    HbWidget* widget = new HbWidget();
    if( itemType ==  HbDataFormModelItem::CustomItemBase )
    {
        QGraphicsLinearLayout* layout = new QGraphicsLinearLayout(Qt::Horizontal);
        widget->setLayout(layout);
    
        HbStyleLoader::registerFilePath(":/style/cntlocationbutton.css");
        HbStyleLoader::registerFilePath(":/style/cntlocationbutton.hbpushbutton.widgetml");
        HbDataForm* form = static_cast<HbDataForm*> (itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*> (form->model());
    
        HbPushButton* mLocationButton = new HbPushButton(this);
        mLocationButton->setObjectName("cntlocationbutton");
        mLocationButton->setText( hbTrId("txt_phob_button_select_location") );
        mLocationButton->setTextAlignment( Qt::AlignCenter );
    
        connect(mLocationButton, SIGNAL(clicked()), this, SLOT(launchLocationPicker()));
    
        layout->addItem(mLocationButton);
        
    }
    return widget;
}

void CntAddressViewItem::launchLocationPicker()
{
    XQApplicationManager *appManager = new XQApplicationManager();
    XQAiwRequest* request = appManager->create("com.nokia.symbian", "ILocationPick", "pick()", true);
    if( request )
    {
        QVariant retValue;
        if( request->send( retValue ) )
        {
            handleLocationChange( retValue );
        }
        
        delete request;
    }
    delete appManager;
}

void CntAddressViewItem::handleLocationChange(const QVariant& aValue)
{
    QLocationPickerItem selectedLocation = aValue.value<QLocationPickerItem>();
    if( selectedLocation.mIsValid )
    {
        HbDataForm* form = static_cast<HbDataForm*>(itemView());
        HbDataFormModel* model = static_cast<HbDataFormModel*>(form->model());
        HbDataFormModelItem* item = model->itemFromIndex( modelIndex() );
    
        QModelIndex nextIndex = modelIndex().sibling( modelIndex().row() + 1 , modelIndex().column() );
        HbDataFormModelItem* street = model->itemFromIndex( nextIndex );
        street->setContentWidgetData( "text", selectedLocation.mStreet );
        street->setContentWidgetData( "maxLength", CNT_STREET_MAXLENGTH );

        nextIndex = modelIndex().sibling( modelIndex().row() + 2 , modelIndex().column() );

        HbDataFormModelItem* postal =  model->itemFromIndex( nextIndex );
        postal->setContentWidgetData( "text", selectedLocation.mPostalCode );
        postal->setContentWidgetData( "maxLength", CNT_POSTCODE_MAXLENGTH );
        
        nextIndex = modelIndex().sibling( modelIndex().row() + 3 , modelIndex().column() );
        HbDataFormModelItem* locality =  model->itemFromIndex( nextIndex );
        locality->setContentWidgetData( "text", selectedLocation.mCity );
        locality->setContentWidgetData( "maxLength", CNT_LOCALITY_MAXLENGTH );
        
        nextIndex = modelIndex().sibling( modelIndex().row() + 4 , modelIndex().column() );
        HbDataFormModelItem* province =  model->itemFromIndex( nextIndex );
        province->setContentWidgetData( "text", selectedLocation.mState );
        province->setContentWidgetData( "maxLength", CNT_REGION_MAXLENGTH );
        
        nextIndex = modelIndex().sibling( modelIndex().row() + 5 , modelIndex().column() );
        HbDataFormModelItem* country =  model->itemFromIndex( nextIndex );
        country->setContentWidgetData( "text", selectedLocation.mCountry );
        country->setContentWidgetData( "maxLength", CNT_COUNTRY_MAXLENGTH );
        

    }
   
}

 Q_IMPLEMENT_USER_METATYPE(QLocationPickerItem)

// End of File
