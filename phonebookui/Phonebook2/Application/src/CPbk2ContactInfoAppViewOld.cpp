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
* Description:  Phonebook 2 contact info application view.
*
*/

// INCLUDE FILES
#include "cpbk2contactinfoappview.h"

// Phonebook 2
#include "cpbk2appui.h"
#include "cpbk2document.h"
#include "cpbk2startupmonitor.h"
#include "cpbk2viewexplorer.h"
#include "cpbk2viewactivationtransaction.h"
#include "pbk2internalcommands.hrh"
#include "cpbk2viewloader.h"
#include <phonebook2.rsg>
#include <pbk2uicontrols.rsg>
#include <pbk2commands.rsg>
#include <cpbk2viewstate.h>
#include <mpbk2contactnavigation.h>
#include <cpbk2controlcontainer.h>
#include <cpbk2contactinfocontrol.h>
#include <mpbk2commandhandler.h>
#include <mpbk2contactdetailscontrol.h>
#include <cpbk2storeconfiguration.h>
#include <cpbk2memoryentrycontactloader.h>
#include <pbk2commands.hrh>
#include <cpbk2viewstatetransformer.h>
#include <tpbk2storecontactanalyzer.h>
#include <cpbk2fieldpropertyarray.h>
#include <cpbk2sortordermanager.h>
#include <cpbk2viewgraph.h>
#include <mpbk2pointereventinspector.h>
#include <pbk2navigatorfactory.h>
#include <mpbk2applicationservices.h>
#include <mpbk2contactviewsupplier.h>
#include <pbk2uid.h>
#include <csxhelp/phob.hlp.hrh>

// Virtual Phonebook
#include <cvpbkcontactmanager.h>
#include <mvpbkstorecontact.h>
#include <mvpbkcontactlink.h>
#include <mvpbkcontactviewbase.h>
#include <mvpbkexpandable.h>
#include <cvpbkcontactstoreuriarray.h>
#include <mvpbkcontactstorelist.h>
#include <tvpbkcontactstoreuriptr.h>

// System includes
#include <avkon.rsg>
#include <aiwcommon.hrh>
#include <eikmenub.h>
#include <aknutils.h>
#include <stringloader.h>

// Debugging headers
#include <pbk2debug.h>
#include <pbk2profile.h>
#include <pbk2config.hrh>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_DoActivateL = 1,
    EPanicPostCond_DoDeactivate,
    EPanicPreCond_HandleCommandL,
    EPanicPreCond_HandleCommandKeyL,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPreCond_DynInitMenuPaneL,
    EPanicPreCond_ContactChangedL,
    EPanicPreCond_SetNavigatorViewL,
    EPanicPreCond_GetViewSpecificMenuFilteringFlagsL,
    EPanicPreCond_NULLPointer,
    EPanicPreCond_ViewLoaded,
    EPanicLogic_LoadContactL
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPbk2ContactInfoAppView" );
    User::Panic( KPanicText, aReason );
    }
#endif // _DEBUG

} /// namespace


// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::CPbk2ContactInfoAppView
// --------------------------------------------------------------------------
//
inline CPbk2ContactInfoAppView::CPbk2ContactInfoAppView
        ( CVPbkContactManager& aContactManager ) :
            iContactManager( aContactManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::~CPbk2ContactInfoAppView
// --------------------------------------------------------------------------
//
CPbk2ContactInfoAppView::~CPbk2ContactInfoAppView()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoAppView(%x)::~CPbk2ContactInfoAppView()"), this);

    delete iViewLoader;
    delete iControlState;
    delete iViewActivationTransaction;
    delete iNavigation;
    delete iContactLoader;
    iStoreList.Reset();
    iStoreList.Close();

    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        delete iContainer;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactInfoAppView* CPbk2ContactInfoAppView::NewL
        ( CVPbkContactManager& aContactManager )
    {
    CPbk2ContactInfoAppView* self =
        new ( ELeave ) CPbk2ContactInfoAppView( aContactManager );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoAppView::ConstructL()
    {
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoConstructL);

    // Init base class
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoViewBaseConstruct);
    CAknView::BaseConstructL(R_PHONEBOOK2_CONTACTINFO_APPVIEW);
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoViewBaseConstruct);

    iCommandHandler = Pbk2AppUi()->ApplicationServices().CommandHandlerL();
    iStoreConfiguration =  &Pbk2AppUi()->ApplicationServices().
        StoreConfiguration();

    PBK2_PROFILE_END(Pbk2Profile::EContactInfoConstructL);

    __ASSERT_DEBUG(iCommandHandler,
        Panic(EPanicPostCond_ConstructL));
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandleCommandL
        (TInt aCommandId)
    {
    __ASSERT_DEBUG(iControl,
        Panic(EPanicPreCond_HandleCommandL));

    switch (aCommandId)
        {
        case EAknSoftkeyBack:       // FALLTHROUGH
        case EPbk2CmdOpenPreviousView:
            {
            // Set the focused contact according to control state -> NULL
            ReturnToPreviousViewL( NULL );
            break;
            }
        default:
            {
            // No command handling in this class, forward to Commands
            if ( !iCommandHandler->HandleCommandL
                    ( aCommandId, *iControl, this ) )
                {
                iControl->ProcessCommandL(aCommandId);
                // Command was not handled in Pbk2Commands,
                // forward it to AppUi
                AppUi()->HandleCommandL(aCommandId);
                }
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::DynInitMenuPaneL
        (TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    __ASSERT_DEBUG(iControl,
        Panic(EPanicPreCond_DynInitMenuPaneL));

    if( aResourceId != R_PHONEBOOK2_SIND_INFOVIEW_MENU )
        {
        // Ask the control to do control specific filtering
        iControl->DynInitMenuPaneL(aResourceId, aMenuPane);

        // Phonebook 2 menu filtering happens in Commands
        iCommandHandler->DynInitMenuPaneL(
                aResourceId, aMenuPane, *this, *iControl);
        }
    else
        {
        // SIND InfoView menu option should not be available
        aMenuPane->SetItemDimmed( KAiwCmdInfoView, ETrue );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::Id
// --------------------------------------------------------------------------
//
TUid CPbk2ContactInfoAppView::Id() const
    {
    return Phonebook2::KPbk2ContactInfoViewUid;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::DoActivateL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::DoActivateL
        ( const TVwsViewId& aPrevViewId,
          TUid aCustomMessageId, const TDesC8& aCustomMessage )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoAppView(%x)::DoActivateL()"), this);

    PBK2_PROFILE_START(Pbk2Profile::EContactInfoStartTime);
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoDoActivateContactChanged);
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoDoActivateL);

    UpdateViewGraphL( aPrevViewId );

    iCommandHandler->AddMenuCommandObserver( *this );
    iStoreConfiguration->AddObserverL( *this );

    // Update application-wide state. UI control updates the title pane.
    delete iViewActivationTransaction;
    iViewActivationTransaction = NULL;
    iViewActivationTransaction =
        Pbk2AppUi()->Pbk2ViewExplorer()->HandleViewActivationLC
            ( Id(), aPrevViewId, NULL, NULL,
              Phonebook2::EUpdateNaviPane | Phonebook2::EUpdateContextPane );
    CleanupStack::Pop(); // iViewActivationTransaction

    if (aCustomMessageId == CPbk2ViewState::Uid() ||
        aCustomMessageId == TUid::Uid(KPbkViewStateUid))
        {
        // Handles both new and legacy view state parameters
        UpdateViewStateL(aCustomMessageId, aCustomMessage);
        }
    else
        {
        // No UI state specified, restore in previous state
        if (!iControlState)
            {
            // No previous state
            User::Leave(KErrArgument);
            }
        }

     if ( !iControlState->FocusedContact() )
        {
        User::Leave( KErrArgument );
        }

    ConstructNavigatorL();
    CreateControlsL();
    AttachAIWProvidersL();

    LoadContactViewL(aPrevViewId);
    iPreviousViewId = aPrevViewId;

    PBK2_PROFILE_END(Pbk2Profile::EContactInfoDoActivateL);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::DoDeactivate()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoAppView(%x)::DoDeactivate()"), this);

    delete iViewLoader;
    iViewLoader = NULL;

    iCommandHandler->RemoveMenuCommandObserver( *this );
    
    iStoreConfiguration->RemoveObserver( *this );

    delete iViewActivationTransaction;
    iViewActivationTransaction = NULL;
    delete iNavigation;
    iNavigation = NULL;

    // Trash the old states
    delete iControlState;
    iControlState = NULL;

    // Reset store list
    iStoreList.Reset();

    if (iContainer)
        {
        iContainer->Control()->HideThumbnail();

        // Remove view and its control from the view stack
        AppUi()->RemoveFromViewStack(*this, iContainer);

        // Store current state, safe to ignore. There's no real harm,
        // if theres no stored state when activating this view again
        TRAP_IGNORE(StoreStateL());

        // Destroy the container control
        delete iContainer;
        iContainer = NULL;
        iControl = NULL;
        }
        
    delete iContactLoader;
    iContactLoader = NULL;    
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleForegroundEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandleForegroundEventL( TBool aForeground )
    {
    CPbk2AppView::HandleForegroundEventL(aForeground);

    // If this view is gaining foreground, then make sure that thumbnail
    // gets drawn
    if (aForeground && iContainer && iContainer->Control())
        {
        if ( iContainer->Control()->IsVisible() )
            {
            iContainer->Control()->ShowThumbnail();
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2ContactInfoAppView::ViewStateLC() const
    {
    __ASSERT_DEBUG( iControl ,Panic(EPanicPreCond_NULLPointer));

    CPbk2ViewState* state = iControl->ControlStateL();
    CleanupStack::PushL(state);
    return state;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoAppView::HandleCommandKeyL
        (const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    __ASSERT_DEBUG(iContainer && iNavigation,
        Panic(EPanicPreCond_HandleCommandKeyL));

    // Pass the key event to the strategy
    TBool ret = iNavigation->HandleCommandKeyL(aKeyEvent, aType);

    if (!ret && aType == EEventKey)
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyOK:
                {
                LaunchPopupMenuL(R_PHONEBOOK2_CONTACTINFO_CONTEXTBAR);
                ret = ETrue;
                break;
                }
            case EKeyBackspace:  // Clear key
                {
                HandleCommandL(EPbk2CmdDeleteMe);
                ret = ETrue;
                break;
                }
            case EKeyPhoneSend:
                {
                // If focused field is either url or email field
                // the proper context menu should be launched.
                if ( IsFocusedFieldTypeL( R_PHONEBOOK2_URL_SELECTOR )
                    || IsFocusedFieldTypeL( R_PHONEBOOK2_EMAIL_SELECTOR ) )
                    {
                    LaunchPopupMenuL( R_PHONEBOOK2_CONTACTINFO_CONTEXTBAR );
                    }
                else
                    {
                    // Use the EPbk2CmdCall id to identify that
                    // call is being launched with send key
                    HandleCommandL( EPbk2CmdCall );
                    }
                ret = ETrue;
                break;
                }
            case EKeyPoC:
                {
                // Use the EPbk2CmdPoC id to identify that
                // call is being launched with PoC key
                HandleCommandL( EPbk2CmdPoC );
                ret = ETrue;
                break;
                }
            default:
                {
                break;
                }
            }
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandlePointerEventL(
        const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        CPbk2AppView::HandlePointerEventL( aPointerEvent );

        switch ( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Up:
                {
                if ( iPointerInspector &&
                     iPointerInspector->FocusedItemPointed() )
                    {
                    // launch context menu if user tapped a focused detail
                    LaunchPopupMenuL( R_PHONEBOOK2_CONTACTINFO_CONTEXTBAR );
                    }
                break;
                }
            default:;
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2ContactInfoAppView::GetViewSpecificMenuFilteringFlagsL() const
    {
    __ASSERT_DEBUG( iControl,
        Panic( EPanicPreCond_GetViewSpecificMenuFilteringFlagsL ) );

    return iControl->GetMenuFilteringFlagsL();
    }


// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::PrepareForContactChangeL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::PrepareForContactChangeL()
    {
    StoreStateL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ContactChangedL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ContactChangedL( MVPbkStoreContact* aContact )
    {
    PBK2_PROFILE_END(Pbk2Profile::EContactInfoDoActivateContactChanged);

    PBK2_PROFILE_START(Pbk2Profile::EContactInfoContactChangedL);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoAppView(%x)::ContactChangedL() begin"), this);

    if ( !iContainer )
        {
        // It is possible that this method get called before
        // the container and control get constructed.
        // In that case, ignore the given contact by deleting
        // it and do not continue the contact changed process.
        delete aContact;

        // iViewActivationTransaction should not be deleted here
        // since it is not tied to this ContactChangedL request

        PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
            ("CPbk2ContactInfoAppView(%x)::ContactChangedL() controls not ready"),
            this);
        }
    else
        {
        __ASSERT_DEBUG(iViewActivationTransaction,
            Panic(EPanicPreCond_ContactChangedL));
        __ASSERT_DEBUG(iNavigation, Panic(EPanicPreCond_ContactChangedL));

        PBK2_PROFILE_START(Pbk2Profile::EContactInfoContactChangedLUpdateControls);

        // Update controls
        if (aContact)
            {
            CleanupDeletePushL( aContact );
            iContainer->Control()->UpdateL(aContact);
            CleanupStack::Pop(aContact);    // ownership was taken
            }
        SetEmptyTextL( R_QTN_PHOB_NO_ITEMS );

        PBK2_PROFILE_END(Pbk2Profile::EContactInfoContactChangedLUpdateControls);

        PBK2_PROFILE_START(Pbk2Profile::EContactInfoContactChangedLSetFocusedField);

        if ( iControlState->FocusedContact() )
            {
            if ( !iControlState->FocusedContact()->RefersTo( *aContact ) )
                {
                iContainer->Control()->SetFocusedFieldIndex( KErrNotFound );
                iControlState->SetFocusedFieldIndex( KErrNotFound );
                }
            }
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoContactChangedLSetFocusedField);

        PBK2_PROFILE_START(Pbk2Profile::EContactInfoContactChangedLRestoreControlState);
        iContainer->Control()->RestoreControlStateL(iControlState);
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoContactChangedLRestoreControlState);

        // Commit application-wide state changes
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoContactChangedLCommitViewTransaction);
        iViewActivationTransaction->Commit();
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoContactChangedLCommitViewTransaction);

        // notifies startup monitor of view activation
        PBK2_PROFILE_START(Pbk2Profile::EContactInfoContactChangedLNotifyStartupMonitor);
        if (Pbk2AppUi()->Pbk2StartupMonitor())
            {
            Pbk2AppUi()->Pbk2StartupMonitor()->NotifyViewActivationL(Id());
            }
        PBK2_PROFILE_END(Pbk2Profile::EContactInfoContactChangedLNotifyStartupMonitor);
        }

    PBK2_PROFILE_END(Pbk2Profile::EContactInfoContactChangedL);

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoAppView(%x)::ContactChangedL() end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ContactChangeFailed
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ContactChangeFailed()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2ContactInfoAppView(%x)::ContactChangeFailed()"), this);

    TInt err = KErrNone;
    
    TRAP( err, SetEmptyTextL( R_QTN_PHOB_NO_ITEMS ) );
    if ( err != KErrNone )
        {
        CEikonEnv::Static()->HandleError( err );
        }

    // Rollback
    if ( iViewActivationTransaction )
        {
        // Rollback by force
        TRAP( err, iViewActivationTransaction->RollbackL() );
        delete iViewActivationTransaction;
        iViewActivationTransaction = NULL;

        if ( err != KErrNone )
            {
            CEikonEnv::Static()->HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::Contact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CPbk2ContactInfoAppView::Contact() const
    {
    const MVPbkStoreContact* result = NULL;
    if (iControl)
        {
        result = iControl->FocusedStoreContact();
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandleControlEventL
        ( MPbk2ContactUiControl& /*aControl*/,
        const TPbk2ControlEvent& aEvent)
    {
    switch (aEvent.iEventType)
        {
        case TPbk2ControlEvent::EReady:
            {
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
// CPbk2ContactInfoAppView::ConfigurationChanged
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ConfigurationChanged()
    {
    if (iPreviousViewId.iViewUid != KNullUid)
        {
        TRAPD(err, LoadContactViewL(iPreviousViewId));
        if (err != KErrNone)
            {
            CCoeEnv::Static()->HandleError(err);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ConfigurationChangedComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ConfigurationChangedComplete()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::PreCommandExecutionL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::PreCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    if (iContainer && iContainer->Control())
        {
        iContainer->Control()->HideThumbnail();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::PostCommandExecutionL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::PostCommandExecutionL
        ( const MPbk2Command& /*aCommand*/ )
    {
    if (iContainer && iContainer->Control())
        {
        iContainer->Control()->ShowThumbnail();
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::CreateControlsL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoAppView::CreateControlsL()
    {
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoCreateControlsL);

    if (!iContainer)
        {
        // Create the container and control
        CContainer* container = CContainer::NewLC(this, this, *this);

        container->SetHelpContext( TCoeHelpContext( ApplicationUid(),
            KPHOB_HLP_CONTACT_INFO_VIEW ));

        CPbk2ContactInfoControl* control = CPbk2ContactInfoControl::NewL
            ( container, iContactManager,
              Pbk2AppUi()->ApplicationServices().NameFormatter(),
              Pbk2AppUi()->ApplicationServices().FieldProperties(),
              Pbk2AppUi()->ApplicationServices().StoreProperties(),
              iNavigation );

        container->SetControl(control, ClientRect()); // takes ownership
        control->AddObserverL(*this);

        // Add this view and container to the view stack
        AppUi()->AddToViewStackL(*this, container);
        CleanupStack::Pop(container);
        iContainer = container;
        iControl = control;

        // Assign pointer event inspector
        iPointerInspector =
            AknLayoutUtils::PenEnabled() ? control : NULL;

        iContainer->ActivateL();
        iContainer->Control()->MakeVisible( ETrue );
        }

    PBK2_PROFILE_END(Pbk2Profile::EContactInfoCreateControlsL);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleNavigationEvent
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandleNavigationEvent
        ( const TEventType& aEventType )
    {
    if ( aEventType == MPbk2NavigationObserver::EContactDeleted )
        {
        // Contact was deleted which means that we must go back to previous
        // view. The previous view must know the contact to focus and it
        // obviously can not be the deleted contact. The navigator works
        // in a same logic as the focus in the previous view. It knows the
        // the contact after deletion.
        TRAPD( err,
            {
            MVPbkContactLink* curContact = CurrentContactInNavigatorLC();
            CleanupStack::Pop(); // curContact
            // Give ownership of curContact
            ReturnToPreviousViewL( curContact );
            });
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ViewLoaded
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ViewLoaded( MVPbkContactViewBase& aView )
    {
    __ASSERT_DEBUG(iContactLoader && iControlState,
        Panic(EPanicPreCond_ViewLoaded));

    if ( &aView == iContactView )
        {
        TRAPD( error, HandleViewLoadedL() );
        if ( error != KErrNone )
            {
            ViewError( aView, error );
            }
        }
    delete iViewLoader;
    iViewLoader = NULL;

    PBK2_PROFILE_END(Pbk2Profile::EContactInfoStartTime);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ViewError
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ViewError
        ( MVPbkContactViewBase& aView, TInt /*aError*/ )
    {
    delete iViewLoader;
    iViewLoader = NULL;

    if ( &aView == iContactView )
        {
        TRAPD( error, iViewActivationTransaction->RollbackL() );
        if ( error != KErrNone )
            {
            CCoeEnv::Static()->HandleError(error);
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::UpdateViewStateL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoAppView::UpdateViewStateL
        ( const TUid aCustomMessageId,
        const TDesC8& aCustomMessage)
    {
    CPbk2ViewState* viewState = NULL;
    if (aCustomMessageId == TUid::Uid(KPbkViewStateUid))
        {
        // handle legacy view state
        CPbk2ViewStateTransformer* transformer =
            CPbk2ViewStateTransformer::NewLC( iContactManager );
        viewState = transformer->
                TransformLegacyViewStateToPbk2ViewStateLC( aCustomMessage );
        CleanupStack::Pop();
        CleanupStack::PopAndDestroy(transformer);
        }
    else if (aCustomMessageId == CPbk2ViewState::Uid())
        {
        // Read the desired UI state from aCustomMessage
        viewState = CPbk2ViewState::NewL(aCustomMessage);
        }
    else
        {
        // a parameter must be passed to this view
        User::Leave(KErrNotSupported);
        }

    if (iControlState)
        {
        // Merge parameter view state with the stored state
        if ( viewState->FocusedContact() &&
            !viewState->FocusedContact()->IsSame
                ( *iControlState->FocusedContact() ))
            {
            iControlState->SetFocusedContact(
                    viewState->TakeFocusedContact());
            iControlState->SetFocusedFieldIndex(KErrNotFound);
            iControlState->SetTopFieldIndex(KErrNotFound);
            }
        if (viewState->FocusedFieldIndex() >= 0)
            {
            iControlState->SetFocusedFieldIndex(
                    viewState->FocusedFieldIndex());
            }
        if (viewState->TopFieldIndex() >= 0)
            {
            iControlState->SetTopFieldIndex(
                    viewState->TopFieldIndex());
            }
        if ( viewState->ParentContact() )
            {
            iControlState->SetParentContact(
                viewState->TakeParentContact());
            }
        // Delete parameter view state
        delete viewState;
        }
    else
        {
        // No stored state, use the parameter supplied one
        iControlState = viewState;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::SetNavigatorViewL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::SetNavigatorViewL()
    {
    __ASSERT_DEBUG(iNavigation,
        Panic(EPanicPreCond_SetNavigatorViewL));

    // Set default navigation
    iNavigation->SetScrollViewL( iContactView, EFalse );

    // Inspect if the parent contact is set and if the parent
    // contact is expandable. In that case set the navigation
    // view to the expandable view.
    const MVPbkContactLink* parentLink = iControlState->ParentContact();
    if ( parentLink )
        {
        // Find parent index in the contact view
        const TInt parentIndex =
            iContactView->IndexOfLinkL( *parentLink );
        // Get the view contact by view index
        const MVPbkViewContact& viewContact =
            iContactView->ContactAtL( parentIndex );

        // Expand the scroll view from the view contact
        MVPbkExpandable* expandable = viewContact.Expandable();
        if ( expandable )
            {
            MVPbkContactViewBase* scrollView = expandable->ExpandLC
                ( *iNavigation, Pbk2AppUi()->ApplicationServices().
                    SortOrderManager().SortOrder() );
            CleanupStack::Pop(); // scrollView

            // Set the navigation scroll view, transfer ownership
            iNavigation->SetScrollViewL( scrollView, ETrue );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::StoreStateL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::StoreStateL()
    {
    CPbk2ViewState* state = ViewStateLC();
    CleanupStack::Pop(); // state

    // state should be always created, but valid state
    // has focusedContact
    if ( state && state->FocusedContact() )
        {
        delete iControlState;
        iControlState = state;
        }
    else
        {
        delete state;
        state = NULL;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::IsFocusedFieldTypeL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactInfoAppView::IsFocusedFieldTypeL( TInt aSelectorResId )
    {
    TPbk2StoreContactAnalyzer analyzer( iContactManager, NULL );
    const MVPbkBaseContactField* field = iContainer->Control()->FocusedField();

    if ( field )
        {
        return analyzer.IsFieldTypeIncludedL( *field, aSelectorResId );
        }
    else
        {
        return EFalse;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::UpdateCbasL()
    {
    if ( iControl )
        {
        // Set cba command set "Options - Context Menu - Back"
        Cba()->SetCommandSetL( R_PBK2_SOFTKEYS_OPTIONS_BACK_CONTEXT );
        Cba()->DrawDeferred();
        // Set context menu
        MenuBar()->SetContextMenuTitleResourceId
            ( R_PHONEBOOK2_CONTACTINFO_CONTEXTBAR );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::AttachAIWProvidersL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactInfoAppView::AttachAIWProvidersL()
    {
    PBK2_PROFILE_START(Pbk2Profile::EContactInfoAttachAIWProvidersL);

    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdCall, R_PHONEBOOK2_CALL_MENU,
        R_PHONEBOOK2_CALLUI_AIW_INTEREST, ETrue );
    
    iCommandHandler->RegisterAiwInterestL
        (KAiwCmdCall, R_PHONEBOOK2_CALL_CONTEXT_MENU,
        R_PHONEBOOK2_CALLUI_AIW_INTEREST, EFalse);

    iCommandHandler->RegisterAiwInterestL
        ( KAiwCmdPoC, R_PHONEBOOK2_POC_MENU,
        R_PHONEBOOK2_POCUI_AIW_INTEREST, ETrue );

    iCommandHandler->RegisterAiwInterestL
        (KAiwCmdPoC, R_PHONEBOOK2_POC_CONTEXT_MENU,
        R_PHONEBOOK2_POCUI_AIW_INTEREST, EFalse);

    PBK2_PROFILE_END(Pbk2Profile::EContactInfoAttachAIWProvidersL);
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleLongTapEventL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandleLongTapEventL
        ( const TPoint& /*aPenEventLocation*/,
        const TPoint& /*aPenEventScreenLocation*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::CurrentContactInNavigatorLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPbk2ContactInfoAppView::CurrentContactInNavigatorLC()
    {
    if ( iNavigation )
        {
        return iNavigation->CurrentContactLC();
        }
    else
        {
        MVPbkContactLink* link = NULL;
        // LC semantics also in NULL case.
        CleanupStack::PushL( link );
        return link;
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ReturnToPreviousViewL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ReturnToPreviousViewL
        ( MVPbkContactLink* aFocusedContact ) const
    {
    // Ownership of aFocusedContact is taken by this function
    CleanupDeletePushL( aFocusedContact );

    CPbk2ViewState* state = ViewStateLC();
    CleanupStack::Pop( state );

    // aFocusedContact contact overwrites the focused contact from control's
    // state.
    if ( aFocusedContact )
        {
        // state takes the ownership if aFocusedContact
        state->SetFocusedContact( aFocusedContact );
        CleanupStack::Pop(); // aFocusedContact
        }
    else
        {
        CleanupStack::PopAndDestroy(); // aFocusedContact
        }

    CleanupStack::PushL( state );

    Pbk2AppUi()->Pbk2ViewExplorer()->ActivatePreviousViewL( state );
    CleanupStack::PopAndDestroy( state );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::LoadContactViewL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::LoadContactViewL
        ( const TVwsViewId& aPrevViewId )
    {
    delete iViewLoader;
    iViewLoader = NULL;

    if ( Pbk2AppUi()->Pbk2ViewExplorer()->
            IsPhonebook2View( aPrevViewId ) &&
            ( aPrevViewId.iViewUid == TUid::Uid
                ( EPbk2GroupMembersListViewId ) ) )
        {
        iContactView = Phonebook2::Pbk2AppUi()->ApplicationServices().
                ViewSupplier().AllGroupsViewL();
        }
    else
        {
        iContactView = Phonebook2::Pbk2AppUi()->ApplicationServices().
                ViewSupplier().AllContactsViewL();
        }

    iViewLoader = CPbk2ViewLoader::NewL( *iContactView, *this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::HandleViewLoadedL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::HandleViewLoadedL()
    {
    SetNavigatorViewL();
    LoadContactL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::LoadContactL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::LoadContactL()
    {
    const MVPbkContactLink* link = iControlState->FocusedContact();
    __ASSERT_DEBUG( link, Panic( EPanicLogic_LoadContactL ) );
    __ASSERT_DEBUG( iContactLoader, Panic( EPanicLogic_LoadContactL ) );

    // Load the contact from view state, this is asynchronous
    iContactLoader->ChangeContactL( *link );
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::ConstructNavigatorL
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::ConstructNavigatorL()
    {
    CVPbkContactStoreUriArray* uriList =
        Pbk2AppUi()->ApplicationServices().StoreConfiguration().
            CurrentConfigurationL();
    CleanupStack::PushL( uriList );
    MVPbkContactStoreList& fullStoreList =
        iContactManager.ContactStoresL();
    for (TInt i = 0; i < uriList->Count(); ++i)
        {
        // Add all the contact stores to the store list
        TVPbkContactStoreUriPtr uri = (*uriList)[i];
        MVPbkContactStore* store = fullStoreList.Find(uri);
        iStoreList.AppendL(store);
        }
    CleanupStack::PopAndDestroy( uriList );

    /// Create contact loader and contact info scroll navigator
    delete iContactLoader;
    iContactLoader = NULL;
    iContactLoader = CPbk2MemoryEntryContactLoader::NewL( *this );

    MVPbkContactStore& storeFromState =
        iControlState->FocusedContact()->ContactStore();
    TBool storeNotInConfiguration = EFalse;
    if ( iStoreList.Find( &storeFromState ) == KErrNotFound )
        {
        storeNotInConfiguration = ETrue;
        // Do not add store into list, opening of this lone
        // store is not navigator's responsibility
        }

    if ( !storeNotInConfiguration )
        {
        iNavigation = Pbk2NavigatorFactory::CreateContactNavigatorL
            ( Id(), *this, *iContactLoader, iStoreList.Array(),
              R_QTN_PHOB_NAVI_POS_INDICATOR_CONTACT_INFO_VIEW );
        }
    else
        {
        // Store was not in configuration --> contact to be shown is not
        // included in the scroll view --> no point to use sideways
        // navigation strategy
        iNavigation = Pbk2NavigatorFactory::CreateVoidNavigatorL
            ( Id(), *this, *iContactLoader, iStoreList.Array() );
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::UpdateViewGraphL
// Sets previous view node. Previous node should be set dynamically
// because it could be some extension view.
// Default is names list view.
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::UpdateViewGraphL
        ( const TVwsViewId& aPrevViewId )
    {
    CPbk2ViewGraph& viewGraph = static_cast<CPbk2ViewExplorer*>
        ( Pbk2AppUi()->Pbk2ViewExplorer() )->ViewGraph();
    CPbk2ViewNode* prevNode =
        viewGraph.FindNodeWithViewId( aPrevViewId.iViewUid );
    if ( prevNode )
        {
        CPbk2ViewNode* thisNode = viewGraph.FindNodeWithViewId( Id() );
        if ( thisNode )
            {
            thisNode->SetPreviousNode( prevNode );
            }
        }
    }

// --------------------------------------------------------------------------
// CPbk2ContactInfoAppView::SetEmptyTextL
// Sets empty text for contact info list box from given resource id.
// --------------------------------------------------------------------------
//
void CPbk2ContactInfoAppView::SetEmptyTextL(
		TInt aResourceId )
	{
    HBufC* emptyText = StringLoader::LoadLC( aResourceId );
    iContainer->Control()->SetTextL( *emptyText );
    CleanupStack::PopAndDestroy( emptyText );
	}


// End of File
