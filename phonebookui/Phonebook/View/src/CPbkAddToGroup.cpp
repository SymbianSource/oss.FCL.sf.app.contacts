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
*       Provides methods for UI control of Phonebook's "Add to group".
*
*/


// INCLUDE FILES
#include "CPbkAddToGroup.h"     // This class
#include <cntitem.h>            // CContactGroup
#include <aknnotewrappers.h>    // AVKON Notes
#include <StringLoader.h>       // StringLoader

#include <PbkView.rsg>  // Phonebook view dll resource IDs
#include "CPbkGroupPopup.h"

#include <CPbkContactEngine.h>  // Phonebook engine

// ==================== LOCAL FUNCTIONS ====================

/// Unnamed namespace for this-file-only helper funtions
namespace {

#ifdef _DEBUG

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicLogic_ExecuteLD = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkAddToGroup");
    User::Panic(KPanicText, aReason);
    }

#endif // _DEBUG

} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkAddToGroup::CPbkAddToGroup()
    {
    }

EXPORT_C CPbkAddToGroup* CPbkAddToGroup::NewL()
    {
    CPbkAddToGroup* self = new(ELeave) CPbkAddToGroup;
    return self;
    }

EXPORT_C TContactItemId CPbkAddToGroup::ExecuteLD
        (CPbkContactEngine& aEngine)
    {
    TBool thisDestroyed = EFalse;
    iDestroyedPtr = &thisDestroyed;
    CleanupStack::PushL(this);

    CPbkGroupPopup::TParams params
        (aEngine.AllGroupsViewL(), 
        R_AVKON_SOFTKEYS_CANCEL,
        R_PBK_SOFTKEYS_ASSIGN_CANCEL);
    params.iPrompt = StringLoader::LoadLC(R_QTN_PHOB_QTL_ADD_TO_GRP);
    params.iEmptyText = &KNullDesC;

    iGroupPopup = CPbkGroupPopup::NewL(params);
    iGroupPopup->ResetWhenDestroyed(&iGroupPopup);
    const TInt result = iGroupPopup->ExecuteLD();
    TContactItemId ret = KNullContactId;
    if (result)
        {
        __ASSERT_DEBUG(!thisDestroyed, Panic(EPanicLogic_ExecuteLD));
        ret = params.iSelectedGroupId;
        }

    CleanupStack::PopAndDestroy();  // params.iPrompt
    if (thisDestroyed)
        {
        CleanupStack::Pop(this);
        }
    else
        {
        CleanupStack::PopAndDestroy(this);
        }
    return ret;
    }

CPbkAddToGroup::~CPbkAddToGroup()
    {
    if (iDestroyedPtr) 
        {
        *iDestroyedPtr = ETrue;
        }
    delete iGroupPopup;
    }

//  End of File  
