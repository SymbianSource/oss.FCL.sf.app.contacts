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
* Description:  Phonebook 2 settings application view.
*
*/


// INCLUDE FILES
#include "CPbk2SettingsView.h"

// Phonebook 2
#include "CPbk2AppUi.h"
#include "CPbk2Document.h"
#include <Pbk2Commands.hrh>
#include <MPbk2ViewExplorer.h>
#include <CPbk2SettingsListControl.h>
#include <CPbk2ControlContainer.h>
#include <MPbk2ViewActivationTransaction.h>
#include <Pbk2UIControls.hrh>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>
#include <MPbk2SettingsViewExtension.h>
#include <MPbk2SettingsControlExtension.h>
#include <CPbk2ContactUiControlSubstitute.h>
#include <MPbk2CommandHandler.h>
#include <MPbk2ApplicationServices.h>
#include <csxhelp/phob.hlp.hrh>
#include <Phonebook2.rsg>
#include <Pbk2UIControls.rsg>

// System includes
#include <StringLoader.h>
#include <aknnavide.h>  // CAknNavigationDecorator

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPbk2SettingsView::CPbk2SettingsView
// --------------------------------------------------------------------------
//
CPbk2SettingsView::CPbk2SettingsView()
    {
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::~CPbk2SettingsView
// --------------------------------------------------------------------------
//
CPbk2SettingsView::~CPbk2SettingsView()
    {
    Release( iSettingsExtension );
    Release( iSettingsControlExtension );
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::NewL
// --------------------------------------------------------------------------
//
CPbk2SettingsView* CPbk2SettingsView::NewL()
    {
    CPbk2SettingsView* self = new ( ELeave ) CPbk2SettingsView();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2SettingsView::ConstructL()
    {
    BaseConstructL( R_PHONEBOOK2_SETTINGS_APPVIEW );
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPbk2SettingsView::HandleCommandL( TInt aCommandId )
    {
    if ( !iSettingsExtension->HandleCommandL( aCommandId ) )
        {
        switch ( aCommandId )
            {
            case EAknSoftkeyBack:               // FALLTHROUGH
            case EPbk2CmdOpenPreviousView:
                {                    
                CPbk2ViewState* state = ViewStateLC();
                // To be sure that there is a view uid available.
                // In some rare cases there is no view uid.
                if ( iPreviousViewId.iAppUid == TUid::Uid( 0 ) && 
                     iPreviousViewId.iViewUid == TUid::Uid( 0 ) )
                    {                    
                    Pbk2AppUi()->Pbk2ViewExplorer()->ActivatePreviousViewL
                        ( state );                    
                    }
                else
                    {
                    Pbk2AppUi()->Pbk2ViewExplorer()->ActivatePhonebook2ViewL
                        ( iPreviousViewId.iViewUid, state );                    
                    }
                CleanupStack::PopAndDestroy(state);
                break;
                }
            case EPbk2CmdEditSetting:
                {
                iControl->EditItemL(
                        iControl->ListBox()->CurrentItemIndex(), ETrue );       
                break;
                }
            case EAknCmdHelp:
                {
                // Help does not need uiControl so we fake it,
                // then we can use commandhandler for help
                CPbk2ContactUiControlSubstitute* uiControlSubstitute =
                    CPbk2ContactUiControlSubstitute::NewL();
                CleanupStack::PushL( uiControlSubstitute );

                Pbk2AppUi()->ApplicationServices().CommandHandlerL()->
                    HandleCommandL
                        ( aCommandId, *uiControlSubstitute, this );
                CleanupStack::PopAndDestroy( uiControlSubstitute );
                break;
                }
            default:
                {
                AppUi()->HandleCommandL( aCommandId );
                break;
                }
            };
        }
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2SettingsView::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    iSettingsExtension->DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::Id
// --------------------------------------------------------------------------
//
TUid CPbk2SettingsView::Id() const
    {
    return Phonebook2::KPbk2SettingsViewUid;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPbk2SettingsView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::DoActivateL
// --------------------------------------------------------------------------
//
void CPbk2SettingsView::DoActivateL( const TVwsViewId& aPrevViewId,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SettingsView(%x)::DoActivateL()"), this );

    if ( !iSettingsExtension )
        {
        iSettingsExtension =
            Pbk2AppUi()->PhonebookDocument()->ExtensionManager().FactoryL()->
                CreatePbk2SettingsViewExtensionL
                    ( Pbk2AppUi()->ApplicationServices().ContactManager() );
        }

    HBufC* title = StringLoader::LoadLC( R_PHOB_SET_TITLE );

    MPbk2ViewActivationTransaction* viewActivationTransaction =
        Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
            ( Id(), aPrevViewId, title, NULL,
              Phonebook2::EUpdateNaviPane | Phonebook2::EUpdateTitlePane |
              Phonebook2::EUpdateContextPane );

    CreateControlsL();

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

    // Transaction is commited after all leaving code has been executed
    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy( 2 ); // viewActivationTransaction, title

    iContainer->SetHelpContext( TCoeHelpContext( ApplicationUid(),
        KPHOB_HLP_SETTINGS ));
        
    iPreviousViewId = aPrevViewId;        
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPbk2SettingsView::DoDeactivate()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2SettingsView(%x)::DoDeactivate()"), this );

    if (iNaviPane && iNaviDecorator)
        {
        iNaviPane->Pop(iNaviDecorator);
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;

    if ( iContainer )
        {
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }

    Release( iSettingsExtension );
    iSettingsExtension = NULL;
    Release( iSettingsControlExtension );
    iSettingsControlExtension = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2SettingsView::GetViewSpecificMenuFilteringFlagsL() const
    {
    return 0;
    }

// --------------------------------------------------------------------------
// CPbk2SettingsView::CreateControlsL
// --------------------------------------------------------------------------
//
inline void CPbk2SettingsView::CreateControlsL()
    {
    if ( !iSettingsControlExtension )
        {
        iSettingsControlExtension =
            iSettingsExtension->CreatePbk2SettingsControlExtensionL();
        }

    if ( !iContainer )
        {
        // Create the container and control
        CContainer* container = CContainer::NewLC( this, NULL, *this );

        container->SetHelpContext
            ( TCoeHelpContext( ApplicationUid(), KPHOB_HLP_SETTINGS ) );

        CPbk2SettingsListControl* control = CPbk2SettingsListControl::NewL
            ( container,
              Pbk2AppUi()->ApplicationServices().SortOrderManager(),
              *iSettingsControlExtension );
        CleanupStack::PushL( control );

        container->SetControl( control, ClientRect() ); // takes ownership
        CleanupStack::Pop( control );

        container->SetMopParent( this );
        container->MakeVisible( ETrue );

        AppUi()->AddToStackL( *this, container );
        CleanupStack::Pop( container );
        iContainer = container;
        iControl = control;

        container->ActivateL();
        }
    }

//  End of File
