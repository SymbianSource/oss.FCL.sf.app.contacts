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
* Description:  Phonebook 2 add to top contacts launcher command
*
*/


// INCLUDE FILES
#include "CPbk2NlxAddToTopContactsCmd.h"

#include <StringLoader.h>
#include <aknnotewrappers.h>
#include <Pbk2UIControls.rsg> 
#include <Pbk2CommonUi.rsg>
#include <Pbk2CmdExtRes.rsg>
#include <Pbk2Commands.rsg>

#include <Pbk2ProcessDecoratorFactory.h>
#include <CPbk2NamesListControl.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ApplicationServices.h>
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <Pbk2ExNamesListRes.rsg>
#include <Phonebook2PrivateCRKeys.h>
#include <centralrepository.h>
#include <CVPbkTopContactManager.h>
#include <CPbk2FetchDlg.h>
#include <MPbk2ContactViewSupplier.h>
#include <CPbk2ContactViewBuilder.h>
#include <MVPbkContactViewBase.h>
#include <CPbk2StoreConfiguration.h>
#include <CVPbkContactStoreUriArray.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <Pbk2StoreProperty.hrh>
#include <CVPbkFilteredContactView.h>
#include <CPbk2ContactRelocator.h>
#include "TPbk2ContactIterator.h"

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

    enum TNlxPanicCodes
        {
        ENlxAtcCommandObserverMissing,
        ENlxAtcSingleContactRelocation,
        ENlxAtcSingleContactRelocation2,
        ENlxAtcSingleTopContactOperation,
        ENlxAtcWronglyActivated,
        ENlxAtcDuplicateCallToExecuteLD,
        ENlxUnexpectedFailurePath,
        ENlxNoHighlightedContact,
        ENlxViewsAlreadyCreated,
        ENlxMergePrecond,
        ENlxRemoveSimPrecond,
        ENlxNoteActive
        };
    
    void Panic(TNlxPanicCodes aReason)
        {
        _LIT( KPanicText, "CPbk2_nlx_AddToTC" );
        User::Panic(KPanicText,aReason);
        }

    CVPbkContactLinkArray* CloneL(
        const MVPbkContactLinkArray& aArray )
        {
        CVPbkContactLinkArray* newArray = CVPbkContactLinkArray::NewLC();
        const TInt count = aArray.Count();
        for ( TInt n = 0; n < count; ++n )
            {
            MVPbkContactLink* link = aArray.At(n).CloneLC();
            newArray->AppendL( link );
            CleanupStack::Pop(); // link
            }
        CleanupStack::Pop( newArray );
        return newArray;
        }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::CPbk2NlxAddToTopContactsCmd
// --------------------------------------------------------------------------
//
CPbk2NlxAddToTopContactsCmd::CPbk2NlxAddToTopContactsCmd(
    MPbk2ContactUiControl& aUiControl,
    TBool aAddFavourites ) :
        CActive( EPriorityStandard ),
        iNextPhase( EPhaseNone ),
        iUiControl( &aUiControl ),
        iAddFavourites( aAddFavourites )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::~CPbk2NlxAddToTopContactsCmd
