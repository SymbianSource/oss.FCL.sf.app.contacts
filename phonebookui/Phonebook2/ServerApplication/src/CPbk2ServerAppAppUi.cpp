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
* Description:  Phonebook 2 application server application UI.
*
*/


#include "CPbk2ServerAppAppUi.h"

// Phonebook 2
#include "CPbk2ServerAppDocument.h"
#include "MPbk2ApplicationServices.h"
#include "MPbk2ApplicationServices2.h"

#include <Pbk2DataCaging.hrh>
#include <Phonebook2PrivateCRKeys.h>

// Virtual Phonebook
#include <VPbkVariant.hrh>

// System includes
#include <centralrepository.h>
#include <eikspane.h>
#include <avkon.rsg>
#include <akntoolbar.h>
#include <akntitle.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KPbk2CommandsDllResFileName, "Pbk2Commands.rsc" );
_LIT( KPbk2UiControlsDllResFileName, "Pbk2UiControls.rsc" );
_LIT( KPbk2CommonUiDllResFileName, "Pbk2CommonUi.rsc" );

} /// namespace

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::CPbk2ServerAppAppUi
// --------------------------------------------------------------------------
//
CPbk2ServerAppAppUi::CPbk2ServerAppAppUi():
        iUiControlsResourceFile( *iCoeEnv ),
        iCommandsResourceFile( *iCoeEnv ),
        iCommonUiResourceFile( *iCoeEnv )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::~CPbk2ServerAppAppUi
