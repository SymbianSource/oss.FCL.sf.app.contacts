/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A contact bookmark implementation for Contacts Model contact
*
*/


#include"CContactBookmark.h"

namespace VPbkCntModel {
    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CContactBookmark::CContactBookmark
// ---------------------------------------------------------------------------
//
CContactBookmark::CContactBookmark( TContactItemId aId,
		CContactStore& aStore ) :
            iId( aId ), iStore( aStore )
    {
    }

// ---------------------------------------------------------------------------
// CContactBookmark::NewL
// ---------------------------------------------------------------------------
//
CContactBookmark* CContactBookmark::NewL(TContactItemId aId,
        CContactStore& aStore )
    {
    CContactBookmark* self = CContactBookmark::NewLC( aId, aStore );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CContactBookmark::NewLC
// ---------------------------------------------------------------------------
//
CContactBookmark* CContactBookmark::NewLC( TContactItemId aId,
        CContactStore& aStore )
    {
    CContactBookmark* self = new( ELeave ) CContactBookmark( aId, aStore );
    CleanupStack::PushL( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CContactBookmark::~CContactBookmark
// ---------------------------------------------------------------------------
//
CContactBookmark::~CContactBookmark()
    {
    }
}
