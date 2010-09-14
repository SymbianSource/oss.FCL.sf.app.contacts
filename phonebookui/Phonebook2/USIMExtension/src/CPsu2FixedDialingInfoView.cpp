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
* Description:  Phonebook 2 USIM UI Extension FDN contact info view.
*
*/


// INCLUDE FILES
#include "CPsu2FixedDialingInfoView.h"

// Phonebook 2
#include "CPsu2ViewManager.h"
#include "CPsu2FixedDialingCall.h"
#include "CPsu2SecUi.h"
#include <CPbk2UIExtensionView.h>
#include <MPbk2ViewActivationTransaction.h>
#include <CPbk2AppUiBase.h>
#include <MPbk2ViewExplorer.h>
#include <pbk2usimuires.rsg>
#include <CPbk2ViewState.h>
#include <MPbk2ContactUiControl.h>
#include <pbk2uicontrols.rsg>
#include <MPbk2CommandHandler.h>
#include <MPbk2ApplicationServices.h>
#include <TPbk2ContactEditorParams.h>
#include <CPbk2ContactEditorDlg.h>
#include <Pbk2UID.h>
#include <csxhelp/phob.hlp.hrh>
#include <CPbk2PresentationContact.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <MPbk2PointerEventInspector.h>

// Virtual Phonebook
#include <MVPbkSimPhone.h>
#include <MVPbkBaseContact.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactOperationBase.h>

// System includes
#include <StringLoader.h>
#include <AknUtils.h>
#include <AiwCommon.hrh>

