/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*          Provides phonebook Names List View controller methods
*
*/


// INCLUDE FILES
// This class' declaration
#include "CPbkNamesListAppView.h"

#include <apgcli.h>
#include <eikmenup.h>
#include <StringLoader.h>            // StringLoader
#include <Phonebook.hrh>
#include <SharedDataClient.h>

// Send UI
#include <sendui.h>                  // Send UI API
#include <sendnorm.rsg>              // for Send UI functionality
#include <SenduiMtmUids.h>           // Send UI MTM uid's

// Phonebook.app include files
#include <Phonebook.rsg>             // Phonebook resources
#include "CPbkAppUi.h"               // Phonebook application UI class
#include "CPbkDeleteContactsCmd.h"
#include "CPbkAssignToGroupCmd.h"
#include "CPbkDocument.h"
#include "CPbkAppGlobals.h"
#include "CPbkMenuFiltering.h"
#include "PbkUid.h"
#include "PbkApplicationLauncherFactory.h"
#include "MPbkApplicationLauncher.h"
#include "CPbkDbRecoveryUi.h"

// Phonebook engine include files
#include <CPbkContactEngine.h>       // Phonebook engine
#include <CPbkFieldsInfo.h>
#include <CPbkContactItem.h>
#include <PbkCommandHandler.h>
#include <CPbkConstants.h>
#include <CPbkAiwInterestArray.h>

// Phonebook view include files
#include <PbkView.rsg>               // PbkView dll resources
#include <CPbkViewState.h>           // Phonebook view state object
#include <CPbkContactViewListControl.h>
#include <CPbkAddToGroup.h>
#include <CPbkContactEditorDlg.h>
#include <CPbkDeleteContactQuery.h>
#include <CPbkPhonebookInfoDlg.h>
#include <CPbkFFSCheck.h>

// From PbkUI
#include <CPbkControlContainer.h>    // Control container
#include <MPbkCommand.h>
#include <MPbkCommandFactory.h>
#include <MenuFilteringFlags.h>

// PbkExtension classes
#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>
#include <MPbkViewExtension.h>
#include <cshelp/phob.hlp.hrh>

// Debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

// Sufficient amount of memory for contact database modification
const TInt KFreeSpaceForDbCompress = 128*1024; // 128kb

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPreCond_HandleCommandL=1,
    EPanicPostCond_DoActivateL,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPreCond_CmdCreateNewL,
    EPanicPreCond_CmdEditMeL,
    EPanicPreCond_CmdAddToGroupL,
    EPanicPreCond_CmdDuplicateMeL,
    EPanicPreCond_DynInitMenuPaneL,
    EPanicPreCond_CmdDeleteMeL,
    EPanicPreCond_HandleContactViewListControlEventL
    };
#endif // _DEBUG


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkNamesListAppView");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG


}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkNamesListAppView::CPbkNamesListAppView()
    : iContactToFocus(KNullContactId)
    {
    }

inline void CPbkNamesListAppView::ConstructL()
    {
    // PreCond
    __ASSERT_DEBUG(!iContainer, Panic(EPanicPreCond_ConstructL));

    // Init base class
    __PBK_PROFILE_START(PbkProfiling::EViewBaseConstruct);
    BaseConstructL(R_PHONEBOOK_NAMESLIST_APPVIEW);
    __PBK_PROFILE_END(PbkProfiling::EViewBaseConstruct);

    iExtGlobal = CPbkExtGlobals::InstanceL();

    // Do not create the container control here: defer creation to first
    // activation of the view (first DoActivateL call). This saves a lot
    // of memory and application starting time, because all the application
    // views (classes like this) must be created at startup. Controls will
    // be created on-demand as the views are accessed.

    // Create recovery UI here instead AppUI so that if recovery
    // fails and we need exit we can set this view container not focusing
    // to prevent focus change event
    iDbRecoveryUi = CPbkDbRecoveryUi::NewL( *Engine(), *this );

    // PostCond
    __ASSERT_DEBUG(!iContainer, Panic(EPanicPostCond_ConstructL));
    }

CPbkNamesListAppView* CPbkNamesListAppView::NewL()
    {
    CPbkNamesListAppView* self = new(ELeave) CPbkNamesListAppView();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

CPbkNamesListAppView::~CPbkNamesListAppView()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkNamesListAppView(%x)::~CPbkNamesListAppView()"), this);

    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        }
    if (iAllContactsView)
        {
        iAllContactsView->Close(*this);
        }
    delete iDbRecoveryUi;
    delete iControlState;
    delete iParamState;
    Release(iViewExtension);
    Release(iExtGlobal);
    delete iPbkFFSCheck;
    if (iSharedDataClient)
        {
        iSharedDataClient->Close();
        delete iSharedDataClient;
        }
    }

