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
*          Provides Groups list View controller methods
*
*/


// INCLUDE FILES
#include "CPbkGroupsListAppView.h"  // This class' declaration

// System APIs
#include <pbkconfig.hrh>
#include <eikmenup.h>        // CEikMenuPane
#include <cntitem.h>         // CContactGroup
#include <StringLoader.h>

// Phonebook.app APIs
#include <phonebook.rsg>  // Phonebook resources
#include <pbkdebug.h>
#include "CPbkAppUi.h"
#include <CPbkControlContainer.h>
#include "CPbkSetToneToGroupCmd.h"
#include "CPbkMenuFiltering.h"
#include "CPbkDocument.h"
#include "CPbkAppGlobals.h"

// PbkEng.dll APIs
#include <CPbkConstants.h>
#include <CPbkContactEngine.h>
#include <PbkCommandHandler.h>
#include <CPbkAiwInterestArray.h>

// PbkView.dll APIs
#include <pbkview.rsg>                   // PbkView dll resources
#include <CPbkViewState.h>               // Phonebook view state object
#include <CPbkContactViewListControl.h>
#include <CPbkGroupNameQueryDlg.h>       // CPbkGroupNameQueryDlg
#include <CPbkRingingToneFetch.h>
#include <CPbkPhonebookInfoDlg.h>

// PbkUi.dll APIs
#include <MPbkCommand.h>
#include <MPbkCommandFactory.h>
#include <MenuFilteringFlags.h>

// PbkExtension classes
#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>
#include <MPbkViewExtension.h>
#include <cshelp/phob.hlp.hrh>

#include <AiwCommon.hrh>            // AIW

// Debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"

// From SendUI
#include <SendUiConsts.h>           // Postcard Uid


/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_DoActivateL = 1,
    EPanicPostCond_DoDeactivate,
    EPanicPreCond_HandleCommandL,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPreCond_CmdOpenGroupL,
    EPanicPreCond_CmdRemoveGroupL,
    EPanicPreCond_CmdRenameGroupL,
    EPanicPreCond_HandleContactViewListControlEventL,
    EPanicPreCond_DynInitMenuPaneL
    };
#endif


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkGroupsListAppView");
    User::Panic(KPanicText, aReason);
    }
#endif

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkGroupsListAppView::CPbkGroupsListAppView() :
        iGroupToFocus(KNullContactId)
    {
    }

inline void CPbkGroupsListAppView::ConstructL()
    {
    // PreCond
     __ASSERT_DEBUG(!iContainer && !iControlState, Panic(EPanicPreCond_ConstructL));

    // Init base class
    __PBK_PROFILE_START(PbkProfiling::EViewBaseConstruct);
    BaseConstructL(R_PHONEBOOK_GROUPSLIST_APPVIEW);
    __PBK_PROFILE_END(PbkProfiling::EViewBaseConstruct);

    iExtGlobal = CPbkExtGlobals::InstanceL();
    
    // Create AIW interest array
    CreateInterestItemAndAttachL(KAiwCmdPoC,
        R_PHONEBOOK_POC_MENU, R_PHONEBOOK_POCUI_AIW_INTEREST, ETrue);

    // PostCond
     __ASSERT_DEBUG(!iContainer && !iControlState, Panic(EPanicPostCond_ConstructL));
    }

CPbkGroupsListAppView* CPbkGroupsListAppView::NewL()
    {
    CPbkGroupsListAppView* self = new (ELeave) CPbkGroupsListAppView();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

CPbkGroupsListAppView::~CPbkGroupsListAppView()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkGroupsListAppView(%x)::~CPbkGroupsListAppView()"), this);

    delete iContainer;
    delete iParamState;
    delete iControlState;
    Release(iViewExtension);
    Release(iExtGlobal);
    }

void CPbkGroupsListAppView::CmdOpenGroupL()
    {
    //PreCond:
    __ASSERT_DEBUG(iContainer->Control()->NumberOfItems() > 0, Panic(EPanicPreCond_CmdOpenGroupL));

    CPbkViewState* state = iContainer->Control()->GetStateLC();
    PbkAppUi()->ActivatePhonebookViewL(CPbkAppUi::KPbkGroupMembersListViewUid, state);
    CleanupStack::PopAndDestroy();  // state
    }

