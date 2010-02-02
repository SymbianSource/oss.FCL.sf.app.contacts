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

#include "ccapbkcontextlauncherprivate.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS

#include <viewclipartner.h>

#endif // SYMBIAN_ENABLE_SPLIT_HEADERS

#include <viewcli.h>
#include <Pbk2UID.h>
#include <CPbk2ViewState.h>
#include <s32mem.h>
#include <Pbk2InternalUID.h>
#include <featmgr.h>
#include <bldvariant.hrh>

/// Current message version number
const TInt32 KVersionNumber     = 1;

/// Commands
const TInt32 KCommandMyCard     = 1;
const TInt32 KCommandContact    = 2;

/// Custom message UID for context launching
const TUid KMessageUid = { KPbk2ContextLaunchCustomMessageUID };

CCCAPbkContextLauncherPrivate* CCCAPbkContextLauncherPrivate::NewL()
    {
    CCCAPbkContextLauncherPrivate* self =
            new (ELeave) CCCAPbkContextLauncherPrivate();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CCCAPbkContextLauncherPrivate::~CCCAPbkContextLauncherPrivate()
    {
    delete iVwsSessionWrapper;
    delete iBuffer;
    FeatureManager::UnInitializeLib();
    }

inline CCCAPbkContextLauncherPrivate::CCCAPbkContextLauncherPrivate()
    {
    }

inline void CCCAPbkContextLauncherPrivate::ConstructL()
    {
    FeatureManager::InitializeLibL();
    iVwsSessionWrapper = CVwsSessionWrapper::NewL();
    iBuffer = CBufSeg::NewL( 256 );
    }

void CCCAPbkContextLauncherPrivate::DoLaunchCCAViewL(
        const TDesC8* aPackedContactLink,
        const TUid* aUid )
    {
    iBuffer->Reset();
    RBufWriteStream stream( *iBuffer );
    stream.PushL();

    stream.WriteInt32L( KVersionNumber );              // version number
    if( aPackedContactLink )
        {
        // contact card
        stream.WriteInt32L( KCommandContact );               // command
        stream.WriteInt32L( aUid ? aUid->iUid : 0 );         // uid
        stream.WriteInt32L( aPackedContactLink->Length() ); // link's length
        stream.WriteL( *aPackedContactLink );                 // link
        }    
    else
        {
        // mycard
        stream.WriteInt32L( KCommandMyCard );          // command
        stream.WriteInt32L( aUid ? aUid->iUid : 0 );   // uid
        }

    stream.CommitL();
    CleanupStack::PopAndDestroy(); // cleanup stream

    const TVwsViewId viewId( TUid::Uid( KPbk2UID3 ), TUid::Uid(1) );

    // Activate the view
    TInt error = iVwsSessionWrapper->ActivateView(
            viewId, KMessageUid, iBuffer->Ptr(0), 0 );

    iBuffer->Reset();
    User::LeaveIfError( error );
    }

void CCCAPbkContextLauncherPrivate::LaunchMyCardViewL(
        const TUid* aUid )
    {
    if( FeatureManager::FeatureSupported( KFeatureIdffContactsMycard ) )
        {
        DoLaunchCCAViewL( NULL, aUid );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }

void CCCAPbkContextLauncherPrivate::LaunchCCAViewL(
        const TDesC8& aPackedContactLink,
        const TUid* aUid )
    {
    DoLaunchCCAViewL( &aPackedContactLink, aUid );
    }

// End of File
