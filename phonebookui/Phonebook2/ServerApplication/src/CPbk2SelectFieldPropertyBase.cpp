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
* Description:  Phonebook 2 contact field property selector base.
*
*/


#include "CPbk2SelectFieldPropertyBase.h"

// Phonebook 2
#include "CPbk2ServerAppAppUi.h"
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <MPbk2FieldProperty2.h>
#include <CPbk2AddItemToContactDlg.h>
#include <TPbk2AddItemWrapper.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <barsread.h>
#include <aknnotewrappers.h>


// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::CPbk2SelectFieldPropertyBase
// --------------------------------------------------------------------------
//
CPbk2SelectFieldPropertyBase::CPbk2SelectFieldPropertyBase
        ( HBufC8& aSelector, TInt aResourceId ) :
            iSelector( aSelector ),
            iDialogTitleResourceId( aResourceId )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::~CPbk2SelectFieldPropertyBase
// --------------------------------------------------------------------------
//
CPbk2SelectFieldPropertyBase::~CPbk2SelectFieldPropertyBase()
    {
    iWrappers.Close();

    if ( iAddItemDlgEliminator )
        {
        iAddItemDlgEliminator->ForceExit();
        }

    delete iFieldProperties;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::BaseConstructL
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldPropertyBase::BaseConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::SelectedFieldType
// --------------------------------------------------------------------------
//
const MVPbkFieldType*
        CPbk2SelectFieldPropertyBase::SelectedFieldType() const
    {
    return iSelectedFieldType;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::Cancel
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldPropertyBase::Cancel( TInt aCommandId )
    {
    if ( iAddItemDlgEliminator )
        {
        TRAPD( err, iAddItemDlgEliminator->RequestExitL( aCommandId ) );
        if ( err != KErrNone && iAddItemDlgEliminator )
            {
            // Use force if necessary
            iAddItemDlgEliminator->ForceExit();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::CreateFieldPropertiesArrayL
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldPropertyBase::CreateFieldPropertiesArrayL
        ( MVPbkContactStore& aStore )
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    CPbk2FieldPropertyArray* temp =  CPbk2FieldPropertyArray::NewL
        ( aStore.StoreProperties().SupportedFields(),
          appUi.ApplicationServices().ContactManager().FsSession() );

    delete iFieldProperties;
    iFieldProperties = temp;
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::CreateWrappersL
// --------------------------------------------------------------------------
//
void CPbk2SelectFieldPropertyBase::CreateWrappersL
        ( TVPbkContactStoreUriPtr aStoreUri )
    {
    TResourceReader reader;
    reader.SetBuffer( &iSelector );

    // Default saving store
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );
    MVPbkContactStore* store =
        appUi.ApplicationServices().ContactManager().ContactStoresL().
            Find( aStoreUri );

    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL( reader,
            store->StoreProperties().SupportedFields() );
    CleanupStack::PushL( selector );

    CreateFieldPropertiesArrayL( *store );

    const TInt count( iFieldProperties->Count() );
    for ( TInt i = 0; i < count; ++i )
        {
        const MPbk2FieldProperty& property =
            iFieldProperties->At( i );
        
        MPbk2FieldProperty2* fieldPropertyExtension =
            reinterpret_cast<MPbk2FieldProperty2*>(
                const_cast<MPbk2FieldProperty&>( property ).
                    FieldPropertyExtension(
                        KMPbk2FieldPropertyExtension2Uid ) );
        
        if ( fieldPropertyExtension != NULL )
            {
            if ( !fieldPropertyExtension->XSpName().Length() &&
                    selector->IsFieldTypeIncluded( property.FieldType() ) )
                {
                TPbk2AddItemWrapper wrapper( property );
                iWrappers.Append( wrapper );
                }
            }
        }
    CleanupStack::PopAndDestroy( selector );
    }

// --------------------------------------------------------------------------
// CPbk2SelectFieldPropertyBase::ShowSelectFieldQueryL
// --------------------------------------------------------------------------
//
TInt CPbk2SelectFieldPropertyBase::ShowSelectFieldQueryL()
    {
    TInt result( KErrNotSupported );

    // Show select field query
    if ( iWrappers.Count() > 1 )
        {
        HBufC* titleBuf =
            CCoeEnv::Static()->AllocReadResourceLC( iDialogTitleResourceId );

        TPbk2AddItemDialogParams params
            ( titleBuf, R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
              KErrNotFound );

        CPbk2AddItemToContactDlg* addItemDlg =
            CPbk2AddItemToContactDlg::NewL();
        iAddItemDlgEliminator = addItemDlg;
        iAddItemDlgEliminator->ResetWhenDestroyed
            ( &iAddItemDlgEliminator );
        result = addItemDlg->ExecuteLD( iWrappers, &params );
        CleanupStack::PopAndDestroy( titleBuf );
        }
    else if ( iWrappers.Count() == 1 )
        {
        result = KErrNone;
        }
    return result;
    }

// End of File
