/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class handles branded presence icons for phonebook contact
*
*/


#include <e32std.h>
#include <MVPbkContactLink.h>
#include <bamdesca.h>
#include <badesca.h>

#include "mpresencetrafficlightsobs.h"

#include "presencetrafficlights.h"
#include "cmsonecontactstatus.h" 
#include "bpas.h"
#include "bpasinfo.h"
#include "bpasobserver.h"
#include "bpaspresenceinfo.h"
#include "bpasserviceinfo.h"
#include "bpasiconinfo.h"

#include "cmsoneservicestatus.h"

_LIT(KColon, ":");

const TInt KTrafficLightLimit = 2;


// ----------------------------------------------------------
// CCmsOneContactStatus::NewL
// 
// ----------------------------------------------------------
//
CCmsOneContactStatus* CCmsOneContactStatus::NewL( MPresenceIconNotifier& aCallback,
        MVPbkContactLink* aContact,            
        HBufC8* aDefaultTrafficLightBrandId, 
        HBufC8* aDefaultOnlineElementId, 
        HBufC8* aDefaultOfflineElementId,
        TInt aId )        
    {
    CCmsOneContactStatus* self = new CCmsOneContactStatus( 
        aCallback, aDefaultTrafficLightBrandId, aDefaultOnlineElementId, aDefaultOfflineElementId, aId );
    CleanupStack::PushL( self );
    self->ConstructL( aContact );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CCmsOneContactStatus::New
// 
// ----------------------------------------------------------
//
void CCmsOneContactStatus::ConstructL( MVPbkContactLink* aContact )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::ConstructL this=%d" ), (TInt)this );
#endif      
    iBrandedPresence = CBPAS::NewL();
    iBrandedPresence->SetObserver( this );
    iContactLink = aContact->CloneLC();
    CleanupStack::Pop(); // CloneLC
    }

// ----------------------------------------------------------
// CCmsOneContactStatus::CCmsOneContactStatus
// 
// ----------------------------------------------------------
//
CCmsOneContactStatus::CCmsOneContactStatus( MPresenceIconNotifier& aCallback,
        HBufC8* aDefaultTrafficLightBrandId, 
        HBufC8* aDefaultOnlineElementId, 
        HBufC8* aDefaultOfflineElementId,
        TInt aId  )
    : iCallback( aCallback ), iId(aId)
    {
    iDefaultTrafficLightBrandId = aDefaultTrafficLightBrandId;
    iDefaultOnlineElementId = aDefaultOnlineElementId;
    iDefaultOfflineElementId = aDefaultOfflineElementId;
    iAvailableServices = 0;
    iOnlineServices = 0;       
    }

// ----------------------------------------------------------
// CCmsOneContactStatus::~CCmsOneContactStatus
// 
// ----------------------------------------------------------
//
CCmsOneContactStatus::~CCmsOneContactStatus()
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::~CCmsOneContactStatus this=%d" ), (TInt)this );
#endif    
    delete iContactLink;
    iContactLink = NULL;
    delete iBrandedPresence;
    iBrandedPresence = NULL;
    delete iLastElement;
    iLastElement = NULL;
    delete iLastService;
    iLastService = NULL;    
    iServices.ResetAndDestroy();
    iServices.Close();    
    }

// ----------------------------------------------------------
// CCmsOneContactStatus::SubscribeL
// 
// ----------------------------------------------------------
//
void CCmsOneContactStatus::SubscribeL( MDesC16Array* aIdentities )
    {  
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::SubscribeL" ) );
#endif    

    RPointerArray<CBPASInfo> myInfos;
    CleanupClosePushL( myInfos );
    
    iBrandedPresence->SubscribePresenceInfosL( *aIdentities );        
    iBrandedPresence->GiveInfosL( *aIdentities, myInfos );          
    DoHandleInfosL( myInfos, ETrue, EFalse );    
    
    CleanupStack::PopAndDestroy(); // myInfos        
    }

