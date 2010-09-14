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
*          Provides Contact List app views class methods.
*
*/


// INCLUDE FILES
#include "CPbkContactListAppView.h"

#include "CPbkAppUi.h"
#include "CPbkAppGlobals.h"

#include <PbkUID.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <PbkCommandHandler.h>
#include <MPbkCommand.h>
#include <MPbkCommandObserver.h>
#include <MPbkCommandFactory.h>
#include <CPbkContactViewListControl.h>
#include <CPbkPhoneNumberSelect.h>
#include <CPbkGroupBelongings.h>
#include <CPbkConstants.h>
#include <phonebook.rsg>            // Phonebook resources
#include <MenuFilteringFlags.h>
#include <CPbkAiwInterestArray.h>

#include <eikmenup.h>
#include <sendui.h>                 // Send UI API
#include <SenduiMtmUids.h>          // Send UI MTM uids
#include <CMessageData.h>
#include <MsgBioUids.h>             // Bio message uids
#include <AiwServiceHandler.h>      // AIW
#include <AiwCommon.hrh>            // AIW
#include <SendUiConsts.h>           // Postcard Uid
#include <FeatMgr.h>                // Feature manager

// Debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"

// Local declarations
/*
* Local helper class to clear marked list if command is succesfull
* Can only be used if command is not using the default command observer
*/
class TClearMarks : public MPbkCommandObserver
    {
    public:
        TClearMarks( CPbkContactViewListControl& aControl );

    public: // MPbkCommandObserver
        void CommandFinished(const MPbkCommand& aCommand);

    private:
        CPbkContactViewListControl& iControl;
    };

TClearMarks::TClearMarks( CPbkContactViewListControl& aControl ) :
iControl( aControl )
    {

    }

void TClearMarks::CommandFinished(const MPbkCommand& /*aCommand*/)
    {
    iControl.HandleMarkableListUpdateAfterCommandExecution();
    }

// ================= MEMBER FUNCTIONS =======================

CPbkContactListAppView::CPbkContactListAppView()
	{
	}

CPbkContactListAppView::~CPbkContactListAppView()
	{
	}

void CPbkContactListAppView::BaseConstructL(TInt aResId)
    {
    __PBK_PROFILE_START(PbkProfiling::EViewBaseConstruct);
    CPbkAppView::BaseConstructL(aResId);
    __PBK_PROFILE_END(PbkProfiling::EViewBaseConstruct);

    // Create AIW interest array for call
    CreateInterestItemAndAttachL(KAiwCmdCall,
        R_PHONEBOOK_CALL_MENU, R_PHONEBOOK_CALLUI_AIW_INTEREST, ETrue);

    // Create AIW interest array for PoC
    CreateInterestItemAndAttachL(KAiwCmdPoC,
        R_PHONEBOOK_POC_MENU, R_PHONEBOOK_POCUI_AIW_INTEREST, ETrue);
    // The last parameter is false, since KAiwCmdPoC is already attached
    // to base services above
    CreateInterestItemAndAttachL(KAiwCmdPoC,
        R_PHONEBOOK_POC_CONTEXT_MENU, R_PHONEBOOK_POCUI_AIW_INTEREST, EFalse);

    // Create AIW interest array for synchronization
    CreateInterestItemAndAttachL(KAiwCmdSynchronize,
        R_PHONEBOOK_SYNCML_MENU, R_PHONEBOOK_SYNCML_AIW_INTEREST, ETrue);
    }

void CPbkContactListAppView::CmdOpenMeViewsL()
	{
    if (!Control().ItemsMarked())
        {
        CPbkViewState* state = Control().GetStateLC();
        PbkAppUi()->ActivatePhonebookViewL(CPbkAppUi::KPbkContactInfoViewUid, state);
        CleanupStack::PopAndDestroy();  // state
        Control().HandleMarkableListUpdateAfterCommandExecution();
        }
	}

void CPbkContactListAppView::CmdWriteToContactsL(
         TPbkSendingParams aParams )
    {
    const CContactIdArray& array = Control().MarkedItemsL();
    if ( array.Count() > 0 )
        {
		// Create the write message command object
		MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
			CommandFactory().CreateSendMultipleMessageCmdL(
			*Engine(),
			aParams,
			array,
			NULL,
			ETrue);
	    TClearMarks clearMarks( Control() );
	    cmd->AddObserver( clearMarks );
		// Execute the command
		cmd->ExecuteLD();

        Control().ResetFindL();
        TInt idx = Control().FindContactIdL(array[0]);
        if (idx >= 0)
            {
            Control().SetCurrentItemIndexAndDraw(idx);
            }
        }
    }

