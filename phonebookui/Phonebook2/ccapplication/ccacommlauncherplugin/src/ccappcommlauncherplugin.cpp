/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of comm launcher view plugin
*
*/


#include "ccappcommlauncherheaders.h"
#include <phonebook2ece.mbg>
#include "ccappcommlaunchersetdefault.h"


#include <mccappengine.h>

// Consts
_LIT( KCCAppCommLauncherResourceFileName, "\\resource\\ccappcommlauncherpluginrsc.rsc" );
const TInt KCCAppCommLauncherMaxOrderBufLength = 128;

const TInt KSupportedMethodsArray[] = {
    VPbkFieldTypeSelectorFactory::EVoiceCallSelector,
    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
    VPbkFieldTypeSelectorFactory::EUniEditorSelector,
    VPbkFieldTypeSelectorFactory::EEmailEditorSelector,
    VPbkFieldTypeSelectorFactory::EInstantMessagingSelector,
    VPbkFieldTypeSelectorFactory::EURLSelector,
    VPbkFieldTypeSelectorFactory::EVideoCallSelector,
    VPbkFieldTypeSelectorFactory::EFindOnMapSelector
    };

const TInt KSupportedMethodsArrayLength = sizeof( KSupportedMethodsArray ) / sizeof( TInt );

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::NewL
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPlugin* CCCAppCommLauncherPlugin::NewL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::NewL()"));
    CCCAppCommLauncherPlugin* self = new ( ELeave ) CCCAppCommLauncherPlugin();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::NewL()"));
    return self;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::~CCCAppCommLauncherPlugin
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPlugin::~CCCAppCommLauncherPlugin()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::~CCCAppCommLauncherPlugin()"));
    iPreferredCommMethods.Close();

    if (AppEngine())
        {
        AppEngine()->RemoveObserver(*this);
        }

    delete iMenuHandler;

    delete iContactHandler;
    delete iSetDefault;
    delete iContactorService;
    
    if ( iWaitFinish && iWaitFinish->IsStarted() )
       {
       iWaitFinish->AsyncStop();
       }
    delete iWaitFinish;

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::~CCCAppCommLauncherPlugin()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::CCCAppCommLauncherPlugin()
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherPlugin::CCCAppCommLauncherPlugin()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherPlugin()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::ConstructL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("ConstructL()"));
    // ConstructL should be as light as possible.
    iWaitFinish = new (ELeave) CActiveSchedulerWait();
    iSetDefault = CCCAppCommLauncherLSetDefault::NewL(*this);
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::PreparePluginViewL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::PreparePluginViewL(
    MCCAppPluginParameter& /*aPluginParameter*/ )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::PreparePluginViewL()"));

    // This plugin has no usage for the aPluginParameter

    if (AppEngine())
        {
        AppEngine()->AddObserverL(*this);
        }

    //PERFORMANCE LOGGING: 7. Preparing plugin
    WriteToPerfLog();

    PrepareViewResourcesL();
    InitialisePreferredCommMethods();

    BaseConstructL( R_COMMLAUNCHER_VIEW );
    
    // Set default for Voice call and Message
    // Not auto setting default for now
    // iSetDefault->ExecuteAssignDefaultL();
    // iWaitFinish->Start();

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::PreparePluginViewL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::DoActivateL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::DoActivateL(
    const TVwsViewId& aPrevViewId,
    TUid aCustomMessageId,
    const TDesC8& aCustomMessage )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::DoActivateL()"));

    //PERFORMANCE LOGGING: 8. Activating view
    WriteToPerfLog();

    AppUi()->SetKeyEventFlags(
        CAknAppUiBase::EDisableSendKeyShort |
        CAknAppUiBase::EDisableSendKeyLong );
    // Forward the activation-call to base-class
    CCCAppViewPluginAknView::DoActivateL(
        aPrevViewId, aCustomMessageId, aCustomMessage );

    CCA_DP(KCommLauncherLogFile, CCA_L("::DoActivateL() - create iContactHandler"));
    iContactHandler = CCCAppCommLauncherContactHandler::NewL(
        *static_cast<CCCAppCommLauncherContainer*>(iContainer),
        *this);
    CCA_DP(KCommLauncherLogFile, CCA_L("::DoActivateL() - request contact data"));

    if (iRefetchContact)
    	{
        iContactHandler->RefetchContactL();
        iRefetchContact = EFalse;
      	}
    else
    	{
        iContactHandler->RequestContactDataL();
    	}

    if (!iContactorService)
        {
        iContactorService = CCAContactorService::NewL();
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::DoActivateL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::DoDeactivate
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::DoDeactivate()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::DoDeactivate()"));

    delete iContactHandler;
    iContactHandler = NULL;

    AppUi()->SetKeyEventFlags( 0 );
    // Forward the deactivation-call to base-class
    CCCAppViewPluginAknView::DoDeactivate();

    // if plugin is showing notifications, this is good place to disable those
    // until ActivatePluginViewL is called again

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::DoDeactivate()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::InitialisePreferredCommMethods
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::InitialisePreferredCommMethods()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::InitialisePreferredCommMethods()"));

    TRAPD( err, LoadPreferredCommMethodsL() );
    if ( err )
        {// something went wrong with loading from cenrep,
        //  use the default order
#ifdef _DEBUG
        CCA_DP(KCommLauncherLogFile, CCA_L("::InitialisePreferredCommMethods - Comm method variation error."));
        CCA_DP(KCommLauncherLogFile, CCA_L("::InitialisePreferredCommMethods - err: %d"), err );
        CEikonEnv::Static()->InfoMsg(_L("Comm method variation error. See logs."));
#endif//_DEBUG
        iPreferredCommMethods.Reset();

        for ( TInt idx = 0; idx < KSupportedMethodsArrayLength; idx++ )
            {
            iPreferredCommMethods.Append(
                (VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector)
                KSupportedMethodsArray[ idx ]);
            }
        }
    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::InitialisePreferredCommMethods()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::LoadPreferredCommMethodsL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::LoadPreferredCommMethodsL()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::LoadPreferredCommMethodsL()"));

    CRepository* repository = CRepository::NewLC( KCRUidCCACommLauncher );
    TBuf<KCCAppCommLauncherMaxOrderBufLength> orderBuf;
    TInt error = repository->Get( KCommunicationMethodOrder, orderBuf );
    CleanupStack::PopAndDestroy( repository );

    TLex orderLex( orderBuf );
    TPtrC numToken;
    TLex numLex;
    TInt commMethodType;
    const TInt bufLength = orderBuf.Length() - 1;

    while ( orderLex.Offset() < bufLength )
        {
        numToken.Set( orderLex.NextToken() );
        numLex.Assign( numToken );
        // if defined value is string instead of a number,
        // use the default array
        User::LeaveIfError( numLex.Val( commMethodType ));
        CCA_DP(KCommLauncherLogFile, CCA_L("::LoadPreferredCommMethodsL - found commMethodType: %d"), commMethodType );
        // if defined value is outside of the possible comm method
        // values, use the default array
        TBool knownMethod(EFalse);
        for( TInt idx = 0; idx < KSupportedMethodsArrayLength; idx++ )
            {
            if( KSupportedMethodsArray[ idx ] == commMethodType )
                {
                knownMethod = ETrue;
                break;
                }
            }
        if(!knownMethod)
            {
            CCA_DP(KCommLauncherLogFile, CCA_L("::LoadPreferredCommMethodsL - skippin unsupported method(s): %d"),
                    commMethodType );
            User::Leave( KErrArgument );
            }

        iPreferredCommMethods.Append(
            VPbkFieldTypeSelectorFactory::CreateActionTypeSelectorIdL(
                commMethodType ) );
        }

    // if no comm methods defined, use the default array
    User::LeaveIfError( iPreferredCommMethods.Count() ? 0 : KErrNotFound );

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::LoadPreferredCommMethodsL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::ProvideBitmapL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::ProvideBitmapL(
    TCCAppIconType aIconType,
    CAknIcon& aIcon )
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("->CCCAppCommLauncherPlugin::ProvideBitmapL()"));

    if ( ECCAppTabIcon == aIconType )
        {
        CFbsBitmap* bmp = NULL;
        CFbsBitmap* bmpMask = NULL;

        AknsUtils::CreateIconL(
            AknsUtils::SkinInstance(),
            KAknsIIDDefault,//todo; get a proper skin
            bmp,
            bmpMask,
            KPbk2ECEIconFileName,
            EMbmPhonebook2eceQgn_prop_pb_comm_tab3,
            EMbmPhonebook2eceQgn_prop_pb_comm_tab3_mask );

        aIcon.SetBitmap( bmp );
        aIcon.SetMask( bmpMask );
        }

    CCA_DP(KCommLauncherLogFile, CCA_L("<-CCCAppCommLauncherPlugin::ProvideBitmapL()"));
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::CCCAppViewPluginBaseExtension
// ---------------------------------------------------------------------------
//
TAny* CCCAppCommLauncherPlugin::CCCAppViewPluginBaseExtension( TUid aExtensionUid )
    {
    if ( aExtensionUid == KMCCAppViewPluginBaseExtension2Uid )
        {
        return static_cast<MCCAppViewPluginBase2*>( this );
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    EnsureMenuHandlerCreatedL();
    iMenuHandler->DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::EnsureMenuHandlerCreatedL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::EnsureMenuHandlerCreatedL()
    {
    if ( !iMenuHandler )
        {
        iMenuHandler = CCCAppCommLauncherMenuHandler::NewL( *this );
        iMenuHandler->SetContactStore( iContactHandler->ContactStore() );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherContainer::ContactorService
// ---------------------------------------------------------------------------
//
CCAContactorService* CCCAppCommLauncherPlugin::ContactorService()
    {
    return iContactorService;
    }

void CCCAppCommLauncherPlugin::DefaultSettingComplete()
	{
	if ( iWaitFinish && iWaitFinish->IsStarted() )
	   {
	   iWaitFinish->AsyncStop();
	   }
	}
// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::IsTopContactL
// ---------------------------------------------------------------------------
//
TBool CCCAppCommLauncherPlugin::IsTopContactL()
    {
    return EFalse;//todo
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::IsContactL
// ---------------------------------------------------------------------------
//
TBool CCCAppCommLauncherPlugin::IsContactL()
    {
    return ETrue;//todo
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::HandleCommandL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::HandleCommandL( TInt aCommand )
    {
    // Forward the command handling 1st to base-class.
    // The "Exit"- and "Back"-commands are handled there.
    CCCAppViewPluginAknView::HandleCommandL( aCommand );

    // Rest to menuhandler
    EnsureMenuHandlerCreatedL();
    iMenuHandler->HandleCommandL( aCommand );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::PreferredCommMethods
// ---------------------------------------------------------------------------
//
RArray<VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector>&
    CCCAppCommLauncherPlugin::PreferredCommMethods()
    {
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherPlugin::PreferredCommMethods()"));
    return iPreferredCommMethods;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::ErrorNotifierL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::ErrorNotifierL(
    TCCAppCommLauncherPluginErrorSource aErrorSource,
    TInt aError )
    {
    // possibility to handle the error cases
    CCA_DP(KCommLauncherLogFile, CCA_L("CCCAppCommLauncherPlugin::ErrorNotifierL() called"));
    CCA_DP(KCommLauncherLogFile, CCA_L("::ErrorNotifierL aError: %d"), aError );
    CCA_DP(KCommLauncherLogFile, CCA_L("::ErrorNotifierL aErrorSource: %d"), (TInt)aErrorSource );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::Container
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherContainer& CCCAppCommLauncherPlugin::Container()
    {
    return *static_cast<CCCAppCommLauncherContainer*>(iContainer);
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::ContactHandler
// ---------------------------------------------------------------------------
//
CCCAppCommLauncherContactHandler& CCCAppCommLauncherPlugin::ContactHandler()
    {
    return *iContactHandler;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::NewContainerL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::NewContainerL()
    {
    iContainer = new (ELeave) CCCAppCommLauncherContainer( *this );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::PluginBusy()
// ---------------------------------------------------------------------------
//
TBool CCCAppCommLauncherPlugin::PluginBusy()
    {
    TBool ret = EFalse;
    
    if ( iContactorService != NULL )
       {
       ret = iContactorService->IsBusy();
       }
    return ret;
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::Id
// ---------------------------------------------------------------------------
//
TUid CCCAppCommLauncherPlugin::Id()const
    {
    return TUid::Uid( KCCACommLauncherPluginImplmentationUid );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::PrepareViewResourcesL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::PrepareViewResourcesL()
    {
    // preparing resources for use
    TFileName fileName( KCCAppCommLauncherResourceFileName );
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );
    iResourceLoader.OpenL( fileName );
    }


// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::SetTitleL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::SetTitleL( const TDesC& aTitle )
    {
    // Forward the SetTitleL-call to base-class
    CCCAppViewPluginAknView::SetTitleL( aTitle );
    }

// ---------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::NotifyPluginOfContactChangeL
// ---------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::NotifyPluginOfContactEventL()
    {
    iRefetchContact = ETrue;
    }

// --------------------------------------------------------------------------
// CCCAppCommLauncherPlugin::UpdateMSKinCbaL
// Sets CBA with empty MSK or MSK with text "Select" when contact has
// communication methods available.
// --------------------------------------------------------------------------
//
void CCCAppCommLauncherPlugin::UpdateMSKinCbaL( TBool aCommMethodsAvailable )
{
    CEikButtonGroupContainer* cba = Cba();

    if ( cba )
        {
        cba->SetCommandSetL(aCommMethodsAvailable
            ? R_AVKON_SOFTKEYS_OPTIONS_BACK__SELECT
            : R_CCACOMMLAUNCHER_SOFTKEYS_OPTIONS_BACK_EMPTY);

        cba->DrawDeferred();
        }
}

// End of File
