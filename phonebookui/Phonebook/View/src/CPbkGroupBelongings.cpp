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
*       Provides methods for UI control for Phonebook's "Group belongings".
*
*/


// INCLUDE FILES
#include "CPbkGroupBelongings.h"  // This class
#include <cntdb.h>      // CContactDatabase
#include <cntitem.h>    // CContactGroup
#include <avkon.hrh>
#include <avkon.rsg>
#include <StringLoader.h>

#include <pbkview.rsg>
#include "CPbkGroupPopup.h"
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkContactSubView.h>


// ==================== LOCAL FUNCTIONS ====================

namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_CreateGroupSubViewL,
    EPanicPreCond_RunPopupL,
    EPanicPreCond_IsContactIncluded
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkGroupBelongings");
    User::Panic(KPanicText, aReason);
    }
#endif

}


// ================= MEMBER FUNCTIONS =======================

inline CPbkGroupBelongings::CPbkGroupBelongings()
    {
    __ASSERT_DEBUG(!iGroupPopup && !iDestroyedPtr,
        Panic(EPanicPostCond_Constructor));
    }

EXPORT_C CPbkGroupBelongings* CPbkGroupBelongings::NewL()
    {
    return new (ELeave) CPbkGroupBelongings;
    }

void CPbkGroupBelongings::CreateGroupSubViewL
        (CPbkContactEngine& aEngine, TContactItemId aContactId)
    {
    __ASSERT_DEBUG(!iGroupView && !iGroups, Panic(EPanicPreCond_CreateGroupSubViewL));

    CPbkContactItem* contact = aEngine.ReadContactLC(aContactId);
    iGroups = contact->GroupsJoinedLC();
    CleanupStack::Pop(iGroups);
    CleanupStack::PopAndDestroy(contact);
    iContactId = aContactId;
    iGroupView = CPbkContactSubView::NewL
        (*this, aEngine.Database(), aEngine.AllGroupsViewL(), *this);
    }

void CPbkGroupBelongings::RunPopupL()
    {
    __ASSERT_DEBUG(!iGroupPopup, Panic(EPanicPreCond_RunPopupL));

    CPbkGroupPopup::TParams params
        (*iGroupView, 
        R_AVKON_SOFTKEYS_BACK,
        R_AVKON_SOFTKEYS_BACK);
    params.iPrompt = StringLoader::LoadLC(R_QTN_PHOB_QTL_ENTRY_IS_IN_GRP);
    params.iEmptyText = StringLoader::LoadLC(R_QTN_PHOB_ENTRY_IS_IN_NO_GRP);
    params.iListBoxFlags = EAknListBoxViewerFlags;

    iGroupPopup = CPbkGroupPopup::NewL(params);
    iGroupPopup->ResetWhenDestroyed(&iGroupPopup);
    iGroupPopup->ExecuteLD();
    CleanupStack::PopAndDestroy(2);  // params.iPrompt, params.iEmptyText
    }

EXPORT_C void CPbkGroupBelongings::ExecuteLD
        (CPbkContactEngine& aEngine,
        TContactItemId aContactId)
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    CleanupStack::PushL(this);

    CreateGroupSubViewL(aEngine, aContactId);
    RunPopupL();

    if (thisDestroyed)
        {
        CleanupStack::Pop(this);
        }
    else
        {
        CleanupStack::PopAndDestroy(this);
        }
    }

CPbkGroupBelongings::~CPbkGroupBelongings()
    {
    if (iDestroyedPtr) 
        {
        *iDestroyedPtr = ETrue;
        }
    delete iGroupPopup;
    if (iGroupView)
        {
        iGroupView->Close(*this);
        }
    delete iGroups;
    }

void CPbkGroupBelongings::HandleContactViewEvent
        (const CContactViewBase& /*aView*/,
        const TContactViewEvent& /*aEvent*/)
    {
    // iGroupPopup handles all events just fine
    }

TBool CPbkGroupBelongings::IsContactIncluded(TContactItemId aId)
    {
    __ASSERT_DEBUG(iGroups, Panic(EPanicPreCond_IsContactIncluded));
    return (iGroups->Find(aId) != KErrNotFound);
    }


//  End of File  
