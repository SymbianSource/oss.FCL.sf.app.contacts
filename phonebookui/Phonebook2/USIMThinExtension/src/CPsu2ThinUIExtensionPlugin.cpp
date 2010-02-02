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
* Description:  Phonebook 2 USIM Thin UI extension plugin.
*
*/


// INCLUDE FILES
#include "CPsu2ThinUIExtensionPlugin.h"

// Phonebook 2
#include "CPsu2CopySimContactsCmd.h"
#include <Pbk2USimUI.hrh>
#include <Pbk2USimUIRes.rsg>
#include <Pbk2Commands.rsg>
#include <MPbk2ContactUiControl.h>
#include <MPbk2StartupMonitor.h>
#include "CPsu2CheckAvailabeSimStore.h"

// Virtual Phonebook
#include <VPbkSimStoreFactory.h>
#include <MVPbkSimPhone.h>
#include <VPbkContactStoreUris.h>

// System includes
#include <eikmenup.h>

#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

/// All stores in "SIM Memory" cascade menu
const TUint32 KAllSimStores=
    MVPbkSimPhone::KVPbkCapsSDNSupported |
    MVPbkSimPhone::KVPbkCapsFDNSupported |
    MVPbkSimPhone::KVPbkCapsONSupported;
    
/**
 * Dims given menu item from given menu pane.
 *
 * @param aMenuPane     Menu pane where menu item resides.
 * @param aCommandId    Id of the menu item to dim.
 */
void DimMenuItem( CEikMenuPane& aMenuPane, TInt aCommandId )
    {
    TInt pos = KErrNotFound;
    if ( aMenuPane.MenuItemExists( aCommandId, pos ) )
        {
        aMenuPane.SetItemDimmed( aCommandId, ETrue );
        }
    }

} /// namespace


// --------------------------------------------------------------------------
// CPsu2CopySimContactsCmd::CPsu2CopySimContactsCmd
// --------------------------------------------------------------------------
//
CPsu2ThinUIExtensionPlugin::CPsu2ThinUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::~CPsu2ThinUIExtensionPlugin
// --------------------------------------------------------------------------
//
CPsu2ThinUIExtensionPlugin::~CPsu2ThinUIExtensionPlugin()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPsu2ThinUIExtensionPlugin::~CPsu2ThinUIExtensionPlugin(0x%x)"), this);
    
    delete iStoreChecker;
    delete iCopySimContactsCmd;
    delete iPhone;
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CPsu2ThinUIExtensionPlugin* CPsu2ThinUIExtensionPlugin::NewL()
    {
    CPsu2ThinUIExtensionPlugin* self =
        new( ELeave ) CPsu2ThinUIExtensionPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::ConstructL()
    {
    iServiceTable.Reset();
    iPhone = VPbkSimStoreFactory::CreatePhoneL();
    iPhone->OpenL( *this );
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane,
          MPbk2ContactUiControl& aControl )
    {
    switch (aResourceId)
        {
        case R_PHONEBOOK2_NAMESLIST_OTHER_MEMORIES_MENU_PLACEHOLDER:
            {
            if ( iServiceTable.IsOff( KAllSimStores ) ||
 //                aControl.ContactsMarked() || 
                 ( iStoreChecker &&
                   !iStoreChecker->IsContactsAvailabe(
                        VPbkContactStoreUris::SimGlobalOwnNumberUri()) &&
//SDN not shown in sim menu anymore (in ECE)
//                 !iStoreChecker->IsContactsAvailabe(
//                      VPbkContactStoreUris::SimGlobalSdnUri()) &&
                   iServiceTable.IsOff( MVPbkSimPhone::KVPbkCapsFDNSupported )
                 ) 
               )
                {
                // Dim "SIM Memory" cascading menu if there are no
                // SIM stores to launch, or there are marked items
                DimMenuItem(*aMenuPane, EPsu2CmdCascadingSimMemory);
                }
            break;
            }

        case R_PSU2_CASCADING_SIM_MEMORY_MENU_PANE:
            {
            // My number dimmed until index bug is corrected in SIM server
            if ( iServiceTable.IsOff
                    ( MVPbkSimPhone::KVPbkCapsONSupported ) ||
                 ( iStoreChecker &&
                  !iStoreChecker->IsContactsAvailabe(
                    VPbkContactStoreUris::SimGlobalOwnNumberUri() ) ) )
                {
                // "My Number" item
                DimMenuItem( *aMenuPane, EPsu2CmdLaunchOwnNumberView );
                }
//SDN not shown in sim menu anymore so dim it always (in ECE)               
//          if ( iServiceTable.IsOff
//                  ( MVPbkSimPhone::KVPbkCapsSDNSupported ) ||
//               ( iStoreChecker &&                    
//                !iStoreChecker->IsContactsAvailabe(
//                  VPbkContactStoreUris::SimGlobalSdnUri() ) ) )
                {
                // "Service Dialing" item
                DimMenuItem( *aMenuPane, EPsu2CmdLaunchServiceDialingView );
                }
            if ( iServiceTable.IsOff
                    ( MVPbkSimPhone::KVPbkCapsFDNSupported ) )
                {
                // "Fixed Dialing" item
                DimMenuItem( *aMenuPane, EPsu2CmdLaunchFixedDialingView );
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
// CPsu2ThinUIExtensionPlugin::ExtensionStartupL
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::ExtensionStartupL
        ( MPbk2StartupMonitor& aStartupMonitor)
    {
    aStartupMonitor.RegisterEventsL( *this );
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::PhoneOpened
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::PhoneOpened( MVPbkSimPhone& aPhone )
    {
    iServiceTable.SetServiceTable( aPhone.ServiceTable() );
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::PhoneError
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::PhoneError
        ( MVPbkSimPhone& /*aPhone*/, TErrorIdentifier /*aIdentifier*/,
          TInt /*aError*/ )
    {
    iServiceTable.Reset();
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::ServiceTableUpdated
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::ServiceTableUpdated( TUint32 aServiceTable )
    {
    iServiceTable.SetServiceTable( aServiceTable );
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::FixedDiallingStatusChanged
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::FixedDiallingStatusChanged
        ( TInt /*aFDNStatus*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::ContactUiReadyL
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::ContactUiReadyL
        ( MPbk2StartupMonitor& aStartupMonitor )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPsu2ThinUIExtensionPlugin::ContactUiReadyL(0x%x) (0x%x) (0x%x)"), this, iStoreChecker, iCopySimContactsCmd);

    aStartupMonitor.DeregisterEvents( *this );

    if ( !iCopySimContactsCmd )
        {
        MPbk2Command* cmd = CPsu2CopySimContactsCmd::NewL( aStartupMonitor );
        cmd->ExecuteLD();
        iCopySimContactsCmd = cmd;
        iCopySimContactsCmd->AddObserver( *this );
        }
    
    delete iStoreChecker; //needed when alwaysOn enabled
    iStoreChecker = NULL; 
    iStoreChecker = CPsu2CheckAvailabeSimStore::NewL();
    }

// --------------------------------------------------------------------------
// CPsu2ThinUIExtensionPlugin::CommandFinished
// --------------------------------------------------------------------------
//
void CPsu2ThinUIExtensionPlugin::CommandFinished
        ( const MPbk2Command& /*aCommand*/ )
    {
    delete iCopySimContactsCmd;
    iCopySimContactsCmd = NULL;
    }

//  End of File
