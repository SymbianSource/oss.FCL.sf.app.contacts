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
* Description:  Phonebook 2 application UI.
*
*/


// INCLUDE FILES
#include "CPbk2AppUi.h"

// Phonebook 2
#include "CPbk2StartupMonitor.h"
#include "CPbk2Application.h"
#include "CPbk2Document.h"
#include "CPbk2ViewExplorer.h"
#include "CPbk2TabGroupContainerExtensionModifier.h"
#include "MPbk2ApplicationServices.h"
#include "MPbk2ApplicationServices2.h"
#include <CPbk2AppViewBase.h>
#include <Pbk2DataCaging.hrh>
#include <CPbk2StoreManager.h>
#include <Pbk2MenuFilteringFlags.hrh>
#include <Pbk2Commands.hrh>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2UIExtensionFactory.h>
#include <CPbk2ViewState.h>
#include <Phonebook2PrivateCRKeys.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2TabGroupContainer.h>
#include <MPbk2ContactViewSupplier.h>
#include <pbk2commonui.rsg>
#include <Pbk2UID.h>
#include <Phonebook2PublicPSKeys.h> 

// Virtual Phonebook
#include <VPbkVariant.hrh>

// System includes
#include <aknview.h>
#include <eikmenup.h>
#include <centralrepository.h>
#include <gfxtranseffect/gfxtranseffect.h>
#include <akntranseffect.h>
#include <e32property.h>

// Debugging headers
#include <Pbk2Config.hrh>
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KPbk2CommandsDllResFileName, "Pbk2Commands.rsc" );
_LIT( KPbk2UiControlsDllResFileName, "Pbk2UiControls.rsc" );
_LIT( KPbk2CommonUiDllResFileName, "Pbk2CommonUi.rsc" );

}  /// namespace


