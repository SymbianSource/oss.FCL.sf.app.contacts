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
* Description:  Phonebook 2 server app assign select multiple
*              : property phase.
*
*/


#include "CPbk2SelectMultiplePropertyPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2AssignSelectFieldDlg.h"
#include "MPbk2SelectFieldProperty.h"
#include "CPbk2AssignMultiProperty.h"
#include <TPbk2DestructionIndicator.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>


/// Unnamed namespace for local definitions
namespace {
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
// CPbk2SelectMultiplePropertyPhase::CPbk2SelectMultiplePropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectMultiplePropertyPhase::CPbk2SelectMultiplePropertyPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          HBufC8& aFilterBuffer,
          MPbk2SelectFieldProperty*& aSelectFieldProperty ) :
            CActive( EPriorityIdle ),
            iObserver( aObserver ),
            iFilterBuffer( aFilterBuffer ),
            iSelectFieldProperty( aSelectFieldProperty )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::~CPbk2SelectMultiplePropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectMultiplePropertyPhase::~CPbk2SelectMultiplePropertyPhase()
    {
    Cancel();

    if ( iSelectFieldDialogEliminator )
        {
        iSelectFieldDialogEliminator->ForceExit();
        }

    delete iContactLinks;

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2SelectMultiplePropertyPhase::ConstructL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    iContactLinks = CVPbkContactLinkArray::NewL();
    // Take a own copy of supplied contact links
    CopyContactLinksL( *aContactLinks, *iContactLinks );
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2SelectMultiplePropertyPhase* CPbk2SelectMultiplePropertyPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray* aContactLinks,
          HBufC8& aFilterBuffer,
          MPbk2SelectFieldProperty*& aSelectFieldProperty )
    {
    CPbk2SelectMultiplePropertyPhase* self =
        new ( ELeave ) CPbk2SelectMultiplePropertyPhase
            ( aObserver, aFilterBuffer, aSelectFieldProperty );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::LaunchServicePhaseL()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::CancelServicePhase()
    {
    if ( iSelectFieldDialogEliminator )
        {
        iSelectFieldDialogEliminator->ForceExit();
        }

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::RequestCancelL( TInt aExitCommandId )
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
// CPbk2SelectMultiplePropertyPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2SelectMultiplePropertyPhase::Results() const
    {
    return iContactLinks;
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2SelectMultiplePropertyPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2SelectMultiplePropertyPhase::TakeStoreContact()
    {
    // Not supported
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2SelectMultiplePropertyPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::RunL
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::RunL()
    {
    SelectFieldL();
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2SelectMultiplePropertyPhase::RunError( TInt aError )
    {
    if ( aError != KErrDied )
        {
        // If this is destroyed it is not safe to use iObserver anymore
        iObserver.PhaseError( *this, aError );
        }
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::DoCancel()
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2SelectMultiplePropertyPhase::OkToExitL( TInt aCommandId )
    {
    return iObserver.PhaseOkToExit( *this, aCommandId );
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::SelectFieldL
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::SelectFieldL()
    {
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    CPbk2AssignMultiProperty* multiProperty = CPbk2AssignMultiProperty::NewL
        ( iFilterBuffer, *iContactLinks );
    iSelectFieldProperty = multiProperty;

    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    TPbk2DestructionIndicator indicator( &thisDestroyed, iDestroyedPtr );

    // Select field query
    CPbk2AssignSelectFieldDlg* dlg =
        CPbk2AssignSelectFieldDlg::NewL( *this );
    iSelectFieldDialogEliminator = dlg;
    iSelectFieldDialogEliminator->ResetWhenDestroyed
        ( &iSelectFieldDialogEliminator );
    TInt result = dlg->ExecuteLD( *iSelectFieldProperty );

    if ( !thisDestroyed )
        {
        if ( result >= KErrNone )
            {
            iObserver.NextPhase( *this );
            }
        else if ( result == KErrAbort )
            {
            iObserver.PhaseAborted( *this );
            }
        else
            {
            iObserver.PhaseCanceled( *this );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectMultiplePropertyPhase::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2SelectMultiplePropertyPhase::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// End of File
