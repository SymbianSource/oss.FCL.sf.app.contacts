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
* Description:  Phonebook 2 USIM UI Extension launch view command.
*
*/


#include "CPsu2LaunchViewCmd.h"

// Phonebook 2
#include "CPsu2ViewManager.h"
#include <Pbk2ProcessDecoratorFactory.h>
#include <Pbk2USimUIRes.rsg>
#include <CPbk2AppUiBase.h>
#include <MPbk2ViewExplorer.h>
#include <CPbk2SortOrderManager.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactViewBase.h>
#include <MPbk2CommandObserver.h>
#include <MVPbkSimPhone.h>

// System includes
#include <coemain.h>

/// Unnamed namespace for local definitions
namespace {

/**
 * Converts the given view id to related contact store URI.
 *
 * @param aViewId   Id to convert.
 * @return  Contact store URI.
 */
const TDesC& GetURI(TPsu2ViewId aViewId)
    {
    switch (aViewId)
        {
        case EPsu2OwnNumberViewId:
            {
            return VPbkContactStoreUris::SimGlobalOwnNumberUri();
            }
        case EPsu2ServiceDialingViewId:
            {
            return VPbkContactStoreUris::SimGlobalSdnUri();
            }
        case EPsu2FixedDialingViewId:
            {
            return VPbkContactStoreUris::SimGlobalFdnUri();
            }
        default:
            {
            return KNullDesC;
            }
        }
    }

} /// namespace


// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::CPsu2LaunchViewCmd
// --------------------------------------------------------------------------
//
CPsu2LaunchViewCmd::CPsu2LaunchViewCmd
        ( CPsu2ViewManager& aViewManager, 
          TPsu2ViewId aViewId ) :
            CActive( EPriorityStandard ),
            iViewManager( aViewManager ),
            iViewId( aViewId ),
            iContactViewReady( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::~CPsu2LaunchViewCmd
// --------------------------------------------------------------------------
//
CPsu2LaunchViewCmd::~CPsu2LaunchViewCmd()
    {
    iViewManager.RemoveViewLaunchCallBack();    
    
    delete iDecorator;
    
    if (iVPbkView)
        {
        iVPbkView->RemoveObserver(*this);
        }
    iViewManager.DeregisterStoreAndView();

    Cancel();
    }


// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPsu2LaunchViewCmd::ConstructL()
    {
    CActiveScheduler::Add( this );

    iDecorator = Pbk2ProcessDecoratorFactory::CreateWaitNoteDecoratorL
        ( R_QTN_FDN_READING_CONTACTS_WAIT_NOTE, ETrue );
    iDecorator->SetObserver( *this );
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::NewL
// --------------------------------------------------------------------------
//
CPsu2LaunchViewCmd* CPsu2LaunchViewCmd::NewL
        ( CPsu2ViewManager& aViewManager, 
          TPsu2ViewId aViewId )
    {
    CPsu2LaunchViewCmd* self = new ( ELeave ) CPsu2LaunchViewCmd
        ( aViewManager, aViewId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::RunL
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::RunL()
    {
    iDecorator->ProcessStartedL( 0 ); // wait note doesn't care about amount
    OpenStoreAndViewL();
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::DoCancel()
    {
    // Nothing to cancel
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPsu2LaunchViewCmd::RunError( TInt aError )
    {
    HandleError( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ExecuteLD()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ResetUiControl
        ( MPbk2ContactUiControl& /*aUiControl*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ProcessDismissed( TInt aCancelCode )
    {
    if ( aCancelCode != EAknSoftkeyDone )
        {
        iVPbkView = NULL;
        }
    else
        {
        if ( iContactViewReady )
            {
            CPbk2AppUiBase<CAknViewAppUi>& appUi = static_cast<CPbk2AppUiBase<CAknViewAppUi>&>
                ( *CEikonEnv::Static()->EikAppUi() );
            
            // if appUi is not in in foreground (that means phonebook2 has hidden itself in background
            // and names list view has been activated so that when re-enter phonebook user will see names 
            // list view is the first view ), then no need to active the fixed dialing view.
            if ( appUi.IsForeground() )
                {
                TRAPD( error, Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
                    ActivatePhonebook2ViewL( TUid::Uid( iViewId ), NULL ) );
                if ( error != KErrNone )
                    {
                    HandleError( error );
                    }          
                }

            if ( iVPbkView )
                {
                iVPbkView->RemoveObserver( *this );
                }
            }
        }
    
    if ( iCommandObserver )
        {
        iCommandObserver->CommandFinished( *this );    
        }    
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ContactViewReady
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ContactViewReady( MVPbkContactViewBase& aView )
    {
    if ( iVPbkView == &aView )
        {
        iContactViewReady = ETrue;
        iDecorator->ProcessStopped();
        }
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ContactViewUnavailable
        ( MVPbkContactViewBase& /*aView*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ContactAddedToView( 
        MVPbkContactViewBase& /*aView*/, 
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ContactRemovedFromView( 
        MVPbkContactViewBase& /*aView*/, 
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::ContactViewError
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::ContactViewError( 
        MVPbkContactViewBase& aView, 
        TInt aError,
        TBool /*aErrorNotified*/ )
    {
    if ( iVPbkView == &aView )
        {
        HandleError( aError );
        }
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::SetContactViewL
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::SetContactViewL( MVPbkContactViewBase& aContactView )
    {
    iVPbkView = &aContactView;
    iVPbkView->AddObserverL( *this );
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::HandleError
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::HandleError( TInt aError )
    {
    iDecorator->ProcessStopped();
    CCoeEnv::Static()->HandleError( aError );
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPsu2LaunchViewCmd::OpenStoreAndViewL
// --------------------------------------------------------------------------
//
void CPsu2LaunchViewCmd::OpenStoreAndViewL()
    {
    TVPbkContactStoreUriPtr uri( GetURI( iViewId ) );
    iViewManager.ResetAndInitiateStoreAndViewL( uri, iViewId, *this );
    iViewManager.RegisterStoreAndView();
    }

//  End of File
