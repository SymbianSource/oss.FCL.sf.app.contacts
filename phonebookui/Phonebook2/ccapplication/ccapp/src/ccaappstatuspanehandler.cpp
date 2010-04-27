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
* Description:  Handling status pane.
 *
*/


// INCLUDE FILES
#include "ccappheaders.h"
#include "ccappcommlauncherpluginuids.hrh"
#include "ccappdetailsviewpluginuids.hrh"
#include <aknlayoutscalable_avkon.cdl.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::CCCAppStatusPaneHandler
// ---------------------------------------------------------------------------
//
CCCAppStatusPaneHandler::CCCAppStatusPaneHandler( CCCAppView& aView ):
    iView ( aView )
    {
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::~CCCAppStatusPaneHandler
// ---------------------------------------------------------------------------
//
CCCAppStatusPaneHandler::~CCCAppStatusPaneHandler()
    {
    delete iDecoratedTabGroup; 
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::NewL
// ---------------------------------------------------------------------------
//
CCCAppStatusPaneHandler* CCCAppStatusPaneHandler::NewL( CCCAppView& aView )
    {
    CCCAppStatusPaneHandler* self = new( ELeave ) CCCAppStatusPaneHandler( aView );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::ConstructL()
    {
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::ConstructL"));                  
	} 

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::CreateTabsL
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::CreateTabsL()
    {    
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::CreateTabsL"));    

    for (TInt i = 0; i < iView.PluginLoader().PluginsCount(); i++)
        {
        TRAP_IGNORE(AddBitmapsL(i)); 
        }
    
    CreateNavipaneL();
    
    //Prepare all plugins, incl hidden plugins
    for (TInt i = 0; i < iView.PluginLoader().PluginsCount(); i++)
        {
        AddTabL( i );
        }
    
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::CreateTabsL: Done."));                       
    }


// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::CreateNavipaneL
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::CreateNavipaneL()
    {    
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::CreateNavipaneL"));                  

    //Reset iTabAvailability to EFalse, since tab will be reconstructed
    iTabAvailability = EFalse;
    const TInt pluginCount = iView.PluginLoader().PluginsCount();
    const TInt visiblePluginCount = iView.PluginLoader().VisiblePluginCount();

    CAknNavigationControlContainer* naviPane = 
        static_cast<CAknNavigationControlContainer*>(
            iView.AppUi().EikonEnv().AppUiFactory()->StatusPane()->
                ControlL( TUid::Uid( EEikStatusPaneUidNavi )));//not owned
    
    //Delete tabs
    if( iDecoratedTabGroup )
        {
        naviPane->Pop( iDecoratedTabGroup );        
        delete iDecoratedTabGroup;
        iDecoratedTabGroup = NULL;
        iTabGroupRef = NULL;
        }
    
    //(Re)create tabs if needed    
    if( KCCAppMinPluginCountForTabs <= visiblePluginCount ) 
	    
        {
        iDecoratedTabGroup = naviPane->CreateTabGroupL( this );
        iTabGroupRef = static_cast<CAknTabGroup*>( iDecoratedTabGroup->DecoratedControl() );
        naviPane->PushL( *iDecoratedTabGroup );
        iTabGroupRef->SetTabFixedWidthL( 
            visiblePluginCount > KCCAppMinPluginCountForTabs  
                ? KTabWidthWithThreeTabs : KTabWidthWithTwoTabs );
        iDecoratedTabGroup->ActivateL();
        //Set it to ETrue, since there is atleast 2items in the tab
        //and the tab will be visible 
        iTabAvailability = ETrue;
        }    

    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::CreateNavipaneL: Done."));                       
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::ActivateTab
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::ActivateTab( TInt aTabIndex )
    {
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::ActivateTabL"));                  
    if( iTabGroupRef )
        iTabGroupRef->SetActiveTabByIndex(aTabIndex);    
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::ActivateTabL
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::ActivateTabL( TBool aForward )
    {
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::ActivateTabL"));                  
    if( iTabGroupRef )
        {
        // Since CCApp supports variation of keys for switching the plugins
        // but CAknTabGroup doesn't, some tweaking is needed.
        TKeyEvent keyEvent;
        keyEvent.iCode = aForward ? EKeyRightArrow : EKeyLeftArrow;
        iTabGroupRef->OfferKeyEventL( keyEvent, EEventKey );
        }  
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::ActiveTabIndex
// ---------------------------------------------------------------------------
//
TInt CCCAppStatusPaneHandler::ActiveTabIndex()
    {
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::ActiveTabIndex"));                  
    return iTabGroupRef ? iTabGroupRef->ActiveTabIndex() : KErrNotFound;    
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::TabCount
// ---------------------------------------------------------------------------
//
TInt CCCAppStatusPaneHandler::TabCount()
    {
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::TabCount"));                  
    return iTabGroupRef ? iTabGroupRef->TabCount() : KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::ReadBitmapsL
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::AddBitmapsL( TInt aPlugin )
    {
    CCCAppPluginData* data = iView.PluginLoader().PluginAt( aPlugin );
    //Bitmaps provided

    if( !data->iPluginBitmap )  
        {
        CAknIcon* icon = CAknIcon::NewL();
        CleanupStack::PushL( icon );        
        iView.PluginLoader().PluginAt( aPlugin )->Plugin().ProvideBitmapL(  
                  CCCAppViewPluginBase::ECCAppTabIcon, *icon );
        data->iPluginBitmap = icon->Bitmap();
        data->iPluginBitmapMask = icon->Mask();
                
        //Calculate preferred size for icons   
        TRect mainPane;
        AknLayoutUtils::LayoutMetricsRect(
            AknLayoutUtils::EMainPane, mainPane );
        TAknLayoutRect naviIconRect;
        naviIconRect.LayoutRect(
            mainPane,
            AknLayoutScalable_Avkon::navi_navi_icon_text_pane_g1() );
        TSize size(naviIconRect.Rect().Size());

        AknIconUtils::SetSize( data->iPluginBitmap, size );         
        AknIconUtils::SetSize( data->iPluginBitmapMask, size );

        icon->SetBitmap( NULL );
        icon->SetMask( NULL );
        CleanupStack::PopAndDestroy( icon );        
        }
    
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::AddBitmapsL: Done."));
    }


// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::AddTabL
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::AddTabL( TInt aPlugin )
    {
    CCCAppPluginData* data = iView.PluginLoader().PluginAt( aPlugin );

    if(iTabGroupRef && data->PluginVisibility() == 1) 
        {
        if(data->iPluginBitmap)
            {
            //Tabgroup takes ownership of duplicated bitmap handles
            CFbsBitmap* bmp = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL(bmp);
            CFbsBitmap* bmpMask = new( ELeave ) CFbsBitmap();
            CleanupStack::PushL(bmpMask);
            
            bmp->Duplicate(data->iPluginBitmap->Handle());   
            bmpMask->Duplicate(data->iPluginBitmapMask->Handle());
            iTabGroupRef->AddTabL( aPlugin, bmp, bmpMask );
            CleanupStack::Pop(2); //bmp, bmpmask                
            
            // CCA-Launcher and Details-View icons are GrayScale, but are detected as with color.
            // They shall display dark on clear background and clear on dark background.
            // SetTabMultiColorMode disables color inversion therefore it does not have to be
            // called for those tabs.
            TUid pluginUid = data->Plugin().Id();
            if (pluginUid != TUid::Uid(KCCACommLauncherPluginImplmentationUid) &&
                pluginUid != TUid::Uid(KCCADetailsViewPluginImplmentationUid))
                {
                iTabGroupRef->SetTabMultiColorMode( aPlugin, ETrue );
                }
            }
        else
            {
            iTabGroupRef->AddTabL( aPlugin, KNullDesC() );
            }
        }    
    
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::AddTabL: Done."));
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::TabChangedL
// ---------------------------------------------------------------------------
//
void CCCAppStatusPaneHandler::TabChangedL( TInt aIndex )
    {
    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::TabChangedL"));

    // Tab changed succesfully, forward event to view to activate
    // appropriate plugin
    iView.TabChangedL( aIndex );

    CCA_DP( KCCAppLogFile, CCA_L( "CCCAppStatusPaneHandler::TabChangedL: Done."));
    }

// ---------------------------------------------------------------------------
// CCCAppStatusPaneHandler::IsTabAvailable
// ---------------------------------------------------------------------------
//
TBool CCCAppStatusPaneHandler::IsTabAvailable()
    {
    CCA_DP( KCCAppLogFile, CCA_L("CCCAppStatusPaneHandler::IsTabAvailable %d"), iTabAvailability );   
    return iTabAvailability;
    }

//  End of File  
