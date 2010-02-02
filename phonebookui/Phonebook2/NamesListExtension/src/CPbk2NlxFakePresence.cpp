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

#include "CPbk2NlxFakePresence.h"

#include <coemain.h>
#include <e32math.h>

#include <MVPbkContactLink.h>
#include <mcontactpresenceobs.h>

 _LIT( KEcePresenceMbmFile, "\\resource\\presenceIcons.mbm" );

_LIT8( KEceTxtSomeBrand, "someBrand");
_LIT8( KEceTxtConnecting,   "connecting" );
_LIT8( KEceTxtAvailable,    "available" );
_LIT8( KEceTxtNotAvailable, "notavailable" );
_LIT8( KEceTxtDoNotDisturb, "donotdisturb" );


CPbk2NlxFakePresenceGenerator* CPbk2NlxFakePresenceGenerator::NewLC(
    MContactPresenceObs& aObserver,
    CVPbkContactManager& aContactManager,
    const MVPbkContactLink& aContactLink )
    {
    CPbk2NlxFakePresenceGenerator* self =
        new ( ELeave ) CPbk2NlxFakePresenceGenerator( aObserver, aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLink );
    return self;
    }


CPbk2NlxFakePresenceGenerator::~CPbk2NlxFakePresenceGenerator()
    {
    delete iContactLink;
    }

CPbk2NlxFakePresenceGenerator::CPbk2NlxFakePresenceGenerator(
    MContactPresenceObs& aObserver,
    CVPbkContactManager& aContactManager ) :
    CTimer(EPriorityStandard),
    iObserver( aObserver ),
    iContactManager( aContactManager )
    {
    }

void CPbk2NlxFakePresenceGenerator::ConstructL( const MVPbkContactLink& aContactLink )
    {
    CTimer::ConstructL();
    iContactLink = aContactLink.CloneLC();
    CleanupStack::Pop(); // iContactLink
    CActiveScheduler::Add( this );
    iNextIconType = 0;

    //Random initialization
    TTime now;
    now.HomeTime();
    iRandomSeed = now.Int64();
    Math::Rand( iRandomSeed );
    }

void CPbk2NlxFakePresenceGenerator::Start()
    {
    RDebug::Print(_L("FAKE: Received subscription request"));
    TInt delayMs = 500 + Random( 1500 ) ;
    After( delayMs * 1000);
    }

TBool CPbk2NlxFakePresenceGenerator::Equals( const MVPbkContactLink& aContactLink ) const
    {
    return iContactLink->IsSame( aContactLink );
    }

void CPbk2NlxFakePresenceGenerator::RunL()
    {
    RDebug::Print(_L("FAKE: completing subscription request"));

    if ( iStatus.Int() != KErrCancel )
        {
        TPtrC8 elementId( NextIconElement() );
        if ( elementId.Length() > 0 )
            {
            HBufC8* package = iContactLink->PackLC();
            iObserver.ReceiveIconInfoL( *package, KEceTxtSomeBrand, elementId );
            CleanupStack::PopAndDestroy( package );

            TInt delayMs = 800 + Random( 4000 );
            After( delayMs * 1000 ); // Change presence status after a while
            }
        else
            {
            __ASSERT_DEBUG( EFalse, User::Panic(_L("Pbk FakePresence"), 1) );
            }
        }
    }

TPtrC8 CPbk2NlxFakePresenceGenerator::NextIconElement()
    {
    TPtrC8 elementId;
    if (iNextIconType == 0 )
        {
        // It was the first time
        iNextIconType = 1;
        elementId.Set( KEceTxtConnecting );
        }
    else
        {
        // Not the first time
        TInt choice = Random( 3 );
        if ( choice == 0 )
            {
            elementId.Set( KEceTxtAvailable );
            }
        else if ( choice == 1 )
            {
            elementId.Set( KEceTxtNotAvailable );
            }
        else if ( choice == 2 )
            {
            elementId.Set( KEceTxtDoNotDisturb );
            }
        }

    return elementId;
    }

TInt CPbk2NlxFakePresenceGenerator::Random( TInt aMax ) const
    {
    return Math::Rand( iRandomSeed ) % aMax;
    }

//========================================================================

