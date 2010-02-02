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
* Description:  A class for icon identification
*
*/


#include <TPbk2IconId.h>
#include <Pbk2IconId.hrh>
#include <barsread.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TPbk2IconId::TPbk2IconId
// ---------------------------------------------------------------------------
//
EXPORT_C TPbk2IconId::TPbk2IconId() : 
    iOwnersUid( TUid::Uid( KPbk2NullIconIdOwnerUid  )),
    iIconId( EPbk2NullIconId )
    {
    }

// ---------------------------------------------------------------------------
// TPbk2IconId::TPbk2IconId
// ---------------------------------------------------------------------------
//
EXPORT_C TPbk2IconId::TPbk2IconId( 
        const TUid& aOwnerUid, 
        TInt aIconId ) :   
    iOwnersUid( aOwnerUid ),
    iIconId( aIconId )
    {
    }

// ---------------------------------------------------------------------------
// TPbk2IconId::TPbk2IconId
// ---------------------------------------------------------------------------
//
EXPORT_C TPbk2IconId::TPbk2IconId( 
        TResourceReader& aReader )
    {
    iOwnersUid = TUid::Uid( aReader.ReadInt32() );
    iIconId = aReader.ReadInt8();
    }

// ---------------------------------------------------------------------------
// TPbk2IconId::IsSame
// ---------------------------------------------------------------------------
//
EXPORT_C TBool TPbk2IconId::operator==( 
        const TPbk2IconId& aRhs ) const
    {
    if ( iOwnersUid == aRhs.iOwnersUid )
        {
        return iIconId == aRhs.iIconId;
        }
    return EFalse;
    }

// end of file