void CPbkNamesListAppView::CmdCreateNewL()
    {
    __ASSERT_DEBUG(iContainer && iContainer->Control(),
                   Panic(EPanicPreCond_CmdCreateNewL));
    CPbkContactViewListControl& control = *iContainer->Control();

    // Create a new contact
    CPbkContactItem* newContact = Engine()->CreateEmptyContactL();
    CleanupStack::PushL(newContact);

    TInt fieldToFocus = -1;
    TBool edited = EFalse;

    // If there is text in the find box, put it in the contact as last name
    const TDesC& findText = control.FindTextL();
    if (findText.Length() > 0)
        {
        TPbkContactItemField* lastNameField = newContact->AddOrReturnUnusedFieldL
            (*Engine()->FieldsInfo().Find(EPbkFieldIdLastName));
        if (lastNameField)
            {
            lastNameField->TextStorage()->SetTextL(findText);
            fieldToFocus = newContact->FindFieldIndex(*lastNameField);
            edited = ETrue;
            }
        }

    // blank control to prevent name list thumbnail update
    // during editing
    control.SetBlank( ETrue );
    control.HideThumbnail();
    iContainer->SetFocusing(EFalse);
    iContainer->Control()->SetFocusing( EFalse );

    // Trap this, so that the thumbnail doesnt get hidden in case of leave
    TRAPD(err, DoOpenContactEditorDlgL(control, *newContact,
        ETrue, fieldToFocus, edited));

    iContainer->Control()->SetFocusing( ETrue );
    iContainer->SetFocusing(ETrue);
    control.SetBlank( EFalse );
    control.ShowThumbnailL();

    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy();  // newContact
    }

void CPbkNamesListAppView::DoOpenContactEditorDlgL
            (CPbkContactViewListControl& aControl,
            CPbkContactItem& aNewContact, TBool aIsNewContact,
            TInt aFieldToFocus/*=-1*/, TBool aEdited /*=EFalse*/)
    {
    // Run the editor dialog
    CPbkContactEditorDlg* editorDlg = CPbkContactEditorDlg::NewL
        (*Engine(), aNewContact, aIsNewContact, aFieldToFocus, aEdited);
    editorDlg->SetExitCommandId(EPbkCmdExit);
    const TContactItemId newContactId = editorDlg->ExecuteLD();

    // Clear Find box contents and markings, because at the UI spec level
    // a view switch occured above from Names List view to New Memory Entry
    // view.
    aControl.ResetFindL();
    aControl.HandleMarkableListUpdateAfterCommandExecution();

    if (newContactId != KNullContactId)
        {
        // Move focus to the newly created contact in contact view event handler
        iContactToFocus = newContactId;
        }
    }

void CPbkNamesListAppView::CmdDeleteMeL()
    {
    CPbkContactViewListControl& control = *iContainer->Control();
    __ASSERT_DEBUG(iContainer && control.NumberOfItems() > 0,
        Panic(EPanicPreCond_CmdDeleteMeL));

    if (control.ItemsMarked())
        {
        // Items marked
        const CContactIdArray& markedItems = control.MarkedItemsL();

        if (markedItems.Count() == 1)
            {
            // One item marked
            DoDeleteContactL(markedItems[0]);
            }
        else if (markedItems.Count() > 1)
            {
            // Multiple items marked
            DoDeleteMarkedContactsL(markedItems);
            }
        }
    else
        {
        // No marked items -> delete focused contact
        DoDeleteContactL(control.FocusedContactIdL());
        }
    }

void CPbkNamesListAppView::CmdEditMeL()
    {
    __ASSERT_DEBUG(
        iContainer &&
        iContainer->Control()->NumberOfItems() > 0 &&
        iContainer->Control()->CurrentItemIndex() >= 0 &&
        !iContainer->Control()->ItemsMarked(),
        Panic(EPanicPreCond_CmdEditMeL));

    // Get focused contact's ID
    const TContactItemId focusId = iContainer->Control()->FocusedContactIdL();

    // Open editor for the focused contact
    iContainer->Control()->SetFocusing( EFalse );
    TRAPD( err , DoEditL(focusId) );
    iContainer->Control()->SetFocusing( ETrue );
    User::LeaveIfError( err );
    }

