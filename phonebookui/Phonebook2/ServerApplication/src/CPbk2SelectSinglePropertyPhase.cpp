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
* Description:  Phonebook 2 server app assign select single property phase.
*
*/


#include "CPbk2SelectSinglePropertyPhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "CPbk2AssignSelectFieldDlg.h"
#include "MPbk2SelectFieldProperty.h"
#include "CPbk2AssignSingleProperty.h"
#include <TPbk2DestructionIndicator.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>


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
// CPbk2SelectSinglePropertyPhase::CPbk2SelectSinglePropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectSinglePropertyPhase::CPbk2SelectSinglePropertyPhase
        ( MPbk2ServicePhaseObserver& aObserver,
          HBufC8* aFilterBuffer,
          MPbk2SelectFieldProperty*& aSelectFieldProperty, TInt& aResult ) :
            iObserver( aObserver ),
            iFilterBuffer( aFilterBuffer ),
            iSelectFieldProperty( aSelectFieldProperty ),
            iResult( aResult )
    {
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::~CPbk2SelectSinglePropertyPhase
// --------------------------------------------------------------------------
//
CPbk2SelectSinglePropertyPhase::~CPbk2SelectSinglePropertyPhase()
    {
    if ( iSelectFieldDialogEliminator )
        {
        iSelectFieldDialogEliminator->ForceExit();
        }

    delete iRetrieveOperation;
    delete iContactLinks;
    delete iStoreContact;

    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2SelectSinglePropertyPhase::ConstructL
        ( MVPbkContactLinkArray* aContactLinks )
    {
    iContactLinks = CVPbkContactLinkArray::NewL();
    // Take a own copy of supplied contact links
    CopyContactLinksL( *aContactLinks, *iContactLinks );
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::NewL
// --------------------------------------------------------------------------
//
CPbk2SelectSinglePropertyPhase* CPbk2SelectSinglePropertyPhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray* aContactLinks,
          HBufC8* aFilterBuffer,
          MPbk2SelectFieldProperty*& aSelectFieldProperty, TInt& aResult )
    {
    CPbk2SelectSinglePropertyPhase* self =
        new ( ELeave ) CPbk2SelectSinglePropertyPhase
            ( aObserver, aFilterBuffer, aSelectFieldProperty, aResult );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::LaunchServicePhaseL()
    {
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::CancelServicePhase()
    {
    if ( iSelectFieldDialogEliminator )
        {
        iSelectFieldDialogEliminator->ForceExit();
        }

    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::RequestCancelL( TInt aExitCommandId )
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
// CPbk2SelectSinglePropertyPhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2SelectSinglePropertyPhase::Results() const
    {
    return iContactLinks;
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2SelectSinglePropertyPhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2SelectSinglePropertyPhase::TakeStoreContact()
    {
    MVPbkStoreContact* contact = iStoreContact;
    iStoreContact = NULL;
    return contact;
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2SelectSinglePropertyPhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    // Contact retrieval complete
    delete iStoreContact;
    iStoreContact = aContact;

    // Locking the contact and after the updating is finished, the contact 
    // will be committed in ContactOperationCompleted() or 
    // SelectFieldL()in case of the user's cancellation
    iStoreContact->LockL(*this);
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2SelectSinglePropertyPhase::OkToExitL( TInt aCommandId )
    {
    return iObserver.PhaseOkToExit( *this, aCommandId );
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::RetrieveContactL()
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

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::SelectFieldL
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::SelectFieldL()
    {
    TInt result( KErrNone );
    
    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *CEikonEnv::Static()->EikAppUi() );

    if( iFilterBuffer )
    	{
        CPbk2AssignSingleProperty* singleProperty =
            CPbk2AssignSingleProperty::NewL
                ( *iFilterBuffer, *iStoreContact,
                  appUi.ApplicationServices().ContactManager() );
        iSelectFieldProperty = singleProperty;

        TBool thisDestroyed = EFalse;
        iDestroyedPtr = &thisDestroyed;
        TPbk2DestructionIndicator indicator( &thisDestroyed, iDestroyedPtr );

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

    if ( result == KErrCancel )
        {
        iStoreContact->CommitL(*this);
        }

    iResult = result;

    iObserver.NextPhase( *this );
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == EContactLock )
        {
        TRAPD( err, SelectFieldL() );
        if ( err != KErrNone )
            {
            iStoreContact->CommitL(*this);
            iObserver.PhaseError( *this, err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2SelectSinglePropertyPhase::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2SelectSinglePropertyPhase::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    iObserver.PhaseError( *this, aErrorCode );
    CancelServicePhase();  
    }
// End of File
