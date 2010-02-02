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
* Description:  Phonebook 2 server app contact data assign phase.
*
*/


#include "CPbk2AssignAttributePhase.h"

// Phonebook 2
#include "MPbk2ServicePhaseObserver.h"
#include "CPbk2ServerAppAppUi.h"
#include "MPbk2ContactAssigner.h"
#include "Pbk2ContactAssignerFactory.h"
#include <MPbk2DialogEliminator.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactLink.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactOperationBase.h>

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
// CPbk2AssignAttributePhase::CPbk2AssignAttributePhase
// --------------------------------------------------------------------------
//
CPbk2AssignAttributePhase::CPbk2AssignAttributePhase
        ( MPbk2ServicePhaseObserver& aObserver,
          TPbk2AttributeAssignData aAttributeData,
          TBool aRemoveAttribute ) :
            iObserver( aObserver ),
            iAttributeData( aAttributeData ),
            iRemoveAttribute( aRemoveAttribute )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::~CPbk2AssignAttributePhase
// --------------------------------------------------------------------------
//
CPbk2AssignAttributePhase::~CPbk2AssignAttributePhase()
    {
    delete iResults;
    delete iContactLinks;
    delete iRetrieveOperation;
    delete iContactAttributeAssigner;
    delete iStoreContactField;
    delete iStoreContact;
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2AssignAttributePhase::ConstructL
        ( const MVPbkContactLinkArray* aContactLinks )
    {
    iEikEnv = CEikonEnv::Static();

    iContactLinks = CVPbkContactLinkArray::NewL();
    // Take a own copy of supplied contact links
    CopyContactLinksL( *aContactLinks, *iContactLinks );
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::NewL
// --------------------------------------------------------------------------
//
CPbk2AssignAttributePhase* CPbk2AssignAttributePhase::NewL
        ( MPbk2ServicePhaseObserver& aObserver,
          MVPbkContactLinkArray* aContactLinks,
          TPbk2AttributeAssignData aAttributeData,
          TBool aRemoveAttribute )
    {
    CPbk2AssignAttributePhase* self =
        new ( ELeave ) CPbk2AssignAttributePhase
            ( aObserver, aAttributeData, aRemoveAttribute );
    CleanupStack::PushL( self );
    self->ConstructL( aContactLinks );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::LaunchServicePhaseL
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::LaunchServicePhaseL()
    {
    RetrieveContactL();
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::CancelServicePhase
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::CancelServicePhase()
    {
    iObserver.PhaseCanceled( *this );
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::RequestCancelL
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::RequestCancelL( TInt aExitCommandId )
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
// CPbk2AssignAttributePhase::AcceptDelayed
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::AcceptDelayedL
        ( const TDesC8& /*aContactLinkBuffer*/ )
    {
    // Nothing to do
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::Results
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CPbk2AssignAttributePhase::Results() const
    {
    return iResults;
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::ExtraResultData
// --------------------------------------------------------------------------
//
TInt CPbk2AssignAttributePhase::ExtraResultData() const
    {
    return KErrNotSupported;
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::TakeStoreContact
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CPbk2AssignAttributePhase::TakeStoreContact()
    {
    // Not supported
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::FieldContent
// --------------------------------------------------------------------------
//
HBufC* CPbk2AssignAttributePhase::FieldContent() const
    {
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::AssignComplete
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::AssignComplete
        ( MPbk2ContactAssigner& /*aAssigner*/, TInt /*aIndex*/ )
    {
    TRAPD( err, iStoreContact->CommitL( *this ) );

    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::AssignFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::AssignFailed
        ( MPbk2ContactAssigner& /*aAssigner*/, TInt aErrorCode )
    {
    iObserver.PhaseError( *this, aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& /*aOperation*/,
          MVPbkStoreContact* aContact )
    {
    // Contact retrieval complete
    delete iStoreContact;
    iStoreContact = aContact;

    TRAPD( err, iStoreContact->LockL( *this ) );

    if ( err != KErrNone )
        {
        iObserver.PhaseError( *this, err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    iObserver.PhaseError( *this, aError );
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::ContactOperationCompleted
        ( TContactOpResult aResult )
    {
    if ( aResult.iOpCode == EContactLock )
        {
        TRAPD( err, AssignAttributeL() );

        if ( err != KErrNone )
            {
            iObserver.PhaseError( *this, err );
            }
        }
    else if ( aResult.iOpCode == EContactCommit )
        {
        TRAPD( err, FinalizeL() );

        if ( err != KErrNone )
            {
            iObserver.PhaseError( *this, err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::ContactOperationFailed
        ( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    iObserver.PhaseError( *this, aErrorCode );
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::RetrieveContactL()
    {
    CPbk2ServerAppAppUi& appUi =
        static_cast<CPbk2ServerAppAppUi&>
            ( *iEikEnv->EikAppUi() );

    delete iRetrieveOperation;
    iRetrieveOperation = NULL;
    iRetrieveOperation = appUi.ApplicationServices().ContactManager().
        RetrieveContactL( iContactLinks->At( KFirstElement ), *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactSpeedDialAttributeAssigner::RetrieveContactFieldL
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::RetrieveContactFieldL()
    {
    MVPbkStoreContactField* preSelectedField =
        iStoreContact->Fields().RetrieveField
            ( iContactLinks->At( KFirstElement ) );

    if ( preSelectedField )
        {
        // There was a pre-selected field to operate with it
        delete iStoreContactField;
        iStoreContactField = NULL;

        // For ownership management, clone it and operate with the clone
        iStoreContactField = preSelectedField->CloneLC();
        CleanupStack::Pop(); // iStoreContactField
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::AppendResultL
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::AppendResultL()
    {
    // Add the contact link to the result array
    if ( iStoreContactField )
        {
        MVPbkContactLink* link = iStoreContactField->CreateLinkLC();

        if ( !iResults )
            {
            iResults = CVPbkContactLinkArray::NewL();
            }

        CleanupStack::Pop(); // iStoreContactField->CreateLinkLC()

        iResults->AppendL( link );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::FinalizeL
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::FinalizeL()
    {
    AppendResultL();
    iObserver.NextPhase( *this );
    }

// --------------------------------------------------------------------------
// CPbk2AssignAttributePhase::AssignAttributeL
// --------------------------------------------------------------------------
//
void CPbk2AssignAttributePhase::AssignAttributeL()
    {
    RetrieveContactFieldL();

    CPbk2ServerAppAppUi& appUi = static_cast<CPbk2ServerAppAppUi&>
        ( *iEikEnv->EikAppUi() );

    delete iContactAttributeAssigner;
    iContactAttributeAssigner = NULL;
    iContactAttributeAssigner =
        Pbk2ContactAssignerFactory::CreateContactAttributeAssignerL
            ( *this, appUi.ApplicationServices().ContactManager(),
              iAttributeData, iRemoveAttribute );

    iContactAttributeAssigner->AssignAttributeL( *iStoreContact,
        iStoreContactField, iAttributeData );
    }

// End of File