void CPbkNamesListAppView::CmdAddToGroupL()
    {
    __ASSERT_DEBUG(
        iContainer &&
        iContainer->Control()->NumberOfItems() > 0 &&
        Engine()->Database().GroupCount() > 0,
        Panic(EPanicPreCond_CmdAddToGroupL));

    CPbkContactViewListControl& control = *iContainer->Control();

    // create parameters
    CContactIdArray* idArray = NULL;
    if (control.ItemsMarked())
        {
        const CContactIdArray& markedIdArray = control.MarkedItemsL();
        idArray = CContactIdArray::NewLC(&markedIdArray);
        }
    else
        {
        const TContactItemId focusedContactId = control.FocusedContactIdL();
        idArray = CContactIdArray::NewLC();
        idArray->AddL(focusedContactId);
        }

    CPbkAddToGroup* addToGroup = CPbkAddToGroup::NewL();
    TContactItemId groupId = addToGroup->ExecuteLD(*Engine());
    if (groupId != KNullContactId)
        {
        CPbkAssignToGroupCmd* cmd = CPbkAssignToGroupCmd::NewL
            (*Engine(), *idArray, *iContainer->Control(), groupId);
        PbkAppUi()->AddAndExecuteCommandL(cmd);
        }

    CleanupStack::PopAndDestroy(idArray);
    }

void CPbkNamesListAppView::CmdDuplicateMeL()
    {
    __ASSERT_DEBUG(
        iContainer &&
        iContainer->Control()->NumberOfItems() > 0 &&
        !iContainer->Control()->ItemsMarked(),
        Panic(EPanicPreCond_CmdDuplicateMeL));

    const TContactItemId newContactId = Engine()->DuplicateContactL
        (iContainer->Control()->FocusedContactIdL(), ETrue);

    iContainer->Control()->SetFocusing( EFalse );
	TRAPD( err , DoEditL(newContactId) );
	iContainer->Control()->SetFocusing( ETrue );
    User::LeaveIfError( err );
    }

inline void CPbkNamesListAppView::CmdContextMenuL()
    {
    if (iContainer->Control()->ItemsMarked())
        {
        LaunchPopupMenuL(R_PBK_NAMESLIST_CONTEXT_MENUBAR_ITEMS_MARKED);
        }
    else
        {
        LaunchPopupMenuL(R_PBK_NAMESLIST_CONTEXT_MENUBAR);
        }
    }

inline void CPbkNamesListAppView::CmdOpenSettingsL()
    {
    // Hide the control
    iContainer->Control()->MakeVisible(EFalse);
    iContainer->SetFocusing(EFalse);

    // Trap this, so that the thumbnail doesnt get hidden in case of leave
    TRAPD(err, DoOpenSettingsDlgL());

    iContainer->SetFocusing(ETrue);
    // Restore control
    iContainer->Control()->MakeVisible(ETrue);

    // In case of an error, leave
    if (err != KErrNone)
        {
        User::Leave(err);
        }
    }

inline void CPbkNamesListAppView::DoOpenSettingsDlgL()
    {
    // Create the settings command object
    MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
        CommandFactory().CreateSettingsCmdL(*Engine(), *this);
    // Execute the command
    cmd->ExecuteLD();

    // Clear Find box contents and markings
    CPbkContactViewListControl& control = *iContainer->Control();
    control.ResetFindL();
    control.HandleMarkableListUpdateAfterCommandExecution();
    }

/**
 * Command handler: go to URL command.
 */
void CPbkNamesListAppView::CmdGoToURLL()
    {
    // Get the focused contact item
    const TContactItemId focusId = iContainer->Control()->FocusedContactIdL();
    CPbkContactItem* focusItem = Engine()->ReadContactLC(focusId);

    // Create the go to URL command object
    MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
        CommandFactory().CreateGoToURLCmdL(*focusItem, NULL);
    // Execute the command
    cmd->ExecuteLD();

    CleanupStack::PopAndDestroy(); // focusItem
    }

void CPbkNamesListAppView::CmdOpenOneTouchL()
    {
    MPbkApplicationLauncher* appLauncher =
        PbkApplicationLauncherFactory::CreateLC();
    appLauncher->LaunchApplicationL(TUid::Uid(KSpeedDialUid));
    CleanupStack::PopAndDestroy(); // appLauncher
    }