// --------------------------------------------------------------------------
//
CPbk2NlxAddToTopContactsCmd::~CPbk2NlxAddToTopContactsCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NlxAddToTopContactsCmd(%x)::~CPbk2NlxAddToTopContactsCmd()"), 
        this);
    
    Cancel();
    if (iUiControl)
        {
        iUiControl->RegisterCommand( NULL );
        }

    CleanAfterFetching();
    delete iRelocatedContacts;
    delete iMarkedEntries;
    delete iContactIterator;
    delete iContactOperation; //Cancels request if pending
    delete iDecorator;
    delete iVPbkTopContactManager;
    delete iContactRelocator;
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2NlxAddToTopContactsCmd* CPbk2NlxAddToTopContactsCmd::NewL(
    MPbk2ContactUiControl& aUiControl, TBool aAddFavourites )
    {
    CPbk2NlxAddToTopContactsCmd* self = 
        new ( ELeave ) CPbk2NlxAddToTopContactsCmd(
        aUiControl, aAddFavourites );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ConstructL()
    {    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }    
    
    iContactManager =
        &Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager();
    iVPbkTopContactManager = CVPbkTopContactManager::NewL( *iContactManager );
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ExecuteLD()
    {
    __ASSERT_ALWAYS( iCommandObserver, Panic( ENlxAtcCommandObserverMissing ));	    
    __ASSERT_ALWAYS( !IsActive(), Panic( ENlxAtcWronglyActivated ));	
    __ASSERT_ALWAYS( iNextPhase == EPhaseNone, Panic( ENlxAtcDuplicateCallToExecuteLD ));	

    if( iUiControl )
        {
        iUiControl->SetBlank( ETrue );
        } 

    if ( iAddFavourites )
        {
        // The user has started the "Add favorites" namesList command.
        // Launch Fetch dialog asynchronously.
        GetSelectionWithFetchUiL();
        // The Fetch dialog will inform fetch completion with a callback.
        }
    else
        {
        // The user has selected "Set as favorite" from options menu.
        GetUiControlSelectionL();
        }
    
    //This can be deleted only when callback of operation is done.
    //Ownership of this object is in caller (i.e. CPbk2CommandActivator)
    //No push and pop to cleanupstack in this function must be done.
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ResetUiControl( 
    MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::AddObserver( 
    MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }        


// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::RunL()
    {
    if( !iUiControl )
        {
        //UiControl reseted, so cancel all processing
        iNextPhase = EFinish ;
        }

    switch( iNextPhase )
    	{
    	case ERelocate:
    	    {
    	    CleanAfterFetching();
            const TInt numberOfSimContacts = CountSimContacts();
    	    if ( numberOfSimContacts > 0 )
                {
                StartSimContactsRelocationL();
                iNextPhase = ESetAsTopContact;
                }
            else
                {
                StartNext(ESetAsTopContact);
                }
    	    ShowWaitNoteL();
    	    }
    	    break;
        case ESetAsTopContact:
            {
            // prepare iMarkedEntries
            ReplaceSimContactsL();
            // set as top
            iContactOperation = iVPbkTopContactManager->AddToTopL(
                    *iMarkedEntries,
                    *this,
                    *this );
            }
            break;
    	case EFinish:
        	{
            if (iUiControl)
                {
                iUiControl->SetBlank(EFalse);
                iUiControl->UpdateAfterCommandExecution();
                }
            if ( iCommandObserver )
                {
                iCommandObserver->CommandFinished( *this );
                }
        	}
    		break;
    	default:
    		Panic( ENlxAtcWronglyActivated );
    		break;
    	}
    }    

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2NlxAddToTopContactsCmd::RunError(TInt aError)
	{
	Finish( aError );
    return KErrNone;
	}
    
// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::DoCancel()
    {
    }

MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection CPbk2NlxAddToTopContactsCmd::AcceptFetchSelectionL(
    TInt /*aNumMarkedEntries*/,
    MVPbkContactLink& /*aLastSelection*/ )
    {
    return MPbk2FetchDlgObserver::EFetchYes;
    }

void CPbk2NlxAddToTopContactsCmd::FetchCompletedL(
    MVPbkContactLinkArray* aMarkedEntries )
    {
    // Don't take ownership of aMarkedEntries!
    
    // If nothing was marked, then aMarkedEntries can be NULL
    if ( aMarkedEntries )
        {
        // Yes, something was selected by user.
        iMarkedEntries = CloneL( *aMarkedEntries );
        TPbk2ContactIterator* iterator = new (ELeave) TPbk2ContactIterator;
        iterator->SetLinkArray( *iMarkedEntries );
        delete iContactIterator;
        iContactIterator = iterator; // takes ownership
        
        StartNext(); // Goes to RunL()
        }
    else
        {
        // Nothing was selected by user.
        Finish( KErrNone );
        }
    }

void CPbk2NlxAddToTopContactsCmd::FetchCanceled()
    {
    Finish( KErrCancel );
    }

void CPbk2NlxAddToTopContactsCmd::FetchAborted()
    {
    Finish( KErrCancel );
    }

TBool CPbk2NlxAddToTopContactsCmd::FetchOkToExit()
    {
    return ETrue;
    }

void CPbk2NlxAddToTopContactsCmd::GetSelectionWithFetchUiL()
    {
    __ASSERT_DEBUG( !iNonEmptyGroupsView, Panic( ENlxViewsAlreadyCreated ));
    __ASSERT_DEBUG( !iAllContactsView, Panic( ENlxViewsAlreadyCreated ));
    
    CPbk2FetchDlg::TParams params;
    params.iResId = R_PBK2_MULTIPLE_ENTRY_FETCH_DLG;
    
    CPbk2StorePropertyArray& storeProperties =
        Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties();
    CPbk2ContactViewBuilder* viewBuilder = CPbk2ContactViewBuilder::NewLC
            ( *iContactManager,
              storeProperties );    
    
    // Fetch dlg uses this view instead of AllNameslistView
    iStoreUris = 
            Phonebook2::Pbk2AppUi()->ApplicationServices().StoreConfiguration().CurrentConfigurationL();
    TBool createNewView = EFalse;
    for ( TInt i = iStoreUris->Count()-1; i >= 0; --i )
        {
        const CPbk2StoreProperty* storeProperty =
            storeProperties.FindProperty( (*iStoreUris)[i] );
        if ( !( storeProperty->Flags() & KPbk2DefaultSavingStorageItem ) )
            {
            // Remove read only URI from the array
            iStoreUris->Remove( (*iStoreUris)[i] );
            createNewView = ETrue;
            }
        }    
    
    if ( createNewView )
        {
        iAllContactsView = CVPbkFilteredContactView::NewL
            ( *Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().
              AllContactsViewL(), *this, *this,
              iContactManager->FieldTypes() );
        
        params.iNamesListView = iAllContactsView;
        }
    else
        {
        params.iNamesListView =
            Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().AllContactsViewL();
        }    

    // create group view without empty groups

    //use ready sort order from normal AllGroups view
    const MVPbkFieldTypeList& sortOrder = 
    Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().AllGroupsViewL()->SortOrder();
    iNonEmptyGroupsView = viewBuilder->CreateGroupViewForStoresLC
                ( *iStoreUris, *this, sortOrder,
                  NULL, // no view filtering
                  EVPbkExcludeEmptyGroups );
    CleanupStack::Pop(); // iNonEmptyGroupsView
    
    CleanupStack::PopAndDestroy( viewBuilder );
    //CleanupStack::PopAndDestroy( storeUris );
        
    params.iGroupsListView = iNonEmptyGroupsView;
    params.iFlags = CPbk2FetchDlg::EFetchMultiple;

    CPbk2FetchDlg* dlg = CPbk2FetchDlg::NewL( params, *this );
    iFetchDlgEliminator = dlg;
    iFetchDlgEliminator->ResetWhenDestroyed( &iFetchDlgEliminator );
    
    // The ExecuteLD is asyncrhonous.
    dlg->ExecuteLD(); // Completion is signalled with a callback.
    
    iNextPhase = ERelocate;
    }

TBool CPbk2NlxAddToTopContactsCmd::IsContactIncluded
        ( const MVPbkBaseContact& aContact )
    {
    for ( TInt i = iStoreUris->Count()-1; i >= 0; --i )
        {
        if ( aContact.MatchContactStore((*iStoreUris)[i].UriDes()) )
            {
            return ETrue;
            }
        }  

    return EFalse;
    }

// insure the fetch dlg is deleted and destroy resources used by it 
void CPbk2NlxAddToTopContactsCmd::CleanAfterFetching()
    {
    if ( iFetchDlgEliminator )
        {
        iFetchDlgEliminator->ForceExit();
        }
    delete iNonEmptyGroupsView;
    iNonEmptyGroupsView = NULL;
    delete iAllContactsView;
    iAllContactsView = NULL;   
    delete iStoreUris;
    iStoreUris = NULL;
    }

void CPbk2NlxAddToTopContactsCmd::GetUiControlSelectionL()
    {
    // First delete any previous iterator.
    if ( iUiControl )
        {
        delete iContactIterator;
		iContactIterator = NULL;

        // Take the selection from the UI control.
        const MVPbkBaseContact* contact = NULL;
	    iContactIterator = iUiControl->SelectedContactsIteratorL();
	    if ( !iContactIterator )
	        {
	        // Nothing was marked. Use the highlighted contact.
	        delete iMarkedEntries;
	        iMarkedEntries = NULL;
	        contact = iUiControl->FocusedContactL();
	        if (contact)
	            {
	            MVPbkContactLink* link = contact->CreateLinkLC();
	            CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewL();
	            iMarkedEntries = array; // take ownership here
	            array->AppendL( link );
	            CleanupStack::Pop(); // link
	            // Create an iterator for the link array.
	            TPbk2ContactIterator* iterator = new (ELeave) TPbk2ContactIterator;
	            iterator->SetLinkArray( *iMarkedEntries );
	            iContactIterator = iterator;
	            }
	        }
		if (contact)
			{
			StartNext(ERelocate);
			}
		else
			{
			Finish(KErrNone);
			}
        }
    }

TInt CPbk2NlxAddToTopContactsCmd::CountSimContacts() const
    {
    TInt result = 0;
    
    const TInt markedCount = iMarkedEntries->Count();
    for (TInt i = 0; i < markedCount; ++i)
        {
        if ( !iMarkedEntries->At(i).ContactStore().StoreProperties().Uri().Compare(
                VPbkContactStoreUris::SimGlobalAdnUri(),
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents))
            {
            ++result;
            }
        }

    return result;
    }

CVPbkContactLinkArray* CPbk2NlxAddToTopContactsCmd::CreateSimContactsArrayLC() const
    {
    CVPbkContactLinkArray* result = CVPbkContactLinkArray::NewLC();
    iContactIterator->SetToFirst();
    while( iContactIterator->HasNext() )
        {
        MVPbkContactLink* link = iContactIterator->NextL();
        CleanupDeletePushL( link );
        
        if ( !link->ContactStore().StoreProperties().Uri().Compare(
                VPbkContactStoreUris::SimGlobalAdnUri(),
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents))
            {
            result->AppendL(link);
            CleanupStack::Pop(); // link
            }
        else
            {
            CleanupStack::PopAndDestroy(); // link
            }
        }
    
    return result;
    }

// Replace or remove sim contact links from iMarkedEntries
void CPbk2NlxAddToTopContactsCmd::ReplaceSimContactsL()
    {
    __ASSERT_DEBUG( iMarkedEntries, Panic(ENlxMergePrecond) );
    
    for (TInt i = iMarkedEntries->Count()-1; i >= 0; --i)
        {
        if ( !iMarkedEntries->At(i).ContactStore().StoreProperties().Uri().Compare(
                VPbkContactStoreUris::SimGlobalAdnUri(),
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents))
            {
            // replace link with relocated contact link if there are any,
            // otherwise just delete it
            iMarkedEntries->Delete(i);
            if ( iRelocatedContacts && iRelocatedContacts->Count() > 0 )
                {
                const TInt lastLinkIndex = iRelocatedContacts->Count()-1;
                MVPbkContactLink* link = iRelocatedContacts->At(lastLinkIndex).CloneLC();
                iMarkedEntries->InsertL( link, i );
                CleanupStack::Pop();//link
                iRelocatedContacts->Delete(lastLinkIndex);
                }
            }
        }
    }

void CPbk2NlxAddToTopContactsCmd::StartSimContactsRelocationL()
    {
    if( !iContactRelocator )
        {
        iContactRelocator = CPbk2ContactRelocator::NewL();
        }
    CVPbkContactLinkArray* simContacts = CreateSimContactsArrayLC();
    iContactRelocator->RelocateContactsL(
            simContacts,
            *this,
            Pbk2ContactRelocator::EPbk2DisplayNoQueries );
    //Relocator took ownership
    CleanupStack::Pop(); //simContacts
    }

// ---------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::Finish
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::Finish( TInt aReason )
	{
	if ( aReason != KErrNone && aReason != KErrCancel )
	    {
	    CCoeEnv::Static()->HandleError( aReason );
	    }
	
    if ( iDecorator )
        {
        // wait for callback from the wait note and finish then
        iDecorator->ProcessStopped();
        }
    else
        {
        StartNext( EFinish );
        }
	}

// ---------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::StartNext
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::StartNext( TPhase aPhase )
    {
    __ASSERT_DEBUG( !IsActive(), Panic( ENlxAtcWronglyActivated ));    
    iNextPhase = aPhase;    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbk2NlxAddToTopContactsCmd::StartNext()
    {
    __ASSERT_DEBUG( !IsActive(), Panic( ENlxAtcWronglyActivated ));    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

void CPbk2NlxAddToTopContactsCmd::ShowWaitNoteL()
    {
    __ASSERT_DEBUG( !iDecorator, Panic( ENlxNoteActive ));
    iDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
        ( R_QTN_GEN_NOTE_SAVING_WAIT, ETrue );
    iDecorator->SetObserver( *this );
    iDecorator->ProcessStartedL( 0 ); // wait note doesn't care about amount
    }

void CPbk2NlxAddToTopContactsCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    StartNext( EFinish );
    }

////////////////////////////// CALLBACKS /////////////////////////////////////

// ---------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactRelocatedL
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactRelocatedL( 
	MVPbkStoreContact* aRelocatedContact )
	{
    __ASSERT_DEBUG( iNextPhase == ESetAsTopContact, 
    	Panic( ENlxAtcSingleContactRelocation ));    

    if ( !iRelocatedContacts )
        {
        iRelocatedContacts = CVPbkContactLinkArray::NewL();
        }
    
    MVPbkContactLink* link = aRelocatedContact->CreateLinkLC();
    iRelocatedContacts->AppendL( link );
    CleanupStack::Pop(); //link
    
    // top contact manager needs links, delete store contacts
    delete aRelocatedContact; 
	}

// ---------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactRelocationFailed
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactRelocationFailed(
    TInt /* aReason */,
    MVPbkStoreContact* /* aContact */ )
	{
    if ( iDecorator )
        {
        // wait for callback from the wait note and finish then
        iDecorator->ProcessStopped();
        }
    else
        {
        StartNext( EFinish );
        }
	}

// ---------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactsRelocationFailed
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactsRelocationFailed(
        TInt aReason,
        CVPbkContactLinkArray* aContacts )
	{
	delete aContacts;
	CCoeEnv::Static()->HandleError( aReason );
	}


// ---------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::RelocationProcessComplete
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::RelocationProcessComplete()
	{
    __ASSERT_DEBUG( iNextPhase == ESetAsTopContact,
        Panic( ENlxAtcSingleContactRelocation2 ));
    
    if ( iRelocatedContacts && iRelocatedContacts->Count() > 0 )
        {
        // All is OK.
        StartNext();
        }
    else
        {
        // Some error has occured. The error has already been handled
        // in ContactRelocationFailed().
        Finish( KErrNone );
        }
	}

// ---------------------------------------------------------------------------    
// CPbk2NlxAddToTopContactsCmd::TopOperationCompleted
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::VPbkOperationCompleted(
    MVPbkContactOperationBase*)
    {
    // This is a callback for CVPbkTopContactManager::AddToTopL()
    __ASSERT_DEBUG( iNextPhase == ESetAsTopContact, 
        	Panic( ENlxAtcSingleTopContactOperation ));
    Finish( KErrNone );
    }

// ---------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::TopErrorOccured
// ---------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::VPbkOperationFailed(
    MVPbkContactOperationBase*,TInt aError )
    {
    __ASSERT_DEBUG( iNextPhase == ESetAsTopContact, 
        Panic( ENlxAtcSingleTopContactOperation ));
    Finish( aError );
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2NlxAddToTopContactsCmd::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2NlxAddToTopContactsCmd::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt /*aError*/,
          TBool /*aErrorNotified*/ )
    {
    // Do nothing
    }

//  End of File
