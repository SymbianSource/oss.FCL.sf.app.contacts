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
* Description:  Copy contacts operation
*
*/


#include "CVPbkCopyContactsOperation.h"

#include <MVPbkBatchOperationObserver.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactCopyPolicyManager.h>
#include <MVPbkContactCopyPolicy.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactCopier.h>
#include <CVPbkContactDuplicatePolicy.h>

namespace {

const TInt KOneStep = 1;
const TInt KDuplicatesToFind = 1;

TInt CurNumberOfField( MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType )
    {
    MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    TInt res = 0;
    const TInt count = fields.FieldCount();
    for (TInt i = 0; i < count; ++i)
        {
        const MVPbkFieldType* type = fields.FieldAt(i).BestMatchingFieldType();
        if (type && type->IsSame(aFieldType))
            {
            ++res;
            }
        }
    return res;
    }
}

CVPbkCopyContactsOperation::CVPbkCopyContactsOperation(
        CVPbkContactManager& aContactManager,
        const MVPbkContactLinkArray& aLinks,
        MVPbkContactStore* aTargetStore,
        CVPbkContactLinkArray* aCopiedContactLinks,
        MVPbkBatchOperationObserver& aObserver)
        :   CActive( EPriorityStandard ),
            iContactManager( aContactManager ),
            iLinks( aLinks ),
            iTargetStore( aTargetStore ),
            iCopiedContactLinks( aCopiedContactLinks ),
            iObserver( aObserver ),
            iState( ERetrieveNextContact )
    {
    CActiveScheduler::Add(this);
    }

inline void CVPbkCopyContactsOperation::ConstructL( TUint32 aCopyContactFlags )
    {
    // If client gave NULL target store then contacts are duplicated
    if ( !iTargetStore )
        {
        iDuplicateContacts = ETrue;
        }

    SetTargetStoreL();

    const TUint32 copyPolicyFlags =
        CVPbkContactCopier::EVPbkUseStoreSpecificCopyPolicy |
        CVPbkContactCopier::EVPbkUsePlatformSpecificDuplicatePolicy;
    const TUint32 duplicatePolicyFlags =
        CVPbkContactCopier::EVPbkUsePlatformSpecificDuplicatePolicy;

    // Check if copy policy is needed
    if ( aCopyContactFlags & copyPolicyFlags )
        {
        iCopyPolicyManager = CVPbkContactCopyPolicyManager::NewL();
        if ( iTargetStore )
            {
            iCopyPolicy = iCopyPolicyManager->GetPolicyL( iContactManager,
                iTargetStore->StoreProperties().Uri() );
            }
        }

    // Check if duplicate policy is needed
    if ( aCopyContactFlags & duplicatePolicyFlags && iCopyPolicy &&
         iCopyPolicy->SupportsContactMerge() )
        {
        CVPbkContactDuplicatePolicy::TParam param( iContactManager );
        TRAPD( res, iDuplicatePolicy = CVPbkContactDuplicatePolicy::NewL( param ));
        // Don't leave if duplicate policy is not found -> duplicate checking
        // is not used.
        if ( res != KErrNone && res != KErrNotFound )
            {
            User::LeaveIfError( res );
            }
        }
    }

CVPbkCopyContactsOperation* CVPbkCopyContactsOperation::NewLC(
        TUint32 aCopyContactFlags,
        CVPbkContactManager& aContactManager,
        const MVPbkContactLinkArray& aLinks,
        MVPbkContactStore* aTargetStore,
        CVPbkContactLinkArray* aCopiedContactLinks,
        MVPbkBatchOperationObserver& aObserver)
    {
    CVPbkCopyContactsOperation* self = new(ELeave) CVPbkCopyContactsOperation(
            aContactManager,
            aLinks,
            aTargetStore,
            aCopiedContactLinks,
            aObserver);
    CleanupStack::PushL(self);
    self->ConstructL( aCopyContactFlags );
    return self;
    }

CVPbkCopyContactsOperation::~CVPbkCopyContactsOperation()
    {
    Cancel();
    delete iDuplicatePolicy;
    delete iCopyPolicyManager;
    delete iStoreContact;
    delete iOperation;
    iDuplicates.ResetAndDestroy();
    iSourceContactsForMerge.ResetAndDestroy();
    }

void CVPbkCopyContactsOperation::DoCancel()
    {
    }

