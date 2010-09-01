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
* Description:  Triplet that contains the service id, presence status and the
*                index of the presence status icon in the listbox's icon array
*
*/



// INCLUDE FILES
#include "clogspresenceindextableentry.h"
#include "simpledebug.h"


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsPresenceIndexTableEntry* CLogsPresenceIndexTableEntry::NewL( 
    const TUint32 aServiceId, 
    const TDesC& aPresenceStatusString,
    TInt aPresenceIconIndex )
    {
    CLogsPresenceIndexTableEntry* self = CLogsPresenceIndexTableEntry::NewLC(
         aServiceId, 
         aPresenceStatusString,
         aPresenceIconIndex );
    CleanupStack::Pop( self );
    return self;    
    }
    
    
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CLogsPresenceIndexTableEntry* CLogsPresenceIndexTableEntry::NewLC( 
    const TUint32 aServiceId, 
    const TDesC& aPresenceStatusString,
    const TInt aPresenceIconIndex )
    {    
    CLogsPresenceIndexTableEntry* self = 
        new( ELeave ) CLogsPresenceIndexTableEntry( aServiceId );
    CleanupStack::PushL( self );
    self->ConstructL( aPresenceStatusString, aPresenceIconIndex );
    return self;
    }

// ---------------------------------------------------------------------------
// Symbian second phase constructor
// ---------------------------------------------------------------------------
//
void CLogsPresenceIndexTableEntry::ConstructL( 
        const TDesC& aPresenceStatusString,
        const TInt aPresenceIconIndex )
    {
    _LOG("CLogsPresenceIndexTableEntry::ConstructL: begin")
    iPresenceStatusString = aPresenceStatusString.AllocL();
    iPresenceIconIndex = aPresenceIconIndex;
    _LOG("CLogsPresenceIndexTableEntry::ConstructL: end")
    }

    
// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogsPresenceIndexTableEntry::~CLogsPresenceIndexTableEntry()
    {    
    _LOG("CLogsPresenceIndexTableEntry::~CLogsPresenceIndexTableEntry: begin")
    delete iPresenceStatusString;
    _LOG("CLogsPresenceIndexTableEntry::~CLogsPresenceIndexTableEntry: end")
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogsPresenceIndexTableEntry::CLogsPresenceIndexTableEntry(
    const TUint32 aServiceId ) : iServiceId( aServiceId )        
    {
    _LOG("CLogsPresenceIndexTableEntry::CLogsPresenceIndexTableEntry: begin")
    }


// ---------------------------------------------------------------------------
// Returns the service id of this index table entry.
// ---------------------------------------------------------------------------
//
TUint32 CLogsPresenceIndexTableEntry::ServiceId() const
    {
    _LOG("CLogsPresenceIndexTableEntry::ServiceId(): begin")
    return iServiceId;
    }


// ----------------------------------------------------------------------------
// Returns the presence status string of this index table entry.
// ----------------------------------------------------------------------------
//
TDesC& CLogsPresenceIndexTableEntry::PresenceStatusString() const
    {
    _LOG("CLogsPresenceIndexTableEntry::PresenceStatusString(): begin")    
    return *iPresenceStatusString;
    }


// ---------------------------------------------------------------------------
// Returns the presence icon index of this index table entry 
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceIndexTableEntry::PresenceIconIndex() const
    {
    return iPresenceIconIndex;
    }


// ---------------------------------------------------------------------------
// Sets the presence icon index of this index table entry 
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceIndexTableEntry::SetPresenceIconIndex( 
        TInt aPresenceIconIndex )
    {
    _LOG("CLogsPresenceIndexTableEntry::SetPresenceIconIndex: begin ...")
    _LOGP("...aPresenceIconIndex=%d", aPresenceIconIndex )
    TInt error( KErrArgument );
    if ( aPresenceIconIndex >= 0 )
        {
        iPresenceIconIndex = aPresenceIconIndex;
        error = KErrNone;
        }
    _LOGP("CLogsPresenceIndexTableEntry::SetPresenceIconIndex: end error=%d ",
        error )
    return error;
    }

