/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 copy contacts starter command.
*
*/


#include "CPbk2CopyContactsStarterCmd.h"

// Phonebook 2
#include "CPbk2CopyContactsCmd.h"
#include "TPbk2CopyContactsUtil.h"
#include <CPbk2AppUiBase.h>
#include <CPbk2StoreListQuery.h>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>
#include <MPbk2CommandObserver.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2AppUi.h>
#include <CPbk2ApplicationServices.h>
#include <Pbk2Commands.hrh>
#include <Pbk2Commands.rsg>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <MVPbkBaseContact.h>

// System includes
#include <StringLoader.h>
#include <aknlistquerydialog.h>
#include <aknnotewrappers.h>
#include <badesca.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_Null_Pointer = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2CopyContactsStarterCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::CPbk2CopyContactsStarterCmd
// --------------------------------------------------------------------------
//
CPbk2CopyContactsStarterCmd::CPbk2CopyContactsStarterCmd
        ( MPbk2ContactUiControl& aUiControl ) :
            CActive( EPriorityStandard ),
            iUiControl( &aUiControl),
            iState( EPbk2CopyOpenStores )
    {
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::~CPbk2CopyContactsStarterCmd
// --------------------------------------------------------------------------
//
CPbk2CopyContactsStarterCmd::~CPbk2CopyContactsStarterCmd()
    {
    Cancel();
        
    delete iExecutiveCommand;
    delete iSelectedContacts;
    delete iTargetStoreUris;
    delete iFailedStoreUris;
    delete iStoreControl;
    delete iIdleQuery;
    if ( iUiControl )
        {
        iUiControl->UpdateAfterCommandExecution();  
        }
    
    // Close all store handles
    // This should be done after other members are deleted because they
    // might use stores that we close here.
    if ( iStoreList )
        {
        const TInt count = iStoreList->Count();
        for (TInt j = 0; j < count; ++j)
            {
            MVPbkContactStore& store = iStoreList->At(j);
            store.Close(*this);
            }
        }
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CopyContactsStarterCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();

    CActiveScheduler::Add( this );
    iUiControl->RegisterCommand( this );
    
    iFailedStoreUris = CVPbkContactStoreUriArray::NewL();

    iStoreControl = CPbk2ContactUiControlSubstitute::NewL();
    
    iIdleQuery = CIdle::NewL( CActive::EPriorityIdle );
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2CopyContactsStarterCmd* CPbk2CopyContactsStarterCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPbk2CopyContactsStarterCmd* self = 
        new ( ELeave ) CPbk2CopyContactsStarterCmd( aUiControl );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::ExecuteLD()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::AddObserver
        ( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::ResetUiControl
        ( MPbk2ContactUiControl& aUiControl )
    {
    if ( iUiControl == &aUiControl )
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::RunL()
    {
    switch ( iState )
        {
        case EPbk2CopyStarterQuery:
            {
            // Cancel any outstanding before starting new one.
            iIdleQuery->Cancel();
            // Request idle query. If the append above failed,
            // it doesn't matter.
            iIdleQuery->Start( TCallBack(
                ( &CPbk2CopyContactsStarterCmd::IdleQueryCallbackL ),this ) );
            break;
            }
        case EPbk2CopyStarterExecuteCopy:
            {
            ExecuteCopyCommandL();
            break;
            }
        case EPbk2CopyOpenStores:
            {
            OpenStoresL();
            break;
            }
        case EPbk2CopyStarterInit:
            {
            InitCommandL();
            break;
            }
        case EPbk2CopyStarterComplete:  // FALLTHROUGH
        default:
            {
            if ( iUiControl )
                {
                iUiControl->UpdateAfterCommandExecution();  
                }            
            iCommandObserver->CommandFinished(*this);
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::DoCancel()
    {
    // Nothing to cancel
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2CopyContactsStarterCmd::RunError( TInt aError )
    {
    CCoeEnv::Static()->HandleError( aError );
    iCommandObserver->CommandFinished( *this );
    return KErrNone;
    }


// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::CommandFinished
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::CommandFinished
        ( const MPbk2Command& /*aCommand*/ )
    {
    __ASSERT_DEBUG( iCommandObserver,
        Panic(EPanicPreCond_Null_Pointer));
    
    iCommandObserver->CommandFinished( *this );
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::StoreReady
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::StoreReady
        ( MVPbkContactStore& /*aContactStore*/ )
    {
    ++iRespondedStores;

    // All stores have responded
    if ( iRespondedStores == iStoreList->Count() )
        {
        iState = EPbk2CopyStarterInit;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::StoreUnavailable
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    ++iRespondedStores;

    TRAPD( err, iFailedStoreUris->AppendL
        ( aContactStore.StoreProperties().Uri() ) );

    if (err != KErrNone)
        {
        CCoeEnv::Static()->HandleError(err);
        }

    // All stores have responded
    if ( iRespondedStores == iStoreList->Count() )
        {
        iState = EPbk2CopyStarterInit;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::HandleStoreEventL
        ( MVPbkContactStore& /*aContactStore*/,
          TVPbkContactStoreEvent /*aStoreEvent*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::IdleQueryCallbackL
// --------------------------------------------------------------------------
//
TInt CPbk2CopyContactsStarterCmd::IdleQueryCallbackL( TAny* aSelf )
	{
	CPbk2CopyContactsStarterCmd* self = static_cast<CPbk2CopyContactsStarterCmd*>( aSelf );
    self->QueryL();
    return 0;
	}

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::QueryL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::QueryL()
    {
    const TInt targetCount = iTargetStoreUris->Count();
    switch( targetCount )
        {
        case EPbk2NoContacts: // No targets
            {
            // There were no writable stores, show note
            HBufC* noStores = StringLoader::LoadLC( R_QTN_PBCOP_INFO_NO_STORES );
            CAknInformationNote* dlg = new(ELeave) CAknInformationNote( EFalse );
            dlg->ExecuteLD( *noStores );
            CleanupStack::PopAndDestroy( noStores );            
            
            // Complete command
            iState = EPbk2CopyStarterComplete;
            IssueRequest();
            break;
            }
        default:
            {
            const TInt KOneStore = 1;
            HBufC* title = GetQueryTitleLC();
            CPbk2StoreListQuery* query = CPbk2StoreListQuery::NewL();
            
            // Query modifies iTargetStoreUris array.
            // After the query there must be only one, the selected, store.
            if ( query->ExecuteLD(*title, *iTargetStoreUris) == EAknSoftkeyOk &&
                iTargetStoreUris->Count() == KOneStore )
                {
                MVPbkContactStore* store = 
                    iAppServices->ContactManager().
                        ContactStoresL().Find( (*iTargetStoreUris )[0] );
                iStoreControl->SetSelectedStore(*store);
                iState = EPbk2CopyStarterExecuteCopy;
                }
            else
                {
                iState = EPbk2CopyStarterComplete;
                }
            CleanupStack::PopAndDestroy(title);
            IssueRequest();
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::ExecuteCopyCommandL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::ExecuteCopyCommandL()
    {
    TInt executiveCommandId = EPbk2CmdCopyContacts;
    
    CPbk2UIExtensionManager* extManager = CPbk2UIExtensionManager::InstanceL();
    extManager->PushL();
    MPbk2Command* cmd = NULL;
    if (iUiControl)
        {
        iStoreControl->SetParentUiControl(*iUiControl);
        cmd = extManager->FactoryL()->CreatePbk2CommandForIdL(
                executiveCommandId, *iStoreControl);
        }
    if (!cmd)
        {
        // Create Phonebook2 copy command if extension
        // didn't offer a command
        cmd = CPbk2CopyContactsCmd::NewL(*iStoreControl); 
        }
    // NULL UI control, because created cmd is topmost command
    iUiControl = NULL;
    cmd->AddObserver(*this);
    cmd->ExecuteLD(); //This not really delete command
    iExecutiveCommand = cmd;
    CleanupStack::PopAndDestroy(); // extManager
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::OpenStoresL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::OpenStoresL()
    {
    // We need to open all the possible stores
    CPbk2StoreConfiguration& config = iAppServices->StoreConfiguration();
    CVPbkContactStoreUriArray* stores =
        config.SupportedStoreConfigurationL();
    CleanupStack::PushL(stores);

    CVPbkContactManager& manager = iAppServices->ContactManager();
    const TInt storeCount( stores->Count() ); 
    for (TInt i=0; i< storeCount; ++i)
        {
        // Ensure stores loaded
        manager.LoadContactStoreL((*stores)[i]);
        }

    CleanupStack::PopAndDestroy(stores);

    iStoreList = &manager.ContactStoresL();

    // Open all stores one by one (in order to be able to close them too)
    const TInt storeListCount( iStoreList->Count() );
    for (TInt j=0; j< storeListCount; ++j)
        {
        MVPbkContactStore& store = iStoreList->At(j);
        store.OpenL(*this);
        }
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::InitCommandL
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::InitCommandL()
    {
    // Stores failed
    if ( iFailedStoreUris->Count() == iStoreList->Count() )
        {
        HBufC* noStores = StringLoader::LoadLC(R_QTN_PBCOP_INFO_NO_STORES);
        CAknInformationNote* dlg =
            new ( ELeave ) CAknInformationNote( EFalse );
        dlg->ExecuteLD( *noStores );
        CleanupStack::PopAndDestroy( noStores );
        }

    if ( iUiControl )
        {
        iTargetStoreUris = 
            TPbk2CopyContactsUtil::CopyContactTargetStoresLC
                ( *iUiControl, iFailedStoreUris );

        CleanupStack::Pop( iTargetStoreUris );
        
        iSelectedContacts = iUiControl->SelectedContactsOrFocusedContactL();
        
        if ( iSelectedContacts )
            {
            iState = EPbk2CopyStarterQuery;
            }
        else
            {
            iState = EPbk2CopyStarterComplete;
            }
            
        }
    else
        {
        // control doesnt exists, end copy operation
        iState = EPbk2CopyStarterComplete;
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::GetQueryTitleLC
// --------------------------------------------------------------------------
//
HBufC* CPbk2CopyContactsStarterCmd::GetQueryTitleLC()
    {
    TInt count = EPbk2NoContacts;
    if (iSelectedContacts)
        {
        count = iSelectedContacts->Count();
        }
    
    TInt resid(0);
    if (count > EPbk2OneContact)
        {
        resid = R_QTN_PHOB_QUERY_TITLE_COPY_MANY;
        }
    else
        {
        resid = R_QTN_PHOB_QUERY_TITLE_COPY;
        }
        
    return StringLoader::LoadLC( resid );        
    }

// --------------------------------------------------------------------------
// CPbk2CopyContactsStarterCmd::HandleError
// --------------------------------------------------------------------------
//
void CPbk2CopyContactsStarterCmd::HandleError(TInt aError)
    {
    if (aError != KErrNone)
        {
        iState = EPbk2CopyStarterComplete;
        IssueRequest();
        CCoeEnv::Static()->HandleError(aError);
        }
    }

//  End of File  
