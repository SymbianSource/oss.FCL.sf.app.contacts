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
*     Contents of Log database's additional Data field
*
*/


#ifndef __Logs_Engine_CLogsEventData_H__
#define __Logs_Engine_CLogsEventData_H__

//  INCLUDES
#include <e32base.h>
#include "MLogsEventData.h"


// FORWARD DECLARATION

// CLASS DECLARATION

class CLogsEventData :  public CBase, 
                        public MLogsEventData
    {

    public:  

       /**
        * Two-phased constructor.
        *
        * @param aData LogEvent's additional free format field
        * @param aReadMessageParts In case this object needs only to be initialised but 
        *                         no  need to read actual (e.g. in case data contains packege
        *                         buffer in some other format )
        * @return CLogsEvent object
        */
        static CLogsEventData* NewL( const TDesC8 &aData, TBool aReadMessageParts );

       /**
        * Destructor.
        */
        virtual ~CLogsEventData();


    public:  
       /**
        *   ALS flag (Alternate line service).
        *
        *   @return  ALS status flag. EFalse if not ALS event, ETrue if ALS event.
        */
        TBool ALS() const;

        /**
        *   CNAP flag (Calling name presentation service).
        *
        *   @return  CNAP status flag. EFalse if not CNAP event, ETrue if CNAP event.
        */
        TBool CNAP() const;

        /**
        *   VT flag (Video Telephony).
        *
        *   @return  VT status flag. EFalse if not VT event, ETrue if VT event.
        */
        TBool VT() const;

        /**
        *   PoC flag.
        *
        *   @return  PoC status flag. EFalse if not PoC event, ETrue if PoC event.
        */
        TBool PoC() const;

        /**
        *   VoIP flag.
        *
        *   @return  VoIP status flag. EFalse if not VoIP event, ETrue if VoIP event.
        */
        TBool VoIP() const;

        /**
        *   Data Recived counter. 
        *
        *   @return  Data received (where applicable).
        */
        TInt64 DataReceived() const;

        /**
        *   URL. 
        *
        *   @return  URL (where applicable).
        */
        TDesC8& Url() const;

        /**
        *   Profile information (normally used for VoIP profile)
        *
        *   @return  Profile data (where applicable).
        */
        TDesC8& Profile() const;

        /**
        *   Additional reference, the Logging application can use to mediate, e.g. reference to PoC call group
        *
        *   @return  Additional reference data (where applicable).
        */
        TDesC8& Id() const;

        /**
        *   Data Sent counter. 
        *
        *   @return  Data sent (where applicable).
        */
        TInt64 DataSent() const;
        
        /**
        *   Data's type
        *
        *   @return  Data's type (where applicable).
        */
        TInt Type() const;

        /**
        *   My address
        *
        *   @return  User's own address (My address  where applicable).
        */
        TDesC8& MyAddress();        

        /**
        *   Emergency flag.
        *
        *   @return  Emergency call flag. EFalse if not emerg call, ETrue if emerg call.
        */
        TBool Emerg() const;

        /**
        *   Parts in sms.
        *
        *   @return  Parts in sms, this info is meaningful only for sms events
        */
        TInt MsgPartsNumber() const;
        
// Sawfish VoIP changes  >>>>
        /**
        *   Service ID of this event.
        *
        *   @return  Service ID. 
        */
        TUint32 ServiceId() const;
        
        /**
        *   ContactLink of this event.
        *
        *   @param aContactLink after return contains contactlink 
        *
        *   @return  KErrNone if link was found otherwise KErrNotFound 
        */
        TInt GetContactLink( TPtrC8& aContactLink );
        
        /**
        *   Returns the unparsed data of the data field.
        *
        *   @return  Data field.
        */
        TDesC8& DataField() const;      
// <<<<  Sawfish VoIP changes  

    private:

       /**
        * C++ default constructor.
        */
        CLogsEventData();


        CLogsEventData( const TDesC8 &aData );  

        /**
        * By default Symbian OS constructor is private.
        */
        void ConstructL( TBool aReadMessageParts );

        TBool CheckNonTaggedData(TPtrC8 aDataPtr );
        void CheckTaggedDataL();        
        void SetMsgPartsNumber();
              
    private:    // data

        //Temporary Pointer to Log database datafield reference (TPtrC8 to TDesC8 &)
        //NOTE! It is NOT guaranteed that the TDesC8& (and hence iDataFieldPtr we're using to point if) 
        //is alive longer than during the call the ConstructL.
        TPtrC8 iTempDataFieldPtr; 
   
        //contains the raw datafield data, this is stored in order to make
        //it possible for extensions to extract that information that they 
        //need. e.g. there might be identifiers used in the data field that
        //the parser in CLogsEventData::CheckTaggedDataL() might not under-
        //stand and then the extension could extact the information directly
        //from the datafield (by using its own parser).
        HBufC8* iDataField;
        
        // Alternate Line flag
        TBool iALS;

        // Calling Name Presentation service flag
        TBool iCNAP;

        // Video Telephone flag
        TBool iVT;

        // PoC flag
        TBool iPoC;

        // VoIP flag
        TBool iVoIP;

        //URL
        HBufC8* iURL8;

        //
        HBufC8* iProfile;

        //Internal ID (can be used to mediate additional Logging application's data)
        HBufC8* iId;

        //Flag that indicates whether this is an emegegency related event (i.e emerg call)
        TBool iEmerg;

        //Sent and received data where applicable (in bytes)
        TInt64 iDataSent;
        TInt64 iDataReceived;

        //Datatype
        TInt iDataType;
        
        //My address fields
        HBufC8* iMyAddress8;
        HBufC*  iMyAddress;        
    
        //Message parts. Meaningful only for sms            
        TInt iMsgPartsNumber;            

// Sawfish VoIP changes ---------
        // ServiceID
        TUint32 iServiceId;
        
        /*
        * ContactLink
        * Own.
        */
        HBufC8* iContactLink;    
// ------------------------------  
    };

#endif      // __Logs_Engine_CLogsEventData_H__



// End of File
