/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles presence related tasks for branding
*
*/


#include <bamdesca.h>
#include <utf.h>

#include <contactpresencebrandids.h> 
#include "bpasheaders.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CBPAS::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CBPAS* CBPAS::NewL()
    {
    CBPAS* self = new( ELeave ) CBPAS( NULL );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
   
// ---------------------------------------------------------------------------
// CBPAS::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CBPAS* CBPAS::NewL( MBPASObserver* aObserver )
    {
    CBPAS* self = new( ELeave ) CBPAS( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CBPAS::CBPAS
// ---------------------------------------------------------------------------
//
CBPAS::CBPAS( MBPASObserver* aObserver ): CActive( CActive::EPriorityStandard ),
iObserver ( aObserver ),
iState( CBPAS::EBPASIdle ),
iSchedulerWait( NULL ),
iSettings( NULL )
    {
    CActiveScheduler::Add( this );
    }
   
// ---------------------------------------------------------------------------
// CBPAS::~CBPAS
// ---------------------------------------------------------------------------
//
CBPAS::~CBPAS()
    {
    Cancel();
    if ( iContacts )
        {
        iContacts->Reset();
        delete iContacts;
        }
    delete iSchedulerWait;
    delete iSettings;
    delete iCacheReader;
    
    iObserver = NULL;
    
    if(iSPNames)
        {
        iSPNames->Reset();
        delete iSPNames;
        }
        
    iSPIds.Close();
    }

// ---------------------------------------------------------------------------
// CBPAS::ConstructL
// ---------------------------------------------------------------------------
//
void CBPAS::ConstructL( )
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ConstructL"));

    iContacts = new (ELeave) CPtrCArray(5);
    iSchedulerWait = new (ELeave)CActiveSchedulerWait();                            
    iSettings = CSPSettings::NewL();
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS:: Settings created"));
    iCacheReader = MPresenceCacheReader2::CreateReaderL();
    iCacheReader->SetObserverForSubscribedNotifications(this);
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS:: Cache created"));
    //Needed for AknIconUtils
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ConstructL: Done."));
    
    iSPNames = new (ELeave) CDesC16ArraySeg(10);
    ReadSPSettingsToLocalStoresL();
    }

// ---------------------------------------------------------------------------
// CBPAS::SetObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CBPAS::SetObserver( MBPASObserver* aObserver )
    {
    iObserver = aObserver;
    }
        
// ---------------------------------------------------------------------------
// CBPAS::GiveInfosL
// ---------------------------------------------------------------------------
//
EXPORT_C void CBPAS::GiveInfosL( MDesCArray& aContacts, 
                          RPointerArray<CBPASInfo>& aInfos )
    {            
    iContacts->CopyL( aContacts );           
    CreateInfosL( aInfos );
    }

// ---------------------------------------------------------------------------
// CBPAS::GiveInfosL
// ---------------------------------------------------------------------------
//
EXPORT_C void CBPAS::GiveInfosL( MDesCArray& aContacts )
    {
    if ( IsActive() )
        {
        User::Leave( KErrInUse );
        }
    if ( iObserver )
        {
        iContacts->CopyL( aContacts );
        iState = CBPAS::EBPASProcessing;
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        iSchedulerWait->Start();
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    return;    
    }
    
// ---------------------------------------------------------------------------
// CBPAS::CreateInfosL
// ---------------------------------------------------------------------------
//
void CBPAS::CreateInfosL( RPointerArray<CBPASInfo>& aInfos )
    {
    // creating aInfos
    // presence
    SetPresenceInfosL( aInfos );    
    
    // service data
    SetServiceInfosL( aInfos );
    
    // branded data
    SetIconInfosL( aInfos );
    }
                          
// ---------------------------------------------------------------------------
// CBPAS::GivePresenceInfosL
// ---------------------------------------------------------------------------
//
void CBPAS::GivePresenceInfosL( MDesCArray& aContacts,
                                   RPointerArray<CBPASPresenceInfo>& aInfos )
    {
    ReadPresenceInfosL( aContacts, aInfos );
    }
   
// ---------------------------------------------------------------------------
// CBPAS::GiveIconInfoL
// ---------------------------------------------------------------------------
//
CBPASIconInfo* CBPAS::GiveIconInfoL( const TDesC8& aImageId, 
                                     const TDesC8& aTextId )
    {
    CBPASIconInfo* iconInfo = CBPASIconInfo::NewL(aImageId);
    iconInfo->SetTextElementIdL(aTextId);
    return iconInfo;
    }
                                   
   
// ---------------------------------------------------------------------------
// CBPAS::CreateElementIdLC
// ---------------------------------------------------------------------------
//  
HBufC8* CBPAS::CreateElementIdLC( EBPASPresenceType aPresenceType,
                                  EBPASElementType aElementType,
                                  const TDesC& aAttributeValue )
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS:: CreateElementIdLC start") ); 
    HBufC8* elementId = NULL;
    TInt length = aAttributeValue.Length(); 
    TPtrC8 presence;
    TPtrC8 element;
    switch ( aPresenceType )
        {
        case EBPASPersonPresence:
            {
            length += KCPBrandElementIdPerson().Length();
            presence.Set( KCPBrandElementIdPerson );
            break;
            }
        case EBPASDevicePresence:
            {
            length += KCPBrandElementIdDevice().Length();
            presence.Set( KCPBrandElementIdDevice );
            break;
            }
        case EBPASServicePresence:
            {
            length += KCPBrandElementIdService().Length();
            presence.Set( KCPBrandElementIdService );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }

    switch ( aElementType )
        {
        case EBPASElementImage:
            {
            length += KCPBrandElementIdImage().Length();
            element.Set( KCPBrandElementIdImage );
            break;
            }
        case EBPASElementText:
            {
            length += KCPBrandElementIdText().Length();
            element.Set( KCPBrandElementIdText );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            }
        }
    elementId = HBufC8::NewLC( length ); // << elementId

    TPtr8 elementIdPtr = elementId->Des();
    elementIdPtr.Append( presence );
    elementIdPtr.Append( aAttributeValue );
    elementIdPtr.Append( element );
    elementIdPtr.LowerCase();   
    
#ifdef _DEBUG
	HBufC8* buf = HBufC8::NewLC ( elementIdPtr.Length ());
	buf->Des().Copy ( elementIdPtr );
	TPtrC8 numberDesc( buf->Des ());
    CMS_DP(KBPasLoggerFile, CMS_L8( "CBPAS:: CreateElementIdLC    %S" ), &numberDesc );
	CleanupStack::PopAndDestroy (); //buf
#endif

 
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS:: CreateElementIdLC end") );   
    return elementId;
    }
   
// ---------------------------------------------------------------------------
// CBPAS::RunL
// ---------------------------------------------------------------------------
//
void CBPAS::RunL()
    {
    switch ( iState )
        {
        case CBPAS::EBPASProcessing:
            {
            RPointerArray<CBPASInfo> infos;
            CleanupClosePushL( infos ); // << infos
            TInt err( KErrNone );
            TRAP( err, CreateInfosL( infos ) );
            if ( KErrNone != err )
                {
                CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::RunL CreateInfosL error=%d"), err );
                }
            TRAP( err, iObserver->HandleInfosL(KErrNone, infos) );
            if ( KErrNone != err )
                {
                CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::RunL HandleInfosL error=%d"), err );       
                }
            iState = CBPAS::EBPASIdle;
            CleanupStack::Pop();        // >> infos
            infos.ResetAndDestroy();
            iSchedulerWait->AsyncStop();
            break;
            }
        default:
            {
            break;
            }
        }
    }       
       
// ---------------------------------------------------------------------------
// CBPAS::RunError
// ---------------------------------------------------------------------------
//
TInt CBPAS::RunError( TInt aError )
    {
    return aError;
    }
   
// ---------------------------------------------------------------------------
// CBPAS::DoCancel
// ---------------------------------------------------------------------------
//
void CBPAS::DoCancel()
    {
    }
    
// ---------------------------------------------------------------------------
// CBPAS::ReadPresenceInfosL
// Gets the presence info from presence cache and extracts the supported 
// attributes. The following attributes are supported:
//  - Availability
// ---------------------------------------------------------------------------
//
void CBPAS::ReadPresenceInfosL( MDesCArray& aContacts,
                                RPointerArray<CBPASPresenceInfo>& aInfos )
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadPresenceInfosL start"));
    TInt count( aContacts.MdcaCount() );
    for ( TInt i(0); i < count; i++ )
        {
        ReadAndCreatePresenceInfoL( aContacts.MdcaPoint(i), aInfos );
        }
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadPresenceInfosL end"));
    }

// ---------------------------------------------------------------------------
// CBPAS::ReadServiceDataLC
// ---------------------------------------------------------------------------
//
CBPASServiceInfo* CBPAS::ReadServiceDataLC(TInt aServiceId)
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadServiceDataLC start") );
    TInt err(KErrNone);
    
    // note that service id is already verified by caller
    CBPASServiceInfo* serviceInfo = CBPASServiceInfo::NewLC(aServiceId);
    
    // Now we are searching for other properties and if found we will set them
    // to our serviceInfo
    
    // Read ServiceType
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadServiceDataLC read serviceType") ); 
    CSPProperty* serviceType = CSPProperty::NewLC();   // << serviceType   
    err = iSettings->FindPropertyL( aServiceId, EPropertyServiceAttributeMask, *serviceType );
    TInt type( KErrNotFound );
    TInt errInType(KErrNone);
    if ( KErrNone == err )
        { 
        errInType = serviceType->GetValue( type );
        CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadServiceDataLC serviceType value = %d, err = %d"), type, errInType );
        if(errInType == KErrNone)
            serviceInfo->SetServiceType(type);        
        }
    CleanupStack::PopAndDestroy( serviceType ); // >>> serviceType
    
    // Read brand id
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadServiceDataLC read brandId") );
    CSPProperty* brandId = CSPProperty::NewLC();   // << brandId   
    err = iSettings->FindPropertyL( aServiceId, EPropertyBrandId, *brandId );
    if ( KErrNone == err )
        {
        RBuf brandValue;
        brandValue.Create( KCPBrandElementIdMaxLength );
        CleanupClosePushL( brandValue ); // << brandValue
        
        RBuf8 brandValue8;
        brandValue8.Create( KCPBrandElementIdMaxLength );
        CleanupClosePushL( brandValue8 ); // << brandValue

        err = brandId->GetValue( brandValue );
        if(err == KErrNone)
            {
            CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadServiceDataLC create service info") );
            err = CnvUtfConverter::ConvertFromUnicodeToUtf8(brandValue8, brandValue);
            if(err==KErrNone)
                serviceInfo->SetBrandIdL(brandValue8);
            }
        CleanupStack::PopAndDestroy(); // >>> brandValue8
        CleanupStack::PopAndDestroy(); // >>> brandValue
        }
    CleanupStack::PopAndDestroy( brandId ); // >>> brandId
    
    
    //Read language ID
    CSPProperty* langId = CSPProperty::NewLC();   // << langId   
    err = iSettings->FindPropertyL( aServiceId, EPropertyBrandLanguage, *langId );
    if ( KErrNone == err)
        {
        RBuf langValue;
        langValue.Create( KCPBrandElementIdMaxLength );
        CleanupClosePushL( langValue ); // << langValue
        err = langId->GetValue( langValue );
        CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadServiceDataLC create language ID err = %d"), err );
        
        // converting language id to integer
        TInt langId;
        TLex lex( langValue );
        err = lex.Val(langId);

        if(err==KErrNone)
            serviceInfo->SetLanguageId(langId);
        CleanupStack::PopAndDestroy(); // >>> langValue              
        }
    CleanupStack::PopAndDestroy( langId ); // >>> langId
    
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::ReadServiceDataLC end serviceInfo = %d"), serviceInfo );
    return serviceInfo;
    }

// ---------------------------------------------------------------------------
// CBPAS::SetServiceInfosL
// ---------------------------------------------------------------------------
//
void CBPAS::SetServiceInfosL( RPointerArray<CBPASInfo>& aInfos )
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::SetServiceInfosL start") );
    TInt infoCount( aInfos.Count() );
    CBPASServiceInfo* serviceInfo(NULL);
    TInt serviceId(KErrNotFound);
    for( TInt i(0); i < infoCount; i++ )
        {
        // create service info only if service id is found from sp table
        if(GetServiceId(serviceId, (aInfos[i])->Identity()))
            {
            serviceInfo = ReadServiceDataLC(serviceId);
            
            if(serviceInfo) // if service info exists
                {
                (aInfos[i])->SetServiceInfoL(serviceInfo);
                CleanupStack::Pop(serviceInfo); // ownership transferred
                serviceInfo = NULL;
                }
            }
        }
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::SetServiceInfosL end") );
    }

   
