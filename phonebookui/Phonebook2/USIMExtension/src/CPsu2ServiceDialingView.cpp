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
* Description:  A service dialing name list view
*
*/



// INCLUDE FILES
#include "CPsu2ServiceDialingView.h"

// Phonebook 2
#include <CPbk2UIExtensionView.h>
#include <MPbk2ViewActivationTransaction.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ViewExplorer.h>
#include <Pbk2USimUIRes.rsg>
#include <CPbk2ViewState.h>
#include <CPbk2NamesListControl.h>
#include <Pbk2UIControls.rsg>

// System includes
#include <StringLoader.h>
#include <eikmenup.h>
#include <aknnavide.h>
#include <aknnavi.h>
#include <AknUtils.h>
#include <eikmenub.h>


/// Unnamed namespace for local definitions
namespace {

/**
 * Returns ETrue if shift is depressed in given key event.
 *
 * @param aKeyEvent     Key event.
 * @return  ETrue if shift is pressed.
 */
inline TBool ShiftDown( const TKeyEvent& aKeyEvent )
    {
    return ( aKeyEvent.iModifiers &
        ( EModifierShift | EModifierLeftShift | EModifierRightShift ) ) != 0;
    }

} /// namespace


// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::CPsu2ServiceDialingView
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CPsu2ServiceDialingView::CPsu2ServiceDialingView(
        CPbk2UIExtensionView& aExtensionView, CPsu2ViewManager& aViewManager)
        :   CPsu2NameListViewBase(aExtensionView, aViewManager)
    {
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::~CPsu2ServiceDialingView
// --------------------------------------------------------------------------
//
CPsu2ServiceDialingView::~CPsu2ServiceDialingView()
    {
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::NewL
// --------------------------------------------------------------------------
//
CPsu2ServiceDialingView* CPsu2ServiceDialingView::NewL
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager )
    {
    CPsu2ServiceDialingView* self = new ( ELeave ) CPsu2ServiceDialingView
        ( aExtensionView, aViewManager );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPsu2ServiceDialingView::HandleCommandKeyL( const TKeyEvent& aKeyEvent,
                                                  TEventCode aType)
    {
    const TBool marked( iControl->ContactsMarked() );
    if ( aKeyEvent.iCode == EKeyOK &&
         aType == EEventKey )
        {
        if ( !ShiftDown( aKeyEvent ) )
            {
            if ( marked )
                {
                iExtensionView.LaunchPopupMenuL(
                    R_PSU2_SERVICE_DIALING_CONTEXT_MENUBAR);
                }
            else
                {
                if ( iControl->NumberOfContacts() > 0 )
                    {
                    OpenInfoViewCmdL( *iControl );
                    }
                }
            return ETrue;
            }
        }
     // green call
     else if( aKeyEvent.iCode == EKeyPhoneSend &&
         aType == EEventKey  )
        {
        if ( iControl->NumberOfContacts() > 0 && !marked )
            {
            HandleCommandL(EPbk2CmdCall);
            }
        return ETrue;
        }

    if ( ShiftDown( aKeyEvent ) )
        {
        // Update cbas when shift and msk is pressed.
        UpdateCbasL();
        }

    return CPsu2NameListViewBase::HandleCommandKeyL( aKeyEvent, aType );
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::DoActivateL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingView::DoActivateL(const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage)
    {
    HBufC* title = StringLoader::LoadLC(R_QTN_SDN_TITLE);
    TContextPaneIcon icon(EPsu2qgn_menu_simin);
    CEikImage* image = icon.CreateLC();

    MPbk2ViewActivationTransaction* viewActivationTransaction =
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
            HandleViewActivationLC
                ( iExtensionView.Id(), aPrevViewId, title, image,
                  Phonebook2::EUpdateNaviPane |
                  Phonebook2::EUpdateContextPane |
                  Phonebook2::EUpdateTitlePane );

    // Call base class
    CPsu2NameListViewBase::DoActivateL(
        aPrevViewId, aCustomMessageId, aCustomMessage);

    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(3,title); // viewActivationTransaction,
                                          // title, image
    // Create empty navigation pane
    if (!iNaviPane)
        {
        iNaviPane = static_cast<CAknNavigationControlContainer*>(
            iAvkonAppUi->StatusPane()->ControlL(
            TUid::Uid(EEikStatusPaneUidNavi)));
        }
    if ( !iNaviDecorator )
        {
        iNaviDecorator = iNaviPane->CreateNavigationLabelL();
        iNaviPane->PushL(*iNaviDecorator);
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingView::DoDeactivate()
    {
    if (iNaviPane && iNaviDecorator)
        {
        iNaviPane->Pop(iNaviDecorator);
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    CPsu2NameListViewBase::DoDeactivate();
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingView::HandleCommandL(TInt aCommand)
    {
    switch( aCommand )
        {
        case EPsu2CmdOpenServiceDialingInfoView:
            {
            OpenInfoViewCmdL( *iControl );
            break;
            }
        default:
            {
            CPsu2NameListViewBase::HandleCommandL( aCommand );
            break;
            }
        };
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingView::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    switch( aResourceId )
        {
        case R_PSU2_SERVICE_DIALING_OPEN_MENUPANE:
            {
            const TInt count( iControl->NumberOfContacts() );
            const TBool marked( iControl->ContactsMarked() );

            if ( count <= 0 || marked )
                {
                aMenuPane->SetItemDimmed( EPsu2CmdOpenServiceDialingInfoView , ETrue );
                }
            break;
            }
        case R_PSU2_SERVICE_DIALING_COPY_MENUPANE:
            {
            const TInt count( iControl->NumberOfContacts() );
            if ( count <= 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdCopy, ETrue );
                }
            break;
            }
        default:
            {
            CPsu2NameListViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        };
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::NameListControlResourceId
// --------------------------------------------------------------------------
//
TInt CPsu2ServiceDialingView::NameListControlResourceId() const
    {
    return R_PSU2_SERVICE_DIALING_NAME_LIST_CONTROL;
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::OpenInfoViewCmdL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingView::OpenInfoViewCmdL
        ( MPbk2ContactUiControl& aUiControl ) const
    {
    if (!aUiControl.ContactsMarked())
        {
        CPbk2ViewState* state = aUiControl.ControlStateL();
        CleanupStack::PushL(state);
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
            ActivatePhonebook2ViewL( TUid::Uid
                ( EPsu2ServiceDialingInfoViewId ), state );

        CleanupStack::PopAndDestroy();  // state
        aUiControl.UpdateAfterCommandExecution();
        }
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingView::UpdateCbasL()
    {
    if ( iControl->NumberOfContacts() > 0
            && !iControl->ContactsMarked() )
            {
            // Set middle softkey as Open.
            iExtensionView.Cba()->SetCommandSetL(R_PSU_SOFTKEYS_OPTIONS_BACK_OPEN);
            iExtensionView.Cba()->DrawDeferred();
            }
        else
            {
            if ( iControl->ContactsMarked() )
                {
                // Set middle softkey as Context menu.
                iExtensionView.Cba()->SetCommandSetL(R_PBK2_SOFTKEYS_OPTIONS_BACK_CONTEXT);
                iExtensionView.Cba()->DrawDeferred();
                // Change context menu when marked items
                iExtensionView.MenuBar()->SetContextMenuTitleResourceId
                    ( R_PSU2_SERVICE_DIALING_CONTEXT_MENUBAR );
                }
            else
                {
                // Set middle softkey as Open.
                iExtensionView.Cba()->SetCommandSetL(R_PBK2_SOFTKEYS_OPTIONS_BACK_EMPTY);
                iExtensionView.Cba()->DrawDeferred();
                // Change context menu when nameslist is empty
                iExtensionView.MenuBar()->SetContextMenuTitleResourceId
                    ( R_PSU2_SERVICE_DIALING_CONTEXT_MENUBAR );
                }
            }
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingView::HandleControlEventL
// --------------------------------------------------------------------------
//    
void CPsu2ServiceDialingView::HandleControlEventL(
        MPbk2ContactUiControl& aControl,
        const TPbk2ControlEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EContactDoubleTapped:
            {
            if ( iControl->ContactsMarked() )
                {
                iExtensionView.LaunchPopupMenuL(
                    R_PSU2_SERVICE_DIALING_CONTEXT_MENUBAR);
                }
            else
                {
                // Open contact
                HandleCommandL( EPsu2CmdOpenServiceDialingInfoView );
                }
            break;
            }

        default:
            {
            CPsu2NameListViewBase::HandleControlEventL( aControl, aEvent );
            break;
            }
        }
    }
//  End of File