// ----------------------------------------------------------
//
void CCmsOneContactStatus::GetAllL( MDesC16Array* aIdentities )
    {  
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::GetAllL" ) );
#endif    

    RPointerArray<CBPASInfo> myInfos;
    CleanupClosePushL( myInfos );
           
    iBrandedPresence->GiveInfosL( *aIdentities, myInfos );          
    DoHandleInfosL( myInfos, ETrue, ETrue );    
    
    CleanupStack::PopAndDestroy(); // myInfos        
    }

// ----------------------------------------------------------
//
TBool CCmsOneContactStatus::IsSameContact( MVPbkContactLink* aLink, MPresenceIconNotifier* aCallback )
    {
    TBool ret(EFalse);
    if ( aCallback == &iCallback && iContactLink->IsSame( *aLink ))
        {
        ret = ETrue;
        }
    return ret;
    }

// ----------------------------------------------------------
//
void CCmsOneContactStatus::HandleInfosL( TInt /*aErrorCode*/, RPointerArray<CBPASInfo>& aInfos )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::HandleInfosL" ) );
#endif    
    DoHandleInfosL( aInfos, EFalse, EFalse );  
    }

// ----------------------------------------------------------
//
void CCmsOneContactStatus::HandleSubscribedInfoL(CBPASInfo* aInfo)
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::HandleSubscribedInfoL" ) );        
#endif      
    RPointerArray<CBPASInfo> tempArray;
    
    tempArray.Append( aInfo );    
    TRAP_IGNORE( DoHandleInfosL( tempArray, EFalse, EFalse ) );     
    tempArray.Close();    
    }       


// --------------------------------------------------------------------------
//

void CCmsOneContactStatus::DoHandleInfosL( RPointerArray<CBPASInfo>& aInfos, TBool aDelete, TBool aGetAllOnce )
    {    
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoHandleInfosL this=%d" ), (TInt)this );
#endif       
    TInt count = aInfos.Count();
    for ( TInt i=0; i<count; i++ )
        {
        TRAP_IGNORE( DoHandleInfoL(aInfos[i]));
        if ( aDelete )
            {
            delete aInfos[i];
            }
        else
            {            
            }
        }    
    UpdateStatusL( aGetAllOnce );    
    }

// --------------------------------------------------------------------------
//
void CCmsOneContactStatus::DoHandleInfoL( CBPASInfo* aInfo )
    { 
    MPresenceBuddyInfo2::TAvailabilityValues newValue = MPresenceBuddyInfo2::ENotAvailable;   
    CBPASServiceInfo* service = aInfo->ServiceInfo();    
    TPtrC identity = aInfo->Identity();     
    TPtrC serviceName = ExtractServiceL( identity );
    CCmsOneServiceStatus* serviceStatus = FindService( identity );     
    CBPASPresenceInfo* presence = aInfo->PresenceInfo();
    if ( presence )
        {
        newValue = presence->AvailabilityEnum( );        
#ifdef _DEBUG
        const TInt debugBufSize = 60;
        CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoHandleInfoL presence found this=%d" ), (TInt)this );        
        TBuf8<debugBufSize> serviceBuf;
        TBuf8<debugBufSize> identityBuf;
        serviceBuf.Copy( serviceName );
        identityBuf.Copy( identity );
        CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoHandleInfoL srv=%S identity=%S avail=%d" ), &serviceBuf, &identityBuf, newValue );          
#endif          
        TBool nowAvailable = (newValue < MPresenceBuddyInfo2::EBusy) ? EFalse : ETrue;            
        if ( !serviceStatus )
            {
            serviceStatus = CCmsOneServiceStatus::NewL( identity, service->BrandId(),          
							service->LanguageId(), nowAvailable );   
            iServices.Append( serviceStatus );
            }
        else
            {
            serviceStatus->SetAvailability( nowAvailable );
            }
        CBPASIconInfo* icon = aInfo->IconInfo(); 
        if ( icon )
            {
            serviceStatus->SetElementL( icon->ImageElementId() );             
            }
        else
            {             
            } 
        if ( newValue == MPresenceBuddyInfo2::EUnknownAvailability )
            {
#ifdef _DEBUG
            CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoHandleInfoL presence not found this=%d" ), (TInt)this );
#endif         
            // service is closed and we delete the corresponding service entity        
            RemoveService( serviceStatus );            
            }
        }
    else
        {
        // nothing to do
        }   
    }

