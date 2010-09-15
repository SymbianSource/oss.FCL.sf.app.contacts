/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of MyCard plugin
*
*/


#include "ccappmycardplugin.h"
#include "ccappmycardcontainer.h"
#include "ccappmycardcommon.h"
#include "ccappmycardpluginuids.hrh"
#include "ccappmycard.h"
#include "ccappmycard.hrh"
#include <spbcontactdatamodel.h>
#include <ccappmycardpluginrsc.rsg>
#include <pbk2uicontrols.rsg>
#include <data_caging_path_literals.hrh>
#include <mccappengine.h>
#include <bautils.h>
#include <AknsUtils.h>
#include <AiwServiceHandler.h>
#include <MVPbkContactLink.h>
#include <AiwContactAssignDataTypes.h>
#include <avkon.hrh>
#include <aknappui.h>
#include <s32mem.h>
#include <CPbk2CommandHandler.h>
#include <Pbk2Commands.hrh>		//pbk2cmdsend
#include <Pbk2DataCaging.hrh>	
#include <TPbk2ContactEditorParams.h>
#include <CPbk2GeneralConfirmationQuery.h>
#include <CPbk2PresentationContact.h>
#include <CPbk2PresentationContactFieldCollection.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <mccapppluginparameter.h>
#include <mccaparameter.h>
#include <CVPbkContactManager.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

/// MyCard own resource file
_LIT( KMyCardResourceFileName,          "ccappmycardpluginrsc.rsc" );
/// Phonebook2 UI controls resource file
_LIT( KMyCardPbk2UiControlsResFileName, "pbk2uicontrols.rsc" );
// pbk2 common ui
_LIT( KPbk2CommonUiDllResFileName,   	"Pbk2CommonUi.rsc"  );
// pbk2 commands
_LIT( KPbk2CommandsDllResFileName,   	"Pbk2Commands.rsc");



// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::NewL
// ---------------------------------------------------------------------------
//
CCCAppMyCardPlugin* CCCAppMyCardPlugin::NewL()
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("==== NEW INSTANCE============================================"));
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardPlugin::NewL()"));
    
    CCCAppMyCardPlugin* self = new ( ELeave ) CCCAppMyCardPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::NewL()"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::~CCCAppMyCardPlugin
