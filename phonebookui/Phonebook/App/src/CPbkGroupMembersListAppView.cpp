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
*          Provides group members list View methods.
*
*/


// INCLUDE FILES
#include    "CPbkGroupMembersListAppView.h"  // This class's declaration

#include <barsread.h>					// TResourceReader
#include <cntview.h>					// CContactGroupView
#include <eikmenup.h>					// CEikMenuPane
#include <akntitle.h>					// CAknTitlePane
#include <aknNaviDe.h>					// CAknNavigationDecorator
#include <akntabgrp.h>					// CAknTabGroup
#include <AknQueryDialog.h>				// CAknQueryDialog
#include <StringLoader.h>
#include <cntitem.h>					// TContactItemId


// Send UI
#include <sendui.h>						// Send UI API
#include <sendnorm.rsg>					// for Send UI functionality
#include <SenduiMtmUids.h>				// Send UI MTM uid's

#include <phonebook.rsg>				// Phonebook resources
#include <CPbkControlContainer.h>
#include "CPbkAppUi.h"					// Phonebook application UI class
#include "CPbkAssignToGroupCmd.h"
#include "CPbkRemoveFromGroupCmd.h"
#include "CPbkAppGlobals.h"

#include <CPbkContactEngine.h>
#include <PbkCommandHandler.h>
#include <CPbkConstants.h>
#include <CPbkAiwInterestArray.h>

#include <pbkview.rsg>					// PbkView dll resources
#include <CPbkViewState.h>
#include <CPbkContactViewListControl.h>
#include <CPbkGroupMembersFetchDlg.h>
#include <PbkIconInfo.h>
#include <Phonebook.hrh>
#include <MenuFilteringFlags.h>
#include <cshelp/phob.hlp.hrh>

// Debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"


/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_DoActivateL = 1,
    EPanicPostCond_DoDeactivate,
    EPanicPreCond_HandleCommandL,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPreCond_DynInitMenuPaneL,
    EPanicPreCond_CmdRemoveFromFolderL,
    EPanicPreCond_CmdFetchMembersL,
    EPanicPreCond_LoadGroupMembersViewL,
    EPanicPostCond_LoadGroupMembersViewL,
    EPanicPreCond_HandleCommandKeyL,
    EPanicIcon_StatusPaneUpdateL
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkGroupMembersListAppView");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

// CPbkGroupMembersListAppView::TIsViewReady
inline CPbkGroupMembersListAppView::TIsViewReady::TIsViewReady()
    : iReadyViewFlags(EReset)
    {
    }

inline TBool AllBitsSet(TUint aFlags, TUint aBits)
    {
    return (((aFlags & aBits)==aBits) ? ETrue : EFalse);
    }

inline TBool CPbkGroupMembersListAppView::TIsViewReady::AreViewsReady() const
    {
    return AllBitsSet(iReadyViewFlags, EAllGroupsReady | EControlReady);
    }

inline void CPbkGroupMembersListAppView::TIsViewReady::SetAllGroupsViewReady()
    {
    iReadyViewFlags |= EAllGroupsReady;
    }

inline void CPbkGroupMembersListAppView::TIsViewReady::SetControlReady()
    {
    iReadyViewFlags |= EControlReady;
    }

///////////////////////////////////////////////////////////////////////////////
// CPbkGroupMembersListAppView

inline CPbkGroupMembersListAppView::CPbkGroupMembersListAppView() :
        iGroupId(KNullContactId),
        iFocusedContact(KNullContactId)
    {
    }

inline void CPbkGroupMembersListAppView::ConstructL()
    {
    // PreCond
     __ASSERT_DEBUG(!iContainer, Panic(EPanicPreCond_ConstructL));

    // Init base class
    // this base construct is profiled in base class CPbkContactListAppView
    BaseConstructL(R_PHONEBOOK_GROUPMEMBERSLIST_APPVIEW);

    // PostCond
     __ASSERT_DEBUG(!iContainer, Panic(EPanicPostCond_ConstructL));
    }