inline void CPbkNamesListAppView::CmdPhonebookInfoL()
    {
    CPbkContactViewListControl& control = *iContainer->Control();

    // Hide thumbnail
    control.HideThumbnail();
    iContainer->SetFocusing(EFalse);
    iContainer->CoeControl()->SetFocusing(EFalse);

    // Trap this, so that the thumbnail doesnt get hidden in case of leave
    TRAPD(err, DoShowPhonebookInfoL());

    // Restore
    iContainer->SetFocusing(ETrue);
    iContainer->CoeControl()->SetFocusing(ETrue);
    control.ShowThumbnailL();

    // In case of an error, leave
    if (err != KErrNone)
        {
        User::Leave(err);
        }
    }

inline void CPbkNamesListAppView::DoShowPhonebookInfoL()
    {
    CPbkPhonebookInfoDlg* infoPopup =
        CPbkPhonebookInfoDlg::NewL(*Engine());
    infoPopup->ExecuteLD();
    }

inline TBool CPbkNamesListAppView::NoOpenOneTouch() const
    {
    TBool ret = ETrue;
    if(Engine()->Constants()->
        LocallyVariatedFeatureEnabled(EPbkLVAllowOpen1Touch))
        {
        ret = EFalse;
        }
    return ret;
    }

CPbkContactViewListControl& CPbkNamesListAppView::Control()
    {
    return *(iContainer->Control());
    }

void CPbkNamesListAppView::DeleteFocusedEntryL()
    {
    CmdDeleteMeL();
    }

CPbkViewState* CPbkNamesListAppView::GetViewStateLC() const
    {
    return iContainer ? iContainer->Control()->GetStateLC() : NULL;
    }

void CPbkNamesListAppView::HandleCommandL(TInt aCommandId)
    {
    // PreCond:
    __ASSERT_DEBUG(iContainer && iViewExtension,
        Panic(EPanicPreCond_HandleCommandL));

    if (iViewExtension->HandleCommandL(aCommandId, *this))
        {
        return;
        }

    // Process view-specific commands
    switch (aCommandId)
        {
        case EPbkCmdCreateNew:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkNamesListAppView::CmdCreateNewL));
            break;
            }

        case EPbkCmdDeleteMe:
            {
            CmdDeleteMeL();
            break;
            }

        case EPbkCmdEditMe:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkNamesListAppView::CmdEditMeL));
            break;
            }

        case EPbkCmdAddToGroup:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkNamesListAppView::CmdAddToGroupL));
            break;
            }

        case EPbkCmdDuplicateMe:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkNamesListAppView::CmdDuplicateMeL));
            break;
            }

        case EPbkCmdGoToURL:
            {
            CmdGoToURLL();
            break;
            }

        case EPbkCmdSettings:
            {
            CmdOpenSettingsL();
            break;
            }

        case EPbkCmdOpenOneTouch:
            {
            CmdOpenOneTouchL();
            break;
            }

        case EPbkCmdContextMenu:
            {
            CmdContextMenuL();
            break;
            }

        case EPbkCmdPhonebookInfo:
            {
            CmdPhonebookInfoL();
            break;
            }
        case EPbkCmdSendUICommand:
            {
            // Set up flags depending on current UI state
            TUint flags = 0;
            if (iContainer->Control()->ItemsMarked())
                {
                flags |= KPbkItemsMarked;
                }
            HandleSendContactsL( flags );
            break;
            }
        case EPbkCmdExit:
			{
			// make sure that the visible control is not set blank
			Control().SetBlank(EFalse);
			// fall through
            }
        default:
            {
            // Command not handled here, forward it to base class
            CPbkContactListAppView::HandleCommandL(aCommandId);
            break;
            }
        }
    }

TUid CPbkNamesListAppView::Id() const
    {
    return CPbkAppUi::KPbkNamesListViewUid;
    }

void CPbkNamesListAppView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

