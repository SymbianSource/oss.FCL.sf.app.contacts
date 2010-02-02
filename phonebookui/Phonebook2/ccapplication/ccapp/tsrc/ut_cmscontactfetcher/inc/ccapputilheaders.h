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
* Description:  Headers of ccaclient
*
*/


#ifndef __CCACLIENTHEADERS_H__
#define __CCACLIENTHEADERS_H__

#include <e32property.h>
// for constants
#include <coemain.h>

#define __CCAPPUNITTESTMODE


#define KCCAppUtilLogFile CCA_L("ccapputil.txt")

// aiw for phone call

#include <e32base.h>

#include "cmscontactfielditem.h"
#include "cmsnotificationhandlerapi.h"
#include <vpbkfieldtypeselectorfactory.h>

//common logger
#include "ccalogger.h"

// test cases
const TInt KTestCreateAndDelete = 0;
const TInt KTestOneContactFieldItem = 1;
const TInt KTestContactLink = 2;
const TInt KTestMSISDN = 3;
const TInt KTestEmail = 4;
const TInt KTestNoDataFields = 5;
const TInt KTestCompleteOpenWithError = 6;
const TInt KTestCompleteFetchWithError = 7;
const TInt KTestManyContactFieldItems = 8;

//  P&S KEYS and categories
const TUid KTestPropertyCat={0x10012349};  
enum TTestPropertyKeys 
    {
	ETestCaseStateProperty = 1,
    ETestParameterTypeProperty
    };    

// used in CCmsContactFieldItem creation
const TInt PHONE = 1;
const TInt EMAIL = 2;
const TInt VOIP = 3;
const TInt MMS = 4;
const TInt PHONE2 = 5;
const TInt EMAIL2 = 6;
const TInt VOIP2 = 7;
const TInt NAME = 8;

///////////////////////////////////////////////////
// Dummy class interface
// MCCAParameter
///////////////////////////////////////////////////
class MCCAParameter
    {

    public:

    /** Flags for controlling opening of CCA application */
    enum TConnectionFlags
        {
        // Default flag
        ENormal = 0x00010
        };

    /** 
     * Flags for indicating type of contact data 
     *
     * Notes related to specific search types below:
     * EContactId
     * EContactLink
     *  - Group IDs are not supported 
     * EContactEmail
     *  - not yet supported
     */
    enum TContactDataFlags
        {
        EContactNone        = 0,
        EContactId,
        EContactLink,
        EContactMSISDN,
        EContactEmail  
        /** ENotInUse       = 0x00100
        ENotInUse           = 0x00200 */
        };

    //virtual TConnectionFlags ConnectionFlag() = 0;

    /**
     * Getter for the contact data flags.
     * @since S60 5.0
     * @return the flags.
     */
    virtual TContactDataFlags ContactDataFlag() = 0;
    
    /**
     * Getter for the contact data.
     * @since S60 5.0
     * @return the reference of contact data.
     */
    virtual HBufC& ContactDataL() = 0;


    };
///////////////////////////////////////////////////
// Dummy class 
// CCCAParameter
///////////////////////////////////////////////////
class CCCAParameter : public CBase, 
					  public MCCAParameter
    {

public:
    CCCAParameter()
        {
        iContactDataFlag = MCCAParameter::EContactNone;
        };

    static CCCAParameter* NewL( )
        {
        CCCAParameter* self = new (ELeave)CCCAParameter();
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );
        return self;
        };

    virtual ~CCCAParameter()
        {
        delete iContactData;
        };

    TInt32 ContactIdL()
        { 
        return 1; 
        };

   HBufC8* ContactLinkL()
        {
        _LIT8( KTestContactLink, "TestContactLink");
        TPtrC8 ptr ( KTestContactLink );
        HBufC8* buf = ptr.AllocL();
        return buf;
        };

    TContactDataFlags ContactDataFlag()
        { 
        return iContactDataFlag; 
        };
    

    HBufC& ContactDataL()
        { 
        return *iContactData; 
        };
    
    private:
    void ConstructL()
        {
        // get property using category and key
        TInt type( MCCAParameter::EContactNone );
        TInt err = RProperty::Get( KTestPropertyCat, ETestParameterTypeProperty, type );

        if ( KErrNone == err )
            {
            switch ( type )
                {
                case EContactId:
                    {
                    iContactDataFlag = MCCAParameter::EContactId;
                    break;
                    }
                case EContactLink:
                    {
                    iContactDataFlag = MCCAParameter::EContactLink;
                    break;
                    }
                 case EContactMSISDN:
                    {
                    iContactDataFlag = MCCAParameter::EContactMSISDN;
                    break;
                    }
                   case EContactEmail:
                    {
                    iContactDataFlag = MCCAParameter::EContactEmail;
                    break;
                    }
                default:
                    {
                    iContactDataFlag = MCCAParameter::EContactNone;
                    break;
                    }
                }
            }
        _LIT( KTestContactData, "TestContactData");
        TPtrC ptr (KTestContactData);
        iContactData = ptr.AllocL();
        };

    TContactDataFlags iContactDataFlag;    
    HBufC* iContactData;    

    };


/** 
* Contact identifier.
*/
enum TCmsContactIdentifierType
    {    
    ECmsPackedContactLinkArray = 0
    };
    
    
///////////////////////////////////////////////////
// Dummy class 
// CCCAParameter
///////////////////////////////////////////////////
class CCmsContactBase : public CBase
    {
    public:
        CCmsContactBase(  )
            {
            };
        ~CCmsContactBase()
            {
            
            };
    };

    