void CVPbkCopyContactsOperation::RunL()
    {
    switch (iState)
        {
        case ERetrieveNextContact:
            {
            RetrieveNextContactL();
            break;
            }
        case EDefaultCopy:
            {
            DefaultCopyL();
            break;
            }
        case ECopyUsingPolicy:
            {
            CopyUsingPolicyL();
            break;
            }
        case EFindDuplicates:
            {
            FindDuplicatesL();
            break;
            }
        case EComplete:
            {
            iObserver.OperationComplete(*this);
            break;
            }
        }
    }

TInt CVPbkCopyContactsOperation::RunError(TInt aError)
    {
    ContinueAfterFailure( aError );
    return KErrNone;
    }

void CVPbkCopyContactsOperation::StartL()
    {
    IssueRequest();
    }

void CVPbkCopyContactsOperation::Cancel()
    {
    CActive::Cancel();
    }

void CVPbkCopyContactsOperation::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact)
    {
    delete iOperation;
    iOperation = NULL;
    delete iStoreContact;
    iStoreContact = aContact;

    // Default action is own copy logic
    iState = EDefaultCopy;
    if ( iDuplicatePolicy && iCopyPolicy )
        {
        // But if the duplicate policy exists it's done before copying
        iState = EFindDuplicates;
        }
    else if ( iCopyPolicy )
        {
        // Duplicate checking was not defined but copy policy was.
        // Make more advanced copy using the policy
        iState = ECopyUsingPolicy;
        }
    IssueRequest();
    }

void CVPbkCopyContactsOperation::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError)
    {
    delete iOperation;
    iOperation = NULL;

    ContinueAfterFailure( aError );
    }

void CVPbkCopyContactsOperation::ContactsSaved(
        MVPbkContactOperationBase& aOperation,
        MVPbkContactLinkArray* aResults)
    {
    delete iOperation;
    iOperation = NULL;
    delete iStoreContact;
    iStoreContact = NULL;

    TRAPD( res,
        {
        CleanupDeletePushL( aResults );
        AppendResultsL( *aResults );
        CleanupStack::PopAndDestroy(); // aResults
        });

    if ( res == KErrNone )
        {
        // One contact copied, notify observer and start retrieve next contact.
        iObserver.StepComplete( *this, KOneStep );

        iState = ERetrieveNextContact;
        IssueRequest();
        }
    else
        {
        ContactsSavingFailed( aOperation, res );
        }
    }

void CVPbkCopyContactsOperation::ContactsSavingFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError)
    {
    ContinueAfterFailure( aError );
    }

void CVPbkCopyContactsOperation::ContactOperationCompleted(
        TContactOpResult aResult )
    {
    // Default copy successfully done -> continue with next contact
    delete iOperation;
    iOperation = NULL;

    TRAPD( res,
        {
        if ( iCopiedContactLinks )
            {
            iCopiedContactLinks->AppendL( iStoreContact->CreateLinkLC() );
            CleanupStack::Pop(); // link
            }
        });

    if ( res == KErrNone )
        {
        iObserver.StepComplete( *this, KOneStep );
        iState = ERetrieveNextContact;
        IssueRequest();
        }
    else
        {
        ContactOperationFailed( aResult.iOpCode, res, EFalse );
        }
    }

void CVPbkCopyContactsOperation::ContactOperationFailed( TContactOp /*aOpCode*/,
        TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    // Default copy failed -> send step failed and continue with next contact
    ContinueAfterFailure( aErrorCode );
    }

void CVPbkCopyContactsOperation::FindCompleteL( MVPbkContactLinkArray* aResults )
    {
    delete aResults;
    delete iOperation;
    iOperation = NULL;

    if ( iDuplicates.Count() > 0 )
        {
        // Merge iStoreContact to the duplicate.
        iSourceContactsForMerge.ResetAndDestroy();
        /// Ownership of the iStoreContact changes
        iSourceContactsForMerge.AppendL( iStoreContact );
        iStoreContact = NULL;
        iOperation = iCopyPolicy->MergeAndSaveContactsL(
            iSourceContactsForMerge, *iDuplicates[0], *this );
        }
    else
        {
        // No duplicates found -> use copy policy to copy contact
        iState = ECopyUsingPolicy;
        IssueRequest();
        }
    }

void CVPbkCopyContactsOperation::FindFailed( TInt aError )
    {
    ContinueAfterFailure( aError );
    }

