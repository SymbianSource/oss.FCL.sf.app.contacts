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
* Description:  Phonebook 2 voice dial handler.
*
*/


// INCLUDE FILES
#include "cpvtvoicedialhandler.h"

// Virtual Phonebook
#include <cvpbkcontactidconverter.h>
#include <mvpbkcontactlink.h>

// System includes
#include <vuinamedialer.h>
#include <cpbkcontactitem.h>

// Debugging headers
#include <pbk2debug.h>

/// Unnamed namespace for local definitions
namespace {

// MODULE DATA STRUCTURES

#ifdef _DEBUG
enum TPanicCodes
    {
    EPanicPreCond_AddVoiceTagL = 1,
    EPanicPreCond_PlaybackVoiceTagL,
    EPanicPreCond_ChangeVoiceTagL,
    EPanicPreCond_EraseVoiceTagL,
    EPanicPreCond_GetContactsWithVoiceTagL,
    EPanicPreCond_VoiceTagAvailable
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPvtVoiceDialHandler");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPvtVoiceDialHandler::CPvtVoiceDialHandler
// --------------------------------------------------------------------------
//
inline CPvtVoiceDialHandler::CPvtVoiceDialHandler()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPvtVoiceDialHandler::~CPvtVoiceDialHandler
// --------------------------------------------------------------------------
//
CPvtVoiceDialHandler::~CPvtVoiceDialHandler()
    {
    delete iVoiceDialer;
    }

// --------------------------------------------------------------------------
// CPvtVoiceDialHandler::NewL
// --------------------------------------------------------------------------
//
CPvtVoiceDialHandler* CPvtVoiceDialHandler::NewL()
    {
    CPvtVoiceDialHandler* self = new ( ELeave ) CPvtVoiceDialHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPvtVoiceDialHandler::ConstructL
// --------------------------------------------------------------------------
//
void CPvtVoiceDialHandler::ConstructL()
    {
    iVoiceDialer = CVoiceDialer::NewL();
    }

// --------------------------------------------------------------------------
// CPvtVoiceDialHandler::PlaybackVoiceTagL
// --------------------------------------------------------------------------
//
void CPvtVoiceDialHandler::PlaybackVoiceTagL
        ( const MVPbkContactLink& aLink, TInt aFieldIndex ) const
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPvtVoiceDialHandler::PlaybackVoiceTagL" ) );

    CVPbkContactIdConverter* idConverter =
        CVPbkContactIdConverter::NewL( aLink.ContactStore() );
    CleanupStack::PushL( idConverter );

    // PreCond:
    __ASSERT_DEBUG( iVoiceDialer && idConverter,
        Panic( EPanicPreCond_PlaybackVoiceTagL ) );

    TInt32 id = idConverter->LinkToIdentifier( aLink );
    if ( id != KNullContactId )
        {
        PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
            ( "CPvtVoiceDialHandler::PlaybackVoiceTagL, CPlaybackVoiceTagDialog(%d, %d)"),
            id, aFieldIndex );

        CPlaybackVoiceTagDialog* dlg =
            new ( ELeave ) CPlaybackVoiceTagDialog( id, aFieldIndex );

        PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
            ( "CPvtVoiceDialHandler::PlaybackVoiceTagL, launch dialog" ) );

        dlg->ExecuteLD();

        PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
            ( "CPvtVoiceDialHandler::PlaybackVoiceTagL, CPlaybackVoiceTagDialog end" ) );
        }

    CleanupStack::PopAndDestroy( idConverter );

    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPvtVoiceDialHandler::PlaybackVoiceTagL end" ) );
    }

// --------------------------------------------------------------------------
// CPvtVoiceDialHandler::GetContactsWithVoiceTagL
// --------------------------------------------------------------------------
//
void CPvtVoiceDialHandler::GetContactsWithVoiceTagL
        ( CVoiceTagContact& aVoiceTagContact ) const
    {
    // PreCond:
    __ASSERT_DEBUG( iVoiceDialer,
        Panic( EPanicPreCond_GetContactsWithVoiceTagL ) );

    iVoiceDialer->GetContactsWithVoiceTagL( aVoiceTagContact );
    }

// --------------------------------------------------------------------------
// CPvtVoiceDialHandler::VoiceTagAvailable
// --------------------------------------------------------------------------
//
TBool CPvtVoiceDialHandler::VoiceTagAvailable() const
    {
    // PreCond:
    __ASSERT_DEBUG( iVoiceDialer, Panic( EPanicPreCond_VoiceTagAvailable ) );

    return iVoiceDialer->VoiceTagAvailable();
    }

//  End of File
