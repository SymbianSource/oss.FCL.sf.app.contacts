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
*       Provides methods for Fetch dialog for Phonebook.
*
*/


// INCLUDE FILES
#include "CPbkFetchDlg.h"   // This class
#include <barsread.h>       // TResourceReader
#include <avkon.rsg>        // AVKON resource IDs
#include <aknnavide.h>      // CAknNavigationDecorator
#include <aknappui.h>       // CEikAppUi
#include <PbkView.rsg>              // Phonebook view dll resource IDs
#include <AknUtils.h> 
#include <CPbkContactIdSet.h>
#include <TPbkContactViewIterator.h>
#include <CPbkExtGlobals.h>

#include <PbkView.hrh>              // Phonebook view dll resource constants
#include "CPbkContextPaneIcon.h"
#include "MPbkFetchDlgPage.h"
#include "PbkFetchDlgPageFactory.h"
#include "MPbkFetchCallbacks.h"
#include "MPbkFetchDlgSelection.h"
#include <CPbkContactViewListControl.h>
#include <PbkIconInfo.h>
#include <akntabgrp.h>					// CAknTabGroup
#include <AknsUtils.h>
#include <PbkDebug.h>



/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS
enum TPanicCode
    {
    EPanicNullContactView = 1,
    EPanicPreCond_ConstructL,
    EPanicInvalidResourceData,
    EPanicPreCond_PreLayoutDynInitL,
    EPanicPostCond_PreLayoutDynInitL,
    EPanicPreCond_ResetWhenDestroyed,
    };


// ==================== LOCAL FUNCTIONS ====================

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbkFetchDlg");
    User::Panic(KPanicText, aReason);
    }

/**
 * Returns ETrue if all dialog pages in aPages are ready.
 */
TBool AllPagesReady(const MPbkFetchDlgPages& aPages)
    {
    const TInt count = aPages.DlgPageCount();
    for (TInt i=0; i < count; ++i)
        {
        if (!aPages.DlgPageAt(i).DlgPageReady())
            {
            return EFalse;
            }
        }
    return ETrue;
    }

}  // namespace


// ================= MEMBER FUNCTIONS =======================

// CPbkFetchDlg::TParams
CPbkFetchDlg::TParams::TParams() :
    iFlags(FETCH_FOCUSED),
    iResId(R_PBK_SINGLE_ENTRY_FETCH_DLG),
    iContactView(NULL),
    iKeyCallback(NULL),
    iAcceptCallback(NULL),
    iFetchSelection(NULL),                    
    iCbaId(0),    
    iFocusedEntry(KNullContactId),
    iMarkedEntries(NULL)
    {
    }

CPbkFetchDlg::TParams::operator TCleanupItem()
    {
    return TCleanupItem(Cleanup,this);
    }

void CPbkFetchDlg::TParams::Cleanup(TAny* aPtr)
    {
    TParams* self = static_cast<TParams*>(aPtr);
    self->iFocusedEntry = KNullContactId;
    delete self->iMarkedEntries;
    self->iMarkedEntries = NULL;
    }


// CPbkFetchDlg::TResData
inline void CPbkFetchDlg::TResData::ReadFromResource(TResourceReader& aReader)
    {
    iDialogId   = aReader.ReadInt32();
    iCbaId      = aReader.ReadInt32();
    iEmptyCbaId = aReader.ReadInt32();
    iNaviPaneId = aReader.ReadInt32();
    }


// CPbkFetchDlg
inline CPbkFetchDlg::CPbkFetchDlg
        (TParams& aParams, 
        CPbkContactEngine& aEngine) :
    iParams(aParams),
    iEngine(aEngine),
    iPreviousStatusPaneLayout(KErrNotFound)
    {
    // CBase::operator new(TLeave) resets other member data
    }

CPbkFetchDlg* CPbkFetchDlg::NewL
        (TParams& aParams, 
        CPbkContactEngine& aEngine)
    {
    // PreCond
    __ASSERT_ALWAYS(aParams.iContactView, Panic(EPanicNullContactView));

    CPbkFetchDlg* dlg = new(ELeave) CPbkFetchDlg(aParams, aEngine);
    CleanupStack::PushL(dlg);
    dlg->ConstructL();
    CleanupStack::Pop(dlg);
    return dlg;
    }

