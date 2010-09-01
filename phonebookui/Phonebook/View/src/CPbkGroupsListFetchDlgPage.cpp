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
*       Provides methods for Phonebook fetch dialog page: Groups List.
*
*/


// INCLUDE FILES

#include "CPbkGroupsListFetchDlgPage.h"
#include <cntitem.h>
#include <calslbs.h>
#include <aknlayout.cdl.h>

// PbkView classes
#include <PbkView.hrh>
#include "MPbkFetchDlg.h"
#include <CPbkContactViewListControl.h>

// PbkEng classes
#include <CPbkContactEngine.h>
#include <CPbkContactIdSet.h>
#include <CPbkContactSubView.h>
#include <TPbkContactViewIterator.h>


namespace {

// LOCAL CONSTANTS AND MACROS

const TInt KPbkSelectionCountNotUsed(-1);
enum TPanicCode
    {
    EPanicPostCond_Constructor = 1,
    EPanicPreCond_HandleContactViewListControlEventL
    };

enum TStateFlags
    {
    EContactViewReady = 0x0001,
    EAllGroupsViewReady = 0x0002,
    EAllViewsReady = EContactViewReady|EAllGroupsViewReady,
    EInitializing = 0x0004
    };


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkGroupsListFetchDlgPage");
    User::Panic(KPanicText, aReason);
    }
#endif

/**
 * Returns ETrue if all the bits set in aBits are set in aFlags.
 */
inline TBool AllBitsSet(TUint aFlags, TUint aBits)
    {
    return ((aFlags & aBits)==aBits);
    }

/**
 * Returns ETrue if at least one member of group aGroup is in aView.
 */