CPbkGroupMembersListAppView* CPbkGroupMembersListAppView::NewL()
    {
    CPbkGroupMembersListAppView* self =
		new (ELeave) CPbkGroupMembersListAppView();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

CPbkGroupMembersListAppView::~CPbkGroupMembersListAppView()
    {
    delete iNaviDeco;

    if (iContainer)
        {
        // Remove the container from the view stack
        AppUi()->RemoveFromViewStack(*this, iContainer);
        // Destroy the control container
        delete iContainer;
        }

    if (iGroupMembersView)
        {
        iGroupMembersView->Close(*this);
        }
    if (iAllGroupsView)
        {
        iAllGroupsView->Close(*this);
        }
    }

inline void CPbkGroupMembersListAppView::CmdBackL()
    {
    CPbkViewState* state = iContainer->Control()->GetStateLC();
    PbkAppUi()->ActivatePreviousViewL(state);
    CleanupStack::PopAndDestroy();  // state
    }

void CPbkGroupMembersListAppView::CmdOpenMeViewsL()
	{
    if (!Control().ItemsMarked())
        {
        CPbkViewState* state = Control().GetStateLC();
        state->SetParentContactId(iGroupId);
        PbkAppUi()->ActivatePhonebookViewL(CPbkAppUi::KPbkContactInfoViewUid, state);
        CleanupStack::PopAndDestroy();  // state
        Control().HandleMarkableListUpdateAfterCommandExecution();
        }
	}

void CPbkGroupMembersListAppView::CmdRemoveFromFolderL()
	{
    __ASSERT_DEBUG(
        iContainer &&
        iContainer->Control()->NumberOfItems() > 0,
        Panic(EPanicPreCond_CmdRemoveFromFolderL));

    iFocusedContact =
		iContainer->Control()->FocusedContactIdL();

    CContactIdArray* items = NULL;
    if (iContainer->Control()->ItemsMarked())
        {
        items = CContactIdArray::NewLC
			(&iContainer->Control()->MarkedItemsL());
        }
    else
        {
        items = CContactIdArray::NewLC();
        items->AddL(iFocusedContact);
        }

    //original we will ask user if they really want to remove contacts from group,
    //but currently we decide that we will just do it without asking user
    if (items->Count() >= 1)
        {
        CPbkRemoveFromGroupCmd* cmd = CPbkRemoveFromGroupCmd::NewL(*Engine(),
                *items, *iContainer->Control(), iGroupId);
        PbkAppUi()->AddAndExecuteCommandL(cmd);
        }

    


    CleanupStack::PopAndDestroy(); // items
    }

void CPbkGroupMembersListAppView::CmdFetchMembersL()
	{
    __ASSERT_DEBUG(
        iContainer &&
        Engine()->AllContactsView().CountL() > 0,
        Panic(EPanicPreCond_CmdFetchMembersL));

    // Remove thumbnail from screen for the duration of the fetch
    iContainer->Control()->HideThumbnail();

    CPbkGroupMembersFetchDlg::TParams params;
    params.iGroupId = iGroupId;
    CPbkGroupMembersFetchDlg* groupMembersFetch =
		CPbkGroupMembersFetchDlg::NewL(params, *Engine());
    TInt res = 0;
    res = groupMembersFetch->ExecuteLD();
    if (res && params.iMarkedEntries)
        {
        CleanupStack::PushL(params.iMarkedEntries);
        CPbkAssignToGroupCmd* cmd = CPbkAssignToGroupCmd::NewL
            (*Engine(), *params.iMarkedEntries, *iContainer->Control(), iGroupId);
        PbkAppUi()->AddAndExecuteCommandL(cmd);
        CleanupStack::PopAndDestroy(); // params.iMarkedEntries
        }
	}

inline void CPbkGroupMembersListAppView::CmdContextMenuL()
    {
    if (iContainer->Control()->ItemsMarked())
        {
        LaunchPopupMenuL(R_PBK_GROUPMEMBERS_CONTEXT_MENUBAR_MARKED_ITEMS);
        }
    else
        {
        LaunchPopupMenuL(R_PBK_GROUPMEMBERS_CONTEXT_MENUBAR);
        }
    }

/**
 * Helper method for HandleCommandKeyL.
 */
inline void CPbkGroupMembersListAppView::NavigateGroupL(TInt aDir)
    {
    if (aDir == 0) return;

    TInt groupIndex = iAllGroupsView->FindL(iGroupId) + aDir;
    const TInt groupCount = iAllGroupsView->CountL();
    if (groupCount > 1)
        {
        if (groupIndex < 0)
            {
            groupIndex = groupCount + groupIndex;
            }
        else if (groupIndex >= groupCount)
            {
            groupIndex = groupIndex - groupCount;
            }
        const TContactItemId groupId = iAllGroupsView->AtL(groupIndex);
        CPbkViewState* state = iContainer->Control()->GetStateLC();
        state->SetFocusedContactId(groupId);
        PbkAppUi()->ActivatePhonebookViewL
			(CPbkAppUi::KPbkGroupMembersListViewUid, state);
        CleanupStack::PopAndDestroy(state);
        }
    }

TBool CPbkGroupMembersListAppView::HandleCommandKeyL
        (const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    // PreCond: (this is the precond for all Cmd* functions at the same time)
    __ASSERT_DEBUG(iContainer, Panic(EPanicPreCond_HandleCommandKeyL));

    // check if groups view is ready
    if (iViewsLoadingReady.AreViewsReady())
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
                NavigateGroupL(dir);
                return ETrue;
                }
            }
        }

    // Key not handled here, try base class
    return CPbkContactListAppView::HandleCommandKeyL(aKeyEvent, aType);
    }

