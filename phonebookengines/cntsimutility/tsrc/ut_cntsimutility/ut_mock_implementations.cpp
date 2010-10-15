/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QObject>
#include <qdebug.h>
#include <qtcontacts.h>

#include <mmtsy_names.h>
#include "ut_cntsimutility.h"
#include "cntsimutility.h"
 #include "centralrepository.h"
#include <etelmm.h>
#include <secui.h>
#include <simutils.h>
#include <e32property.h>
RTelServer::RTelServer(  )
    {
    
    }
TInt RTelServer::Connect( 
        TInt aMessageSlots )
    {
    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        return KErrGeneral;
        }
    return KErrNone;
    }


TInt RTelServer::EnumeratePhones( 
        TInt & aNoOfPhones ) const
    {

    }

TInt RTelServer::GetPhoneInfo( 
        const TInt aIndex,
        TPhoneInfo & aInfo ) const
    {

    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        return KErrGeneral;
        }
    return KErrNone;
    }


TInt RTelServer::GetTsyName( 
        const TInt aIndexOfPhone,
        TDes & aTsyName ) const
    {
    }



TInt RTelServer::LoadPhoneModule( 
        const TDesC & aFileName ) const
    {

    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        return KErrGeneral;
        }
    return KErrNone;
    }


TInt RTelServer::UnloadPhoneModule( 
        const TDesC & aFileName ) const
    {
    }


TInt RTelServer::IsSupportedByModule( 
        const TDesC & aTsyName,
        const TInt aMixin,
        TBool & aResult ) const
    {
    
    }


TInt RTelServer::GetTsyVersionNumber( 
        const TDesC & aTsyName,
        TVersion & aVersion ) const
    {
    
    }



TInt RTelServer::SetPriorityClient(  ) const
    {
    
    }


TInt RTelServer::SetExtendedErrorGranularity( 
        const TErrorGranularity aGranularity ) const
    {
   
    }
TInt RTelServer::__DbgMarkHeap(  )
    {
  
    }

TInt RTelServer::__DbgCheckHeap( 
        TInt aCount )
    {
    
    }

TInt RTelServer::__DbgMarkEnd( 
        TInt aCount )
    {
    
    }
TInt RTelServer::__DbgFailNext( 
        TInt aCount )
    {
    
    }
TInt RTelServer::__DbgFailNextAvailable( 
        TBool & aResult )
    {
    
    }
TInt RTelServer::SetPriorityClientV2(  ) const
    {
    
    }
RMobilePhone::RMobilePhone()
    {
    
    
    }
TInt RMobilePhone::GetMultimodeCaps( 
        TUint32 & aCaps ) const
    {
   
    }
void RMobilePhone::SetFdnSetting(TRequestStatus& aReqStatus, TMobilePhoneFdnSetting aFdnSetting) const
    {
        TRequestStatus* req = & aReqStatus;
        if(true == TestCntSimUtility::returnErrorInStub_1)
            {
            User::RequestComplete(req, KErrGeneral);
            }
        else
            {
            User::RequestComplete(req, KErrNone);    
            }
        

    
    }
void RMobilePhone::NotifyFdnStatusChange(TRequestStatus& aReqStatus, TMobilePhoneFdnStatus& aFdnStatus) const
    {
    
    }

TInt RMobilePhone::GetCurrentMode( 
        TMobilePhoneNetworkMode & aNetworkMode ) const
    {
   
    }


TInt RMobilePhone::GetIccAccessCaps(TUint32 &acaps) const
    {
    if(true == TestCntSimUtility::returnErrorInStub_2)
           {
        return  KErrGeneral;
           }
    acaps = RMobilePhone::KCapsUSimAccessSupported;
    return  KErrNone;
    }
void RMobilePhone::GetServiceTable(TRequestStatus& aReqStatus, TMobilePhoneServiceTable aTable, TDes8& aTableData) const
    {

    TRequestStatus* req = &aReqStatus;
    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        User::RequestComplete(req, KErrGeneral);
        }
    else
        {
        User::RequestComplete(req, KErrNone);    
        }

    }
TInt RMobilePhone::GetFdnStatus(TMobilePhoneFdnStatus& aFdnStatus) const
{
    if(false == TestCntSimUtility::returnErrorInStub_1)
        {
        aFdnStatus = RMobilePhone::EFdnActive ;
        return KErrNone;
    
        }

    aFdnStatus = RMobilePhone::EFdnNotActive ;
    return KErrGeneral;
}
void RMobilePhone::GetFdnStatus(TRequestStatus& aReqStatus, TMobilePhoneFdnStatus& aFdnStatus) const
{

}

void RMobilePhone::GetSignalStrength( 
        TRequestStatus & aReqStatus,
        TInt32 & aSignalStrength,
        TInt8 & aBar ) const
    {
   
    }


void RMobilePhone::NotifySignalStrengthChange( 
        TRequestStatus & aReqStatus,
        TInt32 & aSignalStrength,
        TInt8 & aBar ) const
    {
    }
void RMobilePhone::GetSubscriberId(TRequestStatus& aReqStatus, TMobilePhoneSubscriberId& aId) const
    {

    TRequestStatus* req = & aReqStatus;
    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        User::RequestComplete(req, KErrGeneral);
        }
    else
        {
        User::RequestComplete(req, KErrNone);    
        }

    }
void RMobilePhone::ConstructL()
    {
    
    }
void RMobilePhone::Destruct()
    {
    
    }

RMobilePhoneBookStore::RMobilePhoneBookStore ()
    {
    
    }

