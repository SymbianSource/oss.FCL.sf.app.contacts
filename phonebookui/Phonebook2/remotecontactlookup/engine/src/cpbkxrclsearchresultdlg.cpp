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
* Description:  Definition of the class CPbkxRclSearchResultDlg.
*
*/


#include "emailtrace.h"
#include <pbk2rclengine.rsg>
#include <eikmover.h>
#include <coemain.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <aknnavide.h>
#include <aknappui.h>
#include <AknIconUtils.h>
#include <cntitem.h>
#include <badesca.h>
#include <StringLoader.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <gulicon.h>
#include <avkon.mbg>
//#include <pbkxrclengine.mbg> TODO
#include <aknnotewrappers.h>
#include "cfsccontactactionmenu.h"  
#include "mfsccontactactionmenumodel.h"
#include "mfsccontactaction.h"
#include "cfsccontactactionmenuitem.h"
#include "fsccontactactionservicedefines.h"
#include "fsccontactactionmenudefines.h"

#include <data_caging_path_literals.hrh>
#include <AknsUtils.h>
#include <eikclb.h>
#include <eikclbd.h>

#include <csxhelp/cmail.hlp.hrh>

#include "cpbkxrclsearchresultdlg.h"
#include "pbkxremotecontactlookuppanic.h"
#include "engine.hrh"
#include "pbkxrclutils.h"
#include "cpbkxrclvcardsender.h"
#include "cpbkxrclactionservicewrapper.h"

#include <e32base.h>

// list box field formats
_LIT( KFieldFormat, "\t%S %S" );
_LIT( KFieldFormatWithIcon, "\t%S %S\t%d" );

_LIT( KFieldFormatDispName, "\t%S" );
_LIT( KFieldFormatDispNameWithIcon, "\t%S\t%d" );

