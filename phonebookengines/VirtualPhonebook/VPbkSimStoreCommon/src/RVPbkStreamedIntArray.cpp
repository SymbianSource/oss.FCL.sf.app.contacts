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
* Description:  Streamed sort order array for SIM field types
*
*/



// INCLUDE FILES
#include "RVPbkStreamedIntArray.h"

#include <s32mem.h>

// CONSTANTS
const TInt KSizeTInt = 4;
const TInt KTwo = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::ExternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkStreamedIntArray::ExternalizeL( 
    RWriteStream& aWriteStream ) const
    {
    TInt count = Count();
    aWriteStream.WriteInt32L( count );
    for ( TInt i = 0; i < count; ++i )
        {
        aWriteStream.WriteInt32L( iIntArray[i] );
        }
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::ExternalizedSize
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RVPbkStreamedIntArray::ExternalizedSize() const
    {
    // The data stream contains the amount of fields and field types
    // Let's optimize this sentence for efficiency:
    // sizeof( TInt ) + Count() * sizeof( TInt )
    return KSizeTInt + Count() << KTwo;   
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::InternalizeL
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkStreamedIntArray::InternalizeL( 
    RReadStream& aReadStream )
    {
    TInt count = aReadStream.ReadInt32L();
    for ( TInt i = 0; i < count; ++i )
        {
        TInt val = static_cast<TInt>( aReadStream.ReadInt32L() );
        iIntArray.AppendL( val );
        }
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::RemoveAll
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkStreamedIntArray::RemoveAll()
    {
    // Removes but doesn't free the memory
    const TInt count = iIntArray.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        iIntArray.Remove( i );
        }
    }
    
// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ExternalizeArrayLC
// 
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* ExternalizeArrayLC( 
    const RVPbkStreamedIntArray& aFieldTypes )
    {
    TInt size = aFieldTypes.ExternalizedSize();
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 ptr( buf->Des() );
    RDesWriteStream stream( ptr );
    CleanupClosePushL( stream );
    stream << aFieldTypes;
    ptr.SetLength( size );
    CleanupStack::PopAndDestroy(); // stream
    return buf;
    }

//  End of File  