inline void CPbkGroupsListAppView::CmdWriteToGroupL()
    {
    const TInt selIndex = iContainer->Control()->FocusedContactIdL();
    if (selIndex >= 0)
        {
        CContactGroup* contactGroup = Engine()->ReadContactGroupL( selIndex );
        CleanupStack::PushL( contactGroup );

        TPbkSendingParams params = CreateWriteParamsLC();

		// Create the write message command object
		MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
			CommandFactory().CreateSendMultipleMessageCmdL(
			*Engine(), 
			params,
			*contactGroup->ItemsContained(),
			NULL, 
			ETrue );
		// Execute the command
		cmd->ExecuteLD();
        CleanupStack::PopAndDestroy(); // params.iMtmFilter
		CleanupStack::PopAndDestroy( contactGroup );
        }
    }

void CPbkGroupsListAppView::CmdCreateNewGroupL()
    {
    HBufC* textBuf = HBufC::NewLC(CPbkConstants::GroupLabelLength());
    TPtr text = textBuf->Des();

    CPbkGroupNameQueryDlg* dlg = CPbkGroupNameQueryDlg::NewL(text, *Engine());
    dlg->SetMaxLength(CPbkConstants::GroupLabelLength());
    if (dlg->ExecuteLD(R_PHONEBOOK_GROUPLIST_NEWGROUP_QUERY))
        {
        // insert new group into database
        CContactGroup* group = Engine()->CreateContactGroupL(text, ETrue);
        iGroupToFocus = group->Id();
        delete group;
        }
    CleanupStack::PopAndDestroy(textBuf);
    }

void CPbkGroupsListAppView::CmdRemoveGroupL()
    {
    //PreCond:
    __ASSERT_DEBUG(iContainer->Control()->NumberOfItems() > 0, 
        Panic(EPanicPreCond_CmdRemoveGroupL));

    // get the focused this index and delete the contact group.
    const TContactItemId groupId = iContainer->Control()->FocusedContactIdL();
    if (groupId != KNullContactId)
        {
        CContactGroup* group = NULL;
        TRAPD(err, group = Engine()->ReadContactGroupL(groupId))
        if (err == KErrNotFound)
            {
            // Ignore KErrNotFound which means that somebody got
            // the contact first
            return;
            }
        User::LeaveIfError(err);
        CleanupStack::PushL(group);
        const CContactIdArray* groupContainsIds = group->ItemsContained();
        HBufC* prompt = NULL;
        if (!groupContainsIds || groupContainsIds->Count()==0)
            {
            // Empty group
            prompt = StringLoader::LoadL(R_QTN_QUERY_COMMON_CONF_DELETE, group->GetGroupLabelL());
            }
        else
            {
            // Nonempty group
            prompt = iCoeEnv->AllocReadResourceL(R_QTN_FLDR_DEL_FULL_GROUPS_QUERY);
            }
        CleanupStack::PopAndDestroy(group);
        CleanupStack::PushL(prompt);
        CAknQueryDialog* dlg = CAknQueryDialog::NewL();
        CleanupStack::PushL(dlg);
        dlg->SetPromptL(*prompt);
        CleanupStack::Pop(); // dlg
        if (dlg->ExecuteLD(R_PBK_GENERAL_CONFIRMATION_QUERY))
            {
            TRAPD(err, Engine()->DeleteContactGroupL(groupId))
            if (err == KErrNotFound)
                {
                // Ignore KErrNotFound which means that
                // somebody got the contact group first
                err = KErrNone;
                }
            User::LeaveIfError(err);
            }
        CleanupStack::PopAndDestroy(); // prompt
        }
    }

