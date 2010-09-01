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
* Description:  Fetches a brand icon and its bitmask.
*
*/



// INCLUDE FILES
#include <AknsUtils.h>

#include "clogsextbrandfetcher.h"
#include "mlogsextbrandfetcherobserver.h"
#include "logsextconsts.h"
#include "tlogsextutil.h"
#include "simpledebug.h"

// ---------------------------------------------------------------------------
// ConstructL 
// ---------------------------------------------------------------------------
//
void CLogsExtBrandFetcher::ConstructL()
    {
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsExtBrandFetcher* CLogsExtBrandFetcher::NewL(
    const TUint32 aServiceId,
    MLogsExtBrandFetcherObserver& aObserver)
    {
    _LOGP("CLogsExtBrandFetcher::NewL() begin %d" , aServiceId)
    CLogsExtBrandFetcher* self = CLogsExtBrandFetcher::NewLC( aServiceId,
                                                              aObserver );
    CleanupStack::Pop( self );
    _LOG("CLogsExtBrandFetcher::NewL() end" )
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsExtBrandFetcher* CLogsExtBrandFetcher::NewLC(
        const TUint32 aServiceId, 
        MLogsExtBrandFetcherObserver& aObserver )
    {
    _LOG("CLogsExtBrandFetcher::NewLC() begin" )
    CLogsExtBrandFetcher* self = 
        new( ELeave ) CLogsExtBrandFetcher( aServiceId, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    _LOG("CLogsExtBrandFetcher::NewLC() end" )
    return self;
    }

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogsExtBrandFetcher::CLogsExtBrandFetcher( 
    const TUint32 aServiceId, 
    MLogsExtBrandFetcherObserver& aObserver ) :
        iServiceId( aServiceId ),
        iObserver( &aObserver )
    {    
    _LOG("CLogsExtBrandFetcher::CLogsExtBrandFetcher()" )
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsExtBrandFetcher::~CLogsExtBrandFetcher()
    {
    _LOG("CLogsExtBrandFetcher::~CLogsExtBrandFetcher() begin" )
    _LOG("CLogsExtBrandFetcher::~CLogsExtBrandFetcher() end" )
    }


// ---------------------------------------------------------------------------
// Fetches the icon bitmaps from the branding server.
// ---------------------------------------------------------------------------
//
TInt CLogsExtBrandFetcher::Fetch()
    {
    _LOG("CLogsExtBrandFetcher::Fetch begin" )

    TRAPD( error, GetBitmapsFromBrandServerL() );
   
    _LOG("CLogsExtBrandFetcher::Fetch end" )
    return error;
    }


// ---------------------------------------------------------------------------
// Fetches the brand icon's bitmap and bitmapmask from the Branding server
// ---------------------------------------------------------------------------
//
void CLogsExtBrandFetcher::GetBitmapsFromBrandServerL()
    {
    _LOG("CLogsExtBrandFetcher::GetBitmapsFromBrandServerL() begin" ) 
    
    // we need to get the bitmap file from where we can load the bitmap
    RFile bitmapFile;
    TInt bitmapId, bitmapMaskId;
    CleanupClosePushL( bitmapFile );
    TLogsExtUtil::GetFileHandleL( iServiceId, 
                    KBrandedBitmapItemId, 
                    bitmapFile, 
                    bitmapId, 
                    bitmapMaskId );
    CleanupStack::Pop( &bitmapFile );

    //ownership of the file handle is transferred to the observer
    iObserver->BrandIconFetchDoneL( iServiceId,
                                    bitmapFile,
                                    bitmapId,
                                    bitmapMaskId );
    
    _LOG("CLogsExtBrandFetcher::GetBitmapsFromBrandServerL(): end")
    }


// ---------------------------------------------------------------------------
// Returns the service id.
// ---------------------------------------------------------------------------
//
TUint32 CLogsExtBrandFetcher::ServiceId()
    {
    _LOGP("CLogsExtBrandFetcher::ServiceId %d" , iServiceId)
    return iServiceId;
    }

