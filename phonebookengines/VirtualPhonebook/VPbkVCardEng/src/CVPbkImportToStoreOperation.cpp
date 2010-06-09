/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An operation for importing and saving a contact to the store
*
*/


#include "CVPbkImportToStoreOperation.h"

#include "CVPbkVCardCompactBCardImporter.h"
#include "CVPbkVCardImporter.h"
#include "CVPbkVCardData.h"
#include "VPbkVCardEngError.h"

// From Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactCopyPolicy.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactDuplicatePolicy.h>

/// Unnamed namespace for local definitions
namespace 
    {
    // Max number of duplicate contacts to find
    const TInt KDuplicatesToFind = 1;
    }

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CVPbkImportToStoreOperation::CVPbkImportToStoreOperation
// ---------------------------------------------------------------------------
//
CVPbkImportToStoreOperation::CVPbkImportToStoreOperation(
        MVPbkContactCopyObserver& aObserver,
        MVPbkContactStore& aTargetStore )
        :   iTargetStore( aTargetStore ),
            iObserver( aObserver )
    {
    iOwncardHandler = NULL;
    iGroupcardHandler = NULL;
    }


// ---------------------------------------------------------------------------
// CVPbkImportToStoreOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::ConstructL( TVPbkImportCardType aType,
        RReadStream &aSourceStream, CVPbkVCardData& aData, TBool aSync )
    {
    iCopyPolicy = &aData.CopyPolicyL( iTargetStore.StoreProperties().Uri() );
    if ( !iCopyPolicy )
        {
        User::Leave( KErrNotSupported );
        }

    // If contact data merge is supported then use duplicate checking
    // before copying.
    // During sync duplicate policy is ignored to improve performance
    if ( iCopyPolicy->SupportsContactMerge() && ! aSync )
        {
           iDuplicatePolicy = aData.DuplicatePolicy();
        }

    if ( aType == ECompactBusinessCard )
        {
        iOperationImpl = CVPbkVCardCompactBCardImporter::NewL(
            iImportedContacts, iTargetStore, aSourceStream, aData );
        }
    else if ( aType == EVCard )
        {
        iOperationImpl = CVPbkVCardImporter::NewL(
            iImportedContacts, aSourceStream, iTargetStore, aData );
        }
    else
        {
        User::Leave( KErrArgument );
        }

    iSavedContacts = CVPbkContactLinkArray::NewL();
   // Handle Owncard during Sync
    if(aSync)
	    {
	    iOwncardHandler = CVPbkOwnCardHandler::NewL(aData);	
	    iGroupcardHandler = CVPbkGroupCardHandler::NewL(aData);
	    }
    }