void CPbkNamesListAppView::DoActivateL
        (const TVwsViewId& aPrevViewId,
         TUid aCustomMessageId,
         const TDesC8& aCustomMessage)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkNamesListAppView(%x)::DoActivateL()"),
        this);

    // Load the view status bar title
    HBufC* title = StringLoader::LoadLC(R_QTN_PHOB_TITLE);

    // Update application-wide state leave-safely
    CPbkAppUi::CViewActivationTransaction* viewActivationTransaction =
        PbkAppUi()->HandleViewActivationLC (Id(), aPrevViewId, title, NULL);

    // Check activation parameters
    delete iParamState;
    iParamState = NULL;
    if (aCustomMessageId == CPbkViewState::Uid())
        {
        iParamState = CPbkViewState::NewL(aCustomMessage);
        }

    if (!iAllContactsView)
        {
        iAllContactsView = &Engine()->AllContactsView();
        iAllContactsView->OpenL(*this);
        }

    if (!iContainer)
        {
        // Create the container and control
        CContainer* container = CContainer::NewLC(this, *this);

        container->SetHelpContext
            (TCoeHelpContext(ApplicationUid(), KPHOB_HLP_NAME_LIST));

        if (!iViewExtension)
            {
            iViewExtension = iExtGlobal->FactoryL().
                    CreatePbkViewExtensionL
                            (Id(), *PbkAppUi()->PbkDocument()->Engine(), *this);
            }

        // Create the view-side UI control and connect it to the container
        CPbkContactViewListControl* control = CPbkContactViewListControl::NewL(
            *Engine(), *iAllContactsView, R_PBK_NAMES_LIST_CONTROL, container);

        container->SetControl(control, ClientRect());
        control->AddObserverL(*this);
        iViewExtension->SetContactUiControl(control);

        AppUi()->AddToStackL(*this,container);
        CleanupStack::Pop(container);
        iContainer = container;
        }

    // Activate control
    iContainer->ActivateL();

    // Commit application-wide state changes
    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(2);  // viewActivationTransaction, title
    __PBK_PROFILE_END(PbkProfiling::ENamesListViewActivateControl);
    if (iParamState && iContainer->Control()->IsReady())
        {
        iContainer->Control()->RestoreStateL(iParamState);
        }

    // The application is set here to background if application exit occurs
    if (iParamState && iParamState->Flags() & CPbkViewState::ESendToBackground)
        {
        PbkAppUi()->SendAppToBackgroundL();
        iContainer->Control()->DeleteThumbnail();
        }

    __PBK_PROFILE_END(PbkProfiling::ENamesListViewDoActivateL);

    __PBK_PROFILE_START(PbkProfiling::ENamesListContactViewOpen);

    // PostCond:
    __ASSERT_DEBUG(iContainer && iContainer->Control(),
        Panic(EPanicPostCond_DoActivateL));
    }

void CPbkNamesListAppView::DoDeactivate()
    {
    PBK_DEBUG_PRINT(
        PBK_DEBUG_STRING("CPbkNamesListAppView(%x)::DoDeactivate()"), this);

    delete iParamState;
    iParamState = NULL;
    delete iControlState;
    iControlState = NULL;

    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        TRAP_IGNORE(iControlState = iContainer->Control()->GetStateL(EFalse));
        delete iContainer;
        iContainer = NULL;
        }

    if (iViewExtension)
        {
        iViewExtension->SetContactUiControl(NULL);
        }
    }

void CPbkNamesListAppView::DynInitMenuPaneL
        (TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    __ASSERT_DEBUG(iContainer && iViewExtension,
        Panic(EPanicPreCond_DynInitMenuPaneL));

    // Set up flags depending on current UI state
    TUint flags = 0;
    if (iContainer->Control()->NumberOfItems() == 0)
        {
        flags |= KPbkControlEmpty;
        }
    if (iContainer->Control()->ItemsMarked())
        {
        flags |= KPbkItemsMarked;
        }
    if (Engine()->Database().GroupCount() == 0)
        {
        flags |= KPbkNoGroups;
        }
    if (NoOpenOneTouch())
        {
        flags |= KPbkNoOpenOneTouch;
        }

    // Offer the menu to AIW first
    if (iAiwInterestArray->InitMenuPaneL(aResourceId, *aMenuPane, flags))
        {
        // No need to continue
        return;
        }

    // Then to the extensions
    iViewExtension->DynInitMenuPaneL(aResourceId, aMenuPane);

    switch (aResourceId)
        {
        case R_PHONEBOOK_NAMESLIST_MENU:
            {
            if ( !Engine()->Constants()->
                LocallyVariatedFeatureEnabled( EPbkLVGroupManagementSend ) )
                {
                flags |= KPbkNoGroupSend;
                }
            NamesListPaneMenuFilteringL(*aMenuPane, flags);
            break;
            }
        case R_PHONEBOOK_INFO_MENU_PANE:
            {
            PhonebookInfoPaneMenuFilteringL(*aMenuPane, flags);
            break;
            }
        case R_PBK_NAMESLIST_CONTEXTMENU_MARKED_ITEMS:
            {
            ContextMarkedItemsPaneMenuFilteringL(*aMenuPane, flags);
            break;
            }
        case R_PHONEBOOK_SETTINGS_MENU:
            {
            SettingsPaneMenuFilteringL(*aMenuPane, flags);
            break;
            }
        default:
            {
            break;
            }
        }
    CPbkContactListAppView::DynInitMenuPaneL(aResourceId, aMenuPane);
    }


