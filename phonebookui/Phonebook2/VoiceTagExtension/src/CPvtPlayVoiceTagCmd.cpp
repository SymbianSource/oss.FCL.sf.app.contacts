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
* Description:  Play voice tag command object.
*
*/


// INCLUDES
#include "cpvtplayvoicetagcmd.h"

// Phonebook 2
#include "cpvtvoicedialhandler.h"
#include <mpbk2contactuicontrol.h>
#include <mpbk2commandobserver.h>
#include <mpbk2applicationservices.h>
#include <mpbk2appui.h>
#include <pbk2voicetaguires.rsg>

// Virtual Phonebook
#include <mvpbkbasecontactfield.h>
#include <mvpbkstorecontact.h>
#include <mvpbkcontactattributemanager.h>
#include <cvpbkvoicetagattribute.h>
#include <cvpbkcontactmanager.h>
#include <mvpbkcontactoperationbase.h>

// System includes
#include <stringloader.h>
#include <aknnotewrappers.h>

// Debugging headers
#include <pbk2debug.h>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanic
    {
    EPanicLogic_ExecuteLD,
    EPanic_IncorrectState
    };

void Panic(TPanic aPanic)
    {
    _LIT(KPanicCat, "CPvtPlayVoiceTagCmd");
    User::Panic(KPanicCat, aPanic);
    }

#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::CPvtPlayVoiceTagCmd
// --------------------------------------------------------------------------
//
CPvtPlayVoiceTagCmd::CPvtPlayVoiceTagCmd() :
        CActive( CActive::EPriorityIdle )
    {
    CActiveScheduler::Add( this );
    }


// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::~CPvtPlayVoiceTagCmd
// --------------------------------------------------------------------------
//
CPvtPlayVoiceTagCmd::~CPvtPlayVoiceTagCmd()
    {
    Cancel();
    delete iResult;
    delete iOperation;
    delete iVoiceDialHandler;
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::NewL
// --------------------------------------------------------------------------
//
CPvtPlayVoiceTagCmd* CPvtPlayVoiceTagCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPvtPlayVoiceTagCmd* self = new ( ELeave ) CPvtPlayVoiceTagCmd;
    CleanupStack::PushL( self );
    self->ConstructL(aUiControl);
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::ConstructL(
        MPbk2ContactUiControl& aUiControl)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::ConstructL"));
    iVoiceDialHandler = CPvtVoiceDialHandler::NewL();

    iContact = aUiControl.FocusedStoreContact();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::RunL
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::RunL()
    {
    switch( iState )
        {
        case ECommandInitiliaze:
            {
            FindVoiceTagFieldsL(*iContact);
            break;
            }
        case EPlayVoiceTagCmd:
            {
            PlayVoiceTagCmdL();
            break;
            }
        case ECommandFinished:
            {
            FinishProcess();
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic( EPanic_IncorrectState ) );
            break;
            }
        };
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::DoCancel()
    {
    delete iOperation;
    iOperation = NULL;
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPvtPlayVoiceTagCmd::RunError( TInt /*aError*/ )
    {
    FinishProcess();
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::ExecuteLD()
    {
    CleanupStack::PushL(this);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::ExecuteLD"));

    __ASSERT_DEBUG( iContact, Panic( EPanicLogic_ExecuteLD ) );

    iState = ECommandInitiliaze;
    IssueRequest();

    CleanupStack::Pop(this);
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::ExecuteLD end"));
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    iObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::ResetUiControl(
        MPbk2ContactUiControl& /*aUiControl*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::AttributePresenceOperationComplete
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::AttributePresenceOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContactFieldCollection* aResult )
    {
    if ( iOperation == &aOperation )
        {
        delete iOperation;
        iOperation = NULL;

        if ( iResult )
            {
            delete iResult;
            iResult = NULL;
            }

        iResult = aResult;

        iState = EPlayVoiceTagCmd;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::AttributePresenceOperationFailed
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::AttributePresenceOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/,
        TInt /*aError*/ )
    {
    delete iOperation;
    iOperation = NULL;

    iState = ECommandFinished;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::FindVoiceTagFieldsL
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::FindVoiceTagFieldsL(
        const MVPbkStoreContact& aContact )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::FindVoiceTagFieldIndexL"));

    TInt index(KErrNotFound);
    MVPbkContactAttributeManager& attrManager =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            ContactAttributeManagerL();

    iOperation = attrManager.FindFieldsWithAttributeL(
                    CVPbkVoiceTagAttribute::Uid(),
                    *const_cast<MVPbkStoreContact*>(&aContact),
                    *this );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::FindVoiceTagFieldIndexL end (%d)"), index);
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::PlayVoiceTagCmdL
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::PlayVoiceTagCmdL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::PlayVoiceTagCmdL, PlaybackVoiceTagL"));

    if ( iResult && iResult->FieldCount() > 0 )
        {
        MVPbkStoreContactField& taggedField = iResult->FieldAt(0);

        TInt index( KErrNotFound );
        TInt fieldCount(iContact->Fields().FieldCount());
        for (TInt i = 0; i < fieldCount; ++i)
            {
            const MVPbkStoreContactField& field = iContact->Fields().FieldAt(i);
            if ( taggedField.IsSame( field ) )
                {
                index = i;
                break;
                }
            }
        if ( index != KErrNotFound  )
            {
            MVPbkContactLink* link = iContact->CreateLinkLC();
            iVoiceDialHandler->PlaybackVoiceTagL(*link, index);
            CleanupStack::PopAndDestroy(); // link
            }
        }
    else
        {
        // No voice tag exists, show a note
        ShowNoVoiceTagNoteL();
        }

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPvtPlayVoiceTagCmd::PlayVoiceTagCmdL, PlaybackVoiceTagL end"));

    iState = ECommandFinished;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::FinishProcess
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::FinishProcess()
    {
    if (iObserver)
        {
        iObserver->CommandFinished( *this );
        }
    }

// --------------------------------------------------------------------------
// CPvtPlayVoiceTagCmd::ShowNoVoiceTagNoteL
// --------------------------------------------------------------------------
//
void CPvtPlayVoiceTagCmd::ShowNoVoiceTagNoteL()
    {
    HBufC* prompt = StringLoader::LoadLC( R_QTN_PHOB_PLAY_NO_TAG );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( ETrue );
    dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy( prompt );
    }
    
// End of File