// dummy class
class CCmsContactField : public CCmsContactBase
    {
    public:

        CCmsContactField( TInt aType )
            {
            iType = aType;
            };
        
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
            return CCmsContactFieldItem::ECmsMobilePhoneWork;
            };
            
        TInt ItemCount() const
            {
            return 1;
            };

        TInt HasDefaultAttribute() const 
            { 
            TInt ret ( KErrNotFound );
            return ret;
            };

        TInt iDummyDefaultAttributeBitMask;
        TInt iDummy;
        TInt iType;
    };

///////////////////////////////////////////////////
// Dummy class 
// CCCAParameter
///////////////////////////////////////////////////
class CCmsContactFieldInfo : public CCmsContactBase
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
        TInt testCase( 0 );
        // get property using category and key
        TInt err = RProperty::Get( KTestPropertyCat, ETestCaseStateProperty, testCase );

        if ( KTestOneContactFieldItem == testCase
             || KTestCompleteOpenWithError == testCase
             || KTestCompleteFetchWithError == testCase  )   
            {
            iArray->Append( CCmsContactFieldItem::ECmsMobilePhoneHome );
            }
        else if ( KTestContactLink == testCase  )   
            {
            iArray->Append( CCmsContactFieldItem::ECmsLastName );
            }
        else if ( KTestMSISDN == testCase  )   
            {
            iArray->Append( CCmsContactFieldItem::ECmsVoipNumberHome );
            }
        else if ( KTestEmail == testCase  )   
            {
            iArray->Append( CCmsContactFieldItem::ECmsEmailWork );
            }
        else if ( KTestManyContactFieldItems == testCase  )   
            {
            iArray->Append( CCmsContactFieldItem::ECmsEmailWork );
            iArray->Append( CCmsContactFieldItem::ECmsVoipNumberHome );
            iArray->Append( CCmsContactFieldItem::ECmsLastName );
            }
        };

    const RArray<CCmsContactFieldItem::TCmsContactField>& Fields() const 
        { 
        return *iArray; 
        };
        
    ~CCmsContactFieldInfo()
        {
        iArray->Close();
        delete iArray;
        };
    TInt iDummy;
    RArray<CCmsContactFieldItem::TCmsContactField>* iArray;
    };
   

///////////////////////////////////////////////////
// Dummy class 
// CCCAParameter
///////////////////////////////////////////////////
class RCmsSession : public CBase
    {
    public:
        RCmsSession()
            {
            };
        ~RCmsSession()
            {
            };
        TInt Connect()
            {
            return KErrNone;
            };
        void Close()
            {
            };
    };

///////////////////////////////////////////////////
// Dummy class 
// RCmsContact
///////////////////////////////////////////////////
enum TCmsContactStore
    {    
    ECmsContactStorePbk = 0,
    ECmsContactStoreSim,
    ECmsContactStoreSdn
    };

class RCmsContact : public CBase 
    {
    public:  // New functions
        
        TInt Open( RCmsSession& /*aSession*/,
                            TInt32 aContactId )
            {
            iDesDummy.AppendNum( aContactId );
            return KErrNone;
            };
        
        TInt Open( RCmsSession& /*aSession*/, 
                            const TDesC8& aPackedLink )
            {
            iDes8Dummy.Append( aPackedLink );
            return KErrNone;
            };

        TInt Open( RCmsSession& /*aSession*/,
                            const TDesC& aPhoneNbr )
            {
            iDesDummy.Append( aPhoneNbr );
            return KErrNone;
            };

        CCmsContactField* FieldDataL( 
            TRequestStatus& aStatus,
            CCmsContactFieldItem::TCmsContactField /*aFieldType*/ )
            { 
            iStatus = &aStatus;
            CCmsContactField* data = new (ELeave) CCmsContactField( PHONE );
            return data;
            };

          void FindExternalContact( TRequestStatus& aStatus )
            {
            iStatus = &aStatus;
            };
            
          CCmsContactFieldInfo* EnabledFieldsL( TRequestStatus& aStatus )
            {
            iStatus = &aStatus; 
            return CCmsContactFieldInfo::NewL();
            };
                
          void Close(){};

          void Cancel( CCmsContactBase& /*aContactBase*/ ){};
          
          void CancelExternalContactFind() const {};
          
          HBufC8* GetContactIdentifierL()
              {
              return NULL;
              };

          TBool IsServiceAvailable( VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector /*aContactAction*/ ) const
              {
              return ETrue;
              };
          
          void OrderNotificationsL( 
                      MCmsNotificationHandlerAPI* /*aHandler*/,
                      CCmsContactFieldItem::TCmsContactNotification /*aNotificationType*/ )
              {
              };
                      
          TCmsContactStore ContactStore() const
              {
              const_cast<TBool&>(iContactStore_called) = ETrue;
              };
          
          void CancelNotifications( 
                      CCmsContactFieldItem::TCmsContactNotification /*aNotificationType*/ ) {};
                     
          void DeleteL(){};
          
          ~RCmsContact(){};
          
          TInt GetContactActionFieldCount(
                  VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector /*aContactAction*/)
              {
              return ETrue; //SendReceive( ECmsGetContactActionFieldCount, TIpcArgs( aContactAction ) );
              }

    public:
                            
    TRequestStatus* iStatus;
    TBuf8<128> iDes8Dummy;                    
    TBuf<128> iDesDummy;
    
    TBool iContactStore_called;
    };    

#include "ccacmscontactfetcherwrapper.h"

#endif // __CCACLIENTHEADERS_H__
