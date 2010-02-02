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
* Description:  Phonebook 2 server app assign update
*              : empty property phase.
*
*/


#include "CPbk2SelectUpdateEmptyPropertyPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include <MPbk2ApplicationServices.h>
#include "CPbk2ServerAppAppUi.h"

// Virtual Phonebook
#include <MVPbkStoreContact.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <eikon.hrh>

/// Unnamed namespace for local definitions
namespace {

const TInt KFirstElement = 0;

/**
 * Copies a link array to another.
 *
 * @param aSourceLinkArray    Link array which is copied
 * @param aTargetLinkArray    Links are copied to this
 */
void CopyContactLinksL( const MVPbkContactLinkArray& aSourceLinkArray,
        CVPbkContactLinkArray& aTargetLinkArray )
    {
    const TInt count = aSourceLinkArray.Count();
    for ( TInt i(0); i < count; ++i )
        {
        const MVPbkContactLink& contactLink = aSourceLinkArray.At(i);
        aTargetLinkArray.AppendL( contactLink.CloneLC() );
        CleanupStack::Pop(); // link
        }
    }

} /// namespace

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::CPbk2SelectUpdateEmptyPropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectUpdateEmptyPropertyPhase::CPbk2SelectUpdateEmptyPropertyPhase(
        MPbk2ServicePhaseObserver& aObserver,
        MPbk2SelectFieldProperty*& aSelectFieldProperty, TInt& aResult,
        TUint& /*aNoteFlags*/ ) :
            iObserver( aObserver ),
            iSelectFieldProperty( aSelectFieldProperty ),
            iResult( aResult )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::~CPbk2SelectUpdateEmptyPropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectUpdateEmptyPropertyPhase::~CPbk2SelectUpdateEmptyPropertyPhase()
    {
    delete iRetrieveOperation;
    delete iContactLinks;
    delete iStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2SelectUpdateEmptyPropertyPhase::ConstructL
		( MVPbkContactLinkArray& aContactLinks )
	{
	iContactLinks = CVPbkContactLinkArray::NewL();
	// Take a own copy of supplied contact links
	CopyContactLinksL( aContactLinks, *iContactLinks );
	}

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2SelectUpdateEmptyPropertyPhase* CPbk2SelectUpdateEmptyPropertyPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray& aContactLinks,
          MPbk2SelectFieldProperty*& aSelectFieldProperty,
          TInt& aResult, TUint& aNoteFlags )
    {
    CPbk2SelectUpdateEmptyPropertyPhase* self =
        new ( ELeave ) CPbk2SelectUpdateEmptyPropertyPhase
            ( aObserver, aSelectFieldProperty,
              aResult, aNoteFlags );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks );
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2SelectUpdateEmptyPropertyPhase::LaunchServicePhaseL()
    {
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2SelectUpdateEmptyPropertyPhase::CancelServicePhase()
    {
    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2SelectUpdateEmptyPropertyPhase::RequestCancelL
        ( TInt aExitCommandId )
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
// CPbk2SelectUpdateEmptyPropertyPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2SelectUpdateEmptyPropertyPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2SelectUpdateEmptyPropertyPhase::Results() const
    {
    return iContactLinks;
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2SelectUpdateEmptyPropertyPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2SelectUpdateEmptyPropertyPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2SelectUpdateEmptyPropertyPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2SelectUpdateEmptyPropertyPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    // Contact retrieval complete
    delete iStoreContact;
    iStoreContact = aContact;

    iResult = KErrNotFound;
    iSelectFieldProperty = NULL;
    iObserver.NextPhase( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2SelectUpdateEmptyPropertyPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2SelectUpdateEmptyPropertyPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2SelectUpdateEmptyPropertyPhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>
            ( *CEikonEnv::Static()->EikAppUi() );

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iRetrieveOperation = appUi.ApplicationServices().ContactManager().
        RetrieveContactL( iContactLinks->At( KFirstElement ), *this );
    // Do not delete anything from iContactLinks since it is not necessary
    // and would result to errors when client queries results
    }

// End of File
