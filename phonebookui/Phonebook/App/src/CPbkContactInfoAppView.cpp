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
*       Provides Contact Info App View class methods.
*
*/


// INCLUDE FILES
#include "CPbkContactInfoAppView.h"  // This class' declaration

#include <eikmenub.h> // CEikMenuBar
#include <AknNoteWrappers.h>
#include <FeatMgr.h>
#include <StringLoader.h>
#include <PbkUID.h>

// Phonebook.app include files
#include <Phonebook.rsg>
#include <PbkView.rsg>

// Phone include files
#include <SpdiaControl.h>

// Send UI
#include <sendui.h>
#include <sendnorm.rsg>
#include <SendUiConsts.h>
#include <CMessageData.h>

// AIW
#include <AiwCommon.hrh>

// Clipboard
#include <baclipb.h>

// PbkEng.dll include files
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <CPbkContactItem.h>
#include <PbkCommandHandler.h>
#include <CPbkFieldsInfo.h>
#include <PbkFields.hrh>

// PbkUi.dll include files
#include <MPbkCommand.h>
#include <MPbkCommandFactory.h>
#include <CPbkAiwInterestArray.h>
#include <MenuFilteringFlags.h>

// PbkView.dll classes
#include <CPbkContactInfoControl.h>
#include <CPbkMemoryEntryDefaultsDlg.h>
#include <CPbkViewState.h>
#include <CPbkPhoneNumberSelect.h>
#include <CPbkContactEditorDlg.h>
#include <CPbkRingingToneFetch.h>
#include <CPbkPrependDlg.h>
#include <CPbkFFSCheck.h>

// PbkExtension classes
#include <CPbkExtGlobals.h>
#include <MPbkExtensionFactory.h>
#include <MPbkViewExtension.h>
#include <cshelp/phob.hlp.hrh>

// Postcard Uid
#include <SendUiConsts.h>

// Phonebook.app include files
#include <CPbkControlContainer.h>
#include "CPbkAppUi.h"
#include "CPbkAppGlobals.h"
#include "CPbkDocument.h"
#include "MPbkContactInfoNavigationStrategy.h"
#include "PbkContactInfoNavigationStrategyFactory.h"
#include <pbkconfig.hrh>
#include "CPbkCodImageCmd.h"

// Debugging headers
#include <pbkdebug.h>
#include "PbkProfiling.h"

/// Unnamed namespace for local definitions
namespace {

// LOCAL CONSTANTS AND MACROS

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicPostCond_DoActivateL = 1,
    EPanicPostCond_DoDeactivate,
    EPanicPreCond_HandleCommandL,
    EPanicPreCond_ConstructL,
    EPanicPostCond_ConstructL,
    EPanicPreCond_CmdSpeedDialL,
    EPanicPreCond_CmdWriteToContactL,
    EPanicPreCond_CmdEditMeL,
    EPanicPostCond_CmdEditMeL,
    EPanicPreCond_CmdRemoveSpeedDialL, // 10
    EPanicPreCond_DynInitMenuPaneL,
    EPanicIcon_StatusPaneUpdateL,
    EPanicPreCond_HandleCommandKeyL,
    EPanicPreCond_CmdRemoveCodL,
    EPanicPreCond_CmdAddCodFieldL,
    EPanicFieldInfoNotFound_CmdAddCodFieldL,
    EPanicImageStorageNotFound_CmdAddCodFieldL,
    EPanicPreCond_CmdEditTextCodFieldL,
    EPanicFieldInfoNotFound_HandleCodTextChangeL,
    EPanicTextStorageNotFound_HandleCodTextChangeL
    };
#endif // _DEBUG


// ==================== LOCAL FUNCTIONS ====================

#ifdef _DEBUG
void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CPbkContactInfoAppView");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

}  // namespace


// ================= MEMBER FUNCTIONS =======================

inline CPbkContactInfoAppView::CPbkContactInfoAppView()
    {
    }

inline void CPbkContactInfoAppView::ConstructL()
    {
    // PreCond
     __ASSERT_DEBUG(!iContainer && !iViewState,
        Panic(EPanicPreCond_ConstructL));

    // Init base class
    __PBK_PROFILE_START(PbkProfiling::EViewBaseConstruct);
    BaseConstructL(R_PBK_CONTACTINFO_APPVIEW);
    __PBK_PROFILE_END(PbkProfiling::EViewBaseConstruct);

    iExtGlobal = CPbkExtGlobals::InstanceL();

    // Create AIW interest array for call
    CreateInterestItemAndAttachL(KAiwCmdCall,
        R_PHONEBOOK_CALL_MENU, R_PHONEBOOK_CALLUI_AIW_INTEREST, ETrue);
    // The last parameter is false, since KAiwCmdCall is already attached
    // to base services above
    CreateInterestItemAndAttachL(KAiwCmdCall,
        R_PHONEBOOK_CALL_CONTEXT_MENU, R_PHONEBOOK_CALLUI_AIW_INTEREST, EFalse);

    // Create AIW interest array for PoC
    CreateInterestItemAndAttachL(KAiwCmdPoC,
        R_PHONEBOOK_POC_MENU, R_PHONEBOOK_POCUI_AIW_INTEREST, ETrue);
    // The last parameter is false, since KAiwCmdPoC is already attached
    // to base services above
    CreateInterestItemAndAttachL(KAiwCmdPoC,
        R_PHONEBOOK_POC_CONTEXT_MENU, R_PHONEBOOK_POCUI_AIW_INTEREST, EFalse);

    // FFS checker
    iPbkFFSCheck = CPbkFFSCheck::NewL();

    // PostCond
     __ASSERT_DEBUG(!iContainer && !iContactItem && !iViewState,
        Panic(EPanicPostCond_ConstructL));
    }

CPbkContactInfoAppView* CPbkContactInfoAppView::NewL()
    {
    CPbkContactInfoAppView* self = new(ELeave) CPbkContactInfoAppView();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();  // self
    return self;
    }

CPbkContactInfoAppView::~CPbkContactInfoAppView()
    {
    if (iContainer)
        {
        // Remove the container from the view stack
        AppUi()->RemoveFromViewStack(*this, iContainer);
        // Destroy the control container
        delete iContainer;
        }
    delete iUpdateContact;
    delete iContactItem;
    delete iViewState;
    delete iNaviStrategy;
    Release(iViewExtension);
    Release(iExtGlobal);
    delete iPbkCodImageCmd;
    delete iPbkFFSCheck;
    }

