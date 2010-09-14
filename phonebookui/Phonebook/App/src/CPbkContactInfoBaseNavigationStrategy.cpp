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
*       Contact Info View common base navigation stategy.
*
*/


// INCLUDE FILES
#include "CPbkContactInfoBaseNavigationStrategy.h"  // This class' declaration

#include <barsread.h> // TResourceReader
#include <aknnavide.h>
#include <StringLoader.h>
#include <akntabgrp.h>
#include <gulicon.h>
#include <eikspane.h>

// Phonebook.app include files
#include <phonebook.rsg>
#include <pbkview.rsg>
#include <pbkdebug.h>

// PbkEng.dll include files
#include <PbkIconInfo.h>
#include <CPbkContactItem.h>
#include "MPbkContactInfoNavigationCallback.h"
#include <Phonebook.hrh>


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
//

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicIcon_StatusPaneUpdateL
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactInfoBaseNavigationStrategy");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

CPbkContactInfoBaseNavigationStrategy::CPbkContactInfoBaseNavigationStrategy
        (CPbkContactEngine& aEngine,
        MPbkContactInfoNavigationCallback& aCallback) :
    iEngine(aEngine),
    iNavigationCallback(aCallback),
    iViewReady(EFalse)
    {
    }

CPbkContactInfoBaseNavigationStrategy::~CPbkContactInfoBaseNavigationStrategy()
    {
    delete iNaviDeco;
    }

TBool CPbkContactInfoBaseNavigationStrategy::HandleCommandKeyL
        (const TKeyEvent& aKeyEvent, 
        TEventCode /*aType*/)
    {
    if (iViewReady)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyLeftArrow: // FALLTHROUGH
            case EKeyRightArrow:
                {
                TInt dir = 0;
                switch (aKeyEvent.iCode)
                    {
                    case EKeyLeftArrow:
                        {
                        dir = -1;
                        break;
                        }
                    case EKeyRightArrow:
                        {
                        dir = 1;
                        break;
                        }
                    }
                // Handle mirrored layout by negating the directionality
                if (AknLayoutUtils::LayoutMirrored())
                    {
                    dir = -1 * dir;
                    }
                NavigateContactL(dir);
                return ETrue;
                }
            }
        }
    return EFalse;
    }

void CPbkContactInfoBaseNavigationStrategy::StatusPaneUpdateL()
    {
    // update the navi decorator
    if (iViewReady)
        {
        // check the index and count of the groups to navi pane
        const TInt index = iScrollView->FindL(iNavigationCallback.GetContactItemId()) + 1;
        
        // create navi pane text
        CArrayFixFlat<TInt>* values = new (ELeave) CArrayFixFlat<TInt>(2);
        CleanupStack::PushL(values);
        values->AppendL(index);
        values->AppendL(iScrollView->CountL());      
        HBufC* buf  = StringLoader::LoadLC(R_QTN_PHOB_NAVI_POS_INDICATOR,
			*values);

        CPbkIconInfoContainer* iconInfoContainer =
			CPbkIconInfoContainer::NewL(R_PBK_ICON_INFO_ARRAY, CCoeEnv::Static());
        CleanupStack::PushL(iconInfoContainer);
                
        CGulIcon* icon = iconInfoContainer->LoadBitmapL(
                            EPbkqgn_prop_pb_contacts_tab1 );
        __ASSERT_DEBUG(icon, Panic(EPanicIcon_StatusPaneUpdateL));
        CleanupStack::PopAndDestroy(); // iconInfoContainer
        CleanupStack::PushL(icon);

        static_cast<CAknTabGroup*>(iNaviDeco->DecoratedControl())
            ->ReplaceTabL(EPbkContactInfoViewId, *buf, icon->Bitmap(),
			icon->Mask());
        // tab group takes ownership of bitmap and mask
        icon->SetBitmapsOwnedExternally(ETrue);
        CleanupStack::PopAndDestroy(3); // icon, buf, values
        }
    }

void CPbkContactInfoBaseNavigationStrategy::ConstructNaviIndicatorsL()
    {
    // navi indicators
    CEikStatusPane* statusPane = iNavigationCallback.GetStatusPane();
    if (statusPane && statusPane->PaneCapabilities
		(TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
        {
        CAknNavigationControlContainer* naviPane =
			static_cast<CAknNavigationControlContainer*>
			(statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
        if (!iNaviDeco)
            {
            TResourceReader resReader;
            CCoeEnv::Static()->CreateResourceReaderLC(resReader,
				R_PBK_CONTACT_INFO_TAB_GROUP);
            iNaviDeco = naviPane->CreateTabGroupL(resReader);
            CleanupStack::PopAndDestroy(); // resReader
            }

        iNaviDeco->MakeScrollButtonVisible(ETrue);
        if (iScrollView->CountL() > 1)
            {
            iNaviDeco->SetScrollButtonDimmed
				(CAknNavigationDecorator::ERightButton, EFalse);
            iNaviDeco->SetScrollButtonDimmed
				(CAknNavigationDecorator::ELeftButton, EFalse);
            }
        else
            {
            iNaviDeco->SetScrollButtonDimmed
				(CAknNavigationDecorator::ERightButton, ETrue);
            iNaviDeco->SetScrollButtonDimmed
				(CAknNavigationDecorator::ELeftButton, ETrue);
            }
        naviPane->PushL(*iNaviDeco);
        }
    StatusPaneUpdateL();
    }

void CPbkContactInfoBaseNavigationStrategy::NavigateContactL
        (TInt aDir)
    {
    if (aDir == 0) 
        {
        // no change
        return;
        }

    TInt contactIndex = iScrollView->FindL(iNavigationCallback.GetContactItemId()) + aDir;
    const TInt contactCount = iScrollView->CountL();
    if (contactCount > 1)
        {
        if (contactIndex < 0)
            {
            contactIndex = contactCount + contactIndex;
            }
        else if (contactIndex >= contactCount)
            {
            contactIndex = contactIndex - contactCount;
            }
        const TContactItemId contactId = iScrollView->AtL(contactIndex);
        iNavigationCallback.ChangeContactL(contactId);

        StatusPaneUpdateL();
        }
    }


void CPbkContactInfoBaseNavigationStrategy::HandleContactViewEvent
        (const CContactViewBase& aView,
        const TContactViewEvent& aEvent)
    {
    if (&aView == iScrollView &&
        aEvent.iEventType == TContactViewEvent::EReady)
        {
        iViewReady = ETrue;
        // navi pane construction
        TRAPD(err, ConstructNaviIndicatorsL());
        if (err != KErrNone)
            {
            CCoeEnv::Static()->HandleError(err);
            }
        }
    }

void CPbkContactInfoBaseNavigationStrategy::HandleContactViewListControlEventL
        (CPbkContactViewListControl& /*aControl*/,
        const TPbkContactViewListControlEvent& aEvent)
    {
    if (aEvent.iEventType == TPbkContactViewListControlEvent::EReady)
        {
        // navi pane update
        StatusPaneUpdateL();
        }
    }

//  End of File  
