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
* Description:  A class responsible handling the views
 *
*/


// INCLUDE FILES
#include "ccappheaders.h"
#include <mccappviewpluginbase2.h>
#include "ccappmycardpluginuids.hrh"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppView::CCCAppView
// ---------------------------------------------------------------------------
//
CCCAppView::CCCAppView( CCCAAppAppUi& aAppUi )
    : iAppUi ( aAppUi ),
      iViewChangeInProgress ( EFalse )
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppView::CCCAppView"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::~CCCAppView
// ---------------------------------------------------------------------------
//
CCCAppView::~CCCAppView()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::~CCCAppView"));
    
    iPluginUidToBeSynchd.Close();
    
    if ( iCmsWrapper )
        {
        iCmsWrapper->RemoveObserver( *this );    
        iCmsWrapper->Release();
        }
    iObservers.Reset();
    iObservers.Close();
    delete iPaneHandler; 
    delete iPluginLoader;
    delete iViewResetter;
    delete iDelayedConstructor;

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::~CCCAppView"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::NewL
// ---------------------------------------------------------------------------
//
CCCAppView* CCCAppView::NewL( CCCAAppAppUi& aAppUi )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::NewL"));
    
    CCCAppView* self = new( ELeave ) CCCAppView( aAppUi );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::NewL"));
    return self;
	}

// ---------------------------------------------------------------------------
// CCCAppView::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppView::ConstructL()
    {
    //CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::ConstructL"));

    //CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ConstructL"));
	} 

// ---------------------------------------------------------------------------
// CCCAppView::StartView
// ---------------------------------------------------------------------------
//
TInt CCCAppView::StartView( const TUid aUid )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::StartView"));
    TRAPD( err, StartViewL( aUid ))
    CCA_DP( KCCAppLogFile, CCA_L("::StartView - StartViewL returned err: %d"), err );
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::StartView"));
    return err;
    }

// ---------------------------------------------------------------------------
// CCCAppView::StartViewL
// ---------------------------------------------------------------------------
//
void CCCAppView::StartViewL( const TUid aUid )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::StartViewL"));
    iDelayedStarting = ETrue;

    // 1st start the contact fetching
    iCmsWrapper = CCCAppCmsContactFetcherWrapper::CreateInstanceL( 
         &iAppUi.Parameter(), CCCAppCmsContactFetcherWrapper::EFindContactFromOtherStores );
    iCmsWrapper->AddObserverL( *this );

    // TODO: replace hardcoded uid comparison with better sollution that
    // supports other mycard views too
    TBool isUidMyCard = ( aUid == 
            TUid::Uid( KCCAMyCardPluginImplmentationUid ) );

    
    //In this version support only contact type data (i.e. group data discarded)
    _LIT8( KCcaOpaqueTag_CNT,           "CNT" );    //Contact    
    _LIT8( KCcaOpaqueTag_MCD,           "MCD" );    //MyCard    
    _LIT8( KCcaPluginTypeMyCard,        "application/x-mycard" );    
    
    TPtrC8 properties( isUidMyCard ? KCcaOpaqueTag_MCD : KCcaOpaqueTag_CNT);
    TPtrC8 type( KNullDesC8 );
    if( isUidMyCard )
        {
        type.Set( KCcaPluginTypeMyCard );    
        }
    iPluginLoader = CCCAppPluginLoader::NewL( this, properties, type );
    CCA_DP( KCCAppLogFile, CCA_L("::ConstructL - iPluginLoader created"));
    iPaneHandler = CCCAppStatusPaneHandler::NewL( *this );
    CCA_DP( KCCAppLogFile, CCA_L("::ConstructL - iPaneHandler created"));    
    
    // 2nd start constructing the plugin view with preferred
    // plugin view (if provided)
    TInt pluginToFocus = iPluginLoader->SetPluginInFocus( aUid );

    if(pluginToFocus != KErrNotFound)
        {
        iPluginLoader->SetPluginVisibility(aUid, CCCAppPluginData::EPluginVisible); 
        }
    else
        {
        //Not found, try set the first plugin
        pluginToFocus = 0;
        if(iPluginLoader->PluginAt(pluginToFocus)) 
            {
            iPluginLoader->SetPluginVisibility(
                iPluginLoader->PluginAt(pluginToFocus)->Plugin().Id(), CCCAppPluginData::EPluginVisible);  
            }
        }

    //Start plugin and construct CIdle object to call back to finalise 
    //time consuming construction    
    StartPluginL(pluginToFocus);
    iDelayedConstructor = CIdle::NewL( CActive::EPriorityIdle );
    iDelayedConstructor->Start( TCallBack( ConstructDelayedCallbackL, this) );            
    
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::StartViewL"));
	} 