/**
 * Command handler: Opens the context sensive menu.
 */
void CPbkContactInfoAppView::CmdContextMenuL()
    {
    LaunchPopupMenuL(R_PBK_CONTACTINFO_CONTEXTBAR);
    }

/**
 * Command handler: Edit contact.
 */
void CPbkContactInfoAppView::CmdEditMeL()
    {
    __ASSERT_DEBUG(
        iContainer && iContainer->Control() && iContactItem,
        Panic(EPanicPreCond_CmdEditMeL));
    CPbkContactInfoControl* control = iContainer->Control();

    // Find focused field's index in contact item
    const TPbkContactItemField* field = control->CurrentField();
    const TInt fieldIndex = field ?
        iContactItem->FindFieldIndex(*field) : KErrNotFound;

    // Store the view state
    delete iViewState;
    iViewState = NULL;
    iViewState = iContainer->Control()->GetStateL();

    // Open contact for editing
    CPbkContactItem* ci = Engine()->OpenContactLCX(iContactItem->Id());

    // Run the editor dialog
    CPbkContactEditorDlg* editorDlg = CPbkContactEditorDlg::NewL
        (*Engine(), *ci, EFalse, fieldIndex);
    editorDlg->SetExitCommandId(EPbkCmdExit);

    control->HideThumbnail();
    iSavedId = KNullContactId;
    iContainer->SetFocusing( EFalse );
    iContainer->CoeControl()->SetFocusing( EFalse );
    TRAPD( err, iSavedId = editorDlg->ExecuteLD() );
    iContainer->SetFocusing( ETrue );
    iContainer->CoeControl()->SetFocusing( ETrue );
    control->ShowThumbnailL();
    User::LeaveIfError( err );

    CleanupStack::PopAndDestroy(2);  // lock, ci

    // delay update so that if desctruction of phonebook
    // has caused stopping of editor execution, no update is done
    // if this is deleted.
    delete iUpdateContact;
    iUpdateContact = NULL;
    iUpdateContact = CIdle::NewL(CActive::EPriorityIdle);

    TCallBack callBack( UpdateContact,this );
    iUpdateContact->Start( callBack );
    }

TInt CPbkContactInfoAppView::UpdateContact(TAny* aPbkContactInfoAppView)
    {
    return static_cast<CPbkContactInfoAppView*>(aPbkContactInfoAppView)->DoUpdateContact();
    }

TInt CPbkContactInfoAppView::DoUpdateContact()
    {
    CPbkContactInfoControl* control = NULL;
    if( iContainer )
        {
        control = iContainer->Control();
        }

    if ( !iExitInProgress && control )
        {
        CPbkContactInfoControl* control = iContainer->Control();
        if (iSavedId != KNullContactId)
            {
            // Reload contact and update UI control. Do this manually here instead
            // of in HandleDatabaseEventL() because editor dialog restores previous
            // text into title pane which would be wrong in this case.
            CPbkContactItem* ci = NULL;
            TRAPD( err, ci = Engine()->ReadContactL(iSavedId) );
            if ( err == KErrNone )
                {
                TRAP( err, control->UpdateL(ci));
                if ( err == KErrNone )
                    {
                    delete iContactItem;
                    iContactItem = ci;
                    if ( iViewState )
                        {
                        // Restore UI control state, no harm done
                        // if this fails, focus just changes
                        TRAP_IGNORE( iContainer->Control()->RestoreStateL(iViewState) );
                        // Delete the view state object to save memory
                        delete iViewState;
                        iViewState = NULL;
                        }
                    control->DrawNow();
                    }
                else
                    {
                    delete ci;
                    }
                }
            if ( err != KErrNone )
                {
                iContainer->ControlEnv()->HandleError( err );
                }
            }
        else
            {
            // It is just thumbnail, we can manage if that fails to show
            TRAP_IGNORE( control->ShowThumbnailL() );
            }

        __ASSERT_DEBUG(
            iContainer && iContainer->Control() && iContactItem,
            Panic(EPanicPostCond_CmdEditMeL));
        }
    // Stop idle callback
    return KErrNone;
    }

/**
 * Command handler: Default settings.
 */
void CPbkContactInfoAppView::CmdDefaultSettingsL()
    {
    CPbkContactInfoControl* control = iContainer->Control();
    control->HideThumbnail();
    CPbkMemoryEntryDefaultsDlg* memoryEntryDefaultsDlg =
        CPbkMemoryEntryDefaultsDlg::NewL(iContactItem->Id(), *Engine());
    memoryEntryDefaultsDlg->ExecuteLD();
    control->ShowThumbnailL();
    }

/**
 * Command handler: Assign a speed dial to a phone number.
 */
void CPbkContactInfoAppView::CmdAssignSpeedDialL()
    {
    //PreCond:
    __ASSERT_DEBUG(iContainer->Control()->FieldCount() > 0,
        Panic(EPanicPreCond_CmdSpeedDialL));

    TInt fieldId = iContainer->Control()->CurrentFieldIndex();

    CSpdiaControl* speedDialDlg = CSpdiaControl::NewL();
    if (speedDialDlg->ExecuteLD(iContactItem->Id(), fieldId) && !iExitInProgress)
        {
        // this can removed when CContactDatabase::SetSpeedDialField sends notifications
        CPbkContactItem* ci = Engine()->ReadContactLC(iContactItem->Id());
        // Update the UI control
        TInt focus = iContainer->Control()->ListBox().CurrentItemIndex();
        iContainer->Control()->UpdateL(ci);
        if (focus > 0 && focus < iContainer->Control()->FieldCount())
            {
            iContainer->Control()->ListBox().SetCurrentItemIndex(focus);
            }
        // Update member
        delete iContactItem;
        CleanupStack::Pop(ci);
        iContactItem = ci;
        iContainer->Control()->DrawNow();
        }
    }

/**
 * Command handler: Removes a speed dial from a phone number.
 */
