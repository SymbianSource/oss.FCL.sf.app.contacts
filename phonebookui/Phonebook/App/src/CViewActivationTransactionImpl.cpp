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
*          Phonebook activation view transaction implementation definition.
*
*/


// INCLUDE FILES
#include    "CViewActivationTransactionImpl.h"
#include    <aknnavi.h>     // CAknNavigationControlContainer
#include    <aknnavide.h>   // CAknNavigationDecorator
#include    <akntabgrp.h>   // CAknTabGroup
#include    <akntitle.h>    // CAknTitlePane
#include    <akncontext.h>  // CAknContextPane
#include    <eikimage.h>    // CEikImage
#include    <DigViewGraph.h>
#include    "CPbkViewNavigator.h"


// ================= MEMBER FUNCTIONS =======================

CPbkAppUi::CViewActivationTransactionImpl* CPbkAppUi::CViewActivationTransactionImpl::NewLC
        (CPbkAppUi& aAppUi, 
        const TUid& aViewId, const TVwsViewId& aPrevViewId,
        const TDesC* aTitlePaneText, 
        const CEikImage* aContextPanePicture,
        TUint aFlags)
    {
    // Create object. new(ELeave) resets all members to zero.
    CViewActivationTransactionImpl* self = new(ELeave)
		CViewActivationTransactionImpl(aAppUi, aViewId, aPrevViewId, aFlags);
    CleanupStack::PushL(self);
	self->ConstructL(aTitlePaneText, aContextPanePicture);
    return self;
    }


CPbkAppUi::CViewActivationTransactionImpl::CViewActivationTransactionImpl
		(CPbkAppUi& aAppUi, const TUid& aViewId, const TVwsViewId& aPrevViewId,
		TUint aFlags) : iAppUi(&aAppUi), iViewId(aViewId),
		iPrevViewId(aPrevViewId), iFlags(aFlags)
	{

	}


void CPbkAppUi::CViewActivationTransactionImpl::ConstructL
		(const TDesC* aTitlePaneText, const CEikImage* aContextPanePicture)
	{
    // Make changes to state and save rollback data
    UpdateViewNavigationState();
    
	if (iFlags & EUpdateNaviPane) 
        {
        UpdateNaviPaneL(iViewId);
        }
    if (iFlags & EUpdateTitlePane)
        {
        SetTitlePaneTextL(aTitlePaneText);
        }
    if (iFlags & EUpdateContextPane)
        {
        SetContextPanePictureL(aContextPanePicture);
        }
	}


CPbkAppUi::CViewActivationTransactionImpl::~CViewActivationTransactionImpl()
    {
    // Undo changes in reverse order
    if (iSaveContextPanePicture)
        {
        // iContextPane takes ownership of iSaveContextPanePicture
        iContextPane->SetPicture(iSaveContextPanePicture);
        }
    if (iSaveTitlePaneText)
        {
        // iTitlePane takes ownership of iSaveTitlePaneText
        iTitlePane->SetText(iSaveTitlePaneText);
        }
    if (iPushedNaviDecorator)
        {
        iAppUi->NaviPane()->Pop(iPushedNaviDecorator);
        }
    if (iTabGroup)
        {
        iTabGroup->SetActiveTabByIndex(iSaveTabIndex);
        }
    if (iSavePreviousViewNode)
        {
        CDigViewNode* activatedNode = 
            iAppUi->iViewGraph->FindNodeWithViewId(iViewId);
        activatedNode->SetPreviousNode(iSavePreviousViewNode);
        }
    }

void CPbkAppUi::CViewActivationTransactionImpl::Commit()
    {
    // Delete and reset all rollback data
    delete iSaveContextPanePicture;
    iSaveContextPanePicture = NULL;
    delete iSaveTitlePaneText;
    iSaveTitlePaneText = NULL;
    iPushedNaviDecorator = NULL;
    iTabGroup = NULL;
    iSavePreviousViewNode = NULL;
    }

