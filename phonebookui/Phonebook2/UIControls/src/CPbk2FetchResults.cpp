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
* Description:  Phonebook 2 fetch results.
*
*/


#include "CPbk2FetchResults.h"

// Phonebook 2
#include "MPbk2FetchDlg.h"
#include "MPbk2FetchDlgPage.h"
#include "MPbk2FetchDlgPages.h"
#include "MPbk2FetchResultsObserver.h"
#include <MPbk2FetchDlgObserver.h>
#include <MPbk2ContactUiControl.h>

// Virtual Phonebook
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactViewBase.h>

// System includes
#include <coemain.h>
#include <akninputblock.h>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2FetchResults::CPbk2FetchResults
// --------------------------------------------------------------------------
//
CPbk2FetchResults::CPbk2FetchResults
        ( CVPbkContactManager& aContactManager,
          MPbk2FetchDlg& aFetchDlg,
          MPbk2FetchDlgPages& aPages,
          MPbk2FetchDlgObserver& aObserver,
          MPbk2FetchResultsObserver& aResultsObserver ) :
                iContactManager( aContactManager ),
                iFetchDlg( aFetchDlg ),
                iPages( aPages ),
                iObserver( aObserver ),
                iResultsObserver( aResultsObserver ),
                iWaitingForDelayedAppend( EFalse ),
                iInputBlock( NULL )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::~CPbk2FetchResults