// ---------------------------------------------------------------------------
// CBPAS::SetPresenceInfosL
// ---------------------------------------------------------------------------
//
void CBPAS::SetPresenceInfosL( RPointerArray<CBPASInfo>& aInfos )
    {
    RPointerArray<CBPASPresenceInfo> presInfos;
    CleanupClosePushL( presInfos ); // << presInfos
    ReadPresenceInfosL( *iContacts, presInfos );
    CBPASInfo* info(NULL);
    
    TInt presInfoCount( presInfos.Count() );
    for ( TInt i( presInfoCount - 1 ); 0 <= i; i-- )
        {
         info = CBPASInfo::NewLC(iContacts->MdcaPoint(i));   // << info
         info->SetPresenceInfoL( presInfos[i] );
         aInfos.Append(info);
         CleanupStack::Pop( info );              // >> info
         info = NULL;
         }
        
    CleanupStack::Pop(); // >> presInfos
    presInfos.Close(); // ownerships are transfered
    }
    
// ---------------------------------------------------------------------------
// CBPAS::SetIconInfosL
// ---------------------------------------------------------------------------
//
void CBPAS::SetIconInfosL( RPointerArray<CBPASInfo>& aInfos )
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::SetIconInfosL start") );
    TInt infoCount( aInfos.Count() );
    CBPASIconInfo* iconInfo(NULL);
    for( TInt i(0); i < infoCount; i++ )
        {
        
        // process only if service and presence infos are valid
        if( (aInfos[i])->HasPresence()  && (aInfos[i])->ServiceInfo() )
            {
            TPtrC attrValue = aInfos[i]->PresenceInfo()->AvailabilityText();
            
            // element id
            // NOTE Hardcoded EBPASPersonPresence here is ugly
            HBufC8* imageId =  CreateElementIdLC( EBPASPersonPresence, // << imageId
                                                  EBPASElementImage,
                                                  attrValue );
                                                  
            HBufC8* textId =  CreateElementIdLC( EBPASPersonPresence, // << textId
                              EBPASElementText,
                              attrValue );
            
            // image and text ids must have been created other leave would have happen
            iconInfo = CBPASIconInfo::NewLC(*imageId);
            iconInfo->SetTextElementIdL(*textId);
            aInfos[i]->SetIconInfoL( iconInfo ); //ownership tranfered
            CleanupStack::Pop(iconInfo);
            CleanupStack::PopAndDestroy(textId);
            CleanupStack::PopAndDestroy(imageId);   
            }
        }
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::SetIconInfosL end") );
    }
         
       