CPbkViewState* CPbkGroupMembersListAppView::GetViewStateLC() const
    {
    return iContainer ? iContainer->Control()->GetStateLC() : NULL;
    }

CPbkContactViewListControl& CPbkGroupMembersListAppView::Control()
    {
    return *iContainer->Control();
    }

void CPbkGroupMembersListAppView::HandleCommandL
        (TInt aCommandId)
    {
    // PreCond: (this is the precond for all Cmd* functions at the same time)
    __ASSERT_DEBUG(iContainer, Panic(EPanicPreCond_HandleCommandL));

    // Process view-specific commands
    switch (aCommandId)
        {
        case EAknSoftkeyBack:
			{
            CmdBackL();
            break;
			}

        case EPbkCmdRemoveFromFolder:
			{
            CmdRemoveFromFolderL();
            break;
			}

        case EPbkCmdFetchMembers:
			{
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkGroupMembersListAppView::CmdFetchMembersL));
            break;
			}

        case EPbkCmdContextMenu:
			{
            CmdContextMenuL();
            break;
			}
        case EPbkCmdSendUICommand:
            {
            // Set up flags depending on current UI state
            TUint flags = 0;
            if (iContainer->Control()->ItemsMarked())
                {
                flags |= KPbkItemsMarked;
                }
            HandleSendContactsL( flags );
            break;
            }
        default:
			{
            // Command not handled here, forward it to base class
            CPbkContactListAppView::HandleCommandL(aCommandId);
            break;
			}
        }
    }

void CPbkGroupMembersListAppView::DynInitMenuPaneL
        (TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    __ASSERT_DEBUG(iContainer, Panic(EPanicPreCond_DynInitMenuPaneL));

    // Set up flags depending on current UI state
    TUint flags = 0;
    if (iContainer->Control()->NumberOfItems() == 0)
        {
        flags |= KPbkControlEmpty;
        }
    if (iContainer->Control()->ItemsMarked())
        {
        flags |= KPbkItemsMarked;
        }
    if (Engine()->AllContactsView().CountL() == 0)
        {
        flags |= KPbkContactListEmpty;
        }

    // Offer the menu to AIW first
    if (iAiwInterestArray->InitMenuPaneL(aResourceId, *aMenuPane, flags))
        {
        // No need to continue
        return;
        }

    switch (aResourceId)
        {
        case R_PBK_GROUPMEMBERS_MENU:
            {
            OptionsMenuFilteringL(*aMenuPane, flags);
            break;
            }
        case R_PBK_GROUPMEMBERS_SENDCONTACTMENU:
            {
            SendContactMenuFilteringL(*aMenuPane, flags);
            break;
            }
        case R_PBK_GROUPMEMBERS_CONTEXTMENU:
            {
            ContextMenuFilteringL(*aMenuPane, flags);
            break;
            }
        default:
            {
            break;
            }
        }

    CPbkContactListAppView::DynInitMenuPaneL(aResourceId, aMenuPane);
    }