void CPbkContactInfoAppView::CmdRemoveSpeedDialL()
    {
    //PreCond:
    __ASSERT_DEBUG(iContainer->Control()->FieldCount() > 0,
        Panic(EPanicPreCond_CmdRemoveSpeedDialL));

    CSpdiaControl* speedDialDlg = CSpdiaControl::NewL();
    CleanupStack::PushL( speedDialDlg );

    CArrayFix<TInt>* speedDialIndexes =
        iContainer->Control()->CurrentField()->SpeedDialIndexesL();
    if (speedDialIndexes)
        {
        CleanupStack::PushL(speedDialIndexes);
        for (TInt i = 0; i < speedDialIndexes->Count(); ++i)
            {
            speedDialDlg->RemoveDialIndexL(
                    speedDialDlg->Index( (*speedDialIndexes)[i] ) );
            }
        CleanupStack::PopAndDestroy(speedDialIndexes);
        }

    CleanupStack::PopAndDestroy( speedDialDlg );

    // This can removed when CContactDatabase::RemoveSpeedDialFieldL sends notifications
    CPbkContactItem* ci = Engine()->ReadContactLC(iContactItem->Id());
    // Update the UI control
    TInt focus = iContainer->Control()->ListBox().CurrentItemIndex();
    iContainer->Control()->UpdateL(ci);
    if (focus > 0 && focus < iContainer->Control()->FieldCount())
        {
        iContainer->Control()->ListBox().SetCurrentItemIndex(focus);
        }
    // Update member
    delete iContactItem;
    CleanupStack::Pop(ci);
    iContactItem = ci;
    iContainer->Control()->DrawNow();
    }

/**
 * Command handler: Personal Ringing tone setting
 */
void CPbkContactInfoAppView::CmdPersonalRingingToneL()
    {
    CPbkRingingToneFetch* toneFetch = CPbkRingingToneFetch::NewL(*Engine());
    CleanupStack::PushL(toneFetch);
    TFileName ringingToneFile;
    TPbkContactItemField* field =
        iContactItem->FindField(EPbkFieldIdPersonalRingingToneIndication);
    if (field)
        {
        ringingToneFile.Copy(field->Text().Left(ringingToneFile.MaxLength()));
        }
    if (toneFetch->FetchRingingToneL(ringingToneFile))
        {
        CPbkContactItem* item = Engine()->OpenContactLCX(iContactItem->Id());
        toneFetch->SetRingingToneL(*item, ringingToneFile);

        // Check disk space before assigning.
        // If not enough memory then leaves with KErrDiskFull.
        iPbkFFSCheck->FFSClCheckL();

        Engine()->CommitContactL(*item);
        CleanupStack::PopAndDestroy(2); // lock, item
        }
    CleanupStack::PopAndDestroy(); // toneFetch
    }


void CPbkContactInfoAppView::CmdSendContactL(TPbkSendingParams aParams)
    {
    TPbkContactItemField* field = iContainer->Control()->CurrentField();

    // Create the send contact command object
    MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
        CommandFactory().CreateSendContactCmdL(aParams,*Engine(),
        CPbkAppGlobals::InstanceL()->BCardEngL(*Engine()),
        iContactItem->Id(), field);
    // Execute the command
    cmd->ExecuteLD();
    }

/**
 * Command handler: write message to contact
 */
void CPbkContactInfoAppView::CmdWriteToContactL(TPbkSendingParams aParams)
    {
    __ASSERT_DEBUG(
        iContainer && iContainer->Control() && iContactItem,
        Panic(EPanicPreCond_CmdWriteToContactL));

    TPbkContactItemField* currentField = iContainer->Control()->CurrentField();

    // Create the write message command object
    MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
        CommandFactory().CreateSendMultipleMessageCmdL(
        *Engine(),
        aParams,
        iContainer->Control()->MarkedItemsL(),
        currentField,
        EFalse);
    // Execute the command
    cmd->ExecuteLD();
    }


/**
 * Command handler: go to URL command.
 */
void CPbkContactInfoAppView::CmdGoToURLL()
    {
    // Create the go to URL command object
    MPbkCommand* cmd = CPbkAppGlobalsBase::InstanceL()->
        CommandFactory().CreateGoToURLCmdL(*iContactItem, NULL);
    // Execute the command
    cmd->ExecuteLD();
    }

/**
 * Command handler: prepend.
 */
void CPbkContactInfoAppView::CmdPrependL()
    {
    // Get the Phone number
    RBuf number;
    number.CreateL( iContainer->Control()->CurrentField()->FieldInfo().MaxLength() );
    number.CleanupClosePushL();
    iContainer->Control()->CurrentField()->GetTextL(number);

    // Convert digits to foreign characters if necessary
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion(number);

    CPbkPrependDlg* dlg = CPbkPrependDlg::NewL(number);
    if (dlg->ExecuteLD(R_PBK_PREPEND_QUERY))
        {
        // convert digits back from foreign characters to western digits if necessary
        AknTextUtils::ConvertDigitsTo(number, EDigitTypeWestern);

        // Create a temporary contact
        CPbkContactItem* tempContact = Engine()->CreateEmptyContactL();
        CleanupStack::PushL(tempContact);

        // Add the prepended number to it
        TPbkContactItemField* phoneNumberField =
            tempContact->AddOrReturnUnusedFieldL(
                *Engine()->FieldsInfo().Find(
                iContainer->Control()->CurrentField()->FieldInfo().FieldId()));

        if (phoneNumberField)
            {
            phoneNumberField->TextStorage()->SetTextL(number);
            }

        // Create an array out of the temp contact
        CContactIdArray* array = CContactIdArray::NewL();
        CleanupStack::PushL(array);
        array->AddL(tempContact->Id());

        // Now place a call to the temp contact
        iAiwInterestArray->HandleCommandL(
                EPbkCmdCall,
                *array,
                phoneNumberField,    // "focused field" in this case
                KAiwCmdCall);

        CleanupStack::PopAndDestroy(2); // array, tempContact
        }
    CleanupStack::PopAndDestroy(); //number
    }

/**
 * Command handler: copy number.
 */
void CPbkContactInfoAppView::CmdCopyNumberL()
    {
    const TInt KBeginning = 0;
    // Construct a plain text from the number
    RBuf number;
    number.CreateL( iContainer->Control()->CurrentField()->FieldInfo().MaxLength() );
    number.CleanupClosePushL();
    iContainer->Control()->CurrentField()->GetTextL(number);
    CPlainText* plainText = CPlainText::NewL();
    CleanupStack::PushL(plainText);
    plainText->InsertL(KBeginning, number);

    // Copy to clipboard
    CClipboard* cb = CClipboard::NewForWritingLC(Engine()->FsSession());
    plainText->CopyToStoreL(cb->Store(), cb->StreamDictionary(),
        KBeginning, plainText->DocumentLength());
    cb->CommitL();

    CleanupStack::PopAndDestroy(2); // cb, plainText

    // Show a note
    HBufC* prompt = StringLoader::LoadLC(R_QTN_LOGS_NOTE_COPY_DONE);
    CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
    dlg->ExecuteLD(*prompt);
    CleanupStack::PopAndDestroy(); // prompt
    CleanupStack::PopAndDestroy(); //number
    }

