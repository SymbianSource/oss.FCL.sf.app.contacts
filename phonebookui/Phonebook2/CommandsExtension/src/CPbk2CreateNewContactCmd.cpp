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
* Description:  Phonebook 2 create new contact command object.
*
*/


// INCLUDE FILES
#include "CPbk2CreateNewContactCmd.h"

// Phonebook 2
#include <TPbk2ContactEditorParams.h>
#include <CPbk2ContactEditorDlg.h>
#include <MPbk2CommandObserver.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <pbk2uicontrols.rsg>
#include <pbk2commonui.rsg>
#include <pbk2cmdextres.rsg>
#include <MPbk2ContactUiControl.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2DriveSpaceCheck.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2StoreValidityInformer.h>
#include <CPbk2ApplicationServices.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkUtils.h>
#include <CVPbkContactStoreUriArray.h>
#include <VPbkContactStoreUris.h>

// System includes
#include <barsread.h>
#include <coemain.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EExecuteLD_PreCond = 1,
    EAddFindTextL_Logic,
    EPanicPreCond_Null_Pointer,
    EPanicIncorrectState,
    EPanicPreCond_OpenContactStoreL,
    EPanicPreCond_ValidateStoreL,
    EPanicPreCond_CreateNewContactL
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2CreateNewContactCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

/**
 * Checks is the given field type included in
 * the given selection.
 *
 * @param aFieldType    The field type to check.
 * @param aManager      Virtual Phonebook contact manager.
 * @param aResourceId   Selector's resource id.
 * @return  ETrue if field type is included.
 */
TBool IsFieldTypeIncludedL( const MVPbkFieldType& aFieldType,
        const CVPbkContactManager& aManager, const TInt aResourceId )
    {
    // Get the field type
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, aResourceId);

    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL(resReader, aManager.FieldTypes());
    CleanupStack::PopAndDestroy(); // resReader

    TBool ret = selector->IsFieldTypeIncluded(aFieldType);
    delete selector;
    return ret;
    }

/**
 * Displays a note.
 *
 * @param aResourceId   Note resource id.
 * @param aString       Note parameter.
 */
void ShowNoteL( TInt aResourceId, const TDesC& aString )
    {
    HBufC* prompt = NULL;

    if( aString.Length() )
        {
        prompt = StringLoader::LoadLC( aResourceId, aString );
        }
    else
        {
        prompt = StringLoader::LoadLC( aResourceId );
        }
    CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD(*prompt);
    CleanupStack::PopAndDestroy(); // prompt
    }

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

/**
 * Displays store not available note.
 *
 * @param aStoreProperties      Phonebook store properties.
 */
void ShowStoreNotAvailableNoteL( CPbk2StorePropertyArray& aStoreProperties )
    {
    CPbk2StoreConfiguration* storeConfig = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfig );
    const TDesC& uri = storeConfig->DefaultSavingStoreL().UriDes();

    // Get the property of the default saving store
    const CPbk2StoreProperty* prop =
        aStoreProperties.FindProperty( uri );
    if ( prop && prop->StoreName().Length() > 0 )
        {
        HBufC* text = StringLoader::LoadLC( R_QTN_PHOB_STORE_NOT_AVAILABLE,
            prop->StoreName() );
        CAknInformationNote* note = new(ELeave) CAknInformationNote( ETrue );
        // Show "not available" note
        note->ExecuteLD( *text );
        CleanupStack::PopAndDestroy( text );
        }

    CleanupStack::PopAndDestroy( storeConfig );
    }

/**
 * Checks is the store valid.
 *
 * @param aInformer         Store validity informer.
 * @param aStore            Store to inspect.
 * @return  ETrue if store is valid.
 */