TBool IsAnyGroupMemberInViewL
        (const CContactGroup& aGroup, const CContactViewBase& aView)
    {
    const CContactIdArray* groupMembers = aGroup.ItemsContained();
    const TInt memberCount = groupMembers ? groupMembers->Count() : 0;
    for (TInt i=0; i < memberCount; ++i)
        {
        if (aView.FindL((*groupMembers)[i]) >= 0)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

/**
 * Returns ETrue if at least one member of group aGroupId is in aView.
 */
TBool IsAnyGroupMemberInViewL
        (CPbkContactEngine& aEngine,
        TContactItemId aGroupId,
        const CContactViewBase& aView)
    {
    CContactGroup* group = aEngine.ReadContactGroupL(aGroupId);
    CleanupStack::PushL(group);
    const TBool result = IsAnyGroupMemberInViewL(*group, aView);
    CleanupStack::PopAndDestroy(group);
    return result;
    }


}  //namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkGroupsListFetchDlgPage::CPbkGroupsListFetchDlgPage
        (MPbkFetchDlg& aParentDlg) :
    iParentDlg(aParentDlg),
    iContactView(aParentDlg.FetchDlgNamesView()),
    iStateFlags(EInitializing)
    {
    __ASSERT_DEBUG(!iAllGroupsView && !iGroupSubView &&
        iStateFlags==EInitializing && !iGroupsToInclude,
        Panic(EPanicPostCond_Constructor));
    }

inline void CPbkGroupsListFetchDlgPage::ConstructL()
    {
    CPbkContactEngine& engine = iParentDlg.PbkEngine();

    iContactView.OpenL(*this);
    iAllGroupsView = &engine.AllGroupsViewL();
    iGroupSubView = CPbkContactSubView::NewL(
        *this, engine.Database(), *iAllGroupsView, *this);
    iGroupSubView->AddBaseViewPreEventHandlerL(*this);

    iControl = static_cast<CPbkContactViewListControl*>
        (iParentDlg.FetchDlgControl(ECtrlFetchGroupsList));
        
    iControl->EnableMSKObserver(EFalse); 
     
    LayoutContents();
    }

CPbkGroupsListFetchDlgPage* CPbkGroupsListFetchDlgPage::NewL
        (MPbkFetchDlg& aParentDlg)
    {
    CPbkGroupsListFetchDlgPage* self =
        new(ELeave) CPbkGroupsListFetchDlgPage(aParentDlg);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkGroupsListFetchDlgPage::~CPbkGroupsListFetchDlgPage()
    {
    if (iGroupSubView)
        {
        iGroupSubView->Close(*this);
        }
    delete iGroupsToInclude;
    iContactView.Close(*this);
    }

TInt CPbkGroupsListFetchDlgPage::FetchDlgPageId() const
    {
    return ECtrlFetchGroupsList;
    }

TBool CPbkGroupsListFetchDlgPage::DlgPageReady() const
    {
    return (iControl->IsReady());
    }

void CPbkGroupsListFetchDlgPage::ActivateFetchDlgPageL()
    {
    iControl->DisableRedrawEnablePushL();
    iControl->ClearMarks();

    const TInt groupCount = iGroupsToInclude ? iGroupsToInclude->Count() : 0;
    CPbkContactEngine& engine = iParentDlg.PbkEngine();
    const CPbkContactIdSet& selectionSet = iParentDlg.FetchDlgSelection();
    for (TInt i=0; i < groupCount; ++i)
        {
        // If all group's members which are found iContactView are marked the
        // group should be marked too
        const TContactItemId groupId = (*iGroupsToInclude)[i];
        CContactGroup* group = engine.ReadContactGroupL(groupId);
        CleanupStack::PushL(group);
        const CContactIdArray* groupMembers = group->ItemsContained();
        const TInt memberCount = groupMembers ? groupMembers->Count() : 0;
        TBool markGroup = ETrue;
        for (TInt i = 0; i < memberCount; ++i)
            {
            const TContactItemId groupMemberId = (*groupMembers)[i];
            if (iContactView.FindL(groupMemberId)!=KErrNotFound &&
                !selectionSet.Find(groupMemberId))
                {
                markGroup = EFalse;
                break;
                }
            }
        CleanupStack::PopAndDestroy(group);
        iControl->MarkItemL(groupId, markGroup);
        }
    CleanupStack::PopAndDestroy();  // DisableRedrawEnablePushL()

    if (iControl->ItemsMarked())
        {
        iControl->DrawDeferred();
        }
    }

void CPbkGroupsListFetchDlgPage::DeactivateFetchDlgPage()
    {
    }

TContactItemId CPbkGroupsListFetchDlgPage::FocusedContactIdL() const
    {
    return iControl->FocusedContactIdL();
    }

TBool CPbkGroupsListFetchDlgPage::IsFetchDlgPageEmpty() const
    {
    return (iControl->NumberOfItems() == 0);
    }

void CPbkGroupsListFetchDlgPage::SetMPbkFetchDlgSelection
		(MPbkFetchDlgSelection* aAccepter)
	{
	iFetchSelection = aAccepter;
	iControl->SetSelectionAccepter(this);
	}


TBool CPbkGroupsListFetchDlgPage::ItemsMarked() const
    {
    return iControl->ItemsMarked();
    }


TBool CPbkGroupsListFetchDlgPage::ContactSelectionAcceptedL
		(TContactItemId aGroupId, TInt /*aCurrentSelectedCount*/) const
	{
	// For each member in group that is not already selected, call the
	// accept-callback for it. If callback returns EFalse for ANY member,
	// then we must return EFalse.
	TBool selectionAccepted = ETrue;
	if (iFetchSelection)
		{
	    CContactGroup* group =
	    	iParentDlg.PbkEngine().ReadContactGroupL(aGroupId);
	    CleanupStack::PushL(group);
	    const CContactIdArray* groupMembers = group->ItemsContained();
	    if (groupMembers)
	        {
	        CPbkContactIdSet& selection = iParentDlg.FetchDlgSelection();
	        const TInt currentSelectedCount = 	selection.Count();
	        const TInt memberCount = groupMembers->Count();
	        TInt newMemberCount = 0;
	        // Go throug all members of the group
	        for (TInt i=0; i < memberCount; ++i)
	            {
	            const TContactItemId contactId = (*groupMembers)[i];
	            if (!selection.Find(contactId))
		            {
		            // This group member is not yet selected. Check if the
		            // member can be selected
		            if (!iFetchSelection->ContactSelectionAcceptedL
		            		(contactId,
		            		 currentSelectedCount + newMemberCount))
			            {
			 			// If any member is rejected from being selected,
			 			// the group can't be selected
			            selectionAccepted = EFalse;
			            break;
			            }
			        else
				        {
				        // Note: We are actually not yet adding any members
				        // to the set of selected ids. Adding occurs only
				        // if this method returns ETrue
				        newMemberCount++;
				        }
		            }
	            }
	        }
	    CleanupStack::PopAndDestroy(group);
		}
	else
		{
		selectionAccepted = ETrue;
		}
    return selectionAccepted;
	}
	
TBool CPbkGroupsListFetchDlgPage::ProcessSoftkeyMarkCommandL(TInt aCommandId)
    {
    TBool selectionAccepted(ETrue);
    
    if ( aCommandId == EAknSoftkeyMark )
        {
        selectionAccepted = ContactSelectionAcceptedL( iControl->ContactIdAtL( 
                iControl->CurrentItemIndex()), KPbkSelectionCountNotUsed);
        }
    
    if ( aCommandId == EAknSoftkeyUnmark || selectionAccepted )
        {    
        // Send event
    	TPbkContactViewListControlEvent event( TPbkContactViewListControlEvent::EContactSelected );
        event.iInt = iControl->CurrentItemIndex();
        event.iContactId = iControl->ContactIdAtL( event.iInt );    
        
        switch( aCommandId )
        	{
        	case EAknSoftkeyMark:
        		{
    			// Send event about changed state
    	        HandleContactViewListControlEventL( *iControl, event );    		        	    	        
        		break;
        		}
        		
        	case EAknSoftkeyUnmark:
        		{
    			// Send event about changed state
    			event.iEventType = TPbkContactViewListControlEvent::EContactUnselected;
    	        HandleContactViewListControlEventL( *iControl, event );    		    	        
        		break;
        		}
        		
        	default:;
            	}            
            	
            return ETrue;
            }
        
    return EFalse;
    }
    
CPbkContactViewListControl& CPbkGroupsListFetchDlgPage::Control()
    {
    return *iControl;
    }


void CPbkGroupsListFetchDlgPage::HandleContactViewListControlEventL
        (CPbkContactViewListControl& aControl,
        const TPbkContactViewListControlEvent& aEvent)
    {
    __ASSERT_DEBUG(&aControl==iControl,
        Panic(EPanicPreCond_HandleContactViewListControlEventL));

	// Suppress the unused parameter warning
	(void) aControl;

    switch (aEvent.iEventType)
        {
        case TPbkContactViewListControlEvent::EContactSelected:
            {
            HandleGroupSelectionL(aEvent.iContactId, ETrue);
            break;
            }

        case TPbkContactViewListControlEvent::EContactUnselected:
            {
            HandleGroupSelectionL(aEvent.iContactId, EFalse);
            break;
            }
        case TPbkContactViewListControlEvent::EContactSetChanged: // FALLTHROUGH
        default:
            {
            // Notify parent dialog that this page has changed
            iParentDlg.FetchDlgPageChangedL(*this);
            break;
            }
        }
    }

void CPbkGroupsListFetchDlgPage::HandleContactViewEvent
        (const CContactViewBase& aView,const TContactViewEvent& aEvent)
    {
    TRAPD(err, HandleContactViewEventL(aView,aEvent));
    if (err != KErrNone)
        {
        iParentDlg.FetchDlgHandleError(err);
        }
    }

TBool CPbkGroupsListFetchDlgPage::IsContactIncluded(TContactItemId aId)
    {
    return (iGroupsToInclude && iGroupsToInclude->Find(aId));
    }

void CPbkGroupsListFetchDlgPage::SetupControlL()
    {
    if (AllBitsSet(iStateFlags,EAllViewsReady|EInitializing))
        {
        iStateFlags &= ~EInitializing;
        CPbkContactEngine& engine = iParentDlg.PbkEngine();
        for (TInt i=0; i < iAllGroupsView->CountL(); ++i)
            {
            const TContactItemId groupId = iAllGroupsView->AtL(i);
            if (IsAnyGroupMemberInViewL(engine,groupId,iContactView))
                {
                // Add all groups which have at least one member in iContactView
                if (!iGroupsToInclude)
                    {
                    iGroupsToInclude = CPbkContactIdSet::NewL();
                    }
                iGroupsToInclude->AddL(groupId);
                }
            }
        iGroupSubView->Refresh();
        iControl->ConstructL(engine, *iGroupSubView);
        iControl->AddObserverL(*this);
        }
    }

void CPbkGroupsListFetchDlgPage::HandleGroupSelectionL
        (TContactItemId aGroupId, TBool aSelected)
    {
    CContactGroup* group = iParentDlg.PbkEngine().ReadContactGroupL(aGroupId);
    CleanupStack::PushL(group);
    const CContactIdArray* groupMembers = group->ItemsContained();
    if (groupMembers)
        {
        if (aSelected)
            {
            // Add those group members which are present in iContactView
            const TInt memberCount = groupMembers->Count();
            for (TInt i=0; i < memberCount; ++i)
                {
                const TContactItemId contactId = (*groupMembers)[i];
                if (iContactView.FindL(contactId) != KErrNotFound)
                    {
                    iParentDlg.FetchDlgSelection().AddL(contactId);
                    }
                }
            }
        else
            {
            // Remove all group members from current selection
            iParentDlg.FetchDlgSelection().Remove(*groupMembers);
            }
        }
    CleanupStack::PopAndDestroy(group);
    }

void CPbkGroupsListFetchDlgPage::HandleContactViewEventL
        (const CContactViewBase& aView,const TContactViewEvent& aEvent)
    {
    if (&aView == &iContactView)
        {
        HandleContactViewEventL(aEvent);
        }
    else if (&aView == iAllGroupsView)
        {
        HandleGroupsViewEventL(aEvent);
        }
    }

void CPbkGroupsListFetchDlgPage::HandleContactViewEventL
        (const TContactViewEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EReady:
            {
            iStateFlags |= EContactViewReady;
            SetupControlL();
            break;
            }

        default:
            {
            break;
            }
        }
    }

void CPbkGroupsListFetchDlgPage::HandleGroupsViewEventL
        (const TContactViewEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EReady:
            {
            iStateFlags |= EAllGroupsViewReady;
            SetupControlL();
            break;
            }

        case TContactViewEvent::EItemAdded:
            {
            CPbkContactEngine& engine = iParentDlg.PbkEngine();
            if ((iStateFlags & EContactViewReady) &&
                IsAnyGroupMemberInViewL(engine, aEvent.iContactId, iContactView))
                {
                if (!iGroupsToInclude)
                    {
                    iGroupsToInclude = CPbkContactIdSet::NewL();
                    }
                iGroupsToInclude->AddL(aEvent.iContactId);
                }
            break;
            }

        case TContactViewEvent::EItemRemoved:
            {
            if (iGroupsToInclude)
                {
                iGroupsToInclude->Remove(aEvent.iContactId);
                }
            break;
            }

        default:
            break;
        }
    }

void CPbkGroupsListFetchDlgPage::LayoutContents()
    {
    AknLayoutUtils::LayoutControl(
        iControl,
        iParentDlg.FetchDlgClientRect(),
        AknLayout::list_gen_pane(0));
    }

const CContactIdArray& CPbkGroupsListFetchDlgPage::MarkedItemsL() const
    {
    return iControl->MarkedItemsL();
    }

// End of File