void CPbkNamesListAppView::HandleForegroundEventL(TBool aForeground)
    {
    CPbkContactListAppView::HandleForegroundEventL(aForeground);

    // If this view is gaining foreground, then make sure that thumbnail
    // gets drawn
    if (aForeground && iContainer && iContainer->Control())
        {
        iContainer->Control()->ShowThumbnailL();
        }
    }


void CPbkNamesListAppView::NamesListPaneMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags)
    {
    if ((aFlags & KPbkControlEmpty) || (aFlags & KPbkItemsMarked))
        {
        aMenuPane.SetItemDimmed(EPbkCmdOpenMeViews, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdEditMe, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdDuplicateMe, ETrue);
        }

    if ((aFlags & KPbkControlEmpty) ||
        ((aFlags & KPbkItemsMarked) && (aFlags & KPbkNoGroupSend)))
        {
        aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
        }

    if ((aFlags & KPbkControlEmpty) || (aFlags & KPbkNoGroups))
        {
        aMenuPane.SetItemDimmed(EPbkCmdAddToGroup, ETrue);
        }

    if ((aFlags & KPbkControlEmpty) || (aFlags & KPbkItemsMarked) ||
        (aFlags & KPbkNoGroups))
        {
        aMenuPane.SetItemDimmed(EPbkCmdBelongsToGroups, ETrue);
        }

    if ((aFlags & KPbkItemsMarked))
        {
        aMenuPane.SetItemDimmed(EPbkCmdCreateNew, ETrue);
        }

    if ((aFlags & KPbkControlEmpty))
        {
        aMenuPane.SetItemDimmed(EPbkCmdDeleteMe, ETrue);
        }

    if (aFlags & KPbkNoOpenOneTouch)
        {
        aMenuPane.SetItemDimmed(EPbkCmdOpenOneTouch, ETrue);
        }
    }

void CPbkNamesListAppView::PhonebookInfoPaneMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags)
    {
    CPbkAppGlobals* globals =   // does not take ownership
        CPbkAppGlobals::InstanceL();

    // By default disable filtering of send contact menu,
    // when there are marked items
    TBool markedItemsFiltering = EFalse;

    // Check is multiple vCard sending via BT/IR supported
    if (!Engine()->Constants()->LocallyVariatedFeatureEnabled
        (EPbkLVMultipleVCardOverBTAndIR))
        {
        // It is not supported. Marked items filtering is
        // now needed if there is no mailbox set up
        markedItemsFiltering = !globals->SendUiL()->ValidateServiceL
            (KSenduiMtmSmtpUid, TSendingCapabilities(0,0,0));
        }

    // if names list is empty, no options menu items
    if ((aFlags & KPbkControlEmpty)
        || ((aFlags & KPbkItemsMarked) && markedItemsFiltering))
        {
        // Hide "Send contact"  menu
        aMenuPane.SetItemDimmed(EPbkCmdSendUICommand, ETrue);
        }
    else
        {
        aMenuPane.SetItemDimmed(EPbkCmdSendUICommand, EFalse);
        }

    if ((aFlags & KPbkControlEmpty) || (aFlags & KPbkItemsMarked))
        {
        aMenuPane.SetItemDimmed(EPbkPhonebookInfoSubMenu, ETrue);
        }

    // Go To URL filtering
    if ((aFlags & KPbkControlEmpty) || (aFlags & KPbkItemsMarked))
        {
        aMenuPane.SetItemDimmed(EPbkCmdGoToURL, ETrue);
        }
    else
        {
        TBool hasURL(EFalse);

        // Get the focused contact item
        const TContactItemId focusId =
            iContainer->Control()->FocusedContactIdL();
        CPbkContactItem* focusItem = Engine()->ReadContactLC(focusId);

        // Check does the contact has URL addresses
        if (focusItem->FindField(EPbkFieldIdURL))
            {
            hasURL = ETrue;
            }

        aMenuPane.SetItemDimmed(EPbkCmdGoToURL, !hasURL);
        CleanupStack::PopAndDestroy(); // focusItem
        }

    }

