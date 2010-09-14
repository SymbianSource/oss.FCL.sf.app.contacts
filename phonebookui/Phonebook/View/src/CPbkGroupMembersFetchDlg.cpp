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
*       Provides methods for Phonebook Group members Fetch API.
*
*/


// INCLUDE FILES
#include "CPbkGroupMembersFetchDlg.h"  // This class
#include <cntview.h>

// PbkView APIs
#include <pbkview.rsg>
#include "CPbkFetchDlg.h"
#include "MPbkFetchCallbacks.h"

// PbkEng APIs
#include <CPbkContactEngine.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_ConstructL,
    EPanicPreCond_ExecuteLD,
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkGroupMembersFetchDlg");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

// CPbkGroupMembersFetchDlg::TParams
EXPORT_C CPbkGroupMembersFetchDlg::TParams::TParams() :
    iGroupId(KNullContactId),
    iMarkedEntries(NULL)
    {
    }

EXPORT_C CPbkGroupMembersFetchDlg::TParams::operator TCleanupItem()
    {
    return TCleanupItem(Cleanup,this);
    }

void CPbkGroupMembersFetchDlg::TParams::Cleanup(TAny* aPtr)
    {
    TParams* self = static_cast<TParams*>(aPtr);
    delete self->iMarkedEntries;
    self->iMarkedEntries = NULL;
    }


// CPbkGroupMembersFetchDlg
inline CPbkGroupMembersFetchDlg::CPbkGroupMembersFetchDlg
        (TParams& aParams, CPbkContactEngine& aPbkEngine) :
    iParams(aParams), iPbkEngine(aPbkEngine)
    {
    // PostCond
    __ASSERT_DEBUG(!iFetchDlg && !iDestroyedPtr,
        Panic(EPanicPostCond_Constructor));
    }

inline void CPbkGroupMembersFetchDlg::ConstructL()
    {
    // PreCond
    __ASSERT_DEBUG
        (!iFetchDlg && iParams.iGroupId != KNullContactId && !iGroupNonMembersSubView,
        Panic(EPanicPreCond_ConstructL));

    iGroupNonMembersSubView = CContactGroupView::NewL
        (iPbkEngine.Database(), iPbkEngine.AllContactsView(), *this,
        iParams.iGroupId, CContactGroupView::EShowContactsNotInGroup);

    iGroupNonMembersSubView->SetViewFindConfigPlugin
        (iPbkEngine.AllContactsView().GetViewFindConfigPlugin());
    }

EXPORT_C CPbkGroupMembersFetchDlg* CPbkGroupMembersFetchDlg::NewL(TParams& aParams, CPbkContactEngine& aPbkEngine)
    {
    CPbkGroupMembersFetchDlg* self = new(ELeave) CPbkGroupMembersFetchDlg(aParams, aPbkEngine);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C TInt CPbkGroupMembersFetchDlg::ExecuteLD()
    {
    // PreCond
    __ASSERT_DEBUG(iParams.iGroupId != KNullContactId && !iFetchDlg,
        Panic(EPanicPreCond_ExecuteLD));

    // "D" function semantics
    CleanupStack::PushL(this);
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;

    // Convert aParams for CPbkFetchEntryDlg
    CPbkFetchDlg::TParams params;
    params.iResId = R_PBK_GROUP_MEMBER_FETCH_DLG;
    params.iFlags = CPbkFetchDlg::FETCH_MARKED;
    params.iContactView = iGroupNonMembersSubView;

    // Run CPbkFetchEntryDlg dialog
    iFetchDlg = CPbkFetchDlg::NewL(params, iPbkEngine);
    iFetchDlg->ResetWhenDestroyed(&iFetchDlg);
    TInt result = iFetchDlg->ExecuteLD();

    if (thisDestroyed)
        {
        // This object has been destroyed
        result = 0;
        CleanupStack::Pop(this);
        }
    else
        {
        // Convert output back to our format
        if (result)
            {
            iParams.iMarkedEntries = params.iMarkedEntries;
            params.iMarkedEntries = NULL;
            }
        CleanupStack::PopAndDestroy(this);
        }

    return result;
    }

CPbkGroupMembersFetchDlg::~CPbkGroupMembersFetchDlg()
    {
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }
    if (iGroupNonMembersSubView)
        {
        iGroupNonMembersSubView->Close(*this);
        }
    delete iFetchDlg;
    }

void CPbkGroupMembersFetchDlg::HandleContactViewEvent
        (const CContactViewBase& /*aView*/,
        const TContactViewEvent& /*aEvent*/)
    {
    }


//  End of File
