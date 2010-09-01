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
*     Implements interface for Logs event
*
*/


// INCLUDE FILES
#include <featmgr.h>
#include <charconv.h>

#include "CLogsEngine.h"
#include "CLogsModelFactory.h"
#include "CLogsClearLogFactory.h"
#include "CLogsReaderFactory.h"
#include "CLogsConfigFactory.h"
#include "CLogsGetEventFactory.h"
#include "CLogsSharedDataFactory.h"
#include "CLogsSystemAgentFactory.h"
#include "CLogsEventUpdater.h"
#include "CLogsSMSEventUpdater.h"
#include "CLogsCntLinkChecker.h"

#include "MLogsClearLog.h"
#include "MLogsStateHolder.h"
#include "MLogsReader.h"
#include "MLogsConfig.h"
#include "MLogsGetEvent.h"
#include "MLogsSharedData.h"
#include "MLogsSystemAgent.h"


// #include <TelephonyInternalPSKeys.h> 

// CONSTANTS

// ----------------------------------------------------------------------------
// CLogsEngine::NewL
// ----------------------------------------------------------------------------
//
EXPORT_C CLogsEngine* CLogsEngine::NewL()
    {
    CLogsEngine* self = new (ELeave) CLogsEngine();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::CLogsEngine
// ----------------------------------------------------------------------------
//
CLogsEngine::CLogsEngine() 
    {
    }

// ----------------------------------------------------------------------------
// CLogsEngine::
// ----------------------------------------------------------------------------
//
EXPORT_C CLogsEngine::~CLogsEngine()
    {
    delete iMainModel;
    delete iReceivedModel;
    delete iDialledModel;
    delete iMissedModel;
    delete iClearLogs;
    delete iSMSUpdater;
    delete iEventUpdater;
    delete iCntLinkChecker;
    delete iConfig;
    delete iGetEvent;    
    delete iLogClient;    
    delete iSharedData;
    delete iSystemAgent;
    delete iConverter;
    delete iClearNewMissed;

    iFsSession.Close();
    }


// ----------------------------------------------------------------------------
// CLogsEngine::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsEngine::ConstructL( )
    {
    User::LeaveIfError( iFsSession.Connect() );
    iLogClient = CLogClient::NewL( iFsSession );

    //Texts in LOGWRAP.RLS / LOGWRAP.RSS        
    User::LeaveIfError( iLogClient->GetString( iStrings.iInDirection, R_LOG_DIR_IN ) );
    User::LeaveIfError( iLogClient->GetString( iStrings.iOutDirection, R_LOG_DIR_OUT ) );
    User::LeaveIfError( iLogClient->GetString( iStrings.iMissedDirection, R_LOG_DIR_MISSED ) );
	User::LeaveIfError( iLogClient->GetString( iStrings.iUnKnownNumber, R_LOG_REMOTE_UNKNOWN ) ); //"Unknown" (Logwrap.rls)
    User::LeaveIfError( iLogClient->GetString( iStrings.iInDirectionAlt, R_LOG_DIR_IN_ALT ) );    //"Incoming on alternate line"
    User::LeaveIfError( iLogClient->GetString( iStrings.iOutDirectionAlt, R_LOG_DIR_OUT_ALT ) );
    User::LeaveIfError( iLogClient->GetString( iStrings.iFetched, R_LOG_DIR_FETCHED) );           //"Fetched"

    iSharedData = CLogsSharedDataFactory::SharedDataL();

    //Construct Unicode converter to convert 8-bit strings to 16-bit strings and check to see if the 
    //character set is supported - if not then leave.
    iConverter = CCnvCharacterSetConverter::NewL();
    if (iConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierIso88591, iFsSession ) 
        != CCnvCharacterSetConverter::EAvailable)
        {
        User::Leave(KErrNotSupported);
        }

    //Contruct system agent immediately
    SystemAgentL();
    }


// ----------------------------------------------------------------------------
// CLogsEngine::Model
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsModel* CLogsEngine::Model(TLogsModel aModel) const
    {
    switch( aModel )
        {
        case ELogsMainModel:
            return iMainModel;
            //break;
        case ELogsReceivedModel:
            return iReceivedModel;
            //break;

        case ELogsDialledModel:
            return iDialledModel;
            //break;

        case ELogsMissedModel:
            return iMissedModel;
            //break;

        default:
            return NULL;
            //break;
        }
    }

// ----------------------------------------------------------------------------
// CLogsEngine::ClearLogsL
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsClearLog* CLogsEngine::ClearLogsL()
    {
    if( ! iClearLogs )
        {
        iClearLogs = CLogsClearLogFactory::LogsClearLogL( iFsSession, this ); 
        }
        
    return iClearLogs;
    }


