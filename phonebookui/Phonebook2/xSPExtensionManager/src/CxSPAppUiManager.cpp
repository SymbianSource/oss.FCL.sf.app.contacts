/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       Composite AppUi extension.
*
*/


// INCLUDE FILES
#include "CxSPAppUiManager.h"

// System includes
#include <coemain.h>
#include <barsread.h>
#include <aknsconstants.hrh>
#include <AknsUtils.h>
#include <aknnavide.h>
#include <akntabgrp.h>

// From Phonebook2
#include <CPbk2ViewGraph.h>
#include <MPbk2StartupMonitor.h>
#include <Pbk2ViewId.hrh>
#include <Pbk2ViewInfo.hrh>
#include <CPbk2IconInfoContainer.h>
#include <CPbk2IconInfo.h>
#include <TPbk2IconId.h>
#include <Pbk2UID.h>
#include <MPbk2ApplicationServices2.h>
#include <MPbk2ApplicationServices.h>
#include <CPbk2ServiceManager.h>
#include <MPbk2AppUi.h>

// internal
#include <ExtensionManagerRes.rsg>
#include "ExtensionManager.hrh"
#include "CxSPViewInfo.h"
#include "CxSPViewIdChanger.h"
#include "MxSPFactory.h"

// ==================== MEMBER FUNCTIONS ====================
CxSPAppUiManager::CxSPAppUiManager( CArrayPtrFlat<MxSPFactory>& aFactories,
                                    CxSPViewIdChanger& aViewIdChanger ) :
    iFactories(aFactories),
    iViewIdChanger(aViewIdChanger)
    {
    }

CxSPAppUiManager::~CxSPAppUiManager()
    {
    }

void CxSPAppUiManager::DoRelease()
    {
    delete this;
    }

void CxSPAppUiManager::ConstructL()
    {      
    iViewIdChanger.GetViewCount( iTabViews, iOtherViews );
    }

CxSPAppUiManager* CxSPAppUiManager::NewL( CArrayPtrFlat<MxSPFactory>& aFactories,
                                          CxSPViewIdChanger& aViewIdChanger )
    {
    CxSPAppUiManager* self = new (ELeave) CxSPAppUiManager( aFactories, aViewIdChanger );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);                                                            
    return self;
    }

void CxSPAppUiManager::ExtensionStartupL(MPbk2StartupMonitor& aStartupMonitor)
    {
    aStartupMonitor.HandleStartupComplete();
    }

void CxSPAppUiManager::ApplyExtensionViewGraphChangesL
        (CPbk2ViewGraph& aViewGraph)
    {
    if( iTabViews < 0 ) //sanity check
        {
        return;
        }

    TInt currentId = EExtensionManagerTabViewStartId;
    TUint currentOrdering = EExtensionManagerTabOrderingStart;
    TResourceReader reader; 
    for( TInt i=0; (i<iTabViews) && (currentId <= EExtensionManagerTabViewMaxId); i++)  
        {
        CCoeEnv::Static()->CreateResourceReaderLC( reader, 
                                               R_EXTENSION_MANAGER_BASIC_VIEWGRAPH );
            
        CPbk2ViewNode* node = CPbk2ViewNode::NewL(
                TUid::Uid(currentId), 
                TUid::Uid(EPbk2ExitViewId), 
                EFalse, 
                EPbk2MainTabGroup, 
                (TPbk2TabGroupOrdering)currentOrdering,
                0,  // No tabs
                reader);
        
        CleanupStack::PushL( node );
        aViewGraph.AddViewNodeL( node );
        CleanupStack::Pop( node );
        currentId++;
        currentOrdering++;
        CleanupStack::PopAndDestroy(); //reader     
        }
        
    currentId = EExtensionManagerViewStartId;
    for( TInt i=0; (i<iOtherViews) && (currentId <= EExtensionManagerViewMaxId); i++)  
        {
        CCoeEnv::Static()->CreateResourceReaderLC( reader, 
                                               R_EXTENSION_MANAGER_BASIC_VIEWGRAPH );
            
        CPbk2ViewNode* node = CPbk2ViewNode::NewL(
                TUid::Uid(currentId), 
                TUid::Uid(EPbk2ExitViewId), 
                EFalse, 
                (TPbk2TabGroupId)0, 
                (TPbk2TabGroupOrdering)0,
                0,
                reader);
        
        CleanupStack::PushL( node );
        aViewGraph.AddViewNodeL( node );
        currentId++;
        CleanupStack::Pop( node );
        CleanupStack::PopAndDestroy(); //reader 
        }
    }