// ---------------------------------------------------------------------------
// CBPAS::ReadAndCreatePresenceInfoL
// ---------------------------------------------------------------------------
//    
void CBPAS::ReadAndCreatePresenceInfoL( const TDesC& aContact,
                                        RPointerArray<CBPASPresenceInfo>& aInfos )
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::ReadAndCreatePresenceInfoL start"));
    TInt serviceId(KErrNotFound);

    MPresenceBuddyInfo2* presInfo(NULL);    
    
    if (aContact.Length() == 0)
        User::Leave(KErrArgument);
    
    // get the service id from xsp prefix
    if (GetServiceId(serviceId, aContact))
        {
        //service id is found by xsp id      
        presInfo = iCacheReader->PresenceInfoLC(aContact);        
        }
    
    CMS_DP(KBPasLoggerFile, CMS_L( "________CBPAS::ReadPresenceInfosL presInfo = %d"),presInfo);
  
    CBPASPresenceInfo* PresBpasInfo(NULL);
    
    if (presInfo)
        { 
        MPresenceBuddyInfo2::TAvailabilityValues value = presInfo->Availability();
        TPtrC presTextValue = presInfo->AvailabilityText();
        if ( value != MPresenceBuddyInfo2::EUnknownAvailability)
            {
            PresBpasInfo = CBPASPresenceInfo::NewLC(presTextValue, value);
            }
        else
            {
            // nothing
            }
        }
    // append NULL presence info if its not found, just to keep compatiblity with current code
    aInfos.Append( PresBpasInfo );
    
    if (PresBpasInfo)
        CleanupStack::Pop(PresBpasInfo);// ownership transfered
    if (presInfo)
        CleanupStack::PopAndDestroy(); // >>>  presInfo

    
    CMS_DP(KBPasLoggerFile, CMS_L( "_____CBPAS::ReadAndCreatePresenceInfoL end"));
    }