void CPbkContactListAppView::CmdSendContactsL(
         TPbkSendingParams aParams )
    {
    CPbkContactViewListControl& control = Control();

    const TContactItemId focusedContactId = control.FocusedContactIdL();
    TInt focusedContactIndex( control.FindContactIdL(focusedContactId) );

    if (control.ItemsMarked())
        {
		// Get the contact ids in an array
		const CContactIdArray& array = control.MarkedItemsL();
		// Create the send contacts command object
		MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
			CommandFactory().CreateSendContactsCmdL(
			aParams, *Engine(),
			CPbkAppGlobals::InstanceL()->BCardEngL(*Engine()), array);
	    TClearMarks clearMarks( control );
	    cmd->AddObserver( clearMarks );
		// Execute the command
		cmd->ExecuteLD();
        }
    else
        {
        if (focusedContactId != KNullContactId)
            {
			// Create the send contact command object
			MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
				CommandFactory().CreateSendContactCmdL(
				aParams, *Engine(),
				CPbkAppGlobals::InstanceL()->BCardEngL(*Engine()),
				focusedContactId, NULL);
			// Execute the command
			cmd->ExecuteLD();
            }
        }

    // reset find filter and focus to (first) sent contact

    if ( control.FindTextL() != KNullDesC )
        {
        control.ResetFindL();
        focusedContactIndex = control.FindContactIdL(focusedContactId);
        }

    if (focusedContactIndex >= 0)
        {
        control.SetCurrentItemIndexAndDraw(focusedContactIndex);
        }
    }

void CPbkContactListAppView::CmdBelongsToGroupsL()
    {
    if (!Control().ItemsMarked())
        {
        const TContactItemId contactId = Control().FocusedContactIdL();
        CPbkGroupBelongings* dlg = CPbkGroupBelongings::NewL();
        dlg->ExecuteLD(*Engine(), contactId);
        Control().HandleMarkableListUpdateAfterCommandExecution();
        }
    }

void CPbkContactListAppView::HandleCommandL
        (TInt aCommandId)
    {
    // MarkedItemsL() might leave on low-mem situation
    const CContactIdArray* items = NULL;
    TRAPD(err, items = &Control().MarkedItemsL());
    if (!err && items)
        {
        // Offer the command first to AIW
        TInt serviceCmdId = KNullHandle;
        if (!Control().ItemsMarked())
            {
            if (aCommandId == EPbkCmdCall)
                {
                serviceCmdId = KAiwCmdCall;
                }
            else if ( aCommandId == EPbkCmdPoc )
                {
                serviceCmdId = KAiwCmdPoC;
                }
            }

        if(iAiwInterestArray->HandleCommandL(
            aCommandId,
            *items,
            NULL, // no field level focus
            serviceCmdId,
            this))
            {
            return;
            }
        }

    // Try as normal command
    switch (aCommandId)
        {
        case EPbkCmdOpenMeViews:
            {
            CmdOpenMeViewsL();
            break;
            }

        case EPbkCmdBelongsToGroups:
            {
            CmdBelongsToGroupsL();
            break;
            }
        case EPbkCmdWrite:
            {
            TPbkSendingParams params = CreateWriteParamsLC();

            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkContactListAppView::CmdWriteToContactsL,
                params ));

            CleanupStack::PopAndDestroy(); //mtmFilter
            break;
            }
        default:
            {
            Control().HandleMarkableListProcessCommandL(aCommandId);
            CPbkAppView::HandleCommandL(aCommandId);
            break;
            }

        }
    }

/**
 * Returns ETrue if shift is depressed in aKeyEvent.
 */
inline TBool ShiftDown
        (const TKeyEvent& aKeyEvent)
    {
    return (aKeyEvent.iModifiers & (EModifierShift|EModifierLeftShift|EModifierRightShift)) != 0;
    }

/**
 * Returns ETrue if shift is depressed in aKeyEvent.
 */
inline TBool CtrlDown
        (const TKeyEvent& aKeyEvent)
    {
    return (aKeyEvent.iModifiers & (EModifierCtrl|EModifierLeftCtrl|EModifierRightCtrl)) != 0;
    }