// --------------------------------------------------------------------------
//
CPbk2ServerAppAppUi::~CPbk2ServerAppAppUi()
    {
    iUiControlsResourceFile.Close();
    iCommandsResourceFile.Close();
    iCommonUiResourceFile.Close();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppAppUi::ConstructL()
    {
    // Init base class
    if ( LocalVariationFeatureEnabled( EVPbkLVEnableMiddleSoftKey ) )
        {     
        BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
        }
    else
        {
        BaseConstructL( EAknEnableSkin | EAknSingleClickCompatible );
        }
    
    iUiControlsResourceFile.OpenL
        ( KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR,
          KPbk2UiControlsDllResFileName );

    iCommandsResourceFile.OpenL
        ( KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR,
          KPbk2CommandsDllResFileName );

    iCommonUiResourceFile.OpenL
        ( KPbk2RomFileDrive, KDC_RESOURCE_FILES_DIR,
          KPbk2CommonUiDllResFileName );

    CPbk2ServerAppDocument* document =
        static_cast<CPbk2ServerAppDocument*>( Document() );
    iAppServices = document->ApplicationServicesL();
    document->CreateGlobalsL();

    // Make sure that we are using empty statuspane layout
    ChangeStatuspaneLayoutL( CPbk2ServerAppAppUi::EStatusPaneLayoutEmpty );
    
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::StoreManager
// --------------------------------------------------------------------------
//
CPbk2StoreManager& CPbk2ServerAppAppUi::StoreManager() const
    {
    MPbk2ApplicationServices2* appServicesExtension =
        reinterpret_cast<MPbk2ApplicationServices2*>
            ( iAppServices->MPbk2ApplicationServicesExtension(
                KMPbk2ApplicationServicesExtension2Uid ) );
    
    return appServicesExtension->StoreManager();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::ChangeStatuspaneLayoutL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppAppUi::ChangeStatuspaneLayoutL
        (  TPbk2ServerAppStatusPaneLayout aLayout )
    {
    TInt currentstatuspane = StatusPane()->CurrentLayoutResId();
    switch ( aLayout )
        {
        case EStatusPaneLayoutEmpty:
            {
            if( currentstatuspane != R_AVKON_STATUS_PANE_LAYOUT_EMPTY )
                {
                CEikStatusPane* sp = StatusPane();
                 // Fetch pointer to the title pane control
                sp->SwitchLayoutL(
                    R_AVKON_STATUS_PANE_LAYOUT_EMPTY );
                // To draw the skin on the toolbar requires the toolbar
                // to be visible. Skin drawing is handled by platform. 
                // Not drawing the skin could result in the toolbar area in 
                // landscape orientation being a strange value. 
                CAknToolbar* toolbar = CurrentFixedToolbar();
                if ( toolbar )
                    {
                    toolbar->SetToolbarVisibility( ETrue );
                    }
                }
            break;
            }

        case EStatusPaneLayoutUsual:
            {
            if ( currentstatuspane !=  R_AVKON_STATUS_PANE_LAYOUT_USUAL )
                {
                StatusPane()->SwitchLayoutL(
                    R_AVKON_STATUS_PANE_LAYOUT_USUAL );
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
// CPbk2ServerAppAppUi:ActiveView
// --------------------------------------------------------------------------
//
CPbk2AppViewBase* CPbk2ServerAppAppUi::ActiveView() const
    {
    // Not supported in Server App
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::Pbk2ViewExplorer
// --------------------------------------------------------------------------
//
MPbk2ViewExplorer* CPbk2ServerAppAppUi::Pbk2ViewExplorer() const
    {
    // Not supported in Server App
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::Pbk2StartupMonitor
// --------------------------------------------------------------------------
//
MPbk2StartupMonitor* CPbk2ServerAppAppUi::Pbk2StartupMonitor() const
    {
    // Not supported in Server App
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::TabGroups
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainer* CPbk2ServerAppAppUi::TabGroups() const
    {
    // Not supported in Server App
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::KeyEventHandler
// --------------------------------------------------------------------------
//
MPbk2KeyEventHandler& CPbk2ServerAppAppUi::KeyEventHandler()
    {
    return *this;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::ApplicationServices
// --------------------------------------------------------------------------
//
MPbk2ApplicationServices& CPbk2ServerAppAppUi::ApplicationServices() const
    {
    return *iAppServices;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::HandleCommandL
// --------------------------------------------------------------------------
//
void CPbk2ServerAppAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit: // FALLTHROUGH
        case EAknCmdExit:
            {
            Exit();
            break;
            }
        default:
            {
            CAknAppUi::HandleCommandL( aCommand );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::ExitApplication
// --------------------------------------------------------------------------
//
void CPbk2ServerAppAppUi::ExitApplication()
    {
    Exit();
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2ServerAppAppUi::Pbk2ProcessKeyEventL
        ( const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/ )
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ServerAppAppUi::LocalVariationFeatureEnabled
// --------------------------------------------------------------------------
//
TBool CPbk2ServerAppAppUi::LocalVariationFeatureEnabled( TInt aFeatureFlag )
    {
    TBool ret( EFalse );
    TInt lvFlags( KErrNotFound );
    CRepository* key = NULL;
    // Read local variation flags
    TRAPD( err, key =
            CRepository::NewL( TUid::Uid( KCRUidPhonebook ) ) );
    // If NewL fails do not set iLVFlags, new query is made next time
    if ( err == KErrNone )
        {
        err = key->Get( KPhonebookLocalVariationFlags, lvFlags );
        if ( err != KErrNone )
            {
            // If there were problems reading the flags,
            // assume everything is off
            lvFlags = 0;
            }
        delete key;
        }

    if ( lvFlags != KErrNotFound )
        {
        ret = lvFlags & aFeatureFlag;
        }

    return ret;
    }

void CPbk2ServerAppAppUi::HandleWsEventL(const TWsEvent& aEvent,CCoeControl* aDestination)
    {
    TKeyEvent keyEvent = *aEvent.Key();
    TInt type = aEvent.Type();
    
    // Filter out all the EEventKeyUp and EEventKeyDown send key
    // events. This avoids launching the dailer unnecessarily
    if (!(keyEvent.iScanCode == EStdKeyYes
          && (type == EEventKeyUp || type == EEventKeyDown)))
        {
        CAknAppUi::HandleWsEventL( aEvent, aDestination );
        }
    }

// End of File
