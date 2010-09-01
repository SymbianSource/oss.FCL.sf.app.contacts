/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 belongs to groups command.
*
*/


#include "CPguBelongsToGroupsCmd.h"

// Phonebook 2
#include "CPguGroupPopup.h"
#include <Pbk2GroupUIRes.rsg>
#include <Pbk2UIControls.rsg>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <CPbk2StoreConfiguration.h>
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

// System includes
#include <avkon.hrh>
#include <coemain.h>

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
    _LIT(KPanicText, "CPguBelongsToGroupsCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::CPguBelongsToGroupsCmd
// --------------------------------------------------------------------------
//
CPguBelongsToGroupsCmd::CPguBelongsToGroupsCmd
        ( MPbk2ContactUiControl& aUiControl ) :
            CActive( CActive::EPriorityIdle ),
            iUiControl( &aUiControl )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguBelongsToGroupsCmd::CPguBelongsToGroupsCmd(0x%x)"),this);
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguBelongsToGroupsCmd::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::NewL
// --------------------------------------------------------------------------
//
CPguBelongsToGroupsCmd* CPguBelongsToGroupsCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPguBelongsToGroupsCmd* self =
        new(ELeave) CPguBelongsToGroupsCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::~CPguBelongsToGroupsCmd
// --------------------------------------------------------------------------
//
CPguBelongsToGroupsCmd::~CPguBelongsToGroupsCmd()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguBelongsToGroupsCmd::~CPguBelongsToGroupsCmd(0x%x)"),this);
    Cancel();
    delete iRetrieveOperation;
    delete iStoreContact;
    delete iGroupsJoined;
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::ExecuteLD()
    {
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_ExecuteLD));
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPguBelongsToGroupsCmd::ExecuteLD(0x%x)"), this);

    CleanupStack::PushL( this );

    if (iUiControl)
        {
        MVPbkContactLink* link = iUiControl->FocusedContactL()->CreateLinkLC();
        iRetrieveOperation =
            Phonebook2::Pbk2AppUi()->ApplicationServices().
                ContactManager().RetrieveContactL( *link, *this );
        CleanupStack::PopAndDestroy(); // link
        }
    else
        {
        IssueRequest( EFinishCommand );
        }
    CleanupStack::Pop( this );
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::ResetUiControl(
        MPbk2ContactUiControl& aUiControl)
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::RunL
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::RunL()
    {
    switch ( iState )
        {
        case ELaunchPopup:
            {
            LaunchGroupPopupL();
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
// CPguBelongsToGroupsCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPguBelongsToGroupsCmd::RunError( TInt aError )
    {
    CCoeEnv::Static()->HandleError( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& /*aOperation*/,
        MVPbkStoreContact* aContact )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;

    iStoreContact = aContact;
    IssueRequest( ELaunchPopup );
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt aError )
    {
    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );
        }
    CCoeEnv::Static()->HandleError( aError );
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::IsContactIncluded
// --------------------------------------------------------------------------
//
TBool CPguBelongsToGroupsCmd::IsContactIncluded(
        const MVPbkBaseContact& /*aContact*/ )
    {
    return EFalse;
    }


// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::LaunchGroupPopupL
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::LaunchGroupPopupL()
    {
    iGroupsJoined = iStoreContact->GroupsJoinedLC();
    CleanupStack::Pop();

    MVPbkContactViewBase* groupsView =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            ViewSupplier().AllGroupsViewL();

    CPguGroupPopup* groupPopup = CPguGroupPopup::NewL(
            iGroupsJoined,
            R_QTN_PHOB_QTL_ENTRY_IS_IN_GRP,
            R_QTN_PHOB_ENTRY_IS_IN_NO_GRP,
            R_PBK2_SOFTKEYS_EMPTY_BACK_EMPTY,
            EAknListBoxViewerFlags,
            *groupsView );

    groupPopup->ExecuteLD();

    IssueRequest( EFinishCommand );
    }

// --------------------------------------------------------------------------
// CPguBelongsToGroupsCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPguBelongsToGroupsCmd::IssueRequest( TProcessState aState )
    {
    if ( !IsActive() )
        {
        iState = aState;
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();
        }
    }
//  End of File