void CPbkNamesListAppView::SettingsPaneMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags)
    {
    CPbkMenuFiltering::SettingsPaneMenuFilteringL
        (aMenuPane, aFlags);
    }


void CPbkNamesListAppView::ContextMarkedItemsPaneMenuFilteringL
        (CEikMenuPane& aMenuPane, TUint aFlags)
    {
    if ((aFlags & KPbkNoGroups))
        {
        aMenuPane.SetItemDimmed(EPbkCmdAddToGroup, ETrue);
        }
    }

void CPbkNamesListAppView::HandleContactViewListControlEventL
        (CPbkContactViewListControl& aControl,
        const TPbkContactViewListControlEvent& aEvent)
    {
    __ASSERT_DEBUG(&aControl == iContainer->Control(),
        Panic(EPanicPreCond_HandleContactViewListControlEventL));

    // Suppress unreferenced parameter warning
    (void) aControl;

    switch (aEvent.iEventType)
        {
        case TPbkContactViewListControlEvent::EReady:
            {
            Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
            Cba()->DrawDeferred();
            RestoreControlStateL();

            // This profile is started in the NewApplication() entry point of
            // this application
            __PBK_PROFILE_END(PbkProfiling::EFullStartup);
            // Log the profiling results
#ifdef PBK_ENABLE_PROFILE
            for (TInt bin = 1; bin < PbkProfiling::KEndProfileBins; ++bin)
                {
                __PBK_PROFILE_DISPLAY(bin);
                }
#endif // PBK_ENABLE_PROFILE
            break;
            }

        case TPbkContactViewListControlEvent::EItemAdded:
            {
            if (!PbkAppUi()->IsRunningForeground())
                {
                iContainer->Control()->SetCurrentItemIndex(0);
                }
            else if (iContactToFocus == aEvent.iContactId)
                {
                CPbkContactViewListControl& control = *iContainer->Control();
                const TInt index = control.FindContactIdL(aEvent.iContactId);
                control.SetCurrentItemIndexAndDraw(index);
                iContactToFocus = KNullContactId;
                control.ShowThumbnailL();
                }
            break;
            }

        default:
            {
            break;
            }
        }
    }

inline void CPbkNamesListAppView::HandleRemoveFocusedEventL
        (TInt aIndex)
    {
    TInt index = aIndex;
    if (index >= 0)
        {
        const TInt lastIndex = iAllContactsView->CountL()-1;
        if (index > lastIndex)
            {
            index = lastIndex;
            }
        if (index >= 0)
            {
            iControlState->SetFocusedContactId(iAllContactsView->AtL(index));
            }
        }
    }

inline void CPbkNamesListAppView::HandleRemoveEvent
        (const TContactViewEvent& aEvent)
    {
    if (iControlState && iControlState->FocusedContactId()==aEvent.iContactId)
        {
        TRAP_IGNORE(HandleRemoveFocusedEventL(aEvent.iInt));
        }
    }

void CPbkNamesListAppView::HandleContactViewEvent
        (const CContactViewBase& aView, const TContactViewEvent& aEvent)
    {
    if (!iContainer && &aView==iAllContactsView)
        {
        switch (aEvent.iEventType)
            {
            case TContactViewEvent::EItemRemoved:
                {
                HandleRemoveEvent(aEvent);
                break;
                }
            default:
                break;
            }
        }
    }

void CPbkNamesListAppView::PreCommandExecutionL()
    {
    iContainer->Control()->HideThumbnail();
    iContainer->SetFocusing(EFalse);
    }

void CPbkNamesListAppView::PostCommandExecutionL
        (TPbkCommandStatus aStatus)
    {
    // Following data member check is needed, because e.g. PEC may call
    // this function after asynchronous command and in that case iContainer may
    // not exist anymore.
    if (iContainer)
        {
        if (aStatus == EPbkCommandExecuted)
            {
            iContainer->Control()->HandleMarkableListUpdateAfterCommandExecution();
            iContainer->Control()->DrawNow();
            }
        iContainer->SetFocusing(ETrue);
        iContainer->Control()->ShowThumbnailL();
        }
    }

/**
 * Open contact editor dialog for aContactId.
 */
