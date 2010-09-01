/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*     Defines abstract interface for Log database's additional Data field
*
*/


#ifndef __Logs_Engine_MLogsEventData_H__
#define __Logs_Engine_MLogsEventData_H__

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATION

// CLASS DECLARATION

class MLogsEventData 
    {

    public:  
       /**
        * Destructor.
        */
        virtual ~MLogsEventData() {};


    public:  

       /**
        *   ALS flag (Alternate line service).
        *
        *   @return  ALS status flag. EFalse if not ALS event, ETrue if ALS event.
        */
		virtual TBool ALS() const = 0;

        /**
        *   CNAP flag (Calling name presentation service).
        *
        *   @return  CNAP status flag. EFalse if not CNAP event, ETrue if CNAP event.
        */
		virtual TBool CNAP() const = 0;

        /**
        *   VT flag (Video Telephony).
        *
        *   @return  VT status flag. EFalse if not VT event, ETrue if VT event.
        */
		virtual TBool VT() const = 0;

        /**
        *   PoC flag.
        *
        *   @return  PoC status flag. EFalse if not PoC event, ETrue if PoC event.
        */
		virtual TBool PoC() const = 0;

        /**
        *   VoIP flag.
        *
        *   @return  VoIP status flag. EFalse if not VoIP event, ETrue if VoIP event.
        */
		virtual TBool VoIP() const = 0;

        /**
        *   Data Recived counter. 
        *
        *   @return  Data received (where applicable).
        */
		virtual TInt64 DataReceived() const = 0;

        /**
        *   URL. 
        *
        *   @return  URL (where applicable).
        */
		virtual TDesC8& Url() const = 0;

        /**
        *   Profile information (normally used for VoIP profile)
        *
        *   @return  Profile data (where applicable).
        */
		virtual TDesC8& Profile() const = 0;

        /**
        *   Additional reference, the Logging application can use to mediate, e.g. reference to PoC call group
        *
        *   @return  Additional reference data (where applicable).
        */
		virtual TDesC8& Id() const = 0;

        /**
        *   Data Sent counter. 
        *
        *   @return  Data sent (where applicable).
        */
		virtual TInt64 DataSent() const = 0;
        
        /**
        *   Data's type
        *
        *   @return  Data's type (where applicable).
        */
        virtual TInt Type() const = 0;

        /**
        *   My address
        *
        *   @return  User's own address (My address  where applicable).
        */
        virtual TDesC8& MyAddress() = 0;        

        /**
        *   Emergency flag.
        *
        *   @return  Emergency call flag. EFalse if not emerg call, ETrue if emerg call.
        */
		virtual TBool Emerg() const = 0;      
		
        /**
        *   Parts in sms.
        *
        *   @return  Parts in sms, this info is meaningful only for sms events
        */
        virtual TInt MsgPartsNumber() const = 0;

// Sawfish VoIP changes ---------
        /**
         * Service Id
         *
         * @since S60 v3.2
         * @return Service Id
         */
        virtual TUint32 ServiceId() const = 0;
        
        /**
         * ContactLink
         *
         * @param aContactLink after return contains contactlink 
         *
         * @since S60 v3.2
         * @return KErrNone if link was found otherwise KErrNotFound
         */
        virtual TInt GetContactLink( TPtrC8& aContactLink ) = 0;
        
        /**
         * Returns the unparsed Data Field.
         *
         * @since S60 v3.2
         * @return Data Field
         */
        virtual TDesC8& DataField() const = 0;   
// ------------------------------  
    };

#endif      // __Logs_Engine_MLogsEventData_H__



// End of File
