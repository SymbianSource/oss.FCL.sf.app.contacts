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
* Description:  Phonebook 2 UI extension view.
*
*/


#include "CPbk2UIExtensionView.h"

// Phonebook 2
#include "CPbk2UIExtensionLoader.h"
#include "CPbk2UIExtensionInformation.h"
#include <CPbk2UIExtensionManager.h>
#include <CPbk2UIExtensionPlugin.h>
#include <MPbk2UIExtensionView.h>
#include <MPbk2ViewExplorer.h>
#include <Pbk2PluginCommandListerner.h>
#include <mpbk2uiextensionview2.h>
// System includes
#include <AknUtils.h>

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::CPbk2UIExtensionView
// --------------------------------------------------------------------------
//
CPbk2UIExtensionView::CPbk2UIExtensionView( 
        TUid aViewId, 
        CPbk2UIExtensionLoader& aExtensionLoader ) :
    iViewId( aViewId ), 
    iExtensionLoader( aExtensionLoader )
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::~CPbk2UIExtensionView
// --------------------------------------------------------------------------
//
CPbk2UIExtensionView::~CPbk2UIExtensionView()
    {
    delete iView;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::NewL
// --------------------------------------------------------------------------
//
CPbk2UIExtensionView* CPbk2UIExtensionView::NewL( 
        TUid aViewId, 
        CPbk2UIExtensionLoader& aExtensionLoader )
    {
    CPbk2UIExtensionView* self = new ( ELeave ) CPbk2UIExtensionView( 
            aViewId, aExtensionLoader );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2UIExtensionView::ConstructL()
    {
    TInt resourceId = 0;

    // get the plugin information from the extension loader
    TArray<CPbk2UIExtensionInformation*> pluginInfo =
            iExtensionLoader.PluginInformation();

    // find the plugin that implements this view
    const TInt count = pluginInfo.Count();
    for ( TInt i = 0; i < count && resourceId == 0; ++i )
        {
        if ( pluginInfo[i]->ImplementsView( iViewId ) )
            {
            TArray<TPbk2UIExtensionViewId> viewIds =
                    pluginInfo[i]->ViewIds();
            // find the view in the plugin that implements this view
            const TInt viewIdCount = viewIds.Count();
            for ( TInt viewId = 0; viewId < viewIdCount; ++viewId )
                {
                if ( viewIds[viewId].ViewId() == iViewId )
                    {
                    // then get the resource id of that view
                    resourceId = viewIds[viewId].ViewResourceId();
                    break;
                    }
                }
            }
        }
    BaseConstructL( resourceId );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::Id
// --------------------------------------------------------------------------
//
TUid CPbk2UIExtensionView::Id() const
    {
    return iViewId;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::ViewStateLC
// --------------------------------------------------------------------------
//
CPbk2ViewState* CPbk2UIExtensionView::ViewStateLC() const
    {
    LoadViewL();
    return iView->ViewStateLC();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::HandleCommandKeyL
// --------------------------------------------------------------------------
//
TBool CPbk2UIExtensionView::HandleCommandKeyL
        ( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    LoadViewL();
    return iView->HandleCommandKeyL( aKeyEvent, aType );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::GetViewSpecificMenuFilteringFlagsL
// --------------------------------------------------------------------------
//
TInt CPbk2UIExtensionView::GetViewSpecificMenuFilteringFlagsL() const
    {
    LoadViewL();
    return iView->GetViewSpecificMenuFilteringFlagsL();
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::HandlePointerEventL
        ( const TPointerEvent& aPointerEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        LoadViewL();
        iView->HandlePointerEventL( aPointerEvent );
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::HandleCommandL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::HandleCommandL( TInt aCommand )
    {
    LoadViewL();
    
    const TArray<CPbk2UIExtensionPlugin*> plugin = iExtensionLoader.Plugins();
    const TInt count = plugin.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iExtensionLoader.IsLoaded( plugin[i]->ImplementationUid() ))
            {
            // Pass the command first to the plugins command listener
            MPbk2PluginCommandListerner* cmdListener = 
                reinterpret_cast<MPbk2PluginCommandListerner*>
                ( plugin[i]->UIExtensionPluginExtension( plugin[i]->ImplementationUid() ));
                            
                if ( cmdListener != NULL )
                    {
                    cmdListener->HandlePbk2Command( aCommand );
                    }
            }
        }
    iView->HandleCommandL( aCommand );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::HandleStatusPaneSizeChange
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::HandleStatusPaneSizeChange()
    {
    if ( iView )
        {
        iView->HandleStatusPaneSizeChange();
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::DoActivateL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::DoActivateL
        ( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
          const TDesC8& aCustomMessage)
    {
    LoadViewL();
    iView->DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
   
    /* When phonebook quits, the custom control value will be set to 1 
     * which means the bring-to-foreground is forbidden for phonebook. 
     * The custom control value will be set back to 0 here so that besides
     * name list view, other views such contact info view can also be 
     * brought to foreground by ActivateView(ViewID) from other Applications
     **/
    if( Id() == Phonebook2::KPbk2NamesListViewUid )
    	{
		CEikonEnv::Static()->EikAppUi()->SetCustomControl( 0 );
    	}
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::DoDeactivate
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::DoDeactivate()
    {
    if ( iView )
        {
        iView->DoDeactivate();
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::DynInitMenuPaneL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::DynInitMenuPaneL
        ( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    LoadViewL();
    iView->DynInitMenuPaneL( aResourceId, aMenuPane );
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::LoadViewL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::LoadViewL() const
    {
    if ( !iView )
        {
        TUid pluginUid = PluginForViewId( iViewId );
        iExtensionLoader.EnsureLoadedL( pluginUid );
        CPbk2UIExtensionPlugin* plugin =
            iExtensionLoader.LoadedPlugin( pluginUid );
        if(plugin)  
            {       
            iView = plugin->CreateExtensionViewL(
                    iViewId, const_cast<CPbk2UIExtensionView&>( *this ) );
            }       
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::PluginForViewId
// --------------------------------------------------------------------------
//
TUid CPbk2UIExtensionView::PluginForViewId( TUid aViewId ) const
    {
    TUid result = KNullUid;
    TArray<CPbk2UIExtensionInformation*> pluginInfo =
        iExtensionLoader.PluginInformation();
    const TInt count = pluginInfo.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( pluginInfo[i]->ImplementsView( aViewId ) )
            {
            result = pluginInfo[i]->ImplementationUid();
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::HandlePointerEventL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::HandleLongTapEventL(
        const TPoint& aPenEventLocation, 
        const TPoint& aPenEventScreenLocation )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        LoadViewL();
        iView->HandleLongTapEventL(
            aPenEventLocation, aPenEventScreenLocation );
        }
    }

// --------------------------------------------------------------------------
// CPbk2UIExtensionView::HandleForegroundEventL
// --------------------------------------------------------------------------
//
void CPbk2UIExtensionView::HandleForegroundEventL(TBool aForeground)
    {      
    if ( iView )
        {        
        MPbk2UIExtensionView2* view =
                    reinterpret_cast<MPbk2UIExtensionView2*>
                        (iView->UIExtensionViewExtension
                            (KMPbk2UIExtensionView2Uid ));
        if ( view )
            {
            view->HandleForegroundEventL( aForeground );
            }
        }
    }
            

// End of File
