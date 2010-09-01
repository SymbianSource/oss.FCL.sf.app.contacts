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
* Description:  A SIM field type filter.
*
*/


// INCLUDE FILES
#include "CVPbkSimFieldTypeFilter.h"

// System includes
#include <s32strm.h>
#include <s32mem.h>

// ============================= LOCAL FUNCTIONS ============================


// SIM field type filter static data
namespace KSimFieldTypeFilterStatic
    {
    const TUint8 KFilterVersionNumber = 1;
    }

namespace KSimFieldTypeFilterExternalizeSizes
    {
    const TInt KVersionNumberSize = 1;
    const TInt KCriteriaSize = 16;
    }

/// Unnamed namespace for local definitions
namespace {

TInt DoCalculateBufferSize()
    {
    TInt bufferSize = KSimFieldTypeFilterExternalizeSizes::KVersionNumberSize;
    bufferSize += KSimFieldTypeFilterExternalizeSizes::KCriteriaSize;
        
    return bufferSize;
    }

void DoFillBuffer( RWriteStream& aWriteStream,
        const CVPbkSimFieldTypeFilter& aThis )
    {
    // Write version number
    aWriteStream.WriteUint8L(
        KSimFieldTypeFilterStatic::KFilterVersionNumber );

    // Write criteria
    aWriteStream.WriteUint16L( aThis.FilteringCriteria() );
    }    

} /// unnamed namespace

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CVPbkSimFieldTypeFilter::CVPbkSimFieldTypeFilter
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkSimFieldTypeFilter::CVPbkSimFieldTypeFilter()
    {
    }

// --------------------------------------------------------------------------
// CVPbkSimFieldTypeFilter::~CVPbkSimFieldTypeFilter
// --------------------------------------------------------------------------
//
CVPbkSimFieldTypeFilter::~CVPbkSimFieldTypeFilter()
    {   
    }

// --------------------------------------------------------------------------
// CVPbkSimFieldTypeFilter::AppendFilteringFlag
// --------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimFieldTypeFilter::AppendFilteringFlag
        ( TUint16 aFilteringFlag )
    {
    iFilteringCriteria |= aFilteringFlag;
    }

// --------------------------------------------------------------------------
// CVPbkSimFieldTypeFilter::AppendFilteringFlag
// --------------------------------------------------------------------------
//
EXPORT_C TUint16 CVPbkSimFieldTypeFilter::FilteringFlagForSimFieldType
        ( TVPbkSimCntFieldType aSimFieldType ) const
    {
    TUint16 ret = ESimFilterCriteriaNoFiltering;

    switch ( aSimFieldType )
        {
        case EVPbkSimName:
            {
            ret = ESimFilterCriteriaName;
            break;
            }

        case EVPbkSimGsmNumber:
            {
            ret = ESimFilterCriteriaGsmNumber;
            break;
            }
         
         case EVPbkSimAdditionalNumber1:  // FALLTHROUGH
         case EVPbkSimAdditionalNumber2:  // FALLTHROUGH
         case EVPbkSimAdditionalNumber3:  // FALLTHROUGH
         case EVPbkSimAdditionalNumberLast: // the same as EVPbkSimAdditionalNumber.
            {
            ret = ESimFilterCriteriaAdditionalNumber;
            break;
            }

        case EVPbkSimNickName:  // FALLTHROUGH
        case EVPbkSimReading:
            {
            ret = ESimFilterCriteriaSecondName;
            break;
            }

        case EVPbkSimEMailAddress:
            {
            ret = ESimFilterCriteriaEmailAddress;
            break;
            }

        default:
            {
            ret = ESimFilterCriteriaFilterOut;
            break;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CVPbkSimFieldTypeFilter::FilteringCriteria
// --------------------------------------------------------------------------
//
EXPORT_C TUint16 CVPbkSimFieldTypeFilter::FilteringCriteria() const
    {
    return iFilteringCriteria;
    }

// --------------------------------------------------------------------------
// CVPbkSimFieldTypeFilter::ExternalizeLC
//  SIM field type filter package format 1:
//  stream ^= VersionNumber FilteringFlags
//  FilteringFlags ^= TInt16
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CVPbkSimFieldTypeFilter::ExternalizeLC() const
    {
    const TInt bufferSize = DoCalculateBufferSize();
    HBufC8* buffer = HBufC8::NewLC( bufferSize );
    TPtr8 bufPtr( buffer->Des() );
    RDesWriteStream writeStream( bufPtr );
    writeStream.PushL();
    DoFillBuffer( writeStream, *this );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    return buffer;
    }

// --------------------------------------------------------------------------
// CVPbkSimFieldTypeFilter::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CVPbkSimFieldTypeFilter::InternalizeL( const TDesC8& aBuffer )
    {
    // Reset
    iFilteringCriteria = ESimFilterCriteriaNoFiltering;

    RDesReadStream readStream( aBuffer );
    readStream.PushL();

    // Version number
    const TInt vsn = readStream.ReadUint8L();

    if ( vsn != KSimFieldTypeFilterStatic::KFilterVersionNumber )
        {
        User::Leave( KErrNotSupported );
        }

    // Read criteria
    iFilteringCriteria = readStream.ReadUint16L();

    CleanupStack::PopAndDestroy( &readStream );
    }

//  End of File
