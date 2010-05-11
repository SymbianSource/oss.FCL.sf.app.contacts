/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the class CPbkxRclResultInfoDlg.
*
*/
#include <CPbk2AppUiBase.h>
#include <CPbk2AppViewBase.h>
#include <MPbk2AppUi.h>

#include "emailtrace.h"
#include <pbk2rclengine.rsg>
#include <cntitem.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <aknappui.h>
#include <akntabgrp.h>
#include <akntitle.h>
#include <StringLoader.h>
#include <aknstyluspopupmenu.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>
#include <CVPbkContactManager.h>
#include <eikfrlb.h>
#include <eikfrlbd.h>
#include <txtetext.h>   // CPlainText
#include <baclipb.h>    // CClipboard
#include <aknnotewrappers.h>    // CAknInformationNote
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <AknIconArray.h>

#include <csxhelp/cmail.hlp.hrh>

#include "cpbkxrclresultinfodlg.h"
#include "cpbkxrclcbrsender.h"
#include "cpbkxrclvcardsender.h"
#include "cpbkxrclactionservicewrapper.h"
#include "mpbkxrclcontactretrieval.h"
#include "cpbkxrclcontactupdater.h"
#include "pbkxrclutils.h"
#include "pbkxrclengineconstants.h"
#include "engine.hrh"
#include "cpbkxrclresultinfolistbox.h"
#include "fsccontactactionservicedefines.h"

const TInt KPbkxRemoteContactLookupServiceImplImpUid = 0x2001FE0D;

// list box field format
_LIT( KFieldFormat, "%d\t%S\t%S" );


// LOCAL CONSTANTS AND MACROS
//This order is based on 'Standard field ids' (PbkFields.hrh)
#ifdef __VOIP
const TUint KIcons[11] =               
    {
    EMbmAvkonQgn_prop_empty,
    EMbmAvkonQgn_prop_nrtyp_phone,
    EMbmAvkonQgn_prop_nrtyp_home,
    EMbmAvkonQgn_prop_nrtyp_work,
    EMbmAvkonQgn_prop_nrtyp_mobile,
    EMbmAvkonQgn_prop_nrtyp_video,
    EMbmAvkonQgn_prop_nrtyp_fax,
    EMbmAvkonQgn_prop_nrtyp_pager,
    EMbmAvkonQgn_prop_nrtyp_voip,      
    EMbmAvkonQgn_prop_nrtyp_email,
    EMbmAvkonQgn_prop_nrtyp_address
    };

const TInt KIconsCount(11);              

const TUint KIconsId[11] =               
    {
    EPbkqgn_prop_nrtyp_empty,        
    EPbkqgn_prop_nrtyp_phone,
    EPbkqgn_prop_nrtyp_home,
    EPbkqgn_prop_nrtyp_work,
    EPbkqgn_prop_nrtyp_mobile,
    EPbkqgn_prop_nrtyp_video,
    EPbkqgn_prop_nrtyp_fax,
    EPbkqgn_prop_nrtyp_pager,
    EPbkqgn_prop_nrtyp_voip,
    EPbkqgn_prop_nrtyp_email,
    EPbkqgn_prop_nrtyp_address    
    };

const TUint KIconsMask[11] =             
    {
    EMbmAvkonQgn_prop_empty_mask,
    EMbmAvkonQgn_prop_nrtyp_phone_mask,
    EMbmAvkonQgn_prop_nrtyp_home_mask,
    EMbmAvkonQgn_prop_nrtyp_work_mask,
    EMbmAvkonQgn_prop_nrtyp_mobile_mask,
    EMbmAvkonQgn_prop_nrtyp_video_mask,
    EMbmAvkonQgn_prop_nrtyp_fax_mask,
    EMbmAvkonQgn_prop_nrtyp_pager_mask,
    EMbmAvkonQgn_prop_nrtyp_voip_mask,
    EMbmAvkonQgn_prop_nrtyp_email_mask,
    EMbmAvkonQgn_prop_nrtyp_address_mask
    };
