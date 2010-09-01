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
* Description:  Phonebook 2 USIM UI Extension service dialing info view.
*
*/


#include "CPsu2ServiceDialingInfoView.h"

// Phonebook 2
#include <CPbk2UIExtensionView.h>
#include <MPbk2ViewActivationTransaction.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ViewExplorer.h>
#include <Pbk2USimUIRes.rsg>
#include <Pbk2UIControls.rsg>
#include <MPbk2ContactUiControl.h>
#include <TPbk2StoreContactAnalyzer.h>
#include <Pbk2Commands.hrh>
#include <MPbk2ApplicationServices.h>
#include <MPbk2PointerEventInspector.h>

// System includes
#include <AknUtils.h>
#include <eikmenub.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EPanicLogic_HasContactPhoneNumberL = 1
    };

void Panic( TPanicCode aPanic )
    {
    _LIT( KPanicCat, "CPsu2ServiceDialingInfoView" );
    User::Panic( KPanicCat, aPanic );
    }

#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::CPsu2ServiceDialingInfoView
// --------------------------------------------------------------------------
//
CPsu2ServiceDialingInfoView::CPsu2ServiceDialingInfoView
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager ):
            CPsu2InfoViewBase( aExtensionView, aViewManager )
    {
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::~CPsu2ServiceDialingInfoView
// --------------------------------------------------------------------------
//
CPsu2ServiceDialingInfoView::~CPsu2ServiceDialingInfoView()
    {
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::NewL
// --------------------------------------------------------------------------
//
CPsu2ServiceDialingInfoView* CPsu2ServiceDialingInfoView::NewL
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager )
    {
    CPsu2ServiceDialingInfoView* self =
        new( ELeave ) CPsu2ServiceDialingInfoView
            ( aExtensionView, aViewManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingInfoView::ConstructL()
    {
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::HandleCommandKeyL
// -----------------------------------------------------------------------------
//
TBool CPsu2ServiceDialingInfoView::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool ret = EFalse;

    if ( aKeyEvent.iCode == EKeyPhoneSend && aType == EEventKey )
        {
        HandleCommandL( EPbk2CmdCall );
        ret = ETrue;
        }
    else if ( aKeyEvent.iCode == EKeyOK && aType == EEventKey )
        {
        iExtensionView.LaunchPopupMenuL
            ( R_PSU2_SERVICE_DIALING_INFO_CONTEXT_MENUBAR );
        ret = ETrue;
        }
    else
        {
        ret = CPsu2InfoViewBase::HandleCommandKeyL( aKeyEvent, aType );
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::DoActivateL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingInfoView::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    delete iViewActivationTransaction;
    iViewActivationTransaction = NULL;
    iViewActivationTransaction =
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
            ( iExtensionView.Id(), aPrevViewId, NULL, NULL,
              Phonebook2::EUpdateNaviPane );
    CleanupStack::Pop(); // iViewActivationTransaction

    // Call base class constructor
    CPsu2InfoViewBase::DoActivateL(
        aPrevViewId, aCustomMessageId, aCustomMessage);
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingInfoView::DoDeactivate()
    {
    CPsu2InfoViewBase::DoDeactivate();
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingInfoView::HandleCommandL( TInt aCommand )
    {
    CPsu2InfoViewBase::HandleCommandL( aCommand );
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2ServiceDialingInfoView::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    switch( aResourceId )
        {
        case R_PSU2_SERVICE_DIALING_INFO_VIEW_COPY_MENUPANE:
            {
            // Do nothing
            break;
            }
        default:
            {
            CPsu2InfoViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::NameListControlResourceId
// --------------------------------------------------------------------------
//
TInt CPsu2ServiceDialingInfoView::NameListControlResourceId() const
    {
    return 0;
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::NaviPaneLabelL
// --------------------------------------------------------------------------
//
HBufC* CPsu2ServiceDialingInfoView::NaviPaneLabelL() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::UpdateCbasL
// -----------------------------------------------------------------------------
//
void CPsu2ServiceDialingInfoView::UpdateCbasL()
    {
    TInt resId( KErrNotFound );
    if ( ContactHasPhoneNumberL( iControl->FocusedStoreContact() ) )
        {
        resId = R_PBK2_SOFTKEYS_OPTIONS_BACK_CONTEXT;
        }
    else
        {
        resId = R_PBK2_SOFTKEYS_OPTIONS_BACK_EMPTY;
        }
    // Set cba command set "Options - Context Menu - Back"
    iExtensionView.Cba()->SetCommandSetL( resId );
    iExtensionView.Cba()->DrawDeferred();
    // Set context menu
    iExtensionView.MenuBar()->SetContextMenuTitleResourceId
        ( R_PSU2_SERVICE_DIALING_INFO_CONTEXT_MENUBAR );
    }

// -----------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::ContactHasPhoneNumberL
// -----------------------------------------------------------------------------
//
TBool CPsu2ServiceDialingInfoView::ContactHasPhoneNumberL
        ( const MVPbkStoreContact* aContact )
    {
    __ASSERT_DEBUG( aContact, Panic( EPanicLogic_HasContactPhoneNumberL ) );

    TBool result = EFalse;

    TPbk2StoreContactAnalyzer analyzer
        ( Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager(), aContact );
    if ( analyzer.HasFieldL
            ( R_PHONEBOOK2_PHONENUMBER_SELECTOR ) != KErrNotFound )
        {
        result = ETrue;
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPsu2ServiceDialingInfoView::HandleControlEventL
// --------------------------------------------------------------------------
//    
void CPsu2ServiceDialingInfoView::HandleControlEventL(
        MPbk2ContactUiControl& aControl,
        const TPbk2ControlEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EContactDoubleTapped:
            {
            // launch context menu if user tapped a focused detail
            iExtensionView.LaunchPopupMenuL
                ( R_PSU2_SERVICE_DIALING_INFO_CONTEXT_MENUBAR );
            break;
            }
            
        default:
            {
            CPsu2InfoViewBase::HandleControlEventL( aControl, aEvent );
            break;
            }
        }
    }
//  End of File