// --------------------------------------------------------------------------
//
TPtrC CCmsOneContactStatus::ExtractServiceL( const TDesC& aXspId )
    {         
    TPtrC ret;
    TInt pos = aXspId.Find(KColon);
    if ( pos >= 0)
        {
        // ok input
        ret.Set( aXspId.Left(pos));
        }
    else
        {
       User::Leave( KErrArgument );
        }
    return ret;
    }

// --------------------------------------------------------------------------
//
CCmsOneServiceStatus* CCmsOneContactStatus::FindService( const TDesC& aService )
    {
    CCmsOneServiceStatus* currentService = NULL;          
    TInt counter = iServices.Count();
    for ( TInt i=0; i<counter; i++ )
        {
        currentService = iServices[i];
        if ( currentService->IsSameService( aService ))
            {
            return currentService;            
            }
        }
    return NULL;    
    }

// --------------------------------------------------------------------------
//
void CCmsOneContactStatus::RemoveService( CCmsOneServiceStatus* aService )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::RemoveService this=%d" ), (TInt)this );       
#endif      
    CCmsOneServiceStatus* currentService = NULL;          
    TInt counter = iServices.Count();
    for ( TInt i=0; i<counter; i++ )
        {
        currentService = iServices[i];
        if ( currentService == aService )
            {
            delete iServices[i];    
            iServices.Remove(i);
            break;
            }
        }
    return;
    }

// --------------------------------------------------------------------------
//
void CCmsOneContactStatus::UpdateStatusL( TBool aGetAllOnce )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::UpdateStatusL this=%d" ), (TInt)this );       
#endif    
    
    if ( aGetAllOnce )
        {
        UpdateAllAtOnceStatus();
        }
    else
        {
        UpdateCommonStatusL();
        }
     
    return;
    }

// --------------------------------------------------------------------------
//
void CCmsOneContactStatus::UpdateCommonStatusL( )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::UpdateCommonStatusL this=%d" ), (TInt)this );       
#endif                   
    CCmsOneServiceStatus* currentService = NULL;
    CCmsOneServiceStatus* onlineService = NULL;
    TInt onlineServiceCount = 0;    
    
    // Count how many are online now and compare to the previous situation.
    // If only one service is online then that icon is used in certain cases.
    TInt counter = iServices.Count();
    for ( TInt i=0; i<counter; i++ )
        {
        currentService = iServices[i];      
        onlineServiceCount+= currentService->Availability();
        if ( currentService->Availability() )
            {
            onlineService = currentService;
            }
        } 
    
    iAvailableServices = counter;
    iOnlineServices = onlineServiceCount;
    
    SendNotificationWhenNeededL( onlineService ?  onlineService : currentService );
     
    return;
    }


// --------------------------------------------------------------------------
//
void CCmsOneContactStatus::UpdateAllAtOnceStatus( )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::UpdateAllAtOnceStatus this=%d" ), (TInt)this ); 
#endif      
    
    RPointerArray<MPresenceServiceIconInfo> callbackArray;    
    CCmsOneServiceStatus* currentService = NULL;   
    
    // Add all available service statuses
    TInt counter = iServices.Count();
    for ( TInt i=0; i<counter; i++ )
        {
        currentService = iServices[i];      
        callbackArray.Append( currentService );
        } 
    
    iCallback.NewIconsForContact( 
            iContactLink,
            callbackArray,
            iId );
    
    callbackArray.Close();
     
    return;         
    }

