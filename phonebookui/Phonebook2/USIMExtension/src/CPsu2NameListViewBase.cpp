/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A base class for sim name list views
*
*/


#include "CPsu2NameListViewBase.h"

// Phonebook2
#include "CPsu2ViewManager.h"
#include <CPbk2AppUiBase.h>
#include <CPbk2ControlContainer.h>
#include <CPbk2NamesListControl.h>
#include <CPbk2UIExtensionView.h>
#include <MPbk2CommandHandler.h>
#include <CPbk2ViewState.h>
#include <CPbk2IconInfoContainer.h>
#include <CPbk2IconFactory.h>
#include <Pbk2USimUIRes.rsg>
#include <Pbk2CommonUi.rsg>
#include <Pbk2UIControls.rsg>
#include <MPbk2ViewExplorer.h>
#include <csxhelp/phob.hlp.hrh>
#include <CPbk2SortOrderManager.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2PointerEventInspector.h>
#include <MPbk2ApplicationServices.h>


// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <CVPbkContactManager.h>

// System includes
#include <avkon.rsg>
#include <eikbtgpc.h>
#include <AknsUtils.h>
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <aknlayout.cdl.h>
#include <StringLoader.h>
#include <barsread.h>


/// Unnamed namespace for local definitions
namespace {

/**
 * Returns ETrue if either Shift or Control key is depressed.
 *
 * @param aKeyEvent Key event.
 * @return  ETrue if shift or control is down.
 */
inline TBool IsShiftOrControlDown( const TKeyEvent& aKeyEvent )
    {
    return ( aKeyEvent.iModifiers &
        ( EModifierShift | EModifierLeftShift | EModifierRightShift |
          EModifierCtrl | EModifierLeftCtrl | EModifierRightCtrl ) ) != 0;
    }

}  /// namespace


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::TContextPaneIcon::TContextPaneIcon
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsu2NameListViewBase::TContextPaneIcon::TContextPaneIcon(TPsu2SimIconId aIconId)
:   iIconId( aIconId )
    {
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::TContextPaneIcon::CreateLC
// -----------------------------------------------------------------------------
//
CEikImage* CPsu2NameListViewBase::TContextPaneIcon::CreateLC()
    {
    CEikImage* image = new(ELeave) CEikImage;
    CleanupStack::PushL( image );

    CPbk2IconInfoContainer* iconInfoContainer =
        CPbk2IconInfoContainer::NewL( R_PSU2_ICON_INFO_ARRAY );
    CleanupStack::PushL( iconInfoContainer );
    CPbk2IconFactory* factory = CPbk2IconFactory::NewLC( *iconInfoContainer );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    factory->CreateIconL( iIconId, *AknsUtils::SkinInstance(), bitmap, mask );
    SetSize( *bitmap, *mask );
    image->SetNewBitmaps( bitmap, mask );

    CleanupStack::PopAndDestroy(2); // factory, iconInfoContainer
    return image;
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::TContextPaneIcon::SetSize
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::TContextPaneIcon::SetSize(CFbsBitmap& aBitmap,
    CFbsBitmap& aMask )
    {
    TAknLayoutRect statuspane;
    statuspane.LayoutRect(iAvkonAppUi->ApplicationRect(),
        AknLayout::status_pane(iAvkonAppUi->ApplicationRect(),0));

    // context pane
    TAknWindowLineLayout contextPaneLayout =
        AknLayout::context_pane(statuspane.Rect(), 0);
    TAknLayoutRect contextPaneLayoutRect;
    contextPaneLayoutRect.LayoutRect(statuspane.Rect(), contextPaneLayout);
    TRect contextPaneRect = contextPaneLayoutRect.Rect();

    AknIconUtils::SetSize( &aBitmap, contextPaneRect.Size() );
    AknIconUtils::SetSize( &aMask, contextPaneRect.Size() );
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::CPsu2NameListViewBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsu2NameListViewBase::CPsu2NameListViewBase
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager) :
            iExtensionView( aExtensionView ),
            iViewManager( aViewManager ),
            iFocusIndex( KErrNotFound )
    {
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::~CPsu2NameListViewBase
// -----------------------------------------------------------------------------
//
CPsu2NameListViewBase::~CPsu2NameListViewBase()
    {
    delete iControlState;

    if (iContainer)
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    delete iNameFormatter;
    delete iSortOrderManager;
    }


// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::UpdateListEmptyTextL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::UpdateListEmptyTextL( TInt aListState )
    {    
    switch ( aListState )
        {            
        case CPbk2NamesListControl::EStateEmpty:                
            {
            HBufC* text = StringLoader::LoadLC( R_QTN_SELEC_EMPTY_LIST );
            if ( iControl )
                {
                iControl->SetTextL( *text );                
                }
            CleanupStack::PopAndDestroy( text );
            break;
            }
        case CPbk2NamesListControl::EStateReady: // FALLTHROUGH                    
        case CPbk2NamesListControl::EStateFiltered:
            {
            HBufC* text = StringLoader::LoadLC( R_PBK2_FIND_NO_MATCHES );
            if ( iControl )
                {
                iControl->SetTextL( *text );                
                }
            CleanupStack::PopAndDestroy( text );
            break;
            }
        case CPbk2NamesListControl::EStateNotReady: // FALLTHROUGH
        default:
            {            
            if ( iControl )
                {
                iControl->SetTextL( KNullDesC );                
                }
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::HandleStatusPaneSizeChange
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(iExtensionView.ClientRect());
        }
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::ViewStateLC
// -----------------------------------------------------------------------------
//
CPbk2ViewState* CPsu2NameListViewBase::ViewStateLC() const
    {
    CPbk2ViewState* state = NULL;
    if (iControl)
        {
        state = iControl->ControlStateL();
        }

    CleanupStack::PushL(state);
    return state;
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::HandleCommandKeyL
// -----------------------------------------------------------------------------
//
TBool CPsu2NameListViewBase::HandleCommandKeyL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    // Update selection modifier key state
    iSelectionModifierUsed = IsShiftOrControlDown( aKeyEvent ) ||
        ( (aType == EEventKeyDown) && (aKeyEvent.iCode == EStdKeyHash) );

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::HandlePointerEventL(
        const TPointerEvent& /*aPointerEvent*/ )
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::GetViewSpecificMenuFilteringFlagsL
// -----------------------------------------------------------------------------
//
TInt CPsu2NameListViewBase::GetViewSpecificMenuFilteringFlagsL() const
    {
    TInt flags = 0;
    if ( iControl )
        {
        flags = iControl->GetMenuFilteringFlagsL();
        }
        
    return flags;
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::DoActivateL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::DoActivateL(const TVwsViewId& aPrevViewId,
            TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    MPbk2ApplicationServices& appServices =
        Phonebook2::Pbk2AppUi()->ApplicationServices();

    iSortOrderManager = CPbk2SortOrderManager::NewL
        ( appServices.ContactManager().FieldTypes() );

    HBufC* unnamedBuf = StringLoader::LoadLC( R_QTN_PHOB_UNNAMED );

    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader,
        R_PSU2_FDN_TITLE_FIELD_SELECTOR);
    iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL
        ( appServices.ContactManager().FieldTypes(),
          *iSortOrderManager,
          reader,
          unnamedBuf,
          &appServices.ContactManager().FsSession() );
    CleanupStack::PopAndDestroy(); // reader
    CleanupStack::PopAndDestroy( unnamedBuf );
    iSortOrderManager->SetContactViewL( iViewManager.CurrentSimView() );

    // Add this view to observe command events
    appServices.CommandHandlerL()->AddMenuCommandObserver(*this);

    CreateControlsL();
    iContainer->ActivateL();
    UpdateListEmptyTextL( CPbk2NamesListControl::EStateNotReady );
    if( aPrevViewId.iViewUid != TUid::Uid( EPsu2FixedDialingInfoViewId ) )
      	{
        // Clean up the old view control state 
        // so that it will be a brand new control 
      	// when the FD view is activated from non-fix dialling area
        delete iControlState;
        iControlState  = NULL;
        iFocusIndex = KErrNotFound;   	
        }
    iViewManager.CurrentSimView().AddObserverL(*iControl);
    iViewManager.RegisterStoreAndView();
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::DoDeactivate
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::DoDeactivate()
    {
    delete iNameFormatter;
    iNameFormatter = NULL;
    delete iSortOrderManager;
    iSortOrderManager = NULL;

    TRAP_IGNORE(
        Phonebook2::Pbk2AppUi()->ApplicationServices().CommandHandlerL()->
            RemoveMenuCommandObserver(*this) );

    delete iControlState;
    iControlState = NULL;

    if (iContainer)
        {
        iViewManager.CurrentSimView().RemoveObserver(*iControl);
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iContainer);
        // Store current state, safe to ignore. There's no real harm,
        // if theres no stored state when activating this view again
        TRAP_IGNORE(StoreStateL());
        delete iContainer;
        iContainer = NULL;
        // iControl is owned by iContainer it is deleted with container
        iControl = NULL;
        }

    iViewManager.DeregisterStoreAndView();
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::HandleCommandL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::HandleCommandL(TInt aCommand)
    {
    if ( aCommand == EPbk2CmdExit || aCommand == EAknSoftkeyBack
         || aCommand == EPbk2CmdOpenPreviousView || aCommand == EAknCmdHideInBackground )
        {
        // Initialize this view control state
        delete iControlState;
        iControlState  = NULL;
        iControlState = CPbk2ViewState::NewL();
        iControlState->SetFlags( CPbk2ViewState::EInitialized );
        iFocusIndex = KErrNotFound;
        RestoreStateL();
        }
        
    if ( iControl )
        {
        // No command handling in this class, forward to Commands
        if ( !Phonebook2::Pbk2AppUi()->ApplicationServices().
              CommandHandlerL()->HandleCommandL
                (aCommand, *iControl, &iExtensionView))
            {
            iControl->ProcessCommandL(aCommand);
            Phonebook2::Pbk2AppUi()->HandleCommandL(aCommand);
            // Update cbas, if command handler is consuming the command
            // postcommandexecution is updating cbas
            UpdateCbasL();
            }        
        }
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    if ( iControl )
        {
        // Ask the control do control specific filtering
        // (for example call HandleMarkableListDynInitMenuPane if needed)
        iControl->DynInitMenuPaneL(aResourceId, aMenuPane);

        // Phonebook 2 menu filtering happens in Commands
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            CommandHandlerL()->DynInitMenuPaneL
                ( aResourceId, aMenuPane, iExtensionView, *iControl );        
        }
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::HandleLongTapEventL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::HandleLongTapEventL(
        const TPoint& /*aPenEventLocation*/,
        const TPoint& /*aPenEventScreenLocation*/)
    {
    // Do nothing
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::HandleControlEventL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::HandleControlEventL(
        MPbk2ContactUiControl& /*aControl*/,
        const TPbk2ControlEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TPbk2ControlEvent::EReady:
            {
            UpdateCbasL();
            UpdateListEmptyTextL( aEvent.iInt );
            // Restore control state
            RestoreStateL();
            break;
            }

        case TPbk2ControlEvent::EContactSetChanged:
            {
            if ( iControl )
                {
                iControl->DrawNow();
                }            
            break;
            }

        case TPbk2ControlEvent::EControlStateChanged:   // FALLTHROUGH
        case TPbk2ControlEvent::EContactSelected:   // FALLTHROUGH
        case TPbk2ControlEvent::EContactUnselected: // FALLTHROUGH
        case TPbk2ControlEvent::EContactUnselectedAll:  // FALLTHROUGH
        case TPbk2ControlEvent::EItemRemoved:
            {
            UpdateCbasL();
            UpdateListEmptyTextL( aEvent.iInt );
            if ( iControl )
                {
                iControl->DrawDeferred();
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
// CPsu2NameListViewBase::PreCommandExecutionL
// --------------------------------------------------------------------------
//
void CPsu2NameListViewBase::PreCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    iCommandBetweenPreAndPostExecution = ETrue;
    }

// --------------------------------------------------------------------------
// CPsu2NameListViewBase::PostCommandExecutionL
// --------------------------------------------------------------------------
//
void CPsu2NameListViewBase::PostCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    iCommandBetweenPreAndPostExecution = EFalse;
    UpdateCbasL();
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::CreateControlsL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::CreateControlsL()
    {
    if (!iContainer)
        {
        // Create the container and control
        CContainer* container = CContainer::NewLC(
            &iExtensionView,
            &iExtensionView,
            iExtensionView);

        container->SetHelpContext(
            TCoeHelpContext( iExtensionView.ApplicationUid(),
                             KPHOB_HLP_NAME_LIST ));

        CPbk2NamesListControl* control = CPbk2NamesListControl::NewL(
                NameListControlResourceId(), container,
                Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ContactManager(),
                iViewManager.CurrentSimView(),
                *iNameFormatter,
                Phonebook2::Pbk2AppUi()->ApplicationServices().
                    StoreProperties() );
        CleanupStack::PushL(control);
        control->AddObserverL(*this);

        // takes ownership of the control
        container->SetControl(control, iExtensionView.ClientRect());
        CleanupStack::Pop(control);

        CCoeEnv::Static()->AppUi()->AddToStackL(iExtensionView, container);
        CleanupStack::Pop(container);
        iContainer = container;
        iControl = control;
        iPointerEventInspector = control;
        }
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::StoreStateL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::StoreStateL()
    {
    CPbk2ViewState* state = NULL;
    if ( iControl )
        {
        state = iControl->ControlStateL();        
        }
        
    if ( state )
        {
        delete iControlState;
        iControlState = state;
        }
    if ( iControl )
        {
        // If contact is deleted in other view
        // this will keep focus correct
        iFocusIndex = iControl->FocusedContactIndex();
        }
    }

// -----------------------------------------------------------------------------
// CPsu2NameListViewBase::RestoreStateL
// -----------------------------------------------------------------------------
//
void CPsu2NameListViewBase::RestoreStateL()
    {
    if ( iControl )
        {
        iControl->RestoreControlStateL(iControlState);        
        }
        
    if ( iFocusIndex != KErrNotFound && iControlState )
        {
        // If contact has been deleted in other view
        // this will keep focus correct
        if ( iControlState->FocusedContact() )
            {
            TInt indexOfContact(
                iViewManager.CurrentSimView().IndexOfLinkL(
                    *iControlState->FocusedContact() ) );
            if ( iControl && indexOfContact == KErrNotFound )
                {
                iControl->SetFocusedContactIndexL( iFocusIndex );                
                }
            }
        }
    delete iControlState;
    iControlState = NULL;
    }

//  End of File
