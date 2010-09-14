/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 copy all SIM contacts to phone memory.
*
* NOTE: Contains a lot of similar code that in CPsu2CopySimContactsCmd.
* Reason is that there is no clear place to share common SIM related code between 
* USIMThinExtension.dll and USIMExtension.dll. This is a potential place for 
* future refactoring. 
*/


// INCLUDE FILES
#include "CPsu2CopyAllToPbkCmd.h"

// Phonebook 2
#include <MPbk2CommandObserver.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StoreViewDefinition.h>
#include <CPbk2SortOrderManager.h>
#include <Pbk2ProcessDecoratorFactory.h>
#include <Pbk2UID.h>
/// Use R_PBK2_GENERAL_CONFIRMATION_QUERY from UI controls
/// As a core components UiControl's resource file is loaded in core Pbk2
#include <pbk2uicontrols.rsg>
#include <pbk2usimuires.rsg>
/// Use R_GENERAL_COPY_PROGRESS_NOTE from Commands
/// As a core components Command's resource file is loaded in core Pbk2
#include <pbk2commands.rsg>
#include <Pbk2DataCaging.hrh>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2StoreValidityInformer.h>

// Virtual Phonebook
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>

#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactView.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactCopier.h>

#include <TPsu2SimCopyResults.h>
#include <MPbk2ContactNameFormatter.h>

// System includes
#include <AknQueryDialog.h>
#include <e32property.h>
#include <StringLoader.h>
#include <e32cmn.h>
#include <e32capability.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {


const TInt KTargetStoresCount = 1;

#ifdef _DEBUG
enum TPanicCode
    {
    EStorePropertyNotFound_CreateSimViewsL
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPsu2CopyAllToPbkCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG


} /// namespace


// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::CPsu2CopyAllToPbkCmd
// --------------------------------------------------------------------------
//
CPsu2CopyAllToPbkCmd::CPsu2CopyAllToPbkCmd() 
    : CActive( EPriorityStandard ) 
    {
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::~CPsu2CopyAllToPbkCmd
// --------------------------------------------------------------------------
//
CPsu2CopyAllToPbkCmd::~CPsu2CopyAllToPbkCmd()
    {
    Cancel();
    delete iDecorator;
    delete iCopyOperation;
    delete iCopier;
    delete iCopiedContacts;
    delete iValidSourceStoreUris;
    iSourceViews.ResetAndDestroy();
    CloseStores();
    delete iSourceContactLinks;
    iSourceStores.Close();
    delete iTheOnlyContactTitle;
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::NewL
// --------------------------------------------------------------------------
//
CPsu2CopyAllToPbkCmd* CPsu2CopyAllToPbkCmd::NewL()
    {
    CPsu2CopyAllToPbkCmd* self =
        new( ELeave ) CPsu2CopyAllToPbkCmd( );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ConstructL()
    {
    CActiveScheduler::Add(this);

    iValidSourceStoreUris = CVPbkContactStoreUriArray::NewL();
    iValidSourceStoreUris->AppendL( 
            VPbkContactStoreUris::SimGlobalSdnUri() );  
    iValidSourceStoreUris->AppendL( 
            VPbkContactStoreUris::SimGlobalAdnUri() );  
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::RunL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::RunL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2CopyAllToPbkCmd::RunL res %d task %d"),
        iStatus.Int(), iNextTask);
    
    switch ( iNextTask )
        {
        case EOpenStores:
            {
            OpenStoresL();
            break;
            }
        case ECheckSimStorePrerequisites:
            {
            CheckSimStorePrerequisitesL();
            break;
            }
        case ELaunchCopyAllSIMContactsQueryL:
            {
            LaunchCopyAllSIMContactsQueryL();
            break;
            }
        case ECreateSimViews:
            {
            CreateSimViewsL();
            break;
            }
        case ECopyContacts:
            {
            CopyContactsL();
            break;
            }
        case EComplete: // FALLTHROUGH
        default:
            {
            Complete();
            break;
            }
        }
    }
   
// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::DoCancel()
    {
    // Nothing to do here
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPsu2CopyAllToPbkCmd::RunError( TInt aError )
    {
    Finish( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ExecuteLD()
    {
    StartNext( EOpenStores );
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ResetUiControl
        ( MPbk2ContactUiControl& /*aUiControl*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::StoreReady
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::StoreReady
        ( MVPbkContactStore& /*aContactStore*/)
    {
    ++iCounter;
    // KTargetStoresCount = 1, as we are always copying to phone memory
    if ( iNextTask == EOpenStores &&
         iSourceStores.Count() + KTargetStoresCount == iCounter ) 
        {
        StartNext(ECheckSimStorePrerequisites); 
        }
    
    // iNextTask == EOpenStores above
    //    --> ignore the case where a store later becomes ready and command is 
    //        already in later phases
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::StoreUnavailable
        ( MVPbkContactStore& aContactStore,  TInt /*aReason*/ )
    {
    // Remove URI
    iValidSourceStoreUris->Remove( aContactStore.StoreProperties().Uri() );

    // Remove store
    const TInt count = iSourceStores.Count();
    TBool storeRemoved = EFalse;
    for ( TInt i = 0; i < count && !storeRemoved; ++i )
        {
        if ( iSourceStores[i] == &aContactStore )
            {
            iSourceStores[i]->Close( *this ); 
            iSourceStores.Remove( i );
            storeRemoved = ETrue;
            }
        }

    // If already moved on to next phase, ignore store becoming unavailable
    // let the ContactViewError or StepFailed handle this 
    if ( iNextTask == EOpenStores)
        {
        // If all source stores failed to open -> complete command
        if ( iSourceStores.Count() == 0 )
            {  
            StartNext( EComplete );
            }
        // All available stores are open
        else if ( iSourceStores.Count() + KTargetStoresCount == iCounter )
            {
            StartNext( ECheckSimStorePrerequisites );
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Nothing to be done here
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ContactViewReady
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ContactViewReady
        ( MVPbkContactViewBase& /*aView*/)
    {
    ++iCounter;
    if ( iSourceViews.Count() == iCounter )
        {
        StartNext( ECopyContacts );
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Nothing to be done here, wait view ready events
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ContactAddedToView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Nothing to be done here
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ContactRemovedFromView
        ( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/,
          const MVPbkContactLink& /*aContactLink*/ )
    {
    // Nothing to be done here
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ContactViewError
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ContactViewError
        ( MVPbkContactViewBase& /*aView*/, TInt aError,
          TBool /*aErrorNotified*/ )
    { 
    Finish( aError );
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::StepComplete
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::StepComplete
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aStepSize )
    {
    iCopiedSuccessfully += aStepSize;
    iDecorator->ProcessAdvance( aStepSize );
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::StepFailed
// --------------------------------------------------------------------------
//
TBool CPsu2CopyAllToPbkCmd::StepFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/,
          TInt aError )
    {
    Finish( aError );
    
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::OperationComplete
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::OperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/ )
    {
    // Decorator calls ProcessDismissed
    iDecorator->ProcessStopped();
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    iDecoratorShown = EFalse;
    Cancel();
    delete iCopyOperation;
    iCopyOperation = NULL;
    StartNext( EComplete );
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::OpenStoresL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::OpenStoresL()
    {
    // Load and open source stores to contact manager
    const TInt count = iValidSourceStoreUris->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            LoadContactStoreL( (*iValidSourceStoreUris)[i] );
        MVPbkContactStore* source =
            Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
                ContactStoresL().Find( (*iValidSourceStoreUris)[i] );
       if ( source )          
            {    
            iSourceStores.AppendL( source );
            source->OpenL( *this );
            }
        }
    
    // Also make sure the target store is always loaded and opened
    Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
                LoadContactStoreL( VPbkContactStoreUris::DefaultCntDbUri() );
    iTargetStore = Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
                       ContactStoresL().Find( VPbkContactStoreUris::DefaultCntDbUri() );         
    iTargetStore->OpenL( *this );
    
    // Next: wait for StoreReady and/or StoreUnavailable notifications
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::CheckSimStorePrerequisitesL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::CheckSimStorePrerequisitesL()
    {
    // If there are no contacts to copy then do nothing
    TInt sourceContactCount = 0;
    const TInt count = iSourceStores.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        sourceContactCount +=
            iSourceStores[i]->StoreInfo().NumberOfContactsL();
        }

    if ( sourceContactCount > 0 )
        {    
        StartNext ( ELaunchCopyAllSIMContactsQueryL );
        }
    else
    	{
        StartNext ( EComplete );
    	}
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::LaunchCopyAllSIMContactsQueryL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::LaunchCopyAllSIMContactsQueryL()
    {
    HBufC* text = StringLoader::LoadLC( R_QTN_SIMP_NOTE_COPY_ALL_SIM );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    TInt ret = dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *text );
 	     	
    CleanupStack::PopAndDestroy( text );
    
    StartNext ( ret ? ECreateSimViews : EComplete );    
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::CreateSimViewsL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::CreateSimViewsL()
    {
    // Set views ready counter to zero
    iCounter = 0;

    const TInt count = iSourceStores.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        // Find store property
        const CPbk2StoreProperty* prop =
            Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties().
                FindProperty( iSourceStores[i]->StoreProperties().Uri() );
        __ASSERT_DEBUG( prop,
            Panic( EStorePropertyNotFound_CreateSimViewsL ) );
        // Get Virtual Phonebook view definition
        // There is only one view defintion for SIM stores
        // -> take the first one
        const CVPbkContactViewDefinition& viewDef =
            (prop->Views())[0]->ViewDefinition();
        MVPbkContactView* view = iSourceStores[i]->CreateViewLC(
            viewDef,
            *this,
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                SortOrderManager().SortOrder() );
        iSourceViews.AppendL( view );
        CleanupStack::Pop(); // view
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::CopyContactsL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::CopyContactsL()
    {
    if ( !iCopier )
        {
        iCopier = CVPbkContactCopier::NewL
            ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );
        }
    
    // Create an array for source contact links
    if ( !iSourceContactLinks )
        {
        iSourceContactLinks = CVPbkContactLinkArray::NewL();
        }

    if ( !iCopiedContacts )
        {
        iCopiedContacts = CVPbkContactLinkArray::NewL();
        }
    
    // Create links to the contacts that are copied
    const TInt viewCount = iSourceViews.Count();
    
    for ( TInt i = 0; i < viewCount; ++i )
        {
        const TInt contactCount = iSourceViews[i]->ContactCountL();
        for( TInt j = 0; j < contactCount; ++j )
            {
            // store the first contact's title, if only one contact copied,
            // show the name of that single contact in the results note.
            if ( j == 0 )
                {
                iTheOnlyContactTitle = Phonebook2::Pbk2AppUi()->ApplicationServices().
                        NameFormatter().GetContactTitleL(
                                iSourceViews[i]->ContactAtL(j).Fields(),
                                MPbk2ContactNameFormatter::EPreserveLeadingSpaces);
                }
            // create link
            MVPbkContactLink* link =
                iSourceViews[i]->ContactAtL(j).CreateLinkLC();
            iSourceContactLinks->AppendL( link );
            CleanupStack::Pop(); // link
            }
        }

    // Views can be destroyed now because links have been created
    iSourceViews.ResetAndDestroy();

    // Get target store
    MVPbkContactStore* targetStore =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            ContactStoresL().Find( VPbkContactStoreUris::DefaultCntDbUri() );
   
    
    if (targetStore)
        {
        // Start copying
        iDecorator = Pbk2ProcessDecoratorFactory::CreateProgressDialogDecoratorL
                ( R_GENERAL_COPY_PROGRESS_NOTE, EFalse );
        iDecorator->SetObserver( *this );
     
        iCopiedContacts->ResetAndDestroy();      
        iCopyOperation = iCopier->CopyContactsL(
                CVPbkContactCopier::EVPbkUsePlatformSpecificDuplicatePolicy,
                *iSourceContactLinks, targetStore, *iCopiedContacts, *this );

        // Show progress note
        iDecorator->ProcessStartedL( iSourceContactLinks->Count() );
        iDecoratorShown = ETrue;
        }
    else
        {
        StartNext( EComplete );
        }
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::ShowResultsL
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::ShowResultsL()
    {

    if (iSourceContactLinks->Count() == 1 && iTheOnlyContactTitle)
        {
        TPsu2SimCopyResults  results(
                iCopiedSuccessfully, iTheOnlyContactTitle->Des() );
        results.ShowNoteL();
        }
    else 
        {
        TPsu2SimCopyResults  results(
                iCopiedSuccessfully, iSourceContactLinks->Count() );
        results.ShowNoteL();
        }
   
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::Complete   
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::Complete()
    {    
    if ( iSourceContactLinks )
        {
        TRAP_IGNORE( ShowResultsL() );
        }
    
    iCommandObserver->CommandFinished( *this );
    }

// ---------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::Finish
// ---------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::Finish( TInt aReason )
    {
    if ( aReason != KErrNone && aReason != KErrCancel )
        {
        CCoeEnv::Static()->HandleError( aReason );
        }
    
    if (iDecorator && iDecoratorShown)
        {
        // Decorator calls ProcessDismissed --> StartNext( EComplete );
        iDecorator->ProcessStopped();
        }
    else
        {
        StartNext( EComplete );
        }
    }

// ---------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::StartNext
// ---------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::StartNext( TNextTask aNextTask )
    {
    iNextTask = aNextTask;
    
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPsu2CopyAllToPbkCmd::CloseStores
// --------------------------------------------------------------------------
//
void CPsu2CopyAllToPbkCmd::CloseStores()
    {
    const TInt count = iSourceStores.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        iSourceStores[i]->Close( *this );
        }
    
    iTargetStore->Close( *this );
    }

//  End of File