// ---------------------------------------------------------------------------
// CVPbkImportToStoreOperation::NewL
// ---------------------------------------------------------------------------
//
CVPbkImportToStoreOperation* CVPbkImportToStoreOperation::NewL(
        TVPbkImportCardType aType,
        MVPbkContactStore& aTargetStore,
        RReadStream& aSourceStream,
        CVPbkVCardData& aData,
        MVPbkContactCopyObserver& aObserver,
        TBool aSync )
    {
    CVPbkImportToStoreOperation* self =
        new( ELeave ) CVPbkImportToStoreOperation( aObserver, aTargetStore );
    CleanupStack::PushL( self );
    self->ConstructL( aType, aSourceStream, aData, aSync );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CVPbkImportToStoreOperation::~CVPbkImportToStoreOperation
// ---------------------------------------------------------------------------
//
CVPbkImportToStoreOperation::~CVPbkImportToStoreOperation()
    {
    delete iVPbkOperation;
    delete iSavedContacts;
    delete iOperationImpl;
    iImportedContacts.ResetAndDestroy();
    iDuplicates.ResetAndDestroy();
    delete iOwncardHandler;
    delete iGroupcardHandler;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CVPbkImportToStoreOperation::StartL
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::StartL()
    {
    __ASSERT_DEBUG( iOperationImpl, VPbkVCardEngError::Panic(
        VPbkVCardEngError::EPreCond_CVPbkImportToStoreOperation_StartL ) );

    iOperationImpl->SetObserver( *this );
    iOperationImpl->StartL();
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperation
// CVPbkImportToStoreOperation::Cancel
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::Cancel()
    {
    delete iOperationImpl;
    iOperationImpl = NULL;
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactsImported
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::ContactsImported()
    {
    TInt err( KErrNone );
    if ( iImportedContacts.Count() == 0 )
        {
        err = KErrNotFound;
        }
    else if ( iDuplicatePolicy )
        {
        ResetOperation();
        iDuplicates.ResetAndDestroy();
        TRAP( err, iVPbkOperation = iDuplicatePolicy->FindDuplicatesL(
            *iImportedContacts[0], iTargetStore, iDuplicates, *this,
            KDuplicatesToFind ) );
        }
    else
        {
        TRAP( err, CommitNextContactL() );
        }
    // delegate errors
    if ( err != KErrNone )
        {
        ContactImportingFailed( err );
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactsImportedCompleted
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::ContactsImportingCompleted()
{
    MVPbkContactLinkArray* results = iSavedContacts;
    iSavedContacts = NULL;
    
    // Notice that client can delete this operation in
    // ContactsSaved so there must be no usage of member data
    // after calling this.
    if (results->Count())
        {
        // One or more successfully saved contacts
        iObserver.ContactsSaved( *this, results );
        }
    else
        {
        // No imported and saved contacts available
        ContactImportingFailed(KErrNotFound);
        }
    
};

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactImportingFailed
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::ContactImportingFailed( TInt aError )
    {
    iObserver.ContactsSavingFailed( *this, aError );
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactsSaved
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::ContactsSaved(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkContactLinkArray* aResults )
    {
    ResetOperation();
    TRAPD( res, HandleContactSavedL( aResults ) );
    if ( res != KErrNone )
        {
        ContactImportingFailed( res );
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkImportOperationObserver
// CVPbkImportToStoreOperation::ContactsSavingFailed
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::ContactsSavingFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError)
    {
    ContactImportingFailed( aError );
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactFindObserver
// CVPbkImportToStoreOperation::FindCompleteL
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::FindCompleteL(
        MVPbkContactLinkArray* aResults )
    {
    // Links not needed
    delete aResults;

    if ( iDuplicates.Count() > 0 )
        {
        // Merge and commit, let policy decide how to handle merge and
        // what to do with imported contacts. Policy takes the contacts.
        ResetOperation();
        iVPbkOperation = iCopyPolicy->MergeAndSaveContactsL(
            iImportedContacts, *iDuplicates[0], *this );
        }
    else
        {
        // No duplicates, save imported contacts.
        CommitNextContactL();
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactFindObserver
// CVPbkImportToStoreOperation::FindFailed
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::FindFailed( TInt aError )
    {
    ContactImportingFailed( aError );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToStoreOperation::CommitNextContactL
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::CommitNextContactL()
    {
    __ASSERT_DEBUG( iImportedContacts.Count() > 0,
        VPbkVCardEngError::Panic(
            VPbkVCardEngError::EPreCond_CommitNextContactL ) );
    const TInt firstContact = 0;
    MVPbkStoreContact* contact = iImportedContacts[firstContact];
    /// contact ownership is given
    ResetOperation();
    iVPbkOperation = iCopyPolicy->CommitContactL( contact, *this );
    iImportedContacts.Remove( firstContact );
    }

// ---------------------------------------------------------------------------
// CVPbkImportToStoreOperation::HandleContactSavedL
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::HandleContactSavedL(
        MVPbkContactLinkArray* aResults )
    {
    CleanupDeletePushL( aResults );
    const TInt count = aResults->Count();
    
    TBool added = EFalse;
    for ( TInt i = 0; i < count; ++i )
        {
        iSavedContacts->AppendL( (*aResults)[i].CloneLC() );
        added = ETrue;
        CleanupStack::Pop();
        }

    const TInt duplicateCount = iDuplicates.Count();
    const TInt curUnsavedCount = iImportedContacts.Count();
    // Duplicates are returned to the client if following conditions apply
    // 1) There are duplicates (iDuplicates.Count() > 0)
    // 2) All imported contacts have been saved (iImportedContacts.Count() == 0)
    // 3) Copy policy didn't actually save anything
    //    (iSavedContacts->Count() == 0 )
    //if ( ( duplicateCount > 0 ) &&
    //     ( curUnsavedCount == 0 ) &&
    //     ( iSavedContacts->Count() == 0 ) )
         
     if ( ( duplicateCount > 0 ) &&
          ( curUnsavedCount == 0 ) &&
          ( added == EFalse ) )     
        {
        for ( TInt k = 0; k < duplicateCount; ++k )
            {
            iSavedContacts->AppendL( iDuplicates[k]->CreateLinkLC() );
            CleanupStack::Pop();
            }
        }
		if(iOwncardHandler && iOperationImpl->IsOwncard())
	        {
	    	iOwncardHandler->SetAsOwnContactL(aResults->At(0));
	    	}
     if(iGroupcardHandler && ((CVPbkVCardImporter *)iOperationImpl)->IsGroupcard())
         {
         iGroupcardHandler->BuildContactGroupsHashMapL(iTargetStore);
         iGroupcardHandler->GetContactGroupStoreL(aResults->At(0));
         iGroupcardHandler->DecodeContactGroupInVCardL(((CVPbkVCardImporter *)iOperationImpl)->GetGroupcardvalue());
         }
    CleanupStack::PopAndDestroy(); // aResults

    if ( curUnsavedCount == 0 )
        {
        // Try to read the next entry, if one existed
        iOperationImpl->StartL();
        }
    else
        {
        CommitNextContactL();
        iOperationImpl->StartL();
        }

    }

// ---------------------------------------------------------------------------
// CVPbkImportToStoreOperation::ResetOperation
// ---------------------------------------------------------------------------
//
void CVPbkImportToStoreOperation::ResetOperation()
    {
    delete iVPbkOperation;
    iVPbkOperation = NULL;
    }
// End of file
