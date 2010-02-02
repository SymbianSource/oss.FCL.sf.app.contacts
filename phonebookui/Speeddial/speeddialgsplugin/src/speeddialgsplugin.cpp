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
* Description:     Source file for CSpeeddialGsPlugin class
*
*/





#include <aknViewAppUi.h>
#include <ConeResLoader.h>
#include <barsread.h>       // For TResourceReader
#include <StringLoader.h>
#include <apgcli.h>
#include <AknLaunchAppService.h>
#include <bautils.h>

// Includes from GS framework:
#include <gsfwviewuids.h>
#include <GSPrivatePluginProviderIds.h>

#include <AknNullService.h>
// Plugin includes:
#include <speeddialgspluginrsc.rsg>
#include <speeddialgsplugin.mbg>

// Includes
#include "speeddialgsplugin.h"

#define SPEEDDIALAPPUID 0x1000590A

// CONSTANTS
const TUid KSpeeddialGSPluginUid = { 0x1028339F };

// svg file
_LIT( KGSSecPluginIconFileName, "\\resource\\apps\\speeddialgsplugin.mif");

// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::CSpeeddialGsPlugin()
//
// ---------------------------------------------------------------------------
//
CSpeeddialGsPlugin::CSpeeddialGsPlugin(): iNullService( NULL )
    {
    }

// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::~CSpeeddialGsPlugin()
//
// ---------------------------------------------------------------------------
//
CSpeeddialGsPlugin::~CSpeeddialGsPlugin()
    {
    if (iResId)
	{
        env->DeleteResourceFile(iResId);
	}
    if ( iNullService )
        {
            delete iNullService;
            iNullService=NULL;
        }
    }

// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::ConstructL()
//
// ---------------------------------------------------------------------------
//
void CSpeeddialGsPlugin::ConstructL()
    {
    BaseConstructL( );   
    TFileName resourceFileName;
    TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
    _LIT( KResoureFileName, "speeddialgspluginrsc.rsc" );
          
    resourceFileName.Copy( driveLetter );  
    resourceFileName.Append( KDC_ECOM_RESOURCE_DIR );
    resourceFileName.Append( KResoureFileName );
            
    env = CEikonEnv::Static();

    TFileName fileName(resourceFileName);
	
    BaflUtils::NearestLanguageFile(env->FsSession(), fileName);

    iResId = env->AddResourceFileL(fileName); 
     }

// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::NewL()
//
// ---------------------------------------------------------------------------
//
CGSPluginInterface* CSpeeddialGsPlugin::NewL( TAny* /*aInitParams*/ )
  {
  	CSpeeddialGsPlugin* self = new( ELeave ) CSpeeddialGsPlugin();
  	CleanupStack::PushL( self );
  	self->ConstructL();
  	CleanupStack::Pop( self );
  	return (CGSPluginInterface*)self;
  }

// ---------------------------------------------------------------------------
// CGSParentPlugin::Id()
//
// ---------------------------------------------------------------------------
//
TUid CSpeeddialGsPlugin::Id() const
    {
 	   return KSpeeddialGSPluginUid;
    }

// ---------------------------------------------------------------------------
// CGSParentPlugin::DoActivateL()
//
// ---------------------------------------------------------------------------
//
void CSpeeddialGsPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                     TUid aCustomMessageId,
                                     const TDesC8& aCustomMessage )
    {
    // nothing
    }

// ---------------------------------------------------------------------------
// CGSParentPlugin::DoDeactivate()
//
// ---------------------------------------------------------------------------
//
void CSpeeddialGsPlugin::DoDeactivate()
    {
    // nothing
    }

// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::UpperLevelViewUid()
//
// ---------------------------------------------------------------------------
//
TUid CSpeeddialGsPlugin::UpperLevelViewUid()
    {
    return KGSTelPluginUid;
    }

// -----------------------------------------------------------------------------
// CSpeeddialGsPlugin::GetHelpContext()
//
// -----------------------------------------------------------------------------
//
void CSpeeddialGsPlugin::GetHelpContext( TCoeHelpContext& aContext )
    {
    aContext.iMajor = KUidGS;
    aContext.iContext = NULL;
    }

// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::GetCaptionL()
//
// ---------------------------------------------------------------------------
//
void CSpeeddialGsPlugin::GetCaptionL( TDes& aCaption ) const
    {
    StringLoader::Load( aCaption, R_GS_SPEEDDIAL_VIEW_CAPTION);
    }

// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::PluginProviderCategory()
//
// ---------------------------------------------------------------------------
//
TInt CSpeeddialGsPlugin::PluginProviderCategory() const
    {
   	 return KGSPluginProviderInternal;
    }


// ---------------------------------------------------------------------------
// CSpeeddialGsPlugin::CreateIconL
// Return the icon, if has one.
// ---------------------------------------------------------------------------
//
CGulIcon* CSpeeddialGsPlugin::CreateIconL( const TUid aIconType )
    {
    CGulIcon* icon;

    if( aIconType == KGSIconTypeLbxItem )
        {
        icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(),
        KAknsIIDQgnPropCpTeleSpeed,
        KGSSecPluginIconFileName,
        EMbmSpeeddialgspluginQgn_prop_cp_tele_speed,
        EMbmSpeeddialgspluginQgn_prop_cp_tele_speed_mask );
        }
     else
        {
        icon = CGSPluginInterface::CreateIconL( aIconType );
        }

    return icon;
    }


// -----------------------------------------------------------------------------
// CSpeeddialGsPlugin::ListBoxType()
//
// -----------------------------------------------------------------------------
//
TGSListboxTypes CSpeeddialGsPlugin::ListBoxType()
    {
    return EGSListBoxTypeSettings;
    }
    

// -----------------------------------------------------------------------------
// CSpeeddialGsPlugin::HandleSelection()
// -----------------------------------------------------------------------------
//
void CSpeeddialGsPlugin::HandleSelection(
    const TGSSelectionTypes /*aSelectionType*/ )
{
    TRAP_IGNORE(LaunchEasAppL());
}

// -----------------------------------------------------------------------------
// CSpeeddialGsPlugin::ItemType()
// -----------------------------------------------------------------------------
//
TGSListboxItemTypes CSpeeddialGsPlugin::ItemType()
{
    return EGSItemTypeSettingDialog;
}

// -----------------------------------------------------------------------------
// CEasGSPlugin::LaunchEasAppL()
// -----------------------------------------------------------------------------
//
void CSpeeddialGsPlugin::LaunchEasAppL()
{
    // UID belongs easapp.exe
    const TUid KAppUid = { SPEEDDIALAPPUID };
    
    RWsSession ws;
    User::LeaveIfError( ws.Connect() );
    CleanupClosePushL( ws );

    // Find the task with uid
    TApaTaskList taskList(ws);
    TApaTask task = taskList.FindApp( KAppUid );

    if ( task.Exists() )
    {
        task.BringToForeground();
    }
    else
    {
        // Launch application as embedded.
        if ( iNullService )
        {
            delete iNullService;
            iNullService = NULL;
        }
        iNullService = CAknNullService::NewL( KAppUid, NULL );
    } 
    ws.Close();
    CleanupStack::PopAndDestroy();
}

void CSpeeddialGsPlugin::HandleServerAppExit( TInt aReason )
    {
    if ( aReason != EEikCmdExit )
        {
        MAknServerAppExitObserver::HandleServerAppExit( aReason );
        }
    }
//End of File

