/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Phonebook 2 names list extension view.
*
*/


// INCLUDE FILES
#include "Pbk2NamesListExView.h"
#include "CPbk2CmdItemAddTop.h"
#include "CPbk2CmdItemRemoteContactLookup.h"
#include <cpbk2remotecontactlookupaccounts.h>
#include <Pbk2ExNamesListRes.rsg>
#include "Pbk2NlxMenuFiltering.h"
#include "cpbk2cmditemmycard.h"
#include "cpbk2mycard.h"
#include "Pbk2NamesListEx.hrh"
#include "cpbk2contextlaunch.h"

#include <spbcontentprovider.h>

// Phonebook 2
#include <CPbk2UIExtensionView.h>
#include <MPbk2AppUi.h>
#include <MPbk2ViewExplorer.h>
#include "CPbk2ViewState.h"
#include <MPbk2StartupMonitor.h>
#include <Phonebook2.rsg>
#include <Pbk2Commands.rsg>
#include <Pbk2UIControls.rsg>
#include <Pbk2CommonUi.rsg>
#include <CPbk2ControlContainer.h>
#include <CPbk2NamesListControl.h>
#include <MPbk2CommandHandler.h>
#include <Pbk2Commands.hrh>
#include <MPbk2ViewActivationTransaction.h>
#include <CPbk2ViewStateTransformer.h>
#include <csxhelp/phob.hlp.hrh>
#include <Pbk2UID.h>
#include <MPbk2PointerEventInspector.h>
#include <Phonebook2PrivateCRKeys.h>
#include <CPbk2IconFactory.h>
#include <TPbk2IconId.h>
#include <CPbk2IconInfoContainer.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>
#include <CPbk2StoreConfiguration.h>
#include <Pbk2IconId.hrh>
#include "Pbk2ContactUiControlProxy.h"
#include <MPbk2UiControlCmdItem.h>
#include <pbk2mapcommands.hrh>
#include "Pbk2InternalCommands.hrh"
#include "CPbk2ContactViewListBox.h"

#include "CVPbkContactManager.h"
#include "MVPbkContactStoreList.h"
#include "VPbkContactStoreUris.h"
#include "TVPbkContactStoreUriPtr.h"

//Cca
#ifdef RD_PBK2_USE_CCA
    #include <MVPbkBaseContact.h>
    #include <mccaparameter.h>
    #include <mccaconnection.h>
    #include <ccafactory.h>
//    #include <ccaerrors.h>
#endif
#include <ccafactory.h>
#include <mccaconnection.h>

// System includes
#include <StringLoader.h>
#include <centralrepository.h>
#include <AiwCommon.hrh>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <eikmenub.h>
#include <AknUtils.h>
#include <akntoolbar.h>
#include <akntouchpane.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <aknbutton.h>
#include <eikcolib.h>
#include <eiklbx.h>
#include <touchfeedback.h>
#include <aknstyluspopupmenu.h>
#include <Pbk2Config.hrh>
#include <layoutmetadata.cdl.h>
#include <featmgr.h>
#include <ecom.h>

// Debugging headers
#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

//Phone memory checking
#include <CPbk2ApplicationServices.h>
#include <CVPbkContactStoreUriArray.h>
#include <CPbk2StoreConfiguration.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ConstructL = 1,
    EPanicPostCond_ConstructL,
    EPanicPostCond_DoActivateL,
    EPanicPreCond_HandleCommandL,
    EPanicLogic_DoTransformLegacyViewStateToPbk2ViewStateLC
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2NamesListExView" );
    User::Panic( KPanicText, aReason );
    }
#endif // _DEBUG


/**
 * Returns ETrue if shift is depressed in aKeyEvent.
 *
 * @param aKeyEvent Key event.
 * @return  ETrue if shift is down.
 */
inline TBool ShiftDown
        ( const TKeyEvent& aKeyEvent )
    {
    return ( aKeyEvent.iModifiers &
        ( EModifierShift | EModifierLeftShift | EModifierRightShift ) ) != 0;
    }

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

/**
 * Creates a toolbar button with a skinnable icon
 *
 * @param aIconFactory  Icon factory that must contain the toolbar
 *                      button icon.
 * @param aPbk2IconId   Id for Phonebook2 icon for all button states.
 * @return  CAknButton instance.
 */
CAknButton* CreateToolbarButtonLC
        ( CPbk2IconFactory& aIconFactory, TInt aPbk2IconId, TInt aHelpTextResId )
    {
    TPbk2IconId iconId( TUid::Uid(KPbk2UID3), aPbk2IconId );
    CGulIcon* icon = aIconFactory.CreateIconLC( iconId );

    HBufC* helpText = CCoeEnv::Static()->AllocReadResourceLC( aHelpTextResId );

    CAknButton* button = CAknButton::NewL( icon, NULL, NULL, NULL,
        KNullDesC, *helpText, KAknButtonSizeFitText, 0 );

    CleanupStack::PopAndDestroy();//helpText

    CleanupStack::Pop( icon );
    CleanupStack::PushL( button );
    return button;
    }

}  /// namespace

// --------------------------------------------------------------------------
// CPbk2NamesListExView::CPbk2NamesListExView
// --------------------------------------------------------------------------
//
CPbk2NamesListExView::CPbk2NamesListExView
        ( CPbk2UIExtensionView& aView,
          CSpbContentProvider& aStatusProvider,
          MCCAConnection*& aCCAConnection ) :
        iView( aView ),
        iCommandState( ETrue ),
        iCurrentCommandId( KErrNotFound ),
        iCtrlVisibleStateBeforeLosingForground( ETrue ),
        iFirstTimeActivated( ETrue ),
        iContentProvider( aStatusProvider ),
        iCCAConnection( aCCAConnection )
    {
    }