// ----------------------------------------------------------------------------
// CCCAppView::ConstructDelayedCallbackL
//
// Called from CIdle to finish time consuming construction operations
// ----------------------------------------------------------------------------
//
TInt CCCAppView::ConstructDelayedCallbackL( TAny* aContainer )
    {
    CCCAppView* container = static_cast<CCCAppView*>( aContainer );
    container->DoConstructDelayedL();    
    return 0;
    }    

// ----------------------------------------------------------------------------
// CCCAppView::DoConstructDelayedL
//
// Perform time consuming construction operations
// ----------------------------------------------------------------------------
//
void CCCAppView::DoConstructDelayedL()
    {
    if( iDelayState == EDelayedCreateTabs )
        {
        iPaneHandler->CreateTabsL();  
        iDelayedConstructor->Start( TCallBack( ConstructDelayedCallbackL, this) );
        iDelayState=EDelayedCheckVisibilities;
        }
    else if( iDelayState == EDelayedCheckVisibilities )
        {
        CheckOtherPluginVisibilitiesL();
        iDelayedConstructor->Start( TCallBack( ConstructDelayedCallbackL, this) );
        }
    else if( iDelayState == EDelayedCheckVisibilitiesOk )
        {
        TInt tabNbr = 0;
        CCCAppPluginData* currPluginData = iPluginLoader->PluginInFocus();    
        if( currPluginData )
            {
            TUid currentlyVisiblePluginId = currPluginData->Plugin().Id();
            iPluginLoader->PluginVisibility( currentlyVisiblePluginId, tabNbr );
            }
        
        iPaneHandler->ActivateTab( tabNbr );
        iDelayedConstructor->Start( TCallBack( ConstructDelayedCallbackL, this) );
        iDelayState=EDelayedFinish;
        }    
    else
        {
        delete iDelayedConstructor;
        iDelayedConstructor = NULL;
        iDelayedStarting = EFalse;        
        SynchPluginVisibilityWithTabAfterDelayedStartL();
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::CheckOtherPluginVisibilitiesL
// ---------------------------------------------------------------------------
//
void CCCAppView::CheckOtherPluginVisibilitiesL()
    {
    TBool pluginVisibility(EFalse);
    
    if(iDelayedPluginVisibilityCheck < iPluginLoader->PluginsCount())
        {
        // prepare plugin parameter 
        CCCAppPluginParameter* pluginParam = CCCAppPluginParameter::NewL( *this );
        CleanupStack::PushL( pluginParam );
        TRAPD(err, pluginVisibility = iPluginLoader->
            PluginAt(iDelayedPluginVisibilityCheck)->Plugin().CheckVisibilityL(*pluginParam));
        CleanupStack::PopAndDestroy( pluginParam );
        if (KErrNone == err)
            {
            if(iPluginLoader->SetPluginVisibility(iPluginLoader->
                PluginAt(iDelayedPluginVisibilityCheck)->Plugin().Id(), pluginVisibility))
                {
                iDelayedVisibilityChanged = ETrue;
                }
            }
        else
            {
            CCA_DP( KCCAppLogFile, CCA_L( 
                "CCCAppStatusPaneHandler::CreateTabsL: Plugin %d visibility check failed."), 
                iDelayedPluginVisibilityCheck);
            //Remove plugin from plugin loader
            TUid pluginUid = iPluginLoader->PluginAt(iDelayedPluginVisibilityCheck)->Plugin().Id();
            CCA_DP( KCCAppLogFile, CCA_L("::visibility check - problem plugin uid: %d"), pluginUid.iUid );
            iPluginLoader->RemovePlugin( pluginUid );
            iDelayedVisibilityChanged = ETrue;
            }        
        }
    else 
        {
        //All checked. Reset if changes requested by plugins
        if(iDelayedVisibilityChanged) 
            {
            iPaneHandler->CreateTabsL();
            }
        iDelayState=EDelayedCheckVisibilitiesOk;
        }
    iDelayedPluginVisibilityCheck++;
    }

// ---------------------------------------------------------------------------
// CCCAppView::StartPluginL
// ---------------------------------------------------------------------------
//
void CCCAppView::StartPluginL(TInt aPluginToFocus)
    {
    /* 
     * Activating plugin is done in two TRAP-levels. The lower is activating
     * the plugin and if it fails, takes care of recovering from the
     * situation. The higher level takes care of exiting the application
     * if something leaves while recovering from the plugin activation.
     */
    CCCAppPluginData* pluginDataInFocus = iPluginLoader->PluginInFocus();  
    User::LeaveIfNull(pluginDataInFocus);
    
    TRAPD( err, ActivatePluginL( *pluginDataInFocus ));
    if ( KErrNone == err )
        {// view started successfully - activate correct tab
        // activate 1st view plugin in case preferred plugin uid not found
        iPaneHandler->ActivateTab( KErrNotFound != aPluginToFocus ? aPluginToFocus : 0  );
        }
    else
        {// problem in plugin activation - try to recover
        CCA_DP( KCCAppLogFile, CCA_L("::StartViewL - problem in plugin activation: err %d"), err );
        RecoverFromBadPluginL();            
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::ChangeView
// ---------------------------------------------------------------------------
//
TInt CCCAppView::ChangeView( TBool aForward )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::ChangeView"));
    
    TInt err = KErrNone;
    
    //Forward the call to statusPane only if it is ready
    if ( iPaneHandler )
        {
        if ( iPaneHandler->IsTabAvailable() )
            {
            TRAP( err, ChangeViewL( aForward ));
            CCA_DP( KCCAppLogFile, CCA_L("::ChangeView - ChangeViewL returned err: %d"), err );
            }
        }   
    
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ChangeView"));
    return err;
    }

// ---------------------------------------------------------------------------
// CCCAppView::ChangeViewL
// ---------------------------------------------------------------------------
//
void CCCAppView::ChangeViewL( TBool aForward )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::ChangeViewL"));

    const TInt visiblePluginCount = iPluginLoader->VisiblePluginCount();  
    if ( KCCAppMinPluginCountForTabs > visiblePluginCount // if only 1 plugin, we can stop handling here
        || iViewChangeInProgress ) // OR view change is in progress
        return;    

    __ASSERT_DEBUG ( iPaneHandler->TabCount() == visiblePluginCount, CCAppPanic( ECCAppPanicObjectCountMismatch ));
    
    iViewChangeInProgress = ETrue;    
    
    /* 
     * If ActivateTabL changes tab succesfully, it will call TabChangedL,
     * which then again will do activation the plugin.
     *  
     * Activating plugin is done in two TRAP-levels. The lower is activating
     * the plugin and if it fails, takes care of recovering from the
     * situation. The higher level takes care of exiting the application
     * if something leaves while recovering from the plugin activation.
     */
    TRAPD( err, iPaneHandler->ActivateTabL( aForward ));
    if ( KErrNone != err)
        {
        CCA_DP( KCCAppLogFile, CCA_L("::ChangeViewL - problem in plugin activation: err %d"), err );
        RecoverFromBadPluginL();            
        }
    
    iViewChangeInProgress = EFalse;    

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ChangeViewL"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::TabChangedL
// ---------------------------------------------------------------------------
//
void CCCAppView::TabChangedL( TInt aIndex )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::TabChangedL"));
    
    CCCAppPluginData* pluginDataInFocus = iPluginLoader->VisiblePlugin( aIndex, ETrue );// not owned
    ActivatePluginL( *pluginDataInFocus );
    
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::TabChangedL"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::ActivatePluginL
// ---------------------------------------------------------------------------
//
void CCCAppView::ActivatePluginL( CCCAppPluginData& aPluginDataInFocus )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::ActivatePluginL"));

    //PERFORMANCE LOGGING: 6. Plugin activation
    WriteToPerfLog();
        
    CCCAppViewPluginBase& plugin = aPluginDataInFocus.Plugin();
    
    if ( !aPluginDataInFocus.IsPluginPrepared() )
        {// 1st activation
        // prepare plugin parameter
        CCCAppPluginParameter* pluginParam = CCCAppPluginParameter::NewL( *this );
        CleanupStack::PushL( pluginParam );
        plugin.PreparePluginViewL( *pluginParam );
        CleanupStack::PopAndDestroy( pluginParam );
        iAppUi.AddViewL( &plugin );// transfers ownership to AppUi -framework
        aPluginDataInFocus.PluginIsPrepared();
        }

    // activate plugin
    iAppUi.ActivateLocalViewL( plugin.Id() );

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ActivatePluginL"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::AppUi
// ---------------------------------------------------------------------------
//
CCCAAppAppUi& CCCAppView::AppUi()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppView::AppUi"));
    return iAppUi;
    }

// ---------------------------------------------------------------------------
// CCCAppView::EikonEnv
// ---------------------------------------------------------------------------
//
CEikonEnv& CCCAppView::EikonEnv()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppView::AlfEnv"));
    return iAppUi.EikonEnv();
    }
    
// ---------------------------------------------------------------------------
// CCCAppView::PluginLoader
// ---------------------------------------------------------------------------
//    
CCCAppPluginLoader& CCCAppView::PluginLoader()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppView::PluginLoader"));
    return *iPluginLoader;
    }
    
