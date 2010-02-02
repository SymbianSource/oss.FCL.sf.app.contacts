/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event array item class. This class wraps contactlink and 
*                view index to one item. This is used in compositecontactview
*                to store view events
*
*/


#include "CVPbkEventArrayItem.h"

// From Vpbk
#include <MVPbkContactLink.h>
 
// --------------------------------------------------------------------------
// CVPbkEventArrayItem::CVPbkEventArrayItem
// --------------------------------------------------------------------------
//    
inline CVPbkEventArrayItem::CVPbkEventArrayItem( 
        TInt aContactIndex, 
        TViewEventType aEvent ):
    iContactIndex ( aContactIndex ), 
    iEvent( aEvent )
    {
    // do nothing
    }

// --------------------------------------------------------------------------
// CVPbkEventArrayItem::ConstructL
// --------------------------------------------------------------------------
//    
inline void CVPbkEventArrayItem::ConstructL( 
        const MVPbkContactLink& aLink )
    {
    iLink = aLink.CloneLC();
    CleanupStack::Pop(); // iContactLink
    }

// --------------------------------------------------------------------------
// CVPbkEventArrayItem::~CVPbkEventArrayItem
// --------------------------------------------------------------------------
//
CVPbkEventArrayItem::~CVPbkEventArrayItem()
    {
    delete iLink;
    }

// --------------------------------------------------------------------------
// CVPbkEventArrayItem::NewLC
// --------------------------------------------------------------------------
//    
CVPbkEventArrayItem* CVPbkEventArrayItem::NewLC( 
    TInt aContactLink, 
    const MVPbkContactLink& aLink,
    TViewEventType aEvent )
    {
    CVPbkEventArrayItem* self = 
        new(ELeave) CVPbkEventArrayItem( aContactLink, aEvent );
    CleanupStack::PushL( self );
    self->ConstructL( aLink );
    return self;
    }  

// --------------------------------------------------------------------------
// CVPbkEventArrayItem::Link
// --------------------------------------------------------------------------
//    
MVPbkContactLink* CVPbkEventArrayItem::Link()
    {
    return iLink;
    }

// --------------------------------------------------------------------------
// CVPbkEventArrayItem::Index
// --------------------------------------------------------------------------
//    
TInt CVPbkEventArrayItem::Index()
    {
    return iContactIndex;
    }

// --------------------------------------------------------------------------
// CVPbkEventArrayItem::Event
// --------------------------------------------------------------------------
//
TInt CVPbkEventArrayItem::Event()
    {
    return iEvent;
    }
// End of file