// --------------------------------------------------------------------------
// CPbk2AppUi::CPbk2AppUi
// --------------------------------------------------------------------------
//
CPbk2AppUi::CPbk2AppUi() :
        iUiControlsResourceFile( *iCoeEnv ),
        iCommandsResourceFile( *iCoeEnv ),
        iCommonUiResourceFile( *iCoeEnv )
    {
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::~CPbk2AppUi
// Application views will be deleted by the base class destructor
// --------------------------------------------------------------------------
//
CPbk2AppUi::~CPbk2AppUi()
    {
    CCoeEnv::Static()->RemoveForegroundObserver( *this );

    delete iStartupMonitor;
    delete iTabGroups;
    delete iViewExplorer;
    iUiControlsResourceFile.Close();
    iCommandsResourceFile.Close();
    iCommonUiResourceFile.Close();
    Release( iAppUiExtension );
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ConstructL(0x%x)"), this);

    PBK2_PROFILE_START(Pbk2Profile::EAppUiConstruction);

    // Init base class
    // Use local impl of the LocalVariationFeatureEnabled because app globals
    // haven't been initialized yet at this point.
    PBK2_PROFILE_START(Pbk2Profile::EAppUiBaseConstruct);
    
    //Set the single click compatibility on
    if ( LocalVariationFeatureEnabled( EVPbkLVEnableMiddleSoftKey ) )
        {
        BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
        }
    else
        {
        BaseConstructL( EAknEnableSkin | EAknSingleClickCompatible );
        }
    
    PBK2_PROFILE_END(Pbk2Profile::EAppUiBaseConstruct);

    // Load resource files
    PBK2_PROFILE_START(Pbk2Profile::EAppUiResouceFileLoading);
    iUiControlsResourceFile.OpenL(KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, KPbk2UiControlsDllResFileName);
    iCommandsResourceFile.OpenL(KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, KPbk2CommandsDllResFileName);
    iCommonUiResourceFile.OpenL(KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, KPbk2CommonUiDllResFileName);
    PBK2_PROFILE_END(Pbk2Profile::EAppUiResouceFileLoading);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ConstructL(0x%x) Creating globals"), this);
    // Initialize global objects after loading the resource files
    PBK2_PROFILE_START(Pbk2Profile::EAppUiCreateGlobals);
    iAppServices = PhonebookDocument()->ApplicationServicesL();
    iAppServicesExtension = reinterpret_cast<MPbk2ApplicationServices2*>
        ( PhonebookDocument()->ApplicationServicesL()->
            MPbk2ApplicationServicesExtension(
                KMPbk2ApplicationServicesExtension2Uid ) );
    PhonebookDocument()->CreateGlobalsL();
    PBK2_PROFILE_END(Pbk2Profile::EAppUiCreateGlobals);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ConstructL(0x%x) Globals created"), this);

    // Create view explorer
    PBK2_PROFILE_START(Pbk2Profile::EAppUiCreateViewExplorer);
    iViewExplorer = CPbk2ViewExplorer::NewL( *this );
    PBK2_PROFILE_END(Pbk2Profile::EAppUiCreateViewExplorer);

    // Create AppUi extension
    PBK2_PROFILE_START(Pbk2Profile::EAppUiCreateAppUiExtension);
    MPbk2UIExtensionFactory* extFactory =
        PhonebookDocument()->ExtensionManager().FactoryL();
    iAppUiExtension = extFactory->CreatePbk2AppUiExtensionL
        ( iAppServices->ContactManager() );
    PBK2_PROFILE_END(Pbk2Profile::EAppUiCreateAppUiExtension);

    // Create tab group
    PBK2_PROFILE_START(Pbk2Profile::EViewExplorerConstructionCreateTabGroup);
    // Allow extensions to extend the tab group
    CPbk2TabGroupContainerExtensionModifier* tabGroupModifier =
            CPbk2TabGroupContainerExtensionModifier::NewLC(
                    *iAppUiExtension);
    // Read and create the tab groups
    iTabGroups = CPbk2TabGroupContainer::NewL( *iViewExplorer,
            *tabGroupModifier );
    CleanupStack::PopAndDestroy(tabGroupModifier);
    PBK2_PROFILE_END(Pbk2Profile::EViewExplorerConstructionCreateTabGroup);

    // Create start-up monitor
    PBK2_PROFILE_START(Pbk2Profile::EAppUiCreateStartupMonitor);
    iStartupMonitor = CPbk2StartupMonitor::NewL
        ( *iAppUiExtension, iAppServices->StoreProperties(),
          iAppServices->StoreConfiguration(),
          iAppServices->ContactManager() );
    iStartupMonitor->StartupBeginsL();
    PBK2_PROFILE_END(Pbk2Profile::EAppUiCreateStartupMonitor);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ConstructL(0x%x) Opening AllContactsView"), this);

    // Open the all contacts view...
    Phonebook2::Pbk2AppUi()->ApplicationServices().
        ViewSupplier().AllContactsViewL();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ConstructL(0x%x) Opening stores"), this);

    // ...and only after that, order the stores to be opened
    PBK2_PROFILE_START(Pbk2Profile::EAppUiOpenStores);
    iAppServicesExtension->StoreManager().OpenStoresL();
    PBK2_PROFILE_END(Pbk2Profile::EAppUiOpenStores);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ConstructL(0x%x) Launching names list"), this);

    // Create views
    PBK2_PROFILE_START(Pbk2Profile::EAppUiCreateViews);
    iViewExplorer->CreateViewsL();
    PBK2_PROFILE_END(Pbk2Profile::EAppUiCreateViews);

    CCoeEnv::Static()->AddForegroundObserverL(*this);

    if ( !IsForeground() )
        {
        if ( ExitHidesInBackground() )
            {
            LeavePbkInMemExitL();
            }
        }
    
    PBK2_PROFILE_END(Pbk2Profile::EAppUiConstruction);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::Document
// --------------------------------------------------------------------------
//
CPbk2Document* CPbk2AppUi::PhonebookDocument() const
    {
    // Explicit cast: the document must always be of type CPbk2Document
    return static_cast<CPbk2Document*>( Document() );
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::StoreManager
// --------------------------------------------------------------------------
//
CPbk2StoreManager& CPbk2AppUi::StoreManager() const
    {
    return iAppServicesExtension->StoreManager();
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::Pbk2Application
// --------------------------------------------------------------------------
//
CPbk2Application* CPbk2AppUi::Pbk2Application() const
    {
    // Explicit cast: the application must always be of type CPbk2Application
    return static_cast<CPbk2Application*>( Application() );
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::ExitL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::ExitL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::ExitL"));

    TInt err = RProperty::Set( TUid::Uid( KPbk2PSUidPublic ),
                               KPhonebookOpenCompleted, EPhonebookClosed );
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ExitL set pubsub key to EPhonebookClosed (%d)"), err);

#ifdef _DEBUG
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::ExitL real exit debug"));
    CAknViewAppUi::ProcessCommandL( EAknCmdExit );
#else
    if ( ExitHidesInBackground() )
    	{
    	LeavePbkInMemExitL();
    	}
    else
    	{
    	PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::ExitL real exit"));
		CAknViewAppUi::ProcessCommandL( EAknCmdExit );
    	}   
#endif
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::ExitL end"));
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::ActivatePhonebookViewL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::ActivatePhonebookViewL
        ( TUid aViewId, const CPbk2ViewState* aViewState /*=NULL*/ )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::ActivatePhonebookViewL"));
    
    if ( aViewState )
        {
        HBufC8* stateBuf = aViewState->PackLC();
        ActivateLocalViewL(aViewId, CPbk2ViewState::Uid(), *stateBuf);
        CleanupStack::PopAndDestroy();  // stateBuf
        }
    else
        {
        ActivateLocalViewL(aViewId);
        }
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::ActivatePhonebookViewL end"));
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::ActiveView
// --------------------------------------------------------------------------
//
CPbk2AppViewBase* CPbk2AppUi::ActiveView() const
    {
    // Explicit cast: iView must always be of type CPbk2AppViewBase
    return static_cast<CPbk2AppViewBase*>( iView );
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::Pbk2ViewExplorer
// --------------------------------------------------------------------------
//
MPbk2ViewExplorer* CPbk2AppUi::Pbk2ViewExplorer() const
    {
    return iViewExplorer;
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::Pbk2StartupMonitor
// --------------------------------------------------------------------------
//
MPbk2StartupMonitor* CPbk2AppUi::Pbk2StartupMonitor() const
    {
    return iStartupMonitor;
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::TabGroup
// --------------------------------------------------------------------------
//
CPbk2TabGroupContainer* CPbk2AppUi::TabGroups() const
    {
    return iTabGroups;
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::KeyEventHandler
// --------------------------------------------------------------------------
//
MPbk2KeyEventHandler& CPbk2AppUi::KeyEventHandler()
    {
    return *this;
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::ApplicationServices
// --------------------------------------------------------------------------
//
MPbk2ApplicationServices& CPbk2AppUi::ApplicationServices() const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2AppUi::ApplicationServices() returning 0x%x"), iAppServices );

    return *iAppServices;
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::HandleCommandL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            // Standard exit command
            CmdExit();
            break;
            }
            
        case EAknCmdHideInBackground:
        	{
            // Leave app in ram
        	LeavePbkInMemExitL();
        	break;
        	}

        case EAknCmdExit:
        case EAknSoftkeyExit:
        case EPbk2CmdExit:
            {
            // Perhaps leave app in ram
            ExitL();
            break;
            }      
            
        case EAknSoftkeyBack:
            {
            iViewExplorer->ActivatePreviousViewL( NULL );
            break;
            }

        default:
            {
            CAknViewAppUi::HandleCommandL( aCommand );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::ExitApplication
// --------------------------------------------------------------------------
//
void CPbk2AppUi::ExitApplication()
    {
    Exit();
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::HandleGainingForeground
// --------------------------------------------------------------------------
//
void CPbk2AppUi::HandleGainingForeground()
    {    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::HandleGainingForeground"));

    if ( ExitHidesInBackground() )
        {
        TRAP_IGNORE( iStartupMonitor->RestartStartupL() );
        }
    
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::HandleGainingForeground end"));
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::HandleLosingForeground
// --------------------------------------------------------------------------
//
void CPbk2AppUi::HandleLosingForeground()
    {
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
            "void CPbk2AppUi::HandleLosingForeground" ) );    

    // we need to close possible popup menu when application is sent to
    // background. Otherwise AvKon will likely crash since its
    // intermediate state array is corrupted.
    
    // make sure we have a view
    if (ActiveView())
        {
        ActiveView()->ClosePopup();
        }
    
    PBK2_DEBUG_PRINT( PBK2_DEBUG_STRING(
                "void CPbk2AppUi::HandleLosingForeground end" ) );
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::DynInitMenuPaneL
        ( TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/ )
    {
    // All menu filtering happens elsewhere
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::HandleKeyEventL
// --------------------------------------------------------------------------
//
TKeyResponse CPbk2AppUi::HandleKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if (Pbk2ProcessKeyEventL(aKeyEvent, aType))
        {
        return EKeyWasConsumed;
        }
    return EKeyWasNotConsumed;
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::HandleResourceChangeL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::HandleResourceChangeL(TInt aType)
    {
    CAknAppUiBase::HandleResourceChangeL( aType );

    // This is needed to change the application icon when the skin changes
    // (especially if a contact with a thumbnail is currently selected
    // and the app icon is not visible)
    CEikStatusPane* pane = StatusPane();
    if ( pane )
        {
        pane->HandleResourceChange( aType );
        }
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::HandleForegroundEventL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::HandleForegroundEventL( TBool aForeground )
    {
    iAppServicesExtension->StoreManager().EnableCompression( aForeground );

    CAknViewAppUi::HandleForegroundEventL( aForeground );
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::Pbk2ProcessKeyEventL
// --------------------------------------------------------------------------
//
TBool CPbk2AppUi::Pbk2ProcessKeyEventL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    return iTabGroups->HandleNavigationKeyEventL( aKeyEvent, aType );
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::CmdExit
// --------------------------------------------------------------------------
//
inline void CPbk2AppUi::CmdExit()
    {
    CAknViewAppUi::Exit();
    }

// --------------------------------------------------------------------------
// CPbk2AppUi::LocalVariationFeatureEnabled
// --------------------------------------------------------------------------
//
TBool CPbk2AppUi::LocalVariationFeatureEnabled( TInt aFeatureFlag )
    {
    TBool ret( EFalse );
    TInt lvFlags( KErrNotFound );
    CRepository* key = NULL;
    // Read local variation flags
    TRAPD( err, key =
            CRepository::NewL( TUid::Uid( KCRUidPhonebook ) ) );
    // if NewL fails do not set iLVFlags, new query is made next time
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

// --------------------------------------------------------------------------
// CPbk2AppUi::LeavePbkInMemExitL
// --------------------------------------------------------------------------
//
void CPbk2AppUi::LeavePbkInMemExitL()
	{
	PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::LeavePbkInMemExitL"));
	
	// Diasable bring to foreground on view activation event
	SetCustomControl(1);
	
	// Hide application in background
	HideInBackground();
	 
	// Activation of Names list view will send application to
	// background. Application can not be sent here to background
	// because  activation is asynchronous operation and application
	// could be set to background before the view activation.
	CPbk2ViewState* state = CPbk2ViewState::NewLC();
	            state->SetFlags( CPbk2ViewState::EInitialized |
	                CPbk2ViewState::ESendToBackground );
	
	ActivatePhonebookViewL( Phonebook2::KPbk2NamesListViewUid, state );
	
	CleanupStack::PopAndDestroy( state );
	
	PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING("CPbk2AppUi::LeavePbkInMemExitL end"));
	}
//  End of File
