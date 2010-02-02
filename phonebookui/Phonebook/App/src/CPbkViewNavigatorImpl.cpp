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
*       Phonebook view navigation implementation.
*
*/


//  INCLUDES
#include "CPbkViewNavigator.h"
#include <AknTabObserver.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <CPbkAppUiBase.h>
#include <CPbkAppViewBase.h>


/// Unnamed namespace for local definitions
namespace {

// LOCAL FUNCTIONS
CAknTabGroup* GetTabGroupOnTop(CAknNavigationControlContainer* aNaviPane)
    {
    CAknTabGroup* result = NULL;
    if (aNaviPane)
        {
        CAknNavigationDecorator* topDecorator = aNaviPane->Top();
        if (topDecorator && 
            topDecorator->ControlType() == CAknNavigationDecorator::ETabGroup)
            {
            result = static_cast<CAknTabGroup*>(topDecorator->DecoratedControl());
            }
        }
    return result;
    }

}  // namespace

// CLASS DECLARATION

/**
 * Phonebook view navigation implementation.
 */
class CPbkViewNavigatorImpl : 
        public CPbkViewNavigator,
        private MAknTabObserver
    {
    public:  // Constructors and destructor
        CPbkViewNavigatorImpl
            (const CDigViewGraph& aViewGraph, CPbkAppUiBase& aAppUi);
        void ConstructL();

    public:  // from CPbkViewNavigator
        TBool HandleNavigationKeyEventL
            (const TKeyEvent& aKeyEvent, TEventCode aType);
        CAknNavigationDecorator* CreateTabGroupL(TResourceReader& aResReader);

    private:  // from MAknTabObserver
		void TabChangedL(TInt aIndex);
    };


// ================= MEMBER FUNCTIONS =======================

inline CPbkViewNavigatorImpl::CPbkViewNavigatorImpl
        (const CDigViewGraph& aViewGraph, CPbkAppUiBase& aAppUi) :
    CPbkViewNavigator(aViewGraph, aAppUi)
    {
    }

inline void CPbkViewNavigatorImpl::ConstructL()
    {
    BaseConstructL();
    }

CPbkViewNavigator* CPbkViewNavigator::NewL
        (const CDigViewGraph& aViewGraph,
        CPbkAppUiBase& aAppUi)
    {
    CPbkViewNavigatorImpl* self = 
        new(ELeave) CPbkViewNavigatorImpl(aViewGraph, aAppUi);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CAknNavigationDecorator* CPbkViewNavigatorImpl::CreateTabGroupL
        (TResourceReader& aResReader)
    {
    // Create tab group and register self as an observer to it
    return iNaviPane->CreateTabGroupL(aResReader,this);
    }

TBool CPbkViewNavigatorImpl::HandleNavigationKeyEventL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    TBool result = EFalse;
    CAknTabGroup* topTabGroup = GetTabGroupOnTop(iNaviPane);
    if (topTabGroup)
        {
        result = topTabGroup->OfferKeyEventL(aKeyEvent, aType)==EKeyWasConsumed;
        }
    return result;
    }

void CPbkViewNavigatorImpl::TabChangedL(TInt aIndex)
    {
    CAknTabGroup* topTabGroup = GetTabGroupOnTop(iNaviPane);
    if (topTabGroup)
        {
        // View ID is stored in the tab
        const TInt viewId(topTabGroup->TabIdFromIndex(aIndex));
        // Get current view's state and use it as a parameter to the next view
        CPbkViewState* activeViewState = iAppUi.ActiveView()->GetViewStateLC();
        iAppUi.ActivatePhonebookViewL(TUid::Uid(viewId), activeViewState);
        if (activeViewState)
            {
            CleanupStack::PopAndDestroy(activeViewState);
            }
        }
    }

// End of File