// ---------------------------------------------------------------------------
// CBPAS::SubscribePresenceInfosL
// ---------------------------------------------------------------------------
//
EXPORT_C void CBPAS::SubscribePresenceInfosL( MDesCArray& aContacts)
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::SubscribePresenceInfosL: start."));
    TInt contactsCount = aContacts.MdcaCount();
    CMS_DP(KBPasLoggerFile, CMS_L( "____contactsCount=%d"), contactsCount );
    TInt ignoredErr(KErrNone);
    TInt serviceId(KErrNotFound);
    
    for (TInt i=0;i<contactsCount;i++)
        {
        if (GetServiceId(serviceId, aContacts.MdcaPoint(i)))
            {
            iCacheReader->SubscribePresenceBuddyChangeL( aContacts.MdcaPoint(i) );
            }
        }   
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::SubscribePresenceInfosL: Done ignoredErr(%d)"),ignoredErr);    
    }
    
// ---------------------------------------------------------------------------
// CBPAS::UnSubscribePresenceInfosL
// ---------------------------------------------------------------------------
//
EXPORT_C void CBPAS::UnSubscribePresenceInfosL( MDesCArray& aContacts)
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::UnSubscribePresenceInfosL: start."));
    TInt contactsCount = aContacts.MdcaCount();
    CMS_DP(KBPasLoggerFile, CMS_L( "____contactsCount=%d"), contactsCount );
    TInt serviceId(KErrNotFound);
    TPtrC subUri(KNullDesC);
    
    for(TInt i=0;i<contactsCount;i++)
        {
        if (GetServiceId(serviceId, aContacts.MdcaPoint(i)))
            {
            iCacheReader->UnSubscribePresenceBuddyChangeL( aContacts.MdcaPoint(i) );
            }
         }  
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::UnSubscribePresenceInfosL: Done."));
    }