// Constant to measure when user is scrolling.
const TInt KKeyScrolling = 1;
const TInt KPbkxRemoteContactLookupServiceImplImpUid = 0x2001FE0D;


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::NewL
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultDlg* CPbkxRclSearchResultDlg::NewL(
    TInt& aIndex,
    CDesCArray* aArray,
    MEikCommandObserver* aCommand,
    RPointerArray<CContactCard>& aContactItems,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled,
    TBool aMoreThanMaxResults)
    {
    FUNC_LOG;
    CPbkxRclSearchResultDlg* dialog = new ( ELeave ) CPbkxRclSearchResultDlg(
        aIndex, 
        aArray,
        aCommand,
        aContactItems,
        aContactEngine,
        aActionService,
        aContactSelectorEnabled,
        aMoreThanMaxResults);

    CleanupStack::PushL( dialog );
    dialog->ConstructL();
    CleanupStack::Pop( dialog );
    return dialog;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CPbkxRclSearchResultDlg
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultDlg::CPbkxRclSearchResultDlg(
    TInt& aIndex,
    CDesCArray* aArray,
    MEikCommandObserver* aCommand,
    RPointerArray<CContactCard>& aContactItems,
    CPbkContactEngine& aContactEngine,
    CPbkxRclActionServiceWrapper& aActionService,
    TBool aContactSelectorEnabled,
    TBool aMoreThanMaxResults) :
    CPbkxRclBaseDlg( aIndex, aArray, aCommand, ETrue ), 
    iContactEngine( aContactEngine ),
    iContactActionService( aActionService ),
    iItems( aArray ), iContactItems( aContactItems ), 
    iContactSelectorEnabled( aContactSelectorEnabled ), 
    iSelectedItemIndex( aIndex ), 
    iIsInfoDlgVisible( EFalse ),
    iMoreThanMaxResults(aMoreThanMaxResults)
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ConstructL()
    {
    FUNC_LOG;
    CPbkxRclBaseDlg::ConstructL( R_RCL_SEARCH_RESULT_DIALOG_MENU_BAR );
    LoadNaviTextL();

    // add foreground observer
    CCoeEnv::Static()->AddForegroundObserverL( *this );
    
    // Idle object for scheduling showing of the 
    // "more than 50 results..." note
    iIdleNote = CIdle::NewL(CActive::EPriorityIdle);
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::~CPbkxRclSearchResultDlg
// ---------------------------------------------------------------------------
//
CPbkxRclSearchResultDlg::~CPbkxRclSearchResultDlg()
    {
    FUNC_LOG;
    CCoeEnv::Static()->RemoveForegroundObserver( *this );
    ResetStatusPane();
    
    if ( iTitlePane != NULL )
          {
          // set title pane back to what it was, ownership of 
          // iTitlePaneText is transferred to title pane
          iTitlePane->SetText( iOriginalTitleText );
          }
    
    iNaviText.Close();
    delete iAddRecipientIcon;

    iContactActionService.CancelQuery();
    if ( iActionMenu )
    	{
     	delete iActionMenu;
    	iActionMenu = NULL;
    	}    
    
    delete iIdleNote;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetMoreThanMaxResults
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetMoreThanMaxResults(TBool aMoreThanMax)
    {
    iMoreThanMaxResults = aMoreThanMax;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::MoreThanMaxResults
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::MoreThanMaxResults()
    {
    return iMoreThanMaxResults;
    }
   
// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::DisplayTooManyResultsInfoNoteL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::DisplayTooManyResultsInfoNoteL()
    {
    FUNC_LOG;
    // For making sure the note is not shown again set the flag off.  
    SetMoreThanMaxResults(EFalse); 
       
    HBufC* text = StringLoader::LoadLC( 
            R_QTN_RCL_TOO_MANY_RESULTS_NOTE, 
            KMaxMatches );               
    
    // Note is non blocking and is displayed above the search results until
    // the note expires and closes itself.
    CAknInformationNote* informationNote = new (ELeave) CAknInformationNote; 
    informationNote->SetTimeout(CAknNoteDialog::ELongTimeout);
    informationNote->ExecuteLD(*text);
           
    CleanupStack::PopAndDestroy(text);         
    }  

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ExecuteLD
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ExecuteLD()
    {
    FUNC_LOG;

    PrepareLC( R_RCL_SEARCH_RESULT_DIALOG );

    UpdateGraphics();

    HBufC* noItemText = StringLoader::LoadLC( R_QTN_RCL_NO_RESULTS );
    ListBox()->View()->SetListEmptyTextL( *noItemText );
    CleanupStack::PopAndDestroy( noItemText );

    InitializeActionMenuL();

    SetupStatusPaneL();
    CreateListBoxEntriesL();
    
    if ( iContactItems.Count() == 0 )
      {
      ConstructMenuBarL( R_RCL_OPTIONS_BACK_CONTEXTMENU );
      }

    if ( MoreThanMaxResults() )
        { 
        // If there was more than max number of results found 
        // show a "more than 50 results..." info note to user.  
        iIdleNote->Cancel();  
        // For smooth display of the note, request
        // an idle callback for launching the note later.
        iIdleNote->Start( TCallBack(
                ( &CPbkxRclSearchResultDlg::TooManyResultsIdleCallbackL ),this ) );
        }  
      
    CAknSelectionListDialog::RunLD();
    }

// --------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::TooManyResultsIdleCallbackL
// --------------------------------------------------------------------------
//
TInt CPbkxRclSearchResultDlg::TooManyResultsIdleCallbackL( TAny* aSelf )
    {
    CPbkxRclSearchResultDlg* self = static_cast<CPbkxRclSearchResultDlg*>( aSelf );
    self->DisplayTooManyResultsInfoNoteL();
    return 0;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CurrentItemIndex
// ---------------------------------------------------------------------------
//
TInt CPbkxRclSearchResultDlg::CurrentItemIndex() const
    {
    FUNC_LOG;
    return ListBox()->CurrentItemIndex();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetCurrentItemIndex
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetCurrentItemIndexL( TInt aIndex )
    {
    FUNC_LOG;
    if ( aIndex >= 0 && aIndex < iContactItems.Count() )
        {
        // update list box entrys first
        TInt oldIndex = iSelectedItemIndex;
        
        ListBox()->SetCurrentItemIndex( aIndex );
        iSelectedItemIndex = aIndex;
        
        UpdateListBoxEntryL( iSelectedItemIndex );
        UpdateListBoxEntryL( oldIndex );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::Close
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::Close()
    {
    FUNC_LOG;
    // this will not leave, because TryExitL doesn't call leaving methods
    // when EAknSoftkeyCancel is used
    iExit = ETrue;
    TRAP_IGNORE( TryExitL( EAknSoftkeyCancel ) ); 
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::UpdateDialogL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::UpdateDialogL()
    {
    FUNC_LOG;
    LoadNaviTextL();
    CreateListBoxEntriesL();
    UpdateStatusPaneL();
    
    if ( iContactItems.Count() == 0 )
        {
        ConstructMenuBarL( R_RCL_OPTIONS_BACK_CONTEXTMENU );
        }
    else
        {
        ConstructMenuBarL( R_RCL_OPTIONS_BACK_OPEN );
        }
   
    if ( MoreThanMaxResults() )
        { 
        // If there was more than max number of results found 
        // show a "more than 50 results..." info note to user.  
        iIdleNote->Cancel();  
        // For smooth display of the note, request
        // an idle callback for launching the note later.
        iIdleNote->Start( TCallBack(
                ( &CPbkxRclSearchResultDlg::TooManyResultsIdleCallbackL ),this ) );
        }  
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SendBusinessCardL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SendBusinessCardL()
    {
    FUNC_LOG;
    CPbkxRclvCardSender* sender = CPbkxRclvCardSender::NewLC( iContactEngine );

    TInt curIndex = CurrentItemIndex();


    CContactCard* current = iContactItems[curIndex];
   
    CPbkContactItem* contactItem = PbkxRclUtils::CreateContactItemL(
        current,
        iContactEngine );
    CleanupStack::PushL( contactItem );
    
    sender->SendvCardL( *contactItem, *(iContactActionService.ContactConverter()) );

    CleanupStack::PopAndDestroy( contactItem );
    CleanupStack::PopAndDestroy( sender );
    
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    FUNC_LOG;
    if ( aResourceId == R_RCL_SEARCH_RESULT_DIALOG_MENU_PANE )
        {

        if ( iContactItems.Count() == 0 )
            {
            // when there are 0 results, only new search option is available
            aMenuPane->SetItemDimmed( ERclCmdViewDetails, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdAddAsRecipient, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdSaveToContacts, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdCall, ETrue );
            aMenuPane->SetItemDimmed( ERclCmdSend, ETrue );
            }
        else
            {
            if ( !iContactSelectorEnabled )
                {
                
                aMenuPane->SetItemDimmed( ERclCmdAddAsRecipient, ETrue );
                
                aMenuPane->SetItemDimmed(
                    ERclCmdCall,
                    !CallActionsAvailable() );
                
                // send menu is always visible, since business card can 
                // always be sent
                }
            else
                {
                aMenuPane->SetItemDimmed( ERclCmdCall, ETrue );
                aMenuPane->SetItemDimmed( ERclCmdSend, ETrue );
                }
            
            aMenuPane->SetItemDimmed(
                ERclCmdSaveToContacts,
                !iContactActionService.IsActionEnabled( KFscAtManSaveAs ) );
            }
        }
    else if ( aResourceId == R_RCL_CALL_MENU_PANE )
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
    else if ( aResourceId == R_RCL_SEND_MENU_PANE )
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
        
        aMenuPane->SetItemDimmed( 
            ERclCmdSendBusinessCard, 
            !iContactActionService.IsActionEnabled( KFscAtSendBusinessCard ) );
        
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::GetHelpContext
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    FUNC_LOG;

    if ( !iIsInfoDlgVisible )
        {
        aContext.iMajor = TUid::Uid( KPbkxRemoteContactLookupServiceImplImpUid );
        aContext.iContext = KRLOOK_HLP_RESULT_VIEW;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::HandleGainingForeground
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::HandleGainingForeground()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::HandleLosingForeground
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::HandleLosingForeground()
    {
    FUNC_LOG;
    if ( ScrollingInProgress() )
        {
        // scrolling was in progress
        TRAP_IGNORE( ScrollingStoppedL() );
        iKeyDown = EFalse;
        iKeyCounter = 0;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CPbkxRclSearchResultDlg::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    FUNC_LOG;

    // if there are no items, don't handle key presses
    TInt count = iContactItems.Count();

    if ( iExit || aType != EEventKey )
        {
        if ( aType == EEventKeyUp )
            {
            // added count condition
            if ( count && ScrollingInProgress() && !iIsScrollLaunched )
                {
                // scrolling ended
                iIsScrollLaunched = ETrue;
                ScrollingStoppedL();
                }
            iKeyCounter = 0;
            iKeyDown = EFalse;
            }
        
        return EKeyWasConsumed;
        }
    
    if ( aKeyEvent.iCode == EKeyUpArrow || 
         aKeyEvent.iCode == EKeyDownArrow )
        {
        iKeyDown = ETrue;
        iKeyCounter++;
        }
    else
        {
        // make sure that if we were scrolling we stop it now
        if ( ScrollingInProgress() )
            {
            ScrollingStoppedL();
            }
        iKeyDown = EFalse;
        iKeyCounter = 0;
        }
    
    // by default the key is consumed
    TKeyResponse keyResponse = EKeyWasConsumed;
    
    switch ( aKeyEvent.iCode )
        {
        case EKeyPhoneSend:
            {
            if ( iMenuBar->ItemSpecificCommandsEnabled() )
                {      
                ProcessCommandL( ERclCmdVoiceCall );             
                }
            break;
            }
            
        case EKeyEnter: 
        case EKeyOK:
            {     
            // Item specific enter and msk now handled in the listbox
            // (calls eventually HandleListBoxEventL if list has focus)
            keyResponse = ListBox()->OfferKeyEventL(aKeyEvent, aType);          
            }
            break;
            
        case EKeyUpArrow: // fall through, same functionality with these two
        case EKeyDownArrow:
            {
            if ( count > 0 )
                {
                TInt oldIndex = iSelectedItemIndex;
                CAknSelectionListDialog::OfferKeyEventL( aKeyEvent, aType );
                iSelectedItemIndex = CurrentItemIndex();
                if ( oldIndex != iSelectedItemIndex )
                    {
                    UpdateListBoxEntryL( oldIndex );
                    // update listbox entry if we are not yet fast scrolling
                    if ( !ScrollingInProgress() )
                        {
                        SetCurrentContactToActionServiceL();
                        UpdateListBoxEntryL( iSelectedItemIndex );
                        }
                    }
                }
            }
            break;
        case EKeyRightArrow:
            {
            if ( !AknLayoutUtils::LayoutMirrored() && ActionMenuAvailable() )
                {
                // opens action menu
                //ShowActionMenuL();  //no contact action menu
                }
            }
            break;
        case EKeyLeftArrow:
            {
            if ( AknLayoutUtils::LayoutMirrored() && ActionMenuAvailable() )
                {
                //ShowActionMenuL();  //no contact action menu
                }
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
    
    return keyResponse;  // EKeyWasConsumed, EKeyWasNotConsumed
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::OkToExitL
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::OkToExitL( TInt aButtonId )
	{
    FUNC_LOG;
	if( aButtonId == EAknSoftkeyOpen )
		{
		ProcessCommandL( ERclCmdViewDetails );
		return EFalse;
		}
	else if ( aButtonId == EAknSoftkeyContextOptions )
	    {
	    iMenuBar->TryDisplayMenuBarL();
        return EFalse;
	    }
	else
		{
		return CAknDialog::OkToExitL( aButtonId );
		}
	}

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::HandleListBoxEventL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {
    if ( AknLayoutUtils::PenEnabled() && !iActionMenuStarted )
        {
        switch(aEventType)
            {
            case EEventEmptyListClicked:
                {
                CAknSelectionListDialog::ProcessCommandL( ERclCmdNewSearchPrefilled );
                }
                break;
               
            case EEventEnterKeyPressed:
            case EEventItemSingleClicked:
                {  
                CAknSelectionListDialog::ProcessCommandL(ERclCmdViewDetails);
                }
                break;
                
            default:
                break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetupStatusPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetupStatusPaneL()
    {
    FUNC_LOG;
    CAknAppUi* appUi = static_cast<CAknAppUi*>( CCoeEnv::Static()->AppUi() );
    CEikStatusPane* statusPane = appUi->StatusPane();
    iNaviPane = reinterpret_cast<CAknNavigationControlContainer*>(
        statusPane->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( iNaviText );
    
    iNaviPane->PushL( *iNaviDecorator );
    
    // setup title pane
    iTitlePane = reinterpret_cast<CAknTitlePane*>(
           statusPane->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    
    // store original title pane text 
    const TDesC* titleText = iTitlePane->Text();
    iOriginalTitleText = titleText->AllocL(); 
 
    // set  own title pane text for this view
    SetTitlePaneTextL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetTitlePaneTextL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetTitlePaneTextL()
    {
    // Update title pane text   
    HBufC* titleText  = StringLoader::LoadLC( R_QTN_RCL_TITLE_SERVER_CONTACTS ); 
    iTitlePane->SetTextL(titleText->Des());   
    CleanupStack::PopAndDestroy(titleText);
    }


// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::UpdateStatusPaneL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::UpdateStatusPaneL()
    {
    FUNC_LOG;
    
    ResetStatusPane();
    iNaviDecorator = iNaviPane->CreateNavigationLabelL( iNaviText );
    iNaviPane->PushL( *iNaviDecorator );
   
    SetTitlePaneTextL();  
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ResetStatusPane
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ResetStatusPane()
    {
    FUNC_LOG;
    // navi pane and navi decorator may be null if dialog construction
    // failed
    if ( iNaviPane != NULL && iNaviDecorator != NULL )
        {
        iNaviPane->Pop( iNaviDecorator );
        delete iNaviDecorator;
        iNaviDecorator = NULL;
        }       
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CreateListBoxEntriesL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::CreateListBoxEntriesL()
    {
    FUNC_LOG;
    iItems->Delete( 0, iItems->Count() );
    TInt count = iContactItems.Count();
    
    if ( count > 0 )
        {
        iSelectedItemIndex = 0;
        }
    
    SetCurrentContactToActionServiceL();

    for ( TInt i = 0; i < count; i++ )
        {
        CreateListBoxEntryL( i );
        }
    
    CEikListBox* listBox = ListBox();

    if ( count > 0 )
        {
        listBox->SetTopItemIndex( 0 );
        listBox->SetCurrentItemIndex( 0 );
        }

    listBox->HandleItemAdditionL();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CreateListBoxEntryL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::CreateListBoxEntryL( TInt aIndex )
    {
    FUNC_LOG;
     
    CContactCard* card = iContactItems[aIndex];
    TPtrC dispName = PbkxRclUtils::FieldText( card, KUidContactFieldDefinedText );
    TPtrC firstName = PbkxRclUtils::FieldText( card, KUidContactFieldGivenName );
    TPtrC lastName = PbkxRclUtils::FieldText( card, KUidContactFieldFamilyName );
    
    RBuf entry;
    TPtrC entryFirstName;

    if ( dispName.Length() != 0 )
        {
        entryFirstName.Set(dispName);
        entry.CreateL( entryFirstName.Length() + KExtraSpaceInListboxEntry );
        }
    else
        {
        entryFirstName.Set(firstName);
        entry.CreateL( firstName.Length() + lastName.Length() + KExtraSpaceInListboxEntry );
        }
    
    CleanupClosePushL( entry );
    if ( dispName.Length() != 0 )
        {
        if ( aIndex == iSelectedItemIndex && ActionMenuAvailable() )
            {
            entry.Format( 
                KFieldFormatDispNameWithIcon, 
                &entryFirstName, 
                EActionMenuEnabled );
            }
        else
            {
            entry.Format( KFieldFormatDispName, &entryFirstName );
            }
        }
    else
        {
        if ( aIndex == iSelectedItemIndex && ActionMenuAvailable() )
            {
            entry.Format( 
                KFieldFormatWithIcon, 
                &firstName, 
                &lastName, 
                EActionMenuEnabled );
            }
        else
            {
            entry.Format( KFieldFormat, &firstName, &lastName );
            }
        }
    iItems->InsertL( aIndex, entry );
    CleanupStack::PopAndDestroy( &entry );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::UpdateListBoxEntryL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::UpdateListBoxEntryL( TInt aIndex )
    {
    FUNC_LOG;
    iItems->Delete( aIndex );
    CreateListBoxEntryL( aIndex );
    ListBox()->DrawItem( aIndex );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::SetCurrentContactToActionServiceL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::SetCurrentContactToActionServiceL()
    {
    FUNC_LOG;
   
    if ( iActionMenu )
    	{
        if ( iContactItems.Count() > 0 )
            {
            // set current contact to action service
        
            CPbkContactItem* currentContact = PbkxRclUtils::CreateContactItemL(
                iContactItems[iSelectedItemIndex],
                iContactEngine );
            CleanupStack::PushL( currentContact );
    
            iContactActionService.SetCurrentContactL( 
                currentContact );
        
            CleanupStack::PopAndDestroy( currentContact );
        
            }
        else
            {
            iContactActionService.SetCurrentContactL( NULL );
            }
    
    	if ( iActionMenu )
			{
            // update action menu
            iActionMenu->Model().AddPreQueriedCasItemsL();
            // save as contact not added for this contact
            iSaveAsAdded = EFalse;
            }
    	} 
    iIsScrollLaunched = EFalse;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::LoadNaviTextL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::LoadNaviTextL()
    {
    FUNC_LOG;
    iNaviText.Close();
    HBufC* naviText = NULL;
    if ( iContactItems.Count() == 1 )
        {
        naviText = StringLoader::LoadLC( R_QTN_RCL_ONE_RESULT_NAVI );
        }
    else if ( iContactItems.Count() == 0 )
        {
        naviText = NULL;
        }
    else
        {
        naviText = StringLoader::LoadLC( 
            R_QTN_RCL_RESULT_NAVI,
            iContactItems.Count() );
        }
    
    if (naviText)
        {
        iNaviText.CreateL( *naviText );
        CleanupStack::PopAndDestroy( naviText );
        }
    else
        {
        iNaviText = KNullDesC;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CallActionsAvailable
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::CallActionsAvailable() const
    {
    FUNC_LOG;
    return iContactActionService.IsActionEnabled( KFscAtComCallGSM ) ||
        iContactActionService.IsActionEnabled( KFscAtComCallVideo );
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::InitializeActionMenuL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::InitializeActionMenuL()
    {
    FUNC_LOG;
    
    iActionMenu = CFscContactActionMenu::NewL( 
        *(iContactActionService.ActionService()) );

    MFscContactActionMenuModel& model = iActionMenu->Model();

    if ( iContactSelectorEnabled && iContactItems.Count() > 0 )
        {

        // add static custom item
        HBufC* menuText = StringLoader::LoadLC( 
            R_QTN_RCL_ADD_RECIPIENT_ACTION_MENU_TEXT );
        
        CFscContactActionMenuItem* item = model.NewMenuItemL(
            *menuText,
            iAddRecipientIcon,
            KPbkxRclAddRecipientPriority,
            ETrue,
            KPbkxRclAddRecipientActionUid );
        
        CleanupStack::PushL( item );
        
        model.AddItemL( item );
        
        CleanupStack::Pop( item );
        CleanupStack::PopAndDestroy( menuText );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ShowActionMenuL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ShowActionMenuL( const TPoint& /*aPenEventScreenLocation*/ )
    {
    FUNC_LOG;

    CreateAndAddSaveAsContactActionL();
    
    TFscContactActionMenuPosition pos = GetActionMenuPosition();
             
    TFscActionMenuResult result = iActionMenu->ExecuteL( pos, 0, this );

    iActionMenuStarted = EFalse;
    // if custom item was selected, it was our add to recipient or
    // save as contact
    if ( result == EFscCustomItemSelected )
        {
        const TFscContactActionQueryResult* saveAsContact = 
            iContactActionService.GetResult( KFscAtManSaveAs );
        TInt itemIndex = iActionMenu->FocusedItemIndex();
        CFscContactActionMenuItem& item = 
            iActionMenu->Model().ItemL( itemIndex );
        
        if ( item.ImplementationUid() == KPbkxRclAddRecipientActionUid )
            {
            ProcessCommandL( ERclCmdAddAsRecipient );
            }
        else if ( saveAsContact != NULL &&
                  item.ImplementationUid() == saveAsContact->iAction->Uid() ) 
            {
            ProcessCommandL( ERclCmdSaveToContacts );
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::GetActionMenuPosition
// ---------------------------------------------------------------------------
//
TFscContactActionMenuPosition CPbkxRclSearchResultDlg::GetActionMenuPosition()
    {
    FUNC_LOG;
    const TInt KNumberOfMenuPositions = 3;

    CEikListBox* listBox = ListBox();
    TInt itemsInView = listBox->View()->NumberOfItemsThatFitInRect( Rect() );

    TInt topIndex = listBox->TopItemIndex();
    TInt curIndex = listBox->CurrentItemIndex();

    TInt normalizedIndex = curIndex - topIndex;
    
    TInt segmentSize = itemsInView / KNumberOfMenuPositions;

    if ( normalizedIndex < segmentSize )
        {
        return EFscTop;
        }
    else if ( normalizedIndex < ( 2 * segmentSize) )
        {
        return EFscCenter;
        }
    else
        {
        return EFscBottom;
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ActionMenuAvailable
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::ActionMenuAvailable() const
    {
    FUNC_LOG;
    TInt count = iActionMenu->Model().ItemCount() > 0;
    if ( !iSaveAsAdded )
        {
        if ( iContactActionService.IsActionEnabled( KFscAtManSaveAs ) )
            {
            count++;
            }
        }
    
    return count > 0;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::CreateAndAddSaveAsContactActionL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::CreateAndAddSaveAsContactActionL()
    {
    FUNC_LOG;
    if ( !iSaveAsAdded )
        {
        const TFscContactActionQueryResult* saveAsContact = 
            iContactActionService.GetResult( KFscAtManSaveAs );
        if ( saveAsContact != NULL )
            {
            MFscContactActionMenuModel& model = iActionMenu->Model();
            
            const MFscContactAction* action = saveAsContact->iAction;
            
            CFscContactActionMenuItem* item = model.NewMenuItemL(
                action->ActionMenuText(),
                action->Icon(),
                saveAsContact->iPriority,
                EFalse,
                action->Uid() );
            
            CleanupStack::PushL( item );
            
            model.AddItemL( item );
            
            iSaveAsAdded = ETrue;

            CleanupStack::Pop( item );
            }
        }
    }


// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ScrollingInProgress
// ---------------------------------------------------------------------------
//
TBool CPbkxRclSearchResultDlg::ScrollingInProgress() const
    {
    FUNC_LOG;
    return iKeyDown && iKeyCounter > KKeyScrolling;
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ScrollingStopped
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ScrollingStoppedL()
    {
    FUNC_LOG;
    SetCurrentContactToActionServiceL();
    UpdateListBoxEntryL( iSelectedItemIndex ); 
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::ConstructContextMenuL
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::ConstructMenuBarL( TInt aResourceId )
    {
    FUNC_LOG;
    CEikButtonGroupContainer& butContainer = ButtonGroupContainer();
    butContainer.SetCommandSetL( aResourceId );
    butContainer.DrawNow();
    }

// ---------------------------------------------------------------------------
// CPbkxRclSearchResultDlg::InfoDlgVisible
// ---------------------------------------------------------------------------
//
void CPbkxRclSearchResultDlg::InfoDlgVisible( TBool aVisible )
    {
    FUNC_LOG;
    iIsInfoDlgVisible = aVisible;
    }


 
 // ----------------------------------------------------------------------------
 // CPbkxRclSearchResultDlg::HandlePointerEventL
 // Function to handle all touch (pointer events)
 // ----------------------------------------------------------------------------
 void CPbkxRclSearchResultDlg::HandlePointerEventL(const TPointerEvent& aPointerEvent)
     {       
     if ( AknLayoutUtils::PenEnabled() )
         {
         CEikListBox* listBox = ListBox();
         if ( listBox )
             {
             switch ( aPointerEvent.iType )
                 {
                 case TPointerEvent::EButton1Down:
                     {
                     TInt focusIndex;
                     TBool focusableContactPointed =
                         listBox->View()->XYPosToItemIndex(
                              aPointerEvent.iPosition, focusIndex );
                     
                     if ( focusableContactPointed && focusIndex > 0)
                         {
                         TInt oldIndex = iSelectedItemIndex;                
                         iSelectedItemIndex = focusIndex;
                         if ( oldIndex != iSelectedItemIndex )
                             {                    
                             // update listbox entry if we are not yet fast scrolling
                             if ( !ScrollingInProgress() )
                                  {
                                  SetCurrentContactToActionServiceL();                 
                                  }
                             }                         
                         }
                     listBox->HandlePointerEventL( aPointerEvent );
                     break;
                     }
                 case TPointerEvent::EButton1Up:
                     {
                     listBox->HandlePointerEventL( aPointerEvent );
                     break;
                     }
                 }
             }
         }    
     }
 
 TPoint CPbkxRclSearchResultDlg::ActionMenuPosition()
     {
     return iActionMenuPosition;
     }
