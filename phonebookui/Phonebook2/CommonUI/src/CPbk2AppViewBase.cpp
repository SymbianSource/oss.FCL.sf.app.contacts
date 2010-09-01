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
* Description:  Phonebook 2 application view base.
*
*/


// INCLUDE FILES
#include <CPbk2AppViewBase.h>

// Phonebook 2
#include <CPbk2AppUiBase.h>
#include <CPbk2DocumentBase.h>
#include <MPbk2KeyEventHandler.h>
#include <MPbk2ApplicationServices.h>

// System includes
#include <eikmenub.h>
#include <eikapp.h>
#include <barsread.h>
#include <AknUtils.h>
#include <aknViewAppUi.h>
#include <aknstyluspopupmenu.h>

// --------------------------------------------------------------------------
// CPbk2AppViewBase::CPbk2AppViewBase
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AppViewBase::CPbk2AppViewBase()
    {
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::~CPbk2AppViewBase
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2AppViewBase::~CPbk2AppViewBase()
    {
    ClosePopup();
    CloseStylusPopup();
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::ApplicationUid
// --------------------------------------------------------------------------
//
EXPORT_C TUid CPbk2AppViewBase::ApplicationUid() const
    {
    return AppUi()->Application()->AppDllUid();
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::ViewStateLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ViewState* CPbk2AppViewBase::ViewStateLC() const
    {
    // Default implementation
    CPbk2ViewState* state = NULL;
    CleanupStack::PushL(state);
    return state;
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::LaunchPopupMenuL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::LaunchPopupMenuL( TInt aResourceId )
    {
    // In future we should study if the whole menubar thingy could be
    // removed and use context menu instead since its already used
    // anyway
    
    // these are used to check if our popup or context menu is open
    TBool isPopupVisible=EFalse;
    TBool isContextVisible=MenuBar()->IsDisplayed();
    
    if (iPopup)
        {
        // check if popup is already there
        isPopupVisible=iPopup->IsDisplayed();
        // popup and context menu shouldn't be visible 
        if ((!isPopupVisible) && (!isContextVisible))
            {
            // destroy exisiting
            ClosePopup();
            // and show new popup
            CreatePopupMenuL(aResourceId);
            }
        }
    // we don't have popup
    else
        {
        // and context menu is not shown
        if (!isContextVisible)
            {
            CreatePopupMenuL(aResourceId);
            }
        }
    
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::CreatePopupMenuL
// --------------------------------------------------------------------------
//
void CPbk2AppViewBase::CreatePopupMenuL( TInt aResourceId )
    {
    iPopup = new(ELeave) CEikMenuBar;
    TRAPD(err, DoLaunchPopupL(aResourceId));
    if (err)
        {
        ClosePopup();
        User::Leave(err);
        }
    }




// --------------------------------------------------------------------------
// CPbk2AppViewBase::GetMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2AppViewBase::GetMenuFilteringFlagsL() const
    {
    MPbk2AppUi* appUi = Phonebook2::Pbk2AppUi();

    return ( GetViewSpecificMenuFilteringFlagsL() |
             appUi->ApplicationServices().
                GlobalMenuFilteringFlagsL() );
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::HandleCommandKeyL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2AppViewBase::HandleCommandKeyL
        ( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {    
    return EFalse;
	}

// --------------------------------------------------------------------------
// CPbk2AppViewBase::HandlePointerEventL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::HandlePointerEventL
        ( const TPointerEvent& /*aPointerEvent*/ )
    {
    // Do nothing by default
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::HandleLongTapEventL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::HandleLongTapEventL(
        const TPoint& /*aPenEventLocation*/,
        const TPoint& /*aPenEventScreenLocation*/)
    {
    // Do nothing by default
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::ProcessCommandL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::ProcessCommandL( TInt aCommand )
    {
    // Close popup menu if open
    ClosePopup();

    // Call base class
    CAknView::ProcessCommandL( aCommand );
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2AppViewBase::Pbk2ProcessKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {    
    // If a menu is displayed, handle the send key event 
    // by letting the framework handle it; In practice the 
    // dailler is launched
    if (MenuBar()->MenuPane() && MenuBar()->IsDisplayed())
        {
        if ( (aType == EEventKey || aType == EEventKeyUp 
          || aType == EEventKeyDown ) && aKeyEvent.iScanCode == EStdKeyYes )
            {
            // Key event is not handled by me, offer it to the default handler
            return Phonebook2::Pbk2AppUi()->KeyEventHandler().
                Pbk2ProcessKeyEventL( aKeyEvent, aType );
            }
        }	
	
	// Offer key first to self. Derived class may override HandleCommandKeyL.
    if ( HandleCommandKeyL( aKeyEvent, aType ) )
        {
        return ETrue;
        }
    else
        {
        // Ignore Send Key up and down events to prevent Dialer appearance or
		// any other application from launhing on top of or behind the Phonebook application.
		if ( ( aType == EEventKeyDown || aType == EEventKeyUp )
                && aKeyEvent.iScanCode == EStdKeyYes )
                {
                return ETrue;
                }
        else
            {
            // Key event is not handled by me, offer it to the default handler
            return Phonebook2::Pbk2AppUi()->KeyEventHandler().
                Pbk2ProcessKeyEventL( aKeyEvent, aType );
            }        
        }    
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::Pbk2ProcessPointerEventL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::Pbk2ProcessPointerEventL
        ( const TPointerEvent& aPointerEvent )
    {
    HandlePointerEventL( aPointerEvent );
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::Pbk2HandleLongTapEventL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::Pbk2HandleLongTapEventL(
        const TPoint& aPenEventLocation,
        const TPoint& aPenEventScreenLocation )
    {
    HandleLongTapEventL( aPenEventLocation, aPenEventScreenLocation );
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::DoLaunchPopupL
// --------------------------------------------------------------------------
//
void CPbk2AppViewBase::DoLaunchPopupL(TInt aResourceId)
    {
    iPopup->ConstructL(this, 0, aResourceId);
    iPopup->SetMenuType( CEikMenuBar::EMenuContext );
    AppUi()->AddToViewStackL(*this, iPopup,
        ECoeStackPriorityMenu, ECoeStackFlagRefusesFocus);
    iPopup->TryDisplayMenuBarL();
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::ClosePopup
// Removes the popup from the view stack and deletes it.
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::ClosePopup()
    {
    if ( iPopup && iPopup->MenuPane() )
        {
        iPopup->StopDisplayingMenuBar();
        AppUi()->RemoveFromViewStack(*this, iPopup);
        delete(iPopup);
        iPopup=NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::LaunchStylusPopupMenuL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2AppViewBase::LaunchStylusPopupMenuL(
        TInt aResourceId,
        const TPoint& aPoint )
    {
    CloseStylusPopup();
    TRAPD( err, DoLaunchStylusPopupL( aResourceId, aPoint ) );
    if ( err )
        {
        CloseStylusPopup();
        User::Leave( err );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::LaunchStylusPopupMenuL
// --------------------------------------------------------------------------
//
EXPORT_C CAknStylusPopUpMenu& CPbk2AppViewBase::LaunchStylusPopupMenu2L(
        TInt aResourceId,
        const TPoint& aPoint )
    {
    LaunchStylusPopupMenuL( aResourceId, aPoint );
    return *iStylusPopup;
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::DoLaunchStylusPopupL
// --------------------------------------------------------------------------
//
void CPbk2AppViewBase::DoLaunchStylusPopupL
        ( TInt aResourceId, const TPoint& aPoint )
    {
    iStylusPopup = CAknStylusPopUpMenu::NewL( this, aPoint, NULL );
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, aResourceId );
    iStylusPopup->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy(); // reader

    iStylusPopup->SetPosition( aPoint );
    
    if(IsForeground())
        {
        iStylusPopup->ShowMenu();
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppViewBase::CloseStylusPopup
// --------------------------------------------------------------------------
//
void CPbk2AppViewBase::CloseStylusPopup()
    {
    if ( iStylusPopup )
        {
        delete iStylusPopup;
        iStylusPopup = NULL;
        }
    }

//  End of File
