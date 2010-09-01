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
* Description:  Phonebook 2 go to URL command object.
*
*/


#include "CPbk2GoToURLCmd.h"

// Phonebook 2
#include <Pbk2ProcessDecoratorFactory.h>
#include <MPbk2CommandObserver.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactNameFormatter.h>
#include <TPbk2AddressSelectParams.h>
#include <CPbk2AddressSelect.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <Pbk2UIControls.rsg>
#include <CPbk2FieldPropertyArray.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactFieldTextData.h>

// System includes
#include <browserlauncher.h>
#include <coemain.h>
#include <barsread.h>
#include <StringLoader.h>
#include <aknnotewrappers.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KProtocolForURL, "4 " ); // The protocal format

#ifdef _DEBUG
enum TPanicCode
    {
    ERunL_InvalidState = 1,
    EDoLaunchBrowserL_NullURL,
    EResolveUrlL_NullField,
    EPanicPreCond_Null_Pointer
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2GoToURLCmd");
    User::Panic(KPanicText,aReason);
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::CPbk2GoToURLCmd
// --------------------------------------------------------------------------
//
CPbk2GoToURLCmd::CPbk2GoToURLCmd( MPbk2ContactUiControl& aUiControl ) :
        CActive( EPriorityIdle ), iUiControl( &aUiControl )
    {
    CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::~CPbk2GoToURLCmd
// --------------------------------------------------------------------------
//
CPbk2GoToURLCmd::~CPbk2GoToURLCmd()
    {
    Cancel();

    delete iURL;
    delete iContactLink;
    delete iRetrieveOperation;
    delete iStoreContact;
    delete iBrowserLauncher;
    Release( iAppServices );
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2GoToURLCmd* CPbk2GoToURLCmd::NewL
        ( MPbk2ContactUiControl& aUiControl )
    {
    CPbk2GoToURLCmd* self = new ( ELeave ) CPbk2GoToURLCmd( aUiControl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2GoToURLCmd::ConstructL()
    {
    iAppServices = CPbk2ApplicationServices::InstanceL();

    iContactLink = iUiControl->FocusedContactL()->CreateLinkLC();
    CleanupStack::Pop(); // iContactLink

    iFieldPropertyArray = &(iAppServices->FieldProperties());
    iBrowserLauncher = CBrowserLauncher::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::DoCancel
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::DoCancel()
    {
    iBrowserLauncher->Cancel();
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::RunL()
    {
    switch (iState)
        {
        case ERetrieving:
            {
            RetrieveContactL( *iContactLink );
            break;
            }

        case EResolveURL:
            {
            ResolveUrlL();
            break;
            }

        case EStarting:
            {
            DoLaunchBrowserL();
            break;
            }

        case ELaunching:
            {
            // Do nothing
            break;
            }

        case EStopping:
            {
            ProcessDismissed(KErrNone);
            break;
            }

        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(ERunL_InvalidState));
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2GoToURLCmd::RunError( TInt aError )
    {
    return aError;
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::ExecuteLD()
    {
    iState = ERetrieving;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::AddObserver( MPbk2CommandObserver& aObserver )
    {
    this->iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::ResetUiControl( MPbk2ContactUiControl& aUiControl )
    {
    if (iUiControl == &aUiControl)
        {
        iUiControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::ProcessDismissed
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::ProcessDismissed( TInt /*aCancelCode*/ )
    {
    __ASSERT_DEBUG(iCommandObserver, Panic(EPanicPreCond_Null_Pointer));
    
    if (iUiControl)
        {
        iUiControl->UpdateAfterCommandExecution();
        }

    // Notify command owner that the command has finished
    iCommandObserver->CommandFinished(*this);
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::VPbkSingleContactOperationComplete
// Called when retrieve operation completes.
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::VPbkSingleContactOperationComplete
        ( MVPbkContactOperationBase& aOperation,
          MVPbkStoreContact* aContact )
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We now have a store contact
        iStoreContact = aContact;
        iState = EResolveURL;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::VPbkSingleContactOperationFailed
// Called when retrieve operation fails.
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::VPbkSingleContactOperationFailed
        ( MVPbkContactOperationBase& aOperation, TInt aError )
    {
    if (&aOperation == iRetrieveOperation)
        {
        delete iRetrieveOperation;
        iRetrieveOperation = NULL;

        // We cannot get the contact, so we have to
        // fail. We cannot continue, since this operation
        // was executed only in case of one contact.
        ProcessDismissed(aError);
        }
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::HandleServerAppExit
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::HandleServerAppExit( TInt /*aReason*/ )
    {
    IssueStopRequest();
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::RetrieveContactL
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::RetrieveContactL
        ( const MVPbkContactLink& aContactLink )
    {
    // Retrieve the actual store contact from the given link
    iRetrieveOperation = iAppServices->
        ContactManager().RetrieveContactL( aContactLink, *this );
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::DoLaunchBrowserL
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::DoLaunchBrowserL()
    {
    __ASSERT_DEBUG(iURL,Panic(EDoLaunchBrowserL_NullURL));

    iBrowserLauncher->LaunchBrowserEmbeddedL
        ( *iURL, NULL, this );

    iState = ELaunching;
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::ResolveUrlL
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::ResolveUrlL()
    {
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, R_PBK2_URL_ADDRESS_SELECT);

    TPbk2AddressSelectParams params(
        *iStoreContact,
        iAppServices->ContactManager(),
        iAppServices->NameFormatter(),
        *iFieldPropertyArray,
        resReader);

    CPbk2AddressSelect* addressSelect = CPbk2AddressSelect::NewL(params);
    MVPbkStoreContactField* selectedField = addressSelect->ExecuteLD();

    CleanupStack::PopAndDestroy(); // resReader

    // No field selected. Stop command process.
    if (!selectedField)
        {
        IssueStopRequest();
        }
    else
        {
        CleanupDeletePushL(selectedField);
        SetURLFromFieldL(*selectedField);
        CleanupStack::PopAndDestroy(); // selectedField

        iState = EStarting;
        IssueRequest();
        }
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::SetURLFromFieldL
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::SetURLFromFieldL( MVPbkStoreContactField& aField )
    {
    TPtrC url = MVPbkContactFieldTextData::Cast(aField.FieldData()).Text();
    delete iURL;
    iURL = NULL;
    // For explanation of the protocal format
    // refer to Browser API documentation
    iURL = HBufC::NewL(KProtocolForURL().Length() + url.Length());
    TPtr urlPtr = iURL->Des();
    urlPtr.Append(KProtocolForURL);
    urlPtr.Append(url);
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2GoToURLCmd::IssueStopRequest
// --------------------------------------------------------------------------
//
void CPbk2GoToURLCmd::IssueStopRequest()
    {
    iState = EStopping;
    if (!IsActive())
        {
        IssueRequest();
        }
    }

// End of File
