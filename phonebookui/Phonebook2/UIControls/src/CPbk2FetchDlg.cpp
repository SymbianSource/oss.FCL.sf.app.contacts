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
* Description:  Phonebook 2 fetch dialog.
*
*/


// INCLUDE FILES
#include <CPbk2FetchDlg.h>

// Phonebook 2
#include "MPbk2FetchDlgPage.h"
#include "MPbk2FetchDlgPages.h"
#include "Pbk2FetchDlgPageFactory.h"
#include "CPbk2FetchResults.h"
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>
#include <Pbk2UIControls.hrh>
#include <MPbk2ContactUiControl.h>
#include <MPbk2FetchDlgObserver.h>
#include <Pbk2UIControls.rsg>
#include <MPbk2ContactLinkIterator.h>
#include <CPbk2IconInfoContainer.h>
#include <Pbk2UID.h>
#include <CPbk2IconFactory.h>
#include <TPbk2IconId.h>
#include <MPbk2ExitCallback.h>
#include <TPbk2DestructionIndicator.h>
#include "CPbk2NamesListControl.h"
#include <CPbk2AppUiBase.h>
#include <MPbk2KeyEventHandler.h>

// Virtual Phonebook
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactGroup.h>
#include <VPbkContactView.hrh>

// System includes
#include <barsread.h>
#include <avkon.rsg>
#include <aknnavide.h>
#include <aknappui.h>
#include <StringLoader.h>
#include <akntabgrp.h>
#include <AknsUtils.h>
#include <layoutmetadata.cdl.h>

// Debugging headers
#include <Pbk2Debug.h>


