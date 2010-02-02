/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 merge contacts command.
*
*/


// INCLUDE FILES

//Phonebook2
#include "CPbk2MergeContactsCmd.h"
#include "CPbk2MergeResolver.h"
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <MPbk2DialogEliminator.h>
#include <MPbk2ContactLinkIterator.h>
#include <CPbk2FetchDlg.h>
#include <CPbk2ContactViewBuilder.h>
#include <CVPbkFilteredContactView.h>
#include <CPbk2ApplicationServices.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2ContactViewSupplier.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <Pbk2StoreProperty.hrh>
#include <Pbk2ProcessDecoratorFactory.h>

#include <CPbk2MergeConflictsDlg.h>
#include <CPbk2MergePhotoConflictDlg.h>

#include <pbk2uicontrols.rsg> 
#include <pbk2cmdextres.rsg>
#include <pbk2commonui.rsg>
//Virtual Phonebook
#include <MVPbkContactLink.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldBinaryData.h>
#include <MVPbkContactFieldTextData.h>
#include <vpbkeng.rsg>
#include <MVPbkContactGroup.h>

//System
#include <aknnotewrappers.h>
#include <StringLoader.h>
//#include <aknnavide.h>
#include <akntitle.h>
#include <imageconversion.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstContact = 0;
const TInt KSecondContact = 1;

_LIT( KLocalStore, "cntdb://c:contacts.cdb" );

enum TPbk2PanicCodes
        {
        EPbk2CommandObserverMissing,
        EPbk2WronglyActivated,
        EPbk2DuplicateCallToExecuteLD,
        EPbk2ViewsAlreadyCreated,
        EPbk2WrongTypeOfData,
		EPbk2PhotoConflictError
        };
    
    void Panic(TPbk2PanicCodes aReason)
        {
        _LIT( KPanicText, "CPbk2_Merge_Contacts" );
        User::Panic(KPanicText,aReason);
        }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::CPbk2MergeContactsCmd
// --------------------------------------------------------------------------
//
CPbk2MergeContactsCmd::CPbk2MergeContactsCmd(
    MPbk2ContactUiControl& aUiControl ) :
        CActive( EPriorityStandard ),
        iNextPhase( EPhaseNone ),
        iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::~CPbk2MergeContactsCmd
