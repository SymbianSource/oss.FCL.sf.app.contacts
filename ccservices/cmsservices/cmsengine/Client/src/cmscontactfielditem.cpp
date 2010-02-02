/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#include "cmscontactfielditem.h"
#include <CVPbkDefaultAttribute.h>

// ----------------------------------------------------
// CCmsContactFieldItem::CCmsContactField
// 
// ----------------------------------------------------
//
CCmsContactFieldItem::CCmsContactFieldItem():
    iDefaultAttributeMask( 0 )
    {     
    }

// ----------------------------------------------------
// CCmsContactFieldItem::NewL
// 
// ----------------------------------------------------
//
CCmsContactFieldItem* CCmsContactFieldItem::NewL( const TDesC& aItemData )
    {
    CCmsContactFieldItem* self = new ( ELeave ) CCmsContactFieldItem();
    CleanupStack::PushL( self );
    self->ConstructL( aItemData );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsContactFieldItem::NewL
// 
// ----------------------------------------------------
//
CCmsContactFieldItem* CCmsContactFieldItem::NewL( const TDesC8& aItemData )
    {
    CCmsContactFieldItem* self = new ( ELeave ) CCmsContactFieldItem();
    CleanupStack::PushL( self );
    self->ConstructL( aItemData );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsContactFieldItem::ConstructL
// 
// ----------------------------------------------------
//
void CCmsContactFieldItem::ConstructL( const TDesC& aItemData )
    {
    iFieldDataBuf16 = aItemData.AllocL();
    }

// ----------------------------------------------------
// CCmsContactFieldItem::ConstructL
// 
// ----------------------------------------------------
//
void CCmsContactFieldItem::ConstructL( const TDesC8& aItemData )
    {
    iFieldDataBuf8 = aItemData.AllocL();
    }

// ----------------------------------------------------
// CCmsContactFieldItem::~CCmsContactField
// 
// ----------------------------------------------------
//
CCmsContactFieldItem::~CCmsContactFieldItem()
    {
    delete iFieldDataBuf8;
    delete iFieldDataBuf16;
    delete iFieldDataBufInfo;
    }

// ----------------------------------------------------
// CCmsContactFieldItem::Data
// 
// ----------------------------------------------------
//
EXPORT_C TPtrC CCmsContactFieldItem::Data() const
    {
    return iFieldDataBuf16 ? iFieldDataBuf16->Des() : TPtrC();      
    }

// ----------------------------------------------------
// CCmsContactFieldItem::BinaryData
// 
// ----------------------------------------------------
//
EXPORT_C TPtrC8 CCmsContactFieldItem::BinaryData() const
    {
    return iFieldDataBuf8 ? iFieldDataBuf8->Des() : TPtrC8();      
    }

// ----------------------------------------------------
// CCmsContactFieldItem::Info
// 
// ----------------------------------------------------
//
EXPORT_C TPtrC CCmsContactFieldItem::Info() const
    {
    return iFieldDataBufInfo ? iFieldDataBufInfo->Des() : TPtrC();      
    }

// ----------------------------------------------------
// CCmsContactFieldItem::SetData
// 
// ----------------------------------------------------
//
void CCmsContactFieldItem::SetData( const TDesC& aItemData )
    {
    delete iFieldDataBuf16;
    iFieldDataBuf16 = NULL;
    iFieldDataBuf16 = aItemData.AllocL();    
    }

// ----------------------------------------------------
// CCmsContactFieldItem::SetInfo
// 
// ----------------------------------------------------
//
void CCmsContactFieldItem::SetInfo( const TDesC& aItemInfo )
    {
    delete iFieldDataBufInfo;
    iFieldDataBufInfo = NULL;
    iFieldDataBufInfo = aItemInfo.AllocL();    
    }

// ----------------------------------------------------
// CCmsContactFieldItem::HasDefaultAttribute
// 
// ----------------------------------------------------
//
TBool CCmsContactFieldItem::HasDefaultAttribute( TCmsDefaultAttributeTypes aDefaultType )
    {
    return iDefaultAttributeMask & aDefaultType ? ETrue : EFalse;
    }




// end of file
