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
* Description:  Phonebook 2 USIM contact info view base class.
*
*/


#include "CPsu2InfoViewBase.h"

// Phonebook 2
#include "CPsu2ViewManager.h"
#include <CPbk2AppUiBase.h>
#include <CPbk2ControlContainer.h>
#include <CPbk2ContactInfoControl.h>
#include <CPbk2UIExtensionView.h>
#include <MPbk2CommandHandler.h>
#include <CPbk2ViewState.h>
#include <CPbk2MemoryEntryContactLoader.h>
#include <MPbk2ViewActivationTransaction.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2ViewExplorer.h>
#include <MPbk2ContactNavigation.h>
#include <Pbk2NavigatorFactory.h>
#include <MPbk2ApplicationServices.h>
#include <csxhelp/phob.hlp.hrh>

// Virtual Phonebook
#include <MVPbkContactViewBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactLink.h>

// System includes
#include <avkon.rsg>
#include <eikbtgpc.h>
#include <AknIconUtils.h>
#include <AknUtils.h>
#include <aknnavi.h>
#include <aknnavide.h>

// Debugging headers
#include <Pbk2Debug.h>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_ContactChangedL = 1
    };

void Panic(TPanicCode aReason)
    {
    _LIT( KPanicText, "CPsu2InfoViewBase" );
    User::Panic( KPanicText, aReason );
    }