CPbkFetchDlg::~CPbkFetchDlg()
    {
    delete iTabSkinDelay;
    if (iSelfPtr)
        {
        *iSelfPtr = NULL;
        }
    Release(iExtGlobals);
    delete iPages;
    delete iNaviDecorator;
    delete iContextPaneIcon;
    delete iSelectedIdSet;
    delete iDialogAccepter;
    }

void CPbkFetchDlg::ResetWhenDestroyed(CPbkFetchDlg** aSelfPtr)
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr == this, 
            Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

TInt CPbkFetchDlg::ExecuteLD()
    {
    TBool canceled = EFalse;
    iCanceledPtr = &canceled;
    iContextPaneIcon = CPbkContextPaneIcon::NewL(*iEikonEnv);
    
    // Status pane layout switching made.
    // Otherwise status pane layout problems could occur 
    // e.g. when no contacts in fetch dialog.
    // Calling app has to restore the layout if different.
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    TInt statusPaneLayout = KErrNotFound;
    if (statusPane)
        {
        statusPaneLayout = statusPane->CurrentLayoutResId();
        statusPane->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }
    TInt res = CEikDialog::ExecuteLD(iResData.iDialogId);

    if (statusPane && (statusPaneLayout != KErrNotFound))
        {
        statusPane->SwitchLayoutL(statusPaneLayout);
        }

    return canceled ? 0 : res;
    }

void CPbkFetchDlg::ConstructL()
    {
    __ASSERT_DEBUG(iParams.iContactView, Panic(EPanicPreCond_ConstructL));

    // Read resources
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, iParams.iResId);
    iResData.ReadFromResource(reader);
    __ASSERT_ALWAYS(iResData.iDialogId != 0, Panic(EPanicInvalidResourceData));
    __ASSERT_ALWAYS(iResData.iCbaId != 0, Panic(EPanicInvalidResourceData));
    __ASSERT_ALWAYS(iResData.iEmptyCbaId != 0, Panic(EPanicInvalidResourceData));
    CleanupStack::PopAndDestroy();  // reader

    iSelectedIdSet = CPbkContactIdSet::NewL();

    // Keep a handle to the UI extension factory in this dialog to prevent 
    // multiple inits of the factory by this dialog's subobjects
    iExtGlobals = CPbkExtGlobals::InstanceL();
    
    TCallBack accepter( TryAcceptSelectionL, this );
    iDialogAccepter =
        new (ELeave) CAsyncCallBack( accepter, CActive::EPriorityStandard );

    CAknDialog::ConstructL(R_PBK_EMPTY_MENU_PANE);

    // dialog tabs appears status pane stack only after dialog is fully
    // constructed, so we need to delay tab skinning. Even in PostLayoutDyninit
    // tab group is wrong
    iTabSkinDelay = CIdle::NewL( CActive::EPriorityLow );
    iTabSkinDelay->Start( TCallBack( DelaySkinning, this ));
    }

SEikControlInfo CPbkFetchDlg::CreateCustomControlL(TInt aControlType)
    {
    SEikControlInfo ctrl;
    ctrl.iControl = NULL;
    ctrl.iTrailerTextId = 0;
    ctrl.iFlags = 0;
    ctrl.iControl = PbkFetchDlgPageFactory::CreateCustomControlL(aControlType);
    return ctrl;
    }

