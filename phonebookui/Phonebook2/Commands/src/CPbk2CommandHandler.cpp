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
* Description:  Phonebook 2 command handler.
*
*/


// INCLUDE FILES
#include "CPbk2CommandHandler.h"

// Phonebook 2
#include "CPbk2CommandFactory.h"
#include "CPbk2CommandStore.h"
#include "CPbk2AiwInterestArray.h"
#include <MPbk2ViewExplorer.h>
#include <CPbk2AppViewBase.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2UIExtensionManager.h>
#include <MPbk2Command.h>
#include <pbk2commands.rsg>
#include <pbk2uicontrols.rsg>
#include <CPbk2ViewState.h>
#include <CPbk2PhonebookInfoDlg.h>
#include <TPbk2StoreContactAnalyzer.h>
#include <MPbk2ContactNameFormatter.h>
#include <CPbk2StoreProperty.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2ContactRelocator.h>
#include <MPbk2ContactLinkIterator.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <CPbk2FieldPropertyArray.h>
#include <cpbk2commandactivator.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>

// Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactAttributeManager.h>
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactLink.h>
#include <CVPbkSpeedDialAttribute.h>
#include <MVPbkStoreContact.h>
#include <VPbkContactStoreUris.h>

// System includes
#include <avkon.hrh>
#include <eikmenup.h>
#include <featmgr.h>
#include <hlplch.h>
#include <akntoolbar.h>
#include <eikmenub.h>


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_CmdOpenMeViewsL = 1,
    EPanicPreCond_CmdOpenSettingsViewL,
    EPanicPreCond_CmdOpenPreviousViewL,
    EPanicPreCond_CmdOpenHelpL,
    EPanicLogic_FocusedFieldMatchesFieldTypeL,
    EPanicLogic_FocusedContactHasFieldTypeL
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbk2CommandHandler");
    User::Panic(KPanicText, aReason);
    }
#endif //_DEBUG

/**
 * Checks if focused field is of given field type.
 *
 * @param aUiControl        UI control.
 * @param aSelectorResId    Selector resource id.
 * @param aContactManager   Virtual Phonebook contact manager.
 * @return  ETrue if field type matches with given types.
 */
TBool FocusedFieldMatchesFieldTypeL(
        MPbk2ContactUiControl& aUiControl,
        TInt aSelectorResId,
        CVPbkContactManager& aContactManager )
    {
    const MVPbkStoreContact* contact = aUiControl.FocusedStoreContact();
    __ASSERT_DEBUG(contact, Panic
            ( EPanicLogic_FocusedFieldMatchesFieldTypeL ) );

    TPbk2StoreContactAnalyzer analyzer( aContactManager, NULL );
    const MVPbkBaseContactField* field = aUiControl.FocusedField();
    return field &&
        analyzer.IsFieldTypeIncludedL( *field, aSelectorResId );
    }

/**
 * Checks if the focused contact have a given field type.
 *
 * @param aUiControl        UI control
 * @param aSelectorResId    Selector resource id.
 * @param aContactManager   Virtual Phonebook contact manager.
 * @return  Index of the field having the type, KErrNotFound if
 *          the contact does not have the field type.
 */
 TInt FocusedContactHasFieldTypeL(
        MPbk2ContactUiControl& aUiControl,
        TInt aSelectorResId,
        CVPbkContactManager& aContactManager )
    {
    TInt ret = KErrNotFound;
    const MVPbkStoreContact* contact = aUiControl.FocusedStoreContact();
    __ASSERT_DEBUG( contact,
        Panic( EPanicLogic_FocusedContactHasFieldTypeL ) );

    TPbk2StoreContactAnalyzer analyzer( aContactManager, contact );
    ret = analyzer.HasFieldL( aSelectorResId );
    return ret;
    }

/**
 * Checks if command is 'exit' command.
 *
 * @param aCommandId    The command to check.
 * @return  ETrue if command is an exit command, EFalse otherwise.
 */