// ---------------------------------------------------------------------------
// CCCAppView::ContactFieldDataObserverNotifyL
// ---------------------------------------------------------------------------
//  
void CCCAppView::ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::ContactFieldDataObserverNotifyL"));

    // not interested about other notifications
    if ( MCCAppContactFieldDataObserver::TParameter::EContactDataFieldAvailable 
        == aParameter.iType && NULL != aParameter.iContactField )
        {
        ContactFieldFetchedNotifyL( *aParameter.iContactField );
        }
    else if ( MCCAppContactFieldDataObserver::TParameter::EContactDeleted 
            == aParameter.iType )
        {
        // Quick fix for the urgent need
        //
        // To fix this such manner that CCApplication works as normal Avkon
        // applications do, let the leave with leave code KLeaveExit 
        // go up to CActive's RunL and return it also through RunError.
        // Then normal Avkon AppUi functionality AppUi().Exit() will work.
        AppUi().RunAppShutter();
        }
    
    

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ContactFieldDataObserverNotifyL"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::ContactFieldDataObserverHandleErrorL
// ---------------------------------------------------------------------------
//  
void CCCAppView::ContactFieldDataObserverHandleErrorL( 
    TInt aState, TInt aError )
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppView::ContactFieldDataObserverHandleErrorL"));
    CCA_DP( KCCAppLogFile, CCA_L("::ContactFieldDataObserverHandleErrorL - aState: %d, aError: %d"), aState, aError );
    //todo; what kind of behaviour is wanted in error case?
    // - exit the app if no cms connection/contact is not found?
    }

