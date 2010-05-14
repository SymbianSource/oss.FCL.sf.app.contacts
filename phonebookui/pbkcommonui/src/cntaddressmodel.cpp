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

#include "cntaddressmodel.h"
#include "cntdetailmodelitem.h"
#include "cntdetailconst.h"
#include <cntmaptileservice.h> 

CntAddressModel::CntAddressModel( QContact* aContact ):
CntDetailEditorModel( aContact ),
mAddress(NULL),
mAddressHome(NULL),
mAddressWork(NULL),
mIsLocationPickerEnabled( false )
    {
    HbDataFormModelItem* address = appendDataFormGroup(qtTrId("Address"), invisibleRootItem());
    HbDataFormModelItem* addressHome = appendDataFormGroup(qtTrId("Address (home)"), invisibleRootItem());
    HbDataFormModelItem* addressWork = appendDataFormGroup(qtTrId("Address (work)"), invisibleRootItem());
    
    foreach ( QContactAddress a, mContact->details<QContactAddress>() )
        {
        QStringList context = a.contexts();
        if ( context.isEmpty() && !mAddress ) // no context
            {
            mAddress = new QContactAddress( a );
            createAddressItems( address, mAddress );
            }
        else if (context.first() == QContactAddress::ContextHome && !mAddressHome )
            {
            mAddressHome = new QContactAddress( a );
            createAddressItems( addressHome, mAddressHome );
            }
        else if (context.first() == QContactAddress::ContextWork && !mAddressWork )
            {
            mAddressWork = new QContactAddress( a );
            createAddressItems( addressWork, mAddressWork );
            }
        }
    
    // Check that all items (address, addressHome, addressWork) are created
    if ( !mAddress )
        {
        mAddress = new QContactAddress();
        createAddressItems( address, mAddress );
        }
    
    if ( !mAddressHome )
        {
        QStringList context;
        context << QContactAddress::ContextHome;
                
        mAddressHome = new QContactAddress();
        mAddressHome->setContexts( context );
        
        createAddressItems( addressHome, mAddressHome );
        }
    
    if ( !mAddressWork )
        {
        mAddressWork = new QContactAddress();
        QStringList context;
        context << QContactAddress::ContextWork;
        mAddressWork->setContexts( context );
        createAddressItems( addressWork, mAddressWork );
        }
    }

CntAddressModel::~CntAddressModel()
    {
    delete mAddress;
    delete mAddressHome;
    delete mAddressWork;
    }

void CntAddressModel::createAddressItems( HbDataFormModelItem* aGroup, QContactAddress* aAddress )
    {
	//Show the location picker button only if location feature enabled
    if( CntMapTileService::isLocationFeatureEnabled() )
    {
        // custom item for map button
        HbDataFormModelItem* mapButton = new HbDataFormModelItem( HbDataFormModelItem::CustomItemBase );
        appendDataFormItem( mapButton, aGroup );
        mIsLocationPickerEnabled = true;
    }
    
    // default items for rest of fields
    HbDataFormModelItem* street = new HbDataFormModelItem( HbDataFormModelItem::TextItem, qtTrId("Street"));
    street->setContentWidgetData( "text", aAddress->street() );
    street->setContentWidgetData( "maxLength", CNT_STREET_MAXLENGTH );
    
    HbDataFormModelItem* postal = new HbDataFormModelItem( HbDataFormModelItem::TextItem, qtTrId("Post code"));
    postal->setContentWidgetData( "text", aAddress->postcode() );
    postal->setContentWidgetData( "maxLength", CNT_POSTCODE_MAXLENGTH );
    
    HbDataFormModelItem* city = new HbDataFormModelItem( HbDataFormModelItem::TextItem, qtTrId("City"));
    city->setContentWidgetData( "text", aAddress->locality() );
    city->setContentWidgetData( "maxLength", CNT_LOCALITY_MAXLENGTH );
    
    HbDataFormModelItem* region = new HbDataFormModelItem( HbDataFormModelItem::TextItem, qtTrId("Province"));
    region->setContentWidgetData( "text", aAddress->region() );
    region->setContentWidgetData( "maxLength", CNT_REGION_MAXLENGTH );
    
    HbDataFormModelItem* country = new HbDataFormModelItem( HbDataFormModelItem::TextItem, qtTrId("Country"));
    country->setContentWidgetData( "text", aAddress->country() );
    country->setContentWidgetData( "maxLength", CNT_COUNTRY_MAXLENGTH );
    
    appendDataFormItem( street, aGroup );
    appendDataFormItem( postal, aGroup );
    appendDataFormItem( city, aGroup );
    appendDataFormItem( region, aGroup );
    appendDataFormItem( country, aGroup );
    }

void CntAddressModel::saveContactDetails()
{
    // No Context
    HbDataFormModelItem* addressRoot = invisibleRootItem()->childAt( 0 );
    saveAddressItems( addressRoot, mAddress );
    
    // Home 
    HbDataFormModelItem* addressHomeRoot = invisibleRootItem()->childAt( 1 );
    saveAddressItems( addressHomeRoot, mAddressHome );
    
    // Work
    HbDataFormModelItem* addressWorkRoot = invisibleRootItem()->childAt( 2 );
    saveAddressItems( addressWorkRoot, mAddressWork );
    
    // save and remove empty details
    mContact->saveDetail( mAddress );
    mContact->saveDetail( mAddressHome );
    mContact->saveDetail( mAddressWork );
    
    if ( isAddressEmpty(mAddress) ) 
    {
        mContact->removeDetail( mAddress );
    }
    
    if ( isAddressEmpty(mAddressHome) ) 
    {
        mContact->removeDetail( mAddressHome );
    }
    
    if ( isAddressEmpty(mAddressWork) ) 
    {
        mContact->removeDetail( mAddressWork );
    }
}

void CntAddressModel::saveAddressItems( HbDataFormModelItem* aGroup, QContactAddress* aAddress )
{
    int offset = 0;
    if( CntMapTileService::isLocationFeatureEnabled() )
    {
        offset = 1;
    }
		
    // first item (0) is the map button
    aAddress->setStreet( aGroup->childAt( 0 + offset  )->contentWidgetData("text").toString().trimmed() );
    aAddress->setPostcode( aGroup->childAt( 1 + offset )->contentWidgetData("text").toString().trimmed() );
    aAddress->setLocality( aGroup->childAt( 2 + offset  )->contentWidgetData("text").toString().trimmed() );
    aAddress->setRegion( aGroup->childAt( 3 + offset  )->contentWidgetData("text").toString().trimmed() );
    aAddress->setCountry( aGroup->childAt( 4 + offset  )->contentWidgetData("text").toString().trimmed() );
}

bool CntAddressModel::isAddressEmpty( QContactAddress* aAddress )
{
    return ( aAddress->street().length() == 0 &&
             aAddress->postcode().length() == 0 &&
             aAddress->locality().length() == 0 &&
             aAddress->region().length() == 0 &&
             aAddress->country().length() == 0 );
}