TBool CPbkContactListAppView::HandleCommandKeyL
        (const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    TBool result = EFalse;

    if (aType == EEventKey && Control().IsReady())
        {
        switch (aKeyEvent.iCode)
            {
            case EKeyPhoneSend:
                {
                // Use the EPbkCmdCall id to identify that
                // call is being launched with send key
                if ( Control().NumberOfItems() > 0 )
                    {
                    HandleCommandL(EPbkCmdCall);
                    result = ETrue;
                    }
                break;
                }
            case EKeyPoC:
                {
                HandleCommandL(EPbkCmdPoc);
                break;
                }
            case EKeyBackspace:
                {
                if (aKeyEvent.iCode == EKeyBackspace)
                    {
                    if ((Control().NumberOfItems() > 0) &&
                        (Control().FindTextL() == KNullDesC))
                        {
                        // check that there are names and that find window is empty
                        DeleteFocusedEntryL();
                        result = ETrue;
                        }
                    }
                break;
                }

            case EKeyOK:
                {
                if (!ShiftDown(aKeyEvent) && !CtrlDown( aKeyEvent ))  // pure OK key
                    {
                    CPbkContactViewListControl& control = Control();
                    if (control.NumberOfItems() > 0 && !control.ItemsMarked())
                        {
                        // Select key is mapped to "Open Contact" command
                        HandleCommandL(EPbkCmdOpenMeViews);
                        }
                    else
                        {
                        // select key mapped to context sensitive menu if list is empty
                        HandleCommandL(EPbkCmdContextMenu);
                        }
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


void CPbkContactListAppView::DynInitMenuPaneL
        (TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    Control().HandleMarkableListDynInitMenuPane(aResourceId, aMenuPane);
    CPbkAppView::DynInitMenuPaneL(aResourceId,aMenuPane);
    }

void CPbkContactListAppView::HandleSendContactsL( TUint aFlags )
    {
    TPbkSendingParams params = CreateSendParamsLC(aFlags);
    PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
		&CPbkContactListAppView::CmdSendContactsL,
        params));
    CleanupStack::PopAndDestroy(); //mtmFilter
    }

TPbkSendingParams CPbkContactListAppView::CreateWriteParamsLC()
    {
    // Construct the MTM filter
	CArrayFixFlat<TUid>* mtmFilter =
		new(ELeave) CArrayFixFlat<TUid>(4);
	CleanupStack::PushL(mtmFilter);

    const CContactIdArray& array = Control().MarkedItemsL();
    if( array.Count() > 1 )
        {
    	const TUid KPostcardMtmUid = { KSenduiMtmPostcardUidValue };
        mtmFilter->AppendL(KPostcardMtmUid);	// hide postcard
        }

	TSendingCapabilities capabilities = TSendingCapabilities();
    capabilities.iFlags = TSendingCapabilities::ESupportsEditor;
	return TPbkSendingParams( mtmFilter, capabilities );
    }

TPbkSendingParams CPbkContactListAppView::CreateSendParamsLC(TUint aFlags)
    {
    // Construct the MTM filter
	CArrayFixFlat<TUid>* mtmFilter =
		new(ELeave) CArrayFixFlat<TUid>(4);
    CleanupStack::PushL(mtmFilter);

	const TUid KPostcardMtmUid = { KSenduiMtmPostcardUidValue };

    mtmFilter->AppendL(KPostcardMtmUid);	// hide postcard
    mtmFilter->AppendL(KSenduiMtmAudioMessageUid); // hide audio message

    // Check must we disable SMS menuitem
	if (!CPbkAppGlobals::InstanceL()->SendUiL()
        ->CanSendBioMessage(KMsgBioUidVCard))
        {
        mtmFilter->AppendL(KSenduiMtmSmsUid);	// hide SMS
        }

	// If there are items marked, hide items
    if (aFlags & KPbkItemsMarked)
        {

        mtmFilter->AppendL(KSenduiMtmSmsUid);	// hide SMS

        // If multiple vCard over BT/IR is not supported hide also BT and IR
        if (!Engine()->Constants()->LocallyVariatedFeatureEnabled
            (EPbkLVMultipleVCardOverBTAndIR))
            {
            mtmFilter->AppendL(KSenduiMtmBtUid);	// hide Bluetooth
		    mtmFilter->AppendL(KSenduiMtmIrUid);	// hide Infrared
            }
        }

    // Hide MMS if not supported
    if (!FeatureManager::FeatureSupported(KFeatureIdMMS))
        {
        mtmFilter->AppendL(KSenduiMtmMmsUid);
        }

    // Hide Email if not supported
    if (!FeatureManager::FeatureSupported(KFeatureIdEmailUi))
        {
        mtmFilter->AppendL(KSenduiMtmSmtpUid);
        mtmFilter->AppendL(KSenduiMtmImap4Uid);
        mtmFilter->AppendL(KSenduiMtmPop3Uid);
        mtmFilter->AppendL(KSenduiMtmSyncMLEmailUid);
        }

    TSendingCapabilities capabilities = TSendingCapabilities();
    capabilities.iFlags = TSendingCapabilities::ESupportsBioSending;
    capabilities.iFlags |= TSendingCapabilities::ESupportsAttachments;
	return TPbkSendingParams( mtmFilter, capabilities );
    }

void CPbkContactListAppView::AiwCommandHandledL(TInt /*aMenuCommandId*/,
        TInt aServiceCommandId,
        TInt /*aErrorCode*/)
    {
    if (aServiceCommandId == KAiwCmdCall ||
        aServiceCommandId == KAiwCmdPoC)
        {
        // Reset find and focus the selected contact
        const TInt selIndex = Control().CurrentItemIndex();
        if (selIndex >= 0)
            {
            TContactItemId cid = Control().ContactIdAtL(selIndex);
            Control().ResetFindL();
            TInt idx = Control().FindContactIdL(cid);
            if (idx >= 0)
                {
                Control().SetCurrentItemIndexAndDraw(idx);
                }
            }
        Control().HandleMarkableListUpdateAfterCommandExecution();
        }
    }

//  End of File
