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
* Description:  Phonebook 2 NamesList context launcher
*
*/


// INCLUDE FILES
#include "cpbk2contextlaunch.h"

// pbk2
#include <Pbk2InternalUID.h>

// cca
#include <mccaparameter.h>
#include <mccaconnection.h>
#include <ccafactory.h>
#include <ccappmycardpluginuids.hrh>

// system
#include <s32mem.h>


// CONSTANTS

/// UID definition
const TUid KContextLaunchMessageUid = { KPbk2ContextLaunchCustomMessageUID };

/// Message version number
const TUint32 KContextLaunchVersionNumber = 1;

/// Panic definitions
enum TPanicCode
    {
    EPanicInvalidVersion = 1,
    EPanicInvalidCommand,
    EPanicMissingRequest
    };

/// Panic definitions
void Panic( TPanicCode aReason )
    { 
    _LIT( KPanicText, "Pbk2ContextLaunch" );
    User::Panic( KPanicText, aReason );   
    }


// --------------------------------------------------------------------------
// CPbk2ContextLaunch::CPbk2ContextLaunch
// --------------------------------------------------------------------------
//
CPbk2ContextLaunch::CPbk2ContextLaunch( 
    MCCAConnection*& aCCAConnection ) :
    CActive( EPriorityUserInput ),
    iCCAConnection( aCCAConnection )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::~CPbk2ContextLaunch
// --------------------------------------------------------------------------
//
CPbk2ContextLaunch::~CPbk2ContextLaunch() 
    {
    Cancel();
    delete iMessage;
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::Uid
// --------------------------------------------------------------------------
//
TUid CPbk2ContextLaunch::Uid()
    {
    return KContextLaunchMessageUid;
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::HandleMessageL
// --------------------------------------------------------------------------
//
void CPbk2ContextLaunch::HandleMessageL( const TDesC8& aMessage )
    {
    // handle one message at a time. overflown messages are ignored
    if( !IsActive() && !iMessage )
        {
        // store request
        iMessage = aMessage.AllocL();
        
        if( !IsAdded() )
            {
            CActiveScheduler::Add( this );
            }
        
        // issue async request
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContextLaunch::RunL()
    {
    __ASSERT_DEBUG( iMessage, Panic( EPanicMissingRequest ) );
    if( !iMessage )
        {
        // request ignored in release builds
        return;
        }
    
    RDesReadStream stream( *iMessage );
    stream.PushL();
    
    // read version (int32)
    if( stream.ReadInt32L() == KContextLaunchVersionNumber )
        {
        // read command (int32)
        const TUint32 command = stream.ReadInt32L();
        switch( command )
            {
            case ECmdLaunchMyCard:
                {
                // read view uid (int32)
                const TUid viewUID = TUid::Uid( stream.ReadInt32L() );
                
                // launch
                LaunchMyCardL( viewUID );
                break;
                }
                
            case ECmdLaunchContactCard:
                {
                // read view uid (int32)
                const TUid viewUID = TUid::Uid( stream.ReadInt32L() );

                // read packed link's length
                const TInt length = stream.ReadInt32L();
                HBufC8* pack = HBufC8::NewLC( length );
                TPtr8 packPtr( pack->Des() );
                stream.ReadL( packPtr, length );

                // launch
                LaunchCCAL( *pack, viewUID );
                CleanupStack::PopAndDestroy( pack );
                break;
                }
                
            default:
                {
                __ASSERT_DEBUG( EFalse, Panic( EPanicInvalidCommand ) );
                break;
                }
            }
        }
    else
        {
        __ASSERT_DEBUG( EFalse, Panic( EPanicInvalidVersion ) );
        }
    
    CleanupStack::PopAndDestroy(); // stream
    delete iMessage;
    iMessage = NULL;
    Deque();
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContextLaunch::DoCancel()
    {
    // nothing to cancel
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2ContextLaunch::RunError( TInt /*aError*/ )
    {
    // reset request
    delete iMessage;
    iMessage = NULL;
    
    Deque();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::LaunchCCAL
// --------------------------------------------------------------------------
//
void CPbk2ContextLaunch::LaunchCCAL( 
    const TDesC8& aPackedLink, 
    TUid aViewUid )
    {       
    // launch parameters
    MCCAParameter* parameter = TCCAFactory::NewParameterL();
    CleanupClosePushL( *parameter );
    parameter->SetConnectionFlag(MCCAParameter::ENormal);
    parameter->SetContactDataFlag(MCCAParameter::EContactLink);
    if( aViewUid != KNullUid )
        {
        parameter->SetLaunchedViewUid( aViewUid );
        }
    
    // convert packed link into 16-bit descriptor
    HBufC16* link = HBufC16::NewLC( aPackedLink.Length() );
    link->Des().Copy( aPackedLink );
    parameter->SetContactDataL( *link );
    CleanupStack::PopAndDestroy( link );  

    // launch
    if(!iCCAConnection)
        {
        iCCAConnection = TCCAFactory::NewConnectionL();
        }
    iCCAConnection->LaunchAppL( *parameter );
    CleanupStack::Pop(); // parameter is taken care by iCCAConnection
    }

// --------------------------------------------------------------------------
// CPbk2ContextLaunch::LaunchMyCardL
// --------------------------------------------------------------------------
//
void CPbk2ContextLaunch::LaunchMyCardL( TUid aViewUid )
    {
    if( aViewUid == KNullUid )
        {
        // default mycard view 
        aViewUid = TUid::Uid( KCCAMyCardPluginImplmentationUid );
        }
    
    // launch parameters
    MCCAParameter* parameter = TCCAFactory::NewParameterL();
    CleanupClosePushL( *parameter );
    parameter->SetConnectionFlag( MCCAParameter::ENormal );
    parameter->SetLaunchedViewUid( aViewUid );

    // launch
    if(!iCCAConnection)
        {
        iCCAConnection = TCCAFactory::NewConnectionL();
        }
    iCCAConnection->LaunchAppL( *parameter );
    CleanupStack::Pop(); // parameter is taken care by iCCAConnection
    }

//  End of File
