/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A contact editor strategy for the new contact creation
*
*/



// INCLUDE FILES
#include "CPbk2ContactEditorNewContact.h"

// Phonebook 2
#include <pbk2uicontrols.rsg>
#include <TPbk2ContactEditorParams.h>
#include <CPbk2PresentationContact.h>
#include "CPbk2InputAbsorber.h"
#include <Pbk2Debug.h>
#include <csxhelp/phob.hlp.hrh>
#include <Pbk2UID.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkFieldType.h>

// System includes
#include <avkon.rsg>
#include <StringLoader.h>


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
    _LIT(KPanicText, "EditorNewContact");
    User::Panic(KPanicText, aReason);
    }
} /// namespace

#endif // _DEBUG

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::CPbk2ContactEditorNewContact
// --------------------------------------------------------------------------
//
inline CPbk2ContactEditorNewContact::CPbk2ContactEditorNewContact(
        CPbk2PresentationContact* aContact) :
            CActive(CActive::EPriorityStandard),
            iContact(aContact)
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::~CPbk2ContactEditorNewContact
// --------------------------------------------------------------------------
//
CPbk2ContactEditorNewContact::~CPbk2ContactEditorNewContact()
    {
    Cancel();
    delete iDefaultTitle;
    delete iIdleDelete;
    delete iInputAbsorber;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactEditorNewContact* CPbk2ContactEditorNewContact::NewL
        ( TPbk2ContactEditorParams& /*aParams*/,
          CPbk2PresentationContact* aContact )
    {
    CPbk2ContactEditorNewContact* self =
        new(ELeave) CPbk2ContactEditorNewContact(aContact);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactEditorNewContact::ConstructL()
    {
    iContact->AddSupportedTemplateFieldsL();
    iInputAbsorber = CPbk2InputAbsorber::NewL(R_AVKON_SOFTKEYS_OPTIONS_DONE);
    iDefaultTitle = StringLoader::LoadL(R_QTN_PHOB_TITLE_NEW_ENTRY);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::RunL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::RunL()
    {
    MPbk2ContactEditorEventObserver* observer = iObserver;
    iObserver = NULL;
    observer->ContactEditorOperationCompleted(iLatestResult, iEditorParams);
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::DoCancel()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::SaveContactL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorNewContact::RunError(TInt /*aError*/)
    {
    // No leaving code in RunL
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::SaveContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::SaveContactL(
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
// CPbk2ContactEditorNewContact::DeleteContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::DeleteContactL(
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
// CPbk2ContactEditorNewContact::DefaultTitle
// --------------------------------------------------------------------------
//
const TDesC& CPbk2ContactEditorNewContact::DefaultTitle() const
    {
    return *iDefaultTitle;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::SwitchContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::SwitchContactL
        (CPbk2PresentationContact* aContact)
    {
    iContact = aContact;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::IdleDeleteCallBack
// --------------------------------------------------------------------------
//
TInt CPbk2ContactEditorNewContact::IdleDeleteCallBack(TAny* aSelf)
    {
    static_cast<CPbk2ContactEditorNewContact*>(aSelf)->HandleIdleDelete();
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::HandleIdleDelete
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::HandleIdleDelete()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING
        ( "CPbk2ContactEditorNewContact::HandleIdleDelete(0x%x)" ), this );

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
// CPbk2ContactEditorNewContact::ContactOperationCompleted
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::ContactOperationCompleted(
        MVPbkContactObserver::TContactOpResult aResult )
    {
    __ASSERT_DEBUG(iObserver &&
        aResult.iOpCode == MVPbkContactObserver::EContactCommit,
        Panic(EContactOperationCompleted_PreCond));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactEditorNewContact::ContactOperationCompleted(0x%x)"),
            this);

    iInputAbsorber->StopWait();
    iLatestResult = aResult;

    // Notify client asynchronously to give some time for the all
    // contacts view to get ready. This might not be needed anymore
    // here in new contact strategy... In edit contact strategy
    // this however is a mandatory move.
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::ContactOperationFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::ContactOperationFailed
        ( MVPbkContactObserver::TContactOp aOpCode,
          TInt aErrorCode, TBool aErrorNotified )
    {
    __ASSERT_DEBUG(iObserver, Panic(EContactOperationFailed_PreCond));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactEditorNewContact::ContactOperationFailed(0x%x) aErrorCode(%d)"),
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

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::GetHelpContext
// --------------------------------------------------------------------------
//
void CPbk2ContactEditorNewContact::GetHelpContext(TCoeHelpContext &aContext) const
    {
    aContext.iMajor.iUid = KPbk2UID3;
    aContext.iContext = KHLP_CCA_EDITOR;
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorNewContact::StopQuery
// --------------------------------------------------------------------------
//
TBool CPbk2ContactEditorNewContact::StopQuery()
    {
    return EFalse;
    }

//  End of File
