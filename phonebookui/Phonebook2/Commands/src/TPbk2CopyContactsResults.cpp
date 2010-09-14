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
* Description:  A class for showing copy contacts results
*
*/



// INCLUDE FILES
#include "TPbk2CopyContactsResults.h"

#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <pbk2commands.rsg>

namespace{

const TInt KGranularity( 2 );

    }

const TInt KZero = 0;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TPbk2CopyContactsResults::TPbk2CopyContactsResults
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPbk2CopyContactsResults::TPbk2CopyContactsResults()
:   iCopied( KZero ),
    iNotCopied( KZero ),
    iName( KNullDesC ),
    iOneContactCopiedResId( KErrNotFound ),
    iOneContactNotCopiedResId( KErrNotFound ),
    iMultipleContactsCopiedResId( KErrNotFound )
    {
    }
    
// -----------------------------------------------------------------------------
// TPbk2CopyContactsResults::TPbk2CopyContactsResults
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPbk2CopyContactsResults::TPbk2CopyContactsResults
    ( TInt aCopied, TInt aTotal )
:   iCopied( aCopied ),
    iNotCopied( aTotal - aCopied ),
    iName( KNullDesC ),
    iOneContactCopiedResId( KErrNotFound ),
    iOneContactNotCopiedResId( KErrNotFound ),
    iMultipleContactsCopiedResId( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// TPbk2CopyContactsResults::TPbk2CopyContactsResults
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C TPbk2CopyContactsResults::TPbk2CopyContactsResults
    ( TInt aCopied, const TDesC& aName )
:   iCopied( aCopied ),
    iNotCopied( KZero ),
    iName( aName ),
    iOneContactCopiedResId( KErrNotFound ),
    iOneContactNotCopiedResId( KErrNotFound ),
    iMultipleContactsCopiedResId( KErrNotFound )
    {
    }
    
// -----------------------------------------------------------------------------
// TPbk2CopyContactsResults::ShowNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void TPbk2CopyContactsResults::ShowNoteL()
    {
    HBufC* prompt = NULL;
    
	if ( iName.Length() == 0 )
	    {
		if ( iNotCopied == 0 )
			{
			if ( iMultipleContactsCopiedResId == KErrNotFound )
			    {
			    // Use default text resource
			    prompt = StringLoader::LoadLC
			        ( R_QTN_PBCOP_NOTE_N_ENTRY_COPY_PB, iCopied );
			    }
			else
			    {
			    prompt = StringLoader::LoadLC
			        ( iMultipleContactsCopiedResId, iCopied );
			    }			    
			}
		else // (iNotCopied > 0)
			{
			if ( iCopied == 1 && iNotCopied == 1 )
				{
				prompt = 
				    StringLoader::LoadLC( R_QTN_PBCOP_INFO_ONE_OK_ONE_NOT );
				}
			else if ( iCopied == 1 && iNotCopied > 1 )
				{
				prompt = StringLoader::LoadLC(
				    R_QTN_PBCOP_INFO_ONE_OK_N_NOT, iNotCopied);
				}
			else if ( ( iCopied == 0 || iCopied > 1 ) && iNotCopied == 1 )
				{
				prompt = 
				    StringLoader::LoadLC
				        ( R_QTN_PBCOP_INFO_N_OK_ONE_NOT, iCopied );
				}
			else if ((iCopied == 0 || iCopied > 1) && 
			    (iNotCopied == 0 || iNotCopied > 1))
				{
				CArrayFixFlat<TInt>* array = 
				    new( ELeave ) CArrayFixFlat<TInt>( KGranularity );
				CleanupStack::PushL( array );
				array->AppendL( iCopied );
				array->AppendL( iNotCopied );
				prompt = 
				    StringLoader::LoadL( R_QTN_PBCOP_INFO_N_OK_N_NOT, *array );
				CleanupStack::PopAndDestroy( array );
				CleanupStack::PushL( prompt );
				}
			}
		}
	else // Only one contact was marked to be copied
		{
		if ( iCopied > 0 )
			{
			if ( iOneContactCopiedResId == KErrNotFound )
			    {
			    // Use default text resource
			    prompt = StringLoader::LoadLC
			        ( R_QTN_PBCOP_NOTE_CONTACT_COPIED_PB2, iName );    
			    }
			else
			    {
			    prompt = StringLoader::LoadLC( iOneContactCopiedResId, iName );    
			    }
			}
		else
			{
			if ( iOneContactNotCopiedResId == KErrNotFound )
			    {
			    // Use default text resource
			    prompt = StringLoader::LoadLC
			        ( R_QTN_PBCOP_NOTE_ENTRY_NOT_COPIED, iName );
			    }
			else
			    {
			    prompt = StringLoader::LoadLC( iOneContactNotCopiedResId, iName );
			    }
			}
		}
	
	if ( prompt )
		{
		CAknConfirmationNote* dlg = new( ELeave ) CAknConfirmationNote( EFalse );
		dlg->SetTimeout( CAknNoteDialog::ELongTimeout );
		dlg->ExecuteLD( *prompt );
		CleanupStack::PopAndDestroy(prompt );
		}
	}

// -----------------------------------------------------------------------------
// TPbk2CopyContactsResults::ShowErrorNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void TPbk2CopyContactsResults::ShowErrorNoteL()
    {
    HBufC* prompt = StringLoader::LoadLC( R_QTN_PBCOP_NOTE_COPY_NOT_WORK );
    CAknErrorNote* note = new( ELeave ) CAknErrorNote( ETrue );
    note->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }

EXPORT_C void 
TPbk2CopyContactsResults::SetOneContactCopiedTextRes( TInt aResId )
    {
    iOneContactCopiedResId = aResId;
    }

EXPORT_C void 
TPbk2CopyContactsResults::SetOneContactNotCopiedTextRes( TInt aResId )
    {
    iOneContactNotCopiedResId = aResId;
    }
    
EXPORT_C void 
TPbk2CopyContactsResults::SetMultipleContactsCopiedTextRes( TInt aResId )
    {
    iMultipleContactsCopiedResId = aResId;
    }
    
//  End of File  
