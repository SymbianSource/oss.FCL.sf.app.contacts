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
*       Provides Phonebook Application view methods.
*
*/


// INCLUDE FILES
#include <CPbkAppViewBase.h>     // This class' declaration

#include "CPbkDocumentBase.h"
#include <CPbkAppUiBase.h>
#include <CPbkAiwInterestArray.h>
#include <CPbkAppGlobalsbase.h>

#include <akntitle.h>			// CAknTitlePane
#include <akncontext.h>			// CAknContextPane
#include <eikmenub.h>			// CEikMenuBar
#include <eikapp.h>				// CEikApplication


// MACROS
#undef TEST_CPbkAppViewBase  // define this to turn testing on for this class


// ENUMERATIONS

/**
 * Testing mode bit flags for class CPbkAppViewBase.
 */
enum
    {
    /// Alloc test CPbkAppViewBase::ProcessCommandL(TInt).
    ETestModeAllocFailureProcessCommandL = 1
    };


// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPbkAppViewBase::CPbkAppViewBase()
    {
    }

EXPORT_C CPbkAppViewBase::~CPbkAppViewBase()
    {
    ClosePopup();
    }

EXPORT_C TUid CPbkAppViewBase::ApplicationUid() const
    {
    return AppUi()->Application()->AppDllUid();
    }

EXPORT_C CPbkAppUiBase* CPbkAppViewBase::PbkAppUi() const
    {
    // Explicit cast: the app ui must always be of type CPbkAppUiBase
    return static_cast<CPbkAppUiBase*>(CAknView::AppUi());
    }

EXPORT_C CPbkContactEngine* CPbkAppViewBase::Engine() const
    {
    return PbkAppUi()->PbkDocument()->Engine();
    }

EXPORT_C CAknTitlePane* CPbkAppViewBase::TitlePane() const
    {
    return static_cast<CAknTitlePane*>
		(GetStatusPaneControlSafely(EEikStatusPaneUidTitle));
    }

EXPORT_C CAknContextPane* CPbkAppViewBase::ContextPane() const
    {
    return static_cast<CAknContextPane*>
		(GetStatusPaneControlSafely(EEikStatusPaneUidContext));
    }

EXPORT_C TBool CPbkAppViewBase::HandleCommandKeyL
        (const TKeyEvent& /*aKeyEvent*/, 
        TEventCode /*aType*/)
    {
    return EFalse;
    }

EXPORT_C CPbkViewState* CPbkAppViewBase::GetViewStateLC() const
    {
    return NULL;
    }


EXPORT_C void CPbkAppViewBase::LaunchPopupMenuL
        (TInt aResourceId)
    {
    ClosePopup();
    iPopup=new(ELeave) CEikMenuBar;
    TRAPD(err,DoLaunchPopupL(aResourceId));
    if (err)
        {
        ClosePopup();
        User::Leave(err);
        }
    }

EXPORT_C void CPbkAppViewBase::HandleCommandL
        (TInt aCommand)
    {
    // No command handling in this class, just forward to app ui.
    AppUi()->HandleCommandL(aCommand);
    }

