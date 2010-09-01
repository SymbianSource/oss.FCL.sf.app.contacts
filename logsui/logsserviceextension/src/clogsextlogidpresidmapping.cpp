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
* Description:  Mapping that simply contains a unique log id and a 
                 presentity id. 
*
*/



// INCLUDE FILES
#include "clogsextlogidpresidmapping.h"
#include "logwrap.h"
#include "simpledebug.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsExtLogIdPresIdMapping* CLogsExtLogIdPresIdMapping::NewL(
        const TLogId aLogId, 
        const TDesC& aPresentityId )
    {
    CLogsExtLogIdPresIdMapping* self = 
        CLogsExtLogIdPresIdMapping::NewLC( aLogId, aPresentityId );
    CleanupStack::Pop( self );
    return self;    
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsExtLogIdPresIdMapping* CLogsExtLogIdPresIdMapping::NewLC(
        const TLogId aLogId, 
        const TDesC& aPresentityId )
    {
    CLogsExtLogIdPresIdMapping* self = 
        new( ELeave ) CLogsExtLogIdPresIdMapping( aLogId );
    CleanupStack::PushL( self );
    self->ConstructL( aPresentityId );
    return self;    
    }

// ---------------------------------------------------------------------------
// Symbian Second Phase Constructor.
// ---------------------------------------------------------------------------
// 
void CLogsExtLogIdPresIdMapping::ConstructL( const TDesC& aPresentityId )
    {
    _LOG("CLogsExtLogIdPresIdMapping::ConstructL: begin")

    iPresentityId = aPresentityId.AllocL();

    _LOG("CLogsExtLogIdPresIdMapping::ConstructL: end")    
    }
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsExtLogIdPresIdMapping::~CLogsExtLogIdPresIdMapping()
    {
    _LOG("CLogsExtLogIdPresIdMapping::~CLogsExtLogIdPresIdMapping: begin")
    delete iPresentityId;    
    _LOG("CLogsExtLogIdPresIdMapping::~CLogsExtLogIdPresIdMapping: end")
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
CLogsExtLogIdPresIdMapping::CLogsExtLogIdPresIdMapping( TLogId aLogId ):
    iLogId( aLogId )
    {
    _LOG("CLogsExtLogIdPresIdMapping::CLogsExtLogIdPresIdMapping()" )    
    }


// ---------------------------------------------------------------------------
// Getter function for the log id of this mapping.
// ---------------------------------------------------------------------------
//
TLogId CLogsExtLogIdPresIdMapping::LogId()
    {
    _LOGP("CLogsExtLogIdPresIdMapping::LogId(): logId=%d",iLogId)
    return iLogId;
    }


// ---------------------------------------------------------------------------
// Getter function for the presentity id of this mapping.
// ---------------------------------------------------------------------------
//
const TDesC* CLogsExtLogIdPresIdMapping::PresentityId()
    {
    if ( iPresentityId )
        {
        _LOGDES( *iPresentityId )
        }    
    return iPresentityId;
    }
    


