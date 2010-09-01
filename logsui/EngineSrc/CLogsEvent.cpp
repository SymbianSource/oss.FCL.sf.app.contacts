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
#include <logwrap.hrh>
#include <AiwPoCParameters.h>  //EPoCAdhocDialoutGroupMO etc...
#include <CPhCltEmergencyCall.h>

#include "CLogsEvent.h"
#include "CLogsEventData.h"
#include "LogsEngConsts.h"
#include "LogsApiConsts.h"     //Additional event UIDs

// CONSTANTS
const TInt KMaxDuplicates = 99;

#ifdef _DEBUG
_LIT(KPanicMsg,"CLogsEvent");
#endif


// ----------------------------------------------------------------------------
// CLogsEvent::NewLC
// ----------------------------------------------------------------------------
//
CLogsEvent* CLogsEvent::NewLC()
    {
    CLogsEvent* self = new (ELeave) CLogsEvent;
    CleanupStack::PushL( self );
    return self;
    }
    
// ----------------------------------------------------------------------------
// CLogsEvent::CLogsEvent
// ----------------------------------------------------------------------------
//
CLogsEvent::CLogsEvent() 
    {
    iRemoteParty = 0;  
    iNumber = 0;  
    iLogsEventData = 0;
    iRingDuration = 0;
    iNumberFieldType = -1;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::~CLogsEvent
// ----------------------------------------------------------------------------
//
CLogsEvent::~CLogsEvent() 
    {
    delete iRemoteParty;
    delete iNumber;
    delete iLogsEventData;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::InitializeEventL
// ----------------------------------------------------------------------------
//
void CLogsEvent::InitializeEventL( 
    const CLogEvent& aSource, 
    TLogsEventStrings aStrings,
    TLogsModel aModel )
    {
    delete iRemoteParty;
    iRemoteParty = 0;
    delete iNumber;
    iNumber = 0;

    delete iLogsEventData;
    iLogsEventData = 0;

    SetDuplicates( 0 );
    SetDirection( EDirUndefined );
    SetEventType( ETypeUsual );
    SetALS(EFalse);
    SetNumberFieldType(-1);
    
    //For SMS'es part data is packed into data field (by sms stack), so in this case there is no 
    //need to try to parse S60 specific data from it). Later this parsing can be move here from
    TBool readMessageParts = ( 
        aSource.EventType() == KLogShortMessageEventTypeUid ||
        aSource.EventType() == KLogsEngMmsEventTypeUid );
    
    MLogsEventData* logsEventData = CLogsEventData::NewL( aSource.Data(), readMessageParts ) ;  
    SetLogsEventData( logsEventData );  //Ownership transferred to this object
    
    // TIME NOT needed in event list view (skip to optimise performance of event list view)
    if( aModel != ELogsMainModel )
        {
        TTime universal;
        TTime home;
        TTimeIntervalMinutes interval;

        universal.UniversalTime();
        home.HomeTime();
        TInt err = home.MinutesFrom( universal, interval );

        if( err )
            {
            SetTime( aSource.Time() );
            }
        else
            {
            SetTime( aSource.Time() + interval );
            }
        }
    else
        {
        iTimeSet = EFalse;    
        }
    
    // LOG ID
    SetLogId( aSource.Id() );

    // DIRECTION
    if( aSource.Direction() == aStrings.iInDirection ||
        aSource.Direction() == aStrings.iInDirectionAlt )
        {
        SetDirection( EDirIn );
        }
    else if( aSource.Direction() == aStrings.iOutDirection ||
        aSource.Direction() == aStrings.iOutDirectionAlt )
        {
        SetDirection( EDirOut );
        }
    else if( aSource.Direction() == aStrings.iMissedDirection )
        {
        SetDirection( EDirMissed );
        }

    // UID
    SetEventUid( aSource.EventType() );

    // EVENT TYPE
    TUid uid = aSource.EventType();

    // Private, unknown or payphone call
    if( aSource.RemoteParty().Length() > 0 ) 					
        {
        // if "Unknown" in remote party and no number/url, type of event is unknown
        if( aSource.RemoteParty() == aStrings.iUnKnownNumber &&
            aSource.Number().Length() == 0 &&
            LogsEventData()->Url().Length() == 0 ) 		
            {
            SetEventType( ETypeUnknown );
            }
        // if "Private" in remote party and no number/url, type of event is private
        else if( aSource.RemoteParty() == KLogsPrivateText && 
                 aSource.Number().Length() == 0 &&
                 LogsEventData()->Url().Length() == 0 )  //"Private" LogsEngConsts.h
            {
            SetEventType( ETypePrivate );
            }
        // if "Payphone" in remote party and no number/url, type of event is payphone
        else if( aSource.RemoteParty() == KLogsPayphoneText && 
                 aSource.Number().Length() == 0 &&
                 LogsEventData()->Url().Length() == 0 )	//"Payphone" LogsEngConsts.h
            {
            SetEventType( ETypePayphone );
            }
        }              
    // Poc group call        
    else if( LogsEventData()->PoC() &&         //EventData initialised already (above), so we can use dest here
        ( TPoCOperationTypeId( LogsEventData()->Type() ) == EPoCAdhocDialoutGroupMO   ||    //See TPoCOperationTypeId in 
          TPoCOperationTypeId( LogsEventData()->Type() ) == EPoCAdhocDialoutGroupMT   ||    //AiwPoCParameters.h
          TPoCOperationTypeId( LogsEventData()->Type() ) == EPoCPredefDialoutGroupMO  ||    
          TPoCOperationTypeId( LogsEventData()->Type() ) == EPoCPredefDialoutGroupMT ) )    
          //Dialin groups (EPoCDialinGroupMO) are NOT group calls 
        {
        SetEventType( ETypePoCGroupCall );            
        }
    // SAT event (SIM application toolkit)
    else if( ( uid == KLogCallEventTypeUid || 
               uid == KLogDataEventTypeUid ||
               uid == KLogShortMessageEventTypeUid ) && 
               aSource.Number().Length() == 0 &&
               LogsEventData()->Url().Length() == 0 &&      //Url set above so we can use it here: If URL,
                                                            // then not SAT event but normal Voip or PoC call
             ( aSource.Direction() == aStrings.iOutDirection ||
               aSource.Direction() == aStrings.iOutDirectionAlt ) )
        {
        SetEventType( ETypeSAT );
        }
    // Fallback in case entry wrongly written (is unknown but no "unknown" in remote party)
    else if ( ( aSource.RemoteParty().Length() == 0 ) &&
              ( aSource.Number().Length() == 0 ) && 
                LogsEventData()->Url().Length() == 0 )        //Url set above so we can use it here
        {
        SetEventType( ETypeUnknown );
        }     
    
    // Emergency call. 
    if( aSource.Number().Length() > 0 ) 
        {
        if( IsEmergencyNumberL( aSource.Number()) )  // The new Emergency call API tells this is an emergency number                                       
            {                                                                      
            SetEventType( ETypeEmerg ); 
            }
        }    
        
    // NUMBER
    if( EventType() == ETypeUsual || EventType() == ETypeEmerg )
        {
        if( aSource.Number().Length() )
            {
            SetNumber( aSource.Number().AllocL() );   
            }
        }

    // REMOTE PARTY
    if( EventType() == ETypeUsual || EventType() == ETypePoCGroupCall )
        {
        if( aSource.RemoteParty().Length() )
            {
            SetRemoteParty( aSource.RemoteParty().AllocL() );   
            }
        }
     
  
    //NUMBER FIELD TYEP
    if( EventType() == ETypeUsual )
        {
        if( aSource.Subject().Length() )
            {
            TLex lex( aSource.Subject() );
            TInt tmp;
            TInt err = lex.Val( tmp );
            if( KErrNone == err )
                {
                SetNumberFieldType( tmp );
                }
            }
        }
    
    // Set ALS on/off. A logical AND of KLogEventALS and the event flags passed in.
    SetALS( (aSource.Flags() & KLogEventALS ) );
         
    // We set iIsRead on/off according to flags for indicate new missed calls feature.
    // Used in missed calls view only. 
    if (aSource.Direction() == aStrings.iMissedDirection)		
   		{
   		// A logical AND of KLogEventRead and the event flags passed in.
    	SetIsRead(aSource.Flags() & KLogEventRead);	            
    	}
    else 													    // Otherwise set to already read
    	{													    // so dialled and received views  
    	SetIsRead(ETrue);										// wont show the icon
   		}

    //For ring duation feature
    if ( aSource.Direction() == aStrings.iMissedDirection )
        {
        SetDirection( EDirMissed );
        SetRingDuration( aSource.Duration() );
        }
    }
    

// ----------------------------------------------------------------------------
// CLogsEvent::IsEmergencyNumber
// Checks wether the number is an emergency number
// ----------------------------------------------------------------------------

TBool CLogsEvent::IsEmergencyNumberL(const TDesC& aNumber)
    {
    TBool isEmergencyNumber(EFalse);
    
    
    // Check first the hardcoded values "112" and "911"
    // plus the possible tag Phone app might have set
    if ( aNumber.Compare( KLogsEmergencyCall ) == 0 ||                //112 (however,not in e.g cdma (iStaticEmerg is false))
         aNumber.Compare( KLogsEmergencyCall911 ) == 0 ||              //911
         LogsEventData()->Emerg())          //Other emergency numbers that Phone app logs using data tag EMERG
        {
        isEmergencyNumber = ETrue;
        }
             

    // If the above does not prove the number to be an emergency number
    // check number using phoneclient emergencycall API
    else if (aNumber.Length() <= KPhCltEmergencyNumberSize)
        {
        CPhCltEmergencyCall* emergencyCallAPI = CPhCltEmergencyCall::NewL(NULL);
        TPhCltEmergencyNumber emergNumber = aNumber;
        
        if (emergencyCallAPI)
            {
            TInt result = emergencyCallAPI->IsEmergencyPhoneNumber(emergNumber, 
                                                                   isEmergencyNumber );
                                                            
            delete emergencyCallAPI;                                                
            if ( result != KErrNone ) 
                {
        	    return EFalse; 
                }
            }  
        }
 
    return isEmergencyNumber;             
    }


// ----------------------------------------------------------------------------
// CLogsEvent::SetALS()
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetALS( TBool aALS ) 
	{
	iALS = aALS;	
	}

// ----------------------------------------------------------------------------
// CLogsEvent::SetLogId
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetLogId( const TLogId aLogId )
    {
    iLogId = aLogId;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetNumber
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetNumber( HBufC* aNumber )
    {
    delete iNumber;    
    iNumber = 0;
    iNumber = aNumber;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetRemoteParty
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetRemoteParty( HBufC* aRemoteParty )
    {
    delete iRemoteParty;   
    iRemoteParty = 0;
    iRemoteParty = aRemoteParty;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetDirection
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetDirection( const TLogsDirection aDirection )
    {
    iDirection = static_cast<TInt8>( aDirection );
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetEventUid
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetEventUid( const TUid aUid )
    {
    iUid = aUid;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetEventType
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetEventType( const TLogsEventType aEventType )
    {
    iEventType = static_cast<TInt8>( aEventType );
    }

// ----------------------------------------------------------------------------
// CLogsEvent::ALS()
// ----------------------------------------------------------------------------
//
TBool CLogsEvent::ALS() const
	{
	return iALS;
	}
	
// ----------------------------------------------------------------------------
// CLogsEvent::LogId
// ----------------------------------------------------------------------------
//
TLogId CLogsEvent::LogId() const
    {
    return iLogId;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetIsRead
// ----------------------------------------------------------------------------    
void CLogsEvent::SetIsRead(const TBool aIsRead)
    {
    iIsRead = aIsRead;
    }


// ----------------------------------------------------------------------------
// CLogsEvent::Number
// ----------------------------------------------------------------------------
//
HBufC* CLogsEvent::Number() 
    {
    return iNumber;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::RemoteParty
// ----------------------------------------------------------------------------
//
HBufC* CLogsEvent::RemoteParty() 
    {
    return iRemoteParty;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::Direction
// ----------------------------------------------------------------------------
//
TLogsDirection CLogsEvent::Direction() const
    {
    return static_cast<TLogsDirection>( iDirection );
    }

 // ----------------------------------------------------------------------------
// CLogsEvent::EventUid
// ----------------------------------------------------------------------------
//
TUid CLogsEvent::EventUid() const
    {
    return iUid;
    }

 // ----------------------------------------------------------------------------
// CLogsEvent::EventType
// ----------------------------------------------------------------------------
//
TLogsEventType CLogsEvent::EventType() const
    {
    return static_cast<TLogsEventType>( iEventType );
    }


// ----------------------------------------------------------------------------
// CLogsEvent::IsRead
// ----------------------------------------------------------------------------
 TBool CLogsEvent::IsRead() const
	{
	return iIsRead;
	}

// ----------------------------------------------------------------------------
// CLogsEvent::RingDuration
//
// For ring duation feature
// ----------------------------------------------------------------------------
//
TLogDuration CLogsEvent::RingDuration() const
    {
    return iRingDuration;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetRingDuration
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetRingDuration( TLogDuration aRingDuration )
    {
    iRingDuration = aRingDuration;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::LogsEventData
// ----------------------------------------------------------------------------
//
MLogsEventData* CLogsEvent::LogsEventData() const
    {
    return iLogsEventData;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetLogsEventData
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetLogsEventData( MLogsEventData* aLogsEventData )
    {
    delete iLogsEventData;    
    iLogsEventData = 0;
    iLogsEventData = aLogsEventData;
    }



///////////////////////////////////////////////////////////////////////////////
//
// Additional data that is only needed in specific views
//
///////////////////////////////////////////////////////////////////////////////


// ----------------------------------------------------------------------------
// CLogsEvent::SetTime
//
// Time needed in recent views and in detail view (not needed in event view)
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetTime( const TTime aTime )
    {
    iTime = aTime;
    iTimeSet = ETrue;  
    }

// ----------------------------------------------------------------------------
// CLogsEvent::Time
// ----------------------------------------------------------------------------
//
TTime CLogsEvent::Time() const
    {
    // Time not needed in event list view, so this should not be called in those circumstances
    __ASSERT_DEBUG( iTimeSet, User::Panic(KPanicMsg, KErrNotSupported) );    
    return iTime;        
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetDuplicates
//
// Duplicates needed only in missed calls view
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetDuplicates( const TInt8 aDuplicates )
    {
    iDuplicates = aDuplicates;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::Duplicates
// ----------------------------------------------------------------------------
//
TInt8 CLogsEvent::Duplicates() const
    {
    if( iDuplicates <= KMaxDuplicates )
        {
        return iDuplicates;
        }
    return KMaxDuplicates;
    }

// ----------------------------------------------------------------------------
// CLogsEvent::SetNumberFieldType
// ----------------------------------------------------------------------------
//
void CLogsEvent::SetNumberFieldType( const TInt aNumberFieldType )
    {
    iNumberFieldType = aNumberFieldType;
    }
    
// ----------------------------------------------------------------------------
// CLogsEvent::NumberFieldType
// ----------------------------------------------------------------------------
//
TInt CLogsEvent::NumberFieldType() const
    {
    return iNumberFieldType;
    }

// End of file

