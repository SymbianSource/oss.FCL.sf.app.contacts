/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 move top contacts dialog.
*
*/


// INCLUDE FILES
#include "CPbk2NlxMoveTopContactsDlg.h"

// Phonebook 2
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>
#include <Pbk2UIControls.rsg>

// Virtual Phonebook
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>

#include <avkon.rsg>
#include <aknlists.h>
#include <akntitle.h>
#include <aknnavi.h>

#include <eikclbd.h>
#include <CPbk2IconArray.h>

#include "CPbk2NlxReorderingModel.h"
#include "Pbk2NlxUIControls.hrh"
#include <Pbk2ExNamesListRes.rsg> 

#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::CPbk2NlxMoveTopContactsDlg
// --------------------------------------------------------------------------
inline CPbk2NlxMoveTopContactsDlg::CPbk2NlxMoveTopContactsDlg(
		CPbk2NlxReorderingModel& aModel ):
		iModel(aModel)
    {
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::~CPbk2NlxMoveTopContactsDlg
// --------------------------------------------------------------------------
CPbk2NlxMoveTopContactsDlg::~CPbk2NlxMoveTopContactsDlg()
    {
    TRAP_IGNORE(RestoreTitlePaneTextL());
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::ConstructL
// --------------------------------------------------------------------------
inline void CPbk2NlxMoveTopContactsDlg::ConstructL()
    {
    CAknDialog::ConstructL( R_AVKON_MENUPANE_EMPTY );
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::NewL
// --------------------------------------------------------------------------
CPbk2NlxMoveTopContactsDlg* CPbk2NlxMoveTopContactsDlg::NewL(
		CPbk2NlxReorderingModel& aModel )
    {
    CPbk2NlxMoveTopContactsDlg* dlg = new ( ELeave )
    	CPbk2NlxMoveTopContactsDlg( aModel );
    CleanupStack::PushL( dlg );
    dlg->ConstructL();
    CleanupStack::Pop( dlg );
    return dlg;
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::ExecuteLD
// --------------------------------------------------------------------------
TInt CPbk2NlxMoveTopContactsDlg::ExecuteLD()
    {
    return CEikDialog::ExecuteLD( R_PBK2_MOVE_TOP_CONTACTS_DLG );
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::PostLayoutDynInitL
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::PostLayoutDynInitL()
	{
	iPrevSelectedItem = iModel.FocusedContactIndex();
	if ( iPrevSelectedItem != KErrNotFound )
	    {
	    iListBox->SetCurrentItemIndexAndDraw( iPrevSelectedItem );
	    }
	CEikCaptionedControl* capControl = Line(ECtrlTopContactList);
	capControl->SetPointerEventObserver(this);
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::PreLayoutDynInitL
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::PreLayoutDynInitL()
    {
    CAknDialog::PreLayoutDynInitL();
    iListBox = static_cast<CEikColumnListBox*>(Control(ECtrlTopContactList));
    
    // set our model
    iListBox->Model()->SetItemTextArray( &iModel );
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(
            iModel.TakeIconArray() );
    iModel.SetContactUpdater( this );
    
    //setup scrollbar
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
    		CEikScrollBarFrame::EOff,
    		CEikScrollBarFrame::EAuto );
    iListBox->HandleItemAdditionL();
    
    iListBox->SetListBoxObserver( this );
    iListBox->SetObserver( this );
    
    SetNewTitleTextL();
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::SetNewTitleTextL
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::SetNewTitleTextL ()
	{
	CAknTitlePane* title = static_cast<CAknTitlePane*>(CEikonEnv::Static()->
			AppUiFactory()->StatusPane()->ControlL(
					TUid::Uid ( EEikStatusPaneUidTitle) ) );

	// Store old one
	iOldTitleText = title->Text()->AllocL ();

	// Clear the navi pane
	// Get pointer to status-pane
	CEikStatusPane* statusPane = CEikonEnv::Static()->AppUiFactory()->
	StatusPane ();
	// Get pointer to navi-pane
	CAknNavigationControlContainer * naviPane =
		static_cast<CAknNavigationControlContainer*>(statusPane->ControlL(
				TUid::Uid ( EEikStatusPaneUidNavi) ) );
	naviPane->PushDefaultL (ETrue);

	// Set the new text
	HBufC* tempTitle= CEikonEnv::Static()->AllocReadResourceLC(
			R_PBK2_MOVE_TOP_CONTACTS_DLG_TITLE );
	title->SetTextL ( *tempTitle);
	CleanupStack::PopAndDestroy (tempTitle);
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::RestoreTitlePaneTextL() 
// --------------------------------------------------------------------------
//
void CPbk2NlxMoveTopContactsDlg::RestoreTitlePaneTextL()
	{
	CAknTitlePane* title = static_cast<CAknTitlePane*>(CEikonEnv::Static()->
			AppUiFactory()->StatusPane()->ControlL(
					TUid::Uid ( EEikStatusPaneUidTitle) ) );
	if ( iOldTitleText )
		{
		title->SetTextL(*iOldTitleText);
		delete iOldTitleText;
		iOldTitleText = NULL;
		}
	else
		{
		title->SetTextToDefaultL();
		}

	// restore the navi pane
	// Get pointer to status-pane
	CEikStatusPane* statusPane =
		CEikonEnv::Static()->AppUiFactory()->StatusPane();
	// Get pointer to navi-pane
	CAknNavigationControlContainer
			* naviPane =
					static_cast<CAknNavigationControlContainer*>(
							statusPane->ControlL(
									TUid::Uid( EEikStatusPaneUidNavi) ) );
	naviPane->Pop();
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::OkToExitL
// --------------------------------------------------------------------------
TBool CPbk2NlxMoveTopContactsDlg::OkToExitL( TInt aButtonId )
    {
	iModel.SetFocusedContactL( iListBox->CurrentItemIndex() );
	iModel.SetContactUpdater( NULL ); //stop updating contacts
	return CEikDialog::OkToExitL( aButtonId );
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::OfferKeyEventL
// --------------------------------------------------------------------------
TKeyResponse CPbk2NlxMoveTopContactsDlg::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result = EKeyWasNotConsumed;
        {
        result = CEikDialog::OfferKeyEventL( aKeyEvent, aType );
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::HandleResourceChange
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::HandleResourceChange( TInt aType )
    {
    CAknDialog::HandleResourceChange(aType);
    }

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::PointerEvent
// --------------------------------------------------------------------------
bool CPbk2NlxMoveTopContactsDlg::PointerEvent(
		CEikCaptionedControl* /*aControl*/,
		const TPointerEvent& aPointerEvent )
	{
	if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
		{
		TInt itemUnder;
		if ( iListBox->View()->XYPosToItemIndex( aPointerEvent.iPosition,
				itemUnder) )
			{
			iPrevSelectedItem = itemUnder;
			}
		}
	return ETrue;
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::HandleListBoxEventL
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::HandleListBoxEventL(
		CEikListBox* /*aListBox*/,
		TListBoxEvent aEventType)
	{
	if( AknLayoutUtils::PenEnabled() )  
		{
		if ( aEventType == EEventItemDraggingActioned )
		    {
		    ContinueDragL();
		    }
		}	
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::HandleControlEventL
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::HandleControlEventL( 
		CCoeControl* aControl, 
		TCoeEvent aEventType )
	{
	CEikDialog::HandleControlEventL( aControl, aEventType );
	if ( aEventType == EEventStateChanged )
	    {
	    ContinueDragL();
	    }
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::ContinueDrag
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::ContinueDragL()
	{
	if (iPrevSelectedItem != iListBox->CurrentItemIndex())
	    {
    	iModel.Move( iPrevSelectedItem, iListBox->CurrentItemIndex() );
    	iListBox->DrawNow();
    	iPrevSelectedItem = iListBox->CurrentItemIndex();
	    }
	}

// --------------------------------------------------------------------------
// CPbk2NlxMoveTopContactsDlg::UpdateContact
// --------------------------------------------------------------------------
void CPbk2NlxMoveTopContactsDlg::UpdateContact( const MVPbkContactLink& aContactLink )
    {
    TInt index = iModel.ContactIndex( aContactLink );
    if ( index >= iListBox->TopItemIndex() &&
         index <= iListBox->BottomItemIndex() )
        {
        iListBox->DrawItem(index);
        }
    }
// End of File
