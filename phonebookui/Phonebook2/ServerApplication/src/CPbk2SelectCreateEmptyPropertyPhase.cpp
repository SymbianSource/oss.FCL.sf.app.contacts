/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 server app assign create empty
*              : property phase.
*
*/


#include "CPbk2SelectCreateEmptyPropertyPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include <MPbk2ApplicationServices.h>
#include "CPbk2ServerAppAppUi.h"
#include "Pbk2ServerApp.hrh"

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>

// System includes
#include <eikon.hrh>


// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::CPbk2SelectCreateEmptyPropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectCreateEmptyPropertyPhase::CPbk2SelectCreateEmptyPropertyPhase(
        MPbk2ServicePhaseObserver& aObserver,
        TVPbkContactStoreUriPtr aSavingStoreUri,
        MPbk2SelectFieldProperty*& aSelectFieldProperty, TInt& aResult,
        TUint& aNoteFlags ) :
            CActive( EPriorityIdle ),
            iObserver( aObserver ),
            iSavingStoreUri( aSavingStoreUri ),
            iSelectFieldProperty( aSelectFieldProperty ),
            iResult( aResult ),
            iNoteFlags( aNoteFlags )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::~CPbk2SelectCreateEmptyPropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectCreateEmptyPropertyPhase::~CPbk2SelectCreateEmptyPropertyPhase()
    {
    Cancel();

    delete iStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2SelectCreateEmptyPropertyPhase* CPbk2SelectCreateEmptyPropertyPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          TVPbkContactStoreUriPtr aSavingStoreUri,
          MPbk2SelectFieldProperty*& aSelectFieldProperty,
          TInt& aResult, TUint& aNoteFlags )
    {
    CPbk2SelectCreateEmptyPropertyPhase* self =
        new ( ELeave ) CPbk2SelectCreateEmptyPropertyPhase
            ( aObserver, aSavingStoreUri, aSelectFieldProperty,
              aResult, aNoteFlags );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::LaunchServicePhaseL()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::CancelServicePhase()
    {
    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::RequestCancelL( TInt aExitCommandId )
    {
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
// CPbk2SelectCreateEmptyPropertyPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2SelectCreateEmptyPropertyPhase::Results() const
    {
    // No contact link results
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2SelectCreateEmptyPropertyPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2SelectCreateEmptyPropertyPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2SelectCreateEmptyPropertyPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::RunL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::RunL()
    {
    CreateNewContactL();

    iResult = KErrNotFound;
    iSelectFieldProperty = NULL;
    iObserver.NextPhase( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2SelectCreateEmptyPropertyPhase::RunError( TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2SelectCreateEmptyPropertyPhase::CreateNewContactL
// --------------------------------------------------------------------------
//
void CPbk2SelectCreateEmptyPropertyPhase::CreateNewContactL()
    {
   CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );
   
    MVPbkContactStore* targetStore = appUi.ApplicationServices().
        ContactManager().ContactStoresL().Find( iSavingStoreUri );

    iNoteFlags |= KPbk2NoteFlagOneContactCreated;
    iStoreContact = targetStore->CreateNewContactLC();
    CleanupStack::Pop(); // targetStore->CreateNewContactLC()
    }

// End of File