// --------------------------------------------------------------------------
//
CPbk2MergeContactsCmd::~CPbk2MergeContactsCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NlxMergeContactsCmd(%x)::~CPbk2MergeContactsCmd()"), 
        this);
    
    Cancel();
    
    if ( iUiControl )
        {
        iUiControl->RegisterCommand( NULL );
        }

    CleanAfterFetching();
    delete iRetrieveOperation;
    delete iDeleteOperation;
    delete iCommitOperation;
	delete iDeleteMergedOperation;
    Release( iAppServices );
    delete iContactFirst;
    delete iContactSecond;
    delete iMergedContactLink;
    delete iStoreContactFirst;
    delete iStoreContactSecond;
    delete iMergedContact;
    delete iMergeResolver;
    delete iBitmapFirst;
    delete iBitmapSecond;
    delete iImgDecoder;
    delete iWaitDecorator;
    if( iGroupsToAdd )
        {
        iGroupsToAdd->ResetAndDestroy();
        delete iGroupsToAdd;
        }
    if ( iContactToCommit )
        {
        iContactToCommit->Reset();
        delete iContactToCommit;
        }
    if ( iContactsToDelete )
        {
        iContactsToDelete->Reset();
        delete iContactsToDelete;
        }
    if( iGroupLinksFirst )
        {
        iGroupLinksFirst->ResetAndDestroy();
        delete iGroupLinksFirst;
        }
    if( iGroupLinksSecond )
        {
        iGroupLinksSecond->ResetAndDestroy();
        delete iGroupLinksSecond;
        }
    TRAP_IGNORE( SetTitlePaneL( EFalse ) );
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2MergeContactsCmd* CPbk2MergeContactsCmd::NewL(
    MPbk2ContactUiControl& aUiControl )
    {
    CPbk2MergeContactsCmd* self = 
        new ( ELeave ) CPbk2MergeContactsCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ConstructL()
    {    
    if( iUiControl )
        {
        iUiControl->RegisterCommand( this );
        }    
    
    iContactManager = &Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager();
    iAppServices = CPbk2ApplicationServices::InstanceL();
    iPhotoConflictIndex = KErrNotFound;
    iDataFirst.Set( KNullDesC8() );
    iDataSecond.Set( KNullDesC8() );
    }
    
// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ExecuteLD()
    {
    __ASSERT_ALWAYS( iCommandObserver, Panic( EPbk2CommandObserverMissing ));	    
    __ASSERT_ALWAYS( !IsActive(), Panic( EPbk2WronglyActivated ));	
    __ASSERT_ALWAYS( iNextPhase == EPhaseNone, Panic( EPbk2DuplicateCallToExecuteLD ));
    
    if( iUiControl )
        {
        // Blank UI control to avoid flicker
        iUiControl->SetBlank( ETrue );
        }
    StartNext( EPhaseGetSelection );
    }
    
// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ResetUiControl( 
    MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::AddObserver( 
    MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }        

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::AutomaticMergeL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::AutomaticMergeL()
    {
    if(iStoreContactFirst && iStoreContactSecond)
        {
        iMergeResolver = CPbk2MergeResolver::NewL(
                CPbk2PresentationContact::NewL( *iStoreContactFirst, iAppServices->FieldProperties() ), 
                CPbk2PresentationContact::NewL( *iStoreContactSecond, iAppServices->FieldProperties() ) );
        iMergeResolver->MergeL();
        StartNext(EPhaseResolveConflicts);
        }
    else
        {
        Finish(KErrArgument);
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::AddGroupsL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::AddGroupsL()
    {
    TInt countGroups = iGroupsToAdd->Count();
    if(countGroups)
        {
        MVPbkStoreContact *group = iGroupsToAdd->At(countGroups - 1);
        group->LockL(*this);
        }
    else
        {
        DeleteSourceContactsL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::RunL()
    {
    if( !iUiControl )
        {
        //UiControl reseted, so cancel all processing
        iNextPhase = EPhaseFinish ;
        }

    switch( iNextPhase )
    	{
    	case EPhaseGetSelection:
    	    {
    	    GetContactsFromUiFetchL();    	    
    	    }
    	    break;
    	case EPhaseGetStoreContacts:
            {
            GetStoreContactsL();
            }   
            break;
    	case EPhaseMerge:
    	    {
            AutomaticMergeL();
    	    }
            break;
    	case EPhaseResolveConflicts:
            {
            ResolveConflictsL();
            }
            break;
        case EPhaseResolvePhotoConflict:
            {
            ResolvePhotoConflictL();
            }
            break;
    	case EPhaseCreateMergedContact:
        	{
        	iWaitDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
                                   ( R_QTN_MERGE_CONTACTS_WAIT_NOTE, EFalse );
        	iWaitDecorator->SetObserver( *this );
            iWaitDecorator->ProcessStartedL( 0 );
            FinalizeMergeL();
        	}
    		break;
    	case EPhaseGetGroups:
            {
            GetGroupsL();
            }
            break;
    	case EPhaseAddGroups:
            {
            AddGroupsL();
            }
            break;
    	case EPhaseFinish:
            {
            if ( iWaitDecorator )
                {
                // Decorator calls ProcessDismissed
                iWaitDecorator->ProcessStopped();
                }
            else
                {
                // In case Decorator wasn't initialized we invoke
                // ProcessDismissed to finish merge command execution
                ProcessDismissed( KErrNone );
                }
            }
            break;
            
    	default:
    		Panic( EPbk2WronglyActivated );
    		break;
    	}
    }    

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2MergeContactsCmd::RunError(TInt aError)
	{
	Finish( aError );
    return KErrNone;
	}
    
// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::FinalizeMergeL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::FinalizeMergeL()
    {
    if( !iMergeResolver ||  
            ( !iMergeResolver->CountMerged() && !iMergeResolver->CountConflicts() ) )
        {
        Finish( KErrNone );
        return;
        }
    
    MVPbkContactStore& store = iStoreContactFirst->ParentStore();
    iMergedContact = store.CreateNewContactLC();
    CleanupStack::Pop();
    
    TInt mergedFieldsCount = iMergeResolver->CountMerged();
    // add merged fields
    for ( TInt i = 0; i < mergedFieldsCount; i++ )
        {
        MVPbkStoreContactField& field = iMergeResolver->GetMergedAt( i );
        AddFieldToMergedContactL( field );
        }
    
    TInt conflictCount = iMergeResolver->CountConflicts();
    // add conflicted fields
    for ( TInt i = 0; i < conflictCount; i++ )
        {
        MPbk2MergeConflict& conflict = iMergeResolver->GetConflictAt( i );
        
        RPointerArray<MVPbkStoreContactField> fields;
        conflict.GetChosenFieldsL( fields );
        
        TInt newFields = fields.Count();
        /// if newFields == 0 not resolved conflict exist
        
        for ( TInt j = 0; j < newFields; j++ )
            {
            MVPbkStoreContactField* field = fields[ j ];
            AddFieldToMergedContactL( *field );
            }
        fields.Close();
        }
    
    iContactToCommit = new ( ELeave ) CArrayPtrFlat<MVPbkStoreContact>( 1 );
    iContactToCommit->AppendL( iMergedContact );
    iCommitOperation = iContactManager->CommitContactsL( iContactToCommit->Array(), *this );
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ShowContactsMergedNoteL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ShowContactsMergedNoteL()
    {
    HBufC* firstContactString = ContactAsStringLC( iStoreContactFirst );
    HBufC* secondContactString = ContactAsStringLC( iStoreContactSecond );
    HBufC* mergedContactString = ContactAsStringLC( iMergedContact );
    HBufC* unnamed = StringLoader::LoadLC( R_QTN_PHOB_UNNAMED );
    CDesCArrayFlat* strings = new(ELeave) CDesCArrayFlat( 3 );
    CleanupStack::PushL( strings );
    
    HBufC* prompt = NULL;
    if ( (firstContactString->Length() > 0 || secondContactString->Length() > 0)
            && mergedContactString->Length() > 0 )
        {
        if ( 0 != firstContactString->Length() )
            {
            strings->AppendL( *firstContactString );
            }
        else
            {
            strings->AppendL( *unnamed );
            }
        if ( 0 != secondContactString->Length() )
            {
            strings->AppendL( *secondContactString );
            }
        else
            {
            strings->AppendL( *unnamed );
            }
        strings->AppendL( *mergedContactString );

        prompt = StringLoader::LoadLC( R_QTN_PHOB_NOTE_CONTACTS_WERE_MERGED, *strings );
        }
    else
        {
        strings->AppendL( *unnamed );
        prompt = StringLoader::LoadLC( R_QTN_PHOB_NOTE_UNNAMED_CONTACTS_WERE_MERGED , *strings );
        }

    CAknNoteDialog* dlg = new( ELeave ) CAknNoteDialog( CAknNoteDialog::ENoTone );
    dlg->PrepareLC( R_PBK2_MERGE_CONTACTS_CONFIRMATION_NOTE );
    dlg->SetTextL( *prompt );
    dlg->RunLD();
    CleanupStack::PopAndDestroy( prompt );
    CleanupStack::PopAndDestroy( strings );
    CleanupStack::PopAndDestroy( unnamed );
    CleanupStack::PopAndDestroy( mergedContactString );
    CleanupStack::PopAndDestroy( secondContactString );
    CleanupStack::PopAndDestroy( firstContactString );
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactAsStringLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2MergeContactsCmd::ContactAsStringLC( MVPbkStoreContact* aStoreContact )
    {
    _LIT(KResultFormat, "%S %S");
    TPtrC firstName( KNullDesC() );
    TPtrC lastName( KNullDesC() );
	
    MVPbkStoreContactFieldCollection& fields = aStoreContact->Fields();
    for ( TInt i(0); i < fields.FieldCount(); ++i )
        {
        MVPbkStoreContactField& field = fields.FieldAt( i );
        if ( field.BestMatchingFieldType()->FieldTypeResId() == R_VPBK_FIELD_TYPE_FIRSTNAME )
            {
            MVPbkContactFieldData& data = field.FieldData();
            __ASSERT_DEBUG( data.DataType() == EVPbkFieldStorageTypeText, Panic( EPbk2WrongTypeOfData ) );
            MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast( data );
            firstName.Set( textData.Text() );
            }
        else if ( field.BestMatchingFieldType()->FieldTypeResId() == R_VPBK_FIELD_TYPE_LASTNAME )
            {
            MVPbkContactFieldData& data = field.FieldData();
            __ASSERT_DEBUG( data.DataType() == EVPbkFieldStorageTypeText, Panic( EPbk2WrongTypeOfData ) );
            MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast( data );
            lastName.Set( textData.Text() );        
            }
        }
    
    HBufC* result = NULL;
    if ( firstName.Length() > 0 && lastName.Length() > 0 )
        {
        result = HBufC::NewLC( firstName.Length() + lastName.Length() + KResultFormat().Length() );
        TPtr resAsDes = result->Des();
        resAsDes.Format( KResultFormat, &firstName, &lastName );
        }
    else if ( firstName.Length() > 0 )
        {
        result = firstName.AllocLC(); 
        }    
    else
        {
        result = lastName.AllocLC();
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::NotifyObservers
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::NotifyObservers()
    {
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }
    
    if ( iUiControl )
        {
        iUiControl->SetBlank( EFalse );
        iUiControl->UpdateAfterCommandExecution();
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::AcceptFetchSelectionL
// --------------------------------------------------------------------------
//
MPbk2FetchDlgObserver::TPbk2FetchAcceptSelection CPbk2MergeContactsCmd::AcceptFetchSelectionL(
    TInt /*aNumMarkedEntries*/,
    MVPbkContactLink& /*aLastSelection*/ )
    {
    return MPbk2FetchDlgObserver::EFetchYes;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::FetchCompletedL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::FetchCompletedL(
    MVPbkContactLinkArray* aMarkedEntries )
    {   
    if ( aMarkedEntries )
        {
        TInt count = aMarkedEntries->Count();
        if ( count == 2 )
            {
            iContactFirst = aMarkedEntries->At( KFirstContact ).CloneLC();
            CleanupStack::Pop();
            iContactSecond = aMarkedEntries->At( KSecondContact ).CloneLC();
            CleanupStack::Pop();
            StartNext( EPhaseGetStoreContacts );
            }
        else
            {
            // Show a note
            HBufC* prompt = NULL;
            prompt = StringLoader::LoadLC( R_QTN_PHOB_NOTE_SELECT_2_TO_MERGE );
            CAknNoteDialog* dlg = new( ELeave ) CAknNoteDialog( CAknNoteDialog::ENoTone );
            dlg->PrepareLC( R_PBK2_MERGE_CONTACTS_ERROR_NOTE );
            dlg->SetTextL( *prompt );
            dlg->RunLD();
            CleanupStack::PopAndDestroy( prompt ); 
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::FetchCanceled
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::FetchCanceled()
    {
    Finish( KErrCancel );
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::FetchAborted
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::FetchAborted()
    {
    Finish( KErrCancel );
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::FetchOkToExit
// --------------------------------------------------------------------------
//
TBool CPbk2MergeContactsCmd::FetchOkToExit()
    {
    if ( iNextPhase == EPhaseFinish )
        {
        return ETrue;
        }
    
    if ( iContactFirst && iContactSecond )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::GetContactsFromUiFetchL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::GetContactsFromUiFetchL()
    {
    __ASSERT_DEBUG( !iAllContactsView, Panic( EPbk2ViewsAlreadyCreated ));
    CPbk2FetchDlg::TParams params;
    params.iResId = R_PBK2_MULTIPLE_ENTRY_FETCH_NO_GROUPS_DLG;
    params.iCbaId = R_PBK2_SOFTKEYS_MERGE_BACK_MARK;
    params.iNaviPaneId = R_PBK2_MERGE_CONTACTS_FETCH_NAVILABEL;
    
    CPbk2StorePropertyArray& storeProperties =
        Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties();
    
    // Fetch dlg uses this view instead of AllNameslistView if it is not suitable
    iStoreUris = Phonebook2::Pbk2AppUi()->
        ApplicationServices().StoreConfiguration().CurrentConfigurationL();
    
    TBool createNewView = EFalse;
    for ( TInt i = iStoreUris->Count()-1; i >= 0; --i )
        {
        const CPbk2StoreProperty* storeProperty = 
            storeProperties.FindProperty( ( *iStoreUris )[i] );
        
        if ( storeProperty != NULL && storeProperty->StoreUri().UriDes().Compare( KLocalStore ) )
            {
            iStoreUris->Remove( (*iStoreUris)[i] );
            createNewView = ETrue;
            }
        }    
    
    if ( createNewView )
        {
        iAllContactsView = CVPbkFilteredContactView::NewL
            ( *Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().
              AllContactsViewL(), *this, *this, iContactManager->FieldTypes() );
        
        params.iNamesListView = iAllContactsView;
        iObservedView = iAllContactsView;
        }
    else{
        params.iNamesListView =
            Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().AllContactsViewL();
        iObservedView =
            Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().AllContactsViewL();
        }    

    iObservedView->AddObserverL( *this );
        
    params.iGroupsListView = NULL;
    params.iFlags = CPbk2FetchDlg::EFetchMultiple;
    params.iExitCallback = this;
    
    CPbk2FetchDlg* dlg = CPbk2FetchDlg::NewL( params, *this );
    iFetchDlgEliminator = dlg;
    iFetchDlgEliminator->ResetWhenDestroyed( &iFetchDlgEliminator );
    
    SetTitlePaneL( ETrue );
    dlg->ExecuteLD(); // Completion is signalled with a callback.
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::IsFromReadOnlyStore
// --------------------------------------------------------------------------
//
TBool CPbk2MergeContactsCmd::IsFromReadOnlyStore
        ( const MVPbkContactLink& aContactLink ) const
    {
    TBool ret = EFalse;

    const MVPbkContactStore& store = aContactLink.ContactStore();
    if ( store.StoreProperties().ReadOnly() )
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::CleanAfterFetching
// insure the fetch dlg is deleted and destroy resources used by it 
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::CleanAfterFetching()
    {
    if ( iFetchDlgEliminator )
        {
        iFetchDlgEliminator->ForceExit();
        }
    if ( iObservedView )
        {
        iObservedView->RemoveObserver( *this );
        }
    delete iAllContactsView;
    iAllContactsView = NULL;   
    delete iStoreUris;
    iStoreUris = NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2MergeContactsCmd::Finish
// ---------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::Finish( TInt aReason )
	{
	if ( aReason != KErrNone && aReason != KErrCancel )
	    {
	    CCoeEnv::Static()->HandleError( aReason );
	    }
	
	if( iNextPhase != EPhaseFinish )
		{
    	StartNext( EPhaseFinish );
		}
	}

// ---------------------------------------------------------------------------
// CPbk2MergeContactsCmd::StartNext
// ---------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::StartNext( TPhase aPhase )
    {
    __ASSERT_DEBUG( !IsActive(), Panic( EPbk2WronglyActivated ));    
    iNextPhase = aPhase;    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CPbk2MergeContactsCmd::StartNext
// ---------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::StartNext()
    {
    __ASSERT_DEBUG( !IsActive(), Panic( EPbk2WronglyActivated ));    
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::RetrieveContactL(
        const MVPbkContactLink& aContactLink )
    {
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = iAppServices->
        ContactManager().RetrieveContactL( aContactLink, *this );
    }

////////////////////////////// CALLBACKS /////////////////////////////////////

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        if( !iStoreContactFirst )
            {
            iStoreContactFirst = aContact;
            TRAP_IGNORE( RetrieveContactL( *iContactSecond ) ); 
            }
        else if( !iStoreContactSecond )
            {
            iStoreContactSecond = aContact;
            StartNext();
            }
        else if( iNextPhase == EPhaseGetGroups 
                 && aContact->Group()
                 && iGroupLinksFirst 
                 && iGroupLinksSecond
                 && iGroupsToAdd )
            {
            TBool sameGroup = EFalse;
            for( TInt idx = 0; idx < iGroupsToAdd->Count(); idx++ )
                {
                if( aContact->IsSame( *iGroupsToAdd->At( idx ) ) )
                    {
                    sameGroup = ETrue;
                    break;
                    }
                }
            TInt error = KErrNone;
            if( !sameGroup )
                {
                TRAP( error, iGroupsToAdd->AppendL( aContact ); );
                if ( error != KErrNone )
                    {
                    delete aContact;
                    DeleteMergedContact();
                    return;
                    }
                }
            else
                {
                delete aContact;
                }
            
            TInt countFirst = iGroupLinksFirst->Count();
            TInt countSecond = iGroupLinksSecond->Count();
            
            if( countFirst )
                {
                delete &( iGroupLinksFirst->At( countFirst - 1 ) );
                iGroupLinksFirst->Remove( countFirst - 1 );
                }
            else if( countSecond )
                {
                delete &( iGroupLinksSecond->At( countSecond - 1 ) );
                iGroupLinksSecond->Remove( countSecond - 1 );
                }
            
            countFirst = iGroupLinksFirst->Count();
            countSecond = iGroupLinksSecond->Count();
            
            if( countFirst )
                {
                TRAP( error, RetrieveContactL( iGroupLinksFirst->At( countFirst - 1 ) ); );
                }
            else if( countSecond )
                {
                TRAP( error, RetrieveContactL( iGroupLinksSecond->At( countSecond - 1 ) ); );
                }
            else
                {
                StartNext( EPhaseAddGroups );
                }
            if ( error != KErrNone )
                {
                delete aContact;
                DeleteMergedContact();
                }
            }
        else
            {
            delete aContact;
            DeleteMergedContact();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    if ( &aOperation == iRetrieveOperation )
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;
        
        Finish( aError );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ContactOperationCompleted(
        TContactOpResult aResult )
    {
    if( aResult.iOpCode == MVPbkContactObserver::EContactLock )
        {
        TInt countGroups = iGroupsToAdd->Count();
        if( countGroups )
            {
            MVPbkStoreContact* group = iGroupsToAdd->At( countGroups - 1 );
            TRAPD( error, 
                group->Group()->AddContactL( *iMergedContactLink );
                group->CommitL( *this );
            );
            if ( error != KErrNone )
                {
                DeleteMergedContact();
                }
            }
        }
    else if( aResult.iOpCode == MVPbkContactObserver::EContactCommit) 
        {
        TInt countGroups = iGroupsToAdd->Count();
        if( countGroups )
            {
            MVPbkStoreContact* group = iGroupsToAdd->At( countGroups - 1 );
            delete group;
            iGroupsToAdd->Delete( countGroups - 1 );
            }
        countGroups = iGroupsToAdd->Count();
        if( countGroups )
            {
            MVPbkStoreContact* group = iGroupsToAdd->At( countGroups - 1 );
            TRAPD( error, 
                group->LockL( *this );
            );
           if ( error != KErrNone )
               {
               DeleteMergedContact();
               }
            }
        else
            {
            TRAPD( error, 
                DeleteSourceContactsL();
            );
           if ( error != KErrNone )
               {
               DeleteMergedContact();
               }
            }
        }
    delete aResult.iStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ContactOperationFailed(
        TContactOp /*aOpCode*/, TInt /*aErrorCode*/, TBool /*aErrorNotified*/ )
    {
    DeleteMergedContact();
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ProcessDismissed
        ( TInt /*aCancelCode*/ )
    {
    if( iMergedContact )
        {
        TRAP_IGNORE( ShowContactsMergedNoteL() );
        }
    NotifyObservers();
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& aContactLink )
    {
    if ( iMergedContact && iNextPhase == EPhaseCreateMergedContact )
        {
        TRAPD( error,
            if( !iMergedContactLink  )
                {
                iMergedContactLink = iMergedContact->CreateLinkLC();
                if ( iMergedContactLink )
                    {
                    CleanupStack::Pop();
                    }
                else
                    {
                    User::Leave( KErrGeneral );
                    }
                }
            if ( iMergedContactLink && iMergedContactLink->IsSame( aContactLink ) )
                {
                StartNext( EPhaseGetGroups );
                }
        );
        if ( error != KErrNone )
            {
            Finish( error );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt /*aError*/,
          TBool /*aErrorNotified*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2MergeContactsCmd::OkToExitL( TInt /*aCommandId*/ )
    {
    return ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::IsContactIncluded
// --------------------------------------------------------------------------
//
TBool CPbk2MergeContactsCmd::IsContactIncluded
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

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::StepComplete
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::StepComplete( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/ )
    {
    
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::StepFailed
// --------------------------------------------------------------------------
//
TBool CPbk2MergeContactsCmd::StepFailed( MVPbkContactOperationBase& aOperation, 
                                         TInt /*aStepSize*/, TInt aError )
    {
    if ( &aOperation == iCommitOperation )
        {
        delete iCommitOperation;
        iCommitOperation = NULL;
        iContactToCommit->Reset();
        delete iContactToCommit;
        iContactToCommit = NULL;
        Finish( aError );
        }
    else if ( &aOperation == iDeleteOperation )
        {
        delete iDeleteOperation;
        iDeleteOperation = NULL;
        // Merged contact must be deleted
        DeleteMergedContact();
        }
    else if ( &aOperation == iDeleteMergedOperation )
        {
        delete iDeleteMergedOperation;
        iDeleteMergedOperation = NULL;
        Finish( aError );
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::perationComplete
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::OperationComplete( MVPbkContactOperationBase& aOperation )
    {
    if ( &aOperation == iCommitOperation )
        {
        delete iCommitOperation;
        iCommitOperation = NULL;
        iContactToCommit->Reset();
        delete iContactToCommit;
        iContactToCommit = NULL;
        }
    else if ( &aOperation == iDeleteOperation )
        {
        delete iDeleteOperation;
        iDeleteOperation = NULL;
        Finish( KErrNone );
        }
    else if ( &aOperation == iDeleteMergedOperation )
        {
        delete iDeleteMergedOperation;
        iDeleteMergedOperation = NULL;
        delete iMergedContact;
        iMergedContact = NULL;
        Finish( KErrInUse );
        }
    }


// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::SetTitlePaneL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::SetTitlePaneL( TBool aCustom )
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities( TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
            ( statusPane->ControlL ( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        
        if ( aCustom )
            {
            HBufC* title = StringLoader::LoadLC( R_QTN_PHOB_TITLE_MERGE_CONTACTS );
            titlePane->SetTextL( *title );
            CleanupStack::PopAndDestroy( title );  
            }
        else
            {
            titlePane->SetTextToDefaultL();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::CheckPhotoConflictL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::CheckPhotoConflictL()
    {
    iImageTypeConflictsCount = 0;
    iPhotoConflictIndex = KErrNotFound;
    TInt conflictsCount = iMergeResolver->CountConflicts();
    for ( TInt i = 0; i < conflictsCount; i++ )
        {
        MPbk2MergeConflict& conflict = iMergeResolver->GetConflictAt( i );
        TInt type = conflict.GetConflictType();
        if ( type == EPbk2ConflictTypeImage )
            {
            iImageTypeConflictsCount++;
            CPbk2MergeConflict& conflict =
                ( CPbk2MergeConflict& ) iMergeResolver->GetConflictAt( i );
            const MVPbkStoreContactField* firstField;
            const MVPbkStoreContactField* secondField;
            conflict.GetFieldsL( firstField, secondField );
            
            const MVPbkFieldType* fieldType = firstField->BestMatchingFieldType();
            TArray<TVPbkFieldVersitProperty> versitPropArr = fieldType->VersitProperties();
            TInt count = versitPropArr.Count();
        
            for( TInt idx = 0; idx < count; idx++ )
                {
                TVPbkFieldVersitProperty versitProp = versitPropArr[idx];
                if( versitProp.Name() == EVPbkVersitNamePHOTO )
                    {
                    if ( firstField->FieldData().DataType() == EVPbkFieldStorageTypeBinary && 
                            secondField->FieldData().DataType() == EVPbkFieldStorageTypeBinary )
                        {
                        iDataFirst.Set( MVPbkContactFieldBinaryData::Cast
                            ( firstField->FieldData() ).BinaryData() );
                        iDataSecond.Set( MVPbkContactFieldBinaryData::Cast
                            ( secondField->FieldData() ).BinaryData() );
                        
                        iPhotoConflictIndex = i;
                        break;
                        }
                    }
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ResolveAllPhotoConflicts
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ResolveAllPhotoConflicts( 
        EPbk2ConflictedNumber aConflictResolutionNumber )
    {
    TInt conflictsCount = iMergeResolver->CountConflicts();
    for ( TInt i = 0; i < conflictsCount; i++ )
        {
        MPbk2MergeConflict& conflict = iMergeResolver->GetConflictAt( i );
        if ( conflict.GetConflictType() == EPbk2ConflictTypeImage )
            {
            conflict.ResolveConflict( aConflictResolutionNumber );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::ResolvePhotoConflictL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ResolvePhotoConflictL()
    {
    if ( iStatus != KErrNone || iPhotoConflictIndex == KErrNotFound )
        {
        StartNext( EPhaseCreateMergedContact );
        return;
        }
    
    if ( iDataFirst.Length() == 0 || iDataSecond.Length() == 0 )
        {
        StartNext( EPhaseFinish );
        return;
        }
    
    if ( !iBitmapFirst )
        {
        InitBitmapL( iBitmapFirst, iDataFirst );
        return;
        }
    if ( !iBitmapSecond )
        {
        InitBitmapL( iBitmapSecond, iDataSecond  );
        return;
        }
    
    delete iImgDecoder;
    iImgDecoder = NULL;
    TInt result = EPbk2ConflictedFirst;
    
    CPbk2MergePhotoConflictDlg* dlg = 
        CPbk2MergePhotoConflictDlg::NewL( iBitmapFirst, iBitmapSecond, &result );
    if ( !dlg->ExecuteLD( R_PBK2_MERGE_CONTACTS_PHOTO_CONFLICT_RESOLUTION_DLG ) )
        {
        // dlg returns 0 if canceled
        Finish( KErrNone );
        }
    else
        {
        ResolveAllPhotoConflicts( (EPbk2ConflictedNumber) result );
        StartNext( EPhaseCreateMergedContact );
        }
    delete iBitmapFirst;
    iBitmapFirst = NULL;
    delete iBitmapSecond;
    iBitmapSecond = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::InitBitmapL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::InitBitmapL( CFbsBitmap*& aBitmap, TDesC8& aData )
    {
    delete iImgDecoder;
    iImgDecoder = NULL;
    iImgDecoder = CImageDecoder::DataNewL
        ( iContactManager->FsSession(), aData, CImageDecoder::EOptionAlwaysThread );
    
    TFrameInfo info = iImgDecoder->FrameInfo();
    aBitmap = new ( ELeave ) CFbsBitmap;
    User::LeaveIfError( aBitmap->Create
        ( info.iOverallSizeInPixels, info.iFrameDisplayMode ));
    
    iImgDecoder->Convert( &iStatus, *aBitmap );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::AddFieldToMergedContactL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::AddFieldToMergedContactL( MVPbkStoreContactField& field )
    {
    const MVPbkFieldType* type = field.BestMatchingFieldType();
    if ( type )
        {
        MVPbkStoreContactField* newField = iMergedContact->CreateFieldLC( *type );
        newField->SetFieldLabelL( field.FieldLabel() );
        newField->FieldData().CopyL( field.FieldData() );
        iMergedContact->AddFieldL( newField );
        CleanupStack::Pop( newField );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::DoResolveConflictsL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::ResolveConflictsL()
    {
    CheckPhotoConflictL();
    
    TBool isPhotoConflict = iPhotoConflictIndex != KErrNotFound;
    TInt normalConflicts = 0;
    if( iMergeResolver )
        {
        normalConflicts = iMergeResolver->CountConflicts() - iImageTypeConflictsCount;
        }
    
    if( normalConflicts )
        {
        // resolve conflicts
        CPbk2MergeConflictsDlg* dlg = CPbk2MergeConflictsDlg::NewL( iMergeResolver, isPhotoConflict );
        if ( !dlg->ExecuteLD( R_PBK2_MERGE_CONTACTS_CONFLICT_RESOLUTION_DLG ) )
            {
            // dlg returns 0 if canceled
            Finish( KErrCancel );
            }
        else
            {
            StartNext( EPhaseResolvePhotoConflict );
            }
        }
    else
        {
        StartNext( EPhaseResolvePhotoConflict );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::GetStoreContactsL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::GetStoreContactsL()
    {
    if( iContactFirst && iContactSecond )
        {
        if( !iStoreContactFirst )
            {
            RetrieveContactL( *iContactFirst );
            }
        else if( iStoreContactSecond )
            {
            StartNext( EPhaseMerge );
            }
        else
            {
            Finish( KErrArgument );
            }
        }
    else
        {
        Finish( KErrArgument );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::DeleteSourceContactsL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::DeleteSourceContactsL()
    {
    User::LeaveIfNull( iContactFirst );
    User::LeaveIfNull( iContactSecond );
    if( iContactsToDelete )
        {
        delete iContactsToDelete;
        iContactsToDelete = NULL;
        }
    iContactsToDelete = CVPbkContactLinkArray::NewL();
    iContactsToDelete->AppendL( iContactSecond );
    iContactsToDelete->AppendL( iContactFirst );
    if( iDeleteOperation )
        {
        delete iDeleteOperation;
        iDeleteOperation = NULL;
        }
    iDeleteOperation = iContactManager->DeleteContactsL( *iContactsToDelete, *this );
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::DeleteMergedContact
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::DeleteMergedContact()
    {
    TRAPD( error, 
            DeleteMergedContactL();
    );
    if( error != KErrNone )
        {
        Finish( KErrGeneral );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::DeleteMergedContactL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::DeleteMergedContactL()
    {
    if ( iContactsToDelete )
        {
        iContactsToDelete->Reset();
        }
    else
        {
        iContactsToDelete = CVPbkContactLinkArray::NewL();
        }
    
    if ( iMergedContact )
        {
        if( !iMergedContactLink  )
            {
            MVPbkContactLink* iMergedContactLink = iMergedContact->CreateLinkLC();
            if ( iMergedContactLink )
                {
                CleanupStack::Pop();
                }
            else
                {
                User::Leave( KErrGeneral );
                }
            }
        iContactsToDelete->AppendL( iMergedContactLink );
        iDeleteMergedOperation = iContactManager->DeleteContactsL( *iContactsToDelete, *this );
        }
    else
        {
        User::Leave( KErrGeneral );
        }
    }

// --------------------------------------------------------------------------
// CPbk2MergeContactsCmd::GetGroupsL
// --------------------------------------------------------------------------
//
void CPbk2MergeContactsCmd::GetGroupsL()
    {
    if( iStoreContactFirst && iStoreContactFirst && iMergedContact )
        {
        if( !iGroupsToAdd )
            {
            iGroupsToAdd = new ( ELeave ) CArrayPtrFlat<MVPbkStoreContact>( 1 );
            }
        iGroupLinksFirst = static_cast<CVPbkContactLinkArray*>( iStoreContactFirst->GroupsJoinedLC() );
        CleanupStack::Pop(); 
        iGroupLinksSecond = static_cast<CVPbkContactLinkArray*>( iStoreContactSecond->GroupsJoinedLC() );
        CleanupStack::Pop(); 
        
        TInt countFirst = iGroupLinksFirst->Count();
        TInt countSecond = iGroupLinksSecond->Count();
        
        if( countFirst )
            {
            RetrieveContactL( iGroupLinksFirst->At( countFirst - 1 ) );
            }
        else if( countSecond )
            {
            RetrieveContactL( iGroupLinksSecond->At( countSecond - 1 ) );
            }
        else
            {
            DeleteSourceContactsL();
            }
        }
    else
        {
        DeleteMergedContactL();
        }
    }
//  End of File