void RMobilePhoneBookStore::Close ()
    {
    
    }

RMobilePhoneBookStore::TMobilePhoneBookInfoV5::TMobilePhoneBookInfoV5()
    {
    
    }
RMobilePhone::TMobilePhoneServiceTableV1::TMobilePhoneServiceTableV1()
    {
    
    }
RMmCustomAPI::RMmCustomAPI()
    {
    
    }
CSecuritySettings::CSecuritySettings()
            {

            }
CSecuritySettings::~CSecuritySettings()
            {

            }
CSecuritySettings* CSecuritySettings::NewL()
    {
    CSecuritySettings* self = new( ELeave ) CSecuritySettings;
        CleanupStack::PushL( self );
        //self->ConstructL()
        CleanupStack::Pop( self );
        return self;
    }
TBool CSecuritySettings::AskPin2L()
    {
    return ETrue;
    }
// ----



CRepository * CRepository::NewL( 
        TUid aRepositoryUid )
    {
    

    CRepository* self = new( ELeave ) CRepository;
    CleanupStack::PushL( self );
    //self->ConstructL()
    CleanupStack::Pop( self );
    return self;
    }



CRepository * CRepository::NewLC( 
        TUid aRepositoryUid )
    {
   

    CRepository* self = new( ELeave ) CRepository;
    CleanupStack::PushL( self );
    //self->ConstructL()
    return self;
    }



CRepository::~CRepository(  )
    {
    
    }

TInt CRepository::Get( 
        TUint32 aKey,
        TInt & aValue )
    {
    if(true == TestCntSimUtility::returnErrorInStub_1)
           {
           return KErrGeneral;
           }
       return KErrNone;
    
    }
TInt CRepository::Get(TUint32 aKey,TDes16 &aValue)
    {
    if(true == TestCntSimUtility::returnErrorInStub_1)
           {
           return KErrGeneral;
           }
       return KErrNone;
    }

TInt CRepository::Set(TUint32 aKey, const TDesC16& aValue)
    {
    if(true == TestCntSimUtility::returnErrorInStub_1)
           {
           return KErrGeneral;
           }
       return KErrNone;
    }
TInt CRepository::Set(TUint32 aKey, TInt aValue)
    {
    if(true == TestCntSimUtility::returnErrorInStub_1)
           {
           return KErrGeneral;
           }
       return KErrNone;
    }

TInt RProperty::Get(TUid aCategory, TUint aKey, TInt& aValue)
    {
    if(true == TestCntSimUtility::returnErrorInStub_1)
             {
             return KErrGeneral;
             aValue = ESimNotSupported;
             
             }
    aValue = ESimUsable;
    return KErrNone;
    }
TSecUi::TSecUi(  )
    {
    
    }


void TSecUi::ConstructL(  )
    {
    
    }



void TSecUi::InitializeLibL(  )
    {
   
    }



void TSecUi::UnInitializeLib(  )
    {
    }



TBool TSecUi::CanBeFreed(  )
    {
    }



void TSecUi::IncreaseClientCount(  )
    {
    }


void TSecUi::DecreaseClientCount(  )
    {
    }


RPhone::RPhone()
    {
    
      
}
void RPhone::Close()
    {
    
    }
void RPhone::ConstructL()
    {
    
    }

void RPhone::Destruct()
    {
    
    }
RMobilePhoneStore::RMobilePhoneStore()
    {
    
    
    }

void RMobilePhoneStore::GetInfo(TRequestStatus& aReqStatus, TDes8& aInfo) const
    {
    TRequestStatus* req = & aReqStatus;
    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        User::RequestComplete(req, KErrGeneral);
        }
    else
        {
        User::RequestComplete(req, KErrNone);    
        }
    

    
    
    }
void RMmCustomAPI::GetPndCacheStatus( TRequestStatus& aStatus,
            RMmCustomAPI::TPndCacheStatus& aPndStatus,
            const TName& aPndName ) const
    {
    TRequestStatus* req = & aStatus;
    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        aPndStatus == RMmCustomAPI::ECacheNotReady;
        User::RequestComplete(req, KErrGeneral);
        }
    else
        {
        aPndStatus = RMmCustomAPI::ECacheReady; 
        User::RequestComplete(req, KErrNone);    
        }
    }
void RMmCustomAPI::NotifyPndCacheReady( TRequestStatus& aStatus,
        TName& aPndName )
    {
    
    }
int RMmCustomAPI::Open(RMobilePhone& aPhone)
    {

    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        return KErrGeneral;
        }
    return KErrNone;
    }
RMobilePhoneBookStore::TMobilePhoneBookInfoV2::TMobilePhoneBookInfoV2()
    {
    
    }
RMobilePhoneBookStore::TMobilePhoneBookInfoV1::TMobilePhoneBookInfoV1()
        {
        
        }
RMobilePhoneBookStore::TMobilePhoneStoreInfoV1::TMobilePhoneStoreInfoV1()
    {
    
    }

RTelSubSessionBase::RTelSubSessionBase()
    {
    
    }
RMobilePhone::TMultimodeType::TMultimodeType()
    {
    
    }
TInt RMobilePhoneBookStore::Open(RMobilePhone & aPhone, const TDesC& aStore)
    {

    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        return KErrGeneral;
        }
    return KErrNone;
    }
TInt RPhone::Open(RTelServer& aServer, TDesC16 const & aDes)
    {

    if(true == TestCntSimUtility::returnErrorInStub_1)
        {
        return KErrGeneral;
        }
    return KErrNone;
    }
