/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  AppUi -part of the CCApplication
*
*/


#include "ccappheaders.h"

// ---------------------------------------------------------------------------
// CCCAAppAppUi::CCCAAppAppUi
// ---------------------------------------------------------------------------
//
CCCAAppAppUi::CCCAAppAppUi()
    {
    // The default navigation keys
    iPrevViewKey = EKeyLeftArrow;
    iNextViewKey = EKeyRightArrow;
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::ConstructL()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::ConstructL"));

    FeatureManager::InitializeLibL();
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    iCCAppView = CCCAppView::NewL( *this );

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::ConstructL"));
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::~CCCAAppAppUi
// ---------------------------------------------------------------------------
//
CCCAAppAppUi::~CCCAAppAppUi()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::~CCCAAppAppUi"));

    FeatureManager::UnInitializeLib();
    delete iCCAppView;
    delete iCCAParameter;
    iReservedKeys.Reset();
    delete iRepository;

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::~CCCAAppAppUi"));
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::HandleCommandL( TInt aCommand )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::HandleCommandL"));
    CCA_DP( KCCAppLogFile, CCA_L("::HandleCommandL - aCommand: %d"), aCommand );

    switch( aCommand )
        {
        // Handle the launching of help in CCApplication side
        // (plugin needs just to implement GetHelpContext in container)
        case EAknCmdHelp:
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ))
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), AppHelpContextL() );
                }
            break;
        // Handle the exits in CCApplication side
        case EEikCmdExit:// fallthrough
        case EAknSoftkeyBack:// fallthrough
        case EAknSoftkeyExit:
            Exit();
            break;
        case EAknCmdExit: 
            {
            // Special case with mycard contact editor, which calls 
            // HandleCommandL directly. This command should be handled in 
            // CAknViewAppUi::ProcessCommandL()
            ProcessCommandL( EAknCmdExit );
            break;
            }
        default:
            break;
        }

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::HandleCommandL"));
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::PrepareToExit
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::PrepareToExit()
    {
    CAknViewAppUi::PrepareToExit();
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::HandleKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CCCAAppAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::HandleKeyEventL"));

    // if plug-in is busy with something, just ignore key event
    if ( iCCAppView->CurrentPluginBusy() && !CCoeAppUi::IsDisplayingDialog() )
        {
        return EKeyWasConsumed;
        }

    TKeyResponse returnValue = EKeyWasNotConsumed;

    if ( !iSettingsLoaded )
        {
        // Load the setting to know which navigation key we accept
        TRAP_IGNORE( LoadSettingsL() );
        }

    if ( EEventKey == aType
        && ( iNextViewKey == aKeyEvent.iCode || iPrevViewKey == aKeyEvent.iCode ))
        {
        // Handle the reserved keys
        CCA_DP( KCCAppLogFile, CCA_L("::HandleKeyEventL - reserved key"));
        if ( KErrNone != iCCAppView->ChangeView( iNextViewKey == aKeyEvent.iCode ))
            {
            Exit();
            }
        returnValue = EKeyWasConsumed;
        }

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::HandleKeyEventL"));
    return returnValue;
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::HandleWsEventL
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::HandleWsEventL(const TWsEvent &aEvent,
    CCoeControl *aDestination)
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::HandleWsEventL"));
    // handle WS event only if plug-in is not busy with something
    if ( !iCCAppView->CurrentPluginBusy() || CCoeAppUi::IsDisplayingDialog() )
        {
        CAknViewAppUi::HandleWsEventL( aEvent, aDestination );
        }
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::HandleWsEventL - OUT"));
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::InitializePlugin
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::InitializePluginL()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::InitializePluginL"));



    TInt error = iCCAppView->StartView( iCCAParameter->LaunchedViewUid() );
    if ( KErrNone != error )
        {
        User::Leave( error );
        }
    
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::InitializePluginL"));
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::SetParameter
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::SetParameter( CCCAParameter& aParameter )
	{
	iCCAParameter = &aParameter;
	}

// ---------------------------------------------------------------------------
// CCCAAppAppUi::Parameter
// ---------------------------------------------------------------------------
//
CCCAParameter& CCCAAppAppUi::Parameter()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAAppAppUi::Parameter"));
    return *iCCAParameter;
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::LoadSettingsL
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::LoadSettingsL()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::LoadSettingsL"));

    if ( !iRepository )
        iRepository = CRepository::NewL( KCRUIDCCAApplication );

    TInt error = iRepository->Get( KPrevViewKey, iPrevViewKey );
    CCA_DP( KCCAppLogFile, CCA_L("::LoadSettingsL - iPrevViewKey settings loaded, error: %d"), error );

    if( KErrNone == error )
        {
        error = iRepository->Get( KNextViewKey, iNextViewKey );
        CCA_DP( KCCAppLogFile, CCA_L("::LoadSettingsL - iNextViewKey settings loaded, error: %d"), error );

        if( KErrNone == error )
            {
            iSettingsLoaded = ETrue;
            }
        }

    iReservedKeys.Reset();
    iReservedKeys.Append( iPrevViewKey );
    iReservedKeys.Append( iNextViewKey );

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::LoadSettingsL"));
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::EikonEnv
// ---------------------------------------------------------------------------
//
CEikonEnv& CCCAAppAppUi::EikonEnv()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAAppAppUi::EikonEnv"));
    return *iEikonEnv;
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::ReservedKeys
// ---------------------------------------------------------------------------
//
RArray<TInt>& CCCAAppAppUi::ReservedKeys()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAAppAppUi::ReservedKeysL"));

    if ( !iSettingsLoaded )
        {
        // Load the setting to know which navigation key we accept
        TRAP_IGNORE( LoadSettingsL() );
        }

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAAppAppUi::ReservedKeysL"));
    return iReservedKeys;
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::RemoveInvalidPluginL
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::RecoverFromBadPluginL()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAAppAppUi::RecoverFromBadPluginL"));
    iCCAppView->RecoverFromBadPluginL();
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::HandleError
// ---------------------------------------------------------------------------
//
TErrorHandlerResponse CCCAAppAppUi::HandleError(
    TInt aError,
    const SExtendedError& aExtErr,
    TDes& aErrorText,
    TDes& aContextText )
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAAppAppUi::HandleError"));

    // HandleError seems to be the only way (?) to catch
    // if DoActivateL leaves during the Avkon fw callbacks.
    //
    // Problem is that "normal leaves" like HandleCommandL leave
    // or DynInitMenuPaneL leave will also come HandleError. This
    // is now handled with following if the view ids are out of sync.
    // If they're out of sync, it means that most propably something
    // went wrong during the last view activation. CCAppUi cannot do much
    // for that situation -> plugin is unloaded.
    TBool viewsInSync =
        iView->Id() == iCCAppView->PluginLoader().PluginInFocus()->Plugin().Id();
    if ( !viewsInSync )
        TRAP_IGNORE( RecoverFromBadPluginL() );

    return CAknAppUi::HandleError(
        aError, aExtErr, aErrorText, aContextText );
    }

// ---------------------------------------------------------------------------
// CCCAAppAppUi::SetTitleL
// ---------------------------------------------------------------------------
//
void CCCAAppAppUi::SetTitleL( const TDesC& aTitle )
    {	  
    CEikStatusPane* statuspane = EikonEnv().
            AppUiFactory()->StatusPane();//not owned
    CAknTitlePane* titlepane = (CAknTitlePane*)statuspane->
        ControlL(TUid::Uid(EEikStatusPaneUidTitle));//not owned
    
    TPtrC oldText = *titlepane->Text();
    
    if ( oldText.Compare( aTitle ))
        titlepane->SetTextL( aTitle, ETrue );    
	
    }

// end of file



