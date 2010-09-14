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
* Description:  Phonebook 2 contact field property selector
*              : for new contact creation.
*
*/

#include "CPbk2AssignCreateNewProperty.h"

// Phonebook 2
#include "TPbk2AssignNoteService.h"
#include <pbk2uicontrols.rsg>
#include <pbk2serverapp.rsg>
#include <MPbk2FieldProperty.h>
#include <TPbk2AddItemWrapper.h>

// Virtual Phonebook
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactManager.h>

// System includes
#include <StringLoader.h>
#include <AknQueryDialog.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Checks is the given contact store full.
 *
 * @param aTargetStore      Store to inspect.
 * @return  ETrue if store is full.
 */
TBool StoreFullL( const MVPbkContactStore& aTargetStore )
    {
    TBool ret( EFalse );

    const MVPbkContactStoreInfo& storeInfo = aTargetStore.StoreInfo();
    if ( storeInfo.MaxNumberOfContactsL() != KVPbkStoreInfoUnlimitedNumber &&
         storeInfo.MaxNumberOfContactsL() <= storeInfo.NumberOfContactsL() )
        {
        ret = ETrue;
        }

    return ret;
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2AssignCreateNewProperty::CPbk2AssignCreateNewProperty
// --------------------------------------------------------------------------
//
CPbk2AssignCreateNewProperty::CPbk2AssignCreateNewProperty
        ( HBufC8& aSelector, TInt aResourceId,
          MVPbkContactStore*& aContactStore,
          const CPbk2StorePropertyArray& aStoreProperties,
          CVPbkContactManager& aContactManager ) :
            CPbk2SelectFieldPropertyBase( aSelector, aResourceId ),
            iContactStore( aContactStore ),
            iStoreProperties( aStoreProperties ),
            iContactManager ( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignCreateNewProperty::~CPbk2AssignCreateNewProperty
// --------------------------------------------------------------------------
//
CPbk2AssignCreateNewProperty::~CPbk2AssignCreateNewProperty()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignCreateNewProperty::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignCreateNewProperty* CPbk2AssignCreateNewProperty::NewL
        ( HBufC8& aSelector, MVPbkContactStore*& aContactStore,
          const CPbk2StorePropertyArray& aStoreProperties,
          CVPbkContactManager& aContactManager )
    {
    CPbk2AssignCreateNewProperty* self =
        new ( ELeave ) CPbk2AssignCreateNewProperty
            ( aSelector, R_QTN_PHOB_QTL_ENTRY_CREATE, aContactStore,
              aStoreProperties, aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignCreateNewProperty::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AssignCreateNewProperty::ConstructL()
    {
    BaseConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2AssignCreateNewProperty::PrepareL
// --------------------------------------------------------------------------
//
void CPbk2AssignCreateNewProperty::PrepareL()
    {
    TPbk2AssignNoteService noteService;
    TVPbkContactStoreUriPtr storeUri =
        iContactStore->StoreProperties().Uri();

    if ( StoreFullL( *iContactStore ) )
        {
        noteService.ShowStoreFullNoteL( *iContactStore, iStoreProperties );

        if ( noteService.ShowCreateNewToPhoneQueryL() )
            {
            // Safe to assume that phone memory store never gets full
            storeUri.Set( VPbkContactStoreUris::DefaultCntDbUri() );
            CreateWrappersL( storeUri );
            // Change target store
            iContactStore =
                iContactManager.ContactStoresL().Find( storeUri );
            }
        }
    else
        {
        CreateWrappersL( storeUri );

        if ( iWrappers.Count() <= 0 )
            {
            TInt compare = storeUri.Compare
                ( VPbkContactStoreUris::DefaultCntDbUri(),
                  TVPbkContactStoreUriPtr::EContactStoreUriAllComponents );

            // Show create new entry to phone memory query
            if ( ( compare  != 0 ) &&
                 noteService.ShowCreateNewToPhoneQueryL() )
                {
                storeUri.Set( VPbkContactStoreUris::DefaultCntDbUri() );
                CreateWrappersL( storeUri  );
                // Change target store
                iContactStore =
                    iContactManager.ContactStoresL().Find( storeUri );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignCreateNewProperty::ExecuteL
// --------------------------------------------------------------------------
//
TInt CPbk2AssignCreateNewProperty::ExecuteL()
    {
    TInt result = ShowSelectFieldQueryL();

    if ( result > KErrNotFound &&
         iWrappers.Count() > 0 &&
         iWrappers.Count() > result )
        {
        iSelectedFieldType = &iWrappers[result].PropertyAt( 0 ).FieldType();
        // ShowSelectFieldQuery returns the index of field, anyhow
        // this method should return an error code.
        result = KErrNone;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AssignCreateNewProperty::SelectedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2AssignCreateNewProperty::SelectedFieldIndex() const
    {
    return KErrNotSupported;
    }

// End of File