// ---------------------------------------------------------------------------
// CCCAppView::ContactFieldFetchedNotifyL
// ---------------------------------------------------------------------------
//    
void CCCAppView::ContactFieldFetchedNotifyL( 
        CCmsContactField& aContactField )
    {
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ContactFieldFetchedNotifyL"));

    if ( CCmsContactFieldItem::ECmsFullName == aContactField.Type() )
        {// update titlepane with the name
        CCA_DP( KCCAppLogFile, CCA_L("::ContactFieldFetchedNotifyL - changing title"));
        
        TPtrC newTxt = aContactField.ItemL( 0 ).Data();        
        iAppUi.SetTitleL( newTxt );          
        }    

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ContactFieldFetchedNotifyL"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::RecoverFromBadPluginL
// ---------------------------------------------------------------------------
//
void CCCAppView::RecoverFromBadPluginL()
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::RecoverFromBadPluginL"));

#ifdef _DEBUG
    CAknInformationNote* informationNote = new ( ELeave ) CAknInformationNote;
    informationNote->ExecuteLD( _L("Problem in plugin! Check logs!"));         
#endif

    //Remove plugin from plugin loader
    TUid pluginUid = iPluginLoader->PluginInFocus()->Plugin().Id();
    CCA_DP( KCCAppLogFile, CCA_L("::RecoverFromBadPluginL - problem plugin uid: %d"), pluginUid.iUid );
    iPluginLoader->RemovePlugin( pluginUid );

    if ( iPluginLoader->PluginsCount() )
        {// Plugins available
        CCA_DP( KCCAppLogFile, CCA_L("::RecoverFromBadPluginL - plugins found"));
        
        // Reset tabs
        delete iPaneHandler;
        iPaneHandler = NULL; 
        iPaneHandler = CCCAppStatusPaneHandler::NewL( *this );        
        iPaneHandler->CreateTabsL();

        // Needed to get the focus in most cases to 1st plugin in array
        delete iViewResetter;
        iViewResetter = NULL;
        iViewResetter = CIdle::NewL( CActive::EPriorityStandard );
        iViewResetter->Start( TCallBack( ResetViewL, this ));       
        }
    else
        {// no plugins left
        // (should basically never happen, since there should always be working internal plugins)
        User::Leave( KErrNotFound );
        }
    
    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::RecoverFromBadPluginL"));
    }