// --------------------------------------------------------------------------
//
CPbk2FetchResults::~CPbk2FetchResults()
    {
    if ( iStoreList )
        {
        for ( TInt i = 0; i < iStoreList->Count(); ++i )
            {
            MVPbkContactStore& store = iStoreList->At( i );
            store.Close( *this );
            }
        }

    delete iSelectedContacts;
    
    delete iRetrieveOperation;
    
    if( iOperationQueue )
        {
        for ( TInt k = 0; k < iOperationQueue->Count(); k++ )
            delete iOperationQueue->At(k);
        
        delete iOperationQueue;
        }
    
    delete iWaitForAllOperationComplete;
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FetchResults::ConstructL
        ( CVPbkContactManager& aContactManager )
    {
    iSelectedContacts = CVPbkContactLinkArray::NewL();

    iStoreList = &aContactManager.ContactStoresL();

    const TInt storeCount = iStoreList->Count();
    for (TInt i = 0; i < storeCount; ++i)
        {
        MVPbkContactStore& store = iStoreList->At( i );
        store.OpenL( *this );
        }
    
    iOperationQueue = new (ELeave) CArrayFixFlat<CFRConatactOperation*>(4);
    
    iWaitForAllOperationComplete = new (ELeave) CActiveSchedulerWait();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::NewL
// --------------------------------------------------------------------------
//
CPbk2FetchResults* CPbk2FetchResults::NewL
        ( CVPbkContactManager& aContactManager,
          MPbk2FetchDlg& aFetchDlg, MPbk2FetchDlgPages& aPages,
          MPbk2FetchDlgObserver& aObserver,
          MPbk2FetchResultsObserver& aResultsObserver )
    {
    CPbk2FetchResults* self = new ( ELeave ) CPbk2FetchResults
        ( aContactManager, aFetchDlg, aPages, aObserver,
          aResultsObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aContactManager );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::AppendL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::AppendL( const MVPbkContactLink& aLink )
    { 
    if ( iFetchDlg.IsSelectPermitted() )
        {
        CFRConatactOperation* newAppendOperation = CFRConatactOperation::NewL( aLink, CFRConatactOperation::EAppendContact );
        AppendContactOperationL(newAppendOperation);
        ProcessNextContactOperationL();
        }
    else
        {
        iPages.SelectContactL( aLink, EFalse );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::AppendDelayedL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::AppendDelayedL( const MVPbkContactLink& aLink )
    {
    // Add the feedback contact's operation to the head of the queue
    // in order to ensure that the checked contact's operation to be
    // handled firstly when phonebook receives the client feedback.
    CFRConatactOperation* newAppendOperation = CFRConatactOperation::NewL( aLink, CFRConatactOperation::EAppendContactDelayed );
    iOperationQueue->InsertL( 0, newAppendOperation ); 
    // Waiting is over.
    iWaitingForDelayedAppend = EFalse;
    ProcessNextContactOperationL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::DenyAppendDelayedL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::DenyAppendDelayedL( const MVPbkContactLink& aLink )
    {
    // Waiting is over, process next contact operation
    iWaitingForDelayedAppend = EFalse;
    ProcessNextContactOperationL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::RemoveL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::RemoveL( const MVPbkContactLink& aLink )
    {
    CFRConatactOperation* newRemoveOperation = CFRConatactOperation::NewL( aLink, CFRConatactOperation::ERemoveContact );
    AppendContactOperationL(newRemoveOperation);
    ProcessNextContactOperationL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::ResetAndDestroy
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::ResetAndDestroy()
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    RemoveCurrentContactOperation();

    iSelectedContacts->ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::WaitOperationsCompleteL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::WaitOperationsCompleteL()
    {
    if ( iWaitingForDelayedAppend || iOperationQueue->Count()>0 )
        {
        iInputBlock = CAknInputBlock::NewLC();
        CleanupStack::Pop( iInputBlock );
        
        iWaitForAllOperationComplete->Start();
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::Count
// --------------------------------------------------------------------------
//
TInt CPbk2FetchResults::Count() const
    {
    return iSelectedContacts->Count();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::At
// --------------------------------------------------------------------------
//
const MVPbkContactLink& CPbk2FetchResults::At( TInt aIndex ) const
    {
    return iSelectedContacts->At( aIndex );
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::Find
// --------------------------------------------------------------------------
//
TInt CPbk2FetchResults::Find( const MVPbkContactLink& aLink ) const
    {
    return iSelectedContacts->Find( aLink );
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::PackLC
// --------------------------------------------------------------------------
//
HBufC8* CPbk2FetchResults::PackLC() const
    {
    return iSelectedContacts->PackLC();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::Streamable
// --------------------------------------------------------------------------
//
const MVPbkStreamable* CPbk2FetchResults::Streamable() const
    {
    return iSelectedContacts->Streamable();
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& aOperation,
          MVPbkStoreContact* aContact )
    {
    TRAPD( err, HandleContactOperationCompleteL( aOperation, aContact ) );

    if ( err != KErrNone )
        {
        iResultsObserver.ContactSelectionFailed();
        CCoeEnv::Static()->HandleError( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iResultsObserver.ContactSelectionFailed();

    CCoeEnv::Static()->HandleError( aError );
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::StoreReady
        ( MVPbkContactStore& /*aContactStore*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::StoreUnavailable
        ( MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent aStoreEvent )
    {
    switch ( aStoreEvent.iEventType )
        {
        case TVPbkContactStoreEvent::EContactDeleted:
            {
            // Do not update UI selections - just remove the link
            // from results. The view event, listened by the control,
            // takes care of updating the UI selections.
            TInt findResult = iSelectedContacts->Find( *aStoreEvent.iContactLink );
            if ( findResult != KErrNotFound )
                {
                User::LeaveIfError( findResult );
                iSelectedContacts->Delete( findResult );
                }
            break;
            }

        case TVPbkContactStoreEvent::EGroupDeleted:
            {
            // Do nothing.
            // This means that we keep the selection of the contacts
            // which belonged to a group when the group is deleted.
            break;
            }

        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::DoAppendContactL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::DoAppendContactL
        ( MVPbkStoreContact& aContact, TBool aDelayed )
    {
    MVPbkContactLink* link = aContact.CreateLinkLC();

    TInt count = CountContactsL( aContact, *link );

    // Ask observer for acceptance
    MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection accept =
        MPbk2FetchDlgObserver::EFetchYes;

    if ( !aDelayed )
        {
        // Observer needs to asked
        accept = iObserver.AcceptFetchSelectionL( count, *link );
        // Determine whether accept query is needed
        if ( MPbk2FetchDlgObserver::EFetchDelayed == accept )
            {
            // Start to wait client feedback, block the operation queue,
            // process next operation until receive client feedback.
            iWaitingForDelayedAppend = ETrue;
            }
        }

    if ( accept == MPbk2FetchDlgObserver::EFetchNo )
        {
        // Observer did not accept or delayed the selection
        MVPbkContactViewBase& view = iFetchDlg.FetchDlgViewL
            ( iPages.CurrentPage().FetchDlgPageId() );
        TInt index = view.IndexOfLinkL( *link );
        iPages.CurrentPage().Control().SetSelectedContactL
            ( index, EFalse );
        }
    else if( accept == MPbk2FetchDlgObserver::EFetchYes )
        {
        MVPbkContactGroup* group = aContact.Group();
        if ( group )
            {
            // Contact was a group, insert group members into append queue
            MVPbkContactLinkArray* groupMembers = group->ItemsContainedLC();

            for ( TInt i = 0; i < groupMembers->Count(); ++i )
                {
                AppendToResultsL( groupMembers->At( i ) );
                }

            CleanupStack::PopAndDestroy(); // groupMembers
            }
        else
            {
            AppendToResultsL( *link );
            }

        iResultsObserver.ContactSelected( *link, ETrue );
        }

    CleanupStack::PopAndDestroy(); // link
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::DoRemoveContactL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::DoRemoveContactL( MVPbkStoreContact& aContact )
    {
    MVPbkContactLink* link = aContact.CreateLinkLC();

    MVPbkContactGroup* group = aContact.Group();
    if ( group )
        {
        MVPbkContactLinkArray* groupMembers = group->ItemsContainedLC();

        for ( TInt i = 0; i < groupMembers->Count(); ++i )
            {
            RemoveFromResultsL( groupMembers->At( i ) );
            }

        CleanupStack::PopAndDestroy(); // groupMembers
        }
    else
        {
        RemoveFromResultsL( *link );
        }

    iResultsObserver.ContactSelected( *link, EFalse );

    CleanupStack::PopAndDestroy(); // link
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::AppendToResultsL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::AppendToResultsL( const MVPbkContactLink& aLink )
    {
    TInt findResult = iSelectedContacts->Find( aLink );

    if ( findResult == KErrNotFound )
        {
        iPages.SelectContactL( aLink, ETrue );
        MVPbkContactLink* clone = aLink.CloneLC();
        CleanupStack::Pop(); // clone
        iSelectedContacts->AppendL( clone );
        }
    else
        {
        User::LeaveIfError( findResult );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::RemoveFromResultsL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::RemoveFromResultsL( const MVPbkContactLink& aLink )
    {
    TInt findResult = iSelectedContacts->Find( aLink );
    if ( findResult != KErrNotFound )
        {
        User::LeaveIfError( findResult );
        iPages.SelectContactL( aLink, EFalse );
        iSelectedContacts->Delete( findResult );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::CountContactsL
// --------------------------------------------------------------------------
//
TInt CPbk2FetchResults::CountContactsL
        ( MVPbkStoreContact& aContact, MVPbkContactLink& aLink )
    {
    TInt count = iSelectedContacts->Count();

    MVPbkContactGroup* group = aContact.Group();

    if ( group )
        {
        // Contact was a group, insert group members into append queue
        MVPbkContactLinkArray* groupMembers = group->ItemsContainedLC();

        for ( TInt i = 0; i < groupMembers->Count(); ++i )
            {
            if ( iSelectedContacts->Find( groupMembers->At( i ) )
                    == KErrNotFound )
                {
                ++count;
                }
            }
        CleanupStack::PopAndDestroy(); // groupMembers
        }
    else
        {
        if ( iSelectedContacts->Find( aLink ) == KErrNotFound )
            {
            ++count;
            }
        }

    return count;
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::HandleContactOperationCompleteL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::HandleContactOperationCompleteL(
                         MVPbkContactOperationBase& /*aOperation*/,
                         MVPbkStoreContact* aContact )
    {
    /*************** Do class member operations here *****************/
    
    // Delete retrieve contact operation ptr
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    
    // Copy current operation type before deleting
    CFRConatactOperation::EOperationType operationType = 
                          GetCurrentContactOperation()->GetOperationType();
    
    // Remove executed operation from queue
    RemoveCurrentContactOperation();
    
    
    /**********************************************************************/
    /**************! Dont use class members after SWITCH !*****************/
    /*** DoAppendContactL, DoRemoveContactL are able to call destructor ***/
    /**********************************************************************/
    
    // Finish contact operation
    CleanupDeletePushL( aContact );
    
    switch( operationType )
        {
        case CFRConatactOperation::EAppendContact:
            {
            DoAppendContactL( *aContact, EFalse );
            // Check whether next operation can be processed
            // after sending check request to client.
            ProcessNextContactOperationL();
            break;
            }
            
        case CFRConatactOperation::EAppendContactDelayed:
            {
            // Process new contact opretation before appending 
            // the delayed contact to results. It can save time.
            ProcessNextContactOperationL();   
            DoAppendContactL( *aContact, ETrue );
            break;
            }
        
        case CFRConatactOperation::ERemoveContact:
            {
            // Process new contact opretation before removing contact 
            // from results. It can save time.
            ProcessNextContactOperationL();   
            DoRemoveContactL( *aContact );
            break;
            }
        }
    
    CleanupStack::PopAndDestroy(); // aContact
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::ProcessNextContactOperationL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::ProcessNextContactOperationL()
    {
    if( !iRetrieveOperation && !iWaitingForDelayedAppend )
        {        
        // No operation is executing -> process next one
        
        if( GetCurrentContactOperation() )
            {
            // Start asynchronous contact retrieve operation, which completes
            // in CPbk2FetchResults::HandleContactOperationCompleteL.
			iRetrieveOperation =  iContactManager.RetrieveContactL( 
                    *( GetCurrentContactOperation()->GetContactLink() ),
                    *this );
            }
        else
            {
            if ( !iWaitingForDelayedAppend )
                {
                // Every seleced contacts' operation finishes,
                // stop the scheduler waiting for every operation.
                if (iWaitForAllOperationComplete->IsStarted())
                    iWaitForAllOperationComplete->AsyncStop();
                
                // Stop blocking input 
                delete iInputBlock;
                iInputBlock = NULL;
                }
            }
        }
    else
        {
        // Ongoing operation -> do nothing
        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING( "CPbk2FetchResults::ProcessNextContactOperationL busy" ));
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::AppendContactOperationL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::AppendContactOperationL( CFRConatactOperation* aOperation )
    {
    iOperationQueue->AppendL( aOperation ); // Append to the end
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::GetCurrentContactOperation
// --------------------------------------------------------------------------
//
CPbk2FetchResults::CFRConatactOperation* CPbk2FetchResults::GetCurrentContactOperation()
    {
    if( iOperationQueue->Count() )
        return iOperationQueue->At(0); // Return first element
    else
        return NULL; // If empty, return NULL
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::RemoveCurrentContactOperation
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::RemoveCurrentContactOperation( )
    {
    if( iOperationQueue->Count() )
        {
        // Remove and delete first operation
        delete ( iOperationQueue->At( 0 ) );
        iOperationQueue->Delete( 0 );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::CFRConatactOperation::NewL
// --------------------------------------------------------------------------
//
CPbk2FetchResults::CFRConatactOperation* CPbk2FetchResults::CFRConatactOperation::NewL(
                      const MVPbkContactLink& aContactLink, const EOperationType aType )
    {
    CFRConatactOperation* self = new ( ELeave ) CFRConatactOperation( aType );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLink );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::CFRConatactOperation::CFRConatactOperation
// --------------------------------------------------------------------------
//
CPbk2FetchResults::CFRConatactOperation::CFRConatactOperation(
                                            const EOperationType aType ):
iType( aType )
    {
    // Nothing
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::CFRConatactOperation::~CFRConatactOperation
// --------------------------------------------------------------------------
//
CPbk2FetchResults::CFRConatactOperation::~CFRConatactOperation( )
    {
    delete iContactLink;
    }

// --------------------------------------------------------------------------
// CPbk2FetchResults::CFRConatactOperation::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2FetchResults::CFRConatactOperation::ConstructL( 
                                       const MVPbkContactLink& aContactLink )
    {
    // Copy ContactLink
    iContactLink = aContactLink.CloneLC();
    CleanupStack::Pop();
    }

// End of File
