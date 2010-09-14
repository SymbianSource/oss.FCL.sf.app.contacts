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
*       Provides methods for Phonebook Multiple Entry Fetch API.
*
*/


// INCLUDE FILES
#include "CPbkMultipleEntryFetchDlg.h"  // This class
#include <pbkview.rsg>
#include "CPbkFetchDlg.h"
#include "MPbkFetchDlgSelection.h"
#include <CPbkContactEngine.h>

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPreCond_ExecuteLD
    };

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkMultipleEntryFetchDlg::TParams::TParams() :
    iContactView(NULL),
    iMarkedEntries(NULL),
    iCbaId(0),        
    iFetchSelection(NULL)    
    {
    }

EXPORT_C CPbkMultipleEntryFetchDlg::TParams::operator TCleanupItem()
    {
    return TCleanupItem(Cleanup,this);
    }

void CPbkMultipleEntryFetchDlg::TParams::Cleanup(TAny* aPtr)
    {
    TParams* self = static_cast<TParams*>(aPtr);
    delete self->iMarkedEntries;
    self->iMarkedEntries = NULL;
    }

inline CPbkMultipleEntryFetchDlg::CPbkMultipleEntryFetchDlg
        (TParams& aParams, CPbkContactEngine& aEngine) :
    iParams(aParams), iPbkEngine(aEngine)
    {
    // PostCond
    __ASSERT_DEBUG(!iFetchDlg && iParams.iContactView, 
            Panic(EPanicPostCond_Constructor));
    }

inline void CPbkMultipleEntryFetchDlg::ConstructL()
    {
    // PreCond
    __ASSERT_DEBUG(!iFetchDlg && iParams.iContactView, 
            Panic(EPanicPreCond_ConstructL));
    }

EXPORT_C CPbkMultipleEntryFetchDlg* CPbkMultipleEntryFetchDlg::NewL
        (TParams& aParams, 
        CPbkContactEngine& aEngine)
    {
    CPbkMultipleEntryFetchDlg* self = new(ELeave) CPbkMultipleEntryFetchDlg(aParams, aEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C void CPbkMultipleEntryFetchDlg::SetMopParent(MObjectProvider* aParent)
    {
    iObjectProvider = aParent;
    }

EXPORT_C TInt CPbkMultipleEntryFetchDlg::ExecuteLD()
    {
    // PreCond
    __ASSERT_DEBUG(iParams.iContactView && !iFetchDlg, 
            Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    // Convert iParams for CPbkFetchEntryDlg
    CPbkFetchDlg::TParams params;
    params.iResId = R_PBK_MULTIPLE_ENTRY_FETCH_DLG;
    params.iFlags = CPbkFetchDlg::FETCH_MARKED;
    params.iContactView = iParams.iContactView;
    params.iFetchSelection = iParams.iFetchSelection;
    params.iCbaId = iParams.iCbaId;

    // Run CPbkFetchEntryDlg dialog
    iFetchDlg = CPbkFetchDlg::NewL(params, iPbkEngine);
    iFetchDlg->ResetWhenDestroyed(&iFetchDlg);
    iFetchDlg->SetMopParent(iObjectProvider);
    TInt result = iFetchDlg->ExecuteLD();

    if (thisDestroyed)
        {
        // this object is destroyed
        result = 0;
        CleanupStack::Pop(this);
        }
    else
        {
        // Convert params back to our format
        if (result)
            {
            iParams.iMarkedEntries = params.iMarkedEntries;
            params.iMarkedEntries = NULL;
            }
        CleanupStack::PopAndDestroy(this);
        }

    return result;
    }

CPbkMultipleEntryFetchDlg::~CPbkMultipleEntryFetchDlg()
    {
    if (iDestroyedPtr) 
        {
        *iDestroyedPtr = ETrue;
        }
    // Close the dialog
    delete iFetchDlg;
    }

void CPbkMultipleEntryFetchDlg::Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkMultipleEntryFetchDlg");
    User::Panic(KPanicText, aReason);
    }

//  End of File
