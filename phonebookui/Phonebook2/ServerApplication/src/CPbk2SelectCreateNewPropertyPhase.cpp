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
* Description:  Phonebook 2 server app assign select create
*              : new property phase.
*
*/


#include "CPbk2SelectCreateNewPropertyPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2AssignSelectFieldDlg.h"
#include "MPbk2SelectFieldProperty.h"
#include "CPbk2AssignCreateNewProperty.h"
#include "Pbk2ServerApp.hrh"
#include <TPbk2DestructionIndicator.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <TVPbkContactStoreUriPtr.h>

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::CPbk2SelectCreateNewPropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectCreateNewPropertyPhase::CPbk2SelectCreateNewPropertyPhase(
        TVPbkContactStoreUriPtr  aSavingStoreUri,
        MPbk2ServicePhaseObserver& aObserver, HBufC8* aFilterBuffer,
        MPbk2SelectFieldProperty*& aSelectFieldProperty, TInt& aResult,
        TUint& aNoteFlags ) :
    CActive( EPriorityIdle ),
    iSavingStoreUri(aSavingStoreUri.UriDes()),
    iObserver( aObserver ),
    iFilterBuffer( aFilterBuffer ),
    iSelectFieldProperty( aSelectFieldProperty ),
    iResult( aResult ),
    iNoteFlags( aNoteFlags )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::~CPbk2SelectCreateNewPropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectCreateNewPropertyPhase::~CPbk2SelectCreateNewPropertyPhase()
    {
    Cancel();

    if ( iSelectFieldDialogEliminator )
        {
        iSelectFieldDialogEliminator->ForceExit();
        }

    delete iStoreContact;

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2SelectCreateNewPropertyPhase* CPbk2SelectCreateNewPropertyPhase::NewL( 
        TVPbkContactStoreUriPtr  aSavingStoreUri,
        MPbk2ServicePhaseObserver& aObserver, HBufC8* aFilterBuffer,
        MPbk2SelectFieldProperty*& aSelectFieldProperty,
        TInt& aResult, TUint& aNoteFlags )
    {
    CPbk2SelectCreateNewPropertyPhase* self =
        new ( ELeave ) CPbk2SelectCreateNewPropertyPhase
            ( aSavingStoreUri, aObserver, aFilterBuffer, aSelectFieldProperty,
              aResult, aNoteFlags );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::LaunchServicePhaseL()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::CancelServicePhase()
    {
    if ( iSelectFieldDialogEliminator )
        {
        iSelectFieldDialogEliminator->ForceExit();
        }

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::RequestCancelL( TInt aExitCommandId )
    {
    if ( iSelectFieldDialogEliminator )
        {
        iSelectFieldDialogEliminator->RequestExitL( aExitCommandId );
        }

   if ( aExitCommandId == EEikBidCancel )
        {
        iObserver.PhaseAborted( *this );
        }
    else
        {
        iObserver.PhaseCanceled( *this );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::DenyDelayed
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::DenyDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2SelectCreateNewPropertyPhase::Results() const
    {
    // No contact link results
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2SelectCreateNewPropertyPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2SelectCreateNewPropertyPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2SelectCreateNewPropertyPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2SelectCreateNewPropertyPhase::OkToExitL( TInt aCommandId )
    {
    return iObserver.PhaseOkToExit( *this, aCommandId );
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::RunL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::RunL()
    {
    SelectFieldL();
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2SelectCreateNewPropertyPhase::RunError( TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::SelectFieldL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::SelectFieldL()
    {
    TInt result( KErrNone );

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );
   
    MVPbkContactStore* targetStore = appUi.ApplicationServices().
        ContactManager().ContactStoresL().Find( iSavingStoreUri );

    if( iFilterBuffer )
    	{

        // Select field query
        iSelectFieldProperty = CPbk2AssignCreateNewProperty::NewL
            ( *iFilterBuffer, targetStore,
              appUi.ApplicationServices().StoreProperties(),
              appUi.ApplicationServices().ContactManager() );

        TBool thisDestroyed = EFalse;
        iDestroyedPtr = &thisDestroyed;
        TPbk2DestructionIndicator indicator
            ( &thisDestroyed, iDestroyedPtr );

        CPbk2AssignSelectFieldDlg* dlg =
            CPbk2AssignSelectFieldDlg::NewL( *this );
        iSelectFieldDialogEliminator = dlg;
        iSelectFieldDialogEliminator->ResetWhenDestroyed
            ( &iSelectFieldDialogEliminator );
        result = dlg->ExecuteLD( *iSelectFieldProperty );
        
        if( thisDestroyed )
        	{
        	return;
        	}
    	}
    
    iResult = result;

    if ( result >= KErrNone )
        {
    	iNoteFlags |= KPbk2NoteFlagOneContactCreated;
        iStoreContact = targetStore->CreateNewContactLC();
        CleanupStack::Pop(); // targetStore->CreateNewContactLC()
        iObserver.NextPhase( *this );
        }
    else
        {
        iObserver.PhaseCanceled( *this );
        }
    }


// --------------------------------------------------------------------------
// CPbk2SelectCreateNewPropertyPhase::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateNewPropertyPhase::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
