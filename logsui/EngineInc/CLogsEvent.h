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
*     Implements MLogsEvent interface
*
*/


#ifndef __Logs_Engine_CLogsEvent_H__
#define __Logs_Engine_CLogsEvent_H__

//  INCLUDES
#include <e32base.h>
#include "MLogsEvent.h"

// FORWARD DECLARATION
class CPhCltEmergencyCall;

// CLASS DECLARATION

/**
 *  Implements MLogsEvent interface
 */
class CLogsEvent :  public CBase, 
                    public MLogsEvent
    {

    public:

       /**
        * Two-phased constructor.
        *
        * @return CLogsEvent object
        */
        static CLogsEvent* NewLC();
        
        /**
        * Destructor.
        */
        ~CLogsEvent();

    private:
    
       /**
        * C++ default constructor.
        */
        CLogsEvent();
              

    public: // from MLogsEvent

       /**
        * Has the event been read already
        *
        * @return true/false
        */
		TBool IsRead() const;
		
        virtual TTime Time() const;
        
        TLogId LogId() const;

        HBufC* Number();

        HBufC* RemoteParty( );

        TLogsDirection Direction() const;

        TUid EventUid() const;

        TLogsEventType EventType() const;

        TInt8 Duplicates() const;

        void InitializeEventL( const CLogEvent& aSource,
                               TLogsEventStrings aStrings,
                               TLogsModel aModel );

        MLogsEventData* LogsEventData() const;

   	   /**
        * Set the event read/unread according to flags in 
        *
        * @param true/false
        */
        void SetIsRead(TBool aRead);
        
       /**
        * Get the ALS flag from event 
        *
        * @return true/false
        */
        TBool ALS() const; 

        TBool CNAP() const { return EFalse; };
		//For ring duation feature
        TLogDuration RingDuration() const;
        
       /**
        * Return Phone number field type
        *
        * @return Phone number field type
        */       
        TInt NumberFieldType() const;
        
                               
    private:                               

        //void Reset(); 

        virtual void SetTime( const TTime aTime );

        void SetLogId( const TLogId aLogId );
       
        void SetNumber( HBufC* aNumber );

        void SetRemoteParty( HBufC* aRemoteParty );

        void SetDirection( const TLogsDirection aDirection );

        void SetEventUid( const TUid aUid );

        void SetEventType( const TLogsEventType aEventType );

        void SetDuplicates( const TInt8 aDuplicates );

        void SetLogsEventData( MLogsEventData* aLogsEventData );
        
       /**
        * Set the ALS flag
        *
        * @param true/false
        */
        void SetALS( TBool aALS ); 
        
        void SetRingDuration( TLogDuration aRingDuration );     
        
       /**
        * Check wether the number is an emergency number.
        *
        * @param aNumber, the number
        * @return true/false 
        */
        TBool IsEmergencyNumberL( const TDesC& aNumber );
        
        /**
         * Phone number field type setter
         *
         * @param set number field type
         */
         void SetNumberFieldType( const TInt aNumberFieldType );
        
    private:    // data
    
        //Flag to control skip time processing to optimise performance of event list view 
        TBool iTimeSet;          

        // own: log id
        TLogId  iLogId;        
        
        // own: direction
        TInt8  iDirection;

        // own: event type
        TInt8  iEventType;

        // own: uid
        //TUint8 iUid;
        TUid iUid;

        // own: Logs additional event data 
        MLogsEventData* iLogsEventData;

        /// Own: Remote party
        HBufC* iRemoteParty;

        /// Own: Nunber
        HBufC* iNumber;
        
        // own: duplicates
        TInt8   iDuplicates;               

        // own: time
        TTime   iTime;     
        
		//For ring duation feature    
        TLogDuration iRingDuration;
        
        // For indicate new missed calls feature 
	    TBool iIsRead;
	    
	    TBool iALS;
	    
	    // own: number field type
	    TInt iNumberFieldType;
 
    };

#endif      // __Logs_Engine_CLogsEvent_H__



// End of File
