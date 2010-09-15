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
* Description:  Phonebook 2 USIM UI Extension FDN names list view.
*
*/


// INCLUDE FILES
#include "CPsu2FixedDialingView.h"

// Phonebook 2
#include "CPsu2FixedDialingCall.h"
#include "CPsu2ViewManager.h"
#include "CPsu2SecUi.h"
#include <MPbk2ContactUiControl.h>
#include <CPbk2UIExtensionView.h>
#include <MPbk2ViewActivationTransaction.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ViewExplorer.h>
#include <TPbk2ContactEditorParams.h>
#include <CPbk2ContactEditorDlg.h>
#include <CPbk2FieldPropertyArray.h>
#include <MPbk2FieldProperty.h>
#include <pbk2usimuires.rsg>
#include <pbk2uicontrols.rsg>
#include <CPbk2NamesListControl.h>
#include <CPbk2ControlContainer.h>
#include <CPbk2ViewState.h>
#include <MPbk2CommandHandler.h>
#include <MPbk2ApplicationServices.h>
#include <Pbk2UID.h>
#include <csxhelp/phob.hlp.hrh>

// Virtual Phonebook
#include <MVPbkSimPhone.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkContactLink.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <aknnavide.h>
#include <aknnavi.h>
#include <StringLoader.h>
#include <eikmenup.h>
#include <barsread.h>
#include <aknnotewrappers.h>
#include <AknUtils.h>
#include <AiwCommon.hrh>
#include <avkon.hrh>

