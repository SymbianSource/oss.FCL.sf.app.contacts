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
*     Central repository data base class
*
*/


// INCLUDE FILES
#include "CLogsSharedData.h"
#include "MLogsObserver.h"
#include "LogsUID.h"
#include "LogsConstants.hrh"
//For ring duation feature
#include "LogsVariant.hrh"
#include <LogsDomainCRKeys.h>           //Logs Central Repository keys for logging
                                        //voice and VoIP call duration counters.
                                        //PhoneApp uses the same to write the values.
#include <settingsinternalcrkeys.h>     //PhoneApp's Central Repository keys 
                                        
#include <dclcrkeys.h>                  //DataConnectionLogger's Central Repository keys 
                                        //for logging GPRS/WLAN counters
#include <cenrepnotifyhandler.h>  
#include <AvkonInternalCRKeys.h>
#include <featmgr.h>

// CONSTANTS
// Dynamic VoIP off
const TInt KVoIPOFF = 0;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////CLogsSharedDataListener//////////////////////////////////////    
    //CLogsSharedDataListener is helper class to provide wrapup objects to provide information 
    //from which instance of CCenRepNotifyHandler we have got notification. This can be removed and 
    //replaced by better implementation when notifications provide this information directly
    class CLogsSharedDataListener : public CBase, public MCenRepNotifyHandlerCallback
        {
        public:
            static CLogsSharedDataListener* NewL( TUid aCategory,
                                           CLogsSharedData& aCallback, 
                                           CRepository& aSession);
                                           
            ~CLogsSharedDataListener();     
            
            //From MCenRepNotifyHandlerCallback: Handle integer key value change
            void HandleNotifyInt(TUint32 aKey, TInt aNewValue );

            void HandleNotifyError(TUint32 aId, TInt error, CCenRepNotifyHandler* aHandler);
            void HandleNotifyGeneric(TUint32 aId);

        private:
            CLogsSharedDataListener( TUid aCategory, CLogsSharedData& aCallback, 
                                     CRepository& aSession);
            void ConstructL();        
    
        private: // data
            TUid                          iCategory;
            CRepository&                  iSession;    
            CLogsSharedData&              iCallback;            
            CCenRepNotifyHandler*         iNotifyHandler;

        };
//////////////////////////////////////CLogsSharedDataListener//////////////////////////////////////    
///////////////////////////////////////////////////////////////////////////////////////////////////


// LOCAL FUNCTION PROTOTYPES

// ================= MEMBER FUNCTIONS =======================


// ----------------------------------------------------------------------------
// CLogsSharedData::CLogsSharedData
// ----------------------------------------------------------------------------
//
CLogsSharedData::CLogsSharedData(): 
    iState( EStateUndefined )
    {
    }


// ----------------------------------------------------------------------------
// CLogsSharedData::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSharedData::ConstructL()
    {
    //Repository sessions cannot be recycled, so each UID needs a separate repository session object.        
    iRepository_Logs    = CRepository::NewL( KCRUidLogs );
    iRepository_Dcl     = CRepository::NewL( KCRUidDCLLogs );    
    iRepository_Voip    = CRepository::NewL( KCRUidTelephonySettings );            

    iNotifyHandler_Logs = CLogsSharedDataListener::NewL( KCRUidLogs,              *this, *iRepository_Logs );
    iNotifyHandler_Dcl  = CLogsSharedDataListener::NewL( KCRUidDCLLogs,           *this, *iRepository_Dcl );
    iNotifyHandler_Voip = CLogsSharedDataListener::NewL( KCRUidTelephonySettings, *this, *iRepository_Voip );           
    
    CheckShowRingDurationL();   //Read the ring duration key for showing/not showing ring duration for missed calls
    CheckMSKEnabledInPlatformL();
    CheckVoIPEnabledInPlatform();
    }


