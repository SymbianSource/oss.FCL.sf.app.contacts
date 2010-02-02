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
*       Provides methods for Phonebook fetch dialog page: Names List.
*
*/


// INCLUDE FILES
#include "CPbkNamesListFetchDlgPage.h"
#include <calslbs.h>
#include <aknlayout.cdl.h>

#include <PbkView.hrh>
#include "MPbkFetchDlg.h"
#include <CPbkContactViewListControl.h>
#include <CPbkContactIdSet.h>

// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

const TInt KPbkSelectionCountNotUsed = -1;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_HandleContactViewListControlEventL = 1,
    EPanicPreCond_HandleContactViewEvent
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkNamesListFetchDlgPage");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

} // namespace

// ================= MEMBER FUNCTIONS =======================

inline CPbkNamesListFetchDlgPage::CPbkNamesListFetchDlgPage
        (MPbkFetchDlg& aParentDlg) :
    iParentDlg(aParentDlg),
    iContactView(aParentDlg.FetchDlgNamesView())
    {
    }

inline void CPbkNamesListFetchDlgPage::ConstructL()
    {
    iContactView.OpenL(*this);

    iControl = static_cast<CPbkContactViewListControl*>
        (iParentDlg.FetchDlgControl(ECtrlFetchNamesList));
    iControl->ConstructL(iParentDlg.PbkEngine(), iContactView);
    iControl->AddObserverL(*this);
    iControl->EnableMSKObserver(EFalse);
    LayoutContents();
    }

CPbkNamesListFetchDlgPage* CPbkNamesListFetchDlgPage::NewL
        (MPbkFetchDlg& aParentDlg)
    {
    CPbkNamesListFetchDlgPage* self = 
        new(ELeave) CPbkNamesListFetchDlgPage(aParentDlg);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CPbkNamesListFetchDlgPage::~CPbkNamesListFetchDlgPage()
    {
    iContactView.Close(*this);
    }

TInt CPbkNamesListFetchDlgPage::FetchDlgPageId() const
    {
    return ECtrlFetchNamesList;
    }

TBool CPbkNamesListFetchDlgPage::DlgPageReady() const
    {
    return (iControl->IsReady());
    }

void CPbkNamesListFetchDlgPage::ActivateFetchDlgPageL()
    {
    iControl->DisableRedrawEnablePushL();
    iControl->ResetFindL();
    iControl->ClearMarks();
    const CPbkContactIdSet& selected = iParentDlg.FetchDlgSelection();
    const TInt count = selected.Count();
    for (TInt i = 0; i < count; ++i)
        {
        iControl->MarkItemL(selected[i], ETrue);
        }
    
    CleanupStack::PopAndDestroy();  // DisableRedrawEnablePushL()
    }

void CPbkNamesListFetchDlgPage::DeactivateFetchDlgPage()
    {
    }

TContactItemId CPbkNamesListFetchDlgPage::FocusedContactIdL() const
    {
    return iControl->FocusedContactIdL();
    }

TBool CPbkNamesListFetchDlgPage::IsFetchDlgPageEmpty() const
    {
    return (iControl->NumberOfItems() == 0);
    }
    
void CPbkNamesListFetchDlgPage::SetMPbkFetchDlgSelection
		(MPbkFetchDlgSelection* aAccepter) 
	{
	iFetchSelection = aAccepter;
	iControl->SetSelectionAccepter(this);	
	}
	
	
TBool CPbkNamesListFetchDlgPage::ItemsMarked() const
    {
    return iControl->ItemsMarked();
    };

   
TBool CPbkNamesListFetchDlgPage::ContactSelectionAcceptedL
		(TContactItemId aItemId, TInt /*aCurrentSelectedCount*/) const
	{
	// If selection accepter has been set, then ask it if the contact item
	// selection can be accepted
	if (iFetchSelection)
		{
		const TInt currSelectedCount = 
	        	iParentDlg.FetchDlgSelection().Count();
	        	
		return iFetchSelection->ContactSelectionAcceptedL
			(aItemId, currSelectedCount);
		}
	else
		{
		return ETrue; 
		}
	}
	
TBool CPbkNamesListFetchDlgPage::ProcessSoftkeyMarkCommandL(TInt aCommandId)
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
    
CPbkContactViewListControl& CPbkNamesListFetchDlgPage::Control()
    {
    return *iControl;
    }

void CPbkNamesListFetchDlgPage::HandleContactViewListControlEventL
        (CPbkContactViewListControl& aControl,
        const TPbkContactViewListControlEvent& aEvent)
    {
    __ASSERT_DEBUG(&aControl==iControl, 
        Panic(EPanicPreCond_HandleContactViewListControlEventL));

	// Suppress the unreferenced parameter warning
	(void) aControl;
    
    switch (aEvent.iEventType)
        {
        case TPbkContactViewListControlEvent::EContactSelected:
            {
            HandleContactSelectionL(aEvent.iContactId, ETrue);
            break;
            }

        case TPbkContactViewListControlEvent::EContactUnselected:
            {
            HandleContactSelectionL(aEvent.iContactId, EFalse);
            break;
            }
            
        case TPbkContactViewListControlEvent::EContactSetChanged: // FALLTHROUGH
        default:
            {
            // Notify parent dialog that this page's contents has changed
            iParentDlg.FetchDlgPageChangedL(*this);
            break;
            }
        }
    }

void CPbkNamesListFetchDlgPage::HandleContactViewEvent
        (const CContactViewBase& aView,const TContactViewEvent& aEvent)
    {
    __ASSERT_DEBUG(&aView==&iContactView, Panic(EPanicPreCond_HandleContactViewEvent));

	// Suppress the unreferenced parameter warning
	(void) aView;

    switch (aEvent.iEventType)
        {
        case TContactViewEvent::EItemRemoved:
            {
            iParentDlg.FetchDlgSelection().Remove(aEvent.iContactId);
            break;
            }

        default:
            {
            break;
            }
        }
    }

void CPbkNamesListFetchDlgPage::HandleContactSelectionL
        (TContactItemId aContactId, TBool aSelected)
    {
    CPbkContactIdSet& selectionSet = iParentDlg.FetchDlgSelection();
    if (aSelected)
        {
        selectionSet.AddL(aContactId);
        }
    else
        {
        selectionSet.Remove(aContactId);
        }
    }

void CPbkNamesListFetchDlgPage::LayoutContents()
    {
    AknLayoutUtils::LayoutControl(
        iControl, 
        iParentDlg.FetchDlgClientRect(), 
        AknLayout::list_gen_pane(0));
    }

const CContactIdArray& CPbkNamesListFetchDlgPage::MarkedItemsL() const
    {
    return iControl->MarkedItemsL();
    }

// End of File
