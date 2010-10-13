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
*       ExtensionManager extension factory.
*
*/


// INCLUDE FILES
#include "CExtensionManagerFactory.h"

// System includes
#include <coemain.h>
#include <coeutils.h>
#include <bautils.h>
#include <aknViewAppUi.h>
#include <aknsconstants.hrh>

// phonebook includes
#include <Pbk2Commands.hrh>
#include <CPbk2UIExtensionInformation.h>
#include <PbkUID.h>

// internal includes
#include "CxSPViewManager.h"
#include "MxSPFactory.h"
#include "CxSPAppUiManager.h"
#include "CxSPLoader.h"
#include "ExtensionManager.hrh"
#include <ExtensionManagerRes.rsg>
#include "CxSPViewIdChanger.h"
#include "ExtensionManagerIcons.hrh"
#include "CxSPViewInfo.h"
#include "ExtensionManagerUID.H"
#include "CxSPBaseView.h"

#include "CxSPSortNamesListViewExtension.h"
#include "CxSPSortView.h"
#include "CxSPContactManager.h"
#include "CxSPCommand.h"

/// Unnamed namespace for local definitions
namespace {

// CONSTANTS
const TInt KExtensionGranularity = 1;
const TInt KFactoryGranularity = 2;

}


// ================= MEMBER FUNCTIONS =======================

inline CExtensionManagerFactory::CExtensionManagerFactory() :
        iFactories( KFactoryGranularity ),
        iExtensions( KExtensionGranularity ),
        iSortView( EFalse )
    {
    }

void CExtensionManagerFactory::ConstructL()
    {
    // Scan for extensions but only for Phonebook2 process. For other processes
    //(at the moment ccaapp.exe and Pbk2ServerApp.exe) we skip reloading 
    //extensions again. This is fix for RCHN-7LAAQB as some of extensions 
    //loaded cause problems when reloaded again in other processes.
    if( RProcess().SecureId().iId == KPbkUID3 )
        {
        ScanExtensionsL(); 
        }

    /// change view ids
    iViewIdChanger = CxSPViewIdChanger::NewL();
    iViewIdChanger->ChangeViewIdsL( iFactories );
    // if ScanExtensionsL does not called,iFactories.Count()==0.
    if (iFactories.Count() > 0)
        {
        iViewIdChanger->WriteTabViewNamesToRepositoryL();
        }
    iViewIdChanger->ChangeManagerViewIdsL();

    CreatePbkViewExtensionL();

    // don't leave if view activator server is already running
    TRAPD( err, iViewActivator = CxSPViewActivator::NewL( iViewIdChanger ) );
    if( err != KErrAlreadyExists )
        {
        User::LeaveIfError( err );
        }
    }

CExtensionManagerFactory* CExtensionManagerFactory::NewL()
    {
    CExtensionManagerFactory* self = new (ELeave) CExtensionManagerFactory;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CExtensionManagerFactory::~CExtensionManagerFactory()
    {
    delete iViewManager;
    delete iViewIdChanger;
    delete iContactManager;
    iFactories.Reset();
    iExtensions.ResetAndDestroy();
    delete iScanner;
    delete iViewActivator;
    }

void CExtensionManagerFactory::AppendL( MxSPFactory* aFactory )
    {
    iFactories.AppendL( aFactory );
    }

void CExtensionManagerFactory::CreatePbkViewExtensionL()
    {
    iViewManager = CxSPViewManager::NewL();
    const TInt count = iFactories.Count();
    for( TInt i = 0; i < count; ++i )
        {
        MxSPView* viewExt = iFactories[i]->CreatePbkViewExtensionL();
        if( viewExt )
            {
            CleanupStack::PushL( viewExt );
            iViewManager->AppendL( viewExt, iFactories[i]->Id() );
            CleanupStack::Pop( viewExt );
            }
        }

    TInt tabViews;
    TInt otherViews;
    iViewIdChanger->GetViewCount( tabViews, otherViews );

    // Create also extension for "sorting" menu, but only if there's more than 1 tab view
    if( tabViews > 1 )
        {
        MxSPView* viewExt = CxSPSortNamesListViewExtension::NewL();
        CleanupStack::PushL(viewExt);
        iViewManager->AppendL(viewExt, KEXTENSIONMANAGERIMPLEMENTATIONUID);
        CleanupStack::Pop(viewExt);
        }
    }

MPbk2UIExtensionView* CExtensionManagerFactory::CreateExtensionViewL
        ( TUid aViewId, CPbk2UIExtensionView& aView )
    {
    if ( aViewId.iUid == EExtensionManagerSortViewId )
        {
        CxSPBaseView* view = CxSPSortView::NewL( *iViewIdChanger,
                                                 *iContactManager,
                                                 aView,
                                                 iExtensions );
        return view;
        }

    const TInt count = iFactories.Count();
    for (TInt i=0; i<count; ++i)
        {
        CxSPBaseView* view = iFactories[i]->CreateViewL( aViewId, aView );
        if( view )
            {
            return view;
            }
        }
    return NULL;
    }


void CExtensionManagerFactory::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane, MPbk2ContactUiControl& /*aControl*/ )
    {
    iViewManager->DynInitMenuPaneL( aResourceId, aMenuPane );
    }


