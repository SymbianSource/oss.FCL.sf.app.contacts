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
* Description:  This class handles branded presence icons for phonebook contacts
*
*/


#include <e32std.h>
#include <badesca.h>
#include <flogger.h>

#include "mpresencetrafficlightsobs.h"
#include "cmsonecontactstatus.h"
#include "presencetrafficlighticons.h"
#include "presencetrafficlights.h"



// ---------------------------------------------------------------------------
// MPresenceTrafficLights::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C MPresenceTrafficLights* MPresenceTrafficLights::NewL()
    {
    return CPresenceTrafficLights::NewL();
    }

// ----------------------------------------------------------
// CPresenceTrafficLights::NewL
// 
// ----------------------------------------------------------
//
CPresenceTrafficLights* CPresenceTrafficLights::NewL()     
    {
    CPresenceTrafficLights* self = new CPresenceTrafficLights( );
    CleanupStack::PushL( self );
    self->ConstructL( KTrafficLightService, KTrafficOnline, KTrafficOffline );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CPresenceTrafficLights::New
// 
// ----------------------------------------------------------
//
void CPresenceTrafficLights::ConstructL(
    const TDesC8& aDefaultTrafficLightBrandId, 
    const TDesC8& aDefaultOnlineElementId, 
    const TDesC8& aDefaultOfflineElementId )
    {
    iDefaultTrafficLightBrandId = aDefaultTrafficLightBrandId.AllocL();
    iDefaultOnlineElementId = aDefaultOnlineElementId.AllocL();
    iDefaultOfflineElementId = aDefaultOfflineElementId.AllocL();
    }

// ----------------------------------------------------------
// CPresenceTrafficLights::CPresenceTrafficLights
// 
// ----------------------------------------------------------
//
CPresenceTrafficLights::CPresenceTrafficLights(  )
    {
    }

// ----------------------------------------------------------
// CPresenceTrafficLights::~CPresenceTrafficLights
// 
// ----------------------------------------------------------
//
CPresenceTrafficLights::~CPresenceTrafficLights()
    {
    delete iDefaultTrafficLightBrandId;
    delete iDefaultOnlineElementId;
    delete iDefaultOfflineElementId;
    
    iStatuses.ResetAndDestroy();
    iStatuses.Close();
    }

// ----------------------------------------------------------
// CPresenceTrafficLights::SubscribeBrandingForContactL
// 
// ----------------------------------------------------------
//
void CPresenceTrafficLights::SubscribeBrandingForContactL( 
    MVPbkContactLink* aContact, MDesC16Array* aIdentities, MPresenceIconNotifier* aCallback,
    TInt aId )
    {

#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CPresenceTrafficLights::SubscribeBrandingForContactL" ) );
#endif    
        
    // search first that this does not exist.
    CCmsOneContactStatus* one = FindOne( aContact, aCallback );
    if ( !one )
        {    
        one = CCmsOneContactStatus::NewL( *aCallback, aContact, 
            iDefaultTrafficLightBrandId, iDefaultOnlineElementId, iDefaultOfflineElementId, aId );
    
        iStatuses.Append( one );
        
        one->SubscribeL( aIdentities );        
        } 
    else
        {        
        }
    return;
    }

// ----------------------------------------------------------
//
void CPresenceTrafficLights::GetAllBrandingsForContactL( 
    MVPbkContactLink* aContact, MDesC16Array* aIdentities, MPresenceIconNotifier* aCallback,
    TInt aId )
    {

#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CPresenceTrafficLights::GetAllBrandingsForContactL" ) );
#endif    
        
    // search first that this does not exist.
    CCmsOneContactStatus* one = CCmsOneContactStatus::NewL( *aCallback, aContact, 
            iDefaultTrafficLightBrandId, iDefaultOnlineElementId, iDefaultOfflineElementId, aId );
    CleanupStack::PushL( one );
    
    one->GetAllL( aIdentities );
    
    CleanupStack::PopAndDestroy( one );
    return;
    }

// ----------------------------------------------------------
// CPresenceTrafficLights::UnsubscribeBrandingForContact
// 
// ----------------------------------------------------------
//
void CPresenceTrafficLights::UnsubscribeBrandingForContact( 
    MVPbkContactLink* aContact, MPresenceIconNotifier* aCallback )
    {
#ifdef _DEBUG
    CPresenceTrafficLights::WriteToLog( _L8( "CPresenceTrafficLights::UnsubscribeBrandingForContact" ) );
#endif 
              
    TInt count = iStatuses.Count();
    for ( TInt i=0; i < count; i++ )
        {       
        if ( iStatuses[i]->IsSameContact( aContact, aCallback ))
            {
            delete iStatuses[i];
            iStatuses.Remove(i);
            break;
            }
        }
    return;
    }

// ----------------------------------------------------------
// CPresenceTrafficLights::FindOne
// 
// ----------------------------------------------------------
//
CCmsOneContactStatus* CPresenceTrafficLights::FindOne( MVPbkContactLink* aContact, MPresenceIconNotifier* aCallback )
    {
    CCmsOneContactStatus* ret = NULL;
    
    if ( !aContact )
        {
        return ret;
        } 
    
    TInt count = iStatuses.Count();
    for ( TInt i=0; i < count; i++ )
        {
        if ( iStatuses[i]->IsSameContact( aContact, aCallback ) )
            {
            ret = iStatuses[i];
            break;
            }
        }
    return ret;
    }
    
#ifdef _DEBUG
// ----------------------------------------------------
// CPresenceTrafficLights::WriteToLog
// 
// ----------------------------------------------------
//
void CPresenceTrafficLights::WriteToLog( TRefByValue<const TDesC8> aFmt,... )
    {
    _LIT( KMyLogDir, "CCA");
    _LIT( KMyLogFile, "trafficlights.txt");
    const TInt KMyBufferMaxSize = 200;
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KMyBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KMyLogDir, KMyLogFile, EFileLoggingModeAppend, buf );
    // RDebug::RawPrint( buf );
    }
#endif  //_DEBUG    


