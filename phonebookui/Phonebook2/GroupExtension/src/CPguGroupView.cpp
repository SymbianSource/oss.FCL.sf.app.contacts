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
* Description:  Phonebook 2 Group UI extension group view.
*
*/


#include "CPguGroupView.h"

// Phonebook 2
#include "Pbk2GroupUi.hrh"
#include <CPbk2UIExtensionView.h>
#include <CPbk2AppUiBase.h>
#include <CPbk2NamesListControl.h>
#include <CPbk2ViewState.h>
#include <CPbk2ControlContainer.h>
#include <MPbk2ViewActivationTransaction.h>
#include <MPbk2ViewExplorer.h>
#include <CPbk2StoreConfiguration.h>
#include <pbk2groupuires.rsg>
#include <pbk2commonui.rsg>
#include <pbk2commands.rsg>
#include <pbk2uicontrols.rsg>
#include <csxhelp/phob.hlp.hrh>
#include <MPbk2CommandHandler.h>
#include <Pbk2UID.h>
#include <MPbk2AppUi.h>
#include <CPbk2ViewStateTransformer.h>
#include <MPbk2StartupMonitor.h>
#include <CPbk2StorePropertyArray.h>
#include <CPbk2StoreProperty.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ContactViewSupplier.h>
#include <CPbk2ApplicationServices.h>
#include <MPbk2StoreValidityInformer.h>
#include <Pbk2MenuFilteringFlags.hrh>

// Virtual Phonebook
#include <MVPbkContactStoreList.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreProperties.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>

// System includes
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <avkon.rsg>
#include <featmgr.h>
#include <StringLoader.h>
#include <AiwCommon.hrh>
#include <eikmenub.h>
#include <AknUtils.h>
#include <eiklbx.h>
#include <eiklbo.h>
#include <coecntrl.h>
#include <touchfeedback.h>

/// Unnamed namespace for local definitions
namespace {

_LIT( KNewline, "\n" );

/**
 * Returns ETrue if shift is depressed in given key event.
 *
 * @param aKeyEvent     Key event
 * @return  ETrue if shift is depressed.
 */
inline TBool ShiftDown( const TKeyEvent& aKeyEvent )
    {
    return ( aKeyEvent.iModifiers &
           ( EModifierShift|EModifierLeftShift|EModifierRightShift ) ) != 0;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPguGroupView::CPguGroupView
// --------------------------------------------------------------------------
//
CPguGroupView::CPguGroupView( CPbk2UIExtensionView& aView ) :
            iView( aView )
    {
    }

// --------------------------------------------------------------------------
// CPguGroupView::~CPguGroupView
// --------------------------------------------------------------------------
//
CPguGroupView::~CPguGroupView()
    {
    if ( iStoreConfiguration )
        {
        iStoreConfiguration->RemoveObserver( *this );
        }

    delete iControlState;
    delete iParamState;

    if ( iContainer )
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::NewL
// --------------------------------------------------------------------------
//
CPguGroupView* CPguGroupView::NewL( CPbk2UIExtensionView& aView )
    {
    CPguGroupView* self = new ( ELeave ) CPguGroupView( aView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPguGroupView::ConstructL
// --------------------------------------------------------------------------
//
inline void CPguGroupView::ConstructL()
    {
    iStoreConfiguration = &Phonebook2::Pbk2AppUi()->ApplicationServices().
        StoreConfiguration();

    iStoreConfiguration->AddObserverL( *this );

    iCommandHandler = Phonebook2::Pbk2AppUi()->ApplicationServices().
        CommandHandlerL();

    // Attach AIW providers
    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdPoC, R_PHONEBOOK2_GROUPVIEWS_POC_MENU,
          R_PHONEBOOK2_POCUI_AIW_INTEREST, ETrue );
    }

// --------------------------------------------------------------------------
// CPguGroupView::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPguGroupView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if ( iContainer )
        {
        iContainer->SetRect( iView.ClientRect() );
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPguGroupView::ViewStateLC() const
    {
    CPbk2ViewState* state = iControl->ControlStateL();
    CleanupStack::PushL( state );
    return state;
    }

// --------------------------------------------------------------------------
// CPguGroupView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPguGroupView::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool result = EFalse;
    TBool itemSpecEnabled = 
        Phonebook2::Pbk2AppUi()->ActiveView()->MenuBar()->ItemSpecificCommandsEnabled();
    if ( itemSpecEnabled && (EStdKeyHash == aKeyEvent.iScanCode) )
        {
        //make sure Hash key won't be passed to default keyHandler
        return ETrue;
        }
    if ( aType == EEventKey )
        {
        switch ( aKeyEvent.iCode )
            {
            case EKeyBackspace:
                {
                if ( itemSpecEnabled && ( iControl->NumberOfContacts() > 0 ) &&
                     ( iControl->FindTextL() == KNullDesC ) )
                    {
                    HandleCommandL( EPbk2CmdRemoveGroup );
                    result = ETrue;
                    }
				break;
				}
            case EKeyEnter: // FALLTHROUGH
            case EKeyOK:
                {
                if ( !ShiftDown( aKeyEvent )  // pure OK or ENTER key
                    && iControl->NumberOfContacts() == 0 )
					{
                    result = ETrue;
                    }
                break;
                }
            default:
                {
                break;
                }
            }
        }
    return result;
    }



// ----------------------------------------------------------------------------
// CPguGroupView::HandleListBoxEventL
// ----------------------------------------------------------------------------
//
void CPguGroupView::HandleListBoxEventL(CEikListBox* /*aListBox*/,TListBoxEvent aEventType)
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
            case EEventItemClicked: //Happens after focus changed
            break;
            case EEventEnterKeyPressed:
                {
                // Select key is mapped to "Open Group" command
                HandleCommandL( EPbk2CmdOpenGroup );
                break;
                }    
            default:
               break;
            }
        }
    
    
     
    }


// --------------------------------------------------------------------------
// CPguGroupView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPguGroupView::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        if ( iPointerEventInspector->FocusableItemPointed() && iContainer )
            {
            iContainer->LongTapDetectorL().PointerEventL( aPointerEvent );
            }
        if ( iControl->NumberOfContacts()==0) 
            {
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
                {
                MTouchFeedback* feedback = MTouchFeedback::Instance();
                if ( feedback )
                    {
                    feedback->InstantFeedback( ETouchFeedbackNone );
                    }
                }
            else if ( aPointerEvent.iType == TPointerEvent::EButton1Up )		
                {
                ShowContextMenuL();
                }
            }

        }
    }