EXPORT_C void CPbkAppViewBase::DynInitMenuPaneL
        (TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    // No menu updating in this class, just forward to app ui.
    AppUi()->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

EXPORT_C void CPbkAppViewBase::ProcessCommandL
        (TInt aCommand)
    {
    // Close popup menu if open
    if (iPopup)
        {
        iPopup->StopDisplayingMenuBar();
	    ClosePopup();
        }

#ifdef TEST_CPbkAppViewBase
    if (iTestMode & ETestModeAllocFailureProcessCommandL)
        {
        switch (aCommand)
            {
            // Don't run the test for these commands
            case EEikCmdExit:				// FALLTHROUGH
            case EAknCmdExit:				// FALLTHROUGH
            case EAknSoftkeyOptions:		// FALLTHROUGH
				{
                break;
				}

            default:
                {
                // Testing is one-shot -> turn it off
                iTestMode &= ~(ETestModeAllocFailureProcessCommandL);
                // Run ProcessCommandL in an alloc test loop
                TInt error = KErrNone;
                TInt allocFail = 0;
                do
                    {
                    User::__DbgSetAllocFail(RHeap::EUser, RHeap::EDeterministic, ++allocFail);
                    TRAP(error, CAknView::ProcessCommandL(aCommand));
                    User::__DbgSetAllocFail(RHeap::EUser,RHeap::ENone,1);
                    }
                while (error == KErrNoMemory);
                if (error == KErrNone)
                    {
                    RDebug::Print(_L("CPbkAppViewBase::ProcessCommandL(%d) executed after %d alloc failures."), 
                        aCommand, allocFail);
                    }
                else
                    {
                    // Some other leave than out of memory occured
                    User::Leave(error);
                    }
                return;
                }
            }
        }
#endif // TEST_CPbkAppViewBase

    // Call base class
    CAknView::ProcessCommandL(aCommand);
    }

EXPORT_C TBool CPbkAppViewBase::PbkProcessKeyEventL
        (const TKeyEvent& aKeyEvent, 
        TEventCode aType)
    {
#ifdef TEST_CPbkAppViewBase
    // Check for special debugging keys
    if (aKeyEvent.iCode=='m' && (aKeyEvent.iModifiers & EModifierFunc))
        {
        // Turn alloc testing on for the next command
        iTestMode |= ETestModeAllocFailureProcessCommandL;
        RDebug::Print(_L("CPbkAppViewBase::PbkProcessKeyEventL(): Alloc failure testing turned on"));
        iEikonEnv->InfoMsg(_L("Alloc failure testing ON"));
        return ETrue;
        }
#endif // TEST_CPbkAppViewBase

    // Offer key first to self. Derived class may override HandleCommandKeyL
    if (HandleCommandKeyL(aKeyEvent, aType))
        {
        return ETrue;
        }
    else
        {
        // Key event is not handled by me, offer it to the default handler.
        return PbkAppUi()->PbkProcessKeyEventL(aKeyEvent, aType);
        }
    }

void CPbkAppViewBase::DoLaunchPopupL
        (TInt aResourceId)
    {
    iPopup->ConstructL(this, 0, aResourceId);
    AppUi()->AddToViewStackL(*this, iPopup, ECoeStackPriorityMenu, ECoeStackFlagRefusesFocus);
    iPopup->TryDisplayMenuBarL();
    }

/**
 * Returns status pane control if present.
 * @param aCtrlId   id of the status pane control.
 * @return  control with aPaneId if present in status pane, 
            NULL otherwise.
 */
CCoeControl* CPbkAppViewBase::GetStatusPaneControlSafely
        (TInt aPaneId) const
    {
    const TUid paneUid = { aPaneId };
    CEikStatusPane* statusPane = StatusPane();
    if (statusPane && statusPane->PaneCapabilities(paneUid).IsPresent())
        {
		CCoeControl* control = NULL;
        // ControlL shouldn't leave because the pane is present
		TRAPD(err, control = statusPane->ControlL(paneUid));
		if (err != KErrNone)
			{
			return NULL;
			}
		else
			{
			return control;
			}
        }
    return NULL;
    }

/**
 * Removes the popup from the view stack and deletes it.
 */
void CPbkAppViewBase::ClosePopup()
    {
    if (iPopup)
		{
		AppUi()->RemoveFromViewStack(*this, iPopup);
		delete(iPopup);
		iPopup=NULL;
		}
    }

EXPORT_C void CPbkAppViewBase::CreateInterestItemAndAttachL(
        TInt aInterestId,
        TInt aMenuResourceId,
        TInt aInterestResourceId,
        TBool aAttachBaseServiceInterest)
    {
    if (!iAiwInterestArray)
        {
        iAiwInterestArray =
            CPbkAppGlobalsBase::InstanceL()->AiwInterestArrayL(*Engine());
        }

    // Delegate to the interest array
    iAiwInterestArray->CreateInterestItemAndAttachL(
        aInterestId, aMenuResourceId,
        aInterestResourceId, aAttachBaseServiceInterest);
    }

//  End of File  
