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
* Description:  A command for launching phonebook info dialog
*
*/



// INCLUDE FILES
#include "CPbk2InfoDialogCmd.h"

// Phonebook 2
#include <pbk2commands.rsg>
#include <MPbk2CommandObserver.h>
#include <CPbk2StoreListQuery.h>
#include <CPbk2PhonebookInfoDlg.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2StoreValidityInformer.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactStoreUriArray.h>

// System includes
#include <StringLoader.h>
#include <avkon.hrh>

/// Unnamed namespace for local definitions
namespace {

const TInt KOneValidStore = 1;

enum TPbk2InfoDialogCmdState
    {
    EQuery,
    ELaunchDialog,
    EComplete
    };
   
} /// namespace

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::CPbk2InfoDialogCmd
// --------------------------------------------------------------------------
//
CPbk2InfoDialogCmd::CPbk2InfoDialogCmd
        ( MPbk2ContactUiControl& /*aUiControl*/ ) :
            CActive( EPriorityStandard )
    {
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::~CPbk2InfoDialogCmd
// --------------------------------------------------------------------------
//
CPbk2InfoDialogCmd::~CPbk2InfoDialogCmd()
    {
    Cancel();
    delete iValidStores;
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::NewL
// --------------------------------------------------------------------------
//
CPbk2InfoDialogCmd* CPbk2InfoDialogCmd::NewL(
        MPbk2ContactUiControl& aUiControl)
    {
    CPbk2InfoDialogCmd* self = 
        new( ELeave ) CPbk2InfoDialogCmd(aUiControl);    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::ConstructL()
    {
    CActiveScheduler::Add(this);
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::ExecuteLD
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::ExecuteLD()
    {
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::AddObserver
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::AddObserver(MPbk2CommandObserver& aObserver)
    {
    iCommandObserver = &aObserver;
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::ResetUiControl
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::ResetUiControl(
        MPbk2ContactUiControl& /*aUiControl*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::RunL
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::RunL()
    {
    switch (iState)
        {
        case EQuery:
            {
            QueryL();
            break;
            } 
        case ELaunchDialog:
            {
            LaunchDialogL();
            break;
            }
        case EComplete: // FALLTHROUGH
        default:
            {
            iCommandObserver->CommandFinished(*this);
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::DoCancel
// --------------------------------------------------------------------------
//        
void CPbk2InfoDialogCmd::DoCancel()
    {
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::RunError
// --------------------------------------------------------------------------
//
TInt CPbk2InfoDialogCmd::RunError(TInt aError)
    {
    HandleError(aError);
    return KErrNone;
    }
    
// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::IssueRequest
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::IssueRequest()
    {
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::HandleError
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::QueryL()
    {
    iState = EComplete;
    
    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();

    iValidStores = appServices->StoreValidityInformer().CurrentlyValidStoresL();
    const CPbk2StorePropertyArray& props = appServices->StoreProperties();

    CleanupStack::PopAndDestroy(); // appServices

    TInt count = iValidStores->Count();
    // Remove stores that hasn't define memory info types
    for (TInt i = count - 1; i >= 0; --i)
        {
        const CPbk2StoreProperty* prop =
            props.FindProperty((*iValidStores)[i]);
        if (!prop || (prop && prop->MemoryInfoTypes() == 0))
            {
            iValidStores->Remove((*iValidStores)[i]);
            }
        }
        
    if (iValidStores->Count() > KOneValidStore)
        {
        HBufC* title = StringLoader::LoadLC
            (R_QTN_PHOB_QUERY_TITLE_SELECT_MEM);
        CPbk2StoreListQuery* query = CPbk2StoreListQuery::NewL();
        CleanupStack::PushL(query);
        HBufC* allText = StringLoader::LoadLC
            (R_QTN_PHOB_QUERY_SELECT_MEM_ALL);
        query->SetAllItemTextL(*allText, ETrue);
        CleanupStack::PopAndDestroy(allText);
        CleanupStack::Pop(query);
        
        // Query modifies iTargetStoreUris -array.
        // After the query the must be only one, the selected, store
        if (query->ExecuteLD(*title, *iValidStores) == EAknSoftkeyOk)
            {
            iState = ELaunchDialog;
            }
        CleanupStack::PopAndDestroy(title);
        }
    else
        {
        iState = ELaunchDialog;
        }
    IssueRequest();
    }

// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::LaunchDialogL
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::LaunchDialogL()
    {
    CPbk2PhonebookInfoDlg* dlg = CPbk2PhonebookInfoDlg::NewL();
    dlg->ExecuteLD(*iValidStores);
    iState = EComplete;
    IssueRequest();
    }
    
// --------------------------------------------------------------------------
// CPbk2InfoDialogCmd::HandleError
// --------------------------------------------------------------------------
//
void CPbk2InfoDialogCmd::HandleError(TInt aError)
    {
    if (aError != KErrNone)
        {
        iCommandObserver->CommandFinished(*this);
        CCoeEnv::Static()->HandleError(aError);
        }
    }
    
//  End of File  
