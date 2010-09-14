/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*     Ecom plugin to implement touch support for universal missed calls indicator. 
*
*
*/


// INCLUDE FILES
#include "CLogsIndicatorPlugin.h"

#include <apacmdln.h>  // -> For CApaCommandLine 
#include <apgcli.h>
#include <apgtask.h>

#include <LogsUiCmdStarter.h>

#include <StringLoader.h> 
#include <logs.rsg>
#include <AknUtils.h>
#include <LogsDomainCRKeys.h>

#include <AknPreviewPopUpController.h>
#include <centralrepository.h>   
#include <bautils.h>
#include <pathinfo.h>

#include "LogsDebug.h"


#include <data_caging_path_literals.hrh> // KDC_APP_RESOURCE_DIR


// CONSTANTS

// LogsIndicator.RSS is included in Logs.RSS and compiled to Logs.RSC.
// Needed for the resource strings.
_LIT( KIndicatorResourceFile, "Logs.RSC" );  


// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::NewL
// ---------------------------------------------------------------------------
//
CLogsIndicatorPlugin* CLogsIndicatorPlugin::NewL()
    {
    CLogsIndicatorPlugin* self = new( ELeave ) CLogsIndicatorPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
    
// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CLogsIndicatorPlugin::ConstructL()
    {
    // Access to Central repository is needed to later retrieve
    // the number of missed calls     
    iRepositoryLogs    = CRepository::NewL( KCRUidLogs );
    
    iNotifyHandlerLogs = CCenRepNotifyHandler::NewL
        (*this, *iRepositoryLogs, CCenRepNotifyHandler::EIntKey, KLogsNewMissedCalls);
        
    iNotifyHandlerLogs->StartListeningL();
 
    // Read resource strings    
    CCoeEnv* env = CCoeEnv::Static();
    
    if( !iResourceFileOffset )
        {
        TFileName* resFile = new( ELeave )TFileName( 
            TParsePtrC( PathInfo::RomRootPath() ).Drive() );
        CleanupStack::PushL( resFile );
        resFile->Append( KDC_APP_RESOURCE_DIR );
        resFile->Append( KIndicatorResourceFile );

        BaflUtils::NearestLanguageFile( env->FsSession(), *resFile );
        iResourceFileOffset = env->AddResourceFileL( *resFile );

        CleanupStack::PopAndDestroy( resFile );
        }
   
    iPopupHeading =         env->AllocReadResourceL( R_QTN_LOGS_STYLUS_POPUP_LOGS );
	iPopupTextOne =         env->AllocReadResourceL( R_QTN_LOGS_STYLUS_POPUP_MISSED1 );
	iPopupTextMultiple =    env->AllocReadResourceL( R_QTN_LOGS_STYLUS_POPUP_MISSED );   

	iUid = 0;
    }

// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::CLogsIndicatorPlugin
// ---------------------------------------------------------------------------
//
CLogsIndicatorPlugin::CLogsIndicatorPlugin()
    {
    }
    
// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::~CLogsIndicatorPlugin
// ---------------------------------------------------------------------------
//
CLogsIndicatorPlugin::~CLogsIndicatorPlugin()
    {
    delete iPopupHeading;   
    delete iPopupTextOne;
    delete iPopupTextMultiple;  
    
    // notify cenrep that this observer is no longer here. 
    if (iNotifyHandlerLogs)
        {
        iNotifyHandlerLogs->StopListening();  
        delete iNotifyHandlerLogs; 
        }
    }

// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::HandleIndicatorTapL
// ---------------------------------------------------------------------------
//
void CLogsIndicatorPlugin::HandleIndicatorTapL(const TInt /*aUid*/)
    {
    // Start Logs application Missed calls view
    LogsUiCmdStarter::CmdStartL( LogsUiCmdStarterConsts::KMissedView() );     
    }

// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::TextL
//
// Called by framework first during construction and later after UpdateL
// is called - see CLogsIndicatorPlugin::HandleNotifyInt
// ---------------------------------------------------------------------------
//    
HBufC* CLogsIndicatorPlugin::TextL( const TInt aUid, TInt& aTextType )
    {
    // store the internal uid of this plugin
    iUid = aUid;
    aTextType = EAknIndicatorPluginLinkText;
    
    return CreatePopupContentMessageL();
    }

// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::CreatePopupContentMessageL
// ---------------------------------------------------------------------------
//
HBufC* CLogsIndicatorPlugin::CreatePopupContentMessageL()
    {
    TInt missedCount;
    
    // Get new missed calls count
    iRepositoryLogs->Get( KLogsNewMissedCalls, missedCount );   
      
    // Construct proper popup message (singular or plural)
    HBufC* dynStringBuf = NULL;
    if( missedCount > 1 )
        {
        TInt reqDynLen = iPopupTextMultiple->Length() + KMaxEventNumberLength;
        dynStringBuf = HBufC::NewL( reqDynLen );
        TPtr dynString = dynStringBuf->Des();
        StringLoader::Format( dynString,
                              iPopupTextMultiple->Des(),
                              -1,             //No index code in source string
                              missedCount );
        }
    else // eventCount <= 1
        {
        dynStringBuf = iPopupTextOne->AllocL();
        }
    return dynStringBuf;
    }

// ---------------------------------------------------------------------------
// CLogsIndicatorPlugin::HandleNotifyInt
// ---------------------------------------------------------------------------
//
void CLogsIndicatorPlugin::HandleNotifyInt( TUint32 aId, TInt aNewValue )
    {
    // Sanity check the parameters
    if (aId == KLogsNewMissedCalls && aNewValue >= 0)
        {   
        // Call UpdateL which will eventually lead to a callback to the TextL here and
        // the update of the popup message (missed calls count)        
        TRAPD(err, UpdateL(iUid));
        if (err != KErrNone)
            {
            // nothing to do, the plugin text just won't be updated
            LOGS_DEBUG_PRINT(LOGS_DEBUG_STRING
                ( "CLogsIndicatorPlugin::HandleNotifyInt - UpdateL failed")); 
            }
        }
    }



// end of file