// ----------------------------------------------------------------------------
// CLogsSharedData::CheckMSKEnabledInPlatform
//
// Checks if the MSK support is enabled in the platform and stores the 
// result in iMSKEnabledInPlatform.
// ---------------------------------------------------------------------------
//    
void CLogsSharedData::CheckMSKEnabledInPlatformL()
    {    
    // Check if the MSK support is enabled in the platform and store the 
    // result in iMSKEnabledInPlatform
    CRepository* cenRep = NULL;
    TRAPD(err, cenRep = CRepository::NewL( KCRUidAvkon ));
    if (!err)
        {
        err = cenRep->Get( KAknMiddleSoftkeyEnabled, iMSKEnabledInPlatform );
        delete cenRep;
        }
    User::LeaveIfError( err );
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::CheckVoIPEnabledInPlatform
//
// Checks if the VoIP support is enabled in the platform and stores the 
// result in iVoIPEnabledInPlatform.
// ---------------------------------------------------------------------------
//    
void CLogsSharedData::CheckVoIPEnabledInPlatform()
    {    
    if ( FeatureManager::FeatureSupported(KFeatureIdCommonVoip) )
        {
        TInt VoIPSupported( KVoIPOFF );
        iRepository_Voip->Get( KDynamicVoIP, VoIPSupported );
        iVoIPEnabledInPlatform = KVoIPOFF != VoIPSupported;
        }
    }
    
// ----------------------------------------------------------------------------
// CLogsSharedData::IsMSKEnabledInPlatform
// ----------------------------------------------------------------------------
//   
TBool CLogsSharedData::IsMSKEnabledInPlatform() 
    {
    return iMSKEnabledInPlatform;
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::IsVoIPEnabledInPlatform
// ----------------------------------------------------------------------------
//   
TBool CLogsSharedData::IsVoIPEnabledInPlatform() 
    {
    return iVoIPEnabledInPlatform;
    }
    
// ----------------------------------------------------------------------------
// CLogsSharedData::NewL
// ----------------------------------------------------------------------------
//
CLogsSharedData* CLogsSharedData::NewL() 
    {
    CLogsSharedData* self = new (ELeave) CLogsSharedData;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------------------------------
// CLogsSharedData::CLogsSharedData
// ----------------------------------------------------------------------------
//
CLogsSharedData::~CLogsSharedData()
    {
    delete iNotifyHandler_Logs;
    delete iNotifyHandler_Dcl;    
    delete iNotifyHandler_Voip; 
         
    // Closing connections:
    delete iRepository_Logs;
    delete iRepository_Dcl;    
    delete iRepository_Voip;  
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::LastCallTimer
//
// Get the Last Call Timer's value. 
// ----------------------------------------------------------------------------
//
TInt CLogsSharedData::LastCallTimer( TAlsEnum aWhatLine, TVoipEnum aVoip )
    {
    TInt getValue( 0 );

    if( aWhatLine == ELineOne )                                         //Retrieve last time of a specified line type:       
        {
        iRepository_Logs->Get( KLogsLastCallTimerLine1, getValue);      //   - Last call time line1                            
        }
    else if( aWhatLine == ELineTwo )
        {
        iRepository_Logs->Get( KLogsLastCallTimerLine2, getValue);      //   - Last call time line2                                                
        }
    else if( aWhatLine == EAllLines && aVoip == EVoipIncluded )               
        {
        iRepository_Logs->Get( KLogsActualLastCallTimer,   getValue );  //   - Last call time line1, line2 or voip
        }
    else if( aWhatLine == EVoiceLinesExcluded && aVoip == EVoipIncluded )               
        {
        iRepository_Logs->Get( KLogsLastCallTimerVoIP, getValue);   //   - Last call time voip (SettingsInternalCRKeys.h)
        }
    //FIXME: In the case below we don't have cellular-only timer available for last call, so we have to use 
    //instead a timer for line1 or line2 if actual last call was a voip call.
    else if( aWhatLine == EAllLines && aVoip == EVoipExcluded )               
        {                                                               //   - Last call time line1 or line2 only
        TInt comparisonValue( 0 );        
        iRepository_Logs->Get( KLogsActualLastCallTimer,   getValue );
        iRepository_Logs->Get( KLogsLastCallTimerVoIP, comparisonValue );
        
        if( getValue == comparisonValue ) //Last call was voip. We need to use line1 or line2 value instead
            {
            iRepository_Logs->Get( KLogsLastCallTimerLine1, getValue);            
            
            if( getValue == 0 )
                {
                iRepository_Logs->Get( KLogsLastCallTimerLine2, getValue );            
                }
            }
        }

    return getValue;
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::DialledCallsTimer
//
// Get the Dialled Call Timer's value
// ----------------------------------------------------------------------------
//
TInt CLogsSharedData::DialledCallsTimer( TAlsEnum aWhatLine, TVoipEnum aVoip )
    {
    TInt getValue( 0 );
    TInt tempValue( 0 );

    //First voice lines (if requested)
    if( aWhatLine == EAllLines )
        {
        //Voice line1            
        iRepository_Logs->Get( KLogsDialledCallsTimerLine1, tempValue);                    
        getValue = tempValue;
        //Voice line2                    
        tempValue = 0;
        iRepository_Logs->Get( KLogsDialledCallsTimerLine2, tempValue );                                
        getValue += tempValue;
        }
    else if( aWhatLine == ELineOne )
        {
        //Voice line1            
        iRepository_Logs->Get( KLogsDialledCallsTimerLine1, tempValue);                    
        getValue = tempValue;
        }
    else if( aWhatLine == ELineTwo )
        {
        //Voice line2                    
        iRepository_Logs->Get( KLogsDialledCallsTimerLine2, tempValue );                                
        getValue += tempValue;
        }
        
    //Then voip (if requested)
    if( aVoip == EVoipIncluded )
        {
        //Voip
        tempValue = 0;
        iRepository_Logs->Get( KLogsDialledCallsTimerVoIP, tempValue );     //SettingsInternalCRKeys.h 
        getValue += tempValue;
        }
        
    return getValue;
    }
// ----------------------------------------------------------------------------
// CLogsSharedData::ReceivedCallsTimer
//
// Get the Received Call Timer's value
// ----------------------------------------------------------------------------
//
TInt CLogsSharedData::ReceivedCallsTimer( TAlsEnum aWhatLine, TVoipEnum aVoip )
    {
    TInt getValue( 0 );
    TInt tempValue( 0 );    
    
    //First voice lines (if requested)    
    if( aWhatLine == EAllLines )
        {
        //Voice line1                        
        iRepository_Logs->Get( KLogsReceivedCallsTimerLine1, tempValue);                                
        getValue = tempValue;
        //Voice line2        
        tempValue = 0;
        iRepository_Logs->Get( KLogsReceivedCallsTimerLine2, tempValue);                                            
        getValue += tempValue;
        }
    else if( aWhatLine == ELineOne )
        {
        //Voice line1                        
        iRepository_Logs->Get( KLogsReceivedCallsTimerLine1, tempValue);                                
        getValue = tempValue;
        }
    else if( aWhatLine == ELineTwo )
        {
        //Voice line2        
        iRepository_Logs->Get( KLogsReceivedCallsTimerLine2, tempValue);                                            
        getValue += tempValue;
        }

    //Then voip (if requested)
    if( aVoip == EVoipIncluded )
        {
        //Voip
        tempValue = 0;
        iRepository_Logs->Get( KLogsReceivedCallsTimerVoIP, tempValue );    //SettingsInternalCRKeys.h
        getValue += tempValue;
        }

    return getValue;
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::AllCallsTimer
//
// Get the All Calls Timer's value
// ----------------------------------------------------------------------------
//
TInt CLogsSharedData::AllCallsTimer( TAlsEnum aWhatLine, TVoipEnum aVoip )
    {
    return DialledCallsTimer( aWhatLine, aVoip  ) + ReceivedCallsTimer( aWhatLine, aVoip  );
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::ClearCallTimers
//
// Clears all Call Timers 
// ----------------------------------------------------------------------------
//
void CLogsSharedData::ClearCallTimers()
    {
    //Last calls        
    iRepository_Logs->Set( KLogsActualLastCallTimer,        0 );
    iRepository_Logs->Set( KLogsLastCallTimerLine1,         0 );
    iRepository_Logs->Set( KLogsLastCallTimerLine2,         0 );
    iRepository_Logs->Set( KLogsLastCallTimerVoIP,          0 );     
    //Received calls                
    iRepository_Logs->Set( KLogsReceivedCallsTimerLine1,    0 );
    iRepository_Logs->Set( KLogsReceivedCallsTimerLine2,    0 );
    iRepository_Logs->Set( KLogsReceivedCallsTimerVoIP,     0 );         
    //Dialled calls                    
    iRepository_Logs->Set( KLogsDialledCallsTimerLine1,     0 );
    iRepository_Logs->Set( KLogsDialledCallsTimerLine2,     0 );
    iRepository_Logs->Set( KLogsDialledCallsTimerVoIP,      0 );    
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::SetLoggingEnabled
//
// Sets the Logging ON/OFF in the Central repository
// ----------------------------------------------------------------------------
//
void CLogsSharedData::SetLoggingEnabled( TBool aLogging )
    {
    if( aLogging )
        {
        if (IsLoggingEnabled() == EFalse)
            {
            // If turning Logging ON from OFF state, reset the
            // new missed calls counter as the missed calls events during      EILU-757CLC
            // the time logging was OFF were lost anyway but the 
            // counter kept increasing. We only display the counter when 
            // logging is enabled.
            // (see CLogsSubAppListControlContainer::CreateListBoxContentsL)
            // 
            iRepository_Logs->Set( KLogsNewMissedCalls, 0);
            }
        iRepository_Logs->Set( KLogsLoggingEnabled, 1);     
        }
    else
        {
        // Also reset the new missed calls counter when turning Logging        EILU-757CLC
        // off to clear the missed calls indicator
        //    
        iRepository_Logs->Set( KLogsNewMissedCalls, 0);  
        
        iRepository_Logs->Set( KLogsLoggingEnabled, 0);                          
        }
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::IsLoggingEnabled
//
// Returns the Logging ON/OFF in the Central repository
// ----------------------------------------------------------------------------
//
TBool CLogsSharedData::IsLoggingEnabled()
    {
    TBool loggingEnabled;
    iRepository_Logs->Get( KLogsLoggingEnabled, loggingEnabled);         
    
    return loggingEnabled;   
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::NewMissedCalls
//
// Read and optionally also clear the count of missed calls.  
// ----------------------------------------------------------------------------
//
TInt CLogsSharedData::NewMissedCalls( const TBool aClearMissedCallCounter )
    {
    TInt tempNewMissedCalls( 0 );
    
    iRepository_Logs->Get( KLogsNewMissedCalls, tempNewMissedCalls );         
    if( aClearMissedCallCounter ) 
        {
        iRepository_Logs->Set( KLogsNewMissedCalls, 0);                        
        }
    
    return tempNewMissedCalls;
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::GprsSentCounter
//
// Read value of Gprs Sent Data Counter (provided by Data Connection Logger)
// ----------------------------------------------------------------------------
//
TInt64 CLogsSharedData::GprsSentCounter()
    {
    TInt64 sent = MAKE_TINT64( 0, 0 );  
    TBuf<50> buffer;    

    iRepository_Dcl->Get( KLogsGPRSSentCounter, buffer );            
    TLex lex( buffer );
    TInt error = lex.Val( sent );

    if ( error == KErrNone )
        {
        return sent;
        }
    else 
        {
        return MAKE_TINT64( 0, 0 );  //0;
        }    
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::GprsReceivedCounter
//
// Read value of Gprs Received Data Counter (provided by Data Connection Logger)
// ----------------------------------------------------------------------------
//
TInt64 CLogsSharedData::GprsReceivedCounter()
    {
    TInt64 rec = MAKE_TINT64( 0, 0 ); 
    TBuf<50> buffer;    
    
    iRepository_Dcl->Get( KLogsGPRSReceivedCounter, buffer );            
    TLex lex( buffer );
    TInt error = lex.Val( rec );

    if ( error == KErrNone )
        {
        return rec;
        }
    else 
        {
        return MAKE_TINT64( 0, 0 );  //0;
        }        
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::ClearGprsCounters
//
// ClearGprsCounters of Data Connection Logger
// ----------------------------------------------------------------------------
//
void CLogsSharedData::ClearGprsCounters()
    {
    TBuf<50> buffer;
    buffer.Num( 0 );
    iRepository_Dcl->Set( KLogsGPRSSentCounter, buffer);  
    iRepository_Dcl->Set( KLogsGPRSReceivedCounter, buffer);           
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::ShowCallDurationLogsL
//
// Read Logs Local Variation Flag to check do we need to show active call duration in Logs Timers view.
//(in Logs application, does not apply to Phone application)
// ----------------------------------------------------------------------------
//
TBool CLogsSharedData::ShowCallDurationLogsL()
    {
    TInt setting;
    TBool ret = EFalse;  //By default we don't show active call duration in Logs

    CRepository* repository = NULL;        
    TInt err;
    TRAP(err, repository = CRepository::NewL( KCRUidLogsLV ) );  //Leaves,if KCRUidLogsLV not in centrep
    
    if( err != KErrNone)
        {
        return EFalse; //By default we don' show active call duration in Logs
        }

    CleanupStack::PushL( repository );
    
    if ( repository->Get( KLogsActiveCallDuration, setting ) == KErrNone )
        {
        if ( setting > 0 ) // values: see keys_logs.xls
            {
            ret = ETrue; 
            }
        }

    CleanupStack::PopAndDestroy( repository );  // Close connection
    return ret;
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::ShowRingDuration
//
// For ring duation feature
// ----------------------------------------------------------------------------
//
TBool CLogsSharedData::ShowRingDuration() const
    {
    return iShowRingDuration;
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::CheckShowRingDurationL
// ----------------------------------------------------------------------------
//
void CLogsSharedData::CheckShowRingDurationL()
    {
    iShowRingDuration = EFalse; // initialize the value
   
    CRepository* repository = CRepository::NewL( KCRUidLogsLV );
    CleanupStack::PushL( repository );
        
    TInt setting = 0;
    if ( repository->Get( KLogsLVFlags, setting ) == KErrNone )
       {
       // if bit 0 of local variation flag is set, ring duration feature is ON
       if ( setting & KLogsLVFlagShowRingDuration )
          {
          iShowRingDuration = ETrue;
          }
           
        }
    CleanupStack::PopAndDestroy( repository );  // Close connection
    }

/*******************************************************************************
 FIXME: Toolbar is currently always on - keeping the toolbar visibility handling 
        sources in comments for now - remove later.
 
// ----------------------------------------------------------------------------
// CLogsSharedData::ToolbarVisibility
//
// Get touch UI Toolbar ON/OFF setting
// ----------------------------------------------------------------------------
//
TInt CLogsSharedData::ToolbarVisibility() const
    {
    TInt setting = 0;    
    iRepository_Logs->Get( KLogsShowToolbar, setting );
    return setting; //setting;//1;
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::SetToolbarVisibility
//
// Set touch UI Toolbar ON/OFF 
// ----------------------------------------------------------------------------
//
void CLogsSharedData::SetToolbarVisibility( TInt aToolbarVisibility )
    {
    iRepository_Logs->Set( KLogsShowToolbar, aToolbarVisibility );
    }
    
*******************************************************************************/

// ----------------------------------------------------------------------------
// CLogsSharedData::HandleNotifyInt
//
// Called from CLogsSharedDataListener.
// Handle integer key value change
// ----------------------------------------------------------------------------
//
void CLogsSharedData::HandleNotifyInt( TUid aCategory, TUint32 aKey, TInt /*aNewValue*/ )
    {
    InformObserver( aCategory, aKey );
    }
    
// ----------------------------------------------------------------------------
// CLogsSharedData::InformObserver
//
// If an integer key value we are interested in has been changed, we'll inform it observer.
// ----------------------------------------------------------------------------
//
void CLogsSharedData::InformObserver( TUid aCategory, TUint32 aKey )
    {
    //1. Update state for events that need not to be informed explicitly
    if( aCategory == KCRUidTelephonySettings )
        {
        switch ( aKey )
            {
            case KDynamicVoIP:
                CheckVoIPEnabledInPlatform();
                break;
            }
        }

    //2. Process events that need immediately informed to observer 
    if( iObserver )
        {
        if( aCategory == KCRUidLogs )
            {
             switch ( aKey )
                 {
                 //From KCRUidLogs we are interested only in changes in these keys (LogsInternalCRKeys.h)            
                 case KLogsNewMissedCalls:
                 case KLogsActualLastCallTimer: //This timer is updated every time a call is ended. However, this does not serve also as
                                                //notification for some other call timer below because it might be updated slightly later.
                 case KLogsLastCallTimerLine1:  // (so we have to listen separately to them too)
                 case KLogsLastCallTimerLine2:
                 case KLogsLastCallTimerLineVoIP:                 
                 
                 case KLogsReceivedCallsTimerLine1:
                 case KLogsReceivedCallsTimerLine2:
                 case KLogsReceivedCallsTimerLineVoIP:                 
                 
                 case KLogsDialledCallsTimerLine1:
                 case KLogsDialledCallsTimerLine2:
                 case KLogsDialledCallsTimerLineVoIP:
                     {
                     TInt err;
                     TRAP( err, iObserver->StateChangedL( NULL ) );
                     break;
                     } 
                 case KLogsLoggingEnabled:
                     {
                     iState = EStateLogEnableChanged;                                                    
                     TInt err;
                     TRAP( err, iObserver->StateChangedL( this ) );
                     break;
                     } 
                 }
            }
            
        if( aCategory == KCRUidDCLLogs )
            {
             switch ( aKey )
                 {
                 // From KCRUidDCLLogs we are interested only in changes in these keys (DclInternalKeys.h)            
                 case KLogsGPRSReceivedCounter: 
                 case KLogsGPRSSentCounter:
                 //case KLogsWLANSentCounter:            
                 //case KLogsWLANReceivedCounter:                             
                     {
                     TInt err;
                     TRAP( err, iObserver->StateChangedL( NULL ) );
                     } 
                 }
            }
        }
    }

// ----------------------------------------------------------------------------
// CLogsSharedData::HandleNotifyGeneric
//
// Called from CLogsSharedDataListener
// ----------------------------------------------------------------------------
//
void CLogsSharedData::HandleNotifyGeneric( TUid aCategory, TUint32 aKey)
    {
    if ( aKey == NCentralRepositoryConstants::KUnspecifiedKey )    
        {
        //Repository wide reset (e.g more than one setting has changed simultaneously) has 
        //caused generic notification.  
        
        //1. Update state for events that need not to be informed explicitly
        CheckVoIPEnabledInPlatform();

        //2. Inform observer it needs fetch all the keys it is interested in.
        if( iObserver )
            {    
            TInt err;
            TRAP( err, iObserver->StateChangedL( NULL ) );
            }
        }
    else
        {
        //In this case we know the key so we can call InformObserver 
        InformObserver( aCategory, aKey );                
        }
    }
    
// ----------------------------------------------------------------------------
// CLogsSharedData::SetObserver
// ----------------------------------------------------------------------------
//
void CLogsSharedData::SetObserver( MLogsObserver* aObserver )
    {
    iObserver = aObserver;
    }


TLogsState CLogsSharedData::State() const
    {
    return iState;
    }


///////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////CLogsSharedDataListener//////////////////////////////////////    
//CLogsSharedDataListener is a helper class to provide wrapup objects to provide information 
//from which instance of CCenRepNotifyHandler we have got notification. This can be removed and 
//replaced by better implementation when notifications provide this information directly

// ----------------------------------------------------------------------------
// CLogsSharedDataListener::NewL
// ----------------------------------------------------------------------------
//
CLogsSharedDataListener* CLogsSharedDataListener::NewL( TUid aCategory, 
                                                        CLogsSharedData& aCallback, 
                                                        CRepository& aSession)
    {
    CLogsSharedDataListener* self = new (ELeave) CLogsSharedDataListener( aCategory, 
                                                                          aCallback, 
                                                                          aSession) ;    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLogsSharedDataListener::CLogsSharedDataListener
// ----------------------------------------------------------------------------
//
CLogsSharedDataListener::CLogsSharedDataListener( TUid aCategory, 
                                                  CLogsSharedData& aCallback, 
                                                  CRepository& aSession )
                                                : iCategory( aCategory ), iSession(aSession), iCallback(aCallback) 
    {
    }

// ----------------------------------------------------------------------------
// CLogsSharedDataListener::ConstructL
// ----------------------------------------------------------------------------
//
void CLogsSharedDataListener::ConstructL()
    {
    //We dont't provide separate handlers for keys we are intrested in this category (KLogsNewMissedCalls,
    //KLogsActualLastCallTimer, KLogsGPRSReceivedCounter etc) but just listen changes of any key.
    iNotifyHandler = CCenRepNotifyHandler::NewL( *this, iSession );
    iNotifyHandler->StartListeningL();
    }
   
// ----------------------------------------------------------------------------
// CLogsSharedDataListener::~CLogsSharedDataListener
// ----------------------------------------------------------------------------
//
CLogsSharedDataListener::~CLogsSharedDataListener()
    {
    if ( iNotifyHandler )    
        {
        iNotifyHandler->StopListening();
        }
    delete iNotifyHandler;
    }
   
// ----------------------------------------------------------------------------
// CLogsSharedDataListener::HandleNotifyInt
//
// From MCenRepNotifyHandlerCallback
// Handle integer key value change. We just add category id and forward the call.
// ----------------------------------------------------------------------------
//
void CLogsSharedDataListener::HandleNotifyInt(TUint32 aKey, TInt aNewValue )
    {
    iCallback.HandleNotifyInt( iCategory, aKey, aNewValue );
    }
    
// ----------------------------------------------------------------------------
// CLogsSharedDataListener::HandleNotifyGeneric
//
// From MCenRepNotifyHandlerCallback. We just add category id and forward the call.
// ----------------------------------------------------------------------------
//
void CLogsSharedDataListener::HandleNotifyGeneric( TUint32 aKey )
    {
    iCallback.HandleNotifyGeneric( iCategory, aKey );    
    }
    
// ----------------------------------------------------------------------------
// CLogsSharedDataListener::HandleNotifyError
//
// From MCenRepNotifyHandlerCallback
// ----------------------------------------------------------------------------
//
void CLogsSharedDataListener::HandleNotifyError( TUint32                 //aId 
                                                ,TInt                   //error 
                                                ,CCenRepNotifyHandler*  //aHandler
                                                )
   {
    // No need to handle notify errors
    // RDebug::Print(_L("Handler: %d, error for key: %d, error: %d"), aHandler, aId, error);
    }
    
//////////////////////////////////////CLogsSharedDataListener//////////////////////////////////////    
///////////////////////////////////////////////////////////////////////////////////////////////////


//  End of File
