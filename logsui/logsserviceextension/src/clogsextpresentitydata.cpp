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

// Presence
#include <ximpclient.h>
#include <ximpcontext.h>
#include <presenceobjectfactory.h>
#include <ximpobjectfactory.h>
#include <presencefeatures.h>
#include <presencewatching.h>
#include <ximprequestcompleteevent.h>
#include <ximpcontextstateevent.h>
#include <presentitypresenceevent.h>
#include <ximpidentity.h>
#include <ximpstatus.h>
#include <presenceinfofilter.h>
#include <presenceinfo.h>
#include <personpresenceinfo.h>
#include <presenceinfofieldcollection.h>
#include <presenceinfofield.h>
#include <presenceinfofieldvaluetext.h>
//group related
#include <presentitygroups.h>
#include <presentitygroupcontentevent.h>
#include <presentitygroupinfo.h>
#include <presentitygroupmemberinfo.h>
#include <presentitygrouplistevent.h>
#include <protocolpresentitygroups.h>

#include "clogsextpresentitydata.h"
#include "tlogsextutil.h"
#include "logsextconsts.h"
#include "clogsextservicehandler.h"

#include "simpledebug.h"



// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsExtPresentityData* CLogsExtPresentityData::NewL(
    const TDesC& aPresentityId,
    CLogsExtServiceHandler& aServiceHandler )
    {
    _LOG("CLogsExtPresentityData::NewL(): begin ")
    CLogsExtPresentityData* self = 
        CLogsExtPresentityData::NewLC( aPresentityId, aServiceHandler );
    CleanupStack::Pop( self );
    _LOG("CLogsExtPresentityData::NewL(): end" )
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsExtPresentityData* CLogsExtPresentityData::NewLC(    
    const TDesC& aPresentityId,
    CLogsExtServiceHandler& aServiceHandler )
    {
    _LOG("CLogsExtPresentityData::NewLC(): begin" )
    CLogsExtPresentityData* self = 
        new( ELeave ) CLogsExtPresentityData( aServiceHandler );
    CleanupStack::PushL( self );
    self->ConstructL( aPresentityId );
    _LOG("CLogsExtPresentityData::NewLC(): end" )
    return self;
    }


// ---------------------------------------------------------------------------
// ~CLogsExtPresentityData
// ---------------------------------------------------------------------------
//
CLogsExtPresentityData::~CLogsExtPresentityData()
    {
    _LOG("CLogsExtPresentityData::~CLogsExtPresentityData(): begin" )
    delete iPresentityId;
    _LOG("CLogsExtPresentityData::~CLogsExtPresentityData(): end" )    
    }

// ---------------------------------------------------------------------------
// CLogsExtPresentityData
// ---------------------------------------------------------------------------
//
CLogsExtPresentityData::CLogsExtPresentityData(
    CLogsExtServiceHandler& aServiceHandler ) : 
        iState( EUnsubscribed ),
        iServiceHandler( aServiceHandler )
    {
    _LOG("CLogsExtPresentityData::CLogsExtPresentityData(): begin" )
    _LOG("CLogsExtPresentityData::CLogsExtPresentityData(): end" )
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityData::ConstructL( const TDesC& aPresentityId )
    {
    _LOG("CLogsExtPresentityData::ConstructL(): begin" )

    iPresentitySubscriptionRequestId = TXIMPRequestId::Null();
    iPresentityId = aPresentityId.AllocL();
           
    _LOG("CLogsExtPresentityData::ConstructL(): end" )
    }


// ---------------------------------------------------------------------------
// PresentityId
// ---------------------------------------------------------------------------
//
const TDesC& CLogsExtPresentityData::PresentityId()
    {
    if( iPresentityId )
        {
        _LOGDES( *iPresentityId )         
        return *iPresentityId;
        }        
    return KNullDesC();
    }


// ---------------------------------------------------------------------------
// State of this presentity.
// ---------------------------------------------------------------------------
//
TInt CLogsExtPresentityData::Status()
    {
    return iState;
    }


// ---------------------------------------------------------------------------
// Sets the State of this presentity.
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityData::SetStatus( TPresentityDataState aState )
    {
    _LOG("CLogsExtPresentityData::SetStatus(): begin")
    iState = aState;
    _LOGP("CLogsExtPresentityData::SetStatus(): end new status=%d", iState )
    }


// ---------------------------------------------------------------------------
// Returns the request id of the subscription request.
// ---------------------------------------------------------------------------
//
TXIMPRequestId CLogsExtPresentityData::PresentitySubscriptionRequestId()
    {
    return iPresentitySubscriptionRequestId;
    }


// ---------------------------------------------------------------------------
// Sets the request id of the subscription request.
// ---------------------------------------------------------------------------
//
void CLogsExtPresentityData::SetPresentitySubscriptionRequestId( 
        TXIMPRequestId aRequestId )
    {
    iPresentitySubscriptionRequestId = aRequestId;
    }
