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
* Description: 
*       
*
*/


#ifndef __CMSCONTACTFIELDITEM__
#define __CMSCONTACTFIELDITEM__

// INCLUDES
#include <e32base.h>

// CLASS DECLARATION
class CCmsContactFieldItem : public CBase
    {
    public:  
        
        /** 
        * Field types
        */
        enum TCmsContactField
            {
            ECmsLastName = 10000,       //10000
            ECmsFirstName,              //10001
            ECmsUndefined,              //10002
            ECmsPresenceData,           //10003
            ECmsBrandedAvailability,    //10004
            ECmsThumbnailPic,           //10005
            ECmsSipAddress,             //10006
            ECmsLandPhoneHome,          //10007
            ECmsMobilePhoneHome,        //10008
            ECmsFaxNumberHome,          //10009
            ECmsVideoNumberHome,        //10010
            ECmsVoipNumberHome,         //10011
            ECmsEmailHome,              //10012
            ECmsUrlHome,                //10013
            ECmsLandPhoneWork,          //10014
            ECmsMobilePhoneWork,        //10015
            ECmsVideoNumberWork,        //10016
            ECmsFaxNumberWork,          //10017
            ECmsVoipNumberWork,         //10018
            ECmsEmailWork,              //10019
            ECmsUrlWork,                //10020
            ECmsEmailGeneric,           //10021
            ECmsUrlGeneric,             //10022
            ECmsLandPhoneGeneric,       //10023
            ECmsMobilePhoneGeneric,     //10024
            ECmsVideoNumberGeneric,     //10025
            ECmsAddrLabelGeneric,       //10026
            ECmsAddrPOGeneric,          //10027
            ECmsAddrExtGeneric,         //10028
            ECmsAddrStreetGeneric,      //10029
            ECmsAddrLocalGeneric,       //10030
            ECmsAddrRegionGeneric,      //10031
            ECmsAddrPostcodeGeneric,    //10032
            ECmsAddrCountryGeneric,     //10033
            ECmsVoipNumberGeneric,      //10034
            ECmsAddrLabelHome,          //10035
            ECmsAddrPOHome,             //10036
            ECmsAddrExtHome,            //10037
            ECmsAddrStreetHome,         //10038
            ECmsAddrLocalHome,          //10039
            ECmsAddrRegionHome,         //10040
            ECmsAddrPostcodeHome,       //10041
            ECmsAddrCountryHome,        //10042
            ECmsAddrLabelWork,          //10043
            ECmsAddrPOWork,             //10044
            ECmsAddrExtWork,            //10045
            ECmsAddrStreetWork,         //10046
            ECmsAddrLocalWork,          //10047
            ECmsAddrRegionWork,         //10048
            ECmsAddrPostcodeWork,       //10049
            ECmsAddrCountryWork,        //10050
            ECmsImpp,                   //10051
            ECmsFullName,               //10052
            ECmsNote,                   //10053
            ECmsNickname,               //10054
            ECmsJobTitle,               //10055
            ECmsDepartment,             //10056
            ECmsBirthday,               //10057
            ECmsAnniversary,            //10058
            ECmsCompanyName,            //10059
            ECmsMiddleName,             //10060
            ECmsAssistantNumber,        //10061
            ECmsCarPhone,               //10062
            ECmsPagerNumber,            //10063
            ECmsPushToTalk,             //10064
            ECmsShareView,              //10065
            ECmsAddrGeoGeneric,         //10066
            ECmsAddrGeoHome,            //10067
            ECmsAddrGeoWork,            //10068
            ECmsImageName               //10069
            };

        /** 
        * Field types
        */
        enum TCmsContactNotification
            {
            ECmsUnknownNotification = 0x0,                
            ECmsPhonebookNotification = 0x1,                
            ECmsPresenceVoIPNotification = 0x2,
            ECmsPresenceChatNotification = 0x4,
            ECmsPresenceAllNotification = 0x6,
            ECmsAllNotifications = 0x7                        
            };
            
        /** 
        * Field group types
        */
        enum TCmsContactFieldGroup
            {
            ECmsGroupNone = 0,
            ECmsGroupVoIP,
            ECmsGroupVoice,
            ECmsGroupMessaging,
            ECmsGroupEmail
            };


        /** 
        * Default attribute types for communications methods.
        * This enum is used to pass information about existing default fields
        * for communication methods in one integer.
        */
        enum TCmsDefaultAttributeTypes
            {
            /// Defaults
            ECmsDefaultTypeUndefined = 0,
            ECmsDefaultTypePhoneNumber = 1, 
            ECmsDefaultTypeVideoNumber = 2, 
            ECmsDefaultTypeSms = 4,
            ECmsDefaultTypeMms = 8, 
            ECmsDefaultTypeEmail = 16, 
            ECmsDefaultTypeEmailOverSms = 32,
            ECmsDefaultTypeVoIP = 64,
            ECmsDefaultTypePOC = 128,
            ECmsDefaultTypeImpp = 256,
            ECmsDefaultTypeUrl = 512
            // In case of changing this enum, remember to update source code
            // which uses last item as a number of items in this enum.
            };
        

        /** 
        * Descriptor for the data
        *
        * @return TPtrC 16-bit data descriptor
        */
        IMPORT_C TPtrC Data() const;

        /** 
        * Descriptor for the data
        *
        * @return TPtrC8 8-bit data descriptor
        */
        IMPORT_C TPtrC8 BinaryData() const;
        
        /** 
        * Descriptor for the info
        *
        * @return TPtrC8 8-bit data descriptor
        */        
        IMPORT_C TPtrC Info() const;        
    
    public:

        /** 
        * Set the data for this item
        *
        * @param const TDesC& New data
        * @return void
        */
        void SetData( const TDesC& aItemData );
        
        /** 
        * Set the info for this item
        *
        * @param const TDesC& New info
        * @return void
        */        
        void SetInfo( const TDesC& aItemInfo );        

        /** 
        * Set default attribute for this field item.
        *
        * @param const TInt default attribute bitmask
        * @return void
        */
        void SetDefaultAttribute( const TInt aDefaultAttribute ){ iDefaultAttributeMask = aDefaultAttribute; };

        /** 
        * Helper function to find out is there given default attribute 
        * for the field item.
        *
        * @see TVPbkDefaultType enum from VPbkFieldType.hrh
        * @param TVPbkDefaultType default type
        * @return TBool
        */        
        TBool HasDefaultAttribute( TCmsDefaultAttributeTypes aDefaultType );

    public:
        
        /**
    	* Symbian constructor
        *
        * @return CCmsContactField* New instance
    	*/
        static CCmsContactFieldItem* NewL();

        /**
    	* Symbian constructor
        *
        * @param const TDesC& New data
        * @return CCmsContactField* New instance
    	*/
        static CCmsContactFieldItem* NewL( const TDesC& aItemData );

        /**
    	* Symbian constructor
        *
        * @param const TDesC& New data
        * @return CCmsContactField* New instance
    	*/
        static CCmsContactFieldItem* NewL( const TDesC8& aItemData );

        /** 
        * Destructor
        */
        ~CCmsContactFieldItem();
    
    protected:

        /** 
        * C++ constructor is private
        */
        CCmsContactFieldItem();
        
        /** 
        * Symbian OS second-phase constructor
        *
        * @param TDesC& Item data
        * @return void
        */
        void ConstructL( const TDesC& aItemData );
        
        /** 
        * Symbian OS second-phase constructor
        *
        * @param TDesC8& Item data
        * @return void
        */
        void ConstructL( const TDesC8& aItemData );

    protected:  //Data is protected

        /** 
        * Storage for the item data
        */
        HBufC*                      iFieldDataBuf16;
        HBufC8*                     iFieldDataBuf8;
        HBufC*                      iFieldDataBufInfo;  
        TInt                        iDefaultAttributeMask;
    };

#endif  //__CMSCONTACTFIELDITEM__


// End of File
