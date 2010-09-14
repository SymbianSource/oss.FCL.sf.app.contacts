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
* Description:  Phonebook 2 USIM UI Extension FDN call implementation.
*
*/


// INCLUDE FILES
#include "CPsu2FixedDialingCall.h"

// Phonebook 2
#include "CPsu2NumberQueryDlg.h"
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <pbk2uicontrols.rsg>
#include <MPbk2CommandHandler.h>
#include <CPbk2AppViewBase.h>
#include <CPbk2ContactUiControlSubstitute.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <StringLoader.h>
#include <eikmenup.h>
#include <barsread.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KMaxNumberLength = 52;

/**
 * Checks is the given field type included in
 * the given selection.
 *
 * @param aResourceId   Selector's resource id.
 * @param aFieldType    The field type to check.
 * @param aManager      Virtual Phonebook contact manager.
 * @return  ETrue if field type is included.
 */
TBool IsFieldTypeIncludedL(
        const MVPbkFieldType& aFieldType,
        const CVPbkContactManager& aManager,
        const TInt aResourceId )
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

} /// namespace

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::CPsu2FixedDialingCall
// --------------------------------------------------------------------------
//
CPsu2FixedDialingCall::CPsu2FixedDialingCall
        ( CVPbkContactManager& aContactManager,
          MPbk2CommandHandler& aCommandHandler,
          CPbk2AppViewBase& aAppViewBase ) :
            CActive(EPriorityStandard),
            iContactManager( aContactManager ),
            iCommandHandler( aCommandHandler ),
            iAppViewBase( aAppViewBase )
    {
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::~CPsu2FixedDialingCall
// --------------------------------------------------------------------------
//
CPsu2FixedDialingCall::~CPsu2FixedDialingCall()
    {
    Cancel();
    delete iUiControlSubstitute;
    delete iRetrieveOperation;
    delete iTempContact;
    delete iStoreContact;
    delete iContactLink;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::NewL
// --------------------------------------------------------------------------
//
CPsu2FixedDialingCall* CPsu2FixedDialingCall::NewL(
        CVPbkContactManager& aContactManager,
        MPbk2CommandHandler& aCommandHandler,
        CPbk2AppViewBase& aAppViewBase )
    {
    CPsu2FixedDialingCall* self = new ( ELeave ) CPsu2FixedDialingCall
        ( aContactManager, aCommandHandler, aAppViewBase );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::ConstructL()
    {
    CActiveScheduler::Add(this);

    iUiControlSubstitute = CPbk2ContactUiControlSubstitute::NewL();
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::CreateCallL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::CreateCallL
        ( MVPbkContactLink* aContactLink, TInt aCommand  )
    {
    iCommand = aCommand;
    iContactLink = aContactLink;

    // Load contact
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iRetrieveOperation = iContactManager.RetrieveContactL
        ( *iContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::RunL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::RunL()
    {
    NumberQueryL();
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::DoCancel
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::DoCancel()
    {
    // Nothing to do here
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::RunError
// --------------------------------------------------------------------------
//
TInt CPsu2FixedDialingCall::RunError(TInt aError)
    {
    CCoeEnv::Static()->HandleError(aError);
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    delete iStoreContact;
    iStoreContact = aContact;

    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::ContactTextDataL
// --------------------------------------------------------------------------
//
const TPtrC CPsu2FixedDialingCall::ContactTextDataL
        ( MVPbkStoreContact& aContact, TInt aSelectorResourceId )
    {
    MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    TInt count( fields.FieldCount() );
    for (TInt index(0); index < count; ++index )
        {
        MVPbkStoreContactField& field = fields.FieldAt( index );
        if ( IsFieldTypeIncludedL( *field.BestMatchingFieldType(),
                   iContactManager, aSelectorResourceId ) )
            {
            return MVPbkContactFieldTextData::Cast
                ( field.FieldData() ).Text();
            }
        }
    return KNullDesC();
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::ContactDataTypeL
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CPsu2FixedDialingCall::ContactDataTypeL
    (  const MVPbkFieldTypeList& aFieldTypeList, TInt aSelectorResourceId )
    {
    TInt count( aFieldTypeList.FieldTypeCount() );
    for (TInt index(0); index < count; ++index )
        {
        const MVPbkFieldType& fieldType =
            aFieldTypeList.FieldTypeAt( index );
        if ( IsFieldTypeIncludedL
                ( fieldType, iContactManager, aSelectorResourceId ) )
            {
            return &fieldType;
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::IssueRequest
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::IssueRequest()
    {
    if ( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();        
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingCall::NumberQueryL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingCall::NumberQueryL()
    {
    HBufC* buf = HBufC::NewLC( KMaxNumberLength );
    TPtr numberPtr( buf->Des() );
    CPsu2NumberQueryDlg* dlg = CPsu2NumberQueryDlg::NewL(
        ContactTextDataL( *iStoreContact,
            R_PHONEBOOK2_PHONENUMBER_SELECTOR ),
                numberPtr );

    if ( dlg->ExecuteLD() )
        {
        // Create temporary contact for phone call
        MVPbkContactStore& store = iStoreContact->ParentStore();
        delete iTempContact;
        iTempContact = NULL;
        iTempContact = store.CreateNewContactLC();
        CleanupStack::Pop(); // iTempContact

        const MVPbkFieldTypeList& fieldTypeList =
            store.StoreProperties().SupportedFields();

        // Copy contact name
        const MVPbkFieldType* fieldType = ContactDataTypeL( fieldTypeList,
                R_PHONEBOOK2_LAST_NAME_SELECTOR );
        if ( fieldType )
            {
            MVPbkStoreContactField* tempNameField = iTempContact->CreateFieldLC(
                *fieldType );
            TPtrC namePtr( ContactTextDataL
                ( *iStoreContact, R_PHONEBOOK2_LAST_NAME_SELECTOR ) );
            MVPbkContactFieldTextData::Cast
                ( tempNameField->FieldData() ).SetTextL(namePtr );
            iTempContact->AddFieldL( tempNameField ); // takes ownership
            CleanupStack::Pop(); // tempNameField            
            }

        // Copy phone number
        fieldType =  NULL;
        fieldType = ContactDataTypeL
                ( fieldTypeList, R_PHONEBOOK2_PHONENUMBER_SELECTOR );
        MVPbkStoreContactField* tempNumberField = NULL;
        TInt tempNumberFieldIndex = KErrNotFound;
        if ( fieldType )
            {
            tempNumberField =
                iTempContact->CreateFieldLC( *fieldType );
            MVPbkContactFieldTextData::Cast
                ( tempNumberField->FieldData() ).SetTextL( numberPtr );
            tempNumberFieldIndex =
                iTempContact->AddFieldL(tempNumberField); // takes ownership
            CleanupStack::Pop(); // tempNumberField
            tempNumberField = NULL;            
            tempNumberField = &iTempContact->Fields().FieldAt
                ( tempNumberFieldIndex );
            }
            
        iUiControlSubstitute->SetFocusedStoreContact( *iTempContact );
        iUiControlSubstitute->SetFocusedField( *tempNumberField );
        iUiControlSubstitute->SetFocusedFieldIndex( tempNumberFieldIndex );

        iCommandHandler.HandleCommandL
            ( iCommand, *iUiControlSubstitute, &iAppViewBase );
        }

    CleanupStack::PopAndDestroy( buf );
    }

//  End of File