void CVPbkCopyContactsOperation::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CVPbkCopyContactsOperation::SetTargetStoreL()
    {
    if ( iDuplicateContacts && iCurrentLinkIndex < iLinks.Count() )
        {
        // if duplicates are created then set the store to the store of the
        // current link
        iTargetStore = &iLinks.At( iCurrentLinkIndex ).ContactStore();
        if ( iTargetStore && iCopyPolicyManager )
            {
            iCopyPolicy = iCopyPolicyManager->GetPolicyL( iContactManager,
                    iTargetStore->StoreProperties().Uri() );
            }
        }
    }

void CVPbkCopyContactsOperation::RetrieveNextContactL()
    {
    // In duplicate mode target store is same as source store
    // so this needs to be checked every time
    SetTargetStoreL();

    delete iStoreContact;
    iStoreContact = NULL;

    if ( iCurrentLinkIndex < iLinks.Count() )
        {
        iOperation = iContactManager.RetrieveContactL(
                iLinks.At( iCurrentLinkIndex++ ), *this);
        }
    else
        {
        iState = EComplete;
        IssueRequest();
        }
    }

void CVPbkCopyContactsOperation::DefaultCopyL()
    {
    // Own simple copy logic. Copy the field if the target supports it and
    // store contact has space for the new field.
    // Copy labels if target field supports label.
    const MVPbkContactStoreProperties& props = iTargetStore->StoreProperties();
    const MVPbkFieldTypeList& supportedFields = props.SupportedFields();
    MVPbkStoreContact* newContact = iTargetStore->CreateNewContactLC();
    MVPbkStoreContactFieldCollection& fields = iStoreContact->Fields();
    const TInt fieldCount = fields.FieldCount();
    for (TInt i = 0; i < fieldCount; ++i)
        {
        MVPbkStoreContactField& field = fields.FieldAt(i);
        const MVPbkFieldType* fieldType = field.BestMatchingFieldType();
        if (fieldType && supportedFields.ContainsSame(*fieldType))
            {
            TInt maxNumber = newContact->MaxNumberOfFieldL(*fieldType);
            if (maxNumber == KVPbkStoreContactUnlimitedNumber ||
                CurNumberOfField(*newContact, *fieldType) < maxNumber)
                {
                MVPbkStoreContactField* newField =
                    newContact->CreateFieldLC(*fieldType);
                TPtrC label(field.FieldLabel());
                if (newField->SupportsLabel() &&
                    label.Length() > 0)
                    {
                    newField->SetFieldLabelL(label);
                    }
                newField->FieldData().CopyL(field.FieldData());
                newContact->AddFieldL(newField);
                CleanupStack::Pop(); // newField
                }
            }
        }

    if ( newContact->Fields().FieldCount() > 0 )
        {
        // Save the new contact if fields were copied
        delete iStoreContact;
        iStoreContact = newContact;
        CleanupStack::Pop(); // newContact
        iStoreContact->CommitL( *this );
        }
    else
        {
        // No fields were copied -> Continue with next contact.
        CleanupStack::PopAndDestroy(); // newContact
        // Return success.
        iObserver.StepComplete( *this, KOneStep );
        iState = ERetrieveNextContact;
        IssueRequest();
        }
    }

void CVPbkCopyContactsOperation::CopyUsingPolicyL()
    {
    iOperation = iCopyPolicy->CopyContactL( *iStoreContact, *iTargetStore,
        *this );
    }

void CVPbkCopyContactsOperation::FindDuplicatesL()
    {
    iDuplicates.ResetAndDestroy();
    iOperation = iDuplicatePolicy->FindDuplicatesL( *iStoreContact,
        *iTargetStore, iDuplicates, *this, KDuplicatesToFind );
    }

void CVPbkCopyContactsOperation::ContinueAfterFailure( TInt aError )
    {
    delete iOperation;
    iOperation = NULL;

    iState = EComplete;
    if ( iObserver.StepFailed( *this, KOneStep, aError ) )
        {
        // Continue only if ETrue is answered. This operation might be deleted
        // if EFalse is answered.
        iState = ERetrieveNextContact;
        IssueRequest();
        }
    }

void CVPbkCopyContactsOperation::AppendResultsL( MVPbkContactLinkArray& aLinks )
    {
    if ( iCopiedContactLinks )
        {
        const TInt count = aLinks.Count();
        for ( TInt i = 0; i < count; ++i )
            {
            iCopiedContactLinks->AppendL( aLinks.At(i).CloneLC() );
            CleanupStack::Pop(); // link
            }
        }
    }
// End of File
