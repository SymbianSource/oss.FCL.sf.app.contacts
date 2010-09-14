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
* Description:  Phonebook 2 group rename command.
*
*/


// INCLUDE FILES
#include "CPguRenameGroupCmd.h"

// Phonebook 2
#include "CPguGroupNameQueryDlg.h"
#include "Pbk2GroupConsts.h"
#include <pbk2groupuires.rsg>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2StoreConfiguration.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactGroup.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactOperationBase.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStoreProperties.h>

// System includes
#include <StringLoader.h>
#include <aknnotewrappers.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

    #ifdef _DEBUG
    enum TPanicCode
        {
        EPanicPreCond_NewL = 1,
        EPanicPreCond_ExecuteLD,
        EPanicContactNotGroup,
        ERunL_InvalidState
        };

    void Panic(TPanicCode aReason)
        {
        _LIT(KPanicText, "CPguRenameGroupCmd");
        User::Panic(KPanicText,aReason);
        }
    #endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::CPguRenameGroupCmd
// --------------------------------------------------------------------------
//
CPguRenameGroupCmd::CPguRenameGroupCmd
        ( MPbk2ContactUiControl& aUiControl ) :
            CActive( CActive::EPriorityIdle ),
             iUiControl( &aUiControl )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguRenameGroupCmd::CPguRenameGroupCmd(0x%x)"), this);
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguRenameGroupCmd::ConstructL()
    {
    CActiveScheduler::Add( this );
    iGroupLink = iUiControl->FocusedContactL()->CreateLinkLC();
    CleanupStack::Pop();

    CPbk2StoreConfiguration* storeConfig = CPbk2StoreConfiguration::NewL();
    CleanupStack::PushL( storeConfig );
    iTargetStore = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().ContactStoresL().
            Find( TVPbkContactStoreUriPtr( storeConfig->DefaultSavingStoreL() ) );
    CleanupStack::PopAndDestroy( storeConfig );
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::NewL
// --------------------------------------------------------------------------
//
CPguRenameGroupCmd* CPguRenameGroupCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPguRenameGroupCmd* self = 
            new(ELeave) CPguRenameGroupCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::~CPguRenameGroupCmd
// --------------------------------------------------------------------------
//
CPguRenameGroupCmd::~CPguRenameGroupCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguRenameGroupCmd::~CPguRenameGroupCmd(0x%x)"), this);
    Cancel();
    delete iGroupLink;
    delete iContactOperation;
    delete iContactGroup;
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::ExecuteLD()
    {
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_ExecuteLD));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguRenameGroupCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );
    iContactOperation = Phonebook2::Pbk2AppUi()->ApplicationServices().
        ContactManager().RetrieveContactL( *iGroupLink, *this );
    CleanupStack::Pop( this );
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::RunL
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::RunL()
    {
    switch ( iState )
        {
        case ELauchQuery:
            {
            QueryGroupNameL();
            break;
            }
        case EFinishCommand:
            {
            if ( iCommandObserver )
                {
                iCommandObserver->CommandFinished( *this );
                }
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic(ERunL_InvalidState) );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::DoCancel()
    {
    //Do nothing
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPguRenameGroupCmd::RunError( TInt aError )
    {   
    IssueRequest(EFinishCommand);
   
    return aError;
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::ContactOperationCompleted( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == MVPbkContactObserver::EContactLock )
        {
        IssueRequest( ELauchQuery );
        }
    else if ( aResult.iOpCode == MVPbkContactObserver::EContactCommit )
        {
        CleanupCommand();
        }
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::ContactOperationFailed(
        TContactOp /*aOpCode*/,
        TInt aErrorCode,
        TBool aErrorNotified )
    {
    if ( !aErrorNotified )
        {
        CCoeEnv::Static()->HandleError( aErrorCode );
        }
    CleanupCommand();
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& /*aOperation*/,
                MVPbkStoreContact* aContact )
    {
    __ASSERT_DEBUG(aContact->Group(), Panic(EPanicContactNotGroup));

    iContactGroup = aContact->Group();
    if ( iContactGroup )
        {
        // Lock contact group for editing
        TRAPD( error, iContactGroup->LockL( *this ) );

        if ( error != KErrNone )
            {
            CCoeEnv::Static()->HandleError( error );
            IssueRequest( EFinishCommand );
            }
        }

    delete iContactOperation;
    iContactOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation,
        TInt aError )
    {
    if ( iContactOperation == &aOperation )
        {
        CCoeEnv::Static()->HandleError( aError );
        CleanupCommand();
        delete iContactOperation;
        iContactOperation = NULL;                
        }
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::QueryGroupNameL
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::QueryGroupNameL()
    {
    HBufC* textBuf = HBufC::NewLC( KGroupLabelLength );
    TPtr text = textBuf->Des();
    TInt lengthInUse = Min( KGroupLabelLength, iContactGroup->GroupLabel().Length() );
    text.Append( iContactGroup->GroupLabel().Left( lengthInUse ) );

    MVPbkContactViewBase* groupsView =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            ViewSupplier().AllGroupsViewL();

    CPguGroupNameQueryDlg* dlg = CPguGroupNameQueryDlg::NewL( text,
            *groupsView, Phonebook2::Pbk2AppUi()->ApplicationServices().
                NameFormatter(), EFalse );

    dlg->SetMaxLength( KGroupLabelLength );

    if ( dlg->ExecuteLD( R_PHONEBOOK2_GROUPLIST_NEWGROUP_QUERY ) )
        {
        iContactGroup->SetGroupLabelL( text );
        iContactGroup->CommitL( *this );
        }
    else
        {
        IssueRequest( EFinishCommand );
        }
    CleanupStack::PopAndDestroy( textBuf );
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::CleanupCommand
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::CleanupCommand()
    {
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }
    if (iUiControl)
        {
        iUiControl->UpdateAfterCommandExecution();
        }
    }

// --------------------------------------------------------------------------
// CPguRenameGroupCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPguRenameGroupCmd::IssueRequest( TProcessState aState )
    {
    if ( !IsActive() )
        {
        iState = aState;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }

// End of File
