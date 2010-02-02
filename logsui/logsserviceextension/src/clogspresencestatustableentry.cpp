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
* Description:  Triplet that contains the service id, presentity id and the
*                presence status.
*
*/



// INCLUDE FILES
#include "clogspresencestatustableentry.h"
#include "simpledebug.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsPresenceStatusTableEntry* CLogsPresenceStatusTableEntry::NewL(
        const TUint32 aServiceId, 
        const TDesC& aPresentityId,
        TBool aDisplayStatus, 
        TDesC* aPresenceStatusString )
    {
    CLogsPresenceStatusTableEntry* self = 
        CLogsPresenceStatusTableEntry::NewLC(
            aServiceId,
            aPresentityId,
            aDisplayStatus,
            aPresenceStatusString );
    CleanupStack::Pop( self );
    return self;    
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsPresenceStatusTableEntry* CLogsPresenceStatusTableEntry::NewLC(
        const TUint32 aServiceId, 
        const TDesC& aPresentityId,
        TBool aDisplayStatus,
        TDesC* aPresenceStatusString )
    {
    CLogsPresenceStatusTableEntry* self = 
        new( ELeave ) CLogsPresenceStatusTableEntry( aServiceId, 
                                                     aDisplayStatus );
    CleanupStack::PushL( self );
    self->ConstructL( aPresentityId, aPresenceStatusString );
    return self;    
    }


// ---------------------------------------------------------------------------
// Symbian Second Phase Constructor.
// ---------------------------------------------------------------------------
// 
void CLogsPresenceStatusTableEntry::ConstructL( const TDesC& aPresentityId,
                                                TDesC* aPresenceStatusString )
    {
    _LOG("CLogsPresenceStatusTableEntry::ConstructL: begin")
    iPresentityId = aPresentityId.AllocL();
    if ( aPresenceStatusString )
        {
        iPresenceStatusString = aPresenceStatusString->AllocL();
        }
    else
        {
        iPresenceStatusString = NULL;
        }
    _LOG("CLogsPresenceStatusTableEntry::ConstructL: end")    
    }
    
    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsPresenceStatusTableEntry::~CLogsPresenceStatusTableEntry()
    {
    _LOG("CLogsPresenceStatusTableEntry::~CLogsPresenceStatusTableEntry: beg")
    delete iPresentityId;
    delete iPresenceStatusString;
    _LOG("CLogsPresenceStatusTableEntry::~CLogsPresenceStatusTableEntry: end")
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//    
CLogsPresenceStatusTableEntry::CLogsPresenceStatusTableEntry( 
        const TUint32 aServiceId, TBool aDisplayStatus ): 
            iServiceId( aServiceId ), 
            iDisplayStatus( aDisplayStatus )
    {
    _LOGP("CLogsPresenceStatusTableEntry::CLogsPresenceStatusTableEntry=%d",
        iServiceId)
    }


// ---------------------------------------------------------------------------
// Returns the service id of this status table entry
// ---------------------------------------------------------------------------
//
TUint32 CLogsPresenceStatusTableEntry::ServiceId() const
    {
    _LOGP("CLogsPresenceStatusTableEntry::ServiceId(): serviceId=%d",
        iServiceId)
    return iServiceId;
    }


// ---------------------------------------------------------------------------
// Returns the presense display status of this status table entry
// ---------------------------------------------------------------------------
//
TBool CLogsPresenceStatusTableEntry::DisplayPresence()
    {
    _LOG("CLogsPresenceStatusTableEntry::DisplayPresence(): ...")
    _LOGP("...iDisplayStatus = %d ", (TInt)iDisplayStatus )
    return iDisplayStatus;
    }


// ---------------------------------------------------------------------------
// Sets presense display status of this status table entry
// ---------------------------------------------------------------------------
//
void CLogsPresenceStatusTableEntry::SetDisplayPresence(
          TBool aDisplayStatus )
    {
    _LOG("CLogsPresenceStatusTableEntry::SetDisplayPresence():...")
    _LOGP("... aDisplayStatus = %d ", (TInt)aDisplayStatus )
    
    iDisplayStatus = aDisplayStatus;
    }


// ---------------------------------------------------------------------------
// Returns the presentity id of this status table entry.
// ---------------------------------------------------------------------------
//
const TDesC& CLogsPresenceStatusTableEntry::PresentityId()
    {
    if ( iPresentityId )
        {        
        _LOGDES(*iPresentityId)
        return *iPresentityId;
        }            
    return KNullDesC();
    }
    
    
// ---------------------------------------------------------------------------
// Returns the presence status of this status table entry.
// ---------------------------------------------------------------------------
//    
TDesC* CLogsPresenceStatusTableEntry::PresenceStatusString()
    {   
    if ( iPresenceStatusString )
        {        
        _LOGDES(*iPresenceStatusString)        
        }
    return iPresenceStatusString;
    }
 
 
// ---------------------------------------------------------------------------
// Sets the presence status string of this status table entry.
// ---------------------------------------------------------------------------
//    
TInt CLogsPresenceStatusTableEntry::SetPresenceStatusString( 
        const TDesC& aPresenceStatusString )
    {    
    _LOG("CLogsPresenceStatusTableEntry::SetPresenceStatusStringL(): begin")
    _LOGDES(aPresenceStatusString)
    
    TInt error( KErrGeneral );
        
    delete iPresenceStatusString;
    iPresenceStatusString = NULL;
    iPresenceStatusString = aPresenceStatusString.Alloc();    
    
    if ( iPresenceStatusString )
        {
        error = KErrNone;        
        _LOGDES(*iPresenceStatusString)
        }
    else
        {
        error = KErrNoMemory;
        _LOG("CLogsPresenceStatusTableEntry::SetPresenceStatusStringL():...")
        _LOG("... creation of descriptor failed!")
        }    
    _LOG("CLogsPresenceStatusTableEntry::SetPresenceStatusStringL(): end")
    return error;
    }


// ---------------------------------------------------------------------------
// Resets the presence status string of this status table entry.
// ---------------------------------------------------------------------------
//
void CLogsPresenceStatusTableEntry::ResetPresenceStatusString()
    {
    _LOG("CLogsPresenceStatusTableEntry::ResetPresenceStatusString(): begin")
    
    delete iPresenceStatusString;
    iPresenceStatusString = NULL;    
    
    _LOG("CLogsPresenceStatusTableEntry::ResetPresenceStatusString(): end")
    }