void CPbkFetchDlg::PreLayoutDynInitL()
    {
    // PreCond
    __ASSERT_DEBUG(iParams.iContactView, Panic(EPanicPreCond_PreLayoutDynInitL));

    CEikDialog::PreLayoutDynInitL();

    SetupStatusPaneL();

    iPages = PbkFetchDlgPageFactory::CreatePagesL(*this);
    if (iParams.iFetchSelection)
	    {
	    iPages->DlgPageWithId(ECtrlFetchNamesList)->
	    	SetMPbkFetchDlgSelection(iParams.iFetchSelection);
	    iPages->DlgPageWithId(ECtrlFetchGroupsList)->
	    	SetMPbkFetchDlgSelection(iParams.iFetchSelection);
	    }
    iCurrentPage = iPages->DlgPageWithId(ECtrlFetchNamesList);
    iCurrentPage->ActivateFetchDlgPageL();

    if ( AknLayoutUtils::MSKEnabled() )
        {
        // Set this as MSK observer to be able to receive MSK events.       
        CEikButtonGroupContainer& bgc = ButtonGroupContainer();            
        CEikCba* cba;
        cba = ( static_cast<CEikCba*>( bgc.ButtonGroup() ) ); // downcast from MEikButtonGroup
        cba->SetMSKCommandObserver(this);    
        }
        
    if ( AknLayoutUtils::PenEnabled() )
        {
        // Add observer for contact item double tap events
        const TInt count = iPages->DlgPageCount();
        for ( TInt i = 0; i < count; ++i )
            {
            iPages->DlgPageAt(i).Control().AddObserverL(*this);
            }
        }
    
    __ASSERT_DEBUG(iPages && iCurrentPage, Panic(EPanicPostCond_PreLayoutDynInitL));
    }
    
void CPbkFetchDlg::ProcessCommandL(TInt aCommandId)
    {
    // This method gets called if CPbkFetckDlg has been set as
    // MSK observer. 
    
    // close the menu, otherwise problems with FEP
    HideMenu();
    
    if (!iCurrentPage->IsFetchDlgPageEmpty() &&
        iCurrentPage->ProcessSoftkeyMarkCommandL(aCommandId))
        {        
        if ( aCommandId == EAknSoftkeyMark )
            {
            iCurrentPage->Control().MarkItemL(iCurrentPage->FocusedContactIdL(), ETrue);
            }
        else if ( aCommandId == EAknSoftkeyUnmark )
            {
            iCurrentPage->Control().MarkItemL(iCurrentPage->FocusedContactIdL(), EFalse);
            }
        // update the label of MSK
        UpdateMSKL( iCurrentPage->Control().CurrentItemIndex() );
        }
    }

