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
* Description:  Phonebook 2 Names List UI extension plug-in.
*
*/


// INCLUDE FILES
#include "Pbk2NamesListExtensionPlugin.h"
#include "Pbk2NamesListEx.hrh"
#include "Pbk2NamesListExView.h"

#include "CPbk2NameslistUiControlExtension.h"
#include "CPbk2NlxOpenCcaCmd.h"
#include "CPbk2NlxAddToTopContactsCmd.h"
#include "CPbk2NlxRemoveFromTopContactsCmd.h"
#include "CPbk2NlxMoveTopContactsCmd.h"
#include "Pbk2NlxMenuFiltering.h"
#include "cpbk2deletemycardcmd.h"
#include "cpbk2mycard.h"

#include "cpbk2openmycardcmd.h"
#include "CVPbkContactManager.h"
#include <spbcontentprovider.h>

// Phonebook 2
#include <MPbk2AppUi.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2ApplicationServices2.h>
#include <CPbk2StoreConfiguration.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CPbk2ApplicationServices.h>

#include <MPbk2ContactUiControl.h>
#include <MPbk2ViewExplorer.h>
#include <CPbk2ViewState.h>

#include <MPbk2ContactUiControlExtension.h>
#include <Pbk2Commands.rsg>
#include <MVPbkBaseContact.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldData.h>
#include <VPbkEng.rsg>
#include <CVPbkTopContactManager.h>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactStoreUriArray.h>
#include <MPbk2ContactUiControl2.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStoreInfo.h>
#include <MVPbkContactStoreProperties.h>

// Debugging headers

#include <Pbk2Debug.h>
#include <Pbk2Profile.h>

//CCA
#include <mccaconnection.h>
#include <mccaparameter.h>
#include <ccafactory.h>

// System includes
#include <coemain.h>
#include <eikmenub.h>
#include <AiwCommon.hrh>
#include <avkon.hrh>
#include <avkon.rsg>