void CPbkGroupsListAppView::CmdRenameGroupL()
    {
    //PreCond:
    __ASSERT_DEBUG(iContainer->Control()->NumberOfItems() > 0, 
        Panic(EPanicPreCond_CmdRenameGroupL));

    const TContactItemId groupId = iContainer->Control()->FocusedContactIdL();
    if (groupId != KNullContactId)
        {
        HBufC* textBuf = HBufC::NewLC(CPbkConstants::GroupLabelLength());
        TPtr text = textBuf->Des();

        // get the name of the group into the buffer
        CContactGroup* group = Engine()->OpenContactGroupLCX(groupId);
        text = group->GetGroupLabelL().Left(text.MaxLength());

        CPbkGroupNameQueryDlg* dlg = CPbkGroupNameQueryDlg::NewL(text, *Engine(), EFalse);
        dlg->SetMopParent(this);
        if (dlg->ExecuteLD(R_PHONEBOOK_GROUPLIST_NEWGROUP_QUERY))
            {
            group->SetGroupLabelL(text);
            Engine()->CommitContactGroupL(*group, ETrue);
            iGroupToFocus = groupId;
            }
        CleanupStack::PopAndDestroy(3,textBuf); // group, lock, textBuf
        }
    }

void CPbkGroupsListAppView::CmdSetRingingToneL()
    {
    const TContactItemId groupId = iContainer->Control()->FocusedContactIdL();
    if (groupId != KNullContactId)
        {
        CPbkRingingToneFetch* toneFetch = CPbkRingingToneFetch::NewL(*Engine());
        CleanupStack::PushL(toneFetch);
        TFileName ringingToneFile;
        if (toneFetch->FetchRingingToneL(ringingToneFile))
            {
            CPbkSetToneToGroupCmd* cmd = CPbkSetToneToGroupCmd::NewL
                (*Engine(), groupId, ringingToneFile);
            PbkAppUi()->AddAndExecuteCommandL(cmd);
            }
        CleanupStack::PopAndDestroy(toneFetch);
        }
    }

inline void CPbkGroupsListAppView::CmdOpenSettingsL()
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

inline void CPbkGroupsListAppView::CmdContextMenuL()
    {
    LaunchPopupMenuL(R_PBK_GROUPSLIST_CONTEXT_MENUBAR);
    }

inline void CPbkGroupsListAppView::CmdPhonebookInfoL()
    {
    CPbkContactViewListControl& control = *iContainer->Control();

    // Hide thumbnail
    control.HideThumbnail();
    iContainer->SetFocusing(EFalse);

    // Trap this, so that the thumbnail doesnt get hidden in case of leave
    TRAPD(err, DoShowPhonebookInfoL());

    // Restore
    iContainer->SetFocusing(ETrue);
    control.ShowThumbnailL();

    // In case of an error, leave
    if (err != KErrNone)
        {
        User::Leave(err);
        }
    }
    
inline void CPbkGroupsListAppView::DoShowPhonebookInfoL()
    {
    CPbkPhonebookInfoDlg* infoPopup =
        CPbkPhonebookInfoDlg::NewL(*Engine());
    infoPopup->ExecuteLD();
    }