TBool CPbkFetchDlg::OkToExitL(TInt aButtonId)
    {    
    // normally OkToExitL -method is called when we are really
    // exiting this dialog, but since the MSK came to the 
    // picture, all the MSK related softkeys are handled here.
    // so, we're not going to exit the dialog, just ignore all
    // softkey events related to MSK, those are handled in ProcessCommandL.
    if ( aButtonId == EAknSoftkeyMark ||
         aButtonId == EAknSoftkeyUnmark ||
         aButtonId == EAknSoftkeyEmpty )
        {                    
        return EFalse;        
        }       
    
    // Reset return values
    TBool result = ETrue;
    iParams.iFocusedEntry = KNullContactId;
    iParams.iMarkedEntries = NULL;
    
	

    if (iParams.iFlags & FETCH_FOCUSED)
        {
        const TContactItemId focusId = 
            iPages->DlgPageWithId(ECtrlFetchNamesList)->FocusedContactIdL();
        if (focusId != KNullContactId)
            {
            iParams.iFocusedEntry = focusId;
            }
        else
            {
            if (!(iParams.iFlags & FETCH_MARKED))
                {
                // Don't close the dialog if there is no focus, probably
                // the focus is missing because find text has filtered
                // the list box empty.
                result = EFalse;
                }
            }
        }

    if (iParams.iFlags & FETCH_MARKED)
        {
        // Copy marked entries set to iParams.iMarkedEntries in the same order
        // they appear in iParams.iContactView        
        TPbkContactViewIterator iter(*iParams.iContactView);
        TContactItemId contactId;

        delete iParams.iMarkedEntries;
        iParams.iMarkedEntries = NULL;
        iParams.iMarkedEntries = CContactIdArray::NewLC();
        
        while ((contactId = iter.NextL()) != KNullContactId)
            {
            if (iSelectedIdSet->Find(contactId))
                {
                iParams.iMarkedEntries->AddL(contactId);
                }
            }
        
        // Can't destroy the iMarkedEntries, these entries need to return to client as
        // the fetch datum.
        CleanupStack::Pop(iParams.iMarkedEntries);
        }

    // Run accept callback if specified and this dialog is being accepted
    if (result && iParams.iAcceptCallback)
        {
        MPbkFetchDlgAccept::TPbkFetchAccepted res = 
            iParams.iAcceptCallback->PbkFetchAcceptedL(iParams.iFocusedEntry, iParams.iMarkedEntries);
        switch (res)
            {
            case MPbkFetchDlgAccept::KFetchYes:
                {
                result = ETrue;
                break;
                }
            case MPbkFetchDlgAccept::KFetchCanceled:
                {
                iParams.iFocusedEntry = KNullContactId;
                delete iParams.iMarkedEntries;
                iParams.iMarkedEntries = NULL;
                result = ETrue;
                if (iCanceledPtr) 
                    {
                    *iCanceledPtr = ETrue;
                    }
                break;
                }
            case MPbkFetchDlgAccept::KFetchNo:
                {
                result = EFalse;
                break;
                }
            }
        }

    switch (aButtonId)
        {
        case EAknSoftkeyBack:       // FALLTHROUGH
        case EAknSoftkeyCancel:     // FALLTHROUGH
        case EAknSoftkeyClose:      // FALLTHROUGH
        case EAknSoftkeyNo:         // FALLTHROUGH
        case EAknSoftkeyExit:
            {
            if (iCanceledPtr) 
                {
                *iCanceledPtr = ETrue;
                }
            break;
            }

        case EAknSoftkeyOk:
            {
            // Markable list -dialog shouldn't be closed if there are no marked
            // items in current page. This happens if a selection accepter
            // didn't accept the selection of focused item when EAknSoftkeyOk
            // was pressed. If we close the dialog the User might think that
            // selection succeeded. Now we stay in the dialog and give the
            // selection accepter a change to show some error message about
            // failed selection
            if ( result && (iParams.iFlags & FETCH_MARKED) && 
                iCurrentPage && !iCurrentPage->ItemsMarked())
                {
                result = EFalse;
                }
            break;
            } 
        }

    return result;
    }

void CPbkFetchDlg::PageChangedL(TInt aPageId)
    {
    MPbkFetchDlgPage* newPage = iPages->DlgPageWithId(aPageId);
    if (newPage)
        {
        newPage->ActivateFetchDlgPageL();
        if (iCurrentPage)
            {
            iCurrentPage->DeactivateFetchDlgPage();
            }
        iCurrentPage = newPage;
        	    
	    if ( iParams.iFlags & FETCH_FOCUSED )
	        {
	        // this is for single fetch	                  	    
	        UpdateCbasL();	        
	        }
	    else
	        {	        
            // for multiple fetch and MSK
            // first update the CBA set
            UpdateCbasL();
	        TInt index = iCurrentPage->Control().CurrentItemIndex();
	        if ( index != KErrNotFound && AknLayoutUtils::MSKEnabled())
                {	            
	            // if MSK enable, update it.
	            UpdateMSKL(index);
	            }
            
            }
        }
    }
    
void CPbkFetchDlg::UpdateMSKL( TInt aIndex )
    {    
	// do not update MSK for single fetch dialog
    if ( !(iParams.iFlags & FETCH_FOCUSED) )                
        {
        
        TBool hasSelected (iCurrentPage->Control().ItemMarked(aIndex));
        
        if ( hasSelected )
            {
            CEikButtonGroupContainer& cba = ButtonGroupContainer();
            cba.SetCommandL(3,R_AVKON_SOFTKEY_UNMARK);
            cba.DrawDeferred();
            }        
        else
            {
            CEikButtonGroupContainer& cba = ButtonGroupContainer();            
            cba.SetCommandL(3,R_AVKON_SOFTKEY_MARK);
            cba.DrawDeferred();        
            }
        }   
    }