#else
const TUint KIcons[10] =               
    {
    EMbmAvkonQgn_prop_empty,
    EMbmAvkonQgn_prop_nrtyp_phone,
    EMbmAvkonQgn_prop_nrtyp_home,
    EMbmAvkonQgn_prop_nrtyp_work,
    EMbmAvkonQgn_prop_nrtyp_mobile,
    EMbmAvkonQgn_prop_nrtyp_video,
    EMbmAvkonQgn_prop_nrtyp_fax,
    EMbmAvkonQgn_prop_nrtyp_pager,
    EMbmAvkonQgn_prop_nrtyp_email,
    EMbmAvkonQgn_prop_nrtyp_address
    };

const TInt KIconsCount(10);              

const TUint KIconsId[10] =               
    {
    EPbkqgn_prop_nrtyp_empty,        
    EPbkqgn_prop_nrtyp_phone,
    EPbkqgn_prop_nrtyp_home,
    EPbkqgn_prop_nrtyp_work,
    EPbkqgn_prop_nrtyp_mobile,
    EPbkqgn_prop_nrtyp_video,
    EPbkqgn_prop_nrtyp_fax,
    EPbkqgn_prop_nrtyp_pager,    
    EPbkqgn_prop_nrtyp_email,
    EPbkqgn_prop_nrtyp_address    
    };

const TUint KIconsMask[10] =             
    {
    EMbmAvkonQgn_prop_empty_mask,
    EMbmAvkonQgn_prop_nrtyp_phone_mask,
    EMbmAvkonQgn_prop_nrtyp_home_mask,
    EMbmAvkonQgn_prop_nrtyp_work_mask,
    EMbmAvkonQgn_prop_nrtyp_mobile_mask,
    EMbmAvkonQgn_prop_nrtyp_video_mask,
    EMbmAvkonQgn_prop_nrtyp_fax_mask,
    EMbmAvkonQgn_prop_nrtyp_pager_mask,    
    EMbmAvkonQgn_prop_nrtyp_email_mask,
    EMbmAvkonQgn_prop_nrtyp_address_mask
    };