// --------------------------------------------------------------------------
// CPguGroupView::ShowContextMenuL
// --------------------------------------------------------------------------
//
void CPguGroupView::ShowContextMenuL()
    {

    if ( iStylusPopupMenuLaunched )
        {
        // Absorb EButton1Up event if we already launched the stylus
        // popup menu
        iStylusPopupMenuLaunched = EFalse;
        }
    else if ( !iPointerEventInspector->SearchFieldPointed() )
        {
        if ( iControl->NumberOfContacts() > 0 )
            {
            if ( iPointerEventInspector->FocusedItemPointed() )
                {
                // Open group
                HandleCommandL(EPbk2CmdOpenGroup);
                }
            }
        }
    }
    
    

// --------------------------------------------------------------------------
// CPguGroupView::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPguGroupView::GetViewSpecificMenuFilteringFlagsL() const
    {
    TInt flags( 0 );
    flags |= CurrentStoreSupportsGroupsL();
    if ( iControl )
        {
        flags |= iControl->GetMenuFilteringFlagsL();
        const MVPbkViewContact* contact = iControl->FocusedViewContactL();
        if( contact && contact->Expandable() )
            {
            flags |= KPbk2FocusedItemIsExpandable;
            }
        }
    return flags;
    }

// --------------------------------------------------------------------------
// CPguGroupView::DoActivateL
// --------------------------------------------------------------------------
//
void CPguGroupView::DoActivateL
        ( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
          const TDesC8& aCustomMessage )
    {
    // Add this view to observe menu command events
    iCommandHandler->AddMenuCommandObserver(*this);

    HBufC* title = StringLoader::LoadLC( R_QTN_PHOB_TITLE );

    MPbk2ViewActivationTransaction* viewActivationTransaction =
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
            ( iView.Id(), aPrevViewId, title, NULL,
              Phonebook2::EUpdateNaviPane | Phonebook2::EUpdateTitlePane |
              Phonebook2::EUpdateContextPane );

    // Check activation parameters
    delete iParamState;
    iParamState = NULL;
    if ( aCustomMessageId == CPbk2ViewState::Uid() )
        {
        iParamState = CPbk2ViewState::NewL( aCustomMessage );
        }
    else if (aCustomMessageId == TUid::Uid(KPbkViewStateUid))
        {
        // Handle legacy view state
        CPbk2ViewStateTransformer* transformer = CPbk2ViewStateTransformer::NewLC
            ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager() );
        iParamState = transformer->
            TransformLegacyViewStateToPbk2ViewStateLC( aCustomMessage );
        CleanupStack::Pop();
        CleanupStack::PopAndDestroy( transformer );
        }

    // Notifies startup monitor of view activation
    if (Phonebook2::Pbk2AppUi()->Pbk2StartupMonitor())
        {
        Phonebook2::Pbk2AppUi()->Pbk2StartupMonitor()->
            NotifyViewActivationL( iView.Id(),
            *Phonebook2::Pbk2AppUi()->ApplicationServices().
                ViewSupplier().AllGroupsViewL() );
        }

    CreateControlsL();
    iContainer->ActivateL();
    
  
    CCoeControl* ctrl=iControl->ComponentControl(0);
    CEikListBox* listbox=static_cast <CEikListBox*> (ctrl);
    listbox->SetListBoxObserver( this ); 
    

    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy( 2 ); //title, viewActivationTransaction

    // Should be here because in NOT READY case view does not become ready
    // and cbas are not updated without this.
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPguGroupView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPguGroupView::DoDeactivate()
    {
    iCommandHandler->RemoveMenuCommandObserver( *this );
    // Trash the old states
    delete iParamState;
    iParamState = NULL;
    delete iControlState;
    iControlState = NULL;

    if ( iContainer )
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iContainer );
        // Store current state, safe to ignore. There's no real harm,
        // if theres no stored state when activating this view again
        TRAP_IGNORE( StoreStateL() );
        delete iContainer;
        iContainer = NULL;
        iControl = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPguGroupView::HandleCommandL( TInt aCommand )
    {
    // this will leave if control is NULL due to e.g. bad view switch
    CheckUiControlL();
    switch( aCommand )
    	{
        case EAknCmdExit:
        case EAknSoftkeyExit:
        case EPbk2CmdExit:
        case EAknCmdHideInBackground:
            {
            // AppUi will send the application to background.
            // Because the control is not destroyed it must be reset so that
            // when the view comes foreground again it will look like a fresh
            // and brand new control.
            iControl->Reset();
       	    break;
            }
    	default:
    	    break;
        }
    // No command handling in this class, forward to Commands
    if ( !iCommandHandler->HandleCommandL( aCommand, *iControl, &iView ) )
        {
        iControl->ProcessCommandL( aCommand );
        Phonebook2::Pbk2AppUi()->HandleCommandL( aCommand );
        UpdateCbasL();
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPguGroupView::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {

    // this will leave if control is NULL due to e.g. bad view switch
    CheckUiControlL();
    
    
    if ( iControl )
        {
        // Ask the control do control specific filtering
        // (for example call HandleMarkableListDynInitMenuPane if needed)
        iControl->DynInitMenuPaneL( aResourceId, aMenuPane );
        }

    switch ( aResourceId )
        {
        case R_PHONEBOOK2_GROUPSLIST_MENU:
            {
            if ( !CurrentStoreSupportsGroupsL() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdCreateNewGroup, ETrue );
                }
            // Weed out commands not meant to be used with empty list
            if ( iControl->NumberOfContacts() == 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdOpenGroup, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdWriteGroup, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdRemoveGroup, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdRenameGroup, ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdSetToneToGroup, ETrue );
                }
            break;
            }

        case R_PHONEBOOK2_GROUPLIST_CONTEXT_MENU:
            {
            if( !CurrentStoreSupportsGroupsL() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdCreateNewGroup, ETrue );
                }
            break;
            }
        case R_PHONEBOOK2_INFO_MENU:
            {
            // If there are no available stores, "memory details" should not be shown in Options menu
            if( !CurrentStoreIsAvailableL() )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdPhonebook2Info, ETrue ); 
                }
            break;
            }
        default:
            {
            // Phonebook 2 menu filtering happens in Commands
            iCommandHandler->DynInitMenuPaneL(
                    aResourceId, aMenuPane, iView, *iControl );
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CPguGroupView::HandleLongTapEventL(
        const TPoint& /*aPenEventLocation*/,
        const TPoint& aPenEventScreenLocation )
    {
	if( iControl->ContactsMarked() )
		{
		//if there are marked contacts, context menu should not open
		return;
		}
    }

// --------------------------------------------------------------------------
// CPguGroupView::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPguGroupView::HandleControlEventL
        ( MPbk2ContactUiControl& /*aControl*/,
          const TPbk2ControlEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EReady:
            {
            UpdateCbasL();

            // Restore control state
            RestoreStateL();
            break;
            }
        case TPbk2ControlEvent::EItemAdded:
        case TPbk2ControlEvent::EItemRemoved:
        case TPbk2ControlEvent::EContactSetChanged:
            {
            UpdateCbasL();
            break;
            }
        case TPbk2ControlEvent::EControlStateChanged:
            {
            UpdateCbasL();
            iListControlState = aEvent.iInt;
            UpdateListEmptyTextL();
            iContainer->Control()->DrawDeferred();
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
// CPguGroupView::ConfigurationChanged
// --------------------------------------------------------------------------
//
void CPguGroupView::ConfigurationChanged()
    {
    if ( iContainer )
        {
        iContainer->Control()->Reset();
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::ConfigurationChangedComplete
// --------------------------------------------------------------------------
//
void CPguGroupView::ConfigurationChangedComplete()
    {
    if ( iContainer )
        {
        TRAPD( error, HandleConfigurationChangeL() );
        if ( error != KErrNone )
            {
            CCoeEnv::Static()->HandleError( error );
            iContainer->Control()->Reset();
            }
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::PreCommandExecutionL
// --------------------------------------------------------------------------
//
void CPguGroupView::PreCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPguGroupMembersView::PostCommandExecutionL
// --------------------------------------------------------------------------
//
void CPguGroupView::PostCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    UpdateCbasL();
    }

// --------------------------------------------------------------------------
// CPguGroupView::CurrentStoreSupportsGroupsL
// --------------------------------------------------------------------------
//
TBool CPguGroupView::CurrentStoreSupportsGroupsL() const
    {
    TBool ret = EFalse;
    MVPbkContactStoreList& stores =
        Phonebook2::Pbk2AppUi()->ApplicationServices().
            ContactManager().ContactStoresL();
    CVPbkContactStoreUriArray* uris =
        iStoreConfiguration->CurrentConfigurationL();
    CleanupStack::PushL( uris );

    const TInt count = uris->Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        MVPbkContactStore* store = stores.Find( ( *uris )[i] );
        if ( store )
            {
            // If we found atleast one store having groups we
            // return the flag.
            if ( store->StoreProperties().SupportsContactGroups() )
                {
                ret = ETrue;
                break;
                }
            }
        }

    CleanupStack::PopAndDestroy( uris );
    return ret;
    }

// --------------------------------------------------------------------------
// CPguGroupView::CurrentStoreIsAvailable
// --------------------------------------------------------------------------
//
TBool CPguGroupView::CurrentStoreIsAvailableL() const
    {
    TBool ret = ETrue;
    CPbk2ApplicationServices* appServices = CPbk2ApplicationServices::InstanceLC();

    // Returns the stores that are currently available and open(selected by user in Settings).
    CVPbkContactStoreUriArray* aValidStores = appServices->StoreValidityInformer().CurrentlyValidStoresL();
    CleanupStack::PushL( aValidStores );
    if( 0 == aValidStores->Count() )
        {
        ret = EFalse;
        }
     CleanupStack::PopAndDestroy(); // aValidStores
     CleanupStack::PopAndDestroy(); // appServices
     return ret;
    }

// --------------------------------------------------------------------------
// CPguGroupView::CreateControlsL
// --------------------------------------------------------------------------
//
 void CPguGroupView::CreateControlsL()
    {
    if ( !iContainer )
        {
        // Create the container and control
        CContainer* container = CContainer::NewLC( &iView, &iView, iView );

        container->SetHelpContext( TCoeHelpContext
            ( iView.ApplicationUid(), KPHOB_HLP_GROUP_LIST ) );

        MPbk2ApplicationServices& appServices =
            Phonebook2::Pbk2AppUi()->ApplicationServices();

        CPbk2NamesListControl* control = CPbk2NamesListControl::NewL
            ( R_PBK2_GROUPS_LIST_CONTROL, container,
              appServices.ContactManager(),
              *appServices.ViewSupplier().AllGroupsViewL(),
              appServices.NameFormatter(),
              appServices.StoreProperties() );
        CleanupStack::PushL( control );

        control->AddObserverL( *this );
        // takes ownership
        container->SetControl( control, iView.ClientRect() );
        CleanupStack::Pop( control );

        CCoeEnv::Static()->AppUi()->AddToStackL( iView, container );
        CleanupStack::Pop( container );
        iContainer = container;
        iControl = control;
        iPointerEventInspector = control;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::StoreStateL
// --------------------------------------------------------------------------
//
void CPguGroupView::StoreStateL()
    {
    CPbk2ViewState* state = ViewStateLC();
    CleanupStack::Pop(state);
    if ( state )
        {
        delete iControlState;
        iControlState = state;
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::RestoreStateL
// --------------------------------------------------------------------------
//
void CPguGroupView::RestoreStateL()
    {
    iControl->RestoreControlStateL( iControlState );
    iControl->RestoreControlStateL( iParamState);
    delete iControlState;
    iControlState = NULL;
    }

// --------------------------------------------------------------------------
// CPguGroupView::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPguGroupView::UpdateCbasL()
    {
    if ( iControl )
        {
        if ( iControl->NumberOfContacts() > 0 )
            {
            // Set middle softkey as Open.
            iView.Cba()->SetCommandSetL( R_PGU_SOFTKEYS_OPTIONS_EXIT_OPEN );
            iView.Cba()->DrawDeferred();
            }
        else
            {
            // Set middle softkey as Context menu.
            iView.Cba()->SetCommandSetL(
                R_PBK2_SOFTKEYS_OPTIONS_EXIT_CONTEXT );
            iView.Cba()->DrawDeferred();
            // Change context menu when nameslist is empty
            iView.MenuBar()->SetContextMenuTitleResourceId
                ( R_PHONEBOOK2_GROUPLIST_CONTEXT_MENUBAR );
            }
        }
    }


// --------------------------------------------------------------------------
// CPguGroupView::UpdateListEmptyTextL
// --------------------------------------------------------------------------
//
void CPguGroupView::UpdateListEmptyTextL()
    {
    HBufC* text = NULL;
    switch ( iListControlState )
        {
        case CPbk2NamesListControl::EStateEmpty:
        case CPbk2NamesListControl::EStateNotReady:
            {
            text = StringLoader::LoadLC( R_QTN_PHOB_NO_GROUPS );

            CVPbkContactStoreUriArray* uris =
                iStoreConfiguration->CurrentConfigurationL();
            CleanupStack::PushL( uris );

            HBufC* helpText = NULL;
            TInt numberOfStores = uris->Count();
            if ( CurrentStoreSupportsGroupsL() )
                {
                // At least one memory in use which supports groups
                helpText = StringLoader::LoadLC( R_QTN_PHOB_NO_GROUPS_LONGER );
                }
            else if ( numberOfStores > 1 )
                {
                // More than one memories in use, but none of them support groups
                helpText = StringLoader::LoadLC( R_QTN_PHOB_NO_GROUPS_STORES );
                }
            else if ( numberOfStores == 1 )
                {
                // The only used memory does not support groups
                const CPbk2StoreProperty* props =
                    Phonebook2::Pbk2AppUi()->ApplicationServices().
                        StoreProperties().FindProperty( (*uris)[0] );
                if ( props )
                    {
                    helpText = StringLoader::LoadLC(
                        R_QTN_PHOB_NO_GROUPS_STORE, props->StoreName() );
                    }
                }

            if ( !helpText )
                {
                CleanupStack::PopAndDestroy(); // uris
                }
            else
                {
                TInt newLength = text->Length()
                               + KNewline().Length()
                               + helpText->Length();

                text = text->ReAllocL( newLength );
                // note: no leaves beyond this point, otherwise
                text->Des().Append( KNewline );
                text->Des().Append( *helpText );

                CleanupStack::PopAndDestroy( 2 ); // helpText, uris
                // update the reallocated 'text' pointer in cleanup stack
                CleanupStack::Pop(); // text
                CleanupStack::PushL( text );
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
        iControl->SetTextL( *text );
        CleanupStack::PopAndDestroy(); // text
        }
    }

// --------------------------------------------------------------------------
// CPguGroupView::HandleConfigurationChangeL
// --------------------------------------------------------------------------
//
void CPguGroupView::HandleConfigurationChangeL()
    {
    // Force empty list text update after configuration change
    UpdateListEmptyTextL();

    UpdateCbasL();
    if ( iContainer && iContainer->Control() )
        {
        iContainer->Control()->SetViewL
            ( *Phonebook2::Pbk2AppUi()->ApplicationServices().
                ViewSupplier().AllGroupsViewL() );
        iContainer->Control()->DrawNow();
        }
    }


// --------------------------------------------------------------------------
// CPguGroupView::UiControlL
// --------------------------------------------------------------------------
//
void CPguGroupView::CheckUiControlL()
    {
    
    // check if we have invalid control 
    if (iControl==NULL) 
        {
        // leave with bad handle error since iControl is missing
        User::Leave(KErrBadHandle);
        }
    }


// End of File