void CPbkAppUi::CViewActivationTransactionImpl::UpdateViewNavigationState()
    {
    // Reset saved state
    iSavePreviousViewNode = NULL;

    // Find activated view node in the graph
    CDigViewNode* activatedNode = 
        iAppUi->iViewGraph->FindNodeWithViewId(iViewId);
    if (activatedNode)
        {
        // Set previous node for the newly activated view
        if (iAppUi->IsPhonebookView(iPrevViewId))
            {
            // Find the previous view in the graph
            CDigViewNode* prevNode = 
                iAppUi->iViewGraph->FindNodeWithViewId(iPrevViewId.iViewUid);
            while (prevNode)
                {
                // Get transitions from the new view
                const TInt count = activatedNode->TransitionCount();
                TInt i;
                for (i = 0; i < count; ++i)
                    {
                    // Set previous view if there is a back transition
                    if (activatedNode->TransitionAt(i).iEvent == EPbkViewTransitionBack && 
                        activatedNode->TransitionAt(i).iNode == prevNode)
                        {
                        iSavePreviousViewNode = activatedNode->PreviousNode();
                        activatedNode->SetPreviousNode(prevNode);
                        break;
                        }
                    }
                if (i >= count)
                    {
                    // Matching back transition not found: Move back in 
                    // previous view chain
                    prevNode = prevNode->PreviousNode();
                    }
                else
                    {
                    // Transition found, exit while loop
                    break;
                    }
                }
            }
        else
            {
            iSavePreviousViewNode = activatedNode->PreviousNode();
            activatedNode->SetPreviousNode(activatedNode->DefaultPreviousNode());
            }
        }
    }

void CPbkAppUi::CViewActivationTransactionImpl::UpdateNaviPaneL(TUid aViewId)
    {
    // Reset saved state
    iPushedNaviDecorator = NULL;
    iTabGroup = NULL;
    iSaveTabIndex = 0;

    CAknNavigationDecorator* decorator= 
        iAppUi->iViewNavigator->TabGroupFromId(aViewId.iUid);
    if (decorator)
        {
        CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>
            (decorator->DecoratedControl());
        if (iAppUi->NaviPane()->Top() != decorator)
            {
            iAppUi->NaviPane()->PushL(*decorator);
            iPushedNaviDecorator = decorator;
            }
        iTabGroup = tabGroup;
        iSaveTabIndex = tabGroup->ActiveTabIndex();
        tabGroup->SetActiveTabById(aViewId.iUid);
        }
    }

void CPbkAppUi::CViewActivationTransactionImpl::SetTitlePaneTextL
        (const TDesC* aTitlePaneText)
    {
    // Reset saved state
    iSaveTitlePaneText = NULL;

    CEikStatusPane* statusPane = iAppUi->StatusPane();
    if (statusPane)
        {
        if (statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidTitle)).IsPresent())
            {
            iTitlePane = static_cast<CAknTitlePane*>
                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidTitle)));
            if (iTitlePane->Text())
                {
                iSaveTitlePaneText = iTitlePane->Text()->AllocL();
                }
            if (aTitlePaneText)
                {
                iTitlePane->SetTextL(*aTitlePaneText);
                }
            else
                {
                iTitlePane->SetTextToDefaultL();
                }
            }
        }
    }

CEikImage* CPbkAppUi::CViewActivationTransactionImpl::CopyL(const CEikImage& aSrc)
    {
    CEikImage* copy = new(ELeave) CEikImage;
    CleanupStack::PushL(copy);
    copy->SetPictureOwnedExternally(EFalse);
    CFbsBitmap* bitmap = new(ELeave) CFbsBitmap;
    copy->SetBitmap(bitmap);
    CFbsBitmap* mask = new(ELeave) CFbsBitmap;
    copy->SetMask(mask);
    User::LeaveIfError(bitmap->Duplicate(aSrc.Bitmap()->Handle()));
    User::LeaveIfError(mask->Duplicate(aSrc.Mask()->Handle()));
    CleanupStack::Pop();  // copy
    return copy;
    }

void CPbkAppUi::CViewActivationTransactionImpl::SetContextPanePictureL(const CEikImage* aContextPanePicture)
    {
    // Reset saved state
    iSaveContextPanePicture = NULL;

    CEikStatusPane* statusPane = iAppUi->StatusPane();
    if (statusPane)
        {
        if (statusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidContext)).IsInCurrentLayout())
            {
            iContextPane = static_cast<CAknContextPane*>
                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidContext)));
            iSaveContextPanePicture = CopyL(iContextPane->Picture());
            if (aContextPanePicture)
                {
                iContextPane->SetPicture(CopyL(*aContextPanePicture));
                }
            else
                {
                iContextPane->SetPictureToDefaultL();
                }
            }
        }
    }

//  End of File
