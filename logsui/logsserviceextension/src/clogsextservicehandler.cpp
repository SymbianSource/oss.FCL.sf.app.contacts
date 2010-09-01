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
* Description:  Fetches the presence status,the corresponding presence icon 
                 and its bitmask.
*
*/

// INCLUDE FILES

//for block list
#include <presenceblockinfo.h>

// presence cache
#include <presencecachereader2.h>
#include <mpresencebuddyinfo2.h>

#include "clogsextservicehandler.h"
#include "clogsextensioncchhandler.h"
#include "tlogsextutil.h"
#include "logsextconsts.h"
#include "clogsextpresentitydata.h"
#include "mlogsextservicehandlerobserver.h"

#include "simpledebug.h"


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsExtServiceHandler* CLogsExtServiceHandler::NewL(
    const TUint32 aServiceId,
    MLogsExtServiceHandlerObserver& aObserver )
    {
    _LOGP("CLogsExtServiceHandler::NewL(): begin %d", aServiceId)
    CLogsExtServiceHandler* self = 
        CLogsExtServiceHandler::NewLC( aServiceId, aObserver );
    CleanupStack::Pop( self );
    _LOG("CLogsExtServiceHandler::NewL(): end" )
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsExtServiceHandler* CLogsExtServiceHandler::NewLC(    
    const TUint32 aServiceId,
    MLogsExtServiceHandlerObserver& aObserver )
    {
    _LOG("CLogsExtServiceHandler::NewLC(): begin" )
    CLogsExtServiceHandler* self = 
        new( ELeave ) CLogsExtServiceHandler( aServiceId, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    _LOG("CLogsExtServiceHandler::NewLC(): end" )
    return self;
    }


// ---------------------------------------------------------------------------
// ~CLogsExtServiceHandler
// ---------------------------------------------------------------------------
//
CLogsExtServiceHandler::~CLogsExtServiceHandler()
    {
    _LOG("CLogsExtServiceHandler::~CLogsExtServiceHandler(): begin" )
    // Trap is easiest approach for this, deletion can occur any time
    // ( user initiated )
    TRAP_IGNORE( UnSubscribePresenceInfosL() );
    
    delete iCacheReader;
            
    iPresentityDataArray.ResetAndDestroy();
    
    _LOG("CLogsExtServiceHandler::~CLogsExtServiceHandler(): end" )
    }


// ---------------------------------------------------------------------------
// CLogsExtServiceHandler
// ---------------------------------------------------------------------------
//
CLogsExtServiceHandler::CLogsExtServiceHandler(
    const TUint32 aServiceId,
    MLogsExtServiceHandlerObserver& aObserver) : 
        iServiceId( aServiceId ),        
        iPresentityDataArray( KPresenceFetcherArrayGranularity ),
        iObserver( &aObserver )
    {
    _LOG("CLogsExtServiceHandler::CLogsExtServiceHandler(): begin" )
    
    _LOG("CLogsExtServiceHandler::CLogsExtServiceHandler(): end" )
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::ConstructL()
    {
    _LOG("CLogsExtServiceHandler::ConstructL(): begin" )
    
    iCacheReader = MPresenceCacheReader2::CreateReaderL();
    iCacheReader->SetObserverForSubscribedNotifications(this);
    
    _LOG("CLogsExtServiceHandler::ConstructL(): end" )  
    }


// ---------------------------------------------------------------------------
// ServiceId
// ---------------------------------------------------------------------------
//
TUint32 CLogsExtServiceHandler::ServiceId()
    {
    _LOG("CLogsExtServiceHandler::ServiceId(): start" )
    _LOGP("CLogsExtServiceHandler::iServiceId = %d",iServiceId )
    _LOG("CLogsExtServiceHandler::ServiceId(): finish" )
    return iServiceId;
    }


// ---------------------------------------------------------------------------
// Tests whether presentity data object exists or not.
// ---------------------------------------------------------------------------
//
TBool CLogsExtServiceHandler::PresentityDataExists( 
    const TDesC16& aPresentityId )
    {
    TBool fetcherFound( EFalse );    
    _LOG("CLogsExtServiceHandler::PresentityDataExists begin" )
    
    // check if there is already a presentity data object for the 
    // speciifed presentityid
    CLogsExtPresentityData* presData = NULL;    
    
    if ( KErrNone == GetPresentityData( aPresentityId, presData ) )
        {            
        fetcherFound = ETrue;    
        }       

    _LOGP("CLogsExtServiceHandler::PresentityDataExists %d", fetcherFound );
    
    _LOG("CLogsExtServiceHandler::PresentityDataExists end" )
    return fetcherFound;
    }


// ---------------------------------------------------------------------------
// Gets presentity data object of a certain presentity.
// ---------------------------------------------------------------------------
//
TInt CLogsExtServiceHandler::GetPresentityData(
        const TDesC& aPresentityId,
        CLogsExtPresentityData*& aPresentityData )
    {
    _LOG("CLogsExtServiceHandler::GetPresentityData begin" )
    
    TInt error( KErrNotFound );
    TBool dataFound( EFalse );
         
    // check if there is already a fetcher for the brand
    CLogsExtPresentityData* presData = NULL;
    
    for ( TInt i = 0 ; i < iPresentityDataArray.Count() && !dataFound; i++ )
        {
        presData = iPresentityDataArray[i];
        if ( presData && aPresentityId == presData->PresentityId() )
            {
            dataFound = ETrue;
            aPresentityData = presData;
            }
        }

    error = dataFound ? KErrNone : KErrNotFound;
    _LOGP("CLogsExtServiceHandler::GetPresentityData() error=%d",error )

    _LOG("CLogsExtServiceHandler::GetPresentityData end" )
    return error;
    }


//---------------------------------------------------------------------------
// Adds the specified presentity to this service handler and starts to 
// handle subscription/event handling etc. for this presentity.
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::AddPresentityL( const TDesC& aPresentityId )
    {
    _LOG("CLogsExtServiceHandler::HandlePresentityL begin" )
    
    //add presentityData if it not already exists,
    //and start handling the presentity (make subscription if
    //needed and so on)
    DoPresentityDataCreationL( aPresentityId );
    
    _LOG("CLogsExtServiceHandler::HandlePresentityL end" )
    }


// ---------------------------------------------------------------------------
// CLogsExtServiceHandler::HandlePresenceReadL()
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::HandlePresenceReadL(TInt /*aErrorCode*/,
        RPointerArray<MPresenceBuddyInfo2>&  /*aPresenceBuddyInfoList*/)
    {
    //intentionally not implemented
    }
    
	
// ---------------------------------------------------------------------------
// CLogsExtServiceHandler::HandlePresenceNotificationL()
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::HandlePresenceNotificationL( TInt aErrorCode,
    MPresenceBuddyInfo2* aPresenceBuddyInfo )
    {
    if( aErrorCode == KErrNone && aPresenceBuddyInfo )
        {
        CleanupDeletePushL( aPresenceBuddyInfo );
        TBool showStatus = EFalse;
        TInt index = KErrNotFound;
        MPresenceBuddyInfo2::TAvailabilityValues value = aPresenceBuddyInfo->Availability();
        TPtrC presTextValue = aPresenceBuddyInfo->AvailabilityText();
        TPtrC buddyId = aPresenceBuddyInfo->BuddyId();
        
        // Presence Cache gives only buddy id and in shown presenties array
        // is only presentity id... so find index of that presentity which
        // buddy id is in buddyinfo object for checking shown presentity
        // from shown presentities array.
        for ( TInt j = 0 ; j < iPresentityDataArray.Count(); j++ )
                { 
                CLogsExtPresentityData* presData = iPresentityDataArray[j];
                if ( buddyId == presData->PresentityId() )
                    {
                    index = j;
                    break;
                    }
                }
        
        if( index != KErrNotFound )
            {
            showStatus = ETrue;

            if( value == MPresenceBuddyInfo2::EAvailable )
                {
                _LIT( KConvAvailable, "available" );
                presTextValue.Set( KConvAvailable );    
                }
            
            else if ( value == MPresenceBuddyInfo2::ENotAvailable /*presTextValue == KConvUnavailable*/ )
                {
                // Presence offline value is modified to match branding
                _LIT( KConvOffline, "offline" );
                presTextValue.Set( KConvOffline );
                }

            iObserver->HandlePresentityPresenceStatusL(
                                                iServiceId,
                                                iPresentityDataArray[index]->PresentityId(),                                            
                                                presTextValue,
                                                showStatus );
            }

        CleanupStack::PopAndDestroy( aPresenceBuddyInfo );
        }
    }


// ---------------------------------------------------------------------------
// Create presentity data object for a presentity.
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::DoPresentityDataCreationL( 
        const TDesC& aPresentityId )
    {
    _LOG("CLogsExtServiceHandler::DoPresentityDataCreationL begin" )
    
    if ( !PresentityDataExists( aPresentityId ) )
        {
        CLogsExtPresentityData* presData = 
            CLogsExtPresentityData::NewLC( aPresentityId, *this );
        iPresentityDataArray.AppendL( presData );
        CleanupStack::Pop( presData );
        // check the current state, make subscriptions if necessary, etc.
        CheckServiceHandler();
        }    
    
    _LOG("CLogsExtServiceHandler::DoPresentityDataCreationL end" )
    }


// ---------------------------------------------------------------------------
// Subscribe presentity presence
// ---------------------------------------------------------------------------
//    
void CLogsExtServiceHandler::DoPresentitySubscriptionL( 
        CLogsExtPresentityData& aPresentityData )
    {
    _LOG("CLogsExtServiceHandler::DoPresentitySubscriptionL(): begin" )    

    const TDesC& presentityId = aPresentityData.PresentityId();
    
    // Watch for changes
    TInt err = iCacheReader->SubscribePresenceBuddyChangeL( presentityId );
    
    // Synch check
    MPresenceBuddyInfo2* buddy = iCacheReader->PresenceInfoLC( presentityId );
    aPresentityData.SetStatus( CLogsExtPresentityData::ESubscribed );
    if ( buddy )
        {
        // Ownership is transferred
        CleanupStack::Pop(); // buddy
        HandlePresenceNotificationL( KErrNone, buddy );
        }
    
    _LOG("CLogsExtServiceHandler::DoPresentitySubscriptionL(): end")         
    }


// ---------------------------------------------------------------------------
// CLogsExtServiceHandler::UnSubscribePresenceInfosL
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::UnSubscribePresenceInfosL()
    {
    TInt contactsCount = iPresentityDataArray.Count();
    TPtrC subUri(KNullDesC);
    // go through the array of presentityData objects 
    CLogsExtPresentityData* presData = NULL;
        
    for(TInt i=0;i<contactsCount;i++)
        {
        presData = iPresentityDataArray[i];
            if ( presData )
                {
                iCacheReader->UnSubscribePresenceBuddyChangeL( presData->PresentityId() );
                }
         }  
    }

// ---------------------------------------------------------------------------
// Determines whether the service handler has to make subscriptions
// and also initiates those. It also informs the service handler 
// oberver in case the status of a presentity should not be displayed
// (e.g. because the presentity is not found from the subscribed buddy
// list anymore)
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::CheckServiceHandler()
    {
    _LOG("CLogsExtServiceHandler::CheckServiceHandler() begin" )
    
    _LOG("CLogsExtServiceHandler::CheckServiceHandler() make check!" )
    TRAPD( error , DoCheckServiceHandlerL(); );
    if ( KErrNone != error )
        {
        _LOGP("CLogsExtServiceHandler::CheckServiceHandler()ERROR! = %d",
            error )
        }     
    
    _LOG("CLogsExtServiceHandler::CheckServiceHandler() end" )
    }


// ---------------------------------------------------------------------------
// DoCheckServiceHandlerL
// ---------------------------------------------------------------------------
//
void CLogsExtServiceHandler::DoCheckServiceHandlerL()
    {
    _LOG("CLogsExtServiceHandler::DoCheckServiceHandlerL() begin" )
    
    // go through the array of presentityData objects 
    CLogsExtPresentityData* presData = NULL;
    for ( TInt i(0); i < iPresentityDataArray.Count(); i++ )
        {        
        presData = iPresentityDataArray[i];
        if ( presData )
            {
            TInt state = presData->Status(); 
            
            _LOGP("CLogsExtServiceHandler::DoCheckServiceHandlerL():state=%d",
                state )

            // subscribe those presentities which are:
            // 1) not subcribed
            if ( state == CLogsExtPresentityData::EUnsubscribed )
                {
                _LOG("CLogsExtServiceHandler::DoCheckServiceHandlerL()...")
                _LOG("...DoPresentitySubscriptionL( *presData )" )
                DoPresentitySubscriptionL( *presData );
                }                  
            }        
        }           
    
    _LOG("CLogsExtServiceHandler::DoCheckServiceHandlerL() end" )
    }

//End of file