void CPbkContactInfoAppView::ContactChangedL()
    {
    // if end key has been pressed, do not update the contact
    // infromation in info control.
    if (!iExitInProgress)
        {
        ChangeContactL(iContactItem->Id());
        }
    }

CPbkContactItem* CPbkContactInfoAppView::ContactItemL()
    {
    return iContactItem;
    }

TContactItemId CPbkContactInfoAppView::ContactItemId()
    {
    return iContactItem->Id();
    }

TBool CPbkContactInfoAppView::HandleCommandKeyL
        (const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    __ASSERT_DEBUG(iContainer && iContactItem && iNaviStrategy,
                   Panic(EPanicPreCond_HandleCommandKeyL));

    // Pass the key to the strategy
    TBool ret = iNaviStrategy->HandleCommandKeyL(aKeyEvent, aType);
    if (!ret)
        {
        // Check was it send key press
        if (aKeyEvent.iCode == EKeyPhoneSend)
            {
            HandleCommandL(EPbkCmdCall);
            ret = ETrue;
            }
        else if (aKeyEvent.iCode == EKeyPoC)
            {
            HandleCommandL(EPbkCmdPoc);
            ret = ETrue;
            }
        else
            {
            // Pass the key event to base class
            ret = CPbkMemoryEntryAppView::HandleCommandKeyL(aKeyEvent, aType);
            }
        }
    return ret;
    }

CPbkViewState* CPbkContactInfoAppView::GetViewStateLC() const
    {
    return iContainer ? iContainer->Control()->GetStateLC() : NULL;
    }

void CPbkContactInfoAppView::HandleCommandL
        (TInt aCommandId)
    {
    // PreCond: (this is the precond for all Cmd* functions at the same time)
    __ASSERT_DEBUG(iContainer && iContactItem && iViewExtension,
                   Panic(EPanicPreCond_HandleCommandL));

    TInt serviceCmdId = KNullHandle;
    if (aCommandId == EPbkCmdCall)
        {
        serviceCmdId = KAiwCmdCall;
        }
    else if (aCommandId == EPbkCmdPoc)
        {
        serviceCmdId = KAiwCmdPoC;
        }

    // Offer the command first to AIW
    if (iAiwInterestArray->HandleCommandL(
            aCommandId,
            iContainer->Control()->MarkedItemsL(),
            iContainer->Control()->FocusedField(),
            serviceCmdId))
        {
        return;
        }


    // And then to the extension
    if (iViewExtension->HandleCommandL(aCommandId))
        {
        return;
        }

    // Process view-specific commands
    switch (aCommandId)
        {
        case EPbkCmdContextMenu:
            {
            CmdContextMenuL();
            break;
            }

        case EPbkCmdEditMe:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkContactInfoAppView::CmdEditMeL));
            UpdateUrlFieldInfo();
            break;
            }

        case EPbkCmdDefaultSettings:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkContactInfoAppView::CmdDefaultSettingsL));
            break;
            }

        case EPbkCmdAssignSpeedDial:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkContactInfoAppView::CmdAssignSpeedDialL));
            break;
            }

        case EPbkCmdRemoveSpeedDial:
            {
            CmdRemoveSpeedDialL();
            break;
            }

        case EPbkCmdPersonalRingingTone:
            {
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkContactInfoAppView::CmdPersonalRingingToneL));
            break;
            }

        case EPbkCmdGoToURL:
            {
            CmdGoToURLL();
            break;
            }

        case EPbkCmdPrepend:
            {
            CmdPrependL();
            break;
            }

        case EPbkCmdCopyNumber:
            {
            CmdCopyNumberL();
            break;
            }
        case EPbkCmdWrite:
            {
            TPbkSendingParams params = CreateWriteParamsLC();
            PbkAppUi()->FFSClCheckL(PbkCommandHandler(this,
                &CPbkContactInfoAppView::CmdWriteToContactL,
                params));
            CleanupStack::PopAndDestroy(); //mtmFilter
            break;
            }
        case EPbkCmdSendUICommand:
            {
            TPbkSendingParams params = CreateSendParamsLC();
            CmdSendContactL(params);
            CleanupStack::PopAndDestroy(); //mtmFilter
            break;
            }
        case EPbkCmdCodRemoveObject:
            {
            // Only one Cod field exists at a time.
            // By callig both functions the existing field
            // will be deleted
            CmdRemoveCodL( EPbkFieldIdCodTextID );
            CmdRemoveCodL( EPbkFieldIdCodImageID );
            break;
            }
        // Following methods need critical level check of memory
        case EPbkCmdCodAddObjectText:
        case EPbkCmdCodAddObjectImage:
            {
            CmdAddCodFieldL( aCommandId );
            break;
            }
        case EPbkCmdCodEditText:
            {
            CmdEditTextCodFieldL();
            break;
            }
        case EPbkCmdCodViewImage:
            {
            CmdViewCodImageL();
            break;
            }
        case EPbkCmdCodChangeImage:
            {
            CmdAddCodFieldL(EPbkCmdCodAddObjectImage);
            break;
            }
        case EPbkCmdCodAddObjectSubMenu:
            {
            break;
            }
        case EPbkCmdExit:
            {
            // Turn up the iExitInProgress flag to avoid unwanted
            // contact redraw in exit case. This happens if the editor
            // dialog is running while exit should be executed.
            iExitInProgress = ETrue;
            // fall through to default handler.
            }
        default:
            {
            CPbkMemoryEntryAppView::HandleCommandL(aCommandId);
            break;
            }
        }
    }

