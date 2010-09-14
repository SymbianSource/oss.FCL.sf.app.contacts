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
* Description:  Phonebook 2 contact field type selector for
*              : single contact assign.
*
*/


#include "CPbk2AssignSingleProperty.h"

// Phonebook 2
#include "Pbk2AssignValidateField.h"
#include "CPbk2ServerAppAppUi.h"
#include <pbk2serverapp.rsg>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <TPbk2AddItemWrapper.h>
#include <CPbk2PresentationContact.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactManager.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KFirstElement = 0;

enum TPanicCode
    {
    ENullPointer,
    EInvalidParameter
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2AssignSingleProperty");
    User::Panic(KPanicText,aReason);
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2AssignSingleProperty::CPbk2AssignSingleProperty
// --------------------------------------------------------------------------
//
CPbk2AssignSingleProperty::CPbk2AssignSingleProperty
        ( HBufC8& aSelector, TInt aResourceId,
          MVPbkStoreContact& aStoreContact,
          CVPbkContactManager& aContactManager ) :
            CPbk2SelectFieldPropertyBase( aSelector, aResourceId ),
            iFieldIndex( KErrNotSupported ),
            iStoreContact( aStoreContact ),
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignSingleProperty::~CPbk2AssignSingleProperty
// --------------------------------------------------------------------------
//
CPbk2AssignSingleProperty::~CPbk2AssignSingleProperty()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignSingleProperty::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignSingleProperty* CPbk2AssignSingleProperty::NewL
        ( HBufC8& aSelector, MVPbkStoreContact& aStoreContact,
          CVPbkContactManager& aContactManager )
    {
    CPbk2AssignSingleProperty* self =
        new ( ELeave ) CPbk2AssignSingleProperty
            ( aSelector, R_QTN_PHOB_QTL_ENTRY_ADD_TO, aStoreContact,
              aContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSingleProperty::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AssignSingleProperty::ConstructL()
    {
    BaseConstructL();
    }

// --------------------------------------------------------------------------
// CPbk2AssignSingleProperty::PrepareL
// --------------------------------------------------------------------------
//
void CPbk2AssignSingleProperty::PrepareL()
    {
    // Create wrappers
    CreateWrappersL( iStoreContact.ContactStore().StoreProperties().Uri() );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    CPbk2PresentationContact* contact = CPbk2PresentationContact::NewL
        ( iStoreContact, appUi.ApplicationServices().FieldProperties() );
    CleanupStack::PushL( contact );

    // Go through all the wrappers and validate them
    const TInt count( iWrappers.Count() );
    // Loop backwards
    for ( TInt i( count - 1 ); i >= 0; --i )
        {
        TPbk2AddItemWrapper wrapper = iWrappers[i];

        // Check is the field type used in contact
        iResult = Pbk2AssignValidateField::ValidateFieldTypeUsageInContactL
            ( *contact, wrapper.PropertyAt( KFirstElement ).FieldType(),
              iContactManager.FsSession(), iFieldIndex );

        if ( iResult == KErrAlreadyExists && count > 1)
            {
            // Field type is already fully used so remove
            // the corresponding wrapper. Do not do this if there
            // is only one wrapper left. In that case the user is
            // asked for replace confirmation.
            iWrappers.Remove( i );
            }
        else if ( iResult == KErrNotSupported && count > 1 )
            {
            // Field type not supported, remove corresponding wrapper
            iWrappers.Remove( i );
            }
        }

    // If there is just one wrapper, the result is already correct,
    // but in other cases update result according to wrapper count
    if ( iWrappers.Count() == 0 )
        {
        // No suitable wrappers -> field type is not
        // supported by this contact
        iResult = KErrNotSupported;
        iFieldIndex = KErrNotSupported;
        }
    else if ( iWrappers.Count() > 1 )
        {
        // Several wrappers exist
        iResult = KErrNone;
        }

    CleanupStack::PopAndDestroy( contact );
    }

// --------------------------------------------------------------------------
// CPbk2AssignSingleProperty::ExecuteL
// --------------------------------------------------------------------------
//
TInt CPbk2AssignSingleProperty::ExecuteL()
    {
    __ASSERT_ALWAYS( iFieldProperties, Panic( ENullPointer ) );

    // Display select field query
    TInt index = ShowSelectFieldQueryL();

    if ( index > KErrNotFound && iWrappers.Count() > 0 &&
         iWrappers.Count() > index )
        {
        // Get selected field type
        iSelectedFieldType =
            &iWrappers[index].PropertyAt( KFirstElement ).FieldType();
        }
    else if ( index == KErrCancel )
        {
        // Cancel was pressed
        iResult = KErrCancel;
        }

    return iResult;
    }

// --------------------------------------------------------------------------
// CPbk2AssignSingleProperty::SelectedFieldIndex
// --------------------------------------------------------------------------
//
TInt CPbk2AssignSingleProperty::SelectedFieldIndex() const
    {
    return iFieldIndex;
    }

// End of File
