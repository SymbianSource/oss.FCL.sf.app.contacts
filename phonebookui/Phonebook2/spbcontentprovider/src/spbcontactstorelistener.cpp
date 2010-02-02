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
* Description:  .
*
*/

// CLASS HEADER
#include "spbcontactstorelistener.h"

// INCLUDES
#include <CPbk2StoreManager.h>
#include <CPbk2ApplicationServices.h>	
#include <CPbk2StoreManager.h>
#include "spbcontent.h"        

// ----------------------------------------------------------------------------
// CSpbContactStoreListener::NewL
// ----------------------------------------------------------------------------
//
CSpbContactStoreListener* CSpbContactStoreListener::NewL(
    CPbk2StoreManager& aStoreManager,
    RPointerArray<CSpbContent>& aContentCache )
    {
    CSpbContactStoreListener* self = 
        new (ELeave) CSpbContactStoreListener( aStoreManager, aContentCache );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CSpbContactStoreListener::~CSpbContactStoreListener
// ----------------------------------------------------------------------------
//
CSpbContactStoreListener::~CSpbContactStoreListener()
    {
    iStoreManager.DeregisterStoreEvents( *this );
    }

// ----------------------------------------------------------------------------
// CSpbContactStoreListener::CSpbContactStoreListener
// ----------------------------------------------------------------------------
//
inline CSpbContactStoreListener::CSpbContactStoreListener(
    CPbk2StoreManager& aStoreManager,
    RPointerArray<CSpbContent>& aContentCache ) :
        iStoreManager( aStoreManager ),
        iContentCache(aContentCache)
    {
    }

// ----------------------------------------------------------------------------
// CSpbContactStoreListener::ConstructL
// ----------------------------------------------------------------------------
//
inline void CSpbContactStoreListener::ConstructL()
    {
    iStoreManager.RegisterStoreEventsL( *this );
    }

// --------------------------------------------------------------------------
// CSpbContactStoreListener::StoreReady
// --------------------------------------------------------------------------
//
void CSpbContactStoreListener::StoreReady( MVPbkContactStore& /*aContactStore*/ )
	{
	// do nothing
	}

// --------------------------------------------------------------------------
// CSpbContactStoreListener::StoreUnavailable
// --------------------------------------------------------------------------
//
void CSpbContactStoreListener::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, 
												 TInt /*aReason*/ )
	{
	// do nothing
	}

// --------------------------------------------------------------------------
// CSpbContactStoreListener::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CSpbContactStoreListener::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/, 
												  TVPbkContactStoreEvent aStoreEvent )
	{
	/// contact has changed, check if the number needs to be reloaded
	if( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactChanged )
		{
		CSpbContent* content = ContentByLinkL( *aStoreEvent.iContactLink );
		if( content )
			{
			content->RefreshNumber();
			}
		}
	}

// ----------------------------------------------------------------------------
// CSpbContactStoreListener::ContentByLinkL
// ----------------------------------------------------------------------------
//
CSpbContent* CSpbContactStoreListener::ContentByLinkL( 
    const MVPbkContactLink& aLink )
    {
    const TInt count( iContentCache.Count() );
    for( TInt i = 0 ; i < count ; ++i )
        {
        CSpbContent* content = iContentCache[i];
        if( content->Match( aLink ) )
            {
            return content;
            }
        }
    return NULL;
    }