// ---------------------------------------------------------------------------
// CCCAppView::CurrentPluginBusy
// ---------------------------------------------------------------------------
//
TBool CCCAppView::CurrentPluginBusy()
    {
    TBool isBusy = EFalse;
    if(iPluginLoader)
        {
        CCCAppPluginData* pluginData = iPluginLoader->PluginInFocus(); //not owned
        if ( pluginData != NULL )
            {
            CCCAppViewPluginBase& plugin = pluginData->Plugin();
            MCCAppViewPluginBase2* pluginExtension =
                reinterpret_cast<MCCAppViewPluginBase2*>(
                    plugin.CCCAppViewPluginBaseExtension( KMCCAppViewPluginBaseExtension2Uid ) );
            if ( pluginExtension != NULL )
                {
                isBusy = pluginExtension->PluginBusy();
                }
            }
        }

    return isBusy;
    }

// ---------------------------------------------------------------------------
// CCCAppView::ResetViewL
// ---------------------------------------------------------------------------
//
TInt CCCAppView::ResetViewL( TAny* aSelf )
    {
    CCA_DP( KCCAppLogFile, CCA_L("->CCCAppView::ResetViewL"));
    
    CCCAppView* view = static_cast<CCCAppView*>(aSelf);//not owned
    
    // Change focus and activate 1st plugin in array
    CCCAppPluginData* pluginDataInFocus =
        view->iPluginLoader->PluginAt( 0, ETrue );
    view->ActivatePluginL( *pluginDataInFocus );
    view->iPaneHandler->ActivateTab( 0 );

    CCA_DP( KCCAppLogFile, CCA_L("<-CCCAppView::ResetViewL"));
    return KErrNone;
    }