#endif

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CPbkxRclResultInfoDlg
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoDlg::CPbkxRclResultInfoDlg( 
    TInt& aIndex, 
    CDesCArray* aArray, 
    MEikCommandObserver* aCommand,
    MPbkxRclContactRetrieval* aContactRetrieval,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled,
    TInt aActiveItemIndex ) :
    CPbkxRclBaseDlg( aIndex, aArray, aCommand, EFalse ), iItems( aArray ), 
    iActiveTabIndex( aActiveItemIndex ), 
    iContactSelectorEnabled( aContactSelectorEnabled ),
    iContactRetrieval( aContactRetrieval ), iContactEngine( aContactEngine ),
    iContactActionService( aActionService ), iOkToExit(ETrue)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::ConstructL()
    {
    FUNC_LOG;
    
    CPbkxRclBaseDlg::ConstructL( R_RCL_RESULT_INFO_DIALOG_MENU_BAR );
   
    HBufC* text = StringLoader::LoadLC( R_QTN_RCL_OPENING_WAIT_NOTE );
    iWaitNoteText.CreateL( *text );
    CleanupStack::PopAndDestroy( text );

    iCallbackReqSender = CPbkxRclCbRSender::NewL();

    iVisibleFields = new ( ELeave ) CPbkFieldArray;
    
    iVisibleFocusOnList = EFalse;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoDlg* CPbkxRclResultInfoDlg::NewL( 
    TInt& aIndex, 
    CDesCArray* aArray,
    MEikCommandObserver* aCommand,
    MPbkxRclContactRetrieval* aContactRetrieval,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled,
    TInt aActiveItemIndex ) 
    {
    FUNC_LOG;

    CPbkxRclResultInfoDlg* self = new( ELeave ) CPbkxRclResultInfoDlg( 
        aIndex, 
        aArray, 
        aCommand,
        aContactRetrieval,
        aContactEngine,
        aActionService,
        aContactSelectorEnabled,
        aActiveItemIndex );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::~CPbkxRclResultInfoDlg
// ---------------------------------------------------------------------------
//
CPbkxRclResultInfoDlg::~CPbkxRclResultInfoDlg()
    {
    FUNC_LOG;
    
    if(iSkinIcons !=  NULL)
        {
        iSkinIcons->Close();
        delete iSkinIcons;
        }
    
    ResetStatusPane();
    delete iCurrentContact;
    iWaitNoteText.Close();
    delete iTitlePaneText;
    delete iCallbackReqSender;
    delete iVisibleFields;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    
    if ( aResourceId == R_RCL_RESULT_INFO_DIALOG_MENU_PANE )
        {

        if ( !iContactSelectorEnabled )
            {
            aMenuPane->SetItemDimmed( ERclCmdAddAsRecipient, ETrue );
            
            aMenuPane->SetItemDimmed(
                ERclCmdCall,
                !CallActionsAvailable() );
            
            // send menu is always available, since business card can 
            // always be sent
            }
        else
            {
            aMenuPane->SetItemDimmed( ERclCmdCall, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdSend, ETrue );
            }

        aMenuPane->SetItemDimmed( 
                    ERclCmdAddToContacts, 
                    !iContactActionService.IsActionEnabled( KFscAtManSaveAs ) );
        
        // talk menu is shown if we have PTT enabled
        aMenuPane->SetItemDimmed(
            ERclCmdTalk,
            !iContactActionService.IsActionEnabled( KFscAtComCallPoc ) );
        }
    
    else if ( aResourceId == R_RCL_INFO_CALL_MENU_PANE )
        {
        
        aMenuPane->SetItemDimmed( 
            ERclCmdVoiceCall, 
            !iContactActionService.IsActionEnabled( KFscAtComCallGSM ) );
        
        aMenuPane->SetItemDimmed( 
            ERclCmdVideoCall, 
            !iContactActionService.IsActionEnabled( KFscAtComCallVideo ) );
        
        aMenuPane->SetItemDimmed( 
            ERclCmdVoip, 
            !iContactActionService.IsActionEnabled( KFscAtComCallVoip ) );
        
        }
    else if ( aResourceId == R_RCL_INFO_TALK_MENU_PANE )
        {
        // PTT settings available, no need to hide items
        }
    else if ( aResourceId == R_RCL_INFO_SEND_MENU_PANE )
        {       
        aMenuPane->SetItemDimmed( 
            ERclCmdSendMsg, 
            !iContactActionService.IsActionEnabled( KFscAtComSendMsg ) );
       
        aMenuPane->SetItemDimmed( 
            ERclCmdSendEmail, 
            !iContactActionService.IsActionEnabled( KFscAtComSendEmail ) ); 
        
        aMenuPane->SetItemDimmed( 
            ERclCmdSendMeetingReq, 
            !iContactActionService.IsActionEnabled( KFscAtComSendCalReq ) );
        
        aMenuPane->SetItemDimmed( 
            ERclCmdSendAudioMsg, 
            !iContactActionService.IsActionEnabled( KFscAtComSendAudio ) );

		///Use Send Business Card Plugin
        aMenuPane->SetItemDimmed( 
            ERclCmdSendBusinessCard, 
            !iContactActionService.IsActionEnabled( KFscAtSendBusinessCard ) );
        }
    else if ( aResourceId == R_RCL_RESULT_INFO_DIALOG_CONTEXT_MENU_PANE )
        {
        TBool voiceCallOk = iContactActionService.IsActionEnabled( KFscAtComCallGSM );
        TBool videoCallOk = iContactActionService.IsActionEnabled( KFscAtComCallVideo );
        TBool sendMsgOk   = iContactActionService.IsActionEnabled( KFscAtComSendMsg );
        TBool sendEmailOk = iContactActionService.IsActionEnabled( KFscAtComSendEmail );
    
        TBool noPhoneNbr = !FocusOnPhoneNumberField() && (iVisibleFocusOnList || !iMskEvent);    
        TBool noEmailAddr= !FocusOnEmailAddressField() && (iVisibleFocusOnList || !iMskEvent);
        TBool noMsgAddr  = !(FocusOnPhoneNumberField() || FocusOnEmailAddressField()) && 
              (iVisibleFocusOnList || !iMskEvent);        
        
        if( !iVisibleFocusOnList && iMskEvent )    
            {
            aMenuPane->SetItemDimmed( ERclCmdCopyDetail, ETrue );    
            }    
        
        //Dim items if communication method not available or if visible focus somewhere else 
        //or if directly tapped tapped somewhere else.
        aMenuPane->SetItemDimmed( ERclCmdVoiceCall, !voiceCallOk || noPhoneNbr);
        aMenuPane->SetItemDimmed( ERclCmdVideoCall, !videoCallOk || noPhoneNbr);
        aMenuPane->SetItemDimmed( ERclCmdSendMsg,   !sendMsgOk   || noMsgAddr );
        aMenuPane->SetItemDimmed( ERclCmdSendEmail, !sendEmailOk || noEmailAddr);    
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::GetHelpContext
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    FUNC_LOG;
    aContext.iMajor = TUid::Uid( KPbkxRemoteContactLookupServiceImplImpUid );
    aContext.iContext = KRLOOK_HLP_INFO_VIEW;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::HandleListBoxEventL( 
    CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    FUNC_LOG;
    if ( AknLayoutUtils::PenEnabled() )
        {
        switch(aEventType)
            {
            case   EEventItemSingleClicked:
                {
                iMskEvent = EFalse;                
                iMenuBar->TryDisplayContextMenuBarL();					
                break;
                }
            default:
                break;
            }
        }    
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CPbkxRclResultInfoDlg::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    FUNC_LOG;

    if ( iExit || aType != EEventKey )
        {
        return EKeyWasConsumed;
        }
      
    switch ( aKeyEvent.iCode )
        {
        case EKeyEnter:
        case EKeyOK:
            {
            // open context sensitive menu from msk 
            iMskEvent = ETrue;
            iMenuBar->TryDisplayContextMenuBarL();
            }
            break;
        case EKeyUpArrow: // fall through, same functionality
        case EKeyDownArrow:
            {
            iVisibleFocusOnList = ETrue;
            CAknSelectionListDialog::OfferKeyEventL( aKeyEvent, aType );
            }
            break;
        
        case EKeyEscape:
            {
            // exit application
            iExit = ETrue;
            ProcessCommandL( EAknCmdExit );
            }
            break;
        default:
            break;
        }
    return EKeyWasConsumed;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SetOkToExit
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SetOkToExit(TBool aOkToExit)
    {
    FUNC_LOG;
    iOkToExit = aOkToExit;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::IsOkToExit
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::IsOkToExit()
    {
    FUNC_LOG;
    return iOkToExit;
    }
 
// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::OkToExitL( TInt aButtonId )
	{
    FUNC_LOG;
    
    // IsOkToExit() returns false if we shouldn't exit at this time. For example
    // dialog initialization in ExecuteLD still unfinished. 
    
    if( !IsOkToExit() || aButtonId == EAknSoftkeyContextOptions )
		{
		return EFalse;
		}
	else
		{
		return CAknDialog::OkToExitL( aButtonId );
		}
	}

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::ExecuteLD
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::ExecuteLD()
    {
    FUNC_LOG;
    // Need to set a guard flag on to avoid a situation where the 
    // dialog exits it self in the middle of this ExecuteLD call which 
    // eventually panics. This is a side effect from the heavy use of active wait
    // objects in the code. For example in CreateListBoxEntriesL the call
    // to SetCurrentContactL eventually ends up using active wait and this makes
    // it possible for the user to hit 'Back' key and exit the dialog. 
    SetOkToExit(EFalse);   
     
    // if current contact cannot be fetched, result info dialog is not opened
    if ( !FetchCurrentContactL() )
        {
        delete this;
        return;
        }

    PrepareLC( R_RCL_RESULT_INFO_DIALOG );
    
    UpdateGraphics();
    iMenuBar->SetContextMenuTitleResourceId( 
        R_RCL_RESULT_INFO_DIALOG_CONTEXT_MENU_BAR );    

    SetupStatusPaneL();

    LoadIconArrayL();    
    CreateListBoxEntriesL();
    
    SetOkToExit(ETrue);
    CAknSelectionListDialog::RunLD();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::Close
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::Close()
    {
    FUNC_LOG;
    // this will not leave, because TryExitL doesn't call leaving methods
    // when EAknSoftkeyCancel is used
    iExit = ETrue;
    TRAP_IGNORE( TryExitL( EAknSoftkeyCancel ) ); 
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SendCallbackRequestL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SendCallbackRequestL()
    {
    FUNC_LOG;
    iCallbackReqSender->SendCallbackRequestL( *iCurrentContact );
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SendBusinessCardL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SendBusinessCardL()
    {
    FUNC_LOG;
    CPbkxRclvCardSender* sender = CPbkxRclvCardSender::NewLC( iContactEngine );
    sender->SendvCardL( *iCurrentContact, *(iContactActionService.ContactConverter()) );
    CleanupStack::PopAndDestroy( sender );
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::UpdateExistingContactL
// ---------------------------------------------------------------------------
//
HBufC* CPbkxRclResultInfoDlg::UpdateExistingContactL()
    {
    FUNC_LOG;
    CPbkxRclContactUpdater* updater = CPbkxRclContactUpdater::NewLC(
        iContactEngine );
    TPbkContactItemField& field = CurrentField();
      
    HBufC* resultText = updater->UpdateContactL( field );
    CleanupStack::PopAndDestroy( updater );
    return resultText;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SetupStatusPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SetupStatusPaneL()
    {
    FUNC_LOG;
    CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi() );
    CEikStatusPane* statusPane = appUi->StatusPane();
    
    iTitlePane = reinterpret_cast<CAknTitlePane*>(
        statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    
    const TDesC* titleText = iTitlePane->Text();
    iTitlePaneText = titleText->AllocL();
    
    SetTitlePaneTextL();
    
    //Set NullString to the the NaviPane Decorator
    //The simplest way of doin things. 
    //Add the navigation decorators only if the usual statuspane layout is used
    if ( statusPane->CurrentLayoutResId() == R_AVKON_STATUS_PANE_LAYOUT_USUAL_EXT )
        {
        iNaviPane = reinterpret_cast<CAknNavigationControlContainer*>(
            statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
        iNaviDecorator = iNaviPane->CreateNavigationLabelL( KNullDesC );
    
        iNaviPane->PushL( *iNaviDecorator );  
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::ResetStatusPane
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::ResetStatusPane()
    {
    FUNC_LOG;
    // title pane may be null if dialog constructing failed
    if ( iTitlePane != NULL )
        {
        // ownership of iTitlePaneText is transferred to title pane
        iTitlePane->SetText( iTitlePaneText );
        iTitlePaneText = NULL;
        }
    
    if ( iNaviPane != NULL && iNaviDecorator != NULL )
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }
    }



// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::SetTitlePaneTextL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::SetTitlePaneTextL()
    {
    FUNC_LOG;
   
    HBufC* firstName = PbkxRclUtils::FieldTextL( 
        iCurrentContact, 
        EPbkFieldIdFirstName );
    CleanupStack::PushL( firstName );
    
    HBufC* lastName = PbkxRclUtils::FieldTextL( 
        iCurrentContact, 
        EPbkFieldIdLastName );
    CleanupStack::PushL( lastName );
    
    RBuf name;
    CleanupClosePushL( name );
    name.CreateL( firstName->Length() + lastName->Length() + 1 );
    name.Format( KNameFormat, firstName, lastName );
       
    iTitlePane->SetTextL( name );
    
    CleanupStack::PopAndDestroy( &name );
    CleanupStack::PopAndDestroy( lastName );
    CleanupStack::PopAndDestroy( firstName );
   
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CreateListBoxEntriesL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::CreateListBoxEntriesL()
    {
    FUNC_LOG;

    iItems->Reset();
    iVisibleFields->Reset();

    CPbkFieldArray& fieldArray = iCurrentContact->CardFields();
    TInt count = fieldArray.Count();

    for ( TInt i = 0; i < count; i++ )
        {
    
        TPbkContactItemField field = fieldArray[i];

        HBufC* text = PbkxRclUtils::FieldTextL( field );
        CleanupStack::PushL( text );

        if ( text->Length() > 0 )
            {
            iVisibleFields->AppendL( field );
            const TDesC& header = field.FieldInfo().FieldName();
            
            RBuf entry;
            CleanupClosePushL( entry );
            
            TInt iconIndex = FindIconIndex(field.FieldInfo().IconId());            
            entry.CreateL( 
                header.Length() + text->Length() + 
                KExtraSpaceInListboxEntry );
            
            entry.Format( KFieldFormat, iconIndex, &header, text );
            iItems->AppendL( entry );
            CleanupStack::PopAndDestroy( &entry );
            }
        CleanupStack::PopAndDestroy( text );
        }
    
    CEikListBox* listBox = ListBox();

    if ( count > 0 )
        {
        // if there are items, set top item as selected
        listBox->SetTopItemIndex( 0 );
        listBox->SetCurrentItemIndex( 0 );
        }

    listBox->HandleItemAdditionL();
    
    // update contact action service
    iContactActionService.SetCurrentContactL( 
        iCurrentContact  );

    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FetchCurrentContactL
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FetchCurrentContactL()
    {
    FUNC_LOG;
    CContactCard* currentCard = iContactRetrieval->RetrieveDetailsL( 
        iActiveTabIndex,
        iWaitNoteText );
    
    if ( currentCard != NULL )
        {
        
        delete iCurrentContact;
        iCurrentContact = NULL;
        
        iCurrentContact = PbkxRclUtils::CreateContactItemL(
            currentCard,
            iContactEngine );

        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::UpdateDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclResultInfoDlg::UpdateDialogL()
    {
    FUNC_LOG;
    iContactRetrieval->SetSelectedContactL( iActiveTabIndex );
    CreateListBoxEntriesL();
    SetTitlePaneTextL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CallActionsAvailable
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::CallActionsAvailable() const
    {
    FUNC_LOG;
    return iContactActionService.IsActionEnabled( KFscAtComCallGSM ) ||
        iContactActionService.IsActionEnabled( KFscAtComCallVideo ) ||
        iContactActionService.IsActionEnabled( KFscAtComCallVoip );
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FocusOnPhoneNumberField
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FocusOnPhoneNumberField() const
    {
    FUNC_LOG;
    TPbkContactItemField& field = CurrentField();
    return field.FieldInfo().IsPhoneNumberField();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FocusOnEmailAddressField
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FocusOnEmailAddressField() const
    {
    FUNC_LOG;
    TPbkContactItemField& field = CurrentField();
    return field.FieldInfo().IsEmailField();
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::FocusOnVoipField
// ---------------------------------------------------------------------------
//
TBool CPbkxRclResultInfoDlg::FocusOnVoipField() const
    {
    FUNC_LOG;
    TPbkContactItemField& field = CurrentField();
    return field.FieldInfo().FieldId() == EPbkFieldIdVOIP;
    }

// ---------------------------------------------------------------------------
// CPbkxRclResultInfoDlg::CurrentField
// ---------------------------------------------------------------------------
//
TPbkContactItemField& CPbkxRclResultInfoDlg::CurrentField() const
    {
    FUNC_LOG;
    TInt curIndex = ListBox()->CurrentItemIndex();
    return ( *iVisibleFields )[curIndex];
    }
    

// ----------------------------------------------------------------------------
 // CPbkxRclResultInfoDlg::HandlePointerEventL
 // Function to handle all touch (pointer events)
 // ----------------------------------------------------------------------------
 void CPbkxRclResultInfoDlg::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    { 
     if ( AknLayoutUtils::PenEnabled() )
         {
         CEikListBox* listBox = ListBox();
         if ( listBox )
             {
             switch ( aPointerEvent.iType )
                 {
                 case TPointerEvent::EButton1Down:
                 case TPointerEvent::EButton1Up:   //falltrough from EButton1Down
                     iVisibleFocusOnList = EFalse; 
                     listBox->HandlePointerEventL( aPointerEvent ); 
                     break;
                     
                 default:
                     listBox->HandlePointerEventL( aPointerEvent );
                     break;
                 }
             }
         }     
    }

  // --------------------------------------------------------------------------
  // CPbkxRclResultInfoDlg::CopyDetailToClipboardL
  // --------------------------------------------------------------------------
  //    
  void CPbkxRclResultInfoDlg::CopyDetailToClipboardL( CVPbkContactManager& aContactManager )
      {
      // Construct a plain text from the detail
      const TInt KBeginning = 0;      
      TBuf<64> dateBuffer;
      CEikListBox* listBox = ListBox();     
      
      if ( listBox )
          {
          // Fetch field
          TPbkContactItemField field = CurrentField();              
          TPtrC detail;
          TStorageType fieldType = field.StorageType();
          
          switch(fieldType)
              {
              case KStorageTypeText:
                  {
                  detail.Set( field.Text() );
                  break;
                  }
              case KStorageTypeDateTime:
                  {
                  TLocale locale;                                    
                  TTime time( field.Time() );
    
                  HBufC* dateFormat = CCoeEnv::Static()->AllocReadResourceLC
                      (R_QTN_DATE_USUAL_WITH_ZERO); // in avkon.rsg
                  
                  time.FormatL(dateBuffer, *dateFormat, locale);
                  CleanupStack::PopAndDestroy(dateFormat);           
                  
                  detail.Set(dateBuffer);
                  break;
                  }
              }
              
          CPlainText* plainText = CPlainText::NewL();
          CleanupStack::PushL(plainText);
          plainText->InsertL(KBeginning, detail);
    
          // Copy to clipboard
          CClipboard* cb = CClipboard::NewForWritingLC(
                  aContactManager.FsSession());
          plainText->CopyToStoreL(cb->Store(), cb->StreamDictionary(),
              KBeginning, plainText->DocumentLength());
          cb->CommitL();
    
          CleanupStack::PopAndDestroy(2); // cb, plainText
    
          // Show a note
          HBufC* prompt = StringLoader::LoadLC(R_QTN_FS_INFO_NOTE_COPIED_TO_CLIPBOARD);
          CAknInformationNote* dlg = new(ELeave) CAknInformationNote(ETrue);
          dlg->ExecuteLD(*prompt);
          CleanupStack::PopAndDestroy(prompt);
              
          }
      }

  // ---------------------------------------------------------
  // CPbkxRclResultInfoDlg::FindIconIndex
  //
  // ---------------------------------------------------------
  //
  TInt CPbkxRclResultInfoDlg::FindIconIndex(TInt aId) const
      {
      TInt index(0);
      for (TInt n(0); n < KIconsCount; ++n)
          {
          if (KIconsId[n] == (TUint)aId)
              {
              index = n;
              break;
              }
          }
      return index;
      }
  
  
  // ---------------------------------------------------------
  // CPbkxRclResultInfoDlg::LoadIconArrayL
  //
  // ---------------------------------------------------------
  //
  void CPbkxRclResultInfoDlg::LoadIconArrayL()
      {      
      iSkinIcons = new (ELeave)RArray<TAknsItemID>(KIconsCount);
      
      iSkinIcons->Append(KAknsIIDQgnPropEmpty);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypPhone);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypHome);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypWork);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypMobile);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypVideo);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypFax);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypPager);
  #ifdef __VOIP
      iSkinIcons->Append(KAknsIIDQgnPropNrtypVoip);  
  #endif
      iSkinIcons->Append(KAknsIIDQgnPropNrtypEmail);
      iSkinIcons->Append(KAknsIIDQgnPropNrtypAddress);
              
      CAknIconArray* iconArray = new (ELeave)CAknIconArray(KIconsCount);     
      CleanupStack::PushL( iconArray );
      CFbsBitmap* bitmap,*mask;
      bitmap = NULL;
      mask= NULL;
      MAknsSkinInstance* skinInstance = AknsUtils::SkinInstance();
      // Add icons
      for (TInt i(0); i < KIconsCount; ++i)
          {
          AknsUtils::CreateIconL(skinInstance, (*iSkinIcons)[i],bitmap,mask,AknIconUtils::AvkonIconFileName(),KIcons[i],  KIconsMask[i] );
          iconArray->AppendL(CGulIcon::NewL(bitmap,mask));
          }

      CEikFormattedCellListBox* listBox = 
                      static_cast<CEikFormattedCellListBox*>( ListBox() );
      CPbkxRclResultInfoItemDrawer* fItemDrawer = static_cast<CPbkxRclResultInfoItemDrawer*>(
          listBox->ItemDrawer() );
      
      fItemDrawer->FormattedCellData()->SetIconArrayL(iconArray);
      
      CleanupStack::Pop( iconArray );       
      }

// End of File