void CPbkFetchDlg::HandleContactViewListControlEventL(
        CPbkContactViewListControl& /*aControl*/,
        const TPbkContactViewListControlEvent& aEvent)
    {
    switch ( aEvent.iEventType )
        {
        case TPbkContactViewListControlEvent::EContactDoubleTapped:
            {
            if ( iParams.iFlags & FETCH_FOCUSED )
                {
                // Single item/entry fetch, try to accept the dialog
                iDialogAccepter->Cancel();
                iDialogAccepter->CallBack();
                break;
                }
            // else fallthrough
            }
        case TPbkContactViewListControlEvent::EContactTapped:   // fallthrough
        case TPbkContactViewListControlEvent::EContactSelected: // fallthrough
        case TPbkContactViewListControlEvent::EContactUnselected:
            {
            UpdateMSKL( iCurrentPage->Control().CurrentItemIndex() );
            break;
            }
        default:;
        }
    
    if ( aEvent.iEventType ==
            TPbkContactViewListControlEvent::EContactDoubleTapped ||
         aEvent.iEventType ==
            TPbkContactViewListControlEvent::EContactTapped )
        {
        if ( iCurrentPage->Control().ItemMarked(
                iCurrentPage->Control().CurrentItemIndex() ) )
            {
            iCurrentPage->ProcessSoftkeyMarkCommandL( EAknSoftkeyMark );
            }
        else
            {
            iCurrentPage->ProcessSoftkeyMarkCommandL( EAknSoftkeyUnmark );
            }
        }
    }        

TInt CPbkFetchDlg::TryAcceptSelectionL( TAny* aSelf )
    {
    static_cast<CPbkFetchDlg*>( aSelf )->TryExitL( EEikBidOk );
    return KErrNone;
    }

TKeyResponse CPbkFetchDlg::OfferKeyEventL
        (const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    // if the MSK is enabled, multiple dialog needs to know
    // which contact is going to be focused and set the 
    // label of MSK referring is the certain contact marked
    // or not, or has limited multiple fetch count already
    // achieved.
    if ( AknLayoutUtils::MSKEnabled() )
        {
        if ( !iCurrentPage->IsFetchDlgPageEmpty() )
            {
            if ( ( aKeyEvent.iCode == EKeyUpArrow || aKeyEvent.iCode == EKeyDownArrow ) &&
                 aType == EEventKey )
                {        
                TInt index = iCurrentPage->Control().CurrentItemIndex();                            
                
                if ( aKeyEvent.iCode == EKeyUpArrow )
                    {
                    index--;
                    if (index < 0)
                        {
                        // set the index to the last in the list
                        index = iCurrentPage->Control().ItemCount() - 1;                    
                        }     
                    }
                if ( aKeyEvent.iCode == EKeyDownArrow )
                    {
                    index++;
                    if (index >= iCurrentPage->Control().ItemCount() )
                        {
                        // set the index to first in the list
                        index = 0;
                        }     
                    }                                       
                UpdateCbasL();       
                UpdateMSKL( index );
                }
            }
        }

    if (iParams.iKeyCallback)
        {
        MPbkFetchKeyCallback::TResult result = 
            iParams.iKeyCallback->PbkFetchKeyCallbackL(aKeyEvent, aType);
        
        switch (result)
            {
            case MPbkFetchKeyCallback::EKeyWasNotConsumed:
                {
                break;
                }
            case MPbkFetchKeyCallback::EKeyWasConsumed:
                {
                return EKeyWasConsumed;
                }
            case MPbkFetchKeyCallback::EAccept:
                {
                TryExitL(EEikBidOk);
                return EKeyWasConsumed;
                }
            case MPbkFetchKeyCallback::ECancel:
                {
                TryExitL(EEikBidCancel);
                return EKeyWasConsumed;
                }
			default:
				{
				break;
				}
            }
        }
    return CEikDialog::OfferKeyEventL(aKeyEvent, aType);
    }

CCoeControl* CPbkFetchDlg::FetchDlgControl(TInt aCtrlId) const
    {
    return ControlOrNull(aCtrlId);
    }

