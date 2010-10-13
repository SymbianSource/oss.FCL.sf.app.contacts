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
* Description:  Phonebook 2 USIM Own Numbers view.
*
*/


#include "CPsu2OwnNumbersView.h"

// Phonebook 2
#include "CPsu2OwnNumberControl.h"
#include "CPsu2ViewManager.h"
#include <CPbk2UIExtensionView.h>
#include <MPbk2ViewActivationTransaction.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ViewExplorer.h>
#include <Pbk2USimUIRes.rsg>
#include <CPbk2ControlContainer.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <CPbk2ContactUiControlSubstitute.h>
#include <MPbk2CommandHandler.h>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreInfo.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactViewBase.h>

// System includes
#include <csxhelp/phob.hlp.hrh>
#include <StringLoader.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <AknUtils.h>


// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::CPsu2OwnNumbersView
// --------------------------------------------------------------------------
//
CPsu2OwnNumbersView::CPsu2OwnNumbersView
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager ):
            iExtensionView( aExtensionView ),
            iViewManager( aViewManager )
    {
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::~CPsu2OwnNumbersView
// --------------------------------------------------------------------------
//
CPsu2OwnNumbersView::~CPsu2OwnNumbersView()
    {
    delete iContainer;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::NewL
// --------------------------------------------------------------------------
//
CPsu2OwnNumbersView* CPsu2OwnNumbersView::NewL
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager )
    {
    CPsu2OwnNumbersView* self =
        new( ELeave ) CPsu2OwnNumbersView( aExtensionView, aViewManager );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::DoActivateL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::DoActivateL(const TVwsViewId& aPrevViewId,
            TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    // Load FDN title
    HBufC* title = StringLoader::LoadLC( R_TEXT_OWN_NUMBER_LIST );

    MPbk2ViewActivationTransaction* viewActivationTransaction =
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
            ( iExtensionView.Id(), aPrevViewId, title, NULL,
              Phonebook2::EUpdateNaviPane | Phonebook2::EUpdateTitlePane );

    CreateControlsL();
    iContainer->ActivateL();

    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(2 , title); // viewActivationTransaction
    UpdateCbasL();

    iViewManager.RegisterStoreAndView();
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::DoDeactivate()
    {
    if (iContainer)
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iContainer);
        // Store current state, safe to ignore. There's no real harm,
        // if theres no stored state when activating this view again
        delete iContainer;
        iContainer = NULL;
        // iControl is owned by iContainer and it is deleted too
        iControl = NULL;
        }

    iViewManager.DeregisterStoreAndView();
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::NameListControlResourceId
// --------------------------------------------------------------------------
//
TInt CPsu2OwnNumbersView::NameListControlResourceId() const
    {
    return R_PSU2_OWN_NUMBER_NAME_LIST_CONTROL;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(iExtensionView.ClientRect());
        }
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPsu2OwnNumbersView::ViewStateLC() const
    {
    CPbk2ViewState* state = NULL;
    if ( iControl )
        {
        iControl->ControlStateL();
        }
    CleanupStack::PushL( state );
    return state;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPsu2OwnNumbersView::HandleCommandKeyL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType )
    {
    TBool result( EFalse );
    if( aKeyEvent.iCode == EKeyPhoneSend && aType == EEventKey  )
        {
        if ( iControl->NumberOfContacts() > 0 )
            {
            HandleCommandL(EPbk2CmdCall);
            }
        result = ETrue;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::HandlePointerEventL(
        const TPointerEvent& /*aPointerEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPsu2OwnNumbersView::GetViewSpecificMenuFilteringFlagsL() const
    {
    return KPbk2MenuFilteringFlagsNone;
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::HandleCommandL(TInt aCommand)
    {
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

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::DynInitMenuPaneL(TInt /*aResourceId*/,
        CEikMenuPane* /*aMenuPane*/ )
    {
    // no filtering here
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::HandleLongTapEventL(
    const TPoint& /*aPenEventLocation*/,
    const TPoint& /*aPenEventScreenLocation*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::CreateControlsL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::CreateControlsL()
    {
    if ( !iContainer )
        {
        // Create the container and control
        CContainer* container = CContainer::NewLC
            ( &iExtensionView, &iExtensionView, iExtensionView );

        container->SetHelpContext(
            TCoeHelpContext( iExtensionView.ApplicationUid(),
                             KPHOB_HLP_MY_NUMBERS ));

        CPsu2OwnNumberControl* control = CPsu2OwnNumberControl::NewL
            ( iViewManager.CurrentSimView(), container );
        CleanupStack::PushL(control);

        // takes ownership of the control
        container->SetControl( control, iExtensionView.ClientRect() );
        CleanupStack::Pop(control);

        CCoeEnv::Static()->AppUi()->AddToStackL(iExtensionView, container);
        CleanupStack::Pop(container);
        iContainer = container;
        iControl = control;
        }

    }
// --------------------------------------------------------------------------
// CPsu2OwnNumbersView::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPsu2OwnNumbersView::UpdateCbasL()
    {
    // Set middle softkey as empty
    iExtensionView.Cba()->SetCommandSetL
        ( R_PSU_OWN_SOFTKEYS_OPTIONS_BACK_EMPTY );
    iExtensionView.Cba()->DrawDeferred();
    }

///  End of File