void CxSPAppUiManager::ApplyDynamicPluginTabGroupContainerChangesL(
        const CPbk2ViewNode& aNode, 
        CAknTabGroup& aAknTabGroup,
        CPbk2IconInfoContainer& aTabIcons,
        TInt /*aViewCount*/ )
    {

    // First, load all view infos that contain the icons
    RPointerArray<CxSPViewInfo> infoArray;
    CleanupClosePushL( infoArray );
    const TInt count = iFactories.Count();
    for( TInt i = 0; i < count; i++ )
        {     
        iViewIdChanger.GetTabViewInfoL( iFactories[i]->Id(), infoArray );
        }
    
    // pre-set default skin & color values
    TAknsItemID skin;
    skin.Set( EAknsMajorGeneric, EAknsMinorQgnGrafTab22 );
    TAknsItemID color;
    color.Set( EAknsMajorNone, EAknsMinorNone );
    
    // is the aNode from this extension
    // loop through tab view nodes
    for( TInt currentId = EExtensionManagerTabViewStartId; 
         currentId <= EExtensionManagerTabViewMaxId; currentId++ )
        {
        if (aNode.ViewId().iUid == currentId)
            {           
            for( TInt i = 0; i < infoArray.Count(); i++ )
                {
                CxSPViewInfo* info = infoArray[i];
                if( info->NewViewId() == currentId )
                    {
                    TBool isMonochrome = ETrue;
                    CPbk2IconInfo* iconInfo (NULL);
                    CFbsBitmap* bitmap = NULL;
                    CFbsBitmap* mask = NULL;
                    TPbk2IconId iconInfoId( TUid::Uid(KPbk2UID3), currentId );
                    //info->Name() denotes the ServiceName of the Service in SpSettings                    
                    //PBK2 ServiceManager has the brandicons of all the services installed
                    //to the device.
                    //So iterate thro the PBK2ServiceManager, to find the service which matches
                    //the info->Name()
                    //If matches use this icon else use the default icon for the tabs
                    //get the XSP ServiceName 
                            
                    // CPbk2ServiceManager stores all the brandinfo
                    // related to the services configured to the phone
                    // use this to show uniform icon throughout PhoneBook
                    MPbk2ApplicationServices2* servicesExtension = 
                        reinterpret_cast<MPbk2ApplicationServices2*>
                            ( Phonebook2::Pbk2AppUi()->ApplicationServices().
                                MPbk2ApplicationServicesExtension(
                                    KMPbk2ApplicationServicesExtension2Uid ) );
                    CPbk2ServiceManager& servMan = servicesExtension->ServiceManager();
                    const CPbk2ServiceManager::RServicesArray& services = servMan.Services();    
                    for ( TInt i = 0; i < services.Count(); i++ )
                        {
                        const CPbk2ServiceManager::TService& service = services[i];
                        //Found the appropriate service info
                        if ( service.iName == info->Name() )
                            {
                            if ( service.iBitmap && service.iMask )
                                {
                                
                                // Check if the bitmap is monochrome here
								// before compression happen  
								if ( !service.iBitmap->IsMonochrome())
                                	{
                                	isMonochrome = EFalse;
                                	}
									
                                TSize size( 25, 25 ); //default size used in PB
                                
                                AknIconUtils::SetSize(
                                        service.iBitmap,
                                        size );
                                AknIconUtils::SetSize(
                                        service.iMask,
                                        size );                               
                                
                                bitmap = new (ELeave) CFbsBitmap;
                                CleanupStack::PushL( bitmap );
                                bitmap->Duplicate( service.iBitmap->Handle() );
                                
                                mask = new (ELeave) CFbsBitmap;
                                CleanupStack::PushL( mask );
                                mask->Duplicate( service.iMask->Handle() );
                                
                                iconInfo = CPbk2IconInfo::NewLC(
                                        iconInfoId,
                                        bitmap,
                                        mask,                                                     
                                        size );
                                }
                            break;
                            }
                        }            
                    
                    if ( !iconInfo )
                        {
                        //load default icons using mif file
                        //Branded icon was not found
                        iconInfo = CPbk2IconInfo::NewLC(
                            iconInfoId,
                            (TInt)info->TabIconId(),
                            (TInt)info->TabMaskId(),
                            info->TabIconFile(), 
                            skin, color, 0, 0 );
                        
                        AknsUtils::CreateIconLC( AknsUtils::SkinInstance(),  
                                                 skin, 
                                                 bitmap, 
                                                 mask,
                                                 info->TabIconFile(), 
                                                 info->TabIconId(), 
                                                 info->TabMaskId() );
                                                 
                        AknIconUtils::ExcludeFromCache( bitmap ); 
                        AknIconUtils::ExcludeFromCache( mask );    
                        }
                    
                    aTabIcons.AppendIconL( iconInfo );
                    aAknTabGroup.AddTabL( currentId, bitmap, mask );
                    
                    // Set color mode only for colored icon, 
					// not for monochrome icon
                    if ( !isMonochrome )
                    	{
                    	aAknTabGroup.SetTabMultiColorMode( currentId, ETrue );
                    	}

					//The POP is synchroze the number of items pushed
					//the order of the item might change. Please have a glance on the items pushed
                    CleanupStack::Pop( 3 ); // iconInfo,bitmap,mask
                    break;
                    }
                }
            }
        }
    CleanupStack::PopAndDestroy(); // infoArray 
    }

// End of File
