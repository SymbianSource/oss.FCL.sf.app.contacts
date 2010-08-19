/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A contact editor strategy for the new own contact creation
*
*/

// INCLUDE FILES
#include "cpbk2contacteditornewowncontact.h"

// Phonebook 2
#include <TPbk2ContactEditorParams.h>
#include <CPbk2PresentationContact.h>
#include "CPbk2InputAbsorber.h"
#include <Pbk2Debug.h>
#include <csxhelp/phob.hlp.hrh>
#include <Pbk2UID.h>

// Virtual Phonebook
#include <MVPbkStoreContact.h>

// System includes
#include <avkon.rsg>


#ifdef _DEBUG

/// Unnamed namespace
namespace {
enum TPanicCode
    {
    EContactOperationCompleted_PreCond,
    EContactOperationFailed_PreCond,
    EDeleteContactL_PreCond
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "EditorNewOwnContact");
    User::Panic(KPanicText, aReason);
    }
} /// namespace

#endif // _DEBUG

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::CPbk2ContactEditorNewOwnContact
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorNewOwnContact::CPbk2ContactEditorNewOwnContact(
    CPbk2PresentationContact* aContact ) :
        CActive( CActive::EPriorityStandard ),
        iContact( aContact )
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::~CPbk2ContactEditorNewOwnContact
// --------------------------------------------------------------------------
//
CPbk2ContactEditorNewOwnContact::~CPbk2ContactEditorNewOwnContact()
    {
    Cancel();
    delete iIdleDelete;
    delete iInputAbsorber;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorNewOwnContact* CPbk2ContactEditorNewOwnContact::NewL(
    TPbk2ContactEditorParams& /*aParams*/,
    CPbk2PresentationContact* aContact )
    {
    CPbk2ContactEditorNewOwnContact* self =
        new(ELeave) CPbk2ContactEditorNewOwnContact(aContact);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorNewOwnContact::ConstructL()
    {
    iContact->AddSupportedTemplateFieldsL();
    iInputAbsorber = CPbk2InputAbsorber::NewL(R_AVKON_SOFTKEYS_OPTIONS_DONE);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::RunL()
    {
    MPbk2ContactEditorEventObserver* observer = iObserver;
    iObserver = NULL;
    observer->ContactEditorOperationCompleted(iLatestResult, iEditorParams);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::SaveContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::SaveContactL(
    MPbk2ContactEditorEventObserver& aObserver,
    MPbk2ContactEditorEventObserver::TParams& aParams)
    {
    // If observer is set there is process ongoing.
    // Might happen when user is quicker than should be.
    if ( !iObserver )
        {
        iObserver = &aObserver;
        iEditorParams = aParams;
        iContact->CommitL(*this);
        iInputAbsorber->Wait();        
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::DeleteContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::DeleteContactL(
    MPbk2ContactEditorEventObserver& aObserver,
	MPbk2ContactEditorEventObserver::TParams& aParams)
    {
    __ASSERT_DEBUG(!iObserver, Panic(EDeleteContactL_PreCond));

    if (!iIdleDelete)
        {
        iIdleDelete = CIdle::NewL(CActive::EPriorityStandard);
        }
    else
        {
        iIdleDelete->Cancel();
        }

    iObserver = &aObserver;
    iEditorParams = aParams;
    iIdleDelete->Start(TCallBack(&IdleDeleteCallBack, this));
    iInputAbsorber->Wait();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::DefaultTitle
// --------------------------------------------------------------------------
//
const TDesC& CPbk2ContactEditorNewOwnContact::DefaultTitle() const
    {
    // not used
    return KNullDesC;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::SwitchContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::SwitchContactL(
    CPbk2PresentationContact* aContact)
    {
    iContact = aContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::GetHelpContext
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::GetHelpContext(
    TCoeHelpContext &aContext) const
    {
    aContext.iMajor.iUid = KPbk2UID3;
    aContext.iContext = KHLP_CCA_EDITOR;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::StopQuery
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorNewOwnContact::StopQuery()
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::IdleDeleteCallBack
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorNewOwnContact::IdleDeleteCallBack( TAny* aSelf )
    {
    static_cast<CPbk2ContactEditorNewOwnContact*>(aSelf)->HandleIdleDelete();
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::HandleIdleDelete
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::HandleIdleDelete()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ContactEditorNewOwnContact::HandleIdleDelete(0x%x)" ), this );
    iInputAbsorber->StopWait();   
    // Notify client asynchronously to let Delete Operation complete firstly and
    // then the client will be notified to do the other things in the RunL() function.
    iLatestResult.iExtension = NULL;
    iLatestResult.iStoreContact = NULL;
    iLatestResult.iOpCode = MVPbkContactObserver::EContactDelete;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::ContactOperationCompleted(
    MVPbkContactObserver::TContactOpResult aResult )
    {
    __ASSERT_DEBUG(iObserver &&
        ( aResult.iOpCode == MVPbkContactObserver::EContactCommit ||
          aResult.iOpCode == MVPbkContactObserver::EContactSetOwn ),
        Panic(EContactOperationCompleted_PreCond));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactEditorNewOwnContact::ContactOperationCompleted(0x%x)"),
            this);
    if( aResult.iOpCode == MVPbkContactObserver::EContactCommit )
        {
        // Set contact as own contact
        TRAPD( err, iContact->SetAsOwnL( *this ) );
        if( err )
            {
            ContactOperationFailed( aResult.iOpCode, err, EFalse );
            }
        // store commit result
        iLatestResult = aResult;
        }
    else // MVPbkContactObserver::EContactSetOwn
        {
        iInputAbsorber->StopWait();
    
        // Notify client asynchronously to give some time for the all
        // contacts view to get ready. This might not be needed anymore
        // here in new contact strategy... In edit contact strategy
        // this however is a mandatory move.
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        SetActive();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewOwnContact::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewOwnContact::ContactOperationFailed(
    MVPbkContactObserver::TContactOp aOpCode,
    TInt aErrorCode, TBool aErrorNotified )
    {
    __ASSERT_DEBUG(iObserver, Panic(EContactOperationFailed_PreCond));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactEditorNewOwnContact::ContactOperationFailed(0x%x) aErrorCode(%d)"),
            this, aErrorCode);
    iInputAbsorber->StopWait();
    MPbk2ContactEditorEventObserver* observer = iObserver;
    iObserver = NULL;

    MPbk2ContactEditorEventObserver::TFailParams failParams;
    failParams.iCloseEditor = EFalse;
    failParams.iErrorNotified = aErrorNotified;
    failParams.iErrorCode = KErrNone;

    observer->ContactEditorOperationFailed(aOpCode, aErrorCode, iEditorParams,
        failParams );
    }

//  End of File