void CPbkGroupMembersListAppView::OptionsMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags)
    {
    if ((aFlags & KPbkControlEmpty) || (aFlags & KPbkItemsMarked))
        {
        // if members list is empty or items are marked filter options list
        aMenuPane.SetItemDimmed(EPbkCmdOpenMeViews, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdBelongsToGroups, ETrue);
        }
    if ((aFlags & KPbkItemsMarked) && !(aFlags & KPbkControlEmpty))
        {
        // if items are marked items but list is not empty
        aMenuPane.SetItemDimmed(EPbkCmdFetchMembers, ETrue);
        }
    if (aFlags & KPbkControlEmpty)
        {
        // if members list is empty, no options menu items
        aMenuPane.SetItemDimmed(EPbkCmdRemoveFromFolder, ETrue);
        }
    if (aFlags & KPbkContactListEmpty)
        {
        // Fetch members not available if names list is empty
        aMenuPane.SetItemDimmed(EPbkCmdFetchMembers, ETrue);
        }
    }

void CPbkGroupMembersListAppView::SendContactMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags)
    {
	CPbkAppGlobals* globals =	// does not take ownership
        CPbkAppGlobals::InstanceL();

    // By default disable filtering of send contact menu,
    // when there are marked items
    TBool markedItemsFiltering = EFalse;

    // Check is multiple vCard sending via BT/IR supported
    if (!Engine()->Constants()->LocallyVariatedFeatureEnabled
        (EPbkLVMultipleVCardOverBTAndIR))
        {
        // It is not supported. Marked items filtering is
        // now needed if there is no mailbox set up
        markedItemsFiltering = !globals->SendUiL()->ValidateServiceL
            (KSenduiMtmSmtpUid, TSendingCapabilities(0,0,0));
        }

    // If members list is empty, no options menu items
    if ((aFlags & KPbkControlEmpty)
        || ((aFlags & KPbkItemsMarked) && markedItemsFiltering))
        {
        // Hide "Send contact" menu
        aMenuPane.SetItemDimmed(EPbkCmdSendUICommand, ETrue);
        }
	else
		{
		aMenuPane.SetItemDimmed(EPbkCmdSendUICommand, EFalse);	
		}
    }

void CPbkGroupMembersListAppView::ContextMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags)
    {
    if (aFlags & KPbkContactListEmpty)
        {
        // Fetch members not available if names list is empty
        aMenuPane.SetItemDimmed(EPbkCmdFetchMembers, ETrue);
        }
    }

TUid CPbkGroupMembersListAppView::Id() const
    {
    return CPbkAppUi::KPbkGroupMembersListViewUid;
    }

void CPbkGroupMembersListAppView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

