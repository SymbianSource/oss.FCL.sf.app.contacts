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
*          Provides phonebook multiple contacts removal from group command methods.
*
*/


// INCLUDE FILES
#include "CPbkRemoveFromGroupCmd.h"
#include <eikprogi.h>
#include <StringLoader.h>
#include <CPbkContactEngine.h>
#include <Phonebook.rsg>
#include <CPbkRemoveFromGroup.h>
#include <CPbkProgressNoteWrapper.h>
#include <CPbkContactViewListControl.h>
#include <MPbkCommandObserver.h>

#include <pbkdebug.h>

/// Unnamed namespace for local definitions
namespace {

// LOCAL DEBUG CODE
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ResetWhenDestroyed = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkRemoveFromGroupCmd");
    User::Panic(KPanicText,aReason);
    }
#endif

} // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkRemoveFromGroupCmd::CPbkRemoveFromGroupCmd
        (CPbkContactEngine& aEngine,
        CPbkContactViewListControl& aUiControl) :    
    iEngine(aEngine),
    iUiControl(aUiControl)    
	{
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroupCmd::CPbkRemoveFromGroupCmd(0x%x)"), 
        this);
    }

inline void CPbkRemoveFromGroupCmd::ConstructL
        (const CContactIdArray& aContacts,
        TContactItemId aGroupId)
    {
    iRemoveFromGroupProcess = CPbkRemoveFromGroup::NewLC(iEngine, aContacts, aGroupId);
    CleanupStack::Pop(iRemoveFromGroupProcess);    
    iWaitNoteWrapper = CPbkProgressNoteWrapper::NewL();    
    }

CPbkRemoveFromGroupCmd* CPbkRemoveFromGroupCmd::NewL
        (CPbkContactEngine& aEngine,
        const CContactIdArray& aContacts,
        CPbkContactViewListControl& aUiControl,
        TContactItemId aGroupId)
    {
    CPbkRemoveFromGroupCmd* self = new(ELeave) CPbkRemoveFromGroupCmd(aEngine, aUiControl);
    CleanupStack::PushL(self);
    self->ConstructL(aContacts, aGroupId);
    CleanupStack::Pop(self);
    return self;
    }

void CPbkRemoveFromGroupCmd::ResetWhenDestroyed
        (CPbkRemoveFromGroupCmd** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr==this, 
        Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

CPbkRemoveFromGroupCmd::~CPbkRemoveFromGroupCmd()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroupCmd::~CPbkRemoveFromGroupCmd(0x%x)"), 
        this);

    iUiControl.SetBlank(EFalse);
    delete iWaitNoteWrapper;
    delete iRemoveFromGroupProcess;

    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    iDestroyed = ETrue;
    }

void CPbkRemoveFromGroupCmd::ExecuteLD()
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroupCmd::ExecuteLD(0x%x)"), 
        this);
    CleanupStack::PushL(this);
    
    // Save focus 
	iFocusIndex = iUiControl.CurrentItemIndex();
    iFocusedContact = iUiControl.ContactIdAtL(iFocusIndex);

    // Blank UI control to avoid flicker
    iUiControl.SetBlank(ETrue);
    
    CPbkWaitNoteWrapperBase::TNoteParams noteParams;
        noteParams.iObserver = this;
    // ProcessFinished will be called when execution is finished.
    iWaitNoteWrapper->ExecuteL
            (*iRemoveFromGroupProcess, R_QTN_PHOB_WNOTE_REM_FROM_GROUP, noteParams);
    
    CleanupStack::Pop(this);
    }

void CPbkRemoveFromGroupCmd::AddObserver
        (MPbkCommandObserver& aObserver) 
    {
    iCommandObserver = &aObserver;
    }

void CPbkRemoveFromGroupCmd::ProcessFinished
        (MPbkBackgroundProcess& /*aProcess*/)
    {
    PBK_DEBUG_PRINT
        (PBK_DEBUG_STRING("CPbkRemoveFromGroupCmd::ProcessFinished(0x%x)"), 
        this);

    // Clear listbox selections
    iUiControl.HandleMarkableListUpdateAfterCommandExecution();

    // Restore focus
    TInt focusedIndex(0);
    // If focused contact still exists, then restore the focus to it
    TRAPD(err, focusedIndex = iUiControl.FindContactIdL(iFocusedContact));
    if ((err == KErrNone) && (focusedIndex != KErrNotFound))
        {
        // The focused contact was not deleted (special case)
        iUiControl.SetCurrentItemIndex(focusedIndex);
        }
    else
        {
        // Safe to continue even if err != KErrNone
        TInt items = iUiControl.NumberOfItems();
	    if (iFocusIndex < items)
		    {
		    iUiControl.SetCurrentItemIndex(iFocusIndex);
		    }
	    else if (iFocusIndex >= iUiControl.NumberOfItems())
		    {
		    // Subtract one because index is zero-based
		    iUiControl.SetCurrentItemIndex(items-1);
		    }
        }

    // Unblank and redraw UI control
    iUiControl.SetBlank(EFalse);
    iUiControl.DrawDeferred();
    }
    
//  End of File  
