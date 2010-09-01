/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test suite for ContactPresence API
*
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "contactpresenceapitester.h"
#include "contactpresenceapihandler.h"

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::CContactPresenceApiHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactPresenceApiHandler::CContactPresenceApiHandler( CContactPresenceApiTester& aTesterMain ) :
                              iTesterMain( aTesterMain )
    {
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactPresenceApiHandler::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactPresenceApiHandler* CContactPresenceApiHandler::NewL( CContactPresenceApiTester& aTesterMain )
    {
    CContactPresenceApiHandler* self = new ( ELeave ) CContactPresenceApiHandler( aTesterMain );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::Activate
// 
// -----------------------------------------------------------------------------
//
CContactPresenceApiHandler::~CContactPresenceApiHandler()
    {
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::ReceiveIconInfoL
// 
// -----------------------------------------------------------------------------
//
void CContactPresenceApiHandler::ReceiveIconInfoL( const TDesC8& /*aPackedLink*/, const TDesC8& aBrandId, const TDesC8& aElementId )
    {
    TBuf<50> brand;
    TBuf<50> element;
    brand.Copy(aBrandId);
    element.Copy(aElementId); 
    
    iTesterMain.RequestComplete( KErrNone );    
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::ReceiveIconFileL
// 
// -----------------------------------------------------------------------------
//        
void CContactPresenceApiHandler::ReceiveIconFileL( const TDesC8& /*aBrandId*/, const TDesC8& /*aElementId*/, 
                                                CFbsBitmap* aBrandedBitmap, CFbsBitmap* /*aMask*/  )
    {
    if ( aBrandedBitmap )
        {
        TSize mySize = aBrandedBitmap->SizeInPixels();
        }
    iTesterMain.RequestComplete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::PresenceSubscribeError
// 
// -----------------------------------------------------------------------------
//
void CContactPresenceApiHandler::PresenceSubscribeError( const TDesC8& /*aContactLink*/, TInt /*aStatus*/ )
    {    
    iTesterMain.RequestComplete( KErrGeneral );          
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::ErrorOccuredL
// 
// -----------------------------------------------------------------------------
//      
void CContactPresenceApiHandler::ErrorOccured( TInt /*aOpId*/, TInt /*aStatus*/ )
    {
    iTesterMain.RequestComplete( KErrNone );
    }
    
// -----------------------------------------------------------------------------
// CContactPresenceApiHandler::ReceiveIconInfosL
// 
// -----------------------------------------------------------------------------
//
void CContactPresenceApiHandler::ReceiveIconInfosL(
    const TDesC8& /*aPackedLink*/,
    RPointerArray <MContactPresenceInfo>& aInfoArray,
    TInt aOpId )   
    {
    TInt ret = KErrNone;
    TBuf<50> brand;
    TBuf<50> element;
    TBuf<50> service; 
    
    TInt myId = aOpId;
    
    TInt count = aInfoArray.Count();

    
    MContactPresenceInfo* info = NULL;    
    for ( TInt i = 0; i<count; i++ )
        {
        info = aInfoArray[i];
        brand.Copy( info->BrandId());
        element.Copy( info->ElementId());
        service.Copy( info->ServiceName());
        }

    iTesterMain.RequestComplete( ret );    
    }    

//  End of File