// ---------------------------------------------------------------------------
// CBPAS::GetServiceId
// ---------------------------------------------------------------------------
//
TBool CBPAS::GetServiceId(TInt& aServiceId, TPtrC aUri)
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::GetServiceId: start"));
    TBool ret(EFalse);
    _LIT(KColon, ":");
    TInt pos = aUri.Find(KColon);
    if (pos>0) // if colon found and there is something before colon, i.e. xsp id
        {
        aServiceId = XspToServiceId(aUri.Left(pos));
        if (aServiceId != KErrNotFound) // if service id found
            {
            ret = ETrue;
            }
        else
            {
            // Service not found
            }
        }
        
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::GetServiceId: end (%d)"), ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CBPAS::XspToServiceId
// ---------------------------------------------------------------------------
//
TInt CBPAS::XspToServiceId(TPtrC aXsp)
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::XspToServiceIds: start"));
    TInt pos, ret(KErrNotFound);
    TInt count = iSPNames->Count();
    
    if(count)
        {
        iSPNames->Find(aXsp, pos);
        if(pos<count) // if xsp id found
            ret = iSPIds[pos]; // return the service id at that location
        }
    
    if(ret==KErrNotFound) //update local spsettings and try again
        {
        TRAP_IGNORE( ReadSPSettingsToLocalStoresL() );
        count = iSPNames->Count();
        if(count)
            {
            iSPNames->Find(aXsp, pos);
            if(pos<count) // if xsp id found
                ret = iSPIds[pos]; // return the service id at that location
            }
        }
    
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::XspToServiceIds id(%d): Done"),ret);    
    return ret;    
    }