void CPbkGroupMembersListAppView::DoActivateL
        (const TVwsViewId& aPrevViewId,
         TUid aCustomMessageId,
         const TDesC8& aCustomMessage)
    {
    CContactGroup* group = NULL;

    if (iContainer)
        {
        iContainer->DestroyControl();
        }

    if (aCustomMessageId == CPbkViewState::Uid())
        {
        // Get the group id from the parameter
        CPbkViewState* viewState = CPbkViewState::NewLC(aCustomMessage);
        CContactItem* item = Engine()->Database().
			ReadContactL(viewState->FocusedContactId());
        if (item->Type() == KUidContactGroup)
            {
            iGroupId = viewState->FocusedContactId();
            group = static_cast<CContactGroup*>(item);
            CleanupStack::PopAndDestroy(viewState);
            CleanupStack::PushL(group);
            }
        else if (item->Type() == KUidContactCard)
            {
            iFocusedContact = viewState->FocusedContactId();
            CleanupStack::PopAndDestroy(viewState);
            delete item;
            }
        }
    if (iGroupId == KNullContactId)
        {
        // No state to activate
        User::Leave(KErrNotFound);
        }

    // Read the contact group unless already done
    if (!group)
        {
        group = Engine()->ReadContactGroupL(iGroupId);
        CleanupStack::PushL(group);
        }

    // Update application-wide state leave-safely
    TPtrC groupLabel = group->GetGroupLabelL();
    CPbkAppUi::CViewActivationTransaction* viewActivationTransaction =
        PbkAppUi()->HandleViewActivationLC
            (Id(), aPrevViewId, &groupLabel, NULL);

    // Register this object as observer to all groups view
    if (!iAllGroupsView)
        {
        iAllGroupsView = &Engine()->AllGroupsViewL();
        iAllGroupsView->OpenL(*this);
        }

    LoadGroupMembersViewL(iGroupId);

    // Create the control container
    TBool containerCreated = EFalse;
    if (!iContainer)
        {
        CContainer* container = CContainer::NewLC(this, *this);
        container->SetRect(ClientRect());

        container->SetHelpContext(TCoeHelpContext(ApplicationUid(),
        	KPHOB_HLP_GROUP_MEMBER_LIST));
        	
        CleanupStack::Pop(container);
        iContainer = container;
        containerCreated = ETrue;
        }
        
    // Create the view-side UI control
    CPbkContactViewListControl* control = CPbkContactViewListControl::NewL
        (*Engine(), *iGroupMembersView, R_PBK_GROUP_MEMBERS_LIST_VIEW_CONTROL,
		iContainer);
    iContainer->SetControl(control, ClientRect());
    control->AddObserverL(*this);
    
    if ( containerCreated )
        {
        AppUi()->AddToViewStackL(*this, iContainer);
        }
    ConstructNaviIndicatorsL();

    // Activate UI control
    iContainer->ActivateL();

    // Commit application-wide state changes
    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(2);  // viewActivationTransaction, group

    // PostCond:
    __ASSERT_DEBUG(iContainer && iContainer->Control(),
        Panic(EPanicPostCond_DoActivateL));
    }

void CPbkGroupMembersListAppView::DoDeactivate()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
		("CPbkGroupsListAppView(%x)::DoDeactivate()"), this);

    // delete navigation decorator
    delete iNaviDeco;
    iNaviDeco = NULL;

    if (iContainer)
        {
        // Remove view and its control from the view stack
        AppUi()->RemoveFromViewStack(*this, iContainer);
        // Destroy the container control
        delete iContainer;
        iContainer = NULL;
        }

    // Close contact views
    if (iGroupMembersView)
        {
        iGroupMembersView->Close(*this);
        iGroupMembersView = NULL;
        }
    if (iAllGroupsView)
        {
        iAllGroupsView->Close(*this);
        iAllGroupsView = NULL;
        }

    // PostCond:
    __ASSERT_DEBUG(!iContainer && !iNaviDeco && !iGroupMembersView
		&& !iAllGroupsView, Panic(EPanicPostCond_DoDeactivate));
    }

void CPbkGroupMembersListAppView::DeleteFocusedEntryL()
    {
    CmdRemoveFromFolderL();
    }

void CPbkGroupMembersListAppView::HandleContactViewEvent
        (const CContactViewBase& aView,
        const TContactViewEvent& aEvent)
    {
    if (&aView == iAllGroupsView &&
        aEvent.iEventType == TContactViewEvent::EReady)
        {
        iViewsLoadingReady.SetAllGroupsViewReady();
        // navi pane update
        TRAPD(err, StatusPaneUpdateL(iGroupId));
        if (err != KErrNone)
            {
            iCoeEnv->HandleError(err);
            }
        }
    }

void CPbkGroupMembersListAppView::HandleContactViewListControlEventL
        (CPbkContactViewListControl& /*aControl*/,
        const TPbkContactViewListControlEvent& aEvent)
    {
    if (aEvent.iEventType == TPbkContactViewListControlEvent::EReady)
        {
        if (iFocusedContact != KNullContactId)
            {
            TInt index = iContainer->Control()->
				FindContactIdL(iFocusedContact);
            if (index >= 0)
                {
                iContainer->Control()->SetCurrentItemIndexAndDraw(index);
                }
            }
        iFocusedContact = KNullContactId;
        iViewsLoadingReady.SetControlReady();
        // navi pane update
        StatusPaneUpdateL(iGroupId);
        }
    }

