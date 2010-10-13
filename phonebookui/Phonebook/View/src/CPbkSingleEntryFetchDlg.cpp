/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Provides methods for Phonebook Single Entry Fetch dialog API.
*
*/


// INCLUDE FILES
#include "CPbkSingleEntryFetchDlg.h"  // This class
#include <PbkView.rsg>
#include "CPbkFetchDlg.h"
#include <CPbkContactEngine.h>

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPreCond_ExecuteLD,
    EPanicPreCond_ResetWhenDestroyed,
    EPanicPostCond_ConstructL
    };

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkSingleEntryFetchDlg::TParams::TParams() :
    iPbkEngine(NULL),
    iContactView(NULL),
    iFetchKeyCallback(NULL),
    iFetchDlgAccept(NULL),
    iSelectedEntry(KNullContactId),
    iCbaId(0)
    {
    }

inline CPbkSingleEntryFetchDlg::CPbkSingleEntryFetchDlg(TParams& aParams) :
    iParams(aParams), 
    iPbkEngine(iParams.iPbkEngine),
    iContactView(iParams.iContactView)
    {
    // PostCond
    __ASSERT_DEBUG(!iFetchDlg && !iDestroyedPtr, 
        Panic(EPanicPostCond_Constructor));
    }

EXPORT_C CPbkSingleEntryFetchDlg* CPbkSingleEntryFetchDlg::NewL(TParams& aParams)
    {
    CPbkSingleEntryFetchDlg* self = new(ELeave) CPbkSingleEntryFetchDlg(aParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C void CPbkSingleEntryFetchDlg::SetMopParent(MObjectProvider* aParent)
    {
    iObjectProvider = aParent;
    }

EXPORT_C void CPbkSingleEntryFetchDlg::ResetWhenDestroyed
        (CPbkSingleEntryFetchDlg** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, 
            Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

EXPORT_C TInt CPbkSingleEntryFetchDlg::ExecuteLD()
    {
    // PreCond
    __ASSERT_DEBUG(iContactView && !iFetchDlg && iPbkEngine, 
            Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    TBool thisDestroyed = EFalse;
    // Destructor sets thisDestroyed to ETrue
    iDestroyedPtr = &thisDestroyed;

    // Convert iParams for input to CPbkFetchDlg
    CPbkFetchDlg::TParams params;
    params.iResId = R_PBK_SINGLE_ENTRY_FETCH_DLG;
    params.iFlags = CPbkFetchDlg::FETCH_FOCUSED;
    params.iContactView = iContactView;
    params.iFocusedEntry = iParams.iSelectedEntry;
    params.iKeyCallback = iParams.iFetchKeyCallback;
    params.iAcceptCallback = iParams.iFetchDlgAccept;
    params.iCbaId = iParams.iCbaId;

    iFetchDlg = CPbkFetchDlg::NewL(params, *iPbkEngine);
    iFetchDlg->SetMopParent(iObjectProvider);
    iFetchDlg->ResetWhenDestroyed(&iFetchDlg);
    TInt result = 0;
    result = iFetchDlg->ExecuteLD();
    
    if (thisDestroyed)
        {
        CleanupStack::Pop(this);
        }
    else
        {
        if (result && params.iFocusedEntry != KNullContactId)
            {
            // Convert params back to our format
            iParams.iSelectedEntry = params.iFocusedEntry;
            }
        CleanupStack::PopAndDestroy(this);
        }

    return result;
    }

CPbkSingleEntryFetchDlg::~CPbkSingleEntryFetchDlg()
    {
    if (iSelfPtr) 
        {
        *iSelfPtr = NULL;
        }
    if (iDestroyedPtr) 
        {
        *iDestroyedPtr = ETrue;
        }
    // Close the dialog
    delete iFetchDlg;
    // Delete engine if this object owns one
    delete iOwnPbkEngine;
    }

void CPbkSingleEntryFetchDlg::ConstructL()
    {
    // PreCond
    __ASSERT_DEBUG(!iFetchDlg && !iDestroyedPtr && !iOwnPbkEngine, 
        Panic(EPanicPreCond_ConstructL));

    if (!iPbkEngine)
        {
        CPbkContactEngine* engine = CPbkContactEngine::Static();
        if (engine)
            {
            iPbkEngine = engine;
            }
        else
            {
            iOwnPbkEngine = CPbkContactEngine::NewL();
            iPbkEngine = iOwnPbkEngine;
            }
        }

    // Set up contact view if needed
    if (!iContactView)
        {
        iContactView = &iPbkEngine->AllContactsView();
        }

    // PostCond
    __ASSERT_DEBUG(iPbkEngine, Panic(EPanicPostCond_ConstructL));
    }

void CPbkSingleEntryFetchDlg::Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkSingleEntryFetchDlg");
    User::Panic(KPanicText, aReason);
    }

//  End of File  
