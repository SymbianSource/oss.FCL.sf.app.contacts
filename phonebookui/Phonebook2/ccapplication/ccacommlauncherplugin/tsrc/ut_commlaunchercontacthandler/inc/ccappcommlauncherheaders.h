/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Headers of plugin view implementation to help unit testing
*
*/


#ifndef __CCAPPCOMMLAUNCHERHEADERS_H__
#define __CCAPPCOMMLAUNCHERHEADERS_H__

// logfile definition
#define KCommLauncherLogFile CCA_L("commlauncherplugin.txt")
#include "ccalogger.h"

#include <e32base.h>
#include <centralrepository.h>
#include <vpbkfieldtypeselectorfactory.h>

#include "t_ccappcmscontactfetcher.h"
#include "t_testsingleton.h"

class CCmsContactFieldInfo;
class MCCAppContactFieldDataObserver;

// used in CCmsContactFieldItem creation
const TInt PHONE = 1;
const TInt EMAIL = 2;
const TInt VOIP = 3;
const TInt MMS = 4;
const TInt PHONE2 = 5;
const TInt EMAIL2 = 6;
const TInt VOIP2 = 7;
const TInt NAME = 8;
// test defines
const TInt KTestNormal = 0;
const TInt KTestContactInfoNull = 1;
_LIT8( KTestString, "TestString" );

////////////////////////////////////////////////
// MCCAppContactFieldDataObserver
// Dummy interface
////////////////////////////////////////////////
class MCCAppContactFieldDataObserver
    {
public:

    class TParameter
        {
    public:

        enum TNotifyType
            {
            EUninitialised,
            EContactInfoAvailable,
            EContactDataFieldAvailable,
            EContactsChanged,
            EContactDeleted,
            EContactPresenceChanged
            };

        inline TParameter():
            iType( EUninitialised ),
            iContactInfo( NULL ),
            iContactField( NULL ),
            iStatusFlag( 0 ){};
    
        TNotifyType iType;
        TInt iStatusFlag;
        CCmsContactFieldInfo* iContactInfo;
        CCmsContactField* iContactField;
        };
   
    virtual void ContactFieldDataObserverNotifyL( 
        MCCAppContactFieldDataObserver::TParameter& aParameter ) = 0;

    virtual void ContactFieldDataObserverHandleErrorL( 
        TInt aState, TInt aError ) = 0;

    };    

////////////////////////////////////////////////
// CCmsContactField
// Dummy class
////////////////////////////////////////////////
class CCmsContactFieldItem : public CBase
    {
    public:  
        
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
            ECmsShareView               //10065
            };

        enum TCmsContactNotification
            {
            ECmsUnknownNotification = 0x0,                
            ECmsPhonebookNotification = 0x1,                
            ECmsPresenceVoIPNotification = 0x2,
            ECmsPresenceChatNotification = 0x4,
            ECmsPresenceAllNotification = 0x6,
            ECmsAllNotifications = 0x7                        
            };
            
        enum TCmsContactFieldGroup
            {
            ECmsGroupNone = 0,
            ECmsGroupVoIP,
            ECmsGroupVoice,
            ECmsGroupMessaging,
            ECmsGroupEmail
            };

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
            };
        
        TPtrC Data() const {};
    };

////////////////////////////////////////////////
// CCmsContactField
// Dummy class
////////////////////////////////////////////////
class CCmsContactField : public CBase
    {
public:

    CCmsContactField( TInt aType )
        {
        iType = aType;
        };

    const CCmsContactFieldItem& ItemL( TInt /*aIndex*/) const {};    
    
    CCmsContactFieldItem::TCmsContactField Type() const
        {
        switch ( iType )
            {
            // phone
            case PHONE:
                {
                return CCmsContactFieldItem::ECmsLandPhoneHome;
                break;
                }
            // email
            case EMAIL:
                {
                return CCmsContactFieldItem::ECmsEmailWork;
                break;
                }
           // voip
            case VOIP:
                {
                return CCmsContactFieldItem::ECmsVoipNumberHome;
                break;
                }
            // phone
            case PHONE2:
                {
                return CCmsContactFieldItem::ECmsMobilePhoneHome;
                break;
                }
            // email
            case EMAIL2:
                {
                return CCmsContactFieldItem::ECmsEmailGeneric;
                break;
                }
           // voip
            case VOIP2:
                {
                return CCmsContactFieldItem::ECmsVoipNumberWork;
                break;
                }
            // name
            case NAME:
                {
                return CCmsContactFieldItem::ECmsLastName;
                break;
                }
            // default phone
            default:
                {
                return CCmsContactFieldItem::ECmsMobilePhoneWork;
                break;
                }
            }
        };
    TInt ItemCount() const
        {
        return 1;
        };

    TInt HasDefaultAttribute() const 
        { 
        TInt ret ( KErrNotFound );
        switch ( iType )
            {
            // phone
            case PHONE:
                {
                ret = CCmsContactFieldItem::ECmsDefaultTypePhoneNumber;
                break;
                }
            // email
            case EMAIL:
                {
                ret = CCmsContactFieldItem::ECmsDefaultTypeEmail;
                break;
                }
            // voip
            case VOIP:
                {
                ret = CCmsContactFieldItem::ECmsDefaultTypeVoIP;
                break;
                }
           // mms
            default:
                {
                ret = CCmsContactFieldItem::ECmsDefaultTypeMms;
                break;
                }
            }

        return ret;
        };

    TInt iDummyDefaultAttributeBitMask;
    TInt iDummy;
    TInt iType;
    };

    