CContactViewBase& CPbkFetchDlg::FetchDlgNamesView() const
    {
    return *iParams.iContactView;
    }

CPbkContactIdSet& CPbkFetchDlg::FetchDlgSelection()
    {
    return *iSelectedIdSet;
    }

void CPbkFetchDlg::FetchDlgHandleError(TInt aError)
    {
    iEikonEnv->HandleError(aError);
    }

TRect CPbkFetchDlg::FetchDlgClientRect() const
    {
    TRect appRect = iAvkonAppUi->ApplicationRect();
    TAknLayoutRect mainPane;
    mainPane.LayoutRect(appRect, AKN_LAYOUT_WINDOW_main_pane(appRect,0,1,1));
    return mainPane.Rect();    
    }

CPbkContactEngine& CPbkFetchDlg::PbkEngine()
    {
    return iEngine;
    }

void CPbkFetchDlg::FetchDlgPageChangedL(MPbkFetchDlgPage& /*aPage*/)
    {
    if ( iParams.iFlags & FETCH_FOCUSED )
        {
        UpdateCbasL();	        
        }
    else
        {
        UpdateCbasL();
        if ( AknLayoutUtils::MSKEnabled() )
            {
            // update also the MSK
            TInt index = iCurrentPage->Control().CurrentItemIndex();
	        if ( index != KErrNotFound )
	            {                
                UpdateMSKL(index);
                }
            }        
        }
    }

void CPbkFetchDlg::SetCbaCommandSetL(TInt aResourceId)
    {
    if (aResourceId != iCbaCommandSet)
        {
        CEikButtonGroupContainer& cba = ButtonGroupContainer();
        cba.SetCommandSetL(aResourceId);
        iCbaCommandSet = aResourceId;
        cba.DrawDeferred();
        }
    }