// Debugging headers
#include <Pbk2Debug.h>

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::CPsu2FixedDialingInfoView
// --------------------------------------------------------------------------
//
CPsu2FixedDialingInfoView::CPsu2FixedDialingInfoView
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager ) :
            CPsu2InfoViewBase( aExtensionView, aViewManager )
    {
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::~CPsu2FixedDialingInfoView
// --------------------------------------------------------------------------
//
CPsu2FixedDialingInfoView::~CPsu2FixedDialingInfoView()
    {
    delete iFdnCall;
    delete iContactLink;
    delete iContact;
    delete iContactRetriever;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::NewL
// --------------------------------------------------------------------------
//
CPsu2FixedDialingInfoView* CPsu2FixedDialingInfoView::NewL
        ( CPbk2UIExtensionView& aExtensionView,
          CPsu2ViewManager& aViewManager )
    {
    CPsu2FixedDialingInfoView* self =
        new ( ELeave ) CPsu2FixedDialingInfoView
            ( aExtensionView, aViewManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::ConstructL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::ConstructL()
    {
    BaseConstructL();
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::DoActivateL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::DoActivateL
        ( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
          const TDesC8& aCustomMessage)
    {
    // Set up view
    delete iViewActivationTransaction;
    iViewActivationTransaction = NULL;
    iViewActivationTransaction =
        Phonebook2::Pbk2AppUi()->Pbk2ViewExplorer()->
            HandleViewActivationLC( iExtensionView.Id(), aPrevViewId,
                NULL, NULL, Phonebook2::EUpdateNaviPane );
    CleanupStack::Pop(); //iViewActivationTransaction

    // Call base class
    CPsu2InfoViewBase::DoActivateL
        ( aPrevViewId, aCustomMessageId, aCustomMessage );
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::DoDeactivate()
    {
    CPsu2InfoViewBase::DoDeactivate();
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPsu2FixedDialingInfoView::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TBool ret = EFalse;

    if ( aType == EEventKey )
        {
        if( aKeyEvent.iCode == EKeyPhoneSend )
            {
            CreateCallL( EPbk2CmdCall );
            ret = ETrue;
            }
        else if( aKeyEvent.iCode == EKeyBackspace )
            {
            HandleCommandL( EPbk2CmdDeleteMe );
            ret = ETrue;
            }
        else
            {
            if ( aKeyEvent.iScanCode == EStdKeyNo )
                {
                // 'Red' button pressed, clear PIN2 query info
                iViewManager.SecUi().Reset();
                }
            }            
        }
    else
        {
        ret = CPsu2InfoViewBase::HandleCommandKeyL( aKeyEvent, aType );        
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::HandleCommandL( TInt aCommand )
    {
    if ( Phonebook2::Pbk2AppUi()->ApplicationServices().CommandHandlerL()->
            ServiceCmdByMenuCmd( aCommand ) == KAiwCmdCall )
        {
        if ( iViewManager.StoreAvailableL() )
            {
            CreateCallL( aCommand );
            }
        }
    else
        {
        switch( aCommand )
            {
            case EPbk2CmdEditMe:
                {
                if ( iViewManager.SecUi().ConfirmPin2L() )
                    {     
                    delete iContactLink;
                    iContactLink = NULL;               
                    iContactLink = iControl->FocusedContactL()->CreateLinkLC();
                    CleanupStack::Pop();
                    iContactRetriever = 
                        Phonebook2::Pbk2AppUi()->ApplicationServices().
                            ContactManager().RetrieveContactL(
                                *iContactLink, *this );
                    }
                break;
                }
            case EPbk2CmdDeleteMe:
                {
                if ( iViewManager.SecUi().ConfirmPin2L() )
                    {
                    CPsu2InfoViewBase::HandleCommandL( aCommand );
                    }
                break;
                }
            case EPbk2CmdExit:
            case EAknCmdHideInBackground:
                {
                if ( aCommand == EAknCmdHideInBackground && iDlgEliminator )
                    {
                    iDlgEliminator->RequestExitL( EKeyEscape );
                    }

                // Clear PIN2 query info
                iViewManager.SecUi().Reset();
                CPsu2InfoViewBase::HandleCommandL( aCommand );
                break;
                }
            default:
                {
                CPsu2InfoViewBase::HandleCommandL( aCommand );
                break;
                }
            };
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::DynInitMenuPaneL(
        TInt aResourceId,
        CEikMenuPane* aMenuPane )
    {
    switch( aResourceId )
        {
        case R_PSU2_FIXED_DIALING_INFO_COPY_MENUPANE:
            {
            break;
            }
        case R_PSU2_FIXED_DIALING_INFO_MENUPANE:
            {
            break;
            }
        default:
            {
            CPsu2InfoViewBase::DynInitMenuPaneL( aResourceId, aMenuPane );
            break;
            }
        }
    }


// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::NameListControlResourceId
// --------------------------------------------------------------------------
//
TInt CPsu2FixedDialingInfoView::NameListControlResourceId() const
    {
    return 0;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::NaviPaneLabelL
// --------------------------------------------------------------------------
//
HBufC* CPsu2FixedDialingInfoView::NaviPaneLabelL() const
    {
    HBufC* text = NULL;
    if ( iViewManager.SecUi().IsFDNActive() )
        {
        text = StringLoader::LoadLC( R_QTN_FDN_ACTIVE );
        }
    else
        {
        text = StringLoader::LoadLC( R_QTN_FDN_DEACTIVE );
        }

    CleanupStack::Pop(); // text
    return text;
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::UpdateCbasL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::UpdateCbasL()
    {
    // Set cba command set "Options - Context Menu - Back"
    iExtensionView.Cba()->SetCommandSetL
        ( R_PBK2_SOFTKEYS_OPTIONS_BACK_CONTEXT );
    iExtensionView.Cba()->DrawDeferred();
    // Set context menu
    iExtensionView.MenuBar()->SetContextMenuTitleResourceId
        ( R_PSU2_FIXED_DIALING_INFO_CONTEXT_MENUBAR );
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::VPbkSingleContactOperationComplete
// --------------------------------------------------------------------------
//    
void CPsu2FixedDialingInfoView::VPbkSingleContactOperationComplete(
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
// CPsu2FixedDialingInfoView::VPbkSingleContactOperationFailed
// --------------------------------------------------------------------------
//        
void CPsu2FixedDialingInfoView::VPbkSingleContactOperationFailed(
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
// CPsu2FixedDialingInfoView::ContactEditingCompletebasL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::ContactEditingComplete(
        MVPbkStoreContact* aEditedContact )
    {  
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2FixedDialingInfoView(%x)::ContactEditingComplete()"), 
        this);
        
    iContact = aEditedContact;                      
    }
    
// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::ContactEditingDeletedContact
// --------------------------------------------------------------------------
//        
void CPsu2FixedDialingInfoView::ContactEditingDeletedContact(
        MVPbkStoreContact* aEditedContact )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2FixedDialingInfoView(%x)::ContactEditingDeletedContact()"), 
        this);
    
    delete aEditedContact; // not needed anymore
    }
    
// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::ContactEditingAborted
// --------------------------------------------------------------------------
//        
void CPsu2FixedDialingInfoView::ContactEditingAborted()
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::ContactOperationCompleted
// --------------------------------------------------------------------------
//     
void CPsu2FixedDialingInfoView::ContactOperationCompleted(
        TContactOpResult /*aResult*/ )
    {
    TRAPD(result, EditFdnContactL());
        
    if (result != KErrNone)
        {
        CCoeEnv::Static()->HandleError(result);
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::ContactOperationFailed
// --------------------------------------------------------------------------
//     
void CPsu2FixedDialingInfoView::ContactOperationFailed(
        TContactOp /*aOpCode*/, 
        TInt aErrorCode, 
        TBool /*aErrorNotified*/ )
    {
    CCoeEnv::Static()->HandleError(aErrorCode);
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::CreateCallL
// --------------------------------------------------------------------------
//
void CPsu2FixedDialingInfoView::CreateCallL( TInt aCommand )
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
    CleanupStack::Pop(); // CreateCallL takes contactLink
    iFdnCall->CreateCallL( contactLink, aCommand );
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::EditFdnContactL
// --------------------------------------------------------------------------
//    
void CPsu2FixedDialingInfoView::EditFdnContactL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPsu2FixedDialingInfoView(%x)::EditFdnContactL()"), this);
         
    // adjust focus
    TInt focusedFieldIndex = KErrNotFound;
    if (iControl)
        {
        focusedFieldIndex = iControl->FocusedFieldIndex();
        }
        
    TPbk2ContactEditorParams params;
    if ( focusedFieldIndex != KErrNotFound )
        {
        params.iFocusedContactField = 
            FocusedFieldLC( *iContact );
        }

    TCoeHelpContext helpContext;
    helpContext.iMajor.iUid = KPbk2UID3;
    helpContext.iContext = KFDN_HLP_FDN_NUM_EDIT_VIEW;
    params.iHelpContext = &helpContext;
        
    // create and execute editing dialog
    CPbk2ContactEditorDlg* dlg =
        CPbk2ContactEditorDlg::NewL( params, iContact, *this );
    iContact = NULL; // ownership went to editor
    iDlgEliminator = dlg;
    dlg->ExecuteLD();
    iDlgEliminator = NULL;
    
    // clean after dialog execution
    if ( focusedFieldIndex != KErrNotFound )
        {
        CleanupStack::PopAndDestroy(); //params.iFocusedContactField
        }    
    
    if ( iControl )
        {        
        // If editing has been aborted we don't set focused contact.
        if ( iContact )
            {
            // It is possible that the contact got changed while in edit
            iControl->SetFocusedFieldIndex( params.iFocusedIndex );
            iControl->SetFocusedContactL( *iContact );
            delete iContact;
            iContact = NULL;
            }

        // If contact got deleted in the editor, this call ensures
        // that the contact info view is switched to the names list
        iControl->UpdateAfterCommandExecution();        
        }
    }

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::FocusedFieldLC
// --------------------------------------------------------------------------
//    
MVPbkStoreContactField* CPsu2FixedDialingInfoView::FocusedFieldLC( 
        MVPbkStoreContact& aStoreContact )
    {
    CPbk2PresentationContact* presentationContact = 
      CPbk2PresentationContact::NewL( aStoreContact, 
        Phonebook2::Pbk2AppUi()->ApplicationServices().FieldProperties() );
    CleanupStack::PushL( presentationContact );

    TInt index = KErrNotFound;
    if (iControl)
        {
        index = presentationContact->PresentationFields().StoreIndexOfField( 
                iControl->FocusedFieldIndex() );
        }

    CleanupStack::PopAndDestroy( presentationContact );
    
    if ( index != KErrNotFound )
        {
        // Use FieldAtLC to avoid the unvalidity of the field after new
        // FieldAt call.        
        return aStoreContact.Fields().FieldAtLC( index );
        }
    return NULL;
    }     

// --------------------------------------------------------------------------
// CPsu2FixedDialingInfoView::HandleControlEventL
// --------------------------------------------------------------------------
//    
void CPsu2FixedDialingInfoView::HandleControlEventL(
        MPbk2ContactUiControl& aControl,
        const TPbk2ControlEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case TPbk2ControlEvent::EContactTapped:
        case TPbk2ControlEvent::EContactDoubleTapped:
        case TPbk2ControlEvent::EControlEnterKeyPressed:
            {
            HandleCommandL( EPbk2CmdEditMe );
            break;
            }
        default:
            {
            CPsu2InfoViewBase::HandleControlEventL( aControl, aEvent );
            break;
            }
        }
    }

// End of File