void CPbkContactInfoAppView::DynInitMenuPaneL
        (TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    __ASSERT_DEBUG(iContainer && iViewExtension,
        Panic(EPanicPreCond_DynInitMenuPaneL));

    TUint flags = KPbkInfoView;
    if (iContainer->Control()->FieldCount() == 0)
        {
        flags |= KPbkControlEmpty;
        }

    TPbkContactItemField* currentField =
        iContainer->Control()->CurrentField();

    if (currentField && currentField->FieldInfo().IsPhoneNumberField())
        {
        flags |= KPbkCurrentFieldPhoneNumber;
        }

    if (currentField && currentField->FieldInfo().IsVoipField())
        {
        flags |= KPbkCurrentFieldVoipAddress;
        }

    // The flag KPbkCurrentFieldPoc is valid for using only
    // in PoC context menu filtering in CPbkAiwInterestItemPoc::InitMenuPaneL
    if ( currentField )
        {
        // We can't use the IsPocField() info here, since context
        // menu filtering is more strict than that
        const TPbkFieldId fieldId = currentField->FieldInfo().FieldId();
        switch ( fieldId )
            {
            case EPbkFieldIdVOIP:                   // FALLTHROUGH
            case EPbkFieldIdPushToTalk:             // FALLTHROUGH
            case EPbkFieldIdShareView:              // FALLTHROUGH
            case EPbkFieldIdSIPID:
                {
                flags |= KPbkCurrentFieldPoc;
                }
            default:
                {
                // Do nothing
                }
            }
        }

    // Offer the menu to AIW first
    if (iAiwInterestArray->InitMenuPaneL(aResourceId, *aMenuPane, flags))
        {
        // No need to continue
        return;
        }

    iViewExtension->DynInitMenuPaneL(aResourceId, aMenuPane);

    switch (aResourceId)
        {
        case R_PBK_CONTACTINFO_OPTIONS_MENU:
            {
            OptionsMenuFilteringL(*aMenuPane, flags);
            break;
            }
        case R_PBK_CONTACTINFO_CONTEXT_MENU:
            {
            ContextMenuFilteringL(*aMenuPane, flags);
            break;
            }
        case R_PBK_ADD_OBJECT_SUBMENU: // Add object menu item
            {
            AddObjectSubPaneMenuFilteringL(*aMenuPane, flags);
            break;
            }
        default:
            {
            break;
            }
        }

    CPbkMemoryEntryAppView::DynInitMenuPaneL(aResourceId,aMenuPane);
    }

inline void CPbkContactInfoAppView::OptionsMenuFilteringL
        (CEikMenuPane& aMenuPane,
        TUint aFlags)
    {
    if ((aFlags & KPbkControlEmpty)
        || Engine()->IsSpeedDialAssigned(*iContactItem,
            iContainer->Control()->CurrentFieldIndex()))
        {
        aMenuPane.SetItemDimmed(EPbkCmdAssignSpeedDial, ETrue);
        }
    else if (FeatureManager::FeatureSupported(KFeatureIdCommonVoip))
        {
        if (!iContainer->Control()->CurrentField()->
            FieldInfo().IsVoipField())
            {
            aMenuPane.SetItemDimmed(EPbkCmdAssignSpeedDial, ETrue);
            }
        }
    else
        {
        if (!iContainer->Control()->CurrentField()->
            FieldInfo().IsPhoneNumberField())
            {
            aMenuPane.SetItemDimmed(EPbkCmdAssignSpeedDial, ETrue);
            }
        }

    if ((aFlags & KPbkControlEmpty) ||
        !Engine()->IsSpeedDialAssigned(*iContactItem,
            iContainer->Control()->CurrentFieldIndex()))
        {
        // Field is a phone number and speed dial for this field
        aMenuPane.SetItemDimmed(EPbkCmdRemoveSpeedDial, ETrue);
        }
    if ((aFlags & KPbkControlEmpty) || (!iUrlFieldAvailable))
        {
        // URL field not present
        aMenuPane.SetItemDimmed(EPbkCmdGoToURL, ETrue);
        }
    if ((aFlags & KPbkControlEmpty) || !(aFlags & KPbkCurrentFieldPhoneNumber))
        {
        // Field is not a phone number field
        aMenuPane.SetItemDimmed(EPbkCmdUseNumber, ETrue);
        }

    if (aFlags & KPbkControlEmpty)
        {
        aMenuPane.SetItemDimmed(EPbkCmdDefaultSettings, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdPersonalRingingTone, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdSendUICommand, ETrue);
        }
    else
        {
        aMenuPane.SetItemDimmed(EPbkCmdSendUICommand, EFalse);
        }

    if (FeatureManager::FeatureSupported(KFeatureIdCallImagetext))
        {
        // Empty contact, no fields
        if( (aFlags & KPbkControlEmpty) )
            {
            aMenuPane.SetItemDimmed( EPbkCmdCodEditText, ETrue );
            aMenuPane.SetItemDimmed( EPbkCmdCodViewImage, ETrue );
            aMenuPane.SetItemDimmed( EPbkCmdCodChangeImage, ETrue );
            aMenuPane.SetItemDimmed( EPbkCmdCodRemoveObject, ETrue );
            }
        else
            {
            TPbkContactItemField* field = iContainer->Control()->CurrentField();
            const TPbkFieldId pbkFieldId( field->FieldInfo().FieldId() );

            if (pbkFieldId == EPbkFieldIdCodTextID && CodTextFieldEnabled())
                {
                if (CodImageFieldEnabled())
                    {
                    // Text field contains Cod image filename
                    aMenuPane.SetItemDimmed( EPbkCmdCodEditText, ETrue );
                    }
                else
                    {
                    // Text field is used for Cod text purposes
                    aMenuPane.SetItemDimmed( EPbkCmdCodViewImage, ETrue );
                    aMenuPane.SetItemDimmed( EPbkCmdCodChangeImage, ETrue );
                    }
                }
            else
                {
                // Focus is not on the Object for call field.
                aMenuPane.SetItemDimmed( EPbkCmdCodEditText, ETrue );
                aMenuPane.SetItemDimmed( EPbkCmdCodViewImage, ETrue );
                aMenuPane.SetItemDimmed( EPbkCmdCodChangeImage, ETrue );
                aMenuPane.SetItemDimmed( EPbkCmdCodRemoveObject, ETrue );
                }
            }
        }
    else
        {
        // Image/text for call feature not supported.
        // Dimm all related commands
        aMenuPane.SetItemDimmed( EPbkCmdCodEditText, ETrue);
        aMenuPane.SetItemDimmed( EPbkCmdCodViewImage, ETrue);
        aMenuPane.SetItemDimmed( EPbkCmdCodChangeImage, ETrue);
        aMenuPane.SetItemDimmed( EPbkCmdCodAddObjectSubMenu, ETrue);
        aMenuPane.SetItemDimmed( EPbkCmdCodRemoveObject, ETrue);
        }

    }