// ---------------------------------------------------------------------------
// CBPAS::VerifyServiceIdL
// ---------------------------------------------------------------------------
//
TInt CBPAS::VerifyServiceIdL(TInt aServiceId)
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::VerifyServiceId: start"));
    
    TInt index = iSPIds.Find(aServiceId);
    
    //give another try if service not found, update local sp stores
    if(index==KErrNotFound)
        {
        CMS_DP(KBPasLoggerFile, CMS_L( "______Trying again!"));
        ReadSPSettingsToLocalStoresL();
        index = iSPIds.Find(aServiceId);
        }
    
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::VerifyServiceId: end"));
    return index;
    }

// ---------------------------------------------------------------------------
// CBPAS::ReadSPSettingsToLocalStoresL()
// ---------------------------------------------------------------------------
//
void CBPAS::ReadSPSettingsToLocalStoresL()
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::ReadSPSettingsToLocalStoresL: start"));
    
    iSPIds.Close();
    iSPNames->Reset();

    TInt ignoredError = iSettings->FindServiceIdsL(iSPIds);
    ignoredError =  iSettings->FindServiceNamesL(iSPIds, *iSPNames);
    
    CMS_DP(KBPasLoggerFile, CMS_L( "____CBPAS::ReadSPSettingsToLocalStoresL: end"));
    }
    
// ---------------------------------------------------------------------------
// CBPAS::HandlePresenceReadL()
// ---------------------------------------------------------------------------
//
void CBPAS::HandlePresenceReadL(TInt /*aErrorCode*/,
        RPointerArray<MPresenceBuddyInfo2>&  /*aPresenceBuddyInfoList*/)
    {
    //intentionally not implemented
    return;
    }
    
// ---------------------------------------------------------------------------
// CBPAS::HandlePresenceNotificationL()
// ---------------------------------------------------------------------------
//
void CBPAS::HandlePresenceNotificationL( TInt aErrorCode,
    MPresenceBuddyInfo2* aPresenceBuddyInfo )
    {
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::HandlePresenceNotificationL: start"));
    
    if( (aErrorCode == KErrNone) && (aPresenceBuddyInfo) )
                                    
        {
        CleanupStack::PushL(aPresenceBuddyInfo);
        
        RPointerArray<CBPASInfo> infos;
        CleanupClosePushL(infos);
        CBPASInfo* info = CBPASInfo::NewLC(aPresenceBuddyInfo->BuddyId());
        infos.Append(info); //ownership transfered
        CleanupStack::Pop(info); 
        
        MPresenceBuddyInfo2::TAvailabilityValues value = aPresenceBuddyInfo->Availability();
        TPtrC presTextValue = aPresenceBuddyInfo->AvailabilityText(); 
        CBPASPresenceInfo* PresInfo = CBPASPresenceInfo::NewL( presTextValue, value );
        CleanupStack::PushL(PresInfo);
        info->SetPresenceInfoL(PresInfo);
        CleanupStack::Pop(PresInfo);// ownership transfered        
                                      
        // Make service info
        SetServiceInfosL(infos);  // RPointerArray<CBPASInfo>& aInfos
        
        // make icon infos only if service info is available
        if(info->ServiceInfo())
            SetIconInfosL(infos);

        if(iObserver)
            iObserver->HandleSubscribedInfoL(info);//ownership not transferred
        delete info;    
        CleanupStack::Pop(1); //infos
        infos.Close();
        delete aPresenceBuddyInfo;
        CleanupStack::Pop(1); //aPresenceBuddyInfo    
        }
    CMS_DP(KBPasLoggerFile, CMS_L( "CBPAS::HandlePresenceNotificationL: end"));
    }

    
// end of file