/// Unnamed namespace for local definitions
namespace {
const TInt KMinNumOfContactsToMerge = 2; // minimum number of contacts to merge
} /// namespace

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CNamesListUIExtensionPlugin
// --------------------------------------------------------------------------
//
CNamesListUIExtensionPlugin::CNamesListUIExtensionPlugin()
    {
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::~CNamesListUIExtensionPlugin
// --------------------------------------------------------------------------
//
CNamesListUIExtensionPlugin::~CNamesListUIExtensionPlugin()
    {
    if ( iCCAConnection )
        {
        iCCAConnection->Close();
        }
    delete iContentProvider;
    if ( iLocalStore )
        {
        iLocalStore->Close( *this );
        }
    Release( iAppServices );
    }

    

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::NewL
// --------------------------------------------------------------------------
//
CNamesListUIExtensionPlugin* CNamesListUIExtensionPlugin::NewL()
    {
    CNamesListUIExtensionPlugin* self = 
        new ( ELeave ) CNamesListUIExtensionPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::ConstructL
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::ConstructL()
    {
    iLocalStoreContactsCount = 0;	
    iAppServices = CPbk2ApplicationServices::InstanceL();
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::ContentProviderL
// --------------------------------------------------------------------------
//
inline CSpbContentProvider& CNamesListUIExtensionPlugin::ContentProviderL()
	{
    if( !iContentProvider )
        {
        TAny* ext = iAppServices->MPbk2ApplicationServicesExtension( 
            KMPbk2ApplicationServicesExtension2Uid );
        if( !ext )
            {
            User::Leave( KErrNotReady );
            }
        CPbk2StoreManager& storeManager = 
            static_cast<MPbk2ApplicationServices2*>(ext)->StoreManager();
        iContentProvider = CSpbContentProvider::NewL( iAppServices->ContactManager(), storeManager,  
            CSpbContentProvider::EStatusMessage | CSpbContentProvider::EPhoneNumber );
        }
	return *iContentProvider;
	}


// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CreateExtensionViewL
// --------------------------------------------------------------------------
//
MPbk2UIExtensionView* CNamesListUIExtensionPlugin::CreateExtensionViewL
        ( TUid aViewId, CPbk2UIExtensionView& aView )
    {
    MPbk2UIExtensionView* result = NULL;
    if ( aViewId == TUid::Uid( EPbk2NamesListViewId ) )
        {
        iNamesListExViewRef = CPbk2NamesListExView::NewL( 
            aView, ContentProviderL(), iCCAConnection );
        result = iNamesListExViewRef;    //ownership not taken
        }

    return result;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane, MPbk2ContactUiControl& aControl )
    {
    //This is called before CPbk2CommandHandler::PerformStandardMenuFilteringL
    //so do not remove anything here that may be removed there.
    
    switch ( aResourceId )
        {
        case R_PHONEBOOK2_OPEN_CCA:
            {
            if ( !Pbk2NlxMenuFiltering::OpenReady( aControl ) )
                {
                DimItem( aMenuPane, EPbk2CmdOpenCca );                
                }
            break;
            }
        case R_PHONEBOOK2_SELECT_ADDFAVORITES:
            {
            if ( !Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( aControl ) ||
                aControl.ContactsMarked() ) 
                {
                DimItem( aMenuPane, EPbk2CmdAddFavourites );
                }
            break;
            }
        case R_PHONEBOOK2_SELECT_MYCARD:
            {
            if ( !Pbk2NlxMenuFiltering::MyCardCmdSelected( aControl ) ||
                aControl.ContactsMarked() ) 
                {
                DimItem( aMenuPane, EPbk2CmdOpenMyCard );
                DimItem( aMenuPane, EPbk2CmdSelectMyCard );
                }
            else
                {
                if( MyCardLink() )
                    {
                    // MyCard exists -> show "open"
                    DimItem( aMenuPane, EPbk2CmdSelectMyCard );
                    }
                else
                    {
                    // MyCard not created -> show "select"
                    DimItem( aMenuPane, EPbk2CmdOpenMyCard );
                    }
                }
            break;
            }
        case R_PHONEBOOK2_RCL_MENU:
            {
            if ( Pbk2NlxMenuFiltering::RclCmdSelectOptSelected( aControl )   ||
                 Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( aControl ) ||                    
                 aControl.ContactsMarked() ||
                 !(iNamesListExViewRef && iNamesListExViewRef->IsRclOnL())) 
                {
                DimItem( aMenuPane, EPbk2CmdRcl ); 
                }
            break;
            }
        case R_PHONEBOOK2_SELECT_RCL:
            {
			//Promotion item (RclCmdSelectOptSelected) may only be visible if rcl feature is on
            if ( !Pbk2NlxMenuFiltering::RclCmdSelectOptSelected( aControl )   || 
                aControl.ContactsMarked() ) 
                {
                DimItem( aMenuPane, EPbk2CmdSelectRclCmdItem );
                }
            break;
            }
        case R_PHONEBOOK2_ADD_TO_TOPCONTACTS:
            {
            TBool dimAdd = EFalse;    
            if ( aControl.ContactsMarked() )
                {
// multiple top assign/reassign doesn't work yet, enable when ready                
#if 0                
                dimAdd = !Pbk2NlxMenuFiltering::NonTopContactMarkedL( aControl );
#else
                dimAdd = ETrue;
#endif                
                }
            else
                {
                dimAdd = !Pbk2NlxMenuFiltering::TopReadyL( aControl );
                }
            if ( dimAdd )    
                {
                DimItem( aMenuPane, EPbk2CmdAddToTopContacts );
                }

            break;
            }
        case R_PHONEBOOK2_REMOVE_FROM_TOPCONTACTS:
            {
            TBool dimRemove = EFalse;    
            if ( aControl.ContactsMarked() )
                {
// multiple top assign/reassign doesn't work yet, enable when ready                
#if 0                
                dimRemove = !Pbk2NlxMenuFiltering::TopContactMarkedL( aControl );
#else
                dimRemove = ETrue;
#endif                
                }
            else
                {
                dimRemove = !Pbk2NlxMenuFiltering::TopContactSelectedL( aControl );
                }
            if ( dimRemove )    
                {
                DimItem( aMenuPane, EPbk2CmdRemoveFromTopContacts );
                }
            break;
            }
        case R_PHONEBOOK2_NAMELIST_CALL_CONTACT_MENU:
            {
            if ( Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( aControl ) )
                {
                DimItem( aMenuPane, EPbk2CmdCall );
                }
            if ( Pbk2NlxMenuFiltering::MyCardCmdSelected( aControl ) )
                {
                DimItem( aMenuPane, EPbk2CmdCall );
                }
            break;
            }
        case R_PHONEBOOK2_NAMELIST_CREATE_MESSAGE_MENU:
            {
            if ( !aControl.ContactsMarked() )
                {
                if ( Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( aControl ) )
                    {
                    DimItem( aMenuPane, EPbk2CmdWriteNoQuery );
                    }
                if ( Pbk2NlxMenuFiltering::MyCardCmdSelected( aControl ) )
                    {
                    DimItem( aMenuPane, EPbk2CmdWriteNoQuery );
                    }
                }
            break;
            }
        case R_PHONEBOOK2_NAMESLIST_SEND_URL_MENU:
            {
            if ( Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( aControl ) )
                {
                DimItem( aMenuPane, EPbk2CmdSend );
                }
            // if mycard is not created and is selected
			if( !MyCardLink() && Pbk2NlxMenuFiltering::MyCardCmdSelected( aControl ) )
				{
				DimItem( aMenuPane, EPbk2CmdSend );
				}
			break;
            }

#if 0                        
        case R_PHONEBOOK2_NAMESLIST_EDIT_MENU:  
            {                                   
            if ( aControl.ContactsMarked() ||
                aControl.NumberOfContacts() == 0 ||
                addFavouritesSelected ) 
                {
                DimItem( aMenuPane, EPbk2CmdEditMe ); 
                }
            break;
            }
#endif            
        case R_AVKON_MENUPANE_MARKABLE_LIST_IMPLEMENTATION:
        	{
        	MPbk2ContactUiControl2* tempControl = 
        	     reinterpret_cast<MPbk2ContactUiControl2*>
        	        (aControl.ContactUiControlExtension
        	            (KMPbk2ContactUiControlExtension2Uid ));

        	if ( tempControl->FocusedCommandItem() &&
        		 aControl.ContactsMarked() )
        		{
        		DimItem( aMenuPane, EAknCmdMark );
        		DimItem( aMenuPane, EAknMarkAll );
        		}
            if ( Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( aControl ) )
                {
                DimItem( aMenuPane, EAknCmdMark );
                }
            if ( Pbk2NlxMenuFiltering::MyCardCmdSelected( aControl ) )
                {
                DimItem( aMenuPane, EAknCmdMark );
                }
        	break;
        	}
        case R_PHONEBOOK2_MARKABLE_LIST_CONTEXT_MENU:
            {
            if ( Pbk2NlxMenuFiltering::AddToFavoritesCmdSelected( aControl ) )
                {
                DimItem( aMenuPane, EAknCmdMark );
                DimItem( aMenuPane, EAknCmdUnmark );
                }
            if ( Pbk2NlxMenuFiltering::MyCardCmdSelected( aControl ) )
                {
                DimItem( aMenuPane, EAknCmdMark );
                DimItem( aMenuPane, EAknCmdUnmark );
                }
            break;
            }
        case R_AVKON_MENUPANE_MARKABLE_LIST:
            {
            if ( !aControl.NumberOfContacts() )
                {
                DimItem( aMenuPane, EAknCmdEditListMenu );
                }
            break;
            }
        case R_PHONEBOOK2_NAMESLIST_COPY_MENU:
            {
            MPbk2ContactUiControl2* tempControl = 
                   reinterpret_cast<MPbk2ContactUiControl2*>
                       (aControl.ContactUiControlExtension(
                           KMPbk2ContactUiControlExtension2Uid ));

            if ( tempControl->FocusedCommandItem() &&
                 !aControl.ContactsMarked()   )
                {
                DimItem( aMenuPane, EPbk2CmdCopy );
                }
            break;
            }
        case R_PHONEBOOK2_NAMESLIST_DELETE_MENU:
            {
            // show delete mycard if mycard command is focused / tapped and 
            // mycard is available
            if( !Pbk2NlxMenuFiltering::MyCardCmdSelected( aControl ) ||
                !MyCardLink() )
                {
                DimItem( aMenuPane, EPbk2CmdDeleteMyCard );
                }
            break;
            }
        case R_PHONEBOOK2_MERGE_CONTACTS:
            {
            TInt numberOfContacts = 0;
            MVPbkContactLink* mylink = MyCardLink();
            numberOfContacts = iLocalStoreContactsCount - ( mylink ? 1 : 0 );
            
            // there must be two contacts to do merge
            if ( numberOfContacts  < KMinNumOfContactsToMerge )
                {
                aMenuPane->SetItemDimmed( EPbk2CmdMergeContacts, ETrue );
                }
            break;
            }
        default:
            {
            break;
            }
        }
    }
    
// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::DimItem
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::DimItem( CEikMenuPane* aMenuPane, TInt aCmd )
    {
    TInt p;  
    
    if ( aMenuPane->MenuItemExists( aCmd, p ) )
        {
        aMenuPane->SetItemDimmed( aCmd, ETrue );    
        } 
    }
    
// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::IsTopContact
// --------------------------------------------------------------------------
//
inline TBool CNamesListUIExtensionPlugin::IsTopContact( 
	const MVPbkBaseContact* aContact )
    {
    TBool topContact( EFalse );

    if( aContact ) 
        {
        topContact = CVPbkTopContactManager::IsTopContact( *aContact );
        }
        
    return topContact;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::MyCardLink
// --------------------------------------------------------------------------
//
inline MVPbkContactLink* CNamesListUIExtensionPlugin::MyCardLink() const
    {
    MVPbkContactLink* result = NULL;
    if( iNamesListExViewRef )
        {
        const CPbk2MyCard* mycard = iNamesListExViewRef->MyCard();
        if( mycard )
            {
            result = mycard->MyCardLink();
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::UpdateStorePropertiesL
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& /*aPropertyArray*/ )
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactEditorExtension*
    CNamesListUIExtensionPlugin::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& /*aContactManager*/,
          CPbk2PresentationContact& /*aContact*/,
          MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CreatePbk2UiControlExtensionL
// --------------------------------------------------------------------------
//
MPbk2ContactUiControlExtension*
    CNamesListUIExtensionPlugin::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& aContactManager )
    {
	CPbk2MyCard* mycard = NULL;
	if( iNamesListExViewRef )
		{
		mycard = iNamesListExViewRef->MyCard();
		}
    InitLocalStoreObserverL();
    
    MPbk2ContactUiControlExtension* extension = 
        CPbk2NameslistUiControlExtension::NewL( aContactManager, ContentProviderL(), mycard );
    return extension;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CreatePbk2SettingsViewExtensionL
// --------------------------------------------------------------------------
//
MPbk2SettingsViewExtension*
        CNamesListUIExtensionPlugin::CreatePbk2SettingsViewExtensionL()
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CreatePbk2AppUiExtensionL
// --------------------------------------------------------------------------
//
MPbk2AppUiExtension* CNamesListUIExtensionPlugin::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CreatePbk2CommandForIdL
// --------------------------------------------------------------------------
//
MPbk2Command* CNamesListUIExtensionPlugin::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl ) const
    {
    MPbk2Command* result = NULL;
    
    switch( aCommandId ) 
        {
        case EPbk2CmdOpenCca:
            {
            result = CPbk2NlxOpenCcaCmd::NewL( 
                aUiControl, 
                const_cast<MCCAConnection*&>( iCCAConnection ) );
            break;            
            }
            
        case EPbk2CmdAddToTopContacts:
            {
            // Give the ETrue argument for aAddFavourites = EFalse.
            result = CPbk2NlxAddToTopContactsCmd::NewL( aUiControl, EFalse );
            break;
            }
        case EPbk2CmdAddFavourites:
            {
            // Give the ETrue argument for aAddFavourites = ETrue.
            result = CPbk2NlxAddToTopContactsCmd::NewL( aUiControl, ETrue );
            break;
            }
        case EPbk2CmdRemoveFromTopContacts: 
            {
            result = CPbk2NlxRemoveFromTopContactsCmd::NewL( aUiControl );
            break;
            }     
        case EPbk2CmdMoveInTopContactsList: 
            {
            result = CPbk2NlxMoveTopContactsCmd::NewL( aUiControl );
            break;
            }
        case EPbk2CmdOpenMyCard:    // fallthrough
        case EPbk2CmdSelectMyCard:
			{
            result = CPbk2OpenMyCardCmd::NewL( 
                const_cast<MCCAConnection*&>( iCCAConnection ),
                &aUiControl );
			break;
			}
        case EPbk2CmdDeleteMyCard:
            {
            MVPbkContactLink* mylink = MyCardLink();
            if( mylink )
                {
                result = CPbk2DeleteMyCardCmd::NewL( aUiControl, *mylink );
                }
            break;
            }
        default:
            break;                              
        }
        
    return result;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CreatePbk2AiwInterestForIdL
// --------------------------------------------------------------------------
//
MPbk2AiwInterestItem* CNamesListUIExtensionPlugin::CreatePbk2AiwInterestForIdL
        ( TInt /*aInterestId*/,
          CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    // Do nothing
    return NULL;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::GetHelpContextL
// --------------------------------------------------------------------------
//
TBool CNamesListUIExtensionPlugin::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/,  const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    return EFalse;
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::ApplyDynamicViewGraphChangesL
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::ApplyDynamicViewGraphChangesL(
        CPbk2ViewGraph& /*aViewGraph*/)
    {
    // Do nothing
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::CmdOpenGroupL
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::CmdOpenGroupL
        ( MPbk2ContactUiControl& /*aUiControl*/ ) const
    {
    // Do nothing
    }
// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::ApplyDynamicPluginInformationDataL
//
// Allows extensions to add plugin information dynamically
// during the application initialisation.
//
// The UI extension's start-up policy must be KPbk2LoadInStartup.
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::ApplyDynamicPluginInformationDataL(
        CPbk2UIExtensionInformation& /* aUiExtensionInformation */ )
	{
    // Do nothing	
	}

	MPbk2ContactEditorExtension* CNamesListUIExtensionPlugin::CreatePbk2ContactEditorExtensionL(
            CVPbkContactManager& /*aContactManager*/,
            MVPbkStoreContact& /*aContact*/,
            MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    return NULL;
    }                
                      
MPbk2SettingsViewExtension* CNamesListUIExtensionPlugin::CreatePbk2SettingsViewExtensionL(
            CVPbkContactManager& /*aContactManager*/ )
    {
    return NULL;
    }                
MPbk2AiwInterestItem* CNamesListUIExtensionPlugin::CreatePbk2AiwInterestForIdL(
            TInt /*aInterestId*/,
            CAiwServiceHandler& /*aServiceHandler*/ )
    {
    return NULL;
    }             	

TAny* CNamesListUIExtensionPlugin::UIExtensionPluginExtension( TUid aExtensionUid )
    {
    TAny* ret = NULL;
    if (aExtensionUid == ImplementationUid())
        {
        ret = static_cast<MPbk2PluginCommandListerner*>( this );
        }    
    return ret;
    }

void CNamesListUIExtensionPlugin::HandlePbk2Command( TInt aCommand )
    {
    switch( aCommand )
        {
        // If Pbk2 goes to the backgound, child applications are closed
        case EAknCmdHideInBackground:
            {
            // Close CCA if it is open
            if ( iCCAConnection )
                {
                iCCAConnection->Close();
                iCCAConnection = NULL;
                }
            }
            break;
        
        default:
            break;
        }
    }            
// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::StoreReady
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::StoreReady( MVPbkContactStore& aContactStore )
    {
    if ( aContactStore.StoreProperties().Name().UriDes().Compare
            ( VPbkContactStoreUris::DefaultCntDbUri() ) )
        {
        iLocalStoreContactsCount = iLocalStore->StoreInfo().NumberOfContactsL();
        }
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::StoreUnavailable
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::StoreUnavailable
        ( MVPbkContactStore& aContactStore, TInt /*aReason*/ )
    {
    if ( aContactStore.StoreProperties().Name().UriDes().Compare
            ( VPbkContactStoreUris::DefaultCntDbUri() ) && iLocalStore )
        {
        iLocalStore->Close( *this );
        iLocalStore = NULL;
        }
    }

// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::HandleStoreEventL
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::HandleStoreEventL(
        MVPbkContactStore& aContactStore,
        TVPbkContactStoreEvent aStoreEvent )
    {
    if ( aContactStore.StoreProperties().Name().UriDes().Compare
            ( VPbkContactStoreUris::DefaultCntDbUri() ) )
        {
        switch ( aStoreEvent.iEventType )
           {
           case TVPbkContactStoreEvent::EContactAdded:
               {
               iLocalStoreContactsCount++;
               break;
               }
    
           case TVPbkContactStoreEvent::EContactDeleted:
               {
               iLocalStoreContactsCount--;
               break;
               }
               
           case TVPbkContactStoreEvent::EStoreBackupRestoreCompleted:
           case TVPbkContactStoreEvent::EUnknownChanges:
               {
               iLocalStoreContactsCount = iLocalStore->StoreInfo().NumberOfContactsL();
               break;
               }
               
           default:
               break;
           }
        }
    }
// --------------------------------------------------------------------------
// CNamesListUIExtensionPlugin::InitLocalStoreObserverL
// --------------------------------------------------------------------------
//
void CNamesListUIExtensionPlugin::InitLocalStoreObserverL()
    {
    if ( !iLocalStore )
        {
        MVPbkContactStoreList& storeList = iAppServices->ContactManager().ContactStoresL();
        iLocalStore = storeList.Find( VPbkContactStoreUris::DefaultCntDbUri() );
        if ( iLocalStore )
            {
            iLocalStore->OpenL( *this );
            }
        }
    }
//  End of File