// --------------------------------------------------------------------------
// CPbk2NamesListAppView::~CPbk2NamesListAppView
// --------------------------------------------------------------------------
//
CPbk2NamesListExView::~CPbk2NamesListExView()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListExView(%x)::~CPbk2NamesListExView()"), this);

    if ( iStoreConfiguration )
        {
        iStoreConfiguration->RemoveObserver( *this );
        }

    delete iControlState;
    delete iIdleToolbarWarden;
    delete iToolbarIcons;
    if ( iToolbar )
        {
        iToolbar->SetToolbarObserver( NULL );
        }

    if (iContainer)
        {
        iContainer->RemoveForegroundObserver();
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        }

    delete iIdleTexter;
    delete iControlProxy;

    delete iMyCard;
    CCoeEnv::Static()->RemoveForegroundObserver( *this );

    delete iContextLauncher;
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::NewL
// --------------------------------------------------------------------------
//
CPbk2NamesListExView* CPbk2NamesListExView::NewL
        ( CPbk2UIExtensionView& aView,
          CSpbContentProvider& aStatusProvider,
          MCCAConnection*& aCCAConnection )
    {
    CPbk2NamesListExView* self = new ( ELeave ) CPbk2NamesListExView
        ( aView, aStatusProvider, aCCAConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2NamesListExView::ConstructL()
    {
    FeatureManager::InitializeLibL();

    // PreCond
    __ASSERT_DEBUG(!iContainer, Panic(EPanicPreCond_ConstructL));
    PBK2_PROFILE_START(Pbk2Profile::ENamesListExViewConstruction);
    CCoeEnv::Static()->AddForegroundObserverL(*this);

    iContactManager = &(Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager());
    iNameFormatter = &(Phonebook2::Pbk2AppUi()->ApplicationServices().NameFormatter());

    PBK2_PROFILE_START(Pbk2Profile::ENamesListGetCommandHandler);
    iCommandHandler = Phonebook2::Pbk2AppUi()->ApplicationServices().CommandHandlerL();
    PBK2_PROFILE_END(Pbk2Profile::ENamesListGetCommandHandler);

    // Register this view as a store configuration observer
    PBK2_PROFILE_START(Pbk2Profile::ENamesListRegisterStoreConfiguration);
    iStoreConfiguration = &Phonebook2::Pbk2AppUi()->ApplicationServices().StoreConfiguration();
    iStoreConfiguration->AddObserverL( *this );
    PBK2_PROFILE_END(Pbk2Profile::ENamesListRegisterStoreConfiguration);
    
    iControlProxy = new( ELeave )CPbk2ContactUiControlProxy;

    iContextLauncher = new(ELeave) CPbk2ContextLaunch( iCCAConnection );

    PBK2_PROFILE_END(Pbk2Profile::ENamesListPenSupportConstruction);

    if( FeatureManager::FeatureSupported( KFeatureIdffContactsMycard ) )
        {
        // Get the phoneStore for MyCard
        MVPbkContactStore* phoneStore = iContactManager->ContactStoresL()
            .Find(VPbkContactStoreUris::DefaultCntDbUri());
    
        if(phoneStore)
            {
            iMyCard = CPbk2MyCard::NewL( *phoneStore );
            }
        }
    
    // Attach AIW providers
    AttachAIWProvidersL();

    // Do not create the container control here: defer creation to first
    // activation of the view (first DoActivateL call). This saves a lot
    // of memory and application starting time, because all the application
    // views (classes like this) must be created at startup. Controls will
    // be created on-demand as the views are accessed.

    PBK2_PROFILE_END(Pbk2Profile::ENamesListExViewConstruction);
    // PostCond
    __ASSERT_DEBUG(!iContainer, Panic(EPanicPostCond_ConstructL));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::ConstructToolbarL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::ConstructToolbarL()
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        iToolbar = iView.Toolbar();
        if ( iToolbar )
            {
            // Take toolbar control event handling from touch pane.
            // Events need to be forwarded to touch pane manually.
            iToolbar->SetObserver( this );
            iToolbar->SetToolbarObserver( this );

            // Add toolbar buttons with skinnable icons
            iToolbarIcons = CPbk2IconInfoContainer::NewL(
                R_PBK2_NAMESLIST_TOOLBAR_ICON_INFO_ARRAY );
            CPbk2IconFactory* factory =
                CPbk2IconFactory::NewLC( *iToolbarIcons );

            // Button 1: Call
            CAknButton* button = CreateToolbarButtonLC(
                *factory, EPbk2qgn_indi_tb_call, R_PHOB_TB_CALL );
            iToolbar->AddItemL(
                button, EAknCtButton, EPbk2CmdCall, 0, 0 );
            CleanupStack::Pop( button );

            // Button 2: Write message
            button = CreateToolbarButtonLC(
                *factory, EPbk2qgn_indi_tb_mce, R_PHOB_TB_CREATE_MSG );
            iToolbar->AddItemL(
                button, EAknCtButton, EPbk2CmdWriteNoQuery, 0, 1 );
            CleanupStack::Pop( button );

            // Button 3: Create new contact
            button = CreateToolbarButtonLC(
                *factory, EPbk2qgn_indi_tb_avail_contacts, R_PHOB_TB_NEW_CONTACT );
            iToolbar->AddItemL(
                button, EAknCtButton, EPbk2CmdCreateNew, 0, 2 );
            CleanupStack::Pop( button );

            CleanupStack::PopAndDestroy( factory );

            // Load the previously saved toolbar state
            CRepository* cenrep =
                CRepository::NewLC( TUid::Uid( KCRUidPhonebook ) );
            User::LeaveIfError(
                cenrep->Get( KPhonebookNamesListToolbarVisibility,
                             iToolbarVisible ) );
            CleanupStack::PopAndDestroy( cenrep );

            // FIXME: toolbar is now always on after fixed toolbar is taken to use.
            // so the toolbar visibility storing and handling code including the cenrep key
            // could probably be removed now
            iToolbar->SetToolbarVisibility(ETrue, EFalse);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleCommandL(TInt aCommandId)
    {
    // If this view has already been deactivated, just return.
    if ( Phonebook2::Pbk2AppUi()->ActiveView()->Id() != Phonebook2::KPbk2NamesListViewUid )
        {
        return;
        }

    // In case "open" is chosen and a command item is highlighted, need to remap
    // cmd ID from openCca to the correct cmd ID. For command items "select" is used.
    // Not very elegant. Another way would be to dynamically change the OK cmd assignment
    // all the time as UI state changes.
    if ( iControl->FocusedCommandItem() &&
        !iControl->ContactsMarked() &&
        (aCommandId == EPbk2CmdOpenCca || aCommandId == EAknSoftkeySelect ) )
        {
        aCommandId = iControl->FocusedCommandItem()->CommandId();
        }

    if ( iControl->FocusedCommandItem() &&
        !iControl->ContactsMarked() &&
        aCommandId == EPbk2CmdSelectRclCmdItem )
        {
        aCommandId = EPbk2CmdRcl;
        }    

    if ( EAknCmdHideInBackground != aCommandId )
        {
        iCurrentCommandId = aCommandId;
        }
    
    if ( EPbk2CmdOpenCca == aCommandId )
    	{
        iControl->SetOpeningCca( ETrue );
    	}

    // Set focus, keep current focus on the area of the client screen
    // when scoll to the bottom of Names list view.
    CPbk2ContactViewListBox* listbox = static_cast<CPbk2ContactViewListBox*>(iControl->ComponentControl(0));
    if ( EPbk2CmdCreateNew == aCommandId &&
            listbox->CurrentItemIndex() < listbox->TopItemIndex() &&
                listbox->BottomItemIndex() == listbox->Model()->NumberOfItems()-1 )
        {
        listbox->SetCurrentItemIndex( listbox->TopItemIndex() );
        }

    // No command handling in this class, forward to Commands
    if (!iCommandHandler->HandleCommandL( aCommandId, *iControlProxy, &iView ))
        {
        iControl->ProcessCommandL(aCommandId);
        // Command was not handled in Pbk2Commands, forward it to AppUi
        Phonebook2::Pbk2AppUi()->HandleCommandL(aCommandId);

        // Update CBAs, if command handler is consuming the command
        // postcommandexecution is updating CBAs
        UpdateCbasL();
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::Id
// --------------------------------------------------------------------------
//
TUid CPbk2NamesListExView::Id() const
    {
    return Phonebook2::KPbk2NamesListViewUid;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(iView.ClientRect());
        }
    if ( iToolbar && NeedToHideToolbar(iCurrentCommandId) )
        {
        // If Command is is executing (iCommandState:0)
        // Ordinal priority of iToolbar is set to ECoeWinPriorityLow.
        iToolbar->DrawableWindow()->SetOrdinalPosition
            ( 0, iCommandState ? ECoeWinPriorityNormal : ECoeWinPriorityLow );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::DoActivateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::DoActivateL
        ( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
         const TDesC8& aCustomMessage )
    {
    /// Notice that when "always on" feature is used this is called second
    /// time without calling DoDeactivate, when exit is done.
    /// This function must not do any duplicate resource allocation.

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListExView(%x)::DoActivateL()"), this);

    PBK2_PROFILE_START(Pbk2Profile::ENamesListViewDoActivateL);

    PBK2_PROFILE_START
        ( Pbk2Profile::ENamesListViewDoActivateAddMenuCommandObserver );
    // Do not add duplicate if DoActivateL is called due to Exit in
    // always on mode.
    iCommandHandler->RemoveMenuCommandObserver( *this );
    // Add this view to observ command events
    iCommandHandler->AddMenuCommandObserver(*this);
    PBK2_PROFILE_END
        ( Pbk2Profile::ENamesListViewDoActivateAddMenuCommandObserver );

    PBK2_PROFILE_START(Pbk2Profile::ENamesListViewDoActivateLoadTitle);
    HBufC* title = StringLoader::LoadLC(R_QTN_PHOB_TITLE);
    PBK2_PROFILE_END(Pbk2Profile::ENamesListViewDoActivateLoadTitle);

    PBK2_PROFILE_START
        ( Pbk2Profile::ENamesListViewDoActivateHandleViewActivation );
    MPbk2ViewActivationTransaction* viewActivationTransaction =
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
            ( Id(), aPrevViewId, title, NULL,
              Phonebook2::EUpdateNaviPane | Phonebook2::EUpdateTitlePane |
              Phonebook2::EUpdateContextPane );
    PBK2_PROFILE_END
        ( Pbk2Profile::ENamesListViewDoActivateHandleViewActivation );

    // Create control and activate it.
    PBK2_PROFILE_START(Pbk2Profile::ENamesListViewDoActivateActivateControl);
    CreateControlsL();
    iContainer->ActivateL();
    PBK2_PROFILE_END(Pbk2Profile::ENamesListViewDoActivateActivateControl);

    // Check activation parameters
    PBK2_PROFILE_START(Pbk2Profile::ENamesListViewDoActivateParamHandling);
    delete iParamState;
    iParamState = NULL;
    if( aCustomMessageId == CPbk2ContextLaunch::Uid() )
        {
        // Close up any open CCA instances
        if( iCCAConnection )
            {
            iCCAConnection->Close();
            iCCAConnection = NULL;
            }
        
        // Handle context launch request
        if( iContextLauncher )
            {
            iContextLauncher->HandleMessageL( aCustomMessage );
            }
        }
    else if (aCustomMessageId == CPbk2ViewState::Uid())
        {
        // Restore state when TPbk2ControlEvent::EReady comes.
        iParamState = CPbk2ViewState::NewL(aCustomMessage);
        }
    else if (aCustomMessageId == TUid::Uid(KPbkViewStateUid))
        {
        // Handle legacy view state
        CPbk2ViewStateTransformer* transformer =
            CPbk2ViewStateTransformer::NewLC( *iContactManager );
        // Restore state when TPbk2ControlEvent::EReady comes.
        iParamState = transformer->
            TransformLegacyViewStateToPbk2ViewStateLC
                ( aCustomMessage );
        CleanupStack::Pop(iParamState);
        CleanupStack::PopAndDestroy(transformer);
        }
    PBK2_PROFILE_END(Pbk2Profile::ENamesListViewDoActivateParamHandling);

    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(2,title); // viewActivationTransaction, title

    // The application is set here to background if application exit occurs
    PBK2_PROFILE_START
        ( Pbk2Profile::ENamesListViewDoActivateNotifyViewActivatation );

    // The application is set here to background if application exit occurs
    // When the device is powered on, Phonebook will start up automatically and
    // hide itself in the background. At the first time of names list view activation,
    // event is generated, the notification needs to be sent to Startup Monitor
    // so that making UI controls displaying properly.
    // At the first time of the activation, since all controls are brand new so
    // the iControl->Reset() needn’t to be called.
    if ( iParamState && iParamState->Flags() &
         CPbk2ViewState::ESendToBackground && !iFirstTimeActivated)
        {
        // AppUi will send the application to background.

        // Because the control is not destroyed it must be reset so that
        // when the view comes foreground again it will look like a fresh
        // and brand new control.
        iControl->Reset();
        }
    else if (Phonebook2::Pbk2AppUi()->Pbk2StartupMonitor())
        {
        // We are not going to background. Notify the start-up monitor that
        // will need to know the when the first view is activated.
        Phonebook2::Pbk2AppUi()->Pbk2StartupMonitor()->NotifyViewActivationL(Id());

        // Reset flag
        iFirstTimeActivated = EFalse;
        }
    PBK2_PROFILE_END
        ( Pbk2Profile::ENamesListViewDoActivateNotifyViewActivatation );

    PBK2_PROFILE_END(Pbk2Profile::ENamesListViewDoActivateL);

    CCoeControl* ctrl=iControl->ComponentControl(0);
    CEikListBox* listbox=static_cast <CEikListBox*> (ctrl);
    listbox->SetListBoxObserver( this );

    // iCtrlVisibleStateBeforeLosingForground records the names list control states before
    // losing foreground during a command execution. Set the iCtrlVisibleStateBeforeLosingForground
    // flag as ETrue as the control should be visible when active a view.
    iCtrlVisibleStateBeforeLosingForground = ETrue;

    // PostCond:
    __ASSERT_DEBUG(iContainer, Panic(EPanicPostCond_DoActivateL));
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::AddCommandItemsToContainerL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::AddCommandItemsToContainerL()
    {
    // get command item count
    TInt commandCount = iControl->CommandItemCount();
       
    // Check if there is need to create MyCard
    if( FeatureManager::FeatureSupported( KFeatureIdffContactsMycard ) &&
        ( !iMyCard && IsPhoneMemoryUsedL() ) )
    	{				
		// Get the phoneStore for MyCard
		MVPbkContactStore* phoneStore = iContactManager->ContactStoresL()
    		.Find(VPbkContactStoreUris::DefaultCntDbUri() );
		
		if( phoneStore )
			{
			// Create MyCard if not already exist and the phone memory selected
			iMyCard = CPbk2MyCard::NewL( *phoneStore );
			}
    	}
    
    // Add the MyCard item to the top of the list
    if( iMyCard && IsPhoneMemoryUsedL() )
        {
        CPbk2CmdItemMyCard* cmd = 
            CPbk2CmdItemMyCard::NewLC( *iMyCard, iContentProvider );
        iControl->AddCommandItemL( cmd, commandCount ); // ownership transferred
        CleanupStack::Pop( cmd );
        commandCount++;
        }

    CPbk2CmdItemAddTop* listCmd = CPbk2CmdItemAddTop::NewLC();
    // Add the command item to the bottom of the command list
    iControl->AddCommandItemL( listCmd, commandCount ); // ownership transferred
    CleanupStack::Pop( listCmd );
    commandCount++;
    

   // promotion item disabled until we can support command items at the bottom of the list
   // if ( IsRclOnL() )  
    if( 0 )
	    {
        CPbk2CmdItemRemoteContactLookup* rclCmd = CPbk2CmdItemRemoteContactLookup::NewLC();
        // Add the command item to the bottom of the command list
        iControl->AddCommandItemL( rclCmd, commandCount ); // ownership transferred
        CleanupStack::Pop( rclCmd );
        commandCount++;
	    }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::IsRclOnL
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListExView::IsRclOnL()
    {
    TBool ret(EFalse);

    if( FeatureManager::FeatureSupported( KFeatureIdFfContactsRemoteLookup ))
        {
        //1. Get default protocol account
        TPbkxRemoteContactLookupProtocolAccountId accountID =
        CPbk2RemoteContactLookupAccounts::DefaultAccountIdL();
        
        //2. Check that there really are implementation and account corresponding to 
        //repository's value
        // If the iProtocolUid is NULL, then a default account has not been set.
        if ( accountID.iProtocolUid.iUid )
            {
            CPbkxRemoteContactLookupProtocolAccount* account = NULL;
            // Returns NULL if account or plugin doesn't exist
            account = CPbk2RemoteContactLookupAccounts::GetAccountL( accountID );
            if ( account )
                {
                ret = ETrue;
                delete account;
                }
            }            
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::MyCard
// --------------------------------------------------------------------------
//
CPbk2MyCard* CPbk2NamesListExView::MyCard() const
    {			
	TBool phoneMemory = EFalse;
	
	TRAP_IGNORE( phoneMemory = IsPhoneMemoryUsedL() );
	
	if( phoneMemory )
		{
		return iMyCard;
		}
	
	return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::DoDeactivate()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2NamesListExView(%x)::DoDeactivate()"), this);
    
    // CCA Connection should be closed when Name List View deactivated
    if( iCCAConnection )
        {
        iCCAConnection->Close();
        iCCAConnection = NULL;
        }
    
    if ( iCommandHandler )
        {
        iCommandHandler->RemoveMenuCommandObserver( *this );
        }

    // Cancel toolbar warden to prevent further state saving
    if( iIdleToolbarWarden )
        {
        iIdleToolbarWarden->Cancel();
        }

    // Disable focus of toobar, so that it can't steal it from other views
    if( iToolbar )
        {
        iToolbar->SetFocusing( EFalse );
        }

    if( iIdleTexter )
        {
        iIdleTexter->Cancel();
        }

    // Trash the old states
    delete iParamState;
    iParamState = NULL;
    delete iControlState;
    iControlState = NULL;
    
    if (iContainer)
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack(iContainer);
        // Store current state, safe to ignore. There's no real harm,
        // if theres no stored state when activating this view again
        TRAP_IGNORE(StoreStateL());
        iControl->SetObserver(NULL);
        delete iContainer;
        iContainer = NULL;
        // Reset also the reference to the control
        iControl = NULL;
        iControlProxy->SetControl( NULL );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    // Ask the control to do control specific filtering
    // (for example call HandleMarkableListDynInitMenuPane if needed)
    iControl->DynInitMenuPaneL(aResourceId, aMenuPane);

    // Phonebook 2 menu filtering happens in Commands
    iCommandHandler->DynInitMenuPaneL(
        aResourceId, aMenuPane, iView, *iControl);

    //This is called after CPbk2CommandHandler::PerformStandardMenuFilteringL
    //so we can safely remove here anything that may already also be removed
    //there.
    switch (aResourceId)
        {
        case R_PHONEBOOK2_NAMESLIST_SEND_URL_MENU:
            {
            DimItem( aMenuPane, EPbk2CmdGoToURL );
            break;
            }
        case R_PHONEBOOK2_NAMELIST_CREATE_MESSAGE_MENU:
            {
            TInt pos;
            if ( aMenuPane->MenuItemExists( EPbk2CmdWriteNoQuery, pos ) )
                {
                aMenuPane->SetItemSpecific( EPbk2CmdWriteNoQuery, !iControl->ContactsMarked() );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListAppView::DimItem
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::DimItem( CEikMenuPane* aMenuPane, TInt aCmd )
    {
    TInt pos;
    if ( aMenuPane->MenuItemExists( aCmd, pos ) )
        {
        aMenuPane->SetItemDimmed( aCmd, ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2NamesListExView::ViewStateLC() const
    {
    CPbk2ViewState* state = NULL;
    if (iControl)
        {
        state = iControl->ControlStateL();
        }
    CleanupStack::PushL(state);
    return state;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListExView::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    // re-initialized the findbox focus if the application is back in the
    // foreground.
    if ( iNeedSetFocus  && iView.IsForeground() )
        {
        iNeedSetFocus = EFalse;
        iControl->FocusChanged( EDrawNow );
        }

    /* Uncomment this code if it is necessary to enable navigation hardware keys
     * on devices which have these keys (its Map Extension feature)
    if( aKeyEvent.iScanCode == EStdKeyApplicationD && aType == EEventKeyUp )
        {
        HandleCommandL( EPbk2ExtensionAssignFromMap );
        return ETrue;
        }

    if( aKeyEvent.iScanCode == EStdKeyApplicationE && aType == EEventKeyUp )
        {
        HandleCommandL( EPbk2ExtensionShowOnMap );
        return ETrue;
        }*/
    TBool itemSpecEnabled = iView.MenuBar()->ItemSpecificCommandsEnabled();
    
    TBool result = EFalse;
    if (aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyPhoneSend:
                {
                if ( itemSpecEnabled && iControl->NumberOfContacts() > 0 &&
                     !iControl->ContactsMarked() )
                    {
                    // Do not handle the call command if there is a dialog displayed
                    if ( !(CEikonEnv::Static()->EikAppUi()->IsDisplayingDialog()) )
                        {
                        // Use the EPbk2CmdCall id to identify that
                        // call is being launched with send key
                        HandleCommandL(EPbk2CmdCall);
                        }
                    result = ETrue;
                    }
                break;
                }
            case EKeyPoC:
                {
                // Use the EPbk2CmdPoC id to identify that
                // call is being launched with PoC key
                HandleCommandL( EPbk2CmdPoC );
                result = ETrue;
                break;
                }
            case EKeyBackspace:
                {
                if ( (itemSpecEnabled && (iControl->NumberOfContacts() > 0))
                    || iControl->ContactsMarked() )
                    {
                    if (iControl->FindTextL() == KNullDesC)
                        {
                        HandleCommandL(EPbk2CmdDeleteMe);
                        result = ETrue;
                        }
                    }
                break;
                }

            case EKeyEnter: // FALLTHROUGH
            case EKeyOK:
                {
                if ( !ShiftDown(aKeyEvent) ) // pure OK key
					{
					if ( iControl->ContactsMarked() && itemSpecEnabled )					
					    {
                        iView.LaunchPopupMenuL(
                            R_PHONEBOOK2_NAMESLIST_CONTEXT_MENUBAR_ITEMS_MARKED);
                        result = ETrue;
					    }
					else if ( iControl->NumberOfContacts() == 0 )
					    {
                        result = ETrue;
					    }
					}
                break;
                }
            default:
                {
                break;
                }
            }
        }

    // Update selection modifier key state
    iSelectionModifierUsed = IsShiftOrControlDown( aKeyEvent ) ||
        ( (aType == EEventKeyDown) && (aKeyEvent.iCode == EStdKeyHash) );

    return result;
    }


// ----------------------------------------------------------------------------
// CLogsBaseView::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleListBoxEventL(
    CEikListBox* aListBox,
    TListBoxEvent aEventType)
    {
    if( AknLayoutUtils::PenEnabled() )
        {
        switch ( aEventType )
            {
            case EEventItemSingleClicked:
            	{
                ShowContextMenuL();
                break;
                }
#if 0            	
            case EEventEmptyAreaClicked:  //An empty area of non-empty listbox was clicked 
            case EEventEmptyListClicked:  //An empty listbox was clicked                
                {
                ProcessEmptyAreaClickL();
                break;
                }     
#endif                

            case EEventItemClicked: //Happens after focus changed
            break;

            case EEventPanningStarted:
            case EEventFlickStarted:
            case EEventPanningStopped:
            case EEventFlickStopped:
                {
                iControl->HandleListBoxEventL( aListBox, aEventType );
                break;
                }
            case EEventEnterKeyPressed:
                {
                if ( iControl->FocusedCommandItem() &&
                    !iControl->ContactsMarked() )
                    {
                    HandleCommandL(iControl->FocusedCommandItem()->CommandId() );
                    }
                else if (iControl->NumberOfContacts() > 0 &&
                    !iControl->ContactsMarked())
                    {
                    // Select key is mapped to "Open Contact" command
                    HandleCommandL(EPbk2CmdOpenCca);
                    }
                else if (iControl->ContactsMarked())
            		{
                	iView.LaunchPopupMenuL(
                    	R_PHONEBOOK2_NAMESLIST_CONTEXT_MENUBAR_ITEMS_MARKED);
                    }
                break;
                }
            default:
               break;
            }
          }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandlePointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    // re-initialized the findbox focus when the application is back in the
    // foreground.
    if ( iNeedSetFocus )
        {
        iNeedSetFocus = EFalse;
        iControl->FocusChanged( EDrawNow );
        }

    if ( AknLayoutUtils::PenEnabled() )
        {
// Longtap detection is currently not needed here 
#if 0  
    // If at some point we need to enable longtap detection here, then need
    // to implement a way to prevent passing the event to longtap detector 
    // when a command item is tapped. Because command items should not have the 
    // longtap animation.
    if ( iContainer 
            && <tapped item is not a command item> // <-- implement this logic by any means 
            && !iControl->ContactsMarked() && iPointerEventInspector->FocusableItemPointed() )
        {
        iContainer->LongTapDetectorL().PointerEventL( aPointerEvent );
        }
#endif  
            
// ece has stylus menu for empty state, no need to initiate touch feedback
// context menu disabled as well
#if 0
        if ( iControl->NumberOfContacts()==0)
            {
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                       {
                       feedback->InstantFeedback( ETouchFeedbackBasic );
                       }
                }
            else if ( ( aPointerEvent.iType == TPointerEvent::EButton1Up ) && iCommandState)
                {
                ShowContextMenuL();
                }
            }
#endif
        }

    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::ShowContextMenuL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::ShowContextMenuL()
    {

    if ( iStylusPopupMenuLaunched )
         {
         // Absorb EButton1Up event if we already launched the stylus
         // popup menu
         iStylusPopupMenuLaunched = EFalse;
         }
    else if ( !iPointerEventInspector->SearchFieldPointed() &&
              !iSelectionModifierUsed )
        {
        if ( iControl->NumberOfContacts() > 0 || iControl->CommandItemCount() > 0) 
            {
            if ( iPointerEventInspector->FocusedItemPointed() )
                {
                if ( iControl->ContactsMarked() )
                    {
                    // Display marked items context menu
                    iView.LaunchPopupMenuL
                        ( R_PHONEBOOK2_NAMESLIST_CONTEXT_MENUBAR_ITEMS_MARKED );
                    }
                else
                    {
                    // cancel the long tap animation with pointer up event
                    TPointerEvent event;
                    event.iType = TPointerEvent::EButton1Up;
                    iContainer->LongTapDetectorL().PointerEventL( event );

                    // Open contact
                    HandleCommandL( EPbk2CmdOpenCca );
                    }
                }
            }
        else
            {
#ifndef ECE_DISABLE_CONTEXT_MENU
            // Display regular context menu
            iView.LaunchPopupMenuL
                ( R_PHONEBOOK2_NAMESLIST_CONTEXT_MENUBAR );
#endif
            }
        }
    }

#if 0
// --------------------------------------------------------------------------
// CPbk2NamesListExView::ProcessEmptyAreaClickL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::ProcessEmptyAreaClickL()
    {
    if ( iStylusPopupMenuLaunched )
         {
         // Absorb EButton1Up event if we already launched the stylus
         // popup menu
         iStylusPopupMenuLaunched = EFalse;
         }
    else if ( !iPointerEventInspector->SearchFieldPointed() &&
              !iSelectionModifierUsed )
        {
        if ( iControl->NumberOfContacts() == 0 ) 
            {
            MTouchFeedback* feedback = MTouchFeedback::Instance();
            if ( feedback )
                   {
                   feedback->InstantFeedback( ETouchFeedbackBasic );
                   }			
            HandleCommandL( EPbk2CmdCreateNew );
            }
        }    
    }
#endif

// --------------------------------------------------------------------------
// CPbk2NamesListExView::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListExView::GetViewSpecificMenuFilteringFlagsL() const
    {
    TInt flags(0);

    flags |= iControl->GetMenuFilteringFlagsL();

    return flags;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleControlEventL
        ( MPbk2ContactUiControl& /*aControl*/,
          const TPbk2ControlEvent& aEvent )
    {
    switch (aEvent.iEventType)
        {
        case TPbk2ControlEvent::EUnavailable: // FALLTHROUGH
        case TPbk2ControlEvent::EReady:
            {
            DynInitToolbarL( R_PBK2_NAMESLIST_TOOLBAR, iToolbar );

            // Restore control state
            RestoreStateL();
            if ( iControl )
                {
                iControl->ShowThumbnail();
                iControl->DrawDeferred();
                }
            // Restore control's state before updating cbas to get right state.
            UpdateCbasL();
            PBK2_PROFILE_END(Pbk2Profile::EFullStartup);
            break;
            }

        case TPbk2ControlEvent::EContactSetChanged:
            {
            UpdateCbasL();
            DynInitToolbarL( R_PBK2_NAMESLIST_TOOLBAR, iToolbar );
            iControl->ShowThumbnail();
            break;
            }

        case TPbk2ControlEvent::EContactSelected:   // FALLTHROUGH
        case TPbk2ControlEvent::EContactUnselected: // FALLTHROUGH
        case TPbk2ControlEvent::EContactUnselectedAll:
            {
            // Do not UpdateCbasL here because control handles itself
            // the MSK when it goes to selection mode.
            DynInitToolbarL( R_PBK2_NAMESLIST_TOOLBAR, iToolbar );
            UpdateCbasL();
            break;
            }
        case TPbk2ControlEvent::EControlLeavesSelectionMode:
            {
            // Update CBAs because the control doesn't know should
            // the MSK be Open or Context menu when control leaves
            // the selection mode.
            UpdateCbasL();
            break;
            }

        case TPbk2ControlEvent::EControlStateChanged:
            {
            UpdateCbasL();
            DynInitToolbarL( R_PBK2_NAMESLIST_TOOLBAR, iToolbar );
            UpdateListEmptyTextL( aEvent.iInt );
            iControl->DrawDeferred();
            break;
            }

        case TPbk2ControlEvent::EControlFocusChanged:
            {
            DynInitToolbarL( R_PBK2_NAMESLIST_TOOLBAR, iToolbar );
            UpdateCbasL();
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
// CPbk2NamesListExView::ConfigurationChanged
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::ConfigurationChanged()
    {
    MVPbkContactStore* phoneStore = NULL;
    
    Reset();
           
    if( iMyCard )
    	{
		return;
    	}
       
    if( FeatureManager::FeatureSupported( KFeatureIdffContactsMycard ) )
	   {    				
		// Get the phoneStore for MyCard
		TRAP_IGNORE( phoneStore = iContactManager->ContactStoresL().
			Find( VPbkContactStoreUris::DefaultCntDbUri() ) );    		
	   }
               
    if( !phoneStore )
    	{
        TRAP_IGNORE( iContactManager->LoadContactStoreL( 
                VPbkContactStoreUris::DefaultCntDbUri() ) );
    	}
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::ConfigurationChangedComplete
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::ConfigurationChangedComplete()
    {
    if ( iControl )
        {
        MVPbkContactViewBase* allContactsView = NULL;

        TRAPD( res, allContactsView = Phonebook2::Pbk2AppUi()->
            ApplicationServices().ViewSupplier().AllContactsViewL() );

        if ( res == KErrNone )
            {
            TRAP( res, iControl->SetViewL( *allContactsView ) );
            iControl->DrawNow();
            }

        if ( res != KErrNone )
            {
            CCoeEnv::Static()->HandleError( res );
            iControl->Reset();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::PreCommandExecutionL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::PreCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    iControl->AllowPointerEvents( EFalse );
    iCommandState = EFalse;

    if ( iControl )
        {
        // Note: PostCommandExecutionL() must match this if()
        if ( iToolbar && NeedToHideToolbar(iCurrentCommandId) )
            {
            iToolbarVisible = iToolbar->IsShown();

            // First hide the other components (FindBox etc.)
            // No need to hide listbox and findbox for EPbk2CmdWriteNoQuery as
            // it will go to Message view
            if ( iCurrentCommandId != EPbk2CmdWriteNoQuery )
                {
                iControl->MakeVisible( EFalse );
                }
            // Now hide toolbar items during the command execution...
            iToolbar->HideItemsAndDrawOnlyBackground( ETrue );

            // ..but don't save the state change
            iIdleToolbarWarden->Cancel();
            }

        iControl->HideThumbnail();
        iControl->MassUpdateSkipProgressNote( ETrue );        
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::PostCommandExecutionL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::PostCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    iCommandState = ETrue;

    if ( iControl )
        {
        iControl->MassUpdateSkipProgressNote( EFalse );

        // There are some special cases that the names list view would be hide in backgroud
        // before one command is executing, and then get foreground later after the command
        // finished, which would cause the control to be set invisible. So reset the flag
        // after a command is finished.
        iCtrlVisibleStateBeforeLosingForground = ETrue;

        if( EPbk2CmdOpenCca == iCurrentCommandId )
            {
            iControl->SetOpeningCca( EFalse );
            }
            
        iControl->AllowPointerEvents( ETrue );
        iControl->ShowThumbnail();

        TBool drawToolbar( EFalse );
        // Check the orientation of the screen.
        const TBool isPortrait( Layout_Meta_Data::IsLandscapeOrientation() ? EFalse : ETrue );
        if ( isPortrait )
            {
            // If there is a menu shown do not draw the tool bar. It will be drawn
            // once the dialog or menu disappears and an event is received from
            // the control environment
            CCoeAppUi* appUi = CCoeEnv::Static()->AppUi();

            if ( NeedToHideToolbar(iCurrentCommandId) &&
                appUi && !appUi->IsDisplayingMenuOrDialog() )
                {
                drawToolbar = ETrue;
                }
            }
        else
            {
            // The orientation is landscape
            drawToolbar = ETrue;
            }
        if ( iToolbar && drawToolbar )
            {
            // First show toolbar items
            iToolbar->HideItemsAndDrawOnlyBackground( EFalse );

            // Now show the other components (FindBox etc.)
            // No need set the flag as it is not hidden.
            if ( iCurrentCommandId != EPbk2CmdWriteNoQuery )
                {
                iControl->MakeVisible( ETrue );
                }
            // Without the below lines the Toolbar is drawn
            // above the Find box
            iContainer->SetRect(iView.ClientRect());
            iControl->DrawNow();
            }
        if ( EPbk2CmdCreateNew == iCurrentCommandId )
            {
            iControl->DrawDeferred();
            }
        }
    iCurrentCommandId = KErrNotFound;
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::DynInitToolbarL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::DynInitToolbarL
        ( TInt aResourceId, CAknToolbar* aToolbar )
    {
    if ( aToolbar )
        {
        if ( iControl )
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                    ("CPbk2NamesListExView::DynInitToolbarL(0x%x) enable everything"), this);
            // By default enable everything
            aToolbar->SetItemDimmed( EPbk2CmdCall, EFalse, ETrue );
            aToolbar->SetItemDimmed( EPbk2CmdWriteNoQuery, EFalse, ETrue );
            aToolbar->SetItemDimmed( EPbk2CmdCreateNew, EFalse, ETrue );

            // Forward to command handler
            iCommandHandler->DynInitToolbarL
                ( aResourceId, aToolbar, iView, *iControl );
            }
        else
            {
            PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
                    ("CPbk2NamesListExView::DynInitToolbarL(0x%x) Disable all buttons"), this);

            // Disable all buttons if control has not yet been created
            aToolbar->SetItemDimmed( EPbk2CmdCall, ETrue, ETrue );
            aToolbar->SetItemDimmed( EPbk2CmdWriteNoQuery, ETrue, ETrue );
            aToolbar->SetItemDimmed( EPbk2CmdCreateNew, ETrue, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::OfferToolbarEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::OfferToolbarEventL
        ( TInt aCommand )
    {
    iCurrentCommandId = aCommand;

    if ( iControl )
        {
        // Forward to command handler
        iCommandHandler->HandleCommandL( aCommand, *iControl, &iView );
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleControlEventL(
        CCoeControl* aControl,
        TCoeEvent aEventType )
    {
    if ( aControl == iToolbar )
        {
        // Forward control event to touch pane
        CEikAppUiFactory* factory = new(ELeave) CEikAppUiFactory;
        CleanupStack::PushL(factory);
        CAknTouchPane* pane = factory->TouchPane();
        CleanupStack::PopAndDestroy(factory);

        if ( pane )
            {
            pane->HandleControlEventL( aControl, aEventType );
            if (!aControl->IsVisible() && iControl )
                {
                iControl->DrawDeferred();
                }
            }

        if ( iControl )
            {
            // Forward event to view's control
            iControl->HandleControlEventL( aControl, aEventType );
            }

        if ( aEventType == EEventStateChanged )
            {
            // Save state from idle callback. This gives possibility
            // to prevent save if we are e.g. existing the view or just
            // disabling the toolbar temporarily.
            iIdleToolbarWarden->Cancel();
            iIdleToolbarWarden->Start( TCallBack(
                ( &CPbk2NamesListExView::IdleSaveToolbarStateL ), this ) );
            }
        }
    if ( aControl ==iControl &&
            aEventType == MCoeControlObserver::EEventStateChanged &&
            iCommandState )
        {
        // If the toolbar is not drawn for some reason, draw it here once
        // view's control sends an event after updating itself
        if ( iToolbar && !iToolbar->IsShown() )
            {
            // First show toolbar items
            iToolbar->HideItemsAndDrawOnlyBackground( EFalse );

            // Now show the other components (FindBox etc.)
            // No need set the flag as it is not hide.
            if ( iCurrentCommandId != EPbk2CmdWriteNoQuery )
                {
                iControl->MakeVisible( ETrue );
                }

            // Without the below lines the Toolbar is drawn
            // above the Find box
            iContainer->SetRect(iView.ClientRect());
            iControl->DrawNow();
            }
        }
    }

//---------------------------------------------------------------------------
// CPbk2NamesListExView::CreateControlsL
// --------------------------------------------------------------------------
//
inline void CPbk2NamesListExView::CreateControlsL()
    {
	
    if (!iContainer)
        {
        PBK2_PROFILE_START(Pbk2Profile::ENamesListViewCreateControls);
        // Create the container and control
        PBK2_PROFILE_START(Pbk2Profile::ENamesListViewCreateContainer);
        CContainer* container = CContainer::NewLC( &iView, &iView, iView );
        PBK2_PROFILE_END(Pbk2Profile::ENamesListViewCreateContainer);

        PBK2_PROFILE_START(Pbk2Profile::ENamesListViewSetHelpContext);
        container->SetHelpContext( TCoeHelpContext( iView.ApplicationUid(),
            KPHOB_HLP_NAME_LIST ));
        PBK2_PROFILE_END(Pbk2Profile::ENamesListViewSetHelpContext);

        PBK2_PROFILE_START
            ( Pbk2Profile::ENamesListViewCreateNamesListControl );
        CPbk2NamesListControl* control = CPbk2NamesListControl::NewL(
                R_PBK2_SPB_NAMES_LIST_EXTENSION_CONTROL,
                container,
                *iContactManager,
                *Phonebook2::Pbk2AppUi()->ApplicationServices().
                    ViewSupplier().AllContactsViewL(),
                *iNameFormatter,
                Phonebook2::Pbk2AppUi()->ApplicationServices().StoreProperties() );

        PBK2_PROFILE_END
            ( Pbk2Profile::ENamesListViewCreateNamesListControl );

        PBK2_PROFILE_START(Pbk2Profile::ENamesListViewSetControl);
        container->SetControl(control, iView.ClientRect()); // takes ownership
        container->AddForegroundObserver( control );
        control->AddObserverL(*this);
        PBK2_PROFILE_END(Pbk2Profile::ENamesListViewSetControl);

        PBK2_PROFILE_START(Pbk2Profile::ENamesListViewAddToStack);
        CCoeEnv::Static()->AppUi()->AddToStackL(iView, container);
        PBK2_PROFILE_END(Pbk2Profile::ENamesListViewAddToStack);
        CleanupStack::Pop(container);
        iContainer = container;
        iControl = control;
        iControl->SetObserver(this);
        iControlProxy->SetControl( iControl );
        iPointerEventInspector = control;
        
        // Add command item to container
        AddCommandItemsToContainerL();
        PBK2_PROFILE_END(Pbk2Profile::ENamesListViewCreateControls);
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::StoreStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::StoreStateL()
    {
    CPbk2ViewState* state = ViewStateLC();
    CleanupStack::Pop(state);
    delete iControlState;
    iControlState = state;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::RestoreStateL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::RestoreStateL()
    {
    iControl->RestoreControlStateL(iControlState);
    // Parameter state overrides stored state
    iControl->RestoreControlStateL(iParamState);
    delete iControlState;
    iControlState = NULL;
    delete iParamState;
    iParamState = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::UpdateCbasL()
    {
    if ( iControl )
        {
        TBool listContainsContacts = ( iControl->NumberOfContacts() > 0 );
        listContainsContacts = listContainsContacts || (iControl->CommandItemCount() > 0); 
        TBool contactsMarked = iControl->ContactsMarked();

        if ( listContainsContacts && !contactsMarked )
            {
            //For command items msk is select
            if( iControl->FocusedContactIndex() == KErrNotFound )
                {
                // Set middle softkey as select.
                iView.Cba()->SetCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_EXIT_SELECT );
                }
            else
                {
                // Set middle softkey as open cca.
                iView.Cba()->SetCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_EXIT_CONNECT );
                }
            }
        else if ( listContainsContacts && contactsMarked )
            {
            // Set middle softkey as Context Icon
            iView.Cba()->SetCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_EXIT_CONTEXT );
            // Change context menu when marked items
            iView.MenuBar()->SetContextMenuTitleResourceId
                ( R_PHONEBOOK2_NAMESLIST_CONTEXT_MENUBAR_ITEMS_MARKED );
            }
        else // not listContainsContacts && not contactsMarked
            {
#ifndef ECE_DISABLE_CONTEXT_MENU
            // Set middle softkey as Context Icon.
            iView.Cba()->SetCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_EXIT_CONTEXT );
            // Change context menu when nameslist is empty
            iView.MenuBar()->SetContextMenuTitleResourceId
                ( R_PHONEBOOK2_NAMESLIST_CONTEXT_MENUBAR );
#else
            iView.Cba()->SetCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_EXIT_EMPTY );
#endif
            }
        iView.Cba()->DrawDeferred();
        }

    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::IdleSaveToolbarState
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListExView::IdleSaveToolbarStateL( TAny* aSelf )
    {
    CPbk2NamesListExView* self =
        static_cast<CPbk2NamesListExView*>( aSelf );
    if ( AknLayoutUtils::PenEnabled() && self && self->iToolbar )
        {
        TBool shown = self->iToolbar->IsShown();
        self->ShowToolbarOnViewActivation( shown );

        // Save state to central repository
        CRepository* cenrep =
            CRepository::NewLC( TUid::Uid( KCRUidPhonebook ) );
        User::LeaveIfError(
            cenrep->Set( KPhonebookNamesListToolbarVisibility, shown ) );
        CleanupStack::PopAndDestroy( cenrep );
        }
    return EFalse; // don't call again
    }

void CPbk2NamesListExView::ShowToolbarOnViewActivation(TBool aShown)
    {
    iView.ShowToolbarOnViewActivation(aShown);
    }
// --------------------------------------------------------------------------
// CPbk2NamesListExView::AttachAIWProvidersL
// --------------------------------------------------------------------------
//
inline void CPbk2NamesListExView::AttachAIWProvidersL()
    {
    PBK2_PROFILE_START( Pbk2Profile::ENamesListAIWRegisterInterestCall );
    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdCall, R_PHONEBOOK2_CALL_MENU,
        R_PHONEBOOK2_CALLUI_AIW_INTEREST, ETrue );
    PBK2_PROFILE_END( Pbk2Profile::ENamesListAIWRegisterInterestCall );
    
    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdCall, R_PHONEBOOK2_STYLUS_CALL_MENU,
        R_PHONEBOOK2_CALLUI_AIW_INTEREST, EFalse );

    PBK2_PROFILE_START( Pbk2Profile::ENamesListAIWRegisterInterestPOC );
    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdPoC, R_PHONEBOOK2_POC_MENU,
        R_PHONEBOOK2_POCUI_AIW_INTEREST, ETrue );
    PBK2_PROFILE_END( Pbk2Profile::ENamesListAIWRegisterInterestPOC );

    PBK2_PROFILE_START( Pbk2Profile::ENamesListAIWRegisterInterestSyncML );
    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdSynchronize, R_PHONEBOOK2_SYNCML_MENU,
        R_PHONEBOOK2_SYNCML_AIW_INTEREST, EFalse );
    PBK2_PROFILE_END( Pbk2Profile::ENamesListAIWRegisterInterestSyncML );
    iCommandHandler->RegisterAiwInterestL
        ( KOtasAiwCmdSynchronize, R_PHONEBOOK2_OVISYNC_MENU,
        R_PHONEBOOK2_OVISYNC_AIW_INTEREST, ETrue );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::UpdateListEmptyTextL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::UpdateListEmptyTextL(
        TInt aListState )
    {
    iListState = aListState;

    if ( !iIdleTexter )
        {
        iIdleTexter = CIdle::NewL( CActive::EPriorityStandard );
        }

    iIdleTexter->Cancel();
    iIdleTexter->Start( TCallBack(
        ( &CPbk2NamesListExView::IdleSetEmptyTextL ), this ) );
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleLongTapEventL(
        const TPoint& /*aPenEventLocation*/,
        const TPoint& /*aPenEventScreenLocation*/ )
    {
//no implementation needed currently
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::Reset
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::Reset()
    {
    if ( iControl )
        {
        iControl->Reset();
        }

    delete iControlState;
    iControlState = NULL;
    delete iParamState;
    iParamState = NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::IdleSetEmptyTextL
// --------------------------------------------------------------------------
//
TInt CPbk2NamesListExView::IdleSetEmptyTextL( TAny* aSelf  )
    {
    CPbk2NamesListExView* self =
        static_cast<CPbk2NamesListExView*>( aSelf );

    HBufC* text = NULL;
    switch ( self->iListState )
        {
        case CPbk2NamesListControl::EStateEmpty:
        case CPbk2NamesListControl::EStateNotReady:
            {
            text = StringLoader::LoadLC( R_QTN_PHOB_NO_ENTRIES_LONG );

            // Append secondary text
            TInt secondaryTextId = R_QTN_PHOB_NO_ENTRIES_LONGER;
            if ( self->iListState == CPbk2NamesListControl::EStateNotReady )
                {
                secondaryTextId = R_QTN_PHOB_STORES_NA;
                }
            _LIT( KNewline, "\n" );
            HBufC* secondaryText = StringLoader::LoadLC( secondaryTextId );
            TInt newLength = text->Length()
                           + KNewline().Length()
                           + secondaryText->Length();
            text = text->ReAllocL( newLength );
            TPtr ptr = text->Des();
            ptr.Append( KNewline );
            ptr.Append( *secondaryText );
            CleanupStack::PopAndDestroy( secondaryText );

            // Fix text ptr in cleanup stack, see ReAllocL above
            CleanupStack::Pop(); // text
            CleanupStack::PushL( text );
            break;
            }
        case CPbk2NamesListControl::EStateReady: // FALLTHROUGH
        case CPbk2NamesListControl::EStateFiltered:
            {
            text = StringLoader::LoadLC( R_PBK2_FIND_NO_MATCHES );

            if (self->iListState == CPbk2NamesListControl::EStateFiltered &&
                self->iControl->NumberOfContacts() == 0 &&
                self->iStylusPopupMenuLaunched
                )
                {
                self->iPopupMenu->RemoveMenuItem(EPbk2CmdAddToTopContacts);
                self->iPopupMenu->RemoveMenuItem(EPbk2CmdRemoveFromTopContacts);
                self->iPopupMenu->RemoveMenuItem(EPbk2CmdEditMe);
                self->iPopupMenu->RemoveMenuItem(EPbk2CmdSend);
                }
            break;
            }
        default:
            {
            text = KNullDesC().AllocLC();
            break;
            }
        }
    if ( text )
        {
        self->iControl->SetTextL( *text );
        CleanupStack::PopAndDestroy( text );
        self->iControl->DrawDeferred();
        }

    return EFalse; // don't call again
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::NeedToHideToolbar
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListExView::NeedToHideToolbar(TInt aCurrentCommand)
    {
    // before show new dialog or pop-up list, etc for the below commands, it is
    // required to hide the toolbar, and retrieve it after command complete
    if ( aCurrentCommand == EPbk2CmdCreateNew ||
         aCurrentCommand == EPbk2CmdWrite ||
         aCurrentCommand == EPbk2CmdAddFavourites ||
         aCurrentCommand == EPbk2CmdRcl ||    
         aCurrentCommand == EPbk2CmdMoveInTopContactsList ||
         aCurrentCommand == EPbk2CmdEditMe ||
         aCurrentCommand == EPbk2CmdWriteNoQuery )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::IsPhoneMemoryUsedL
// --------------------------------------------------------------------------
//
TBool CPbk2NamesListExView::IsPhoneMemoryUsedL() const
	{
	CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceL();
			
	// Get current configuration
	CPbk2StoreConfiguration& storeConfig = appServices->StoreConfiguration();
	
	CVPbkContactStoreUriArray* uriArray = storeConfig.CurrentConfigurationL();
			
	TVPbkContactStoreUriPtr contactUri( VPbkContactStoreUris::DefaultCntDbUri() );		
			
	// Check is phone memory included
	TBool phoneMemory = uriArray->IsIncluded( contactUri );
	
	delete uriArray;		
	Release( appServices );
	
	return phoneMemory;
	}
	
// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleGainingForeground
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleGainingForeground()
    {
    // If the control state is not the same as the state before losing forground
    // set it back to the state before losing forground.
    if( iControl && ( ( iControl->IsVisible() && !iCtrlVisibleStateBeforeLosingForground ) ||
                     ( !iControl->IsVisible() && iCtrlVisibleStateBeforeLosingForground ) ) )
        {
        iControl->MakeVisible( iCtrlVisibleStateBeforeLosingForground );
        }

    // This flag is used to force the find box to be re-initialized when the application
    // is back in the foreground.
    iNeedSetFocus = ETrue;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleLosingForeground
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleLosingForeground()
    {
    // If some command is executing now and control state is invisible. Set the control
    // visible before it hide in background.
    // To do this is because some special states, like creating new contact or
    // creating new message, the iControl will be set invisible (In the function the control
    // set invisible in the function PreCommandExecutionL which is to avoid  displaying two
    // find boxes after press the create new contact button). And during the command
    // executing, set the control visible before it is hided into background.This is make
    // sure that the control should be updated after it set foreground again.
    if( iControl )
        {
        iCtrlVisibleStateBeforeLosingForground = iControl->IsVisible();
        if( !iCommandState && !iCtrlVisibleStateBeforeLosingForground )
            {
            iControl->MakeVisible( ETrue );
            }
        }

    // This flag is used to force the find box to be re-initialized when the application
    // is back in the foreground.
    iNeedSetFocus = EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::UIExtensionViewExtension
// --------------------------------------------------------------------------
//
TAny* CPbk2NamesListExView::UIExtensionViewExtension(
                TUid aExtensionUid )
    {
    if( aExtensionUid == KMPbk2UIExtensionView2Uid )
        {
        return static_cast<MPbk2UIExtensionView2*>( this );
        }
    
    return NULL;
    }

// --------------------------------------------------------------------------
// CPbk2NamesListExView::HandleForegroundEventL
// --------------------------------------------------------------------------
//
void CPbk2NamesListExView::HandleForegroundEventL(TBool aForeground)
    {
    if ( iControl )
        {
        iControl->HandleViewForegroundEventL( aForeground );
        }
    }

//  End of File
