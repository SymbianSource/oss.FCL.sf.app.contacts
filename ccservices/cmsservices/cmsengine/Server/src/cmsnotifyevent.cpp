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
#include "cmsnotifyevent.h"
#include "cmsservercontact.h"
#include "cmsserverutils.h"

// ----------------------------------------------------------
// CCmsPhonebookProxy::ConstructL
// 
// ----------------------------------------------------------
// 
CCmsNotifyEvent::CCmsNotifyEvent( CCmsServerContact& aContact ) :
                                  iContact( aContact )
    {
    }
    
// ----------------------------------------------------------
// CCmsNotifyEvent::NewL
// 
// ----------------------------------------------------------
// 
CCmsNotifyEvent* CCmsNotifyEvent::NewL( const TDesC& aMessage,
                                        CCmsServerContact& aContact )
    {
    CCmsNotifyEvent* self = new( ELeave ) CCmsNotifyEvent( aContact );
    CleanupStack::PushL( self );
    self->ConstructL( aMessage );
    CleanupStack::Pop();
    return self;   
    }

 // ----------------------------------------------------------
// CCmsNotifyEvent::NewL
// 
// ----------------------------------------------------------
//        
CCmsNotifyEvent* CCmsNotifyEvent::NewL( const TDesC8& aMessage,
                                        CCmsServerContact& aContact )
    {
    CCmsNotifyEvent* self = new( ELeave ) CCmsNotifyEvent( aContact );
    CleanupStack::PushL( self );
    self->ConstructL( aMessage );
    CleanupStack::Pop();
    return self; 
    }

// ----------------------------------------------------------
// CCmsNotifyEvent::ConstructL
// 
// ----------------------------------------------------------
// 
CCmsNotifyEvent::~CCmsNotifyEvent()
    {
    delete iData8;
    delete iData16;
    }

// ----------------------------------------------------------
// CCmsNotifyEvent::ConstructL
// 
// ----------------------------------------------------------
// 
void CCmsNotifyEvent::ConstructL( const TDesC& aMessage )
    {
    iData16 = aMessage.AllocL();
    }

// ----------------------------------------------------------
// CCmsNotifyEvent::ConstructL
// 
// ----------------------------------------------------------
// 
void CCmsNotifyEvent::ConstructL( const TDesC8& aMessage )
    {
    iData8 = aMessage.AllocL();
    }

// ----------------------------------------------------------
// CCmsNotifyEvent::ConstructL
// 
// ----------------------------------------------------------
// 
TInt CCmsNotifyEvent::Complete( TInt aMessageSlot, const RMessage2& aMessage ) const
    {
    TInt ret = KErrGeneral;    
    TInt desLength( aMessage.GetDesMaxLength( aMessageSlot ) );
    
    
    if ( iData8 )
        {
        TRAP_IGNORE( ret = Complete8BitL( aMessageSlot, desLength, aMessage ) );
        }
    else
        {
        TRAP_IGNORE( ret = Complete16BitL( aMessageSlot, desLength, aMessage ) );
        }                
    return ret;
    }

// ----------------------------------------------------------
// CCmsNotifyEvent::Complete8BitL
// 
// ----------------------------------------------------------
//
TInt CCmsNotifyEvent::Complete8BitL( TInt aMessageSlot,
                                     TInt aClientDesLength,
                                     const RMessage2& aMessage ) const
    {
    TInt error = KErrGeneral;
    if( aClientDesLength >= iData8->Des().Length() )
        {
        error = aMessage.Write( aMessageSlot, *iData8 );
        aMessage.Complete( error );
        }
    else
        {
        iContact.CacheData( iData8 );
        TInt requiredLength( iData8->Des().Length() );
        aMessage.Complete( requiredLength );
        error = KErrNone;
        }
    return error;
    }

// ----------------------------------------------------------
// CCmsNotifyEvent::Complete16BitL
// 
// ----------------------------------------------------------
//
TInt CCmsNotifyEvent::Complete16BitL( TInt aMessageSlot, 
                                      TInt aClientDesLength,
                                      const RMessage2& aMessage ) const
    {
    TInt error = KErrGeneral;
    if( aClientDesLength >= iData16->Des().Length() )
        {
        error = aMessage.Write( aMessageSlot, *iData16 );
        aMessage.Complete( error );
        }
    else
        {
        iContact.CacheData( iData16 );
        TInt requiredLength( iData16->Des().Length() );
        aMessage.Complete( requiredLength );
        error = KErrNone;
        }
    return error;
    }