////////////////////////////////////////////////
// CCmsContactFieldInfo
// Dummy class
////////////////////////////////////////////////
class CCmsContactFieldInfo : public CBase
    {
public:
    static CCmsContactFieldInfo* NewL()
        {
        CCmsContactFieldInfo* self = new (ELeave) CCmsContactFieldInfo();
        self->ConstructL();
        return self;
        };
    void ConstructL()
        {
        iArray = new (ELeave) RArray<CCmsContactFieldItem::TCmsContactField>();
        // phone
        iArray->Append( CCmsContactFieldItem::ECmsMobilePhoneHome );
        // email
        iArray->Append( CCmsContactFieldItem::ECmsEmailWork );
        // voip
        iArray->Append( CCmsContactFieldItem::ECmsVoipNumberHome );
        // not number
        iArray->Append( CCmsContactFieldItem::ECmsLastName );
        };
       
    void AddMoreFields()
        {
        // phone
        iArray->Append( CCmsContactFieldItem::ECmsLandPhoneHome );
        iArray->Append( CCmsContactFieldItem::ECmsMobilePhoneGeneric );
        iArray->Append( CCmsContactFieldItem::ECmsMobilePhoneHome );
        // email
        iArray->Append( CCmsContactFieldItem::ECmsEmailGeneric );
        iArray->Append( CCmsContactFieldItem::ECmsEmailHome );
        // voip
        iArray->Append( CCmsContactFieldItem::ECmsVoipNumberWork );
        // im
        iArray->Append( CCmsContactFieldItem::ECmsImpp );
        // url
        iArray->Append( CCmsContactFieldItem::ECmsUrlHome );
        }; 
    const RArray<CCmsContactFieldItem::TCmsContactField>& Fields() const { return *iArray; };
    ~CCmsContactFieldInfo()
        {
        iArray->Close();
        delete iArray;
        };
    TInt iDummy;
    RArray<CCmsContactFieldItem::TCmsContactField>* iArray;
    };

#include "../../../inc/ccappcommlauncherprivatecrkeys.h"
#include "../../../inc/ccappcommlaunchercontacthandler.h"
   
////////////////////////////////////////////////
// CCCAppCommLauncherView
// Dummy class
////////////////////////////////////////////////
class CCCAppCommLauncherView : 
    public CBase,
    public MCCAppContactHandlerNotifier
    {
public:

    CCCAppCommLauncherView( /*CCCAppCommLauncherPlugin& aPlugin*/ ) /*: iPlugin( &aPlugin )*/
        {
        iContactChangedNotifyCalled = EFalse;
        iContactEnabledFieldsChangedNotifyCalled = EFalse;
        };

    static CCCAppCommLauncherView* NewL( /*CCCAppCommLauncherPlugin& aPlugin*/ )
        {
        CCCAppCommLauncherView* self = new (ELeave) CCCAppCommLauncherView( /*aPlugin*/ );
        return self;
        };

    // MCCAppContactHandlerNotifier
    void ContactInfoFetchedNotifyL( 
        const CCmsContactFieldInfo& /*aContactFieldInfo*/ )
        {
        iContactEnabledFieldsChangedNotifyCalled = ETrue;
        };

    // MCCAppContactHandlerNotifier
    void ContactFieldFetchedNotifyL( 
        const CCmsContactField& /*aContactField*/ )
        {
        iContactChangedNotifyCalled = ETrue;
        };

    // MCCAppContactHandlerNotifier
    void ContactFieldFetchingCompletedL(){};
    // MCCAppContactHandlerNotifier
    void ContactsChangedL(){};
    // MCCAppContactHandlerNotifier
    void ContactPresenceChangedL( const CCmsContactField& /*aContactField*/){};
        
    TBool ContactChangedNotifyCalled()
        {
        return iContactChangedNotifyCalled;
        };
    TBool ContactEnabledFieldsChangedNotifyCalled()
        {
        return iContactEnabledFieldsChangedNotifyCalled;
        }
    void ResetVariables()
        {
        iContactChangedNotifyCalled = EFalse;
        iContactEnabledFieldsChangedNotifyCalled = EFalse;
        };
    
    //CCCAppCommLauncherPlugin& Plugin(){ return *iPlugin; };        

    //CCCAppCommLauncherPlugin* iPlugin;
    TBool iContactChangedNotifyCalled;
    TBool iContactEnabledFieldsChangedNotifyCalled;
    };    

////////////////////////////////////////////////
// CCCAppCommLauncherPlugin
// Dummy class
////////////////////////////////////////////////
class CCCAppCommLauncherPlugin : 
    public CBase
    {

public:
    
    CCCAppCommLauncherPlugin()
    {      
    };
    
    ~CCCAppCommLauncherPlugin() 
    {      
    };
    
    /*
     * Sets the title to the plugin
     * @param aTitle - title to be set     
     */
    void SetTitleL( const TDesC& aTitle )
    {      
    };

    };
    
    
    
#endif // __CCAPPCOMMLAUNCHERHEADERS_H__