CPbk2NlxFakeIconRequest* CPbk2NlxFakeIconRequest::NewLC(
    MContactPresenceObs& aObserver,
    CVPbkContactManager& aContactManager,
    const TDesC8& aBrandId,
    const TDesC8& aElementId )
    {
    CPbk2NlxFakeIconRequest* self =
        new ( ELeave ) CPbk2NlxFakeIconRequest( aObserver, aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL( aBrandId, aElementId );
    return self;
    }


CPbk2NlxFakeIconRequest::~CPbk2NlxFakeIconRequest()
    {
    delete iBrandId;
    delete iElementId;
    }

CPbk2NlxFakeIconRequest::CPbk2NlxFakeIconRequest(
    MContactPresenceObs& aObserver,
    CVPbkContactManager& aContactManager ) :
    CTimer(EPriorityStandard),
    iObserver( aObserver ),
    iContactManager( aContactManager )
    {
    }

void CPbk2NlxFakeIconRequest::ConstructL(
    const TDesC8& aBrandId,
    const TDesC8& aElementId )
    {
    CTimer::ConstructL();
    CActiveScheduler::Add( this );
    iBrandId = aBrandId.AllocL();
    iElementId = aElementId.AllocL();
    }

void CPbk2NlxFakeIconRequest::Start()
    {
    RDebug::Print(_L("FAKE: Received icon request"));
    After( 200000 );
    }

TBool CPbk2NlxFakeIconRequest::Equals( const TDesC8& aBrandId, const TDesC8& aElementId ) const
    {
    return iBrandId->Compare( aBrandId ) == 0 && iElementId->Compare( aElementId ) == 0;
    }

void CPbk2NlxFakeIconRequest::RunL()
    {
    RDebug::Print(_L("FAKE: completing icon request"));

    if ( iStatus.Int() != KErrCancel )
        {
        // We are ignoring the brand for now.
        const TInt iconIndex = IconIndexForElementId( *iElementId );

        if ( iconIndex != KErrNotFound )
            {
            TFindFile findFile( CCoeEnv::Static()->FsSession() );
            User::LeaveIfError( findFile.FindByDir( KEcePresenceMbmFile, KNullDesC ) );        

            CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL( bitmap );
            User::LeaveIfError( bitmap->Load( findFile.File(), iconIndex ) );

            CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap;
            CleanupStack::PushL( bitmapMask );
            User::LeaveIfError( bitmapMask->Load( findFile.File(), iconIndex + 1 ) ); // mask offset is 1

            iObserver.ReceiveIconFileL( *iBrandId, *iElementId, bitmap, bitmapMask );

            CleanupStack::Pop( bitmapMask );
            CleanupStack::Pop( bitmap );
            }
        else
            {
            __ASSERT_DEBUG( EFalse, User::Panic(_L("Pbk FakePresence"), 0) );
            }
        }
    }

TInt CPbk2NlxFakeIconRequest::IconIndexForElementId( const TDesC8& aElementId )
    {
    TInt index = KErrNotFound;
    if ( aElementId.Compare( KEceTxtConnecting ) == 0 )
        {
        index = 0;
        }
    else if ( aElementId.Compare( KEceTxtAvailable ) == 0 )
        {
        index = 2;
        }
    else if ( aElementId.Compare( KEceTxtNotAvailable ) == 0 )
        {
        index = 4;
        }
    else if ( aElementId.Compare( KEceTxtDoNotDisturb ) == 0 )
        {
        index = 6;
        }
    return index;
    }

//========================================================================

CPbk2NlxFakePresence* CPbk2NlxFakePresence::NewL(
    MContactPresenceObs& aObserver,
    CVPbkContactManager& aContactManager )
    {
    CPbk2NlxFakePresence* self =
        new ( ELeave ) CPbk2NlxFakePresence( aObserver, aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }


CPbk2NlxFakePresence::~CPbk2NlxFakePresence()
    {
    CancelAll();
    }

CPbk2NlxFakePresence::CPbk2NlxFakePresence(
    MContactPresenceObs& aObserver,
    CVPbkContactManager& aContactManager ) :
    iObserver( aObserver ),
    iContactManager( aContactManager )
    {
    }

void CPbk2NlxFakePresence::ConstructL()
    {
    }

void CPbk2NlxFakePresence::Close()
    {
    delete this;
    }

void CPbk2NlxFakePresence::SubscribePresenceInfoL( const MDesC8Array& aLinkArray )
    {
    for ( TInt x = 0; x < aLinkArray.MdcaCount(); ++x )
        {
        TPtrC8 packedItem( aLinkArray.MdcaPoint( x ) );
        // Unpack the link
        CVPbkContactLinkArray* linkArray =
            CVPbkContactLinkArray::NewLC( packedItem, iContactManager.ContactStoresL() );
        __ASSERT_ALWAYS( linkArray->Count() > 0, User::Panic(_L("PbkPresenceFake"), 0) );
        const MVPbkContactLink& link = linkArray->At( 0 );
        CPbk2NlxFakePresenceGenerator* generator = FindGenerator( link );
        if ( !generator )
            {
            // Need to create a generator for this contact identity
            CPbk2NlxFakePresenceGenerator *newGener =
                CPbk2NlxFakePresenceGenerator::NewLC( iObserver, iContactManager, link );
            User::LeaveIfError( iGenerators.Append( newGener ) );
            CleanupStack::Pop( newGener );
            newGener->Start();
            }
        CleanupStack::PopAndDestroy( linkArray );
        }
    }


void CPbk2NlxFakePresence::CancelSubscribePresenceInfo( const MDesC8Array& aLinkArray )
    {
    const TInt count = aLinkArray.MdcaCount();
    for ( TInt n = 0; n < count; ++n )
        {
        TPtrC8 packedItem( aLinkArray.MdcaPoint( n ) );
        // Unpack the link
        CVPbkContactLinkArray* linkArray =
            CVPbkContactLinkArray::NewLC( packedItem, iContactManager.ContactStoresL() );
        __ASSERT_ALWAYS( linkArray->Count() > 0, User::Panic(_L("PbkPresenceFake"), 0) );
        const MVPbkContactLink& link = linkArray->At( 0 );

        CancelAndDeletePresenceGenerator( link );
        CleanupStack::PopAndDestroy( linkArray );
        }
    }


TInt CPbk2NlxFakePresence::GetPresenceIconFileL(
        const TDesC8& aBrandId, const TDesC8& aElementId )
    {
    // First check to see if we already have a pending icon request.
    const TInt count = iIconRequests.Count();
    TBool alreadyRequested = EFalse;
    for ( TInt n = 0; n < count && !alreadyRequested; ++n )
        {
        const CPbk2NlxFakeIconRequest& request = *iIconRequests[n];
        alreadyRequested = request.Equals( aBrandId, aElementId ) && request.IsActive();
        }

    if ( !alreadyRequested )
        {
        // Need to create a request
        CPbk2NlxFakeIconRequest* request =
            CPbk2NlxFakeIconRequest::NewLC( iObserver, iContactManager, aBrandId, aElementId );
        User::LeaveIfError( iIconRequests.Append( request ) );
        CleanupStack::Pop( request );
        request->Start();
        }
    }

void CPbk2NlxFakePresence::CancelAll()
    {
    // Cancel and delete all generators
    for ( TInt n = 0; n < iGenerators.Count(); ++n )
        {
        iGenerators[n]->Cancel();
        }
    iGenerators.ResetAndDestroy();    

    // Cancel and delete all icon requests
    for ( TInt n = 0; n < iIconRequests.Count(); ++n )
        {
        iIconRequests[n]->Cancel();
        }
    iIconRequests.ResetAndDestroy();
    }

void CPbk2NlxFakePresence::CancelAndDeletePresenceGenerator( const MVPbkContactLink& aLink )
    {
    for ( TInt n = 0; n < iGenerators.Count(); ++n )
        {
        CPbk2NlxFakePresenceGenerator* gen = iGenerators[n];
        if ( gen->Equals( aLink ) )
            {
            gen->Cancel();
            iGenerators.Remove( n );
            delete gen;
            break;
            }
        }
    }

CPbk2NlxFakePresenceGenerator* CPbk2NlxFakePresence::FindGenerator( const MVPbkContactLink& aLink )
    {
    CPbk2NlxFakePresenceGenerator* generator = NULL;
    for ( TInt n = 0; n < iGenerators.Count() && !generator; ++n )
        {
        CPbk2NlxFakePresenceGenerator* tmpGen = iGenerators[n];
        if ( tmpGen->Equals( aLink ) )
            {
            generator = tmpGen;
            }
        }
    return generator;
    }

// End of File