void CPbkGroupMembersListAppView::StatusPaneUpdateL
        (TContactItemId aGroupId)
    {
    // update the navi decorator
    if (iViewsLoadingReady.AreViewsReady())
        {
        // update the title pane to hold the Groups label
        CContactGroup* group = Engine()->ReadContactGroupL(aGroupId);
        CleanupStack::PushL(group);
        TitlePane()->SetTextL(group->GetGroupLabelL());
        CleanupStack::PopAndDestroy(group);

        // check the index and count of the groups to navi pane
        const TInt index = iAllGroupsView->FindL(aGroupId) + 1;

        // create navi pane text
        CArrayFixFlat<TInt>* values = new (ELeave) CArrayFixFlat<TInt>(2);
        CleanupStack::PushL(values);
        values->AppendL(index);
        values->AppendL(iAllGroupsView->CountL());
        HBufC* buf  = StringLoader::LoadLC(R_QTN_PHOB_NAVI_POS_INDICATOR,
			*values);

        CPbkIconInfoContainer* iconInfoContainer =
			CPbkIconInfoContainer::NewL(R_PBK_ICON_INFO_ARRAY, iCoeEnv);
        CleanupStack::PushL(iconInfoContainer);
        CGulIcon* icon =
                iconInfoContainer->LoadBitmapL(EPbkqgn_prop_group_open_tab1);
        __ASSERT_DEBUG(icon, Panic(EPanicIcon_StatusPaneUpdateL));
        CleanupStack::PopAndDestroy(); // iconInfoContainer
        CleanupStack::PushL(icon);

        static_cast<CAknTabGroup*>(iNaviDeco->DecoratedControl())
            ->ReplaceTabL(EPbkGroupMembersListViewId, *buf, icon->Bitmap(),
			icon->Mask());
        // tab group takes ownership of bitmap and mask
        icon->SetBitmapsOwnedExternally(ETrue);
        CleanupStack::PopAndDestroy(3); // icon, buf, values
        }
    }

void CPbkGroupMembersListAppView::LoadGroupMembersViewL
		(TContactItemId aGroupId)
    {
    // PreCond
    __ASSERT_DEBUG(Engine(),
        Panic(EPanicPreCond_LoadGroupMembersViewL));

    CContactGroupView* groupMembersView = CContactGroupView::NewL(
        Engine()->Database(), Engine()->AllContactsView(),
        *this, aGroupId, CContactGroupView::EShowContactsInGroup);
	groupMembersView->SetViewFindConfigPlugin
		(Engine()->AllContactsView().GetViewFindConfigPlugin());

    if (iGroupMembersView)
        {
        iGroupMembersView->Close(*this);
        iGroupMembersView = NULL;
        }
    iGroupMembersView = groupMembersView;

    // PostCond
    __ASSERT_DEBUG(Engine() && iGroupMembersView,
        Panic(EPanicPostCond_LoadGroupMembersViewL));
    }

void CPbkGroupMembersListAppView::ConstructNaviIndicatorsL()
    {
    // navi indicators
    CEikStatusPane* statusPane = iEikonEnv->AppUiFactory()->StatusPane();
    if (statusPane && statusPane->PaneCapabilities
		(TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
        {
        CAknNavigationControlContainer* naviPane =
			static_cast<CAknNavigationControlContainer*>
			(statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
        if (!iNaviDeco)
            {
            TResourceReader resReader;
            iCoeEnv->CreateResourceReaderLC(resReader,
				R_PBK_GROUP_MEMBERS_TAB_GROUP);
            iNaviDeco = naviPane->CreateTabGroupL(resReader);
            CleanupStack::PopAndDestroy(); // resReader
            }

        iNaviDeco->MakeScrollButtonVisible(ETrue);
        if (Engine()->Database().GroupCount() > 1)
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
    }


//  End of File