// ----------------------------------------------------------------------------
// CLogsEngine::ClearNewMissedL
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsClearNewMissed* CLogsEngine::ClearNewMissedL()
    {
    if( !iClearNewMissed )
        {
        iClearNewMissed = CLogsClearNewMissed::NewL( iFsSession ); 
        }
        
    return iClearNewMissed;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::DeleteClearNewMissedL
// ----------------------------------------------------------------------------
//
void CLogsEngine::DeleteClearNewMissedL()
    {
    delete iClearNewMissed;
    iClearNewMissed = NULL;
    }


// ----------------------------------------------------------------------------
// CLogsEngine::EventUpdaterL
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsReader* CLogsEngine::EventUpdaterL()
    {
    if( ! iEventUpdater )
        {
        iEventUpdater = CLogsEventUpdater::NewL( 
                    iFsSession,        
                    this,                                                                
                    CVPbkPhoneNumberMatchStrategy::EVPbkExactMatchFlag  );                                                       
        }
    return iEventUpdater;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::CntLinkCheckerL
// ----------------------------------------------------------------------------
//
EXPORT_C CLogsCntLinkChecker* CLogsEngine::CntLinkCheckerL()
    {
    if( ! iCntLinkChecker )
        {
        iCntLinkChecker = CLogsCntLinkChecker::NewL( 
                    iFsSession,        
                    NULL );                                                       
        }
    return iCntLinkChecker;
    }
// ----------------------------------------------------------------------------
// CLogsEngine::ConfigL
//
// For Log db settings manipulation
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsConfig* CLogsEngine::ConfigL()
    {
    if( ! iConfig )
        {
        iConfig = CLogsConfigFactory::LogsConfigL( iFsSession ); 
        }
    return iConfig;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::GetEventL
//
// Reads full Event data for Logs detail view
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsGetEvent* CLogsEngine::GetEventL()
    {
    if( ! iGetEvent )
        {
        iGetEvent = CLogsGetEventFactory::LogsGetEventL( iFsSession, 
                        iStrings );
        }
    return iGetEvent;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::SharedDataL
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsSharedData* CLogsEngine::SharedDataL()
    {
	if ( !iSharedData )
		{
		iSharedData = CLogsSharedDataFactory::SharedDataL();
		}
    return iSharedData;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::SystemAgentL
// ----------------------------------------------------------------------------
//
EXPORT_C MLogsSystemAgent* CLogsEngine::SystemAgentL()
    {
    if( ! iSystemAgent )
        {
        iSystemAgent = CLogsSystemAgentFactory::LogsSystemAgentL( NULL, this );
        }
    return iSystemAgent;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::DeleteGetEvent
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::DeleteGetEvent()
    {
    delete iGetEvent;
    iGetEvent = NULL;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::DeleteEventUpdater
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::DeleteEventUpdater()
    {
    delete iEventUpdater;
    iEventUpdater = NULL;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::DeleteCntLinkChecker
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::DeleteCntLinkChecker()
    {
    delete iCntLinkChecker;
    iCntLinkChecker = NULL;
    }
// ----------------------------------------------------------------------------
// CLogsEngine::DeleteSMSEventUpdater
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::DeleteSMSEventUpdater()
    {
    delete iSMSUpdater;
    iSMSUpdater = NULL;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::DeleteSystemAgent
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::DeleteSystemAgent()
    {
    delete iSystemAgent;
    iSystemAgent = NULL;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::DeleteConfig
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::DeleteConfig()
    {
    delete iConfig;
    iConfig = NULL;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::DeleteClearLog
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::DeleteClearLog()
    {
    delete iClearLogs;
    iClearLogs = NULL;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::CreateModelL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::CreateModelL( TLogsModel aModel )
    {
    switch( aModel )
        {
        case ELogsMainModel:
            if( ! iMainModel )
                {
                iMainModel = CLogsModelFactory::ModelL( iFsSession, 
                                        ELogsMainModel, iStrings, this );
                }
            break;

        case ELogsReceivedModel:
            if( ! iReceivedModel )
                {
                iReceivedModel = CLogsModelFactory::ModelL( iFsSession, 
                                        ELogsReceivedModel, iStrings, this  );
                }
            break;

        case ELogsDialledModel:
        if( ! iDialledModel )
                {
                iDialledModel = CLogsModelFactory::ModelL( iFsSession, 
                                        ELogsDialledModel, iStrings, this  );
                }
            break;

        case ELogsMissedModel:
            if( ! iMissedModel )
                {
                iMissedModel = CLogsModelFactory::ModelL( iFsSession, 
                                        ELogsMissedModel, iStrings, this  );
                }
            break;

        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CLogsEngine::StartSMSEventUpdaterL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::StartSMSEventUpdaterL(  )
    {
    if( ! iSMSUpdater )
        {
		TInt err = KErrNone;

		TRAP( err,	
              iSMSUpdater = CLogsSMSEventUpdater::NewL( 
                      iFsSession, 
                      this, //MLogsObserver
                      CVPbkPhoneNumberMatchStrategy::EVPbkStopOnFirstMatchFlag );
		              // fore SMS event updater we use the same match flag that messaging uses
                                                                                 
			  iSMSUpdater->StartL()
			);

		// If there is an error, we can't use CLogsSMSEventUpdater
		// delete and return
		// As the CLogsEngine::StartSMSEventUpdaterL is used in DoActivateL, it's better not to leave, but return
		// since view won't be activated and it will mess up viewstack
		if( err )
			{
			delete iSMSUpdater;
			iSMSUpdater = NULL;

			return;
			}
		User::LeaveIfError( err ); 
		}
    }

// ----------------------------------------------------------------------------
// CLogsEngine::StateChangedL
// ----------------------------------------------------------------------------
//
void CLogsEngine::StateChangedL( MLogsStateHolder* aHolder )
    {    
    if( aHolder->State() == EStateClearLogFinished ||
        aHolder->State() == EStateEventUpdaterFinished     )//We have finished deleting/updating an entry in LogDb
        {                                                   //and have to now reread the corresponding view data
                                                            //from db. See also CLogsBaseReader::HandleLogViewChangeEventDeletedL
        
        // If event updater finished, don't reset the array. 
        // Avoids unnecessary flicker of "Retrieving data" text 
        // since the old list is anyway shown before event reading starts.
        TBool resetArray(ETrue);        
        if ( aHolder->State() == EStateEventUpdaterFinished )
            {
            resetArray = EFalse;
            }
        
        if( iReceivedModel )
            {
            iReceivedModel->KickStartL( resetArray );//Does nothing if model is deactivated state
            }
        if( iDialledModel )
            {
            iDialledModel->KickStartL( resetArray ); //Does nothing if model is deactivated state
            }
        if( iMissedModel )
            {
            iMissedModel->KickStartL( resetArray );  //Does nothing if model is deactivated state
            }
        }

    //Delete unneeded objects when finished
    if( aHolder->State() == EStateEventUpdaterFinished )
        {
        //FIXME: We cannot do this here because VPbk processing maybe ongoing, this is only
        //ok when all the asyncronous opening processes for Vpbk have completed successfully
        //(see CLogsBaseUpdater::BaseConstructL)
        //DeleteEventUpdater();  
        }

    if( aHolder->State() == EStateClearLogFinished )
        {
        DeleteClearLog(); 
        }
    }

/********************************************
EXPORT_C void CLogsEngine::CreateSharedDataL()
	{
	if ( !iSharedData )
		{
		iSharedData = CLogsSharedDataFactory::SharedDataL();
		}
	}
*********************************************/	

// ----------------------------------------------------------------------------
// CLogsEngine::FreeResourcesForBGModeL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLogsEngine::FreeResourcesForBGModeL()
	{
	// Read heap-related statistics for debug use
//	RHeap heap = User::Heap();
//	TInt size1 = heap.Size();
//	TInt bb1;
//	TInt avail1 = heap.Available( bb1 );
//	TInt frees1;
//	TInt count1 = heap.Count( frees1 );

	DeleteClearLog();
	DeleteSMSEventUpdater();
	DeleteEventUpdater();
	DeleteConfig();             //Logs settings 
	DeleteGetEvent();
    DeleteClearNewMissedL();
	User::LeaveIfError( User::CompressAllHeaps() );
	
	// Read heap-related statistics for debug use
//	TInt size2 = heap.Size();
//	TInt bb2;
//	TInt avail2 = heap.Available( bb2 );
//	TInt frees2;
//	TInt count2 = heap.Count( frees2 );
	}


// ----------------------------------------------------------------------------
// CLogsEngine::ConvertToUnicode
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CLogsEngine::ConvertToUnicode(
    const TDesC8& aForeignText,
    TDes16& aConvertedText )
    {       
    TInt ret = KErrNone;
    TInt maxConvertLength = aForeignText.Length();        
    aConvertedText.SetLength(0);    // reset the output buffer

    //If aConvertedText is too small, truncate input
    if( aForeignText.Length() > aConvertedText.MaxLength() )
        {
        maxConvertLength = aConvertedText.MaxLength();        
        ret = KErrOverflow;
        }

    TBuf16<20> outputBuffer;        // Create a small output buffer 

    // Initialise a pointer for the unconverted text
    TPtrC8 remainderOfForeignText( aForeignText.Left( maxConvertLength ) ); 

    // Create a "state" variable and initialise it with CCnvCharacterSetConverter::KStateDefault
    // After initialisation the state variable must not be tampered with.
    // Simply pass into each subsequent call of ConvertToUnicode()
    TInt state=CCnvCharacterSetConverter::KStateDefault;

    for(;;) // conversion loop
        {
        // Start conversion. When the output buffer is full, return the number
        // of characters that were not converted
        const TInt returnValue=iConverter->ConvertToUnicode(outputBuffer, remainderOfForeignText, state);

        // check to see that the descriptor isn’t corrupt - leave if it is
        if (returnValue==CCnvCharacterSetConverter::EErrorIllFormedInput)
            {
            return KErrCorrupt;//User::Leave(KErrCorrupt);
            }
        else if (returnValue<0) // future-proof against "TError" expanding
            {
            return KErrGeneral; //User::Leave(KErrGeneral);
            }
        
        aConvertedText.Append(outputBuffer);    // Store the contents of the output buffer. There should be 
                                                // enough space as we've already checked Length() of buffer.

        // Finish conversion if there are no unconverted characters in the remainder buffer
        if (returnValue==0)
            {
            break;
            }

        // Remove converted source text from the remainder buffer.
        // The remainder buffer is then fed back into loop
        remainderOfForeignText.Set(remainderOfForeignText.Right(returnValue));
        }
        
    return ret;
    }

// ----------------------------------------------------------------------------
// CLogsEngine::LogDbStrings
//
// Returns identification strings used by Log database
// ----------------------------------------------------------------------------
//
EXPORT_C TLogsEventStrings CLogsEngine::LogDbStrings()
	{
	return iStrings;
	}


// ----------------------------------------------------------------------------
// CLogsEngine::LogDbStrings
//
// Returns identification strings used by Log database
// ----------------------------------------------------------------------------
//
EXPORT_C CLogClient* CLogsEngine::CLogClientRef()
	{
	return iLogClient;
	}

// ----------------------------------------------------------------------------
// CLogsEngine::CallStateChangedL
// ----------------------------------------------------------------------------
//
void CLogsEngine::CallStateChangedL( TInt /* aCallState */ )
    {
/****if telephone status changes need to be observed, here can be added code to call the iCallObserver->CallStateChangedL
     (add call code to CLogsSystemAgent too)

    switch( aCallState )      //see TelephonyInternalPSKeys.h
        {
        case EPSTelephonyCallStateAlerting:
        case EPSTelephonyCallStateRinging:
            {
            //We must immediately stop any db activities as otherwise there is risk that ring tune loading
            //fails because of too much I/O happening in phone (EMSH-6JDFBV)
            if( iMissedModel )
                {
                iMissedModel->DoDeactivate( MLogsModel::ESkipClearing, ETrue  ); 
                }
            if( iReceivedModel )
                {
                iReceivedModel->DoDeactivate( MLogsModel::ENormalOperation, ETrue );
                }
            if( iDialledModel )
                {
                iDialledModel->DoDeactivate( MLogsModel::ENormalOperation, ETrue  );
                }
            if( iMainModel )
                {
                iMainModel->DoDeactivate( MLogsModel::ENormalOperation, ETrue  );
                }
                
            FLOG( _L("CLogsEngine: CallState deactivate") );                   
                
            }
            break;
            
        case EPSTelephonyCallStateUninitialized:
        case EPSTelephonyCallStateNone:
        case EPSTelephonyCallStateDialling:
        case EPSTelephonyCallStateAnswering:
        case EPSTelephonyCallStateDisconnecting:
        case EPSTelephonyCallStateConnected:
        case EPSTelephonyCallStateHold:
            
        default:            
            {
            //We must asap recover the db activities in order to provide the user quick perceived performance
            if( iMissedModel )
                {
                iMissedModel->DoActivateL( EFalse );
                }
            if( iReceivedModel )
                {
                iReceivedModel->DoActivateL( EFalse );
                }
            if( iDialledModel )
                {
                iDialledModel->DoActivateL( EFalse );
                }
            if( iMainModel )
                {
                iMainModel->DoActivateL( EFalse );
                }
                
            FLOG( _L("CLogsEngine: CallState activate") );                                   
            
            }
            break;            
        }
****************/        
    }