// ---------------------------------------------------------------------------
// CCCAppView::ContactRelocationL
// ---------------------------------------------------------------------------
//
void CCCAppView::CCAppContactEventL()
    {    
    TInt count = iObservers.Count();
    for (TInt i = 0; i < count; i++)
        {
        // Inform observers of contact event
        TRAP_IGNORE(iObservers[i]->NotifyPluginOfContactEventL());
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::AddObserverL
// ---------------------------------------------------------------------------
//
void CCCAppView::AddObserverL( MCCAppPluginsContactObserver& aObserver )
    {
    TInt index = iObservers.Find(&aObserver);
    if (index == KErrNotFound)
        {
        iObservers.AppendL(&aObserver);
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::RemoveObserver
// ---------------------------------------------------------------------------
//
void CCCAppView::RemoveObserver( MCCAppPluginsContactObserver& aObserver )
    {
    TInt index = iObservers.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::Parameter
// ---------------------------------------------------------------------------
//
MCCAParameter& CCCAppView::Parameter()
    {
    return iAppUi.Parameter();
    }

// ---------------------------------------------------------------------------
// CCCAppView::ShowPluginL
// ---------------------------------------------------------------------------
//  
void CCCAppView::ShowPluginL( TUid aUid )
    {
    if(iDelayedStarting)
        {        
        iPluginLoader->SetPluginVisibility(aUid, CCCAppPluginData::EPluginVisible);
        iPluginUidToBeSynchd.Append(aUid);
        }
    else
        {                
        PerformShowPluginL( aUid ); 
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::PerformShowPluginL
// ---------------------------------------------------------------------------
//  
void CCCAppView::PerformShowPluginL( TUid aUid )
    {
    TInt currentTab(KErrNotFound);    
    CCCAppPluginData* pluginData = iPluginLoader->PluginInFocus( );
    iPluginLoader->SetPluginVisibility(aUid, CCCAppPluginData::EPluginVisible);
    iPaneHandler->CreateTabsL();    
    
    if( pluginData )
        {
        iPluginLoader->PluginVisibility( pluginData->Plugin().Id(), currentTab );
        iPaneHandler->ActivateTab( currentTab ); 
        }
    else
        {
        iPaneHandler->ActivateTab( 0 );
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::HidePluginL
// ---------------------------------------------------------------------------
//  
void CCCAppView::HidePluginL( TUid aUid )
    {
    if(iDelayedStarting)
        {        
        iPluginLoader->SetPluginVisibility(aUid, CCCAppPluginData::EPluginHidden);
        iPluginUidToBeSynchd.Append(aUid);
        }
    else
        {        
        PerformHidePluginL( aUid ); 
        }
    }

// ---------------------------------------------------------------------------
// CCCAppView::PerformHidePluginL
// ---------------------------------------------------------------------------
//  
void CCCAppView::PerformHidePluginL( TUid aUid )
    {
    TInt newTab(KErrNotFound);    

    //Currently visible plugin
    TInt currentTab(KErrNotFound);
    CCCAppPluginData* currPluginData = iPluginLoader->PluginInFocus( );
    iPluginLoader->PluginVisibility( currPluginData->Plugin().Id(), currentTab );    

    //Hide plugin aUid
    iPluginLoader->SetPluginVisibility(aUid, CCCAppPluginData::EPluginHidden);
    iPaneHandler->CreateTabsL();  
    
    //Set new visible plugin
    if( currPluginData )
        {
        if(currPluginData->Plugin().Id() == aUid)
            {
            //Current will be hidden, bring previous or next to foreground 
            if( CCCAppPluginData* previousPlugin = iPluginLoader->PreviousPlugin( ETrue )) 
                {
                ActivatePluginL( *previousPlugin );
                iPluginLoader->PluginVisibility( previousPlugin->Plugin().Id(), newTab );
                iPaneHandler->ActivateTab( newTab );
                return;
                }
            else if( CCCAppPluginData* nextPlugin = iPluginLoader->NextPlugin( ETrue ))
                {
                ActivatePluginL( *nextPlugin );
                iPluginLoader->PluginVisibility( nextPlugin->Plugin().Id(), newTab );
                iPaneHandler->ActivateTab( newTab );
                return;                
                }
            else
                {
                //Nothing to activate properly
                ResetViewL( this );                
                return;                
                }
            }
        else
            {
            //Retain current plugin in foreground            
            iPluginLoader->PluginVisibility( currPluginData->Plugin().Id(), newTab );
            iPaneHandler->ActivateTab( newTab );
            return;            
            }
        }
        //Nothing to activate properly
        ResetViewL( this ); 
        return;
    }


// ---------------------------------------------------------------------------
// CCCAppView::SynchPluginVisibilityWithTabAfterDelayedStartL
// ---------------------------------------------------------------------------
// 
void CCCAppView::SynchPluginVisibilityWithTabAfterDelayedStartL()
    {
    CCCAppPluginData* currPluginData = iPluginLoader->PluginInFocus();    
    TBool syncDone = EFalse;   
    TInt pluginsRequireSynch = iPluginUidToBeSynchd.Count();
    if( currPluginData && pluginsRequireSynch )
        {
        TUid currentlyVisiblePluginId = currPluginData->Plugin().Id();
        
        for ( TInt index=0; index<pluginsRequireSynch; index++ )
            {        
            if ( currentlyVisiblePluginId == iPluginUidToBeSynchd[index] )
                {
                //is there any change in the visibility of the current visible plugin 
                //the only option could be is that it can become invisible
                //only on the above case we willbe interested
                TInt tabNbr = 0;
                if ( !iPluginLoader->PluginVisibility( currentlyVisiblePluginId, tabNbr ) )
                    {
                    //This plugin is no more visible so its better to call just HidePluginL()
                    syncDone = ETrue;
                    HidePluginL( currentlyVisiblePluginId );                
                    }
                break;
                }
            }
        
        if ( !syncDone )
            {
            //Still synchronisation has not been done
            //Possible reason could be is that, this plugin is not the currently visible one
            //so now it becomes much more easy, so just go ahead and call CreateTabsL
            //only the tab will be reset and the current active plugin will stay active
            TInt tabNbr = 0;
            iPaneHandler->CreateTabsL();  
            //Current the repositioned active tab & activate the same
            iPluginLoader->PluginVisibility( currentlyVisiblePluginId, tabNbr );        
            iPaneHandler->ActivateTab( tabNbr );
            }    
        }
    }

//  End of File  