TBool CPbkGroupsListAppView::HandleCommandKeyL
        (const TKeyEvent& aKeyEvent, 
        TEventCode /*aType*/)
    {
    TBool result = EFalse;
    CPbkContactViewListControl& control = *iContainer->Control();
    if (control.IsReady())
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyOK:
                {
                if (control.NumberOfItems() > 0)
                    {
                    // Select key is mapped to "Open Group" command
                    HandleCommandL(EPbkCmdOpenGroup);
                    result = ETrue;
                    }
                else 
                    {
                    // select key mapped to context sensitive menu if list is empty
                    HandleCommandL(EPbkCmdContextMenu);
                    result = ETrue;
                    }
                break;
                }
            
            case EKeyBackspace:
                {
                if (control.NumberOfItems() > 0)
                    {
                    // Clear key is mapped to Remove Group command if the list
                    // is not empty
                    HandleCommandL(EPbkCmdRemoveGroup);
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

CPbkViewState* CPbkGroupsListAppView::GetViewStateLC() const
    {
    return iContainer ? iContainer->Control()->GetStateLC() : NULL;
    }

void CPbkGroupsListAppView::HandleCommandL
        (TInt aCommandId)
    {
    // PreCond: (this is the precond for all Cmd* functions at the same time)
    __ASSERT_DEBUG(iContainer && iViewExtension, Panic(EPanicPreCond_HandleCommandL));
            
    // Offer the command first to AIW
    const TInt focusedContactId = iContainer->Control()->FocusedContactIdL();
    if (focusedContactId != -1)
        {
        CContactGroup* contactGroup = Engine()->ReadContactGroupL(focusedContactId);
        CleanupStack::PushL( contactGroup );
        TInt serviceCmdId = KNullHandle;        
        if (iAiwInterestArray->HandleCommandL(
                aCommandId,
                *contactGroup->ItemsContained(),
                NULL, // no field level focus
                serviceCmdId, this))
            {
            // command handled by AIW
            CleanupStack::PopAndDestroy(); //contactGroup
            return;
            }
        CleanupStack::PopAndDestroy(); //contactGroup
        }

    if (iViewExtension->HandleCommandL(aCommandId/*, *this*/))
        {
        return;
        }

    // Process view-specific commands
    switch (aCommandId) 
        {
        case EPbkCmdOpenGroup:
            {
            CmdOpenGroupL();
            break;
            }

        case EPbkCmdCreateNewGroup:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this, 
                &CPbkGroupsListAppView::CmdCreateNewGroupL));
            break;
            }

        case EPbkCmdRemoveGroup:
            {
            CmdRemoveGroupL();
            break;
            }

        case EPbkCmdRenameGroup:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this, 
                &CPbkGroupsListAppView::CmdRenameGroupL));
            break;
            }

        case EPbkCmdSetRingingTone:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this, 
                &CPbkGroupsListAppView::CmdSetRingingToneL));
            break;
            }

        case EPbkCmdPhonebookInfo:
            {
            CmdPhonebookInfoL();
            break;
            }

        case EPbkCmdContextMenu:
            {
            CmdContextMenuL();
            break;
            }

        case EPbkCmdSettings:
            {
            CmdOpenSettingsL();
            break;
            }
            
        case EPbkCmdWrite:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkGroupsListAppView::CmdWriteToGroupL ));
            break;
            }

        default:
            {
            // Command not handled here, to app ui.
            AppUi()->HandleCommandL(aCommandId);
            break;
            }
        }
    }

void CPbkGroupsListAppView::DynInitMenuPaneL
        (TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    __ASSERT_DEBUG(iContainer && iViewExtension, 
        Panic(EPanicPreCond_DynInitMenuPaneL));

    iViewExtension->DynInitMenuPaneL(aResourceId, aMenuPane);

    // Set up flags depending on current UI state
    TUint flags = KPbkGroupsView;
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
        
    // Offer the menu to AIW first
    if (iAiwInterestArray->InitMenuPaneL(aResourceId, *aMenuPane, flags))
        {
        // No need to continue
        return;
        }

    if(aResourceId == R_PBK_GROUPSLIST_MENU)
        {
        if ( !Engine()->Constants()->
            LocallyVariatedFeatureEnabled( EPbkLVGroupManagementSend ) )
            {
            flags |= KPbkNoGroupSend;
            }
        CPbkMenuFiltering::GroupsListPaneMenuFilteringL(*aMenuPane, flags);
        }

    CPbkAppView::DynInitMenuPaneL(aResourceId,aMenuPane);
    }

TUid CPbkGroupsListAppView::Id() const
    {
    return CPbkAppUi::KPbkGroupsListViewUid;
    }

void CPbkGroupsListAppView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