#endif // _DEBUG

} /// namespace

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::CPsu2InfoViewBase
// --------------------------------------------------------------------------
//
CPsu2InfoViewBase::CPsu2InfoViewBase
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager):
            iExtensionView( aExtensionView ),
            iViewManager( aViewManager )
    {
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::~CPsu2InfoViewBase()
// --------------------------------------------------------------------------
//
CPsu2InfoViewBase::~CPsu2InfoViewBase()
    {
    delete iNaviDecorator;
    delete iControlState;
    delete iViewActivationTransaction;
    delete iContactLoader;
    delete iNavigation;
    iStoreList.Reset();
    iStoreList.Close();

    if ( iContainer )
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        }

    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::BaseConstructL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::BaseConstructL()
    {
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(iExtensionView.ClientRect());
        }
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPsu2InfoViewBase::ViewStateLC() const
    {
    CPbk2ViewState* state = NULL;
    if ( iControl )
        {
        state = iControl->ControlStateL();
        }

    CleanupStack::PushL(state);
    return state;
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPsu2InfoViewBase::HandleCommandKeyL(
        const TKeyEvent& /*aKeyEvent*/,
        TEventCode /*aType*/)
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::HandlePointerEventL(
        const TPointerEvent& /*aPointerEvent*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPsu2InfoViewBase::GetViewSpecificMenuFilteringFlagsL() const
    {
    TInt flags = 0;
    if ( iControl )
        {
        flags = iControl->GetMenuFilteringFlagsL();
        }
        
    return flags;
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::DoActivateL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::DoActivateL
    ( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
      const TDesC8& aCustomMessage)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2InfoViewBase(%x)::DoActivateL()"), this);
    
    // Navigation view remains the same
    iContactLoader = CPbk2MemoryEntryContactLoader::NewL( *this );
    // Create store list
    iStoreList.Append(&iViewManager.ContactStore());

    iNavigation = Pbk2NavigatorFactory::CreateVoidNavigatorL
        ( iExtensionView.Id(), *this, *iContactLoader, iStoreList.Array() );

    CreateControlsL();

    if ( aCustomMessageId == CPbk2ViewState::Uid() )
        {
        UpdateViewStateL(aCustomMessage);
        const MVPbkContactLink* link = iControlState->FocusedContact();
        if ( link )
            {
            // Load the contact from view state, this is asynchronous
            iContactLoader->ChangeContactL( *link );
            }
        else
            {
            User::Leave( KErrArgument );
            }
        }
    else
        {
        // No UI state specified, restore in previous state
        if ( !iControlState )
            {
            // No previous state
            User::Leave(KErrArgument);
            }
        }

    // Activate control
    iContainer->ActivateL();
    iPreviousViewId = aPrevViewId;

    HBufC* naviPaneLabel = NaviPaneLabelL();
    CleanupStack::PushL( naviPaneLabel );


    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>
            ( iAvkonAppUi->StatusPane()->ControlL
                ( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    if ( iNaviDecorator )
        {
        naviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }

    if ( naviPaneLabel )
        {
        iNaviDecorator = naviPane->CreateNavigationLabelL( *naviPaneLabel );
        }
    else
        {
        iNaviDecorator = naviPane->CreateNavigationLabelL();
        }
    naviPane->PushL( *iNaviDecorator );

    CleanupStack::PopAndDestroy(); // naviPaneLabel

    iViewManager.RegisterStoreAndView();
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::DoDeactivate
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::DoDeactivate()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2InfoViewBase(%x)::DoDeactivate()"), this);
    
    if ( iNaviDecorator )
        {
        CAknNavigationControlContainer* naviPane =
            static_cast<CAknNavigationControlContainer*>
                ( iAvkonAppUi->StatusPane()->ControlL
                    ( TUid::Uid( EEikStatusPaneUidNavi ) ) );

        naviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }

    delete iControlState;
    iControlState = NULL;

    if ( iContainer )
        {
        CCoeEnv::Static()->AppUi()->RemoveFromStack( iContainer );
        // Store current state, safe to ignore. There's no real harm,
        // if theres no stored state when activating this view again
        TRAP_IGNORE(StoreStateL());
        delete iContainer;
        iContainer = NULL;
        // iControl is owned by iContainer and it deleted with container
        iControl = NULL;
        }

    delete iNavigation;
    iNavigation = NULL;
    delete iContactLoader;
    iContactLoader = NULL;
    iStoreList.Reset();

    iViewManager.DeregisterStoreAndView();
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::HandleCommandL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::HandleCommandL( TInt aCommand )
    {
    // No command handling in this class, forward to Commands
    if ( !Phonebook2::Pbk2AppUi()->ApplicationServices().
            CommandHandlerL()->HandleCommandL
                ( aCommand, *iControl, &iExtensionView ) )
        {
        iControl->ProcessCommandL( aCommand );
        Phonebook2::Pbk2AppUi()->HandleCommandL( aCommand );
        }
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    // Ask the control do control specific filtering
    // (for example call HandleMarkableListDynInitMenuPane if needed)
    iControl->DynInitMenuPaneL(aResourceId, aMenuPane);

    // Phonebook 2 menu filtering happens in Commands
    Phonebook2::Pbk2AppUi()->ApplicationServices().CommandHandlerL()->
        DynInitMenuPaneL
            ( aResourceId, aMenuPane, iExtensionView, *iControl );
    }

// -----------------------------------------------------------------------------
// CPsu2InfoViewBase::HandleLongTapEventL
// -----------------------------------------------------------------------------
//
void CPsu2InfoViewBase::HandleLongTapEventL
        ( const TPoint& /*aPenEventLocation*/,
          const TPoint& /*aPenEventScreenLocation*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::HandleControlEventL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::HandleControlEventL
        ( MPbk2ContactUiControl& /*aControl*/,
          const TPbk2ControlEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EReady:
            {
            UpdateCbasL();
            RestoreStateL();
            if ( iContainer )
                {
                iContainer->Control()->DrawNow();                
                }
            break;
            }
        case TPbk2ControlEvent::EContactSetChanged:
            {
            if ( iContainer )
                {
                iContainer->Control()->DrawNow();
                }            
            break;
            }
        case TPbk2ControlEvent::EItemRemoved:
            {
            // Our contact got deleted, return back
            ReturnToPreviousViewL( NULL );
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
// CPsu2InfoViewBase::PrepareForContactChangeL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::PrepareForContactChangeL()
    {
    StoreStateL();
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::ContactChangedL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::ContactChangedL( MVPbkStoreContact* aContact )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2InfoViewBase(%x)::ContactChangedL() begin"), this);

    __ASSERT_DEBUG(iContainer, Panic(EPanicPreCond_ContactChangedL));
    __ASSERT_DEBUG(iViewActivationTransaction,
        Panic(EPanicPreCond_ContactChangedL));

    if ( !iStore )
        {
        // Register to store to get delete event
        iStore = &aContact->ParentStore();
        }

    // Update controls
    if (aContact)
        {
        CleanupDeletePushL(aContact);
        iContainer->Control()->UpdateL(aContact);
        CleanupStack::Pop(aContact);    // ownership was taken
        }

    iContainer->Control()->RestoreControlStateL(iControlState);

    // Commit application-wide state changes
    iViewActivationTransaction->Commit();

    // DrawDeferred needed here to refresh the control contents
    // in the store contact has been set for it for the first time.
    iContainer->Control()->DrawDeferred();

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2InfoViewBase(%x)::ContactChangedL() end"), this);
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::ContactChangeFailed
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::ContactChangeFailed()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2InfoViewBase(%x)::ContactChangeFailed()"), this);

    // Rollback
    if ( iViewActivationTransaction )
        {
        // Rollback by force
        TRAPD( err, iViewActivationTransaction->RollbackL() );
        delete iViewActivationTransaction;
        iViewActivationTransaction = NULL;

		if ( err != KErrNone )
            {
            CEikonEnv::Static()->HandleError( err );
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::Contact
// --------------------------------------------------------------------------
//
const MVPbkStoreContact* CPsu2InfoViewBase::Contact() const
    {
    const MVPbkStoreContact* result = NULL;
    if ( iContainer && iContainer->Control() )
        {
        result = iContainer->Control()->FocusedStoreContact();
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::HandleNavigationEvent
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::HandleNavigationEvent
        ( const TEventType& aEventType )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2InfoViewBase(%x)::HandleNavigationEvent() EventType=%d"), 
         this, aEventType );
    
    if (aEventType == MPbk2NavigationObserver::EContactDeleted)
        {
        TRAPD( err,
            {
            MVPbkContactLink* curContact = CurrentContactInNavigatorLC();
            CleanupStack::Pop(); // curContact
            // Give ownership of curContact
            ReturnToPreviousViewL( curContact );
            });
        if ( err != KErrNone )
            {
            CCoeEnv::Static()->HandleError(err);
            }         
        }
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::CreateControlsL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::CreateControlsL()
    {
    if (!iContainer)
        {
        // Create the container and control
        CContainer* container = CContainer::NewLC(
            &iExtensionView,
            &iExtensionView,
            iExtensionView);

        container->SetHelpContext(
            TCoeHelpContext( iExtensionView.ApplicationUid(),
                             KPHOB_HLP_NAME_LIST ));

        MPbk2ApplicationServices& appServices =
            Phonebook2::Pbk2AppUi()->ApplicationServices();

        CPbk2ContactInfoControl* control = CPbk2ContactInfoControl::NewL
            ( container, appServices.ContactManager(),
              appServices.NameFormatter(), appServices.FieldProperties(),
              appServices.StoreProperties(), NULL );

        // takes ownership of the control
        container->SetControl(control, iExtensionView.ClientRect());
        control->AddObserverL(*this);

        CCoeEnv::Static()->AppUi()->AddToStackL(iExtensionView, container);
        CleanupStack::Pop(container);
        iContainer = container;
        iControl = control;
        iPointerEventInspector = control;
        }
    }

// --------------------------------------------------------------------------
// CPsu2InfoViewBase::UpdateViewStateL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::UpdateViewStateL( const TDesC8& aCustomMessage )
    {
    // Read the desired UI state from aCustomMessage
    CPbk2ViewState* viewState = CPbk2ViewState::NewL(aCustomMessage);

    if (iControlState)
        {
        // Merge parameter view state with the stored state
        if (viewState->FocusedContact() != iControlState->FocusedContact())
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
        if (viewState->ParentContact() != iControlState->ParentContact())
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
// CPsu2InfoViewBase::StoreStateL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::StoreStateL()
    {
    CPbk2ViewState* state = NULL;
    if ( iControl )
        {
        state = iControl->ControlStateL();    
        }    

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
// CPsu2InfoViewBase::RestoreStateL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::RestoreStateL()
    {
    if ( iControl )
        {
        iControl->RestoreControlStateL(iControlState);
        delete iControlState;
        iControlState = NULL;        
        }
    }
    
// --------------------------------------------------------------------------
// CPsu2InfoViewBase::CurrentContactInNavigatorLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CPsu2InfoViewBase::CurrentContactInNavigatorLC()
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
// CPsu2InfoViewBase::ReturnToPreviousViewL
// --------------------------------------------------------------------------
//
void CPsu2InfoViewBase::ReturnToPreviousViewL(
        MVPbkContactLink* aFocusedContact ) const
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

    Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->ActivatePhonebook2ViewL( 
        iPreviousViewId.iViewUid, state );
    CleanupStack::PopAndDestroy( state );
    }

// End of File