/// Unnamed namespace for local definitions
namespace {

#ifdef _DEBUG

enum TPanicCode
    {
    EAddFindTextL_Logic = 0
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "CPbk2CreateNewContactCmd");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG

/**
 * Checks is the given field type included in
 * the given selection.
 *
 * @param aResourceId   Selector's resource id.
 * @param aFieldType    The field type to check.
 * @param aManager      Virtual Phonebook contact manager.
 * @return  ETrue if field type is included.
 */
TBool IsFieldTypeIncludedL(
        const MVPbkFieldType& aFieldType,
        const CVPbkContactManager& aManager,
        const TInt aResourceId )
    {
    // Get the field type
    TResourceReader resReader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (resReader, aResourceId);

    CVPbkFieldTypeSelector* selector =
        CVPbkFieldTypeSelector::NewL(resReader, aManager.FieldTypes());
    CleanupStack::PopAndDestroy(); // resReader

    TBool ret = selector->IsFieldTypeIncluded(aFieldType);
    delete selector;
    return ret;
    }

/**
 * Displays a note.
 *
 * @param aResourceId   Note resource.
 */
void ShowNoteL( TInt aResourceId )
    {
    HBufC* prompt = StringLoader::LoadLC( aResourceId );
    CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote( EFalse );
    dlg->ExecuteLD( *prompt );
    CleanupStack::PopAndDestroy(); // prompt
    }

/**
 * Returns ETrue if shift is depressed in given key event.
 *
 * @param aKeyEvent     Key event.
 * @return  ETrue if shift is pressed.
 */
inline TBool ShiftDown
        (const TKeyEvent& aKeyEvent)
    {
    return (aKeyEvent.iModifiers &
        (EModifierShift | EModifierLeftShift | EModifierRightShift)) != 0;
    }

} /// namespace

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::CPsu2FixedDialingView
// --------------------------------------------------------------------------
//
CPsu2FixedDialingView::CPsu2FixedDialingView(
        CPbk2UIExtensionView& aExtensionView,
        CPsu2ViewManager& aViewManager ) :
    CPsu2NameListViewBase( aExtensionView, aViewManager ),
    iShowFdnNotActiveNote( ETrue ),
    iMarkingModeOn( EFalse )
    {
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::~CPsu2FixedDialingView
// --------------------------------------------------------------------------
//
CPsu2FixedDialingView::~CPsu2FixedDialingView()
    {
    delete iFdnCall;
    delete iContactLink;
    delete iContact;
    delete iContactRetriever;
    delete iWaitOtherComplete;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::NewL
// --------------------------------------------------------------------------
//
CPsu2FixedDialingView* CPsu2FixedDialingView::NewL(
        CPbk2UIExtensionView& aExtensionView,
        CPsu2ViewManager& aViewManager )
    {
    CPsu2FixedDialingView* self = new (ELeave) CPsu2FixedDialingView(
            aExtensionView, aViewManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::ConstructL()
    {
    iFdnStore =
        Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager().
            ContactStoresL().Find
                ( VPbkContactStoreUris::SimGlobalFdnUri() );
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPsu2FixedDialingView::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    const TInt count = iControl->NumberOfContacts();
    const TBool marked = iControl->ContactsMarked();
    TBool itemSpecCommEnabled = Phonebook2::Pbk2AppUi()->ActiveView()->MenuBar()->ItemSpecificCommandsEnabled();
    
    if ( aType == EEventKey )
        {
        if ( itemSpecCommEnabled && ( aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter ) )
            {
            if (!ShiftDown(aKeyEvent))  // pure OK/Enter key
                {
                if ( marked || count <= 0 )
                    {
                    iExtensionView.LaunchPopupMenuL(
                        R_PSU2_FIXED_DIALING_CONTEXT_MENUBAR );
                    }
                else
                    {
                    if ( iControl->NumberOfContacts() > 0 )
                        {
                        OpenInfoViewCmdL( *iControl );
                        }
                    }
                return ETrue;
                }
            }
        // Send key
        if ( itemSpecCommEnabled && aKeyEvent.iCode == EKeyPhoneSend )
            {
            if ( count > 0 && !marked )
                {
                CreateCallL( EPbk2CmdCall );
                }
            return ETrue;
            }
        // Delete key
        else if ( ( itemSpecCommEnabled || marked ) && aKeyEvent.iCode == EKeyBackspace )
            {
             if ( iControl->FindTextL() == KNullDesC )
                {
                HandleCommandL( EPbk2CmdDeleteMe );
                return ETrue;
                }
            }
        else if ( aKeyEvent.iScanCode == EStdKeyNo )
            {
            // 'Red' button pressed, clear PIN2 query info
            iViewManager.SecUi().Reset();
            iShowFdnNotActiveNote = ETrue;
            }
        else { /* Empty to make PC-lint happy */ }
        }
        
    if ( ShiftDown( aKeyEvent ) )
        {
        // Update cbas when shift and msk is pressed.
        UpdateCbasL();
        }

    return CPsu2NameListViewBase::HandleCommandKeyL( aKeyEvent, aType );
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::HandlePointerEventL(
        const TPointerEvent& /*aPointerEvent*/ )
    {
    // nothing to do
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::DoActivateL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::DoActivateL(
        const TVwsViewId& aPrevViewId,
        TUid aCustomMessageId,
        const TDesC8& aCustomMessage)
    {
    // Load FDN title
    HBufC* title = StringLoader::LoadLC(R_TEXT_FDN_LIST);
    // Create FDN application context pane icon
    TContextPaneIcon icon(EPsu2qgn_menu_simfdn);
    CEikImage* image = icon.CreateLC();

    // Set up view
    MPbk2ViewActivationTransaction* viewActivationTransaction =
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
            HandleViewActivationLC
                ( iExtensionView.Id(), aPrevViewId, title, image,
                  Phonebook2::EUpdateNaviPane |
                  Phonebook2::EUpdateContextPane |
                  Phonebook2::EUpdateTitlePane );

    // Call base class
    CPsu2NameListViewBase::DoActivateL(
            aPrevViewId, aCustomMessageId, aCustomMessage );

    viewActivationTransaction->Commit();
    CleanupStack::PopAndDestroy(3, title); // viewActivationTransaction,

    // Update navi pane                    // icon, title
    UpdateNaviPaneTextL();
   
    if( aPrevViewId.iViewUid != TUid::Uid( EPsu2FixedDialingInfoViewId ) )
    	{
    	iShowFdnNotActiveNote = ETrue;
    	// Clear PIN2 query info
    	iViewManager.SecUi().Reset();
    	}
    // Don't show note here, because there are many controls intializing, drawing and layouting 
    // will make the note flash. So delay the note pop up time to aviod the problem.
    iWaitOtherComplete = CIdle::NewL( CActive::EPriorityIdle );
    TCallBack callback( WaitOtherCompleteL, this );
    iWaitOtherComplete->Start( callback );
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::WaitOtherComplete
// --------------------------------------------------------------------------
//
TInt CPsu2FixedDialingView::WaitOtherCompleteL( TAny* object )
    {
    STATIC_CAST( CPsu2FixedDialingView*, object )->CheckFDNActivityL();
    return 0;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::CheckFDNActivity
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::CheckFDNActivityL()
    {
    if ( iShowFdnNotActiveNote )
        {
        if ( !iViewManager.SecUi().IsFDNActive() )
            {
            ShowNoteL( R_QTN_FDN_NOTE_NOT_ACTIVE );
            }
        iShowFdnNotActiveNote = EFalse;
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::DoDeactivate()
    {
    DestroyNaviPaneText();
    CPsu2NameListViewBase::DoDeactivate();
    
    delete iWaitOtherComplete;
    iWaitOtherComplete = NULL;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::HandleCommandL(TInt aCommand)
    {
    iCommandIsBeingHandled = ETrue;
    if ( Phonebook2::Pbk2AppUi()->ApplicationServices().CommandHandlerL()->
            ServiceCmdByMenuCmd ( aCommand ) == KAiwCmdCall )
        {
        CreateCallL( aCommand );
        }
    else
        {
        switch( aCommand )
            {
            case EPsu2CmdOpenFixedDialingInfoView: // FALLTHROUGH
            case EPsu2CmdActivateFDN: // FALLTHROUGH
            case EPsu2CmdDeactivateFDN: // FALLTHROUGH
            case EPsu2CmdNewContact: // FALLTHROUGH
            case EPsu2CmdCopyFromContacts: // FALLTHROUGH
            case EPbk2CmdEditMe:        // FALLTHROUGH
            case EPbk2CmdDeleteMe:
                {
                if ( iViewManager.StoreAvailableL() )
                    {
                    switch( aCommand )
                        {
                        case EPsu2CmdOpenFixedDialingInfoView:
                            {
                            OpenInfoViewCmdL( *iControl );
                            break;
                            }
                        case EPsu2CmdActivateFDN:
                            {
                            ActivateFDNCmdL();
                            break;
                            }
                        case EPsu2CmdDeactivateFDN:
                            {
                            DeactivateFDNCmdL();
                            break;
                            }
                        case EPsu2CmdNewContact:
                            {
                            if ( !FdnStoreFullL() &&
                                iViewManager.SecUi().ConfirmPin2L() )
                                {
                                CreateNewFdnContactL();
                                }
                            break;
                            }
                        case EPsu2CmdCopyFromContacts:
                            {
                            if ( !FdnStoreFullL() &&
                                    iViewManager.SecUi().ConfirmPin2L() )
                                {
                                CPsu2NameListViewBase::HandleCommandL( aCommand );
                                }
                            break;
                            }
                        case EPbk2CmdEditMe:
                            {
                            if ( iViewManager.SecUi().ConfirmPin2L() )
                                {
                                delete iContactLink;
                                iContactLink = NULL;
                                iContactLink = 
                                    iControl->FocusedContactL()->CreateLinkLC();
                                CleanupStack::Pop();
                                iContactRetriever = 
                                    Phonebook2::Pbk2AppUi()->ApplicationServices().
                                        ContactManager().RetrieveContactL(
                                            *iContactLink, *this );
                                }
                            break;
                            }
                        default: 
                            {
                            if ( iViewManager.SecUi().ConfirmPin2L() )
                                {
                                CPsu2NameListViewBase::HandleCommandL( aCommand );
                                }
                            break;
                            }
                        }
                    }
                break;
                }
            case EPbk2CmdExit:          // FALLTHROUGH
            case EAknSoftkeyBack:
            case EAknCmdHideInBackground:
                {
                // Clear PIN2 query info
                iViewManager.SecUi().Reset();
                // If the note has not popped up, cancel the popping up of the note.
                if( iWaitOtherComplete->IsActive() )
                    {
                    iWaitOtherComplete->Cancel();
                    }
                iShowFdnNotActiveNote = ETrue;
                
                if ( aCommand == EAknCmdHideInBackground && iDlgEliminator )
                    {
                    iDlgEliminator->RequestExitL( EKeyEscape );
                    }
                
                CPsu2NameListViewBase::HandleCommandL( aCommand );
                break;
                }
            default:
                {
                CPsu2NameListViewBase::HandleCommandL( aCommand );
                break;
                }
            }
        }
    // Update CBAs, if command handler is consuming the command
    // postcommandexecution is updating cbas
    UpdateCbasL();
    iCommandIsBeingHandled = EFalse;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    const TInt count = iControl->NumberOfContacts();
    const TBool marked = iControl->ContactsMarked();

    switch( aResourceId )
        {
        case R_PSU2_FIXED_DIALING_OPEN_MENUPANE:
            {
            if ( count <= 0 || marked )
                {
                aMenuPane->SetItemDimmed
                    ( EPsu2CmdOpenFixedDialingInfoView , ETrue );
                }
            break;
            }
        case R_PSU2_FIXED_DIALING_MENUPANE:
            {
            if ( iViewManager.SecUi().IsFDNActive() )
                {
                aMenuPane->SetItemDimmed( EPsu2CmdActivateFDN, ETrue );
                }
            else
                {
                aMenuPane->SetItemDimmed( EPsu2CmdDeactivateFDN, ETrue );
                }
            if ( count <= 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdEditMe , ETrue );
                aMenuPane->SetItemDimmed( EPbk2CmdDeleteMe , ETrue );
                }
            else
                {
                if( marked )
                    {
                    aMenuPane->SetItemDimmed( EPsu2CmdNewContact , ETrue );
                    aMenuPane->SetItemDimmed( EPbk2CmdEditMe , ETrue );
                    }
                }
            break;
            }
        case R_PSU2_FIXED_DIALING_COPY_MENUPANE:        // FALLTHROUGH
        case R_PSU2_FIXED_DIALING_COPY_CONTEXT_MENUPANE:
            {
            if ( count <= 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdCopy, ETrue );
                }
            break;
            }
        case R_AVKON_MENUPANE_MARK_MULTIPLE:
                {
                TInt markedContactCount = 0;
              
                CCoeControl* ctrl = iControl->ComponentControl(0);
                CEikListBox* listbox = static_cast <CEikListBox*> (ctrl);
                if ( listbox )
                    {
                    markedContactCount = listbox->SelectionIndexes()->Count();
                    }
                // dim the makr all item if all contacts are marked. 
                if ( markedContactCount > 0 && markedContactCount == iControl->NumberOfContacts() )
                    {
                    aMenuPane->SetItemDimmed( EAknCmdMarkingModeMarkAll, ETrue );
                    }
             
                break;
            }
        case R_PSU2_FIXED_DIALING_DELETE_MENUPANE:
            {
            if ( count <= 0 )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdDeleteMe , ETrue );
                }
            break;
            }
        case R_PSU2_FIXED_DIALING_NEWCONTACT_MENUPANE:
            {
            if ( count > 0 )
                {
                aMenuPane->SetItemDimmed( EPsu2CmdNewContact , ETrue );
                }
            break;
            }
        default:
            {
            CPsu2NameListViewBase::DynInitMenuPaneL
                ( aResourceId, aMenuPane );
            break;
            }
        };
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::UpdateCbasL()
    {
    if ( iControl )
        {
        if ( iControl->NumberOfContacts() > 0 && !iControl->ContactsMarked() )
            {
            // Set middle softkey as Open.
            iExtensionView.Cba()->SetCommandSetL(
                    R_PSU_FDN_SOFTKEYS_OPTIONS_BACK_OPEN );
            iExtensionView.Cba()->DrawDeferred();
            }
        else
            {
            // Set middle softkey as Context menu.
            iExtensionView.Cba()->SetCommandSetL(
                    R_PSU_SOFTKEYS_OPTIONS_BACK_CONTEXT );
            iExtensionView.Cba()->DrawDeferred();
            // Change context menu when marked items
            iExtensionView.MenuBar()->SetContextMenuTitleResourceId(
                    R_PSU2_FIXED_DIALING_CONTEXT_MENUBAR );
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::NameListControlResourceId
// --------------------------------------------------------------------------
//
TInt CPsu2FixedDialingView::NameListControlResourceId() const
    {
    return R_PSU2_FIXED_DIALING_NAME_LIST_CONTROL;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::ContactEditingComplete
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::ContactEditingComplete(
        MVPbkStoreContact* aEditedContact)
    {
    if( iControl )
        {
        // Reset Find before setting the focus because if new contact is created
        // through find box, ResetFindL corrupts the focus.
        TRAP_IGNORE( iControl->ResetFindL() );
        if (aEditedContact)
            {
            // Focus the created contact
            TRAPD( error, iControl->SetFocusedContactL( *aEditedContact ) );
            if ( error != KErrNone )
                {
                CCoeEnv::Static()->HandleError( error );
                }
            }
        }
    delete aEditedContact; // not needed anymore
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::ContactEditingDeletedContact
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::ContactEditingDeletedContact(
        MVPbkStoreContact* aEditedContact)
    {
    delete aEditedContact; // not needed anymore
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::ContactEditingAborted
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::ContactEditingAborted()
    {
    // Do nothing
    }
    
// --------------------------------------------------------------------------
// CPsu2FixedDialingView::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//    
void CPsu2FixedDialingView::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation,
        MVPbkStoreContact* aContact )
    {
    
    if ( iContactRetriever == &aOperation && aContact)
        {
        delete iContactRetriever;
        iContactRetriever = NULL;
        
        delete iContact;    
        iContact = aContact;     
               
        TRAPD( error, iContact->LockL(*this) );
        if ( error != KErrNone )
            {
            CCoeEnv::Static()->HandleError(error);
            } 
        }      
    }
        
// --------------------------------------------------------------------------
// CPsu2FixedDialingView::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//        
void CPsu2FixedDialingView::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& aOperation, 
        TInt aError )
    {
    if ( iContactRetriever == &aOperation )
        {
        delete iContactRetriever;
        iContactRetriever = NULL;
        
        CCoeEnv::Static()->HandleError(aError);
        }    
    }    
    
// --------------------------------------------------------------------------
// CPsu2FixedDialingView::ContactOperationCompleted
// --------------------------------------------------------------------------
//     
void CPsu2FixedDialingView::ContactOperationCompleted(
        TContactOpResult /*aResult*/ )
    {
    TRAPD(result, EditFdnContactL());
        
    if (result != KErrNone)
        {
        CCoeEnv::Static()->HandleError(result);
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::ContactOperationFailed
// --------------------------------------------------------------------------
//     
void CPsu2FixedDialingView::ContactOperationFailed(
        TContactOp /*aOpCode*/, 
        TInt aErrorCode, 
        TBool /*aErrorNotified*/ )
    {
    CCoeEnv::Static()->HandleError(aErrorCode);
    }    

// -----------------------------------------------------------------------------
// CPsu2FixedDialingView::MarkingModeStatusChanged
// -----------------------------------------------------------------------------
//
void CPsu2FixedDialingView::MarkingModeStatusChanged( TBool aActivated )
    {
    iMarkingModeOn = aActivated;
    }

// -----------------------------------------------------------------------------
// CPsu2FixedDialingView::ExitMarkingMode
// Called by avkon, if the return value is ETrue, 
// the Marking mode will be canceled after any operation, 
// otherwise the Marking mode keep active.
// -----------------------------------------------------------------------------
//
TBool CPsu2FixedDialingView::ExitMarkingMode() const
    {
    return ETrue; 
    }
    


// --------------------------------------------------------------------------
// CPsu2FixedDialingView::UpdateNaviPaneTextL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::UpdateNaviPaneTextL()
    {
    HBufC* text = NULL;
    if (iViewManager.SecUi().IsFDNActive())
        {
        text = StringLoader::LoadLC(R_QTN_FDN_ACTIVE);
        }
    else
        {
        text = StringLoader::LoadLC(R_QTN_FDN_DEACTIVE);
        }

    DestroyNaviPaneText();
    if (!iNaviPane)
        {
        iNaviPane = static_cast<CAknNavigationControlContainer*>(
            iAvkonAppUi->StatusPane()->ControlL(
            TUid::Uid(EEikStatusPaneUidNavi)));
        }
    iNaviDecorator = iNaviPane->CreateNavigationLabelL(*text);
    iNaviPane->PushL(*iNaviDecorator);

    CleanupStack::PopAndDestroy(); // text
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::DestroyNaviPaneText
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::DestroyNaviPaneText()
    {
    if (iNaviPane && iNaviDecorator)
        {
        iNaviPane->Pop(iNaviDecorator);
        }
    delete iNaviDecorator;
    iNaviDecorator = NULL;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::OpenInfoViewCmdL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::OpenInfoViewCmdL
        (MPbk2ContactUiControl& aUiControl) const
    {
    if (!aUiControl.ContactsMarked())
        {
        CPbk2ViewState* state = aUiControl.ControlStateL();
        CleanupStack::PushL(state);
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
            ActivatePhonebook2ViewL
                (TUid::Uid(EPsu2FixedDialingInfoViewId), state);
        CleanupStack::PopAndDestroy( state );  // state
        aUiControl.UpdateAfterCommandExecution();
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::ActivateFDNCmdL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::ActivateFDNCmdL()
    {
    if ( !iViewManager.SecUi().IsFDNActive() )
        {
        iViewManager.SecUi().ActivateFDNL();
        UpdateNaviPaneTextL();
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::DeactivateFDNCmdL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::DeactivateFDNCmdL()
    {
    if ( iViewManager.SecUi().IsFDNActive() )
        {
        iViewManager.SecUi().DeactivateFDNL();
        UpdateNaviPaneTextL();
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::CreateNewFdnContactL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::CreateNewFdnContactL()
    {
    MVPbkStoreContact* newFdnContact = iFdnStore->CreateNewContactLC();

    // Add find text to last name field, if any
    AddFindTextL(*newFdnContact);

    TUint32 flags = TPbk2ContactEditorParams::ENewContact;
    
    TCoeHelpContext helpContext;
    helpContext.iMajor.iUid = KPbk2UID3;
    helpContext.iContext = KFDN_HLP_FDN_NUM_EDIT_VIEW;
        
    TPbk2ContactEditorParams params( flags, NULL, &helpContext );
    CPbk2ContactEditorDlg* editor =
        CPbk2ContactEditorDlg::NewL(params, newFdnContact, *this);
    CleanupStack::Pop(); // newFdnContact, whose ownership was taken away
    
    iDlgEliminator = editor;
    editor->ExecuteLD();
    iDlgEliminator = NULL;
    }


// --------------------------------------------------------------------------
// CPsu2FixedDialingView::AddFindTextL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::AddFindTextL(MVPbkStoreContact& aContact)
    {
    const TDesC& findText = iControl->FindTextL();

    if (findText != KNullDesC)
        {
        CPbk2FieldPropertyArray& fieldProperties =
            Phonebook2::Pbk2AppUi()->ApplicationServices().FieldProperties();

        const TInt fieldCount = fieldProperties.Count();
        for (TInt i=0; i<fieldCount; ++i)
            {
            const MPbk2FieldProperty& prop = fieldProperties.At(i);

            if (IsFieldTypeIncludedL(prop.FieldType(),
                    Phonebook2::Pbk2AppUi()->ApplicationServices().
                        ContactManager(),
                    R_PHONEBOOK2_LAST_NAME_SELECTOR))
                {
                MVPbkStoreContactField* field = aContact.CreateFieldLC
                    (prop.FieldType());

                __ASSERT_DEBUG(field->FieldData().DataType() ==
                    EVPbkFieldStorageTypeText, Panic(EAddFindTextL_Logic));

                MVPbkContactFieldTextData* fieldData =
                    &MVPbkContactFieldTextData::Cast(field->FieldData());
                //The max length of a name in the Sim Card
                TInt maxLength = fieldData->MaxLength();
                fieldData->SetTextL( findText.Left( maxLength ));

                TInt index = aContact.AddFieldL(field);
                CleanupStack::Pop(); // field
                break;
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::FdnStoreFullL
// --------------------------------------------------------------------------
//
TBool CPsu2FixedDialingView::FdnStoreFullL()
    {
    TBool ret = EFalse;
    const MVPbkContactStoreInfo& storeInfo = iFdnStore->StoreInfo();
    if ( storeInfo.MaxNumberOfContactsL() <= storeInfo.NumberOfContactsL() )
        {
        ret = ETrue;
        ShowNoteL( R_QTN_FDN_LIST_FULL );
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::CreateCallL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingView::CreateCallL( TInt aCommand )
    {
    if ( !iFdnCall )
        {
        iFdnCall = CPsu2FixedDialingCall::NewL
            ( Phonebook2::Pbk2AppUi()->ApplicationServices().ContactManager(),
              *Phonebook2::Pbk2AppUi()->ApplicationServices().
                CommandHandlerL(),
              iExtensionView );
        }

    MVPbkContactLink* contactLink =
        iControl->FocusedContactL()->CreateLinkLC();
    CleanupStack::Pop(); // CreateCall takes contactLink ownership
    iFdnCall->CreateCallL( contactLink, aCommand );
    }
    
// --------------------------------------------------------------------------
// CPsu2FixedDialingView::EditFdnContactL
// --------------------------------------------------------------------------
//    
void CPsu2FixedDialingView::EditFdnContactL()
    {
    TCoeHelpContext helpContext;
    helpContext.iMajor.iUid = KPbk2UID3;
    helpContext.iContext = KFDN_HLP_FDN_NUM_EDIT_VIEW;
    
    TPbk2ContactEditorParams params;
    params.iHelpContext = &helpContext;
        
    // create and execute editing dialog
    CPbk2ContactEditorDlg* dlg =
        CPbk2ContactEditorDlg::NewL( params, iContact, *this );
    iContact = NULL; // ownership went to editor
    
    iDlgEliminator = dlg;
    dlg->ExecuteLD();      
    iDlgEliminator = NULL;
    }    

// --------------------------------------------------------------------------
// CPsu2FixedDialingView::HandleControlEventL
// --------------------------------------------------------------------------
//    
void CPsu2FixedDialingView::HandleControlEventL(
        MPbk2ContactUiControl& aControl,
        const TPbk2ControlEvent& aEvent )
    {
    const TInt count = iControl->NumberOfContacts();
    const TBool marked = iControl->ContactsMarked();
    
    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EContactTapped:
        case TPbk2ControlEvent::EContactDoubleTapped:
            {
            if ( marked )
                {
                iExtensionView.LaunchPopupMenuL(
                    R_PSU2_FIXED_DIALING_CONTEXT_MENUBAR );
                }
            else
                {
                // Open contact
                HandleCommandL( EPsu2CmdOpenFixedDialingInfoView );
                }
            break;
            }
        default:
            {
            CPsu2NameListViewBase::HandleControlEventL( aControl, aEvent );
            break;
            }
        }
    }

//  End of File