// ---------------------------------------------------------------------------
//
CCCAppMyCardPlugin::~CCCAppMyCardPlugin()
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardPlugin::~CCCAppMyCardPlugin()"));
    
    iPbk2UiControlResource.Close();
    
    iCommonUiResourceFile.Close();
    
    iCommandsResourceFile.Close();
    
    delete iModel;

    delete iMyCard;
    
    delete iCommandHandler;
    
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::~CCCAppMyCardPlugin()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::CCCAppMyCardPlugin()
// ---------------------------------------------------------------------------
//
CCCAppMyCardPlugin::CCCAppMyCardPlugin() :
    iPbk2UiControlResource( *iCoeEnv ),
    iCommonUiResourceFile( *iCoeEnv ),
    iCommandsResourceFile( *iCoeEnv )
    {
    CCA_DP(KMyCardLogFile, CCA_L("CCCAppMyCardPlugin::CCCAppMyCardPlugin()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::ConstructL()
    {
    CCA_DP(KMyCardLogFile, CCA_L("CCCAppMyCardPlugin::ConstructL()"));
    // ConstructL should be as light as possible.

    //prepare view resources before creating command handler and mycard (was called on PreparePluginViewL before)
    PrepareViewResourcesL();
    
    iMyCard = CCCAppMyCard::NewL( *this );
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::MyCard
// ---------------------------------------------------------------------------
//
CCCAppMyCard& CCCAppMyCardPlugin::MyCard()
    {
    return *iMyCard;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::Model
// ---------------------------------------------------------------------------
//
CSpbContactDataModel& CCCAppMyCardPlugin::Model()
    {
    return *iModel;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::PreparePluginViewL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::PreparePluginViewL(
    MCCAppPluginParameter& aPluginParameter )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardPlugin::PreparePluginViewL()"));

    iModel = CSpbContactDataModel::NewL( 
        iMyCard->ContactManager(), *iCoeEnv, R_MYCARD_CLIP_FIELD_SELECTOR );
    
    BaseConstructL( R_CCAMYCARD_VIEW );
    MCCAParameter& param = aPluginParameter.CCAppLaunchParameter();
    if( param.ContactDataFlag() == MCCAParameter::EContactLink )
        {
        HBufC& data = param.ContactDataL();
        HBufC8* data8 = HBufC8::NewLC( data.Size() );
        data8->Des().Copy( data );
        CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC( 
            *data8, iMyCard->ContactManager().ContactStoresL() );
        if( array->Count() )
            {
            iMyCard->SetLinkL( array->At( 0 ) );
            }
        CleanupStack::PopAndDestroy( 2 ); // data, array
        }
    else if( param.ContactDataFlag() == MCCAParameter::EContactDataModel )
        {
        HBufC& cntData = param.ContactDataL();
        TPtrC8 data( (TUint8*)cntData.Ptr(), cntData.Size() );
        RDesReadStream stream( data );
        CleanupClosePushL( stream );
        iModel->InternalizeL( stream );
        CleanupStack::PopAndDestroy(); // strean

        MVPbkContactLink* link = iModel->ContactLink();
        if( link )
            {
            iMyCard->SetLinkL( *link );
            }
        else
            {
            // model without a link means that mycard does not exist.
            iMyCard->ForceCreateMyCard();
            }
        }
    
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::PreparePluginViewL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::DoActivateL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardPlugin::DoActivateL()"));

    // Forward the activation-call to base-class
    CCCAppViewPluginAknView::DoActivateL(
        aPrevViewId, aCustomMessageId, aCustomMessage );
    
    iMyCard->FetchMyCardL(); 
    
    // Set view title
    HBufC* title = iCoeEnv->AllocReadResourceLC( R_QTN_CCA_TITLE_MY_CARD );
    SetTitleL( *title );
    CleanupStack::PopAndDestroy( title );

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::DoActivateL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::DoDeactivate
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::DoDeactivate()
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardPlugin::DoDeactivate()"));

    // iOwnContainer should update command to reset the UiControl when deactivated.
    iOwnContainer->UpdateAfterCommandExecution();
    
    // Forward the deactivation-call to base-class
    CCCAppViewPluginAknView::DoDeactivate();

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::DoDeactivate()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::ProvideBitmapL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::ProvideBitmapL(
    TCCAppIconType aIconType,
    CAknIcon& aIcon )
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardPlugin::ProvideBitmapL()"));

    if ( ECCAppTabIcon == aIconType )
        {
        // CCA expects to always receive real icons. MyCard does not have
        // tab icon defined, so provide some dummy icons.
        aIcon.SetBitmap( new(ELeave) CFbsBitmap );
        aIcon.SetMask( new(ELeave) CFbsBitmap );
        }

    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::ProvideBitmapL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::CCCAppViewPluginBaseExtension
// ---------------------------------------------------------------------------
//
TAny* CCCAppMyCardPlugin::CCCAppViewPluginBaseExtension( TUid aExtensionUid )
    {
    if ( aExtensionUid == KMCCAppViewPluginBaseExtension2Uid )
        {
        return static_cast<MCCAppViewPluginBase2*>( this );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::HandleCommandL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::HandleCommandL( TInt aCommand )
    {
    CCA_DP(KMyCardLogFile, 
        CCA_L("->CCCAppMyCardPlugin::HandleCommandL command=%d"), aCommand );
                
    switch( aCommand )
        {                
        case ECCAppMyCardCmdEdit:
            {            
            TInt index;            
            MVPbkStoreContact& storeContact = iMyCard->StoreContact();
            
            TRAPD( err,
                {
                index = iMyCard->PresentationContactL().PresentationFields().StoreIndexOfField( 0 );                        
        
                MVPbkBaseContactField* field = storeContact.Fields().FieldAtLC( index );
                                        
                const MVPbkFieldType* type = field->BestMatchingFieldType();
                const TArray<TVPbkFieldVersitProperty> fieldProperty = type->VersitProperties();
    
                if( fieldProperty.Count() )
                    {
                    const TVPbkFieldVersitProperty property = fieldProperty[0];
                        
                    if( property.Name() == EVPbkVersitNameADR )
                        {
                        index = KErrNotSupported;
                        }            
                    }                       
                                                 
                if( field )
                    {
                    CleanupStack::PopAndDestroy( field );
                    }
            
                EditL( index );
                } );
            
            if( err != KErrNone )
                {
                HandleError( err );                
                }
            break;
            }
        case ECCappMyCardCmdSendVCard:
        	{
        	SendBusinessCardL();
        	break;
        	}
        case ECCappMyCardCmdDelete: 
        	{
            HBufC* prompt = 
                StringLoader::LoadLC( R_QTN_PHOB_MY_CARD_CLEAR_CONFIRM );
            CAknQueryDialog* dlg = CAknQueryDialog::NewL();
            if( dlg->ExecuteLD( R_PBK2_GENERAL_CONFIRMATION_QUERY, *prompt ) )
                {
                iMyCard->PresentationContactL().DeleteL( *iMyCard );                
                }
            CleanupStack::PopAndDestroy( prompt );
        	break;
        	}        	
        default:
            {
            // Forward rest to base class
            CCCAppViewPluginAknView::HandleCommandL( aCommand );
            }
        }
    
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::HandleCommandL()"));
    }


// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::EditL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::EditL( TInt aFocusedFieldIndex )
	{
	iMyCard->EditContactL(  aFocusedFieldIndex );
	}

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::HandleError
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::HandleError( TInt aError )
    {
    CCoeEnv::Static()->HandleError( aError );
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::NewContainerL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::NewContainerL()
    {
    iOwnContainer = new (ELeave) CCCAppMyCardContainer( *this );
    iContainer = iOwnContainer;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::PluginBusy
// ---------------------------------------------------------------------------
//
TBool CCCAppMyCardPlugin::PluginBusy()
    {
    return EFalse;
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::Id
// ---------------------------------------------------------------------------
//
TUid CCCAppMyCardPlugin::Id() const
    {
    return TUid::Uid( KCCAMyCardPluginImplmentationUid );
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::PrepareViewResourcesL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::PrepareViewResourcesL()
    {
    CCA_DP(KMyCardLogFile, CCA_L("->CCCAppMyCardPlugin::PrepareViewResourcesL()"));
    
    // MyCard own resource file
    TFileName fileName( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KMyCardResourceFileName );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResourceLoader.OpenL( fileName );
    
    /// Phonebook 2 UI controls resource
    fileName.Copy( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KMyCardPbk2UiControlsResFileName );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iPbk2UiControlResource.OpenL( fileName );
    
    // commands
    fileName.Copy( KDC_RESOURCE_FILES_DIR );
    fileName.Append( KPbk2CommandsDllResFileName );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iCommandsResourceFile.OpenL( fileName );
    
    // common ui
    fileName.Copy( KDC_RESOURCE_FILES_DIR );
	fileName.Append( KPbk2CommonUiDllResFileName );
	BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
	iCommonUiResourceFile.OpenL( fileName );
	
    CCA_DP(KMyCardLogFile, CCA_L("<-CCCAppMyCardPlugin::PrepareViewResourcesL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::CommandHandlerL
// ---------------------------------------------------------------------------
//
CPbk2CommandHandler* CCCAppMyCardPlugin::CommandHandlerL()
	{
	// create command handler if not created yet
	if( !iCommandHandler )
		{
		// command handler
		iCommandHandler = CPbk2CommandHandler::NewL();
		}
	return iCommandHandler;
	}


// ---------------------------------------------------------------------------
// CCCAppMyCardPlugin::SendBusinessCardL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::SendBusinessCardL()
	{
	// send business card
	CommandHandlerL()->HandleCommandL( EPbk2CmdSend, *iOwnContainer, NULL );
	}

// ---------------------------------------------------------------------------
// ProcessCommandL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::ProcessCommandL(TInt aCommandId)
     {
     TInt err = KErrNone;
     
     switch(aCommandId)
         {
         case ECCAppMyCardCmdStylusViewImageCmd:             
             TRAP( err, ViewImageCmdL() );             
             break;             
             
         case ECCAppMyCardCmdStylusChangeImageCmd:
             TRAP( err, ChangeImageCmdL() );             
             break;
             
         case ECCAppMyCardCmdStylusRemoveImageCmd:
             TRAP( err, RemoveImageCmdL() );             
             break;
             
         case ECCAppMyCardCmdStylusAddImageCmd:
             TRAP( err, AddImageCmdL() );             
             break;
             
         case ECCAppMyCardCmdStylusCopyDetailCmd:             
              TRAP( err, CopyDetailL() );              
              break;                        
             
         default:
             CAknView::ProcessCommandL( aCommandId );
             break;
         }
     
      if( err != KErrNone )
         {
         HandleError( err );
         }
     }

// --------------------------------------------------------------------------- 
// CCCAppMyCardPlugin::RemoveImageCmdL
// --------------------------------------------------------------------------- 
// 
void CCCAppMyCardPlugin::RemoveImageCmdL() 
    { 
	CommandHandlerL()->HandleCommandL( EPbk2CmdRemoveImage, *iOwnContainer, NULL );
    } 

// --------------------------------------------------------------------------- 
// CCCAppMyCardPlugin::AddImageCmdL
// --------------------------------------------------------------------------- 
// 
void CCCAppMyCardPlugin::AddImageCmdL() 
    { 
	CommandHandlerL()->HandleCommandL( EPbk2CmdAddImage, *iOwnContainer, NULL );
    } 
 
// --------------------------------------------------------------------------- 
// CCCAppMyCardPlugin::ViewImageCmdL
// --------------------------------------------------------------------------- 
// 
void CCCAppMyCardPlugin::ViewImageCmdL() 
    { 
	CommandHandlerL()->HandleCommandL( EPbk2CmdViewImage, *iOwnContainer, NULL );
    } 

// --------------------------------------------------------------------------- 
// CCCAppMyCardPlugin::ChangeImageCmdL
// --------------------------------------------------------------------------- 
// 
void CCCAppMyCardPlugin::ChangeImageCmdL() 
    { 
	CommandHandlerL()->HandleCommandL( EPbk2CmdChangeImage, *iOwnContainer, NULL );
    } 

// ---------------------------------------------------------------------------
// CCCAppMyCardContainer::CopyDetailL
// ---------------------------------------------------------------------------
//
void CCCAppMyCardPlugin::CopyDetailL()
    {
    CommandHandlerL()->HandleCommandL( EPbk2CmdCopyDetail, *iOwnContainer, NULL );
    }

void CCCAppMyCardPlugin::EnableOptionsMenu( TBool aEnable )
    {
    Cba()->MakeCommandVisibleByPosition( 
           CEikButtonGroupContainer::ELeftSoftkeyPosition, aEnable );
    }
// End of File