void CPbkNamesListAppView::DoEditL(TContactItemId aContactId)
    {
    CPbkContactViewListControl& control = *iContainer->Control();

    // Open the contact for editing (leaves 2 objects to cleanup stack)
    CPbkContactItem* ci = Engine()->OpenContactLCX(aContactId);

    // Make control invisible to save an extra redraw
    control.HideThumbnail();
    iContainer->SetFocusing(EFalse);

    // Trap this, so that the thumbnail doesnt get hidden in case of leave
    TRAPD(err, DoOpenContactEditorDlgL(control, *ci, EFalse));

    iContainer->SetFocusing(ETrue);
    // Restore thumbnail
    control.ShowThumbnailL();

    // In case of an error, leave
    if (err != KErrNone)
        {
        User::Leave(err);
        }

    CleanupStack::PopAndDestroy(2);  // ci, lock
    }

inline TBool CPbkNamesListAppView::DoDeleteContactQueryL(
    TContactItemId aContactId)
    {
    CPbkDeleteContactQuery* dlg = CPbkDeleteContactQuery::NewLC();
    return (dlg->RunLD(*Engine(),aContactId) != 0);
    }

void CPbkNamesListAppView::DoDeleteContactL(TContactItemId aContactId)
    {
    if (DoDeleteContactQueryL(aContactId))
        {
        // Compress contact database in low disk space situation.
        // This compress guarantees that contact db size decreases
        // when phone user deletes single contact manually several times.
        // Compression is done before delete because compression may release
        // disk space. This extra space may be needed for successful delete
        // operation.
        TRAPD(err,FFSCheckL());
        if (KErrDiskFull == err)
            {
            //Db comppression does not cancel delete operation.
            TRAP_IGNORE(DbCompactL());
            err = KErrNone;
            }
        User::LeaveIfError(err);

        TRAP(err, Engine()->DeleteContactL(aContactId, ETrue));
        if ( KErrNotFound == err )
            {
            // Ignore KErrNotFound which means that somebody got
            // the contact first
            err = KErrNone;
            }
        User::LeaveIfError(err);
        iContainer->Control()->HandleMarkableListUpdateAfterCommandExecution();
        }
    }

inline TBool CPbkNamesListAppView::DoDeleteMarkedContactsQueryL
        (const CContactIdArray& aMarkedItems)
    {
    CPbkDeleteContactQuery* dlg = CPbkDeleteContactQuery::NewLC();
    return (dlg->RunLD(*Engine(), aMarkedItems) != 0);
    }

void CPbkNamesListAppView::DoDeleteMarkedContactsL
        (const CContactIdArray& aMarkedItems)
    {
    if (DoDeleteMarkedContactsQueryL(aMarkedItems))
        {
        CPbkDeleteContactsCmd* cmd = CPbkDeleteContactsCmd::NewL
            (*Engine(), aMarkedItems, *iContainer->Control());
        PbkAppUi()->AddAndExecuteCommandL(cmd);
        }
    }

void CPbkNamesListAppView::RestoreControlStateL()
    {
    CPbkContactViewListControl& control = *iContainer->Control();
    control.RestoreStateL(iControlState);
    // parameter state overrides stored state
    control.RestoreStateL(iParamState);
    delete iControlState;
    iControlState = NULL;
    delete iParamState;
    iParamState = NULL;
    }

inline void CPbkNamesListAppView::FFSCheckL()
    {
    if (!iPbkFFSCheck)
        {
        iPbkFFSCheck = CPbkFFSCheck::NewL();
        }
    // Leaves with KErrDiskFull if free disk space is below critical level.
    iPbkFFSCheck->FFSClCheckL();
    }

inline void CPbkNamesListAppView::RequestFreeDiskSpaceLC()
    {
    if (!iSharedDataClient)
        {
        iSharedDataClient = new (ELeave) RSharedDataClient();
        User::LeaveIfError(iSharedDataClient->Connect());
        }
    iSharedDataClient->RequestFreeDiskSpaceLC(KFreeSpaceForDbCompress);
    }

inline void CPbkNamesListAppView::DbCompactL()
    {
    RequestFreeDiskSpaceLC();
    Engine()->Database().CompactL();
    CleanupStack::PopAndDestroy();// RequestFreeDiskSpaceLC
    }

void CPbkNamesListAppView::DisableController()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromStack(iContainer);
        delete iContainer;
        iContainer = NULL;
        }
    }

//  End of File
