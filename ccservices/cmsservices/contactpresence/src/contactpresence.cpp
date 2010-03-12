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
* Description:  contact presence
*
*/


// INCLUDE FILES

#include <e32std.h>
#include <e32debug.h>
#include <cbsfactory.h>
#include <flogger.h>

#include "contactpresence.h"
#include <mcontactpresenceobs.h>
#include "presenceiconinfo.h"
#include "presencebrandedicon.h"

#include "presenceiconinfolistener.h"

#include <contactpresencebrandids.h> 

const TInt KDefaultBitmapSize(10);

// ================= MEMBER FUNCTIONS =======================
//

// ----------------------------------------------------------
// CContactPresence::CContactPresence
// ----------------------------------------------------------
//
CContactPresence::CContactPresence( MContactPresenceObs& aObs )
    : iObs( aObs ),
    iIconReqList(CPresenceBrandedIcon::LinkOffset()        )
    {
    }

// ----------------------------------------------------------
// CContactPresence::~CContactPresence
// ----------------------------------------------------------
//
CContactPresence::~CContactPresence()
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::~CContactPresence starts this=%d" ),(TInt)this );
#endif
    DeleteAllRequests();
    delete iBrandingFactory;

    delete iListener;
    iListener = NULL;

#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::~CContactPresence ends this=%d" ),(TInt)this );
#endif
    }

// -----------------------------------------------------------------------------
// CContactPresence::Close
// -----------------------------------------------------------------------------
void CContactPresence::Close()
    {
    delete this;
    }

// ----------------------------------------------------------
// CContactPresence::NewL
// ----------------------------------------------------------
//
CContactPresence* CContactPresence::NewL(
    MContactPresenceObs& aObs )
    {
    CContactPresence* self = new (ELeave) CContactPresence(aObs );
    CleanupStack::PushL( self );
    self->ConstructL( NULL   );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CContactPresence::NewL
// ----------------------------------------------------------
//
CContactPresence* CContactPresence::NewL(
    CVPbkContactManager& aContactManager,
    MContactPresenceObs& aObs )
    {
    CContactPresence* self = new (ELeave) CContactPresence(aObs );
    CleanupStack::PushL( self );
    self->ConstructL( &aContactManager );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CContactPresence::ConstructL
// ---------------------------------------------------------------------------
//
void CContactPresence::ConstructL( CVPbkContactManager* aManager )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::ConstructL this=%d" ), (TInt)this );
#endif
    iBrandingFactory = CBSFactory::NewL(KCPBrandDefaultId, KCPBrandAppId );
    iSize.SetSize( KDefaultBitmapSize, KDefaultBitmapSize );

    iListener = CPresenceIconInfoListener::NewL( aManager, iObs );
    }

// ----------------------------------------------------------
// CContactPresence::SubscribePresenceInfoL
// ----------------------------------------------------------
//
void CContactPresence::SubscribePresenceInfoL(
        const MDesC8Array& aLinkArray  )
    {
#ifdef _DEBUG
    TInt count = aLinkArray.MdcaCount();
    CContactPresence::WriteToLog( _L8( "CContactPresence::SubscribePresenceInfoL array count=%d this=%d" ), count, (TInt)this );
#endif
    iListener->SubscribePresenceInfoL( aLinkArray );
    }

// ----------------------------------------------------------
// CContactPresence::GetPresenceIconFileL
// ----------------------------------------------------------
//
TInt CContactPresence::GetPresenceIconFileL(
        const TDesC8& aBrandId,
        const TDesC8& aElementId /*AKA aImageId*/ )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::GetPresenceIconFileL this=%d" ), (TInt)this );
#endif
    // We always create a new instance, since they delete itself in complication
    iOpId++;

    CPresenceBrandedIcon* icon = CPresenceBrandedIcon::NewL(iBrandingFactory, iObs );
    iIconReqList.AddLast( *icon );
    icon->StartGetIconInfoL( iOpId, aBrandId, aElementId, iSize );

    return iOpId;
    }

// ----------------------------------------------------------
//
TInt CContactPresence::GetPresenceInfoL( const TDesC8& aPackedLink )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::GetPresenceInfoL this=%d" ), (TInt)this );
#endif 
    iOpId++; 
    
    iListener->GetPresenceInfoL( aPackedLink, iOpId );
    
    return iOpId;    
    }

// ----------------------------------------------------------
// CContactPresence::SetPresenceIconSize
// ----------------------------------------------------------
//
void CContactPresence::SetPresenceIconSize(
    const TSize aSize )
    {
    iSize = aSize;
    }

// ----------------------------------------------------------
// CContactPresence::CancelSubscribePresenceInfo
// ----------------------------------------------------------
//
void CContactPresence::CancelSubscribePresenceInfo( const MDesC8Array& aLinkArray )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::CancelSubscribePresenceInfo this=%d" ), (TInt)this );
#endif

    iListener->CancelSubscribePresenceInfo( aLinkArray );
    }

// ----------------------------------------------------------
// CContactPresence::CancelAll
// ----------------------------------------------------------
//
void CContactPresence::CancelAll()
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::CancelAll this=%d" ), (TInt)this );
#endif
    DeleteAllRequests();
    }

// ----------------------------------------------------------
// CContactPresence::CancelOperation
// ----------------------------------------------------------
//
void CContactPresence::CancelOperation( TInt aOpId )
    {
#ifdef _DEBUG
    CContactPresence::WriteToLog( _L8( "CContactPresence::CancelOperation this=%d" ), (TInt)this );
#endif
        
    // synchronous is good enough
    CPresenceBrandedIcon* iconH = CContactPresence::SearchBrandedIcon( aOpId );
    if ( iconH )
        {
        iconH->Cancel();
        iconH->Destroy();
        }
    else
        {        
        iListener->CancelGet( aOpId );       
        }
    return;
    }

// ----------------------------------------------------------
//
void CContactPresence::DeleteAllRequests()
    {
    if ( iListener )
        {
        iListener->DeleteAllRequests();
        }
    else
        {
        }

    TDblQueIter<CPresenceBrandedIcon> rIter2( iIconReqList );
    rIter2.SetToFirst();

    while ( rIter2 )
        {
        CPresenceBrandedIcon* req2 = rIter2;
        rIter2++;
        req2->Destroy();
        }
    
    return;
    }

// ----------------------------------------------------------
//
CPresenceBrandedIcon* CContactPresence::SearchBrandedIcon( TInt aOpId )
    {
    TDblQueIter<CPresenceBrandedIcon> rIter( iIconReqList );
    rIter.SetToFirst();

    while ( rIter )
        {
        CPresenceBrandedIcon* req = rIter;
        rIter++;
        if ( req->GetOpId() == aOpId )
            {
            return req;
            }
        }
    return (CPresenceBrandedIcon*)NULL;
    }

#ifdef _DEBUG
// ----------------------------------------------------
// CContactPresence::WriteToLog
//
// ----------------------------------------------------
//
void CContactPresence::WriteToLog( TRefByValue<const TDesC8> aFmt,... )
    {
    _LIT( KMyLogDir, "CCA");
    _LIT( KMyLogFile, "cp.txt");
    const TInt KMyBufferMaxSize = 100;
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KMyBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KMyLogDir, KMyLogFile, EFileLoggingModeAppend, buf );
    RDebug::RawPrint( buf );
    }
#endif  //_DEBUG

