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

#include "cmsoneservicestatus.h" 

// ----------------------------------------------------------
// CCmsOneServiceStatus::NewL
// 
// ----------------------------------------------------------
//
CCmsOneServiceStatus* CCmsOneServiceStatus::NewL( 
    const TDesC& aService,        
    const TDesC8& aBrandId,
    TInt aBrandLanguage,
    TBool aIsAvailable )   
    {
    CCmsOneServiceStatus* self = new CCmsOneServiceStatus( aIsAvailable, aBrandLanguage );
    CleanupStack::PushL( self );
    self->ConstructL( aService, aBrandId );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::New
// 
// ----------------------------------------------------------
//
void CCmsOneServiceStatus::ConstructL( 
    const TDesC& aService, const TDesC8& aBrandId )
    {
    iService = aService.AllocL();
    iBrandId = aBrandId.AllocL(); 
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::CCmsOneServiceStatus
// 
// ----------------------------------------------------------
//
CCmsOneServiceStatus::CCmsOneServiceStatus( TBool aIsAvailable, TInt aBrandLanguage )
    : iPresenceAvailable( aIsAvailable ),
    iBrandLanguage(aBrandLanguage)
    {
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::~CCmsOneServiceStatus
// 
// ----------------------------------------------------------
//
CCmsOneServiceStatus::~CCmsOneServiceStatus()
    {
    delete iService;
    iService = NULL;    
    delete iBrandId;
    iBrandId = NULL;
    delete iElementId;
    iElementId = NULL;
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::SetAvailability
// 
// ----------------------------------------------------------
//
void CCmsOneServiceStatus::SetAvailability( TBool aIsAvailable )
    {
    iPresenceAvailable = aIsAvailable;
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::Availability
// 
// ----------------------------------------------------------
//
TBool CCmsOneServiceStatus::Availability( )
    {
    return iPresenceAvailable;
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::IsSameService
// 
// ----------------------------------------------------------
//
TBool CCmsOneServiceStatus::IsSameService( const TDesC& aService )
    {
    TBool ret(EFalse);
    if ( !aService.CompareF( ServiceName() ))
        {
        ret = ETrue;
        }
    return ret;
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::BrandId
// ----------------------------------------------------------
//
TPtrC8 CCmsOneServiceStatus::BrandId( )
    {
    return iBrandId? iBrandId->Des() : TPtrC8();
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::Service
// 
// ----------------------------------------------------------
//
TPtrC CCmsOneServiceStatus::ServiceName( )
    {
    return iService? iService->Des() : TPtrC();
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::SetElementL
// 
// ----------------------------------------------------------
//
void CCmsOneServiceStatus::SetElementL( const TDesC8& aElementId )
    {
    delete iElementId;
    iElementId = NULL;
    iElementId = aElementId.AllocL();
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::ElementId
// 
// ----------------------------------------------------------
//
TPtrC8 CCmsOneServiceStatus::ElementId( )
    {
    return iElementId? iElementId->Des() : TPtrC8();
    }

// ----------------------------------------------------------
// CCmsOneServiceStatus::BrandLanguage
// 
// ----------------------------------------------------------
//
TInt CCmsOneServiceStatus::BrandLanguage()
    {
    return iBrandLanguage;
    }