void CPbkFetchDlg::SetupStatusPaneL()
    {
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if (statusPane)
        {
        // Setup navi pane if defined in resources
        if (iResData.iNaviPaneId && 
            statusPane->PaneCapabilities
            (TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
            {
            CAknNavigationControlContainer* naviPane = 
                static_cast<CAknNavigationControlContainer*> 
                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
            TResourceReader reader;
            iCoeEnv->CreateResourceReaderLC(reader, iResData.iNaviPaneId);
            iNaviDecorator =
                naviPane->ConstructNavigationDecoratorFromResourceL(reader);
            naviPane->PushL(*iNaviDecorator);
            CleanupStack::PopAndDestroy();  // reader
            }
        }
    }

void CPbkFetchDlg::HandleResourceChange(TInt aType)
    {    
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        // Layout dialog itself
        SetRect( iAvkonAppUi->ClientRect() );
        // Layout dialog page controls
        const TInt count(iPages->DlgPageCount());
        for (TInt i = 0; i < count; ++i)
            {
            iPages->DlgPageAt(i).LayoutContents();
            }
        // If the fetch dialog has been launched in landscape mode, 
        // and then later changed to the portrait mode, the icon has to 
        // be set again, otherwise there will be shown the icon of
        // the "parent" -application.
        delete iContextPaneIcon;            
        iContextPaneIcon = NULL;        
        TRAP_IGNORE(iContextPaneIcon = CPbkContextPaneIcon::NewL(*iEikonEnv));

        if (statusPane)
            {
            TRAP_IGNORE(statusPane->SwitchLayoutL(
                R_AVKON_STATUS_PANE_LAYOUT_USUAL));
            }
        }
    else if (aType == KAknsMessageSkinChange)
        {
        // Just visual effect, no need to handle error
        TRAP_IGNORE( SkinTabsL() );
        }
        
    if (statusPane)
		{
		statusPane->HandleResourceChange(aType);
		}    
		
	CAknDialog::HandleResourceChange(aType);
    }
    
// --------------------------------------------------------------------------
// CPbkFetchDlg::LineChangedL
// --------------------------------------------------------------------------
//
void CPbkFetchDlg::LineChangedL(TInt /*aControlId*/)
    {
    // never gets called
    }

// --------------------------------------------------------------------------
// CPbkFetchDlg::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPbkFetchDlg::UpdateCbasL()
    {
    if ( AllPagesReady(*iPages) && 
    		iCurrentPage && !iCurrentPage->IsFetchDlgPageEmpty() )
    	{       		        	
    	// Use user defined cba resources if exist
	    if (iParams.iCbaId)
	        {
	        SetCbaCommandSetL(iParams.iCbaId);
	        }    	        
	    else if ( iResData.iCbaId )
	        {
	        SetCbaCommandSetL( iResData.iCbaId );
	        }
	    
	    // Resources for single fetch
	    else if ( iParams.iFlags & FETCH_FOCUSED )
	    	{
	    	SetCbaCommandSetL( R_PBK_SOFTKEYS_OK_BACK_OK );
	    	} 
	    // Resources for multiple fetch
	    else
	        {
	        SetCbaCommandSetL(R_PBK_SOFTKEYS_OK_BACK_MARK);	  
	        }
    	}
    else
    	{    	
    	SetCbaCommandSetL(iResData.iEmptyCbaId);
    	}
    }    

// --------------------------------------------------------------------------
// CPbkFetchDlg::SkinTabsL
// --------------------------------------------------------------------------
//
void CPbkFetchDlg::SkinTabsL()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("SkinTabsL start"));
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();

    if (statusPane && statusPane->PaneCapabilities
		(TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
        {
        CAknNavigationControlContainer* naviPane =
			static_cast<CAknNavigationControlContainer*>
			(statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
        CAknNavigationDecorator* naviDeco = naviPane->Top();

        if ( naviDeco )
            {
            PBK_DEBUG_PRINT(PBK_DEBUG_STRING("SkinTabsL naviDeco"));
            if ( naviDeco->ControlType() == CAknNavigationDecorator::ETabGroup )
                {
                PBK_DEBUG_PRINT(PBK_DEBUG_STRING("SkinTabsL ETabGroup"));
                CPbkIconInfoContainer* iconInfoContainer = 
                    CPbkIconInfoContainer::NewL( R_PBK_FETCH_TAB_ICON_INFO_ARRAY );
                CleanupStack::PushL(iconInfoContainer);

                MAknsSkinInstance* skin = AknsUtils::SkinInstance();
                CAknTabGroup* tabGroup = 
                    static_cast<CAknTabGroup*>(naviDeco->DecoratedControl());
                                
                const TInt count = tabGroup->TabCount();
                PBK_DEBUG_PRINT(PBK_DEBUG_STRING("SkinTabsL TabCount = %d"), count);
                for (TInt i = 0; i < count; ++i)
                    {
                    TInt tabId = tabGroup->TabIdFromIndex(i);
                    // tabId is used as icon id
                    const TPbkIconInfo* iconInfo = 
                        iconInfoContainer->Find(TPbkIconId(tabId));
                    if (iconInfo)
                        {
                        CFbsBitmap* bitmap = NULL;
                        CFbsBitmap* mask = NULL;
                        PbkIconUtils::CreateIconLC(
                            skin, bitmap, mask, *iconInfo);

                        PBK_DEBUG_PRINT(PBK_DEBUG_STRING("SkinTabsL ReplaceTabL %d"), tabId);
                        tabGroup->ReplaceTabL(tabId, bitmap, mask);

                        CleanupStack::Pop(2); // mask, bitmap
                        }
                    }
                CleanupStack::PopAndDestroy( iconInfoContainer );
                }
            }
        }
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("SkinTabsL end"));
    }

// --------------------------------------------------------------------------
// CPbkFetchDlg::SkinTabsL
// --------------------------------------------------------------------------
//
TInt CPbkFetchDlg::DelaySkinning( TAny* aFetchDlg )
    {
    // Tab skinning is just visual effect, no need to handle error
    TRAP_IGNORE( ( (CPbkFetchDlg*)aFetchDlg)->SkinTabsL() );
    return EFalse; // one time only
    }

//  End of File  
