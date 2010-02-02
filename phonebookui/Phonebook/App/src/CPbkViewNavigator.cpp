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
*       Phonebook view navigation implementation base class.
*
*/


//  INCLUDES
#include "CPbkViewNavigator.h"
#include <barsread.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <akntabgrp.h>
#include <AknsUtils.h>

#include <CPbkAppUiBase.h>
#include <DigViewGraph.h>

#include <PbkIcons.hrh>

#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>
#include <PbkIconInfo.h>
#include <Phonebook.rsg>

// ================= MEMBER FUNCTIONS =======================

CPbkViewNavigator::~CPbkViewNavigator()
    {
    iViewTabGroups.ResetAndDestroy();
    Release(iExtGlobals);
    }

CAknNavigationDecorator* CPbkViewNavigator::TabGroupFromId(TInt aTabId) const
    {
    const TInt tabGroupCount = iViewTabGroups.Count();
    for (TInt i=0; i < tabGroupCount; ++i)
        {
        CAknNavigationDecorator* decorator = 
            CONST_CAST(CAknNavigationDecorator*, iViewTabGroups[i]);
        CAknTabGroup* tabGroup = 
            static_cast<CAknTabGroup*>(decorator->DecoratedControl());
        if (tabGroup->TabIndexFromId(aTabId) >= 0)
            {
            return decorator;
            }
        }
    return NULL;
    }

void CPbkViewNavigator::BaseConstructL()
    {
    iExtGlobals = CPbkExtGlobals::InstanceL();

    iNaviPane = static_cast<CAknNavigationControlContainer*>
        (iAppUi.StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));

    RArray<TInt> tabGroupIds;
    CleanupClosePushL(tabGroupIds);
    const TInt nodeCount = iViewGraph.Count();
    for (TInt i=0; i < nodeCount; ++i)
        {
        const CDigViewNode& viewNode = iViewGraph[i];
        const TInt resId = viewNode.AdditionalResId();
        if (resId != 0 && tabGroupIds.Find(resId) == KErrNotFound)
            {
            User::LeaveIfError(tabGroupIds.Append(resId));
            TResourceReader resReader;
            iAppUi.ControlEnv()->CreateResourceReaderLC(resReader, resId);
            CAknNavigationDecorator* tabGroup = CreateTabGroupL(resReader);
            CleanupStack::PushL(tabGroup);
            User::LeaveIfError(iViewTabGroups.Append(tabGroup));
            CleanupStack::Pop(tabGroup);
            CleanupStack::PopAndDestroy();  // resReader
            }
        }

    // Call this to skin the tab bitmaps
    HandleResourceChange(KAknsMessageSkinChange);

    CleanupStack::PopAndDestroy(&tabGroupIds);
    }

void CPbkViewNavigator::HandleResourceChange(TInt aType)
	{
	const TInt tabGroupCount = iViewTabGroups.Count();
	for (TInt i=0; i<tabGroupCount; ++i)
		{
        CAknTabGroup* tabGroup = TabGroupFromIndex(i);
		tabGroup->HandleResourceChange(aType);            
		}

    if (aType == KAknsMessageSkinChange)
        {
        SkinTabBitmaps();
        }
	}

CAknTabGroup* CPbkViewNavigator::TabGroupFromIndex (TInt aIndex) const
	{
	CAknNavigationDecorator* decorator = 
        CONST_CAST(CAknNavigationDecorator*, iViewTabGroups[aIndex]);
	CAknTabGroup* tabGroup = 
        static_cast<CAknTabGroup*>(decorator->DecoratedControl());

	return tabGroup;
	}

void CPbkViewNavigator::SkinTabBitmaps()
    {
    TRAP_IGNORE(DoSkinTabBitmapsL());
    }

void CPbkViewNavigator::DoSkinTabBitmapsL()
    {
    CPbkIconInfoContainer* iconInfoContainer = 
        CPbkIconInfoContainer::NewL(R_PBK_TAB_ICON_INFO_ARRAY);
    CleanupStack::PushL(iconInfoContainer);

    // let extensions add their own tab icons
    iExtGlobals->FactoryL().AddPbkTabIconsL(iconInfoContainer);
    
	const TInt tabGroupCount = iViewTabGroups.Count();
	for (TInt i=0; i<tabGroupCount; ++i)
        {
        CAknTabGroup* tabGroup = TabGroupFromIndex(i);
        SkinTabGroupL(*tabGroup, *iconInfoContainer);
        }

    CleanupStack::PopAndDestroy(iconInfoContainer);
    }

void CPbkViewNavigator::SkinTabGroupL
        (CAknTabGroup& aTabGroup, CPbkIconInfoContainer& aIconInfoContainer)
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    const TInt count = aTabGroup.TabCount();
    for (TInt i = 0; i < count; ++i)
        {
        TInt tabId = aTabGroup.TabIdFromIndex(i);
        // tabId is used as icon id
        const TPbkIconInfo* iconInfo = 
            aIconInfoContainer.Find(TPbkIconId(tabId));
        if (iconInfo)
            {
            CFbsBitmap* bitmap = NULL;
            CFbsBitmap* mask = NULL;
            PbkIconUtils::CreateIconLC(
                skin, bitmap, mask, *iconInfo);

            aTabGroup.ReplaceTabL(tabId, bitmap, mask);

            CleanupStack::Pop(2); // mask, bitmap
            }
        }
    }

// End of File