inline void CPbkContactInfoAppView::ContextMenuFilteringL
        (CEikMenuPane& aMenuPane,
        TUint aFlags)
    {
    TPbkContactItemField* field = iContainer->Control()->CurrentField();

    // context sensitive menu filtering
    if (aFlags & KPbkControlEmpty)
        {
        aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdUseNumber, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdPersonalRingingTone, ETrue);
        aMenuPane.SetItemDimmed(EPbkCmdGoToURL, ETrue);
        }
    else if (field)
        {
        switch(field->FieldInfo().FieldId())
            {
            case EPbkFieldIdPhoneNumberStandard:    // FALLTHROUGH
            case EPbkFieldIdPhoneNumberHome:        // FALLTHROUGH
            case EPbkFieldIdPhoneNumberWork:        // FALLTHROUGH
            case EPbkFieldIdPhoneNumberMobile:      // FALLTHROUGH
            case EPbkFieldIdPhoneNumberVideo:       // FALLTHROUGH
            case EPbkFieldIdFaxNumber:              // FALLTHROUGH
            case EPbkFieldIdAssistantNumber:        // FALLTHROUGH
            case EPbkFieldIdCarNumber:
                {
                aMenuPane.SetItemDimmed(EPbkCmdDeleteMe, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdPersonalRingingTone, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdGoToURL, ETrue);
                break;
                }
            case EPbkFieldIdEmailAddress:
                {
                aMenuPane.SetItemDimmed(EPbkCmdDeleteMe, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdUseNumber, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdPersonalRingingTone, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdGoToURL, ETrue);
                break;
                }
            case EPbkFieldIdPersonalRingingToneIndication:
                {
                aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdUseNumber, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdEditMe, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdDeleteMe, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdGoToURL, ETrue);
                break;
                }
            case EPbkFieldIdURL:
                {
                aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdUseNumber, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdDeleteMe, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdPersonalRingingTone, ETrue);
                break;
                }
            default:
                {
                // By default only Edit item is shown
                aMenuPane.SetItemDimmed(EPbkCmdWrite, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdUseNumber, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdDeleteMe, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdPersonalRingingTone, ETrue);
                aMenuPane.SetItemDimmed(EPbkCmdGoToURL, ETrue);
                break;
                }
            }
        }
    }

TPbkSendingParams CPbkContactInfoAppView::CreateWriteParamsLC()
    {
    // Construct the MTM filter
    CArrayFixFlat<TUid>* mtmFilter =
        new(ELeave) CArrayFixFlat<TUid>(4);
    CleanupStack::PushL(mtmFilter);

    if (iContainer->Control()->FieldCount() > 0)
        {
        switch(iContainer->Control()->CurrentField()->FieldInfo().FieldId())
            {
            case EPbkFieldIdPhoneNumberStandard:    // FALLTHROUGH
            case EPbkFieldIdPhoneNumberHome:        // FALLTHROUGH
            case EPbkFieldIdPhoneNumberWork:        // FALLTHROUGH
            case EPbkFieldIdPhoneNumberMobile:      // FALLTHROUGH
            case EPbkFieldIdAssistantNumber:        // FALLTHROUGH
            case EPbkFieldIdCarNumber:
                {
                mtmFilter->AppendL(KSenduiMtmSmtpUid); //Email
                break;
                }
            case EPbkFieldIdEmailAddress:
                {
                mtmFilter->AppendL(KSenduiMtmSmsUid); //Sms
                break;
                }
            default:
                break;
            }
        }

    TSendingCapabilities capabilities = TSendingCapabilities();
    capabilities.iFlags = TSendingCapabilities::ESupportsEditor;
    return TPbkSendingParams( mtmFilter, capabilities );
    }

TPbkSendingParams CPbkContactInfoAppView::CreateSendParamsLC()
    {
    // Construct the MTM filter
    CArrayFixFlat<TUid>* mtmFilter =
        new(ELeave) CArrayFixFlat<TUid>(4);
    CleanupStack::PushL(mtmFilter);

    const TUid KPostcardMtmUid = { KSenduiMtmPostcardUidValue };

    mtmFilter->AppendL(KPostcardMtmUid);    // hide postcard
    mtmFilter->AppendL(KSenduiMtmAudioMessageUid); // hide audio message

    TSendingCapabilities capabilities = TSendingCapabilities();
    capabilities.iFlags = TSendingCapabilities::ESupportsBioSending;
    capabilities.iFlags |= TSendingCapabilities::ESupportsAttachments;
    return TPbkSendingParams( mtmFilter, capabilities );
    }

TUid CPbkContactInfoAppView::Id() const
    {
    return CPbkAppUi::KPbkContactInfoViewUid;
    }

void CPbkContactInfoAppView::HandleStatusPaneSizeChange()
    {
    // Resize the container to fill the client rectangle
    if (iContainer)
        {
        iContainer->SetRect(ClientRect());
        }
    }