TBool IsValidStoreL
        ( MPbk2StoreValidityInformer& aInformer, MVPbkContactStore* aStore )
    {
    TBool isValid ( EFalse );
    if ( aStore )
        {
        CVPbkContactStoreUriArray* currentlyValidStores =
            aInformer.CurrentlyValidStoresL();
        TVPbkContactStoreUriPtr uri =
            aStore->StoreProperties().Uri();
        isValid = currentlyValidStores->IsIncluded( uri );
        delete currentlyValidStores;
        }

    return isValid;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::CPbk2CreateNewContactCmd
// --------------------------------------------------------------------------
//
CPbk2CreateNewContactCmd::CPbk2CreateNewContactCmd(
        MPbk2ContactUiControl& aUiControl ) : 
    CActive( EPriorityIdle ),
    iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::~CPbk2CreateNewContactCmd
// --------------------------------------------------------------------------
//
CPbk2CreateNewContactCmd::~CPbk2CreateNewContactCmd()
    {
    Cancel();
    if( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }
    if ( iTargetStore )
        {
        iTargetStore->Close( *this );
        }
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CreateNewContactCmd* CPbk2CreateNewContactCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPbk2CreateNewContactCmd* self =
        new(ELeave) CPbk2CreateNewContactCmd(aUiControl);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CreateNewContactCmd::ConstructL()
    {
    // check drive space
    CPbk2DriveSpaceCheck* driveSpaceCheck = CPbk2DriveSpaceCheck::NewL
        ( CCoeEnv::Static()->FsSession() );
    CleanupStack::PushL( driveSpaceCheck );
    // check FFS situation
    driveSpaceCheck->DriveSpaceCheckL();
    CleanupStack::PopAndDestroy( driveSpaceCheck );

    iAppServices = CPbk2ApplicationServices::InstanceL();

    CPbk2StoreConfiguration* storeConfig = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL(storeConfig);
    iTargetStore = iAppServices->ContactManager().ContactStoresL().Find
        ( storeConfig->DefaultSavingStoreL() );
    CleanupStack::PopAndDestroy(storeConfig);
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::RunL
// --------------------------------------------------------------------------
//    
void CPbk2CreateNewContactCmd::RunL()
    {
    switch( iState )
        {        
        case ELoadContactStore:
            {
            LoadContactStoreL();
            break;
            }
        case EOpenContactStore:
            {
            OpenContactStoreL();
            break;
            }        
        case EValidateStore:
            {
            ValidateStoreL();
            break;
            }                  
        case ECreateNewContact:
            {
            CreateNewContactL();
            break;
            }
        case EFinishCommand:
            {
            iCommandObserver->CommandFinished(*this);
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EPanicIncorrectState ) );
            }
        };
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2CreateNewContactCmd::RunError( TInt aError )
    {	
    HandleError( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::HandleError
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::HandleError( TInt aError )
    {	
    CCoeEnv::Static()->HandleError( aError );
    iState = EFinishCommand;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::ExecuteLD()
    {
    __ASSERT_DEBUG( iCommandObserver, Panic( EExecuteLD_PreCond ) );

    CleanupStack::PushL(this);

    if ( !iTargetStore || !IsValidStoreL
            ( iAppServices->StoreValidityInformer(), iTargetStore ) )
        {
        // if target store not available finish command
        ShowStoreNotAvailableNoteL( iAppServices->StoreProperties() );
        
        iTargetStore = NULL;
        if ( ShowCreateNewToPhoneQueryL() )
            {
            iTargetStore = iAppServices->ContactManager().ContactStoresL().
                    Find( VPbkContactStoreUris::DefaultCntDbUri() );
                    
            if ( iTargetStore )
                {
                if ( !IsValidStoreL( 
                     iAppServices->StoreValidityInformer(), iTargetStore ) )
                    {
                    // Store is loaded, but not opened                   
                    iState = EOpenContactStore;
                    }
                else
                    {
                    // Store is loaded and opened                    
                    iState = EValidateStore;
                    }
                }
            else
                {
                // Store is neither loaded nor opened
                iState = ELoadContactStore;
                }                    
            }
        else
            {
            // Canceled
            iState = EFinishCommand;
            }
        }
    else
        {
        iState = EValidateStore;
        }

    IssueRequest();
    CleanupStack::Pop(this);
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ContactEditingComplete
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ContactEditingComplete
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::ContactEditingComplete
        ( MVPbkStoreContact* aEditedContact )
    {
    TRAPD( error, HandleContactEditingCompleteL( aEditedContact ) );
    if ( error != KErrNone )
        {
        CCoeEnv::Static()->HandleError( error );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ContactEditingDeletedContact
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::ContactEditingDeletedContact
        ( MVPbkStoreContact* aEditedContact )
    {
    delete aEditedContact; // not needed anymore
    if (iUiControl)
        {
        // reset find if control exists
        TRAP_IGNORE( iUiControl->ResetFindL() );
        }
    iState = EFinishCommand;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ContactEditingAborted
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::ContactEditingAborted()
    {
    if (iUiControl)
        {
        // reset find if control exists
        TRAP_IGNORE( iUiControl->ResetFindL() );
        }
    iState = EFinishCommand;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::StoreReady
// --------------------------------------------------------------------------
//    
void CPbk2CreateNewContactCmd::StoreReady( 
        MVPbkContactStore& aContactStore )
    {
    iTargetStore = &aContactStore;
    iState = EValidateStore;
    IssueRequest();
    }
// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::StoreUnavailable( 
        MVPbkContactStore& /*aContactStore*/, 
        TInt /*aReason*/ )
    {
    iState = EFinishCommand;
    IssueRequest();
    }
        
// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::HandleStoreEventL
// --------------------------------------------------------------------------
//        
void CPbk2CreateNewContactCmd::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::AddFindTextL
// --------------------------------------------------------------------------
//
TInt CPbk2CreateNewContactCmd::AddFindTextL( MVPbkStoreContact& aContact )
    {
    TInt index = KErrNotFound;
    if (iUiControl)
        {
        const TDesC& findText = iUiControl->FindTextL();
        if (findText != KNullDesC)
            {
            CPbk2FieldPropertyArray& fieldProperties =
                iAppServices->FieldProperties();

            const TInt count( fieldProperties.Count() );
            for (TInt i=0; i < count ; ++i)
                {
                const MPbk2FieldProperty& prop = fieldProperties.At(i);

                if ( IsFieldTypeIncludedL( prop.FieldType(),
                        iAppServices->ContactManager(),
                                R_PHONEBOOK2_LAST_NAME_SELECTOR ) )
                    {
                    MVPbkStoreContactField* field = aContact.CreateFieldLC
                        ( prop.FieldType() );

                    __ASSERT_DEBUG(field->FieldData().DataType() ==
                        EVPbkFieldStorageTypeText, Panic(EAddFindTextL_Logic));

                    MVPbkContactFieldTextData* fieldData =
                        &MVPbkContactFieldTextData::Cast(field->FieldData());
                    fieldData->SetTextL(findText);

                    index = aContact.AddFieldL(field);
                    CleanupStack::Pop(); // field
                    break;
                    }
                }
            }
        }
    return index;
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ShowStoreFullNoteL
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::ShowStoreFullNoteL(
        const MVPbkContactStore& aTargetStore )
    {
    // Fetch store name
    TVPbkContactStoreUriPtr uri =
            aTargetStore.StoreProperties().Uri();
    const CPbk2StoreProperty* storeProperty =
            iAppServices->StoreProperties().FindProperty(uri);

    const TDesC* storeName = NULL;
    if (storeProperty)
        {
        storeName = &storeProperty->StoreName();
        }
    else
        {
        storeName = &uri.UriDes();
        }
    ShowNoteL( R_QTN_PHOB_NOTE_STORE_FULL, *storeName );
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::HandleContactEditingCompleteL
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::HandleContactEditingCompleteL(
        MVPbkStoreContact* aEditedContact )
    {
    __ASSERT_DEBUG( iCommandObserver, Panic(EPanicPreCond_Null_Pointer));
    
    CleanupDeletePushL( aEditedContact );

    // Reset Find before setting the focus because if new contact is created
    // through find box, ResetFindL corrupts the focus.
    if ( iUiControl )
        {
        iUiControl->ResetFindL();
        if ( aEditedContact )
            {
            // Focus the created contact if the contact is shown in the list
            CVPbkContactStoreUriArray* shownStores =
                iAppServices->
                    StoreValidityInformer().CurrentlyValidShownStoresL();
            TVPbkContactStoreUriPtr uri =
                aEditedContact->ParentStore().StoreProperties().Uri();
            TBool valid = shownStores->IsIncluded( uri );
            delete shownStores;

            if ( valid )
                {
                iUiControl->SetFocusedContactL( *aEditedContact );
                }
            }
        }
    CleanupStack::PopAndDestroy(); // aEditedContact
    iState = EFinishCommand;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ShowCreateNewToPhoneQueryL
// --------------------------------------------------------------------------
//
TInt CPbk2CreateNewContactCmd::ShowCreateNewToPhoneQueryL() const
    {
    TInt ret = 0;

    // Do not ask the question if phone memory itself is unavailable
    if ( iAppServices->
            StoreConfiguration().DefaultSavingStoreL().Compare
            ( VPbkContactStoreUris::DefaultCntDbUri(),
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) != 0 )
        {
        HBufC* prompt = 
            StringLoader::LoadLC( R_QTN_PHOB_QUERY_CREATE_NEW_TO_STORE );

        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        ret = dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt );
        CleanupStack::PopAndDestroy( prompt );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::LoadContactStoreL
// --------------------------------------------------------------------------
//    
void CPbk2CreateNewContactCmd::LoadContactStoreL()
    {
    CVPbkContactManager& manager = iAppServices->ContactManager();
    
    manager.LoadContactStoreL( VPbkContactStoreUris::DefaultCntDbUri() );            
    iTargetStore = manager.ContactStoresL().
                    Find( VPbkContactStoreUris::DefaultCntDbUri() );
    
    if ( iTargetStore )
        {
        if ( IsValidStoreL( 
                iAppServices->StoreValidityInformer(), iTargetStore ) )
            {
            iState = EValidateStore;
            }
        else
            {
            iState = EOpenContactStore;
            }        
        }
    else
        {
        iState = EFinishCommand;        
        }
    IssueRequest();        
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::OpenContactStoreL
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::OpenContactStoreL() 
    {
    __ASSERT_DEBUG( iTargetStore, 
        Panic( EPanicPreCond_OpenContactStoreL ) );
                                   
    iTargetStore->OpenL( *this );
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::ValidateStoreL
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::ValidateStoreL()
    {
    __ASSERT_DEBUG( iTargetStore, 
        Panic( EPanicPreCond_ValidateStoreL ) );
    
    if ( StoreFullL( *iTargetStore ) )
        {
        ShowStoreFullNoteL( *iTargetStore );
        iState = EFinishCommand;
        }
    else
        {
        iState = ECreateNewContact; 
        }
        
    IssueRequest();
    }
    
// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::CreateNewContactL
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::CreateNewContactL()
    {
    __ASSERT_DEBUG( iTargetStore, 
        Panic( EPanicPreCond_CreateNewContactL ) );
        
    MVPbkStoreContact* contact = iTargetStore->CreateNewContactLC();

    // verify syncronization field existance and content
    VPbkUtils::VerifySyncronizationFieldL
        ( CCoeEnv::Static()->FsSession(),
          iTargetStore->StoreProperties().SupportedFields(), *contact );

    TUint32 flags = TPbk2ContactEditorParams::ENewContact;
    TPbk2ContactEditorParams params( flags );

    CPbk2ContactEditorDlg* dlg =
        CPbk2ContactEditorDlg::NewL(params, contact, *this);
    CleanupStack::Pop(); // contact, whose ownership was taken away
    dlg->ExecuteLD();
    }

// --------------------------------------------------------------------------
// CPbk2CreateNewContactCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2CreateNewContactCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }
    
// End of File
