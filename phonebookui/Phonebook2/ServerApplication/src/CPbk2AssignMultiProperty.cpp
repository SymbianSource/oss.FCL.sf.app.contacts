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
*              : for multi contact assign.
*
*/


#include "CPbk2AssignMultiProperty.h"

// Phonebook 2
#include "CPbk2ServerAppAppUi.h"
#include <Pbk2ServerApp.rsg>
#include <MPbk2FieldProperty.h>
#include <TPbk2AddItemWrapper.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <barsread.h>

/// Unnamed namespace for local definitions
namespace {

enum TPanicCode
    {
    ENullPointer,
    EInvalidParameter
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2AssignMultiProperty");
    User::Panic(KPanicText,aReason);
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::CPbk2AssignMultiProperty
// --------------------------------------------------------------------------
//
CPbk2AssignMultiProperty::CPbk2AssignMultiProperty
        ( HBufC8& aSelector, TInt aResourceId,
          MVPbkContactLinkArray& aMarkedEntries ) :
            CPbk2SelectFieldPropertyBase( aSelector,  aResourceId ),
            iMarkedEntries( aMarkedEntries )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::~CPbk2AssignMultiProperty
// --------------------------------------------------------------------------
//
CPbk2AssignMultiProperty::~CPbk2AssignMultiProperty()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignMultiProperty* CPbk2AssignMultiProperty::NewL
        ( HBufC8& aSelector, MVPbkContactLinkArray& aMarkedEntries )
    {
    CPbk2AssignMultiProperty* self = new ( ELeave ) CPbk2AssignMultiProperty
        ( aSelector, R_QTN_PHOB_QTL_ENTRY_ADD_TO, aMarkedEntries );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AssignMultiProperty::ConstructL()
    {
    BaseConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::PrepareL
// --------------------------------------------------------------------------
//
void CPbk2AssignMultiProperty::PrepareL()
    {
    __ASSERT_ALWAYS( iMarkedEntries.Count() > 0,
        Panic( EInvalidParameter ) );

    if ( AreAllContactsFromSameStore( iMarkedEntries ) )
        {
        TVPbkContactStoreUriPtr uriPtr =
            iMarkedEntries.At( 0 ).ContactStore().StoreProperties().Uri();
        CPbk2SelectFieldPropertyBase::CreateWrappersL( uriPtr );
        if ( iWrappers.Count() <= 0 )
            {
            CreateWrappersFromCntDbL();
            }
        }
    else
        {
        CreateWrappersFromCntDbL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::ExecuteL
// --------------------------------------------------------------------------
//
TInt CPbk2AssignMultiProperty::ExecuteL()
    {
    __ASSERT_ALWAYS( iFieldProperties, Panic( ENullPointer ) );

    TInt result = ShowSelectFieldQueryL();

    if ( result > KErrNotFound &&
         iWrappers.Count() > 0 &&
         iWrappers.Count() > result )
        {
        iSelectedFieldType = &iWrappers[result].PropertyAt( 0 ).FieldType();
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::SelectedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2AssignMultiProperty::SelectedFieldIndex() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::AreAllContactsFromSameStore
// --------------------------------------------------------------------------
//
TBool CPbk2AssignMultiProperty::AreAllContactsFromSameStore
        ( MVPbkContactLinkArray& aMarkedEntries )
    {
    TBool ret = ETrue;

    const TInt count( aMarkedEntries.Count() );
    if ( count > 1 )
        {
        // Take the first URI from array
        TVPbkContactStoreUriPtr uriPtr =
            aMarkedEntries.At( 0 ).ContactStore().StoreProperties().Uri();
        for ( TInt i(1); i < count; ++i )
            {
            TVPbkContactStoreUriPtr nextUriPtr =
                aMarkedEntries.At( i ).ContactStore().
                    StoreProperties().Uri();
            if ( uriPtr.Compare( nextUriPtr,
                   TVPbkContactStoreUriPtr::EContactStoreUriAllComponents )
                    != 0 )
                {
                ret = EFalse;
                break;
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2AssignMultiProperty::CreateWrappersFromCntDbL
// --------------------------------------------------------------------------
//
void CPbk2AssignMultiProperty::CreateWrappersFromCntDbL()
    {
    TResourceReader reader;
    reader.SetBuffer( &iSelector );

    // Default saving store
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );
    MVPbkContactStore* store =
        appUi.ApplicationServices().ContactManager().ContactStoresL().Find
            ( VPbkContactStoreUris::DefaultCntDbUri() );

    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL( reader,
            appUi.ApplicationServices().ContactManager().FieldTypes() );

    CleanupStack::PushL( selector );

    CreateFieldPropertiesArrayL( *store );

    const TInt count( iFieldProperties->Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        const MPbk2FieldProperty& property =
            iFieldProperties->At( i );
        if ( selector->IsFieldTypeIncluded( property.FieldType() ) )
            {
            TPbk2AddItemWrapper wrapper( property );
            iWrappers.Append( wrapper );
            }
        }
    CleanupStack::PopAndDestroy( selector );
    }

// End of File

