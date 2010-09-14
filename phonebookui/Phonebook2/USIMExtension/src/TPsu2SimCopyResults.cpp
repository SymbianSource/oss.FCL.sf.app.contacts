/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 SIM copy results.
*
*/


// INCLUDE FILES
#include "TPsu2SimCopyResults.h"

// Phonebook 2
#include <pbk2usimuires.rsg>
#include <pbk2commands.rsg>

// System includes
#include <StringLoader.h>
#include <aknnotewrappers.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KZeroContacts( 0 );
const TInt KOneContact( 1 );
const TInt KGranularity( 2 );

} /// namespace

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::TPsu2SimCopyResults
// --------------------------------------------------------------------------
//
TPsu2SimCopyResults::TPsu2SimCopyResults()
    {
    }

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::TPsu2SimCopyResults
// --------------------------------------------------------------------------
//
TPsu2SimCopyResults::TPsu2SimCopyResults( TInt aCopied, TInt aTotal ):
        iCopied( aCopied ), iNotCopied( aTotal - aCopied )
    {
    }

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::TPsu2SimCopyResults
// --------------------------------------------------------------------------
//
TPsu2SimCopyResults::TPsu2SimCopyResults( TInt aCopied, const TDesC& aName ):
        iCopied( aCopied ), iName( aName )
    {
    }

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::ShowNoteL
// --------------------------------------------------------------------------
//
void TPsu2SimCopyResults::ShowNoteL()
    {
    if ( iName.Length() == 0 )
        {
        // Multiple contacts were marked to be copied
        ShowMultipleCopyNoteL();
        }
    else
        {
        // Only one contact was marked to be copied
        ShowSingleCopyNoteL();
        }
    }

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::ShowErrorNoteL
// --------------------------------------------------------------------------
//
void TPsu2SimCopyResults::ShowErrorNoteL()
    {
    HBufC* prompt = StringLoader::LoadLC( R_QTN_PBCOP_NOTE_COPY_NOT_WORK );
    CAknErrorNote* note = new ( ELeave ) CAknErrorNote( ETrue );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::ShowPartlyCopiedNoteL
// --------------------------------------------------------------------------
//
void TPsu2SimCopyResults::ShowPartlyCopiedNoteL()
    {
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    HBufC* prompt = NULL;

    if ( iName.Length() == 0 )
        {
        // For multiple contacts copy
        prompt = StringLoader::LoadLC
            ( R_QTN_PBCOP_NOTE_ENTRIES_COPIED_PARTLY );
        }
    else
        {
        // For one contact copy
        prompt = StringLoader::LoadLC
            ( R_QTN_PBCOP_NOTE_ENTRY_COPIED_PARTLY );
        }
    dlg->ExecuteLD( *prompt );

    CleanupStack::PopAndDestroy( prompt );
    }

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::ShowSingleCopyNoteL
// --------------------------------------------------------------------------
//
inline void TPsu2SimCopyResults::ShowSingleCopyNoteL()
    {
    HBufC* prompt = NULL;
    CAknResourceNoteDialog* dlg = NULL;
    if ( iCopied > 0 )
        {
        prompt = StringLoader::LoadLC
            ( R_QTN_PBCOP_NOTE_CONTACT_COPIED_PB2, iName );
        dlg = new ( ELeave ) CAknConfirmationNote( ETrue );
        }
    else
        {
        prompt = StringLoader::LoadLC
            ( R_QTN_PBCOP_NOTE_ENTRY_NOT_COPIED, iName );
        dlg = new ( ELeave ) CAknInformationNote( ETrue );
        }
    dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

// --------------------------------------------------------------------------
// TPsu2SimCopyResults::ShowMultipleCopyNoteL
// --------------------------------------------------------------------------
//
inline void TPsu2SimCopyResults::ShowMultipleCopyNoteL()
    {
    if ( iNotCopied == KZeroContacts )
        {
        HBufC* prompt = StringLoader::LoadLC
            ( R_QTN_PBCOP_NOTE_N_ENTRY_COPY_PB, iCopied );

        CAknConfirmationNote* dlg =
            new ( ELeave ) CAknConfirmationNote( ETrue );
        dlg->ExecuteLD( *prompt );

        CleanupStack::PopAndDestroy( prompt );
        }
    else
        {
        HBufC* errorPrompt = NULL;
        if ( iCopied == KOneContact && iNotCopied == KOneContact )
            {
            errorPrompt = StringLoader::LoadLC
                ( R_QTN_PBCOP_INFO_ONE_OK_ONE_NOT );
            }
        else if ( iCopied == KOneContact && iNotCopied > KOneContact )
            {
            errorPrompt = StringLoader::LoadLC
                ( R_QTN_PBCOP_INFO_ONE_OK_N_NOT, iNotCopied );
            }
        else if ( ( iCopied == KZeroContacts || iCopied > KOneContact ) &&
                  ( iNotCopied == KOneContact ) )
            {
            errorPrompt = StringLoader::LoadLC
                ( R_QTN_PBCOP_INFO_N_OK_ONE_NOT, iCopied );
            }
        else if ( ( iCopied == KZeroContacts || iCopied > KOneContact ) &&
                  ( iNotCopied == KZeroContacts || iNotCopied > KOneContact ) )
            {
            CArrayFixFlat<TInt>* array =
                new( ELeave ) CArrayFixFlat<TInt>( KGranularity );
            CleanupStack::PushL( array );

            array->AppendL( iCopied );
            array->AppendL( iNotCopied );
            errorPrompt = StringLoader::LoadL
                ( R_QTN_PBCOP_INFO_N_OK_N_NOT, *array );

            CleanupStack::PopAndDestroy( array );
            CleanupStack::PushL( errorPrompt );
            }

        if ( errorPrompt )
            {

	        CAknInformationNote* dlg =
	            new ( ELeave ) CAknInformationNote( ETrue );
	        dlg->ExecuteLD( *errorPrompt );
	        CleanupStack::PopAndDestroy( errorPrompt );
            }
        }
    }
//  End of File
