/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     Prepend query implementation
*
*/


#include <w32std.h>
#include <eikdialg.h>

#include "CLogsPrependQuery.h"

// ========================== MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CLogsPrependQuery::NewL
// Instantiate prepend query
// ---------------------------------------------------------------------------
CLogsPrependQuery* CLogsPrependQuery::NewL( TDes& aResource )
    {
    CLogsPrependQuery* self = 
        new( ELeave ) CLogsPrependQuery( aResource );
    return self;
    }

// ---------------------------------------------------------------------------
// CLogsPrependQuery::~CLogsPrependQuery
// Destructor
// ---------------------------------------------------------------------------
CLogsPrependQuery::~CLogsPrependQuery()
    {
    }

// ---------------------------------------------------------------------------
// CLogsPrependQuery::CLogsPrependQuery
// C++ constructor
// ---------------------------------------------------------------------------
CLogsPrependQuery::CLogsPrependQuery( TDes& aResource )
    : CAknTextQueryDialog( aResource )
    {
    }

// ---------------------------------------------------------------------------
// CLogsPrependQuery::OfferKeyEventL
// Send key handling overriden
// ---------------------------------------------------------------------------
TKeyResponse CLogsPrependQuery::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyPhoneSend || aKeyEvent.iCode == EKeyOK 
    	 || aKeyEvent.iCode == EKeyEnter || aKeyEvent.iScanCode == EStdKeyYes)
        {
        TryExitL( EAknSoftkeyCall );
        return EKeyWasConsumed;
        }
    return CAknTextQueryDialog::OfferKeyEventL( aKeyEvent, aType );
    }

//  end of file