// --------------------------------------------------------------------------
//
void CCmsOneContactStatus::SendNotificationWhenNeededL( CCmsOneServiceStatus* aOnlyStatus )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::SendNotificationWhenNeededL this=%d" ), (TInt)this );       
#endif
    TPtrC8 brandPtr(KNullDesC8);
    TPtrC8 elementPtr(KNullDesC8);
    TInt brandLanguage (KErrNotFound );
    if ( !iAvailableServices )
        {
#ifdef _DEBUG
        CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus:: NO SERVICES this=%d" ), (TInt)this );       
#endif         
        }
    else if (( 1 == iAvailableServices ) || (1 == iOnlineServices ))
        {
#ifdef _DEBUG
        CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus:: SERVICE ICON this=%d" ), (TInt)this );       
#endif         
        brandPtr.Set( aOnlyStatus->BrandId() );
        elementPtr.Set( aOnlyStatus->ElementId() );          
        brandLanguage = aOnlyStatus->BrandLanguage();
        }
    else if ( iOnlineServices >= KTrafficLightLimit )
        {
#ifdef _DEBUG
        CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus:: COMMON TRAFFIC LIGHTS ON this=%d" ), (TInt)this );       
#endif        
        brandPtr.Set( TrafficLightService() );
        elementPtr.Set( TrafficLightOnline() );
        }
    else
        {
#ifdef _DEBUG
        CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus:: COMMON TRAFFIC LIGHTS OFF this=%d" ), (TInt)this );       
#endif         
        brandPtr.Set( TrafficLightService() );
        elementPtr.Set( TrafficLightOffline() );
        }    
    
    if ( brandPtr.CompareF( LastBrand() ) || elementPtr.CompareF( LastElement() ))
    	{        
        DoSendNotificationL( brandPtr, elementPtr, brandLanguage );
    	}
    
    return;
    }

// --------------------------------------------------------------------------
//
void CCmsOneContactStatus::DoSendNotificationL( const TDesC8& aService, const TDesC8& aElement,
                        TInt aBrandLanguage )
	{
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoSendNotificationL begins this=%d" ), (TInt)this );
#endif	
	delete iLastElement;
	iLastElement = NULL;
	iLastElement = aElement.AllocL();
	
	delete iLastService;
	iLastService = NULL;	
	iLastService = aService.AllocL();
        	
    iCallback.NewIconForContact( 
            iContactLink, 
            aService, 
            aElement,
            iId,
            aBrandLanguage );
    
#ifdef _DEBUG
    const TInt KMyBufferSize = 60;
    TBuf8<KMyBufferSize> serviceBuf;
    TBuf8<KMyBufferSize> elementBuf; 
    serviceBuf.Copy( aService );
    elementBuf.Copy( aElement );
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoSendNotificationL     srv=%S" ), 
            &serviceBuf );
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoSendNotificationL     element=%S" ), 
            &elementBuf );
    CPresenceTrafficLights::WriteToLog( _L8( "CCmsOneContactStatus::DoSendNotificationL ends this=%d" ), 
            (TInt)this );    
#endif      
	}

// --------------------------------------------------------------------------
//
TPtrC8 CCmsOneContactStatus::LastBrand( )
	{	
	return iLastService ? iLastService->Des() : TPtrC8();
	}

// --------------------------------------------------------------------------
//
TPtrC8 CCmsOneContactStatus::LastElement( )
	{	
	return iLastElement ? iLastElement->Des() : TPtrC8();
	}


// --------------------------------------------------------------------------
//
TPtrC8 CCmsOneContactStatus::TrafficLightService()
    {
    return iDefaultTrafficLightBrandId ? iDefaultTrafficLightBrandId->Des() : TPtrC8();
    }

// --------------------------------------------------------------------------
//
TPtrC8 CCmsOneContactStatus::TrafficLightOnline()
    {
    return iDefaultOnlineElementId ? iDefaultOnlineElementId->Des() : TPtrC8();
    }

// --------------------------------------------------------------------------
//
TPtrC8 CCmsOneContactStatus::TrafficLightOffline()
    {
    return iDefaultOfflineElementId ? iDefaultOfflineElementId->Des() : TPtrC8();
    }
 
