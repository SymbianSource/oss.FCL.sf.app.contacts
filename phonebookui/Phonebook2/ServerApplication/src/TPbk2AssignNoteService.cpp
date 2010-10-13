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
* Description:  Phonebook 2 assign note service.
*
*/


#include "TPbk2AssignNoteService.h"

// Phonebook 2
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <Pbk2ServerApp.rsg>
#include <Pbk2UIControls.rsg>
#include <Pbk2CommonUi.rsg>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <aknnotewrappers.h>
#include <StringLoader.h>


// --------------------------------------------------------------------------
// TPbk2AssignNoteService::TPbk2AssignNoteService
// --------------------------------------------------------------------------
//
TPbk2AssignNoteService::TPbk2AssignNoteService()
    {
    }

// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowInformationNoteL
// --------------------------------------------------------------------------
//
void TPbk2AssignNoteService::ShowInformationNoteL( TInt aResourceId ) const
    {
    HBufC* text = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowInformationNoteL
// --------------------------------------------------------------------------
//
void TPbk2AssignNoteService::ShowInformationNoteL
        ( TInt aResourceId, TVPbkContactStoreUriPtr aStoreUri,
          const CPbk2StorePropertyArray& aStoreProperties ) const
    {
    const CPbk2StoreProperty* storeProperty =
        aStoreProperties.FindProperty( aStoreUri );

    TDesC* storeName = NULL;

    if ( storeProperty )
        {
        storeName = const_cast<TDesC*>( &storeProperty->StoreName() );
        }
    else
        {
        storeName = const_cast<TDesC*>( &aStoreUri.UriDes() );
        }

    HBufC* text = StringLoader::LoadLC( aResourceId, *storeName );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text ); //text
    }

// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowConfirmationNoteL
// --------------------------------------------------------------------------
//
TInt TPbk2AssignNoteService::ShowConfirmationNoteL
        ( TInt aResourceId, const TDesC& aText ) const
    {
    HBufC* text = StringLoader::LoadLC( aResourceId, aText );
    CAknConfirmationNote* query = new ( ELeave ) CAknConfirmationNote;
    TInt result = query->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    return result;
    }

// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowReplaceDetailQueryL
// --------------------------------------------------------------------------
//
TInt TPbk2AssignNoteService::ShowReplaceDetailQueryL
        ( MPbk2ContactNameFormatter& aContactNameFormatter,
          MVPbkStoreContact& aStoreContact ) const
    {
    HBufC* contactName = aContactNameFormatter.GetContactTitleL
        ( aStoreContact.Fields(),
          MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
    CleanupStack::PushL( contactName );

    HBufC* prompt = StringLoader::LoadLC
        ( R_QTN_PHOB_NOTE_REPLACE_DETAIL, *contactName );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt queryResult = dlg->ExecuteLD
        ( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt );

    CleanupStack::PopAndDestroy( 2 ); //prompt, contactName

    return queryResult;
    }

// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowDetailAddedNoteL
// --------------------------------------------------------------------------
//
void TPbk2AssignNoteService::ShowDetailAddedNoteL( TInt aCount ) const
    {
    HBufC* text = NULL;
    if ( aCount <= 0 )
        {
        text = StringLoader::LoadLC( R_QTN_PHOB_NOTE_DETAIL_NOT_ADDED );
        }
    else if ( aCount == 1 )
        {
        text = StringLoader::LoadLC( R_QTN_PHOB_NOTE_DETAIL_ADDED );
        }
    else
        {
        text = StringLoader::LoadLC
            ( R_QTN_PHOB_NOTE_DETAIL_ADDED_TO_N, aCount );
        }

    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );
    CleanupStack::PopAndDestroy( text );
    }

// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowCanNotAddDetailNoteL
// --------------------------------------------------------------------------
//
void TPbk2AssignNoteService::ShowCanNotAddDetailNoteL
        ( MPbk2ContactNameFormatter& aContactNameFormatter,
          MVPbkStoreContact& aStoreContact ) const
    {
    HBufC* contactName = aContactNameFormatter.GetContactTitleL
        ( aStoreContact.Fields(),
          MPbk2ContactNameFormatter::EPreserveLeadingSpaces );
    CleanupStack::PushL( contactName );

    HBufC* text = StringLoader::LoadLC
        ( R_QTN_PHOB_NOTE_CANNOT_ADD_DETAIL, *contactName );
    CAknInformationNote* note = new ( ELeave ) CAknInformationNote( ETrue );
    note->ExecuteLD( *text );

    CleanupStack::PopAndDestroy( 2 ); // text, contactName
    }


// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowCreateNewToPhoneQueryL
// --------------------------------------------------------------------------
//
TInt TPbk2AssignNoteService::ShowCreateNewToPhoneQueryL() const
    {
    HBufC* prompt = StringLoader::LoadLC
        ( R_QTN_PHOB_QUERY_CREATE_NEW_TO_STORE );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt queryResult = dlg->ExecuteLD
        ( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt );
    CleanupStack::PopAndDestroy( prompt );

    return queryResult;
    }

// --------------------------------------------------------------------------
// TPbk2AssignNoteService::ShowStoreFullNoteL
// --------------------------------------------------------------------------
//
void TPbk2AssignNoteService::ShowStoreFullNoteL
        ( const MVPbkContactStore& aTargetStore,
          const CPbk2StorePropertyArray& aStoreProperties ) const
    {
    // Fetch store name
    TVPbkContactStoreUriPtr uri = aTargetStore.StoreProperties().Uri();

    const CPbk2StoreProperty* storeProperty =
        aStoreProperties.FindProperty( uri );

    const TDesC* storeName = NULL;
    if ( storeProperty )
        {
        storeName = &storeProperty->StoreName();
        }
    else
        {
        storeName = &uri.UriDes();
        }

    HBufC* prompt = StringLoader::LoadLC
        ( R_QTN_PHOB_NOTE_STORE_FULL, *storeName );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy(); // prompt
    }

// End of File