/// Unnamed namespace for local definitions
namespace {

const TInt KPbk2MSKControlId( CEikButtonGroupContainer::EMiddleSoftkeyPosition );
const TInt KFirstElement = 0;
const TInt KDefaultMinSelection = 1;

enum TPanicCode
    {
    EPanicInvalidResourceData = 1,
    EPanicPostCond_PreLayoutDynInitL,
    EPanicPreCond_ResetWhenDestroyed,
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2FetchDlg");
    User::Panic(KPanicText, aReason);
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2FetchDlg::TParams::TParams
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2FetchDlg::TParams::TParams() :
        iFlags( EFetchSingle ),
        iResId( R_PBK2_SINGLE_ENTRY_FETCH_DLG ),
        iMarkedEntries( NULL ),
        iNamesListView( NULL ),
        iGroupsListView( NULL ),
        iCbaId( 0 ),
        iNaviPaneId( 0 ),
        iExitCallback( NULL ),
        iMinSelection( KDefaultMinSelection )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::TParams::TCleanupItem
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2FetchDlg::TParams::operator TCleanupItem()
    {
    return TCleanupItem(Cleanup,this);
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::TParams::Cleanup
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::TParams::Cleanup(TAny* aPtr)
    {
    TParams* self = static_cast<TParams*>(aPtr);
    delete self->iMarkedEntries;
    self->iMarkedEntries = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::TResData::ReadFromResource
// --------------------------------------------------------------------------
//
inline void CPbk2FetchDlg::TResData::ReadFromResource
        ( TResourceReader& aReader )
    {
    iDialogId = aReader.ReadInt32();
    iCbaId = aReader.ReadInt32();
    iEmptyCbaId = aReader.ReadInt32();
    iNaviPaneId = aReader.ReadInt32();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::CPbk2FetchDlg
// --------------------------------------------------------------------------
//
inline CPbk2FetchDlg::CPbk2FetchDlg
        ( TParams aParams, MPbk2FetchDlgObserver& aObserver ) :
            iParams( aParams ),
            iObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::~CPbk2FetchDlg
// --------------------------------------------------------------------------
//
CPbk2FetchDlg::~CPbk2FetchDlg()
    {
    if (iDestroyedPtr)
        {
        *iDestroyedPtr = ETrue;
        }

    if ( iSelfPtr )
        {
        *iSelfPtr = NULL;
        }

    RemoveCommandFromMSK();
    delete iTabSkinDelay;
    delete iSelectionRestorer;
    delete iPages;
    delete iNaviDecorator;
    delete iDialogAccepter;
    delete iResults;
    delete iFocusedContactLink;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FetchDlg::ConstructL()
    {
    // Read resources
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, iParams.iResId );
    iResData.ReadFromResource( reader );
    __ASSERT_ALWAYS( iResData.iDialogId != 0,
        Panic( EPanicInvalidResourceData ) );
    __ASSERT_ALWAYS( iResData.iCbaId != 0,
        Panic( EPanicInvalidResourceData ) );
    __ASSERT_ALWAYS( iResData.iEmptyCbaId != 0,
        Panic( EPanicInvalidResourceData ) );
    CleanupStack::PopAndDestroy(); // reader

    iExitRecord.ClearAll();

    CAknDialog::ConstructL( R_AVKON_MENUPANE_EMPTY );

    TCallBack accepter( TryAcceptSelectionL, this );
    iDialogAccepter =
        new (ELeave) CAsyncCallBack( accepter, CActive::EPriorityIdle );

    // Dialog tabs appear in status pane stack only after dialog is fully
    // constructed, so we need to delay tab skinning.
    // Even in PostLayoutDyninit tab group is wrong.
    // Must use higher priority than user input, so the user
    // has no chance to write into find box. Tab skinning resets
    // the find input.
    iTabSkinDelay = CIdle::NewL( CActive::EPriorityHigh );
    iTabSkinDelay->Start( TCallBack( DelaySkinning, this ));
	
    // Initialize whether MSK is enabled in current layout.
    // Currently AknLayoutUtils::MSKEnabled() returns ETrue
    // if the application supports MSK (constructed with EAknEnableMSK flag).
    // In addition Layout_Meta_Data::IsMSKEnabled() is expected to be called,
    // which checks from the layout data whether or not the MSK is supported 
    // in the current resolution/orientation. 
    // So if either one returns EFalse, then MSK is not shown.
    iMSKEnabled = (AknLayoutUtils::MSKEnabled() && Layout_Meta_Data::IsMSKEnabled());
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2FetchDlg* CPbk2FetchDlg::NewL
        ( TParams aParams, MPbk2FetchDlgObserver& aObserver )
    {
    CPbk2FetchDlg* dlg = new ( ELeave ) CPbk2FetchDlg( aParams, aObserver );
    CleanupStack::PushL( dlg );
    dlg->ConstructL();
    CleanupStack::Pop( dlg );
    return dlg;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::AcceptDelayedFetchL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2FetchDlg::AcceptDelayedFetchL
        ( const TDesC8& aContactLink )
    {
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC
        ( aContactLink, Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().ContactStoresL() );

    if ( linkArray->Count() > 0 )
        {
        const MVPbkContactLink& link = linkArray->At( KFirstElement );
        iResults->AppendDelayedL( link );
        }

    CleanupStack::PopAndDestroy(); // linkArray
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::ExecuteLD
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2FetchDlg::ExecuteLD()
    {
    return CEikDialog::ExecuteLD( iResData.iDialogId );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::RequestExitL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::RequestExitL( TInt aCommandId )
    {
    iExitRecord.Set( EExitApproved );
    TryExitL( aCommandId );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::ForceExit
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::ForceExit()
    {
    iExitRecord.Set( EExitApproved );
    iExitRecord.Set( EExitOrdered );

    TRAPD( err, TryExitL( EAknSoftkeyBack ) );
    if ( err != KErrNone )
        {
        // If not nicely then use the force
        delete this;
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::ResetWhenDestroyed
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::ResetWhenDestroyed( MPbk2DialogEliminator** aSelfPtr )
    {
    __ASSERT_DEBUG(!aSelfPtr || *aSelfPtr == this,
        Panic(EPanicPreCond_ResetWhenDestroyed));

    iSelfPtr = aSelfPtr;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::CreateCustomControlL
// --------------------------------------------------------------------------
//
SEikControlInfo CPbk2FetchDlg::CreateCustomControlL( TInt aControlType )
    {
    SEikControlInfo ctrl;
    ctrl.iControl = NULL;
    ctrl.iTrailerTextId = 0;
    ctrl.iFlags = 0;
    ctrl.iControl = Pbk2FetchDlgPageFactory::CreateCustomControlL
        ( aControlType, this, *this );
    
    if ( aControlType == EPbk2CtNamesContactViewList )
        {
        iNamesListControl = static_cast<CPbk2NamesListControl*>(ctrl.iControl);
        }
    else if ( aControlType == EPbk2CtGroupsContactViewList )
        {
        iGroupListControl = static_cast<CPbk2NamesListControl*>(ctrl.iControl);
        }
    
    return ctrl;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::PreLayoutDynInitL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::PreLayoutDynInitL()
    {
    CAknDialog::PreLayoutDynInitL();

    SetupStatusPaneL();

    iPages = Pbk2FetchDlgPageFactory::CreatePagesL
        ( *this, Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager() );
    iPages->ActiveFirstPageL();

    iResults = CPbk2FetchResults::NewL( Phonebook2::Pbk2AppUi()->
        ApplicationServices().ContactManager(),
        *this, *iPages, iObserver, *this );

    // Use the current page view because we know the first page is
    // names list view that contains all contacts that can be marked
    PageChangedL(iPages->CurrentPage().FetchDlgPageId());

    __ASSERT_DEBUG(iPages, Panic(EPanicPostCond_PreLayoutDynInitL));
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::OkToExitL
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlg::OkToExitL( TInt aButtonId )
    {
    TBool okToExit = ETrue;
    TBool canceled = Canceled( aButtonId );
    TBool aborted = ( aButtonId == EEikBidCancel );

    if ( iExitRecord.IsSet( EExitApproved ) )
        {
        okToExit = ETrue;
        }
    else
        {
        if ( !canceled )
            {
            okToExit = CheckIsOkToExitL( aButtonId );
            }
        }

    // Notify observer
    if ( canceled )
        {
        iObserver.FetchCanceled();
        }
    else if ( okToExit && aborted )
        {
        // End key exit aborts the fetch
        iObserver.FetchAborted();
        ExitApplication( EAknCmdExit );
        }
    else if ( okToExit && !iExitRecord.IsSet( EExitApproved ) )
        {
        // Do not announce fetch completion if exit has already
        // been approved by the client
        TBool thisDestroyed = EFalse;
        iDestroyedPtr = &thisDestroyed;
        TPbk2DestructionIndicator indicator
            ( &thisDestroyed, iDestroyedPtr );


        iObserver.FetchCompletedL( &FetchDlgSelection() );

        if ( thisDestroyed )
            {
            // If this has been destroyed, don't return ok since
            // returning ETrue would make the base class to
            // commence more prosessing with already deleted objects
            okToExit = EFalse;
            }
        }

    // Client can prevent the exit here at the last step. This query
    // has to be done after the client has been told about fetch
    // completion/cancellation. If client has already approved
    // exit, do not query this.
    if ( okToExit && !iExitRecord.IsSet( EExitApproved ) &&
         !iObserver.FetchOkToExit() )
        {
        okToExit = EFalse;
        }

    if ( okToExit )
        {
        // Reset results
        iResults->ResetAndDestroy();
        }
    
    iExitRecord.Set( EExitOrdered );    // exit is now ordered and
                                        // when client later requests
                                        // exit, the ordered exit
                                        // becomes approved exit

    return okToExit;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::PageChangedL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::PageChangedL( TInt aPageId )
    {
    iPages->HandlePageChangedL( aPageId );
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::OfferKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2FetchDlg::OfferKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result = EKeyWasNotConsumed;

    TBool selectContact = EFalse;
    if ( aKeyEvent.iCode == EKeyOK &&
            ( iParams.iFlags & EFetchSingle ) )
        {
        selectContact = ETrue;
        }
    else if ( aKeyEvent.iCode == EKeyPhoneSend )
        {
        if ( iParams.iFlags & EFetchCallItem ) 
            {
            // Send key is used to select the contact, but only when
            // operating in 'call item fetch' mode
            // As the call item fetch is a single fetch, fetch dialog
            // needs to be exited after item fetched.
            TryExitL( EAknSoftkeySelect );
            }
        else if ( iParams.iFlags & EFetchMultiple || 
                    ( ( iParams.iFlags & EFetchSingle ) && 
                        ( EStdKeyYes == aKeyEvent.iScanCode ) ) )
            {
            // Call key is inactive when operating in 'multifetch' mode
            // and 'single item fetch' mode. 
            // Call key should be active when operating in 'single item 
            // call fetch' mode
            return EKeyWasConsumed;
            }
        }
    else if ( aKeyEvent.iCode == EKeyEnter &&
                ( iParams.iFlags & EFetchSingle ) )
        {
        // Single item fetch selects the highlighted contact and exits
        // dialog.
        // Enter key is handled similarly to MSK event, which is
        // received and handled by CBA (CBA's observer uses TryExitL).
        TryExitL( EAknSoftkeySelect );
        }

    if ( selectContact )
        {
        const MVPbkBaseContact* focusedContact =
            iPages->FocusedContactL();
        if ( focusedContact )
            {
            if ( !( iParams.iFlags & EFetchMultiple ) )
                {
                delete iFocusedContactLink;
                iFocusedContactLink = NULL;
                iFocusedContactLink = focusedContact->CreateLinkLC();
                CleanupStack::Pop(); // iFocusedContactLink
                }
            MVPbkContactLink* link = focusedContact->CreateLinkLC();
            SelectContactL( *link, selectContact );
            CleanupStack::PopAndDestroy(); // link
            result = EKeyWasConsumed;
            }
        else
            {
            result = CEikDialog::OfferKeyEventL( aKeyEvent, aType );
            }
        }
    else
        {
        if( !Phonebook2::Pbk2AppUi()->KeyEventHandler().Pbk2ProcessKeyEventL( aKeyEvent, aType ) )
            {
            
            if( iNamesListControl && iNamesListControl->IsVisible() 
                && aKeyEvent.iCode != EKeyLeftArrow
                && aKeyEvent.iCode != EKeyRightArrow 
                && aKeyEvent.iCode != EKeyEscape )
                {
                result = iNamesListControl->OfferKeyEventL( aKeyEvent, aType );
                }
            else if( iGroupListControl && iGroupListControl->IsVisible() 
                     && aKeyEvent.iCode != EKeyLeftArrow
                     && aKeyEvent.iCode != EKeyRightArrow 
                     && aKeyEvent.iCode != EKeyEscape )
                {
                result = iGroupListControl->OfferKeyEventL( aKeyEvent, aType );
                }
            if( result == EKeyWasNotConsumed )
                {
                result = CEikDialog::OfferKeyEventL( aKeyEvent, aType );
                }
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::HandleResourceChange
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::HandleResourceChange( TInt aType )
    {
    if( aType == KEikDynamicLayoutVariantSwitch )
        {
        // Update iMSKEnabled value
        iMSKEnabled = AknLayoutUtils::MSKEnabled();
        
        CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
        if (statusPane)
            {
            TInt currentStatusPaneResId = statusPane->CurrentLayoutResId();
            // Make the statusPane update when it was already set to EMPTY status,
            // in other case it'll update itself automaticlly
            if( currentStatusPaneResId == R_AVKON_STATUS_PANE_LAYOUT_EMPTY )
                {
                statusPane->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );
                statusPane->HandleResourceChange(aType);                
                }
            }
            
        if ( iPages )
            {
            // Handle resouce change for all pages 
            for (TInt i = 0; i < iPages->DlgPageCount() ; i++ )
                {
                iPages->DlgPageAt( i ).HandleResourceChange( aType );
                }
            }
        }
    else if ( aType == KAknsMessageSkinChange )
        {
        // Just visual effect, no need to handle error
        TRAP_IGNORE( SkinTabsL() );
        }

    CAknDialog::HandleResourceChange(aType);
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::LineChangedL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::LineChangedL( TInt /*aControlId*/ )
    {
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::FetchDlgControl
// --------------------------------------------------------------------------
//
CCoeControl* CPbk2FetchDlg::FetchDlgControl( TInt aCtrlId ) const
    {
    return ControlOrNull( aCtrlId );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::FetchDlgViewL
// --------------------------------------------------------------------------
//
MVPbkContactViewBase& CPbk2FetchDlg::FetchDlgViewL( TInt aControlId ) const
    {
    MVPbkContactViewBase* view = NULL;

    if ( iPages )
        {
        // Return page's view
        MPbk2FetchDlgPage* page = iPages->DlgPageWithId( aControlId );
        if ( page )
            {
            MVPbkContactViewBase& pageView = page->View();
            view = &pageView;
            }
        }

    if ( !view )
        {
        if ( aControlId == ECtrlFetchNamesList )
            {
            if ( iParams.iNamesListView )
                {
                // Use view given as parameter
                view = iParams.iNamesListView;
                }
            else
                {
                // If there is no view given, we use all contacts view
                view = Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ViewSupplier().AllContactsViewL();
                }
            }
        else if (aControlId == ECtrlFetchGroupsList)
            {
            if ( iParams.iGroupsListView )
                {
                // Use view given as parameter
                view = iParams.iGroupsListView;
                }
            else
                {
                // If there is no view given, we use all groups view
                view = Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ViewSupplier().AllGroupsViewL();
                }
            }

        }

    return *view;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::SetFetchDlgViewL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::SetFetchDlgViewL
        ( TInt aControlId, MVPbkContactViewBase& aView )
    {
    MPbk2FetchDlgPage* page = iPages->DlgPageWithId( aControlId );

    if ( page )
        {
        page->SetViewL( aView );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::FetchDlgSelection
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray& CPbk2FetchDlg::FetchDlgSelection()
    {
    return *iResults;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::FetchDlgHandleError
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::FetchDlgHandleError( TInt aError )
    {
    iEikonEnv->HandleError( aError );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::FetchDlgClientRect
// --------------------------------------------------------------------------
//
TRect CPbk2FetchDlg::FetchDlgClientRect() const
    {
    TRect appRect = iAvkonAppUi->ApplicationRect();
    TAknLayoutRect mainPane;
    mainPane.LayoutRect( appRect,
        AKN_LAYOUT_WINDOW_main_pane( appRect, 0, 1, 1 ) );
    return mainPane.Rect();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::FetchDlgPageChangedL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::FetchDlgPageChangedL( MPbk2FetchDlgPage& /*aPage*/ )
    {
    // An view event burst from VPbk results this function being called
    // several times a row. It is not meaningful to restore selections
    // every time, but instead wait for a while and restore the selections
    // after all events have been received. Hence the idle object is used.
    delete iSelectionRestorer;
    iSelectionRestorer = NULL;
    iSelectionRestorer = CIdle::NewL( CActive::EPriorityIdle );
    iSelectionRestorer->Start( TCallBack( RestoreSelections, this ));
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::FetchDlgObserver
// --------------------------------------------------------------------------
//
MPbk2FetchDlgObserver& CPbk2FetchDlg::FetchDlgObserver() const
    {
    return iObserver;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::SelectContactL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::SelectContactL
        ( const MVPbkContactLink& aLink, TBool aSelected )
    {
    if ( aSelected )
        {
        iResults->AppendL( aLink );

        MVPbkContactLink* link = iPages->CurrentPage().Control().
            FocusedContactL()->CreateLinkLC();

        if ( link->IsSame( aLink ) )
            {
            UpdateMultiSelectionMSKL( ETrue );
            }

        CleanupStack::PopAndDestroy( ); // link
        }
    else
        {
        iResults->RemoveL( aLink );
        

        // Names list control always delegates its behavior to the associated state, when 
        // the names list control is in empty state, then it may return NULL here;

        const MVPbkBaseContact* focusedContact = iPages->CurrentPage().Control().FocusedContactL();
        if ( focusedContact )
        	{  
        	MVPbkContactLink* link = focusedContact->CreateLinkLC();
            if ( link->IsSame( aLink ) )
                {
                UpdateMultiSelectionMSKL( EFalse );
                }    
            CleanupStack::PopAndDestroy( ); // link
        	}
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::HandleControlEventL
        ( MPbk2ContactUiControl& /*aControl*/,
          const TPbk2ControlEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EContactDoubleTapped:
            {
            if ( !(iParams.iFlags & EFetchMultiple) )
                {
                // Single item fetch dialog double tap
                // -> try to accept the dialog
                iDialogAccepter->Cancel();
                iDialogAccepter->CallBack();
                }
            break;
            }

        case TPbk2ControlEvent::EContactSetChanged:     // FALLTHROUGH
        case TPbk2ControlEvent::EControlStateChanged:
            {
            // Find pane events are handled here
            UpdateCbasL();
            break;
            }
            
        case TPbk2ControlEvent::EReady:
            {
            MVPbkContactLinkArray* markedEntries = iParams.iMarkedEntries;
        
            if ( markedEntries && iPages && iResults)
                {
                const TInt markedEntriesCount = markedEntries->Count();
                for ( TInt i = 0; i < markedEntriesCount; i ++ )
                    {
                    const MVPbkContactLink& link = markedEntries->At(i);
                    iPages->SelectContactL( link, ETrue );
                    iResults->AppendToResultsL( link );
                    }
                }
            break;
            }

        default:
            {
            // Do nothing
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::ContactSelected
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::ContactSelected
        ( const MVPbkContactLink& aLink, TBool aSelected )
    {
    if ( aSelected && iFocusedContactLink && iFocusedContactLink->IsSame( aLink ) )
        {
        // Deleting focused contact link that next time single fetch
        // also fetches the contact.
        delete iFocusedContactLink;
        iFocusedContactLink = NULL;

        TRAPD( err, TryExitL( EEikBidOk ) );
        if ( err != KErrNone )
            {
            // If not nicely then use the force
            delete this;
            }
        }
    else
        {
        TRAP_IGNORE( UpdateCbasL() );
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::ContactSelectionFailed
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::ContactSelectionFailed()
    {
    // Delete the focused link so that the exit procedure can be tried again
    delete iFocusedContactLink;
    iFocusedContactLink = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::SetCbaCommandSetL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::SetCbaCommandSetL( TInt aResourceId )
    {
    // Only need to call SetCommandSetL for the CBA if the resource actually changes.
    // And also, unnecessary to call the DrawDeferred since the CBA SetCommandSetL
    // does a redraw on the CBA.
    if( iCbaCommandSet != aResourceId )
        {
        CEikButtonGroupContainer& cba = ButtonGroupContainer();
        cba.SetCommandSetL( aResourceId );
        iCbaCommandSet = aResourceId;
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::SetupStatusPaneL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::SetupStatusPaneL()
    {
    delete iNaviDecorator;
    iNaviDecorator = NULL;

    TInt navipaneResId = KErrNone;

    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if (statusPane)
        {
        // Switch to Phonebook's status pane layout. Calling app has to
        // restore the layout if different
        statusPane->SwitchLayoutL( R_AVKON_STATUS_PANE_LAYOUT_USUAL );

        // First test if there was a navi pane parameter
        if ( iParams.iNaviPaneId > 0 )
            {
            navipaneResId = iParams.iNaviPaneId;
            }
        // If no parameter was found, test is navi pane defined in resources
        else if ( iResData.iNaviPaneId > 0 )
            {
            navipaneResId = iResData.iNaviPaneId;
            }

        // Setup navi pane if defined in resources
        if ( navipaneResId &&
            statusPane->PaneCapabilities
            (TUid::Uid(EEikStatusPaneUidNavi)).IsPresent())
            {
            CAknNavigationControlContainer* naviPane =
                static_cast<CAknNavigationControlContainer*>
                (statusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
            TResourceReader reader;
            iCoeEnv->CreateResourceReaderLC( reader, navipaneResId );
            iNaviDecorator =
                naviPane->ConstructNavigationDecoratorFromResourceL
                    ( reader );
            naviPane->PushL(*iNaviDecorator);
            CleanupStack::PopAndDestroy();  // reader
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::UpdateCbasL()
    {
    if ( iResults && iPages &&  iPages->CurrentPage().DlgPageReady() &&
         !iPages->CurrentPage().DlgPageEmpty() )
        {
        // Use user defined CBA parameter if exist
        if ( iParams.iCbaId )
            {
            SetCbaCommandSetL( iParams.iCbaId );
            }
        // Use CBA resources if exist
        else if ( iResData.iCbaId )
            {
            SetCbaCommandSetL( iResData.iCbaId );
            }
        // Resources for single fetch
        else if ( iParams.iFlags & EFetchSingle )
            {
            SetCbaCommandSetL( R_PBK2_SOFTKEYS_OK_BACK_OK );
            }
    
        if ( iParams.iFlags & EFetchMultiple )
            {
            CEikButtonGroupContainer& cba = ButtonGroupContainer();
            if ( iResults->Count() < iParams.iMinSelection )
                {
                cba.MakeCommandVisibleByPosition
                    ( CEikButtonGroupContainer::ELeftSoftkeyPosition, EFalse );
                }
            else
                {
                cba.MakeCommandVisibleByPosition
                    ( CEikButtonGroupContainer::ELeftSoftkeyPosition, ETrue );
                }
            }
        
        MVPbkContactLink* link = NULL;
        TBool contactSelected = EFalse;
        //Used the TRAPD to catch the exception when the contact can't be found in DB. 
        TRAPD( err,
                {link=iPages->CurrentPage().Control().FocusedContactL()->CreateLinkLC();CleanupStack::Pop();});
        TInt findResult = KErrNotFound;
        if ( KErrNone == err )
            {
            findResult = iResults->Find( *link );
            delete link;
            }
        else
            {
            // ignore KErrNotFound  as it will cause dead loop in CPbk2PredictiveViewStack::Reset.
            if ( KErrNotFound != err )
                {
                User::LeaveIfError( err );
                }
            }

        if ( findResult != KErrNotFound )
            {
            User::LeaveIfError( findResult );
            contactSelected = ETrue;
            }

        UpdateMultiSelectionMSKL( contactSelected );

        }
    else
        {
        if ( iResData.iEmptyCbaId )
            {
            SetCbaCommandSetL( iResData.iEmptyCbaId );
            }
        else
            {
            SetCbaCommandSetL( R_PBK2_SOFTKEYS_EMPTY_BACK_EMPTY );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::UpdateMultiSelectionMSKL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::UpdateMultiSelectionMSKL( TBool aContactSelected )
    {
    if ( iMSKEnabled && iParams.iFlags & EFetchMultiple )
        {
        RemoveCommandFromMSK();

        if ( aContactSelected )
            {
            SetMiddleSoftKeyLabelL
                ( R_QTN_MSK_UNMARK, EAknSoftkeyUnmark );
            }
        else
            {
            SetMiddleSoftKeyLabelL( R_QTN_MSK_MARK, EAknSoftkeyMark );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::RemoveCommandFromMSK
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::RemoveCommandFromMSK()
    {
    ButtonGroupContainer().
        RemoveCommandFromStack( KPbk2MSKControlId, EAknSoftkeyMark );
    ButtonGroupContainer().
        RemoveCommandFromStack( KPbk2MSKControlId, EAknSoftkeyUnmark );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::SetMiddleSoftKeyLabelL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::SetMiddleSoftKeyLabelL
        ( const TInt aResourceId, const TInt aCommandId )
    {
    CEikButtonGroupContainer& cbaGroup = ButtonGroupContainer();
    HBufC* middleSKText = StringLoader::LoadLC( aResourceId );
    TPtr mskPtr = middleSKText->Des();
    cbaGroup.AddCommandToStackL(
        KPbk2MSKControlId,
        aCommandId,
        mskPtr );
    CleanupStack::PopAndDestroy( middleSKText );
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::TryAcceptSelectionL
// --------------------------------------------------------------------------
//
TInt CPbk2FetchDlg::TryAcceptSelectionL( TAny* aSelf )
    {
    static_cast< CPbk2FetchDlg* >( aSelf )->TryExitL( EAknSoftkeySelect );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::CheckIsOkToExitL
// --------------------------------------------------------------------------
//
inline TBool CPbk2FetchDlg::CheckIsOkToExitL( TInt aButtonId )
    {
    TBool ret = ETrue;

    switch ( aButtonId )
        {
        case EAknSoftkeyMark:
            {
            iPages->CurrentPage().Control().
                ProcessCommandL( EAknCmdMark );
            // Not ok to exit
            ret = EFalse;
            break;
            }

        case EAknSoftkeyUnmark:
            {
            iPages->CurrentPage().Control().
                ProcessCommandL( EAknCmdUnmark );
            // Not ok to exit
            ret = EFalse;
            break;
            }

        case EEikBidCancel:
            {
            // End key pressed, query permission for application exit
            if ( !OkToExitApplicationL( EEikBidCancel ) )
                {
                ret = EFalse;
                }
            break;
            }
        }

    if ( ret && aButtonId != EEikBidCancel )
        {
        // Focused contact must be returned if no contacts yet selected,
        // and the focused contact selection is not already on its way
        if ( FetchDlgSelection().Count() == 0 && !( iParams.iFlags & EFetchMultiple ) )
            {
            const MVPbkBaseContact* focusedContact = iPages->FocusedContactL();    
            if ( focusedContact )
                {
                delete iFocusedContactLink;
                iFocusedContactLink = NULL;
                iFocusedContactLink = focusedContact->CreateLinkLC();
                CleanupStack::Pop(); // iFocusedContactLink
                if ( iParams.iFlags & EFetchSingle )
                    {
                    // Clean result if this is single fetch.
                    iResults->ResetAndDestroy();
                    }
                SelectContactL( *iFocusedContactLink, ETrue );
                }

            // Not ok to to exit, since contact selection is asynchronous
            ret = EFalse;
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::Canceled
// EEikBidCancel is omitted on purpose. End key sends EEikBidCancel,
// and it indicates abortion, not cancellation.
// --------------------------------------------------------------------------
//
inline TBool CPbk2FetchDlg::Canceled( TInt aButtonId )
    {
    TBool canceled = EFalse;

    switch ( aButtonId )
        {
        case EAknSoftkeyBack:       // FALLTHROUGH
        case EAknSoftkeyClose:      // FALLTHROUGH
        case EAknSoftkeyNo:         // FALLTHROUGH
        case EAknSoftkeyExit:
            {
            canceled = ETrue;
            break;
            }
        }

    return canceled;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::SkinTabsL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::SkinTabsL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("SkinTabsL start"));
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
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("SkinTabsL naviDeco"));
            if ( naviDeco->ControlType() ==
                    CAknNavigationDecorator::ETabGroup )
                {
                MAknsSkinInstance* skin = AknsUtils::SkinInstance();

                PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("SkinTabsL ETabGroup"));
                CPbk2IconInfoContainer* tabIcons =
                    CPbk2IconInfoContainer::NewL
                        ( R_PBK2_FETCH_TAB_ICON_INFO_ARRAY );
                CleanupStack::PushL( tabIcons );

                CAknTabGroup* tabGroup =
                    static_cast<CAknTabGroup*>(naviDeco->DecoratedControl());

                const TInt count = tabGroup->TabCount();
                PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                    ("SkinTabsL TabCount = %d"), count);
                for (TInt i = 0; i < count; ++i)
                    {
                    TInt tabId = tabGroup->TabIdFromIndex(i);
                    // tabId is used as icon id
                    TPbk2IconId iconId( TUid::Uid(KPbk2UID3), tabId );
                    const CPbk2IconInfo* iconInfo = tabIcons->Find(iconId);
                    if (iconInfo)
                        {
                        CFbsBitmap* bitmap = NULL;
                        CFbsBitmap* mask = NULL;

                        CPbk2IconFactory* factory =
                            CPbk2IconFactory::NewLC( *tabIcons );
                        factory->CreateIconLC(iconId, *skin, bitmap, mask);
                        tabGroup->ReplaceTabL(tabId, bitmap, mask);
                        CleanupStack::Pop(2); // mask, bitmap
                        CleanupStack::PopAndDestroy(factory);
                        }
                    }
                CleanupStack::PopAndDestroy( tabIcons );
                }
            }
        }
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("SkinTabsL end"));
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::DelaySkinning
// --------------------------------------------------------------------------
//
TInt CPbk2FetchDlg::DelaySkinning( TAny* aFetchDlg )
    {
    // Tab skinning is just visual effect, no need to handle error
    TRAP_IGNORE( ( (CPbk2FetchDlg*)aFetchDlg)->SkinTabsL() );
    return 0; // one time only
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::RestoreSelections
// --------------------------------------------------------------------------
//
TInt CPbk2FetchDlg::RestoreSelections( TAny* aFetchDlg )
    {
    CPbk2FetchDlg* self = static_cast<CPbk2FetchDlg*> ( aFetchDlg );
    TRAPD( err, self->RestoreSelectionsL() );
    if ( err != KErrNone )
    	{
    	self->FetchDlgHandleError( err );
    	}

    return 0; // one time only
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::RestoreSelectionsL
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::RestoreSelectionsL()
    {
    if ( iResults )
        {
        for ( TInt i = 0; i < iResults->Count(); ++i )
            {
            iPages->SelectContactL( iResults->At( i ), ETrue );
            }
        }
    
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::OkToExitApplicationL
// --------------------------------------------------------------------------
//
TBool CPbk2FetchDlg::OkToExitApplicationL( TInt aCommandId )
    {
    TBool okToExit = ETrue;

    // If exit callback returned EFalse, the exit is cancelled
    if ( iParams.iExitCallback &&
         !iParams.iExitCallback->OkToExitL( aCommandId ) )
        {
        okToExit = EFalse;
        }

    if ( !okToExit )
        {
        iExitRecord.Clear( EExitApproved );
        }

    return okToExit;
    }

// --------------------------------------------------------------------------
// CPbk2FetchDlg::ExitApplication
// --------------------------------------------------------------------------
//
void CPbk2FetchDlg::ExitApplication( TInt aCommandId )
    {
    if ( iExitRecord.IsSet( EExitOrdered ) &&
         iExitRecord.IsSet( EExitApproved ) )
        {
        CEikAppUi* appUi = iEikonEnv->EikAppUi();
        MEikCommandObserver* cmdObs =
            static_cast<MEikCommandObserver*>( appUi );

        // Dialog is closed so there is nothing to do if
        // ProcessCommandL leaves. Of course it shouldn't leave in
        // practice because it's exit command.
        TRAP_IGNORE( cmdObs->ProcessCommandL( aCommandId ) );
        }
    }

// End of File