void CPbkContactInfoAppView::DoActivateL
        (const TVwsViewId& aPrevViewId,
         TUid aCustomMessageId,
         const TDesC8& aCustomMessage)
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactInfoAppView(%x)::DoActivateL()"), this);

    // reset the exit in progress flag.
    iExitInProgress = EFalse;

    // Call base class, this must be done!
    CPbkMemoryEntryAppView::DoActivateL(aPrevViewId,
        aCustomMessageId, aCustomMessage);

    // Update application-wide state. UI control updates the title pane.
    CPbkAppUi::CViewActivationTransaction* viewActivationTransaction =
        PbkAppUi()->HandleViewActivationLC
            (Id(), aPrevViewId, NULL, NULL,
            CPbkAppUi::EUpdateNaviPane | CPbkAppUi::EUpdateContextPane);

    if (aCustomMessageId == CPbkViewState::Uid())
        {
        // Read the desired UI state from aCustomMessage
        CPbkViewState* viewState = CPbkViewState::NewL(aCustomMessage);
        if (iViewState)
            {
            // Merge parameter view state with the stored state
            if (viewState->FocusedContactId() != iViewState->FocusedContactId())
                {
                iViewState->SetFocusedContactId(viewState->FocusedContactId());
                iViewState->SetFocusedFieldIndex(-1);
                iViewState->SetTopFieldIndex(-1);
                }
            if (viewState->FocusedFieldIndex() >= 0)
                {
                iViewState->SetFocusedFieldIndex(viewState->FocusedFieldIndex());
                }
            if (viewState->TopFieldIndex() >= 0)
                {
                iViewState->SetTopFieldIndex(viewState->TopFieldIndex());
                }
            if (viewState->ParentContactId() != iViewState->ParentContactId())
                {
                iViewState->SetParentContactId(viewState->ParentContactId());
                }
            // delete parameter view state
            delete viewState;
            }
        else
            {
            // No stored state, use the parameter supplied one
            iViewState = viewState;
            }
        }
    else
        {
        // No UI state specified, restore in previous state
        if (!iViewState)
            {
            // No previous state
            User::Leave(KErrArgument);
            }
        }

    // Read the contact
    if (!iContactItem || iContactItem->Id() != iViewState->FocusedContactId())
        {
        CPbkContactItem* ci = Engine()->ReadContactLC
            (iViewState->FocusedContactId());
        if (iContainer && iContainer->Control())
            {
            // Change the contact item inside the UI control
            iContainer->Control()->UpdateL(ci);
            }
        delete iContactItem;
        CleanupStack::Pop();  // ci
        iContactItem = ci;

        //Check does the contact have an URL field
        UpdateUrlFieldInfo();
        }

    if (!iContainer)
        {
        // Create the container control
        iContainer = CContainer::NewL(this, *this);

        iContainer->SetHelpContext(TCoeHelpContext(ApplicationUid(),
            KPHOB_HLP_CONTACT_INFO_VIEW));

        // Create extension
        if (!iViewExtension)
            {
            iViewExtension =
                iExtGlobal->FactoryL().CreatePbkViewExtensionL
                    (Id(), *PbkAppUi()->PbkDocument()->Engine(), *this);
            }

        // Create the view-side UI control
        CPbkContactInfoControl* control = CPbkContactInfoControl::NewL
            (R_PBK_DEFAULT_CONTACTINFO_CONTROL,
            *iContainer,
            iContactItem,
            Engine());

        // Set up the container. iContainer takes ownership of control safely
        iContainer->SetControl(control, ClientRect());
        iViewExtension->SetContactUiControl(control);
        iContainer->ActivateL();

        // create navigation strategy
        iNaviStrategy = PbkContactInfoNavigationStrategyFactory::CreateL(
                *Engine(), *this, iViewState->ParentContactId());

        // Add this view and container to the view stack
        AppUi()->AddToViewStackL(*this, iContainer);
        }

    // Restore UI control state
    iContainer->Control()->RestoreStateL(iViewState);
    // Delete the view state object to save memory
    delete iViewState;
    iViewState = NULL;

    // Commit application-wide state changes
    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy();  // viewActivationTransaction

    // Draw the control
    iContainer->Control()->DrawNow();

    // PostCond
    __ASSERT_DEBUG(
        iContainer && iContainer->Control() &&
        iContactItem && !iViewState,
        Panic(EPanicPostCond_DoActivateL));
    }

void CPbkContactInfoAppView::DoDeactivate()
    {
    PBK_DEBUG_PRINT(PBK_DEBUG_STRING
        ("CPbkContactInfoAppView(%x)::DoDeactivate()"), this);

    // Delete any previous view state
    delete iViewState;
    iViewState = NULL;

    // delete navigation strategy
    delete iNaviStrategy;
    iNaviStrategy = NULL;

    if (iContainer)
        {
        if (iContainer->Control())
            {
            // Store the view state for next activation
            TRAP_IGNORE(iViewState = iContainer->Control()->GetStateL());
            }
        // Remove view and its control from the view stack
        AppUi()->RemoveFromViewStack(*this, iContainer);
        // Destroy the container control
        delete iContainer;
        iContainer = NULL;
        }

    if (iViewExtension)
        {
        iViewExtension->SetContactUiControl(NULL);
        }

    // Delete the contact item
    delete iContactItem;
    iContactItem = NULL;

    // Call base class, this must be done!
    CPbkMemoryEntryAppView::DoDeactivate();

    // PostCond
    __ASSERT_DEBUG(!iContainer && !iContactItem,
        Panic(EPanicPostCond_DoDeactivate));
    }


void CPbkContactInfoAppView::UpdateUrlFieldInfo()
    {
    iUrlFieldAvailable = EFalse;
    if (iContactItem->FindField(EPbkFieldIdURL))
        {
        iUrlFieldAvailable = ETrue;
        }
    }

void CPbkContactInfoAppView::ChangeContactL
        (TContactItemId aContactId)
    {
    // Load the contact
    CPbkContactItem* ci = Engine()->ReadContactLC(aContactId);
    // Check if the contact content has changed or the contact
    // has changed. CPbkContactItem's == operator only compares
    // fields, therefore the id check has to be done manually.
    if ((aContactId != iContactItem->Id()) || (*ci != *iContactItem))
        {
        // Update the UI control
        CPbkContactInfoControl* control = iContainer->Control();
        CPbkViewState* controlState = control->GetStateLC();
        control->UpdateL(ci);
        control->RestoreStateL(controlState);
        CleanupStack::PopAndDestroy(controlState);
        // Update members
        delete iContactItem;
        CleanupStack::Pop(ci);
        iContactItem = ci;
        UpdateUrlFieldInfo();
        control->DrawNow();
        }
    else
        {
        // Contact data not changed, no need to update
        CleanupStack::PopAndDestroy();  // ci
        }
    }

CEikStatusPane* CPbkContactInfoAppView::GetStatusPane() const
    {
    return StatusPane();
    }

TContactItemId CPbkContactInfoAppView::GetContactItemId() const
    {
    return iContactItem->Id();
    }

void CPbkContactInfoAppView::AddObjectSubPaneMenuFilteringL(
    CEikMenuPane& aMenuPane, TUint
    aFlags
    )
    {
    if (FeatureManager::FeatureSupported(KFeatureIdCallImagetext))
        {
        // If empty contanct then no more execution needed
        if (! (aFlags & KPbkControlEmpty) )
            {
            if (CodTextFieldEnabled())
                {
                if (CodImageFieldEnabled())
                    {
                    aMenuPane.SetItemDimmed( EPbkCmdCodAddObjectImage, ETrue );
                    }
                else
                    {
                    aMenuPane.SetItemDimmed( EPbkCmdCodAddObjectText, ETrue );
                    }
                }
            }
        }
    else
        {
        aMenuPane.SetItemDimmed( EPbkCmdCodAddObjectImage, ETrue );
        }
    }