TBool IsExitCommand( TInt aCommandId )
    {
    TBool ret = EFalse;

    if ( EEikCmdExit == aCommandId || EAknSoftkeyExit == aCommandId ||
         EAknCmdExit == aCommandId || EPbk2CmdExit == aCommandId )
        {
        ret = ETrue;
        }

    return ret;
    }

} /// namespace


// --------------------------------------------------------------------------
// CPbk2CommandHandler::CPbk2CommandHandler
// --------------------------------------------------------------------------
//
inline CPbk2CommandHandler::CPbk2CommandHandler()
    {
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::~CPbk2CommandHandler
// --------------------------------------------------------------------------
//
CPbk2CommandHandler::~CPbk2CommandHandler()
    {
    delete iAiwInterestArray;
    delete iCommandStore;
    delete iCommandFactory;
    delete iContactRelocator;
    Release(iExtensionManager);
    Release(iAppServices);
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2CommandHandler* CPbk2CommandHandler::NewL()
    {
    CPbk2CommandHandler* self = new(ELeave) CPbk2CommandHandler();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2CommandHandler::ConstructL()
    {
    iCommandFactory = CPbk2CommandFactory::NewL();
    iCommandStore = CPbk2CommandStore::NewL();
    iAiwInterestArray = CPbk2AiwInterestArray::NewL();
    iExtensionManager = CPbk2UIExtensionManager::InstanceL();
    iContactRelocator = CPbk2ContactRelocator::NewL();
    iAppServices = CPbk2ApplicationServices::InstanceL();

    // Initialize feature manager
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::HandleCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2CommandHandler::HandleCommandL
        ( const TInt aCommandId, MPbk2ContactUiControl& aControl,
          const CPbk2AppViewBase* aAppView )
    {
    TBool ret = ETrue;

    // Exit commands are forwarded right away to appui
    // Otherwise there is possibility that
    // 'exit' command is not handled in low memory situations
    if ( IsExitCommand( aCommandId ) )
        {
        ret = EFalse;
        }

    // Offer the command to AIW unless it is exit command
    if ( ret && !iAiwInterestArray->HandleCommandL
            ( aCommandId, aControl, KNullHandle ) )
        {
        // Try generic command, this can also be a command from an extension
        ret = CmdGenericCommandL( TPbk2CommandId( aCommandId ), aControl );
        if ( !ret )
            {
            ret = ETrue;
            switch ( aCommandId )
                {
                case EAknSoftkeyBack:               // FALLTHROUGH
                case EPbk2CmdOpenPreviousView:
                    {
                    CmdOpenPreviousViewL( aControl, aAppView );
                    break;
                    }
                case EPbk2CmdSettings:
                    {
                    CmdOpenSettingsViewL( aControl, aAppView,
                        Phonebook2::KPbk2SettingsViewUid );
                    break;
                    }
                case EAknCmdHelp:
                    {
                    CmdOpenHelpL( aControl, aAppView );
                    break;
                    }
                default:
                    {
                    ret = EFalse;
                    break;
                    }
                }
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane,
        CPbk2AppViewBase& aViewBase,
        MPbk2ContactUiControl& aControl )
    {
    // First, delegate to AIW interests.
    // If the menu initialization is handled there,
    // there's no need to continue here
    if ( !iAiwInterestArray->DynInitMenuPaneL
            ( aResourceId, *aMenuPane, aControl ) )
        {
        // Then call extensions
        iExtensionManager->DynInitMenuPaneL( aResourceId, aMenuPane,
            aViewBase, aControl );

        // First perform standard menu filtering
        PerformStandardMenuFilteringL( aResourceId, aMenuPane, aControl );

        // Then perform field type based filtering
        PerformFieldTypeBasedFilteringL( aResourceId, aMenuPane, aControl );

        // Finally perform store specific filtering
        PerformStoreSpecificFilteringL( aResourceId, aMenuPane, aControl );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::RegisterAiwInterestL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::RegisterAiwInterestL(
        const TInt aInterestId,
        const TInt aMenuResourceId,
        const TInt aInterestResourceId,
        const TBool aAttachBaseService)
    {
    iAiwInterestArray->QueueInterestL(aInterestId,
        aMenuResourceId, aInterestResourceId, aAttachBaseService);
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::ServiceCmdByMenuCmd
// --------------------------------------------------------------------------
//
TInt CPbk2CommandHandler::ServiceCmdByMenuCmd(
        TInt aMenuCmdId ) const
    {
    return iAiwInterestArray->ServiceCmdByMenuCmd( aMenuCmdId );
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::AddAndExecuteCommandL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::AddAndExecuteCommandL(
        MPbk2Command* aCommand)
    {
    iCommandStore->AddAndExecuteCommandL(aCommand);
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::AddMenuCommandObserver
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::AddMenuCommandObserver(
        MPbk2MenuCommandObserver& aObserver)
    {
    iCommandStore->AddMenuCommandObserver(aObserver);
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::RemoveMenuCommandObserver
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::RemoveMenuCommandObserver(
        MPbk2MenuCommandObserver& aObserver)
    {
    iCommandStore->RemoveMenuCommandObserver(aObserver);
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::DynInitToolbarL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::DynInitToolbarL
        ( TInt aResourceId, CAknToolbar* aToolbar,
          const CPbk2AppViewBase& /*aAppView*/,
          MPbk2ContactUiControl& aControl )
    {
    switch ( aResourceId )
        {
        case R_PBK2_NAMESLIST_TOOLBAR:
            {
            if ( aControl.FocusedContactIndex() == KErrNotFound )
                {
                // If there is not focused contact (empty list or otherwise no
                // focus) then disable calling and message writing
                aToolbar->SetItemDimmed( EPbk2CmdCall, ETrue, ETrue );
                aToolbar->SetItemDimmed( EPbk2CmdWriteNoQuery, ETrue, ETrue );
                }
            else if ( aControl.ContactsMarked() )
                {
                // Disable calling to many contacts
                aToolbar->SetItemDimmed( EPbk2CmdCall, ETrue, ETrue );
                }
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::OfferToolbarEventL
        ( TInt aCommand, MPbk2ContactUiControl& aControl,
          const CPbk2AppViewBase* aAppView )
    {
    if ( aAppView )
        {
    HandleCommandL( aCommand, aControl, aAppView );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::CmdOpenSettingsViewL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::CmdOpenSettingsViewL
        ( MPbk2ContactUiControl& aControl,
          const CPbk2AppViewBase* aAppView, TUid aViewId )
    {
    if (aAppView)
        {
        // Phonebook 2 view explorer for view switching
        MPbk2ViewExplorer* viewExplorer = Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer();

        __ASSERT_DEBUG( viewExplorer,
            Panic( EPanicPreCond_CmdOpenSettingsViewL ) );

        CPbk2ViewState* state = aAppView->ViewStateLC();
        viewExplorer->ActivatePhonebook2ViewL
            ( aViewId, state );
        CleanupStack::PopAndDestroy( state );
        aControl.UpdateAfterCommandExecution();
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::CmdOpenPreviousViewL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::CmdOpenPreviousViewL
        ( MPbk2ContactUiControl& /*aControl*/,
          const CPbk2AppViewBase* aAppView )
    {
    if (aAppView)
        {
        // Phonebook 2 view explorer for view switching
        MPbk2ViewExplorer* viewExplorer = Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer();

        CPbk2ViewState* state = aAppView->ViewStateLC();
        viewExplorer->ActivatePreviousViewL( state );
        CleanupStack::PopAndDestroy( state );
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::CmdOpenHelpL
// --------------------------------------------------------------------------
//
void CPbk2CommandHandler::CmdOpenHelpL
        ( MPbk2ContactUiControl& aControl,
          const CPbk2AppViewBase* aAppView )
    {
    if ( aAppView )
        {
    TCoeHelpContext helpContext;
    if ( iExtensionManager->GetHelpContextL
            ( helpContext, *aAppView, aControl) )
        {
        CArrayFix<TCoeHelpContext>* helpContextArray =
                new (ELeave) CArrayFixFlat<TCoeHelpContext>( 1 );
        CleanupStack::PushL( helpContextArray );
        helpContextArray->AppendL( helpContext );

        // HlpLauncher destroys the help context array
        CleanupStack::Pop( helpContextArray );
        HlpLauncher::LaunchHelpApplicationL
            ( CCoeEnv::Static()->WsSession(), helpContextArray );
        }
    else
        {
        HlpLauncher::LaunchHelpApplicationL
            ( CCoeEnv::Static()->WsSession(),
              CEikonEnv::Static()->EikAppUi()->AppHelpContextL() );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::CmdGenericCommandL
// --------------------------------------------------------------------------
//
TBool CPbk2CommandHandler::CmdGenericCommandL(
        TPbk2CommandId aCommandId,
        MPbk2ContactUiControl& aControl)
    {
    // Create the command object
    TBool result = EFalse;
    MPbk2Command* cmd = iCommandFactory->CreateCommandForIdL(
            aCommandId, aControl);
    if (cmd)
        {
        // If command factory was able to create the command,
        // add it to the command store and execute it
        iCommandStore->AddAndExecuteCommandL(cmd);
        result = ETrue;
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::PerformStandardMenuFilteringL
// --------------------------------------------------------------------------
//
inline void CPbk2CommandHandler::PerformStandardMenuFilteringL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane,
        MPbk2ContactUiControl& aControl )
    {
    switch (aResourceId)
        {
        case R_PHONEBOOK2_NAMESLIST_DELETE_MENU:
            {
            if ( aControl.NumberOfContacts() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdDeleteMe, ETrue );
                }
            break;
            }
        
        case R_PHONEBOOK2_NAMELIST_CALL_CONTACT_MENU:
            {
            // Weed out commands not meant to be used with marked items
            if ( aControl.ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdCall, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_NAMESLIST_CREATE_MENU:
            {
            // Weed out commands not meant to be used with marked items
            if ( aControl.ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdCreateNew, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_NAMESLIST_SEND_URL_MENU:
            {
            // Weed out commands not meant to be used with empty list
            if ( aControl.NumberOfContacts() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdGoToURL, ETrue );
                }

            if ( aControl.ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdGoToURL, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_NAMESLIST_COPY_MENU:          // FALLTHROUGH
        case R_PHONEBOOK2_NAMESLIST_COPY_CONTEXT_MENU:
            {
            // The copy menu is not shown if there are no contacts
            TBool itemSpecCommEnabled = Phonebook2::Pbk2AppUi()->ActiveView()->MenuBar()->ItemSpecificCommandsEnabled();
            if ( aControl.NumberOfContacts() == 0 || 
                    ( !aControl.ContactsMarked() && !itemSpecCommEnabled ) ||
                    ( !aControl.FocusedContactL() && itemSpecCommEnabled && 
                            !aControl.ContactsMarked() ) )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdCopy, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_EDIT_MENU:
            {
            // Weed out commands not meant to be used with empty contact
            if ( aControl.NumberOfContactFields() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdWrite, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdGoToURL, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdDefaultSettings, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_EXTENDED_MENU:
            {
            CVPbkContactManager& manager = iAppServices->ContactManager();

            // Always dim thumbnail if image/text supported or vice versa
            if ( FeatureManager::FeatureSupported
                    ( KFeatureIdCallImagetext ) )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdFetchThumbnail, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdRemoveThumbnail, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EPbk2CmdAddImage, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdImage, ETrue );
                }

            // Weed out commands not meant to be used with empty contact
            if ( aControl.NumberOfContactFields() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdSend, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdAssignSpeedDial, ETrue);
                aMenuPane->SetItemDimmed( EPbk2CmdRemoveSpeedDial, ETrue );
                aMenuPane->SetItemDimmed
                    ( EPbk2CmdPersonalRingingTone, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdCopy, ETrue );

                if ( !FeatureManager::FeatureSupported
                        ( KFeatureIdCallImagetext ) )
                    {
                    aMenuPane->SetItemDimmed
                        ( EPbk2CmdFetchThumbnail, ETrue );
                    aMenuPane->SetItemDimmed
                        ( EPbk2CmdRemoveThumbnail, ETrue );
                    }
                else
                    {
                    aMenuPane->SetItemDimmed( EPbk2CmdAddImage, ETrue );
                    aMenuPane->SetItemDimmed( EPbk2CmdImage, ETrue );
                    }
                }
            else
                {
                // Utilise attribute manager to find out if
                // the contact have a speed dial defined
                TBool hasSpeedDial = HasSpeedDialL( aControl );

                // Filtering is based on the speed dial existence
                if ( !hasSpeedDial )
                    {
                    aMenuPane->SetItemDimmed
                        ( EPbk2CmdRemoveSpeedDial, ETrue );
                    }
                else
                    {
                    aMenuPane->SetItemDimmed
                        ( EPbk2CmdAssignSpeedDial, ETrue );
                    }
                }
            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_CONTEXT_MENU:
            {
            // Weed out commands not meant to be used with empty contact
            if ( aControl.NumberOfContactFields() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdWrite, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdGoToURL, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdPersonalRingingTone, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdUseNumber, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdImage, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_INFO_MENU:
            {
            // Filter memory info items when list box is empty
            // or items are marked
            if ( aControl.NumberOfContacts() == 0 ||
                 aControl.ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdPhonebook2Info, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_SYSTEM_MENU:
            {
            if ( !FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                // Remove non-supported help from menu
                aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
                }
            break;
            }
        case R_PHONEBOOK2_MERGE_CONTACTS:
            {
            TBool supported = FeatureManager::FeatureSupported( KFeatureIdFfContactsMerge );
                       
            if ( !supported || aControl.ContactsMarked() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdMergeContacts, ETrue );
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
// CPbk2CommandHandler::PerformStoreSpecificFiltering
// --------------------------------------------------------------------------
//
inline void CPbk2CommandHandler::PerformStoreSpecificFilteringL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane,
            MPbk2ContactUiControl& aControl )
    {
    switch (aResourceId)
        {
        case R_PHONEBOOK2_NAMESLIST_CONTEXT_MENU_MARKED_ITEMS: // FALLTHROUGH
        case R_PHONEBOOK2_NAMESLIST_DELETE_MENU:
            {
            // Weed out commands not meant to be used with read only stores
            if (AreSelectedContactsFromReadOnlyStoreL( aControl ))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdDeleteMe, ETrue);
                }
            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_EDIT_MENU:
            {
            TBool phoneMemoryInConfiguration =
                    iContactRelocator->IsPhoneMemoryInConfigurationL();

            // Weed out commands not meant to be used with read only stores
            if (AreSelectedContactsFromReadOnlyStoreL( aControl ))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdDeleteMe, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdEditMe, ETrue);
                }

            // Weed out phone memory specific commands, if there is no
            // phone memory in configuration
            if (!phoneMemoryInConfiguration)
                {
                aMenuPane->SetItemDimmed(EPbk2CmdDefaultSettings, ETrue);
                }
            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_EXTENDED_MENU:
            {
            TBool phoneMemoryInConfiguration =
                    iContactRelocator->IsPhoneMemoryInConfigurationL();

            // Weed out phone memory specific commands, if there is no
            // phone memory in configuration
            if (!phoneMemoryInConfiguration)
                {
                aMenuPane->SetItemDimmed(EPbk2CmdAssignSpeedDial, ETrue);
                if (!FeatureManager::FeatureSupported(KFeatureIdCallImagetext))
                    {
                    aMenuPane->SetItemDimmed(EPbk2CmdFetchThumbnail, ETrue);
                    }
                else
                    {
                    aMenuPane->SetItemDimmed(EPbk2CmdAddImage, ETrue);
                    aMenuPane->SetItemDimmed(EPbk2CmdImage, ETrue);
                    }
                aMenuPane->SetItemDimmed(EPbk2CmdPersonalRingingTone, ETrue);
                }

            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_CONTEXT_MENU:
            {
            // Weed out commands not meant to be used with read only stores
            if (AreSelectedContactsFromReadOnlyStoreL( aControl ))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdEditMe, ETrue);
                }
            break;
            }
        case R_PHONEBOOK2_MERGE_CONTACTS:
            {
            TBool phoneMemoryInConfiguration =
                    iContactRelocator->IsPhoneMemoryInConfigurationL();
            
            if( !phoneMemoryInConfiguration ) 
                {
                aMenuPane->SetItemDimmed( EPbk2CmdMergeContacts, ETrue );
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
// CPbk2CommandHandler::PerformFieldTypeBasedFilteringL
// --------------------------------------------------------------------------
//
inline void CPbk2CommandHandler::PerformFieldTypeBasedFilteringL(
            TInt aResourceId,
            CEikMenuPane* aMenuPane,
            MPbk2ContactUiControl& aControl )
    {
    CVPbkContactManager& manager = iAppServices->ContactManager();

    switch (aResourceId)
        {
        case R_PHONEBOOK2_CONTACTINFO_EDIT_MENU:
            {
            // Weed out commands which are not shown if currently focused field is
            // not phone number
            if (!FocusedFieldMatchesFieldTypeL(aControl,
                    R_PHONEBOOK2_PHONENUMBER_SELECTOR, manager))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue);
                }

            // If the contact does not have URL field, there is no
            // "Go To URL" option available
            if (FocusedContactHasFieldTypeL(aControl,
                    R_PHONEBOOK2_URL_SELECTOR, manager) < 0)
                {
                aMenuPane->SetItemDimmed(EPbk2CmdGoToURL, ETrue);
                }
            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_EXTENDED_MENU:
            {
            // Speed dial menus are variated based on VoIP support
            if( FeatureManager::FeatureSupported( KFeatureIdCommonVoip ) )
                {
                // Weed out commands which are not shown if currently focused field is
                // neither phone number nor VoIP field
                if( !FocusedFieldMatchesFieldTypeL( aControl,
                        R_PHONEBOOK2_SIP_MSISDN_SELECTOR, manager ) )
                    {
                    aMenuPane->SetItemDimmed( EPbk2CmdAssignSpeedDial, ETrue );
                    aMenuPane->SetItemDimmed( EPbk2CmdRemoveSpeedDial, ETrue );
                    }
                }
            else
                {
                // Weed out commands which are not shown if currently focused field is
                // not phone number
                if( !FocusedFieldMatchesFieldTypeL( aControl,
                        R_PHONEBOOK2_PHONENUMBER_SELECTOR, manager ) )
                    {
                    aMenuPane->SetItemDimmed( EPbk2CmdAssignSpeedDial, ETrue );
                    aMenuPane->SetItemDimmed( EPbk2CmdRemoveSpeedDial, ETrue );
                    }
                }

            // Alternate Thumbnail/Call object menus based on whether data
            // has been added or not
            if (!FeatureManager::FeatureSupported(KFeatureIdCallImagetext))
                {
                // Weed out command Remove Thumbnail if contact does not have one
                if (FocusedContactHasFieldTypeL(aControl,
                        R_PHONEBOOK2_THUMBNAIL_SELECTOR, manager) < 0)
                    {
                    aMenuPane->SetItemDimmed(EPbk2CmdRemoveThumbnail, ETrue);
                    }
                }
            else
                {
                if ((FocusedContactHasFieldTypeL(aControl,
                        R_PHONEBOOK2_IMAGE_SELECTOR, manager) < 0) &&
                    (FocusedContactHasFieldTypeL(aControl,
                        R_PHONEBOOK2_THUMBNAIL_SELECTOR, manager) < 0))
                    {
                    // Weed out Image submenu if contact does not have image
                    aMenuPane->SetItemDimmed(EPbk2CmdImage, ETrue);
                    }
                else
                    {
                    // Weed out command Add Image if contact already has one
                    aMenuPane->SetItemDimmed(EPbk2CmdAddImage, ETrue);
                    }
                }
            break;
            }

        case R_PHONEBOOK2_CONTACTINFO_CONTEXT_MENU:
            {
            TBool filtered = EFalse;

            // Telephone field
            if (FocusedFieldMatchesFieldTypeL(aControl,
                    R_PHONEBOOK2_PHONENUMBER_SELECTOR, manager))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdGoToURL, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdPersonalRingingTone, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdImage, ETrue);
                filtered = ETrue;

                // Further filtering of phone number fields
                // Pager field
                // Pager returns a match for R_PHONEBOOK2_PHONENUMBER_SELECTOR
                // too, but it requires further filtering, which is handled here
                if (FocusedFieldMatchesFieldTypeL(aControl,
                        R_PHONEBOOK2_PAGER_SELECTOR, manager))
                    {
                    aMenuPane->SetItemDimmed(EPbk2CmdWrite, ETrue);
                    aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue );
                    }

                // VoIP
                // VoIP returns a match for R_PHONEBOOK2_PHONENUMBER_SELECTOR
                // too, but it requires further filtering, which is handled here
                if (FocusedFieldMatchesFieldTypeL(aControl,
                        R_PHONEBOOK2_VOIP_SELECTOR, manager))
                    {
                    aMenuPane->SetItemDimmed(EPbk2CmdWrite, ETrue);
                    aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue );
                    }
                } // further filtering

            // Email field
            if (FocusedFieldMatchesFieldTypeL(aControl,
                    R_PHONEBOOK2_EMAIL_SELECTOR, manager))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdGoToURL, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue );
                aMenuPane->SetItemDimmed(EPbk2CmdPersonalRingingTone, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdImage, ETrue);
                filtered = ETrue;
                }

            // URL field
            if (FocusedFieldMatchesFieldTypeL(aControl,
                    R_PHONEBOOK2_URL_SELECTOR, manager))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdWrite, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue );
                aMenuPane->SetItemDimmed(EPbk2CmdPersonalRingingTone, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdImage, ETrue);
                filtered = ETrue;
                }

            // Ringing tone field
            if (FocusedFieldMatchesFieldTypeL(aControl,
                    R_PHONEBOOK2_RINGTONE_SELECTOR, manager))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdWrite, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdGoToURL, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdEditMe, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue );
                aMenuPane->SetItemDimmed(EPbk2CmdImage, ETrue);
                filtered = ETrue;
                }

            // Image/text context submenus
            if( FocusedFieldMatchesFieldTypeL(aControl,
                    R_PHONEBOOK2_IMAGE_SELECTOR, manager))
                {
                aMenuPane->SetItemDimmed(EPbk2CmdWrite, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdGoToURL, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue );
                aMenuPane->SetItemDimmed(EPbk2CmdPersonalRingingTone, ETrue);
                filtered = ETrue;
                }

            if (!filtered)
                {
                // Default filtering leaves only edit option in the menu
                aMenuPane->SetItemDimmed(EPbk2CmdWrite, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdGoToURL, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdUseNumber, ETrue );
                aMenuPane->SetItemDimmed(EPbk2CmdPersonalRingingTone, ETrue);
                aMenuPane->SetItemDimmed(EPbk2CmdImage, ETrue);
                }
            break;
            }

        case R_PHONEBOOK2_IMAGE_SUBMENU:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdCallImagetext))
                {
                if (FocusedContactHasFieldTypeL(aControl,
                        R_PHONEBOOK2_IMAGE_SELECTOR, manager) < 0)
                    {
                    // Weed out Image View command if contact has no image
                    // (it might have thumbnail from e.g. business card and
                    // and thus other image submenu options)
                    aMenuPane->SetItemDimmed(EPbk2CmdViewImage, ETrue);
                    }
                }
            break;
            }

        default:
            {
            // AIW provider menus are filtered in the corresponding
            // interest item, for example R_PHONEBOOK2_CALL_CONTEXT_MENU
            // is filtered in CPbk2AiwInterestItemCall
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::AreSelectedContactsFromReadOnlyStoreL
// Returns ETrue if all selected contacts are read-only.
// --------------------------------------------------------------------------
//
inline TBool CPbk2CommandHandler::AreSelectedContactsFromReadOnlyStoreL(
            MPbk2ContactUiControl& aControl)
    {
    // Default behavior is that selection is read-only, e.g
    // a link can not be created for folding contact in which case
    // the folding contact cannot be destroyed
    TBool ret = ETrue;

    MPbk2ContactLinkIterator* iterator =
        aControl.SelectedContactsIteratorL();
    if ( iterator )
        {
        CleanupDeletePushL( iterator );

        while( iterator->HasNext() && ret )
            {
            MVPbkContactLink* link = iterator->NextL();

            if ( link && !IsFromReadOnlyStore( *link ) )
                {
                ret = EFalse;
                }

            delete link;
            link = NULL;
            }

        CleanupStack::PopAndDestroy(); // iterator
        }
    else
        {
        // There was no iterator so there are no selected contacts,
        // this means we are operating with the focused contact
        const MVPbkBaseContact* focusedContact =
            aControl.FocusedContactL();
        if ( focusedContact )
            {
            MVPbkContactLink* link = focusedContact->CreateLinkLC();
            if ( link && !IsFromReadOnlyStore( *link ) )
                {
                ret = EFalse;
                }
            CleanupStack::PopAndDestroy(); // link
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::IsFromReadOnlyStore
// --------------------------------------------------------------------------
//
TBool CPbk2CommandHandler::IsFromReadOnlyStore(
        const MVPbkContactLink& aContactLink ) const
    {
    TBool ret = EFalse;

    const MVPbkContactStore& store = aContactLink.ContactStore();
    if ( store.StoreProperties().ReadOnly() )
        {
        ret = ETrue;
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::HasSpeedDialL
// --------------------------------------------------------------------------
//
TBool CPbk2CommandHandler::HasSpeedDialL(
        MPbk2ContactUiControl& aControl )
    {
    TBool hasAttribute(EFalse);

    const MVPbkStoreContact* storeContact =
            aControl.FocusedStoreContact();

    // Convert presentation index to store index.
    TInt index( ToStoreFieldIndexL(
                    aControl.FocusedFieldIndex(),
                    storeContact ) );

    if ( index != KErrNotFound )
        {
        const MVPbkStoreContactField& field =
            storeContact->Fields().FieldAt( index );


        hasAttribute =
            iAppServices->ContactManager().ContactAttributeManagerL().
            HasFieldAttributeL(
                CVPbkSpeedDialAttribute::Uid(),
                field );
        }
    return hasAttribute;
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::ToStoreFieldIndexL
// --------------------------------------------------------------------------
//
TInt CPbk2CommandHandler::ToStoreFieldIndexL(
        TInt aPresIndex,
        const MVPbkStoreContact* aStoreContact )
    {
    // It is ok to remove const, we don't make any changes
    MVPbkStoreContact* storeContact =
        const_cast<MVPbkStoreContact*>( aStoreContact );

    TInt index( KErrNotFound );
    if ( aPresIndex >= 0 )
        {
        CPbk2PresentationContact* presentationContact =
            CPbk2PresentationContact::NewL(
                *storeContact,
                iAppServices->FieldProperties() );
        CleanupStack::PushL( presentationContact );

        index = presentationContact->PresentationFields().
                    StoreIndexOfField( aPresIndex );

        CleanupStack::PopAndDestroy( presentationContact );
        }
    return index;
    }

// --------------------------------------------------------------------------
// CPbk2CommandHandler::CommandStore
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2CommandStore* CPbk2CommandHandler::CommandStore()
    {
    return iCommandStore;
    }

// End of File