void CPbkGroupsListAppView::DoActivateL
        (const TVwsViewId& aPrevViewId,
         TUid aCustomMessageId,
         const TDesC8& aCustomMessage)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkGroupsListAppView(%x)::DoActivateL()"), this);

    // Load the view status bar title
    HBufC* title = StringLoader::LoadLC(R_QTN_PHOB_TITLE);

    // Update application-wide state leave-safely
    CPbkAppUi::CViewActivationTransaction* viewActivationTransaction = 
        PbkAppUi()->HandleViewActivationLC(Id(), aPrevViewId, title, NULL);

    // Get the view state given as a parameter
    delete iParamState;
    iParamState = NULL;
    if (aCustomMessageId == CPbkViewState::Uid())
        {
        iParamState = CPbkViewState::NewL(aCustomMessage);
        }

    if (!iContainer)
        {
        // Create the control container
        CContainer* container = CContainer::NewLC(this, *this);

        container->SetHelpContext(
                TCoeHelpContext(ApplicationUid(), KPHOB_HLP_GROUP_LIST));

        if (!iViewExtension)
            {                  
            iViewExtension = iExtGlobal->FactoryL().CreatePbkViewExtensionL(
                    Id(), *PbkAppUi()->PbkDocument()->Engine());
            }
        
        // Create the view-side UI control and connect it to the container
        CPbkContactViewListControl* control = CPbkContactViewListControl::NewL(
            *Engine(), Engine()->AllGroupsViewL(), R_PBK_GROUPS_LIST_VIEW_CONTROL, container);

        container->SetControl(control, ClientRect());
        control->AddObserverL(*this);
        iViewExtension->SetContactUiControl(control);

        AppUi()->AddToStackL(*this, container);
        CleanupStack::Pop(container);
        iContainer = container;
        }

    // Activate the UI control
    iContainer->ActivateL();

    // Commit application-wide state changes
    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(2);  // viewActivationTransaction, title

    // PostCond:
    __ASSERT_DEBUG(iContainer, Panic(EPanicPostCond_DoActivateL));
    }

void CPbkGroupsListAppView::DoDeactivate()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING("CPbkGroupsListAppView(%x)::DoDeactivate()"), this);

    // Delete any previous control state
    delete iControlState;
    iControlState = NULL;
    delete iParamState;
    iParamState = NULL;

    if (iContainer)
        {
        // Remove view and its control from the view stack
        AppUi()->RemoveFromStack(iContainer);

        // Save UI control's state. Ignore errors because this function cannot 
        // leave and control state saving is not a critical operation.
        CPbkContactViewListControl* control = iContainer->Control();
        if (control)
            {
            TRAP_IGNORE(iControlState = control->GetStateL(EFalse));
            }

        // Destroy the container and the control
        delete iContainer;
        iContainer = NULL;
        }

    if (iViewExtension)
        {
        iViewExtension->SetContactUiControl(NULL);
        }

    // PostCond:
    __ASSERT_DEBUG(!iContainer, Panic(EPanicPostCond_DoDeactivate));
    }

void CPbkGroupsListAppView::HandleContactViewListControlEventL
        (CPbkContactViewListControl& aControl,
        const TPbkContactViewListControlEvent& aEvent)
    {
    __ASSERT_DEBUG(&aControl == iContainer->Control(),
        Panic(EPanicPreCond_HandleContactViewListControlEventL));

    switch (aEvent.iEventType)
        {
        case TPbkContactViewListControlEvent::EReady:
            {
            Cba()->SetCommandSetL(R_AVKON_SOFTKEYS_OPTIONS_EXIT);
            Cba()->DrawDeferred();
            aControl.RestoreStateL(iControlState);
            aControl.RestoreStateL(iParamState);
            delete iControlState;
            iControlState = NULL;
            delete iParamState;
            iParamState = NULL;
            break;
            }

        case TPbkContactViewListControlEvent::EItemAdded:
            {
            if (iGroupToFocus == aEvent.iContactId)
                {
                const TInt index = aControl.FindContactIdL(aEvent.iContactId);
                aControl.SetCurrentItemIndexAndDraw(index);
                iGroupToFocus = KNullContactId;
                }
            break;
            }

        default:
            {
            break;
            }
        }
    }

void CPbkGroupsListAppView::AiwCommandHandledL(TInt /*aMenuCommandId*/,
        TInt /*aServiceCommandId*/,
        TInt /*aErrorCode*/)
    {
    
    }
    
TPbkSendingParams CPbkGroupsListAppView::CreateWriteParamsLC()
    {
    // Construct the MTM filter
	CArrayFixFlat<TUid>* mtmFilter =
		new(ELeave) CArrayFixFlat<TUid>(4);
	CleanupStack::PushL(mtmFilter);

    const TUid KPostcardMtmUid = { KSenduiMtmPostcardUidValue };
    mtmFilter->AppendL(KPostcardMtmUid);	// hide postcard        

	TSendingCapabilities capabilities = TSendingCapabilities();
    capabilities.iFlags = TSendingCapabilities::ESupportsEditor;
	return TPbkSendingParams( mtmFilter, capabilities );
    }
    


//  End of File  