void CExtensionManagerFactory::UpdateStorePropertiesL
        ( CPbk2StorePropertyArray& aPropertyArray )
    {
    const TInt count = iFactories.Count();
    for( TInt i = 0; i < count; i++ )
        {
        iFactories[i]->UpdateStorePropertiesL( aPropertyArray );
        }
    }


MPbk2ContactEditorExtension*
    CExtensionManagerFactory::CreatePbk2ContactEditorExtensionL
        ( CVPbkContactManager& /*aContactManager*/,
          MVPbkStoreContact& /*aContact*/,
          MPbk2ContactEditorControl& /*aEditorControl*/ )
    {
    // Do nothing
    return NULL;
    }


MPbk2ContactUiControlExtension*
    CExtensionManagerFactory::CreatePbk2UiControlExtensionL
        ( CVPbkContactManager& /*aContactManager*/ )
    {
    // Do nothing
    return NULL;
    }


MPbk2SettingsViewExtension*
        CExtensionManagerFactory::CreatePbk2SettingsViewExtensionL
        ( CVPbkContactManager& /*aContactManager */ )
    {
    // Do nothing
    return NULL;
    }

MPbk2AppUiExtension* CExtensionManagerFactory::CreatePbk2AppUiExtensionL
        ( CVPbkContactManager& aContactManager )
    {
    const TInt count = iFactories.Count();
    for( TInt i = 0; i < count; ++i )
        {
        iFactories[i]->SetVPbkContactManager( aContactManager );
        }

   if( !iContactManager )
        {
        iContactManager = CxSPContactManager::NewL( iFactories, aContactManager );
        }
    if( !iAppUiManager )
        {
        iAppUiManager = CxSPAppUiManager::NewL( iFactories, *iViewIdChanger );
        }
    return iAppUiManager;
    }


MPbk2Command* CExtensionManagerFactory::CreatePbk2CommandForIdL
        ( TInt aCommandId, MPbk2ContactUiControl& aUiControl ) const
    {
    // check if this is an extension command
    const TInt count = iFactories.Count();
    TInt err( KErrNotFound );

    TInt dummy;
    // try first the manager itself
    err = iViewManager->GetOldCommand( KEXTENSIONMANAGERIMPLEMENTATIONUID,
                                       aCommandId,
                                       dummy );
    for( TInt i = 0; i < count && err; i++ )
        {
        err = iViewManager->GetOldCommand( iFactories[i]->Id(), aCommandId, dummy );
        }

    MPbk2Command* command = NULL;
    if( !err )
        {
        command = CxSPCommand::NewL( aCommandId, aUiControl, *iViewManager );
        }
    return command;
    }

MPbk2AiwInterestItem* CExtensionManagerFactory::CreatePbk2AiwInterestForIdL
        ( TInt /*aInterestId*/,
          CAiwServiceHandler& /*aServiceHandler*/ ) const
    {
    // Do nothing
    return NULL;
    }

TBool CExtensionManagerFactory::GetHelpContextL
        ( TCoeHelpContext& /*aContext*/,  const CPbk2AppViewBase& /*aView*/,
          MPbk2ContactUiControl& /*aUiControl*/ )
    {
    return EFalse;
    }


void CExtensionManagerFactory::ApplyDynamicViewGraphChangesL(
        CPbk2ViewGraph& aViewGraph )
    {
    if( !iAppUiManager )
        {
        iAppUiManager = CxSPAppUiManager::NewL( iFactories, *iViewIdChanger );
        }

    iAppUiManager->ApplyExtensionViewGraphChangesL( aViewGraph );
    }


void CExtensionManagerFactory::ApplyDynamicPluginInformationDataL(
                CPbk2UIExtensionInformation& aUiExtensionInformation )
    {
    TInt tabViews = 0;
    TInt otherViews = 0;
    iViewIdChanger->GetViewCount( tabViews, otherViews );

    TInt currentId = EExtensionManagerTabViewStartId;
    for( TInt i=0; (i<tabViews) && (currentId <= EExtensionManagerTabViewMaxId); i++)
        {
        // create TPbk2UIExtensionViewId structures
        TPbk2UIExtensionViewId viewId(TUid::Uid(currentId), R_EXTENSION_MANAGER_BASICVIEW );

        // add the view id structures to aUiExtensionInformation
        aUiExtensionInformation.AddViewIdL(viewId);
        currentId++;
        }

    currentId = EExtensionManagerViewStartId;
    for( TInt i=0; (i<otherViews) && (currentId <= EExtensionManagerViewMaxId); i++)
        {
        // create TPbk2UIExtensionViewId structures
        TPbk2UIExtensionViewId viewId(TUid::Uid(currentId), R_EXTENSION_MANAGER_BASICVIEW );

        // add the view id structures to aUiExtensionInformation
        aUiExtensionInformation.AddViewIdL(viewId);
        currentId++;
        }
    }


void CExtensionManagerFactory::ScanExtensionsL()
    {
    iExtensions.ResetAndDestroy();
    iFactories.Reset();

    iScanner = CxSPScanner::NewL( iExtensions );
    iScanner->ScanL();

    const TInt count = iExtensions.Count();
    for (TInt i=0; i<count; ++i)
        {
        AppendL(iExtensions.At(i)->ExtensionFactory());
        }
    }

//  End of File