/**
 * Command handler: Removes object for call from the contact.
 */
void CPbkContactInfoAppView::CmdRemoveCodL(TInt aFieldType)
    {
    //PreCond: Field type id should be either Cod image for Cod text.
    __ASSERT_DEBUG( ( aFieldType == EPbkFieldIdCodTextID ||
             aFieldType == EPbkFieldIdCodImageID ),
        Panic(EPanicPreCond_CmdRemoveCodL) );

    TPbkContactItemField* field = iContactItem->FindField( aFieldType );
    if (field && aFieldType == EPbkFieldIdCodTextID )
        {
        CPbkContactItem* ci = Engine()->OpenContactLCX(iContactItem->Id());
        ci->RemoveField(iContactItem->FindFieldIndex(*field));
        Engine()->CommitContactL(*ci);
        // Update member
        CleanupStack::PopAndDestroy(ci);
        CleanupStack::PopAndDestroy(); // lock
        // UI refresh is handled by HandleDatabaseEventL method in base class
        }
    else
        {
        PbkCodImageCmdL().RemoveImageL(iContactItem->Id());
        }
    }

inline void CPbkContactInfoAppView::CmdAddCodFieldL( TInt aCommandId )
    {
    __ASSERT_DEBUG( EPbkCmdCodAddObjectImage == aCommandId ||
        EPbkCmdCodAddObjectText == aCommandId,
        Panic(EPanicPreCond_CmdAddCodFieldL) );
    #ifdef _DEBUG
    TInt fieldId(EPbkCmdCodAddObjectText==aCommandId?
        EPbkFieldIdCodTextID:EPbkFieldIdCodImageID);
    // Field info should be found
    CPbkFieldInfo* fieldInfo = Engine()->FieldsInfo().Find(fieldId);
    #endif
    __ASSERT_DEBUG( fieldInfo,
        Panic(EPanicFieldInfoNotFound_CmdAddCodFieldL) );

    if (aCommandId==EPbkCmdCodAddObjectText)
        {
        // Check if Cod image exists.
        if ( CodTextFieldEnabled() && CodImageFieldEnabled() )
            {
            // Reset data.
            CmdRemoveCodL(EPbkFieldIdCodTextID);
            CmdRemoveCodL(EPbkFieldIdCodImageID);
            }
        HandleCodTextChangeL(iContactItem->Id());
        }
    else
        {
        HandleCodImageChangeL(iContactItem->Id());
        }
    }

inline TBool CPbkContactInfoAppView::HandleCodTextChangeL(TContactItemId aId) const
    {
    TBool ret(EFalse);

    // Field info should be found
    CPbkFieldInfo* fieldInfo = Engine()->FieldsInfo().Find(EPbkFieldIdCodTextID);
    __ASSERT_DEBUG( fieldInfo,
        Panic( EPanicFieldInfoNotFound_HandleCodTextChangeL ) );

    CPbkContactItem* ci = Engine()->OpenContactLCX(aId);
    // Use text field if exists
    TPbkContactItemField* field = ci->FindField( EPbkFieldIdCodTextID );
    if (!field)
        {
        // Otherwise create new field for filename purposes
        field = ci->AddOrReturnUnusedFieldL(*fieldInfo);
        }
    __ASSERT_DEBUG( field->TextStorage(),
        Panic(EPanicTextStorageNotFound_HandleCodTextChangeL) );

    HBufC* textToBeModified = HBufC::NewMaxLC(KPbkCodTextMaxLength);
    CContactTextField& contactTextField = *field->TextStorage();
    textToBeModified->Des().Copy(contactTextField.Text().Left(KPbkCodTextMaxLength));
    TPtr ptr = textToBeModified->Des();
    HBufC* prompt = StringLoader::LoadLC(R_QTN_PHOB_PRTX_TEXT_FOR_CALL);
    CAknTextQueryDialog* aknTextQueryDialog = CAknTextQueryDialog::NewL(ptr);
    aknTextQueryDialog->SetMaxLength(KPbkCodTextMaxLength);
    if (aknTextQueryDialog->ExecuteLD(R_PBK_EDIT_FIELD_TEXT_FOR_QUERY, *prompt))
        {
        // The text is set if data query not cancelled
        contactTextField.SetTextL(*textToBeModified);

        TPbkContactItemField* imageField=ci->FindField(EPbkFieldIdCodImageID);
        if (imageField)
            {
            // Image can not exist if Cod text field is used.
            ci->RemoveField(ci->FindFieldIndex(*field));
            }
        Engine()->CommitContactL(*ci);
        ret = ETrue;
        }
    CleanupStack::PopAndDestroy(4); //textToBeModified, prompt, ci, lock

    return ret;
    }

inline void CPbkContactInfoAppView::CmdEditTextCodFieldL()
    {
    __ASSERT_DEBUG( iContactItem->FindField(EPbkFieldIdCodTextID),
        Panic(EPanicPreCond_CmdEditTextCodFieldL) );
    HandleCodTextChangeL(iContactItem->Id());
    }

inline TBool CPbkContactInfoAppView::HandleCodImageChangeL(TContactItemId aId)
    {
    return PbkCodImageCmdL().FetchImageL(aId);
    }

CPbkCodImageCmd& CPbkContactInfoAppView::PbkCodImageCmdL()
    {
    if (!iPbkCodImageCmd)
        {
        iPbkCodImageCmd = CPbkCodImageCmd::NewL(*Engine());
        }
    return *iPbkCodImageCmd;
    }

inline void CPbkContactInfoAppView::CmdViewCodImageL()
    {
    PbkCodImageCmdL().ViewImageL(iContactItem->Id());
    }

TBool CPbkContactInfoAppView::CodTextFieldEnabled() const
    {
    return iContactItem->FindField(EPbkFieldIdCodTextID)?ETrue:EFalse;
    }

TBool CPbkContactInfoAppView::CodImageFieldEnabled() const
    {
    return iContactItem->FindField(EPbkFieldIdCodImageID)?ETrue:EFalse;
    }

//  End of File
