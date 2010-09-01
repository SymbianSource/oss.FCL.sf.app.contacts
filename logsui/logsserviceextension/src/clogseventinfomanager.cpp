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
* Description:  Used to manage TLogsEventInfo objects
*
*/



// INCLUDE FILES
#include "clogseventinfomanager.h"
#include "tlogseventinfo.h"
#include "logsextconsts.h"
#include "simpledebug.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsEventInfoManager* CLogsEventInfoManager::NewL()
    {
    CLogsEventInfoManager* self = CLogsEventInfoManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
CLogsEventInfoManager* CLogsEventInfoManager::NewLC()
    {
    CLogsEventInfoManager* self = new( ELeave ) CLogsEventInfoManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Descructor
// ---------------------------------------------------------------------------
//        
CLogsEventInfoManager::~CLogsEventInfoManager()
    {
    _LOG("CLogsEventInfoManager::~CLogsEventInfoManager(): begin")
    iEventInfoArray.ResetAndDestroy();    
    _LOG("CLogsEventInfoManager::~CLogsEventInfoManager(): end")
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogsEventInfoManager::CLogsEventInfoManager()
    : iEventInfoArray( KEventInfoManagerArrayGranularity )
    {    
    }    


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CLogsEventInfoManager::ConstructL()
    {
    _LOG("CLogsEventInfoManager::ConstructL(): begin")    
    _LOG("CLogsEventInfoManager::ConstructL(): end")
    }


// ---------------------------------------------------------------------------
// Adds TLogsEventInfo entry to this manager.Manager takes ownership of the
// entry. Before adding the entry it is checked whether the entry already 
// exists to avoid duplicates. Entries with the same service id are considered
// to be duplicates.
// ---------------------------------------------------------------------------
//
void CLogsEventInfoManager::AddEventEntryL( TUint32 aServiceId )
    {
    _LOG("CLogsEventInfoManager::AddEventEntryL(): begin")
        
    if ( !EntryExists ( aServiceId ) )
        {
        // Pc-lint warning: #429: Custodial pointer 'eventInfo'has not been 
        // freed or returned.
        // Explanation: pointer ownership transferred to the array or deleted
        // if transfer not successful
        TLogsEventInfo* eventInfo = 
                            new( ELeave ) TLogsEventInfo( aServiceId );

        TInt error = iEventInfoArray.Append( eventInfo );
        if ( KErrNone != error )
            {
            delete eventInfo;
            eventInfo = NULL;
            User::LeaveNoMemory();
            }
        }
    else
        {
        //entry exists already
        User::Leave( KErrAlreadyExists );
        }
        
    _LOG("CLogsEventInfoManager::AddEventEntryL(): end ")
    }


// ---------------------------------------------------------------------------
// Checks whether an entry exists for the specified service id
// ---------------------------------------------------------------------------
//
TBool CLogsEventInfoManager::EntryExists( TUint32 aServiceId )
    {
    _LOGP("CLogsEventInfoManager::EntryExists(): begin=%d", aServiceId)

    TBool success( EFalse );
    for ( TInt i = 0 ; ( i < Count() ) && ( success == EFalse ) ; i++ )
        {
        TLogsEventInfo* info = iEventInfoArray[i];
        if ( info && info->ServiceId() )
            {
            if ( info->ServiceId() == aServiceId ) 
                {
                success = ETrue;
                }
            }
        }

    _LOG("CLogsEventInfoManager::EntryExists(): end")
    return success;
    }


// ---------------------------------------------------------------------------
// Get an TLogsEventInfo entry with the specified service id. Returns Null if
// such an entry does not exist. Ownership not transferred.
// ---------------------------------------------------------------------------
//
TLogsEventInfo* CLogsEventInfoManager::GetEventInfoEntry( 
                                                TUint32 aServiceId ) const
    {
    _LOG("CLogsEventInfoManager::GetEventInfoEntry(): begin")

    TLogsEventInfo* entry = NULL;
    TBool success( EFalse );
    for ( TInt i = 0 ; ( i < Count() ) && ( success == EFalse ) ; i++ )    
        {
        TLogsEventInfo* info = iEventInfoArray[i];
        if ( info && info->ServiceId() )
            {
            if ( info->ServiceId() == aServiceId )
                {
                entry = info;
                success = ETrue;
                }
            }
        }
        
    _LOG("CLogsEventInfoManager::GetEventInfoEntry(): end")
    return entry;
    }

// ---------------------------------------------------------------------------
// Get an TLogsEventInfo entry with the specified service id. Returns Null if
// such an entry does not exist. Ownership not transferred.
// ---------------------------------------------------------------------------
//
TLogsEventInfo* CLogsEventInfoManager::GetEventInfoEntryByIndex( 
                                                        TInt aIndex ) const
    {
    _LOGP("CLogsEventInfoManager::GetEventInfoEntryByIndex():begin=%d",aIndex)

    TLogsEventInfo* entry = NULL;

    if ( aIndex >= 0 && aIndex < Count() )
        {
        entry = iEventInfoArray[aIndex];
        }


    _LOG("CLogsEventInfoManager::GetEventInfoEntryByIndex(): end")
    return entry;
    }


// ---------------------------------------------------------------------------
// Sets the brand icon index of the event info entry with a certain
// service id to 'aBrandingIconIndex'.
// ---------------------------------------------------------------------------
//
TInt CLogsEventInfoManager::SetBrandIndex(
    TUint32 aServiceId, 
    TInt aBrandingIconIndex )
    {
    _LOGPP("CLogsEventInfoManager::SetBrandIndex(): %d, %d", 
        aServiceId, aBrandingIconIndex)            
    TInt err( KErrNotFound );
    TLogsEventInfo* entry = GetEventInfoEntry( aServiceId );
    if ( entry )
        {
        entry->SetBrandingIconIndex( aBrandingIconIndex );
        err = KErrNone;
        }
        
    _LOGP("CLogsEventInfoManager::SetBrandIndex(): end return==%d",err)
    return err;
    }



// ---------------------------------------------------------------------------
// The brand icon index for a log event with the specified service id 
// is retrieved and assigned to 'aIndex'.
// ---------------------------------------------------------------------------
//
TBool CLogsEventInfoManager::GetBrandIconIndex( TUint32 aServiceId, 
                                                TInt& aIndex) const
    {
    _LOG("CLogsEventInfoManager::GetBrandIconIndex(): begin")

    TBool success( EFalse );

    // find the entry that contains 'aServiceId' and return its brandindex
    // or do nothing    
    for ( TInt i = 0 ; ( i < Count() ) && ( success == EFalse ) ; i++ )
        {
        TLogsEventInfo* info = iEventInfoArray[i];
        if ( info && info->ServiceId() )
            {
            if ( info->ServiceId() == aServiceId )
                {
                aIndex = info->BrandingIconIndex();
                success = ETrue;
                }
            }
        }

    _LOG("CLogsEventInfoManager::GetBrandIconIndex(): end")
    return success;
    }



// ---------------------------------------------------------------------------
// Returns the number of TLogsEventInfo objects that the manager maintains
// ---------------------------------------------------------------------------
//
TInt CLogsEventInfoManager::Count() const
    {    
    _LOGP("CLogsPresenceStatusTableEntry::Count(): =%d",
        iEventInfoArray.Count())
    return iEventInfoArray.Count();
    }


// ---------------------------------------------------------------------------
// Check whether the brand icon index for the specified service
// id is already available or not.
// ---------------------------------------------------------------------------
//
TBool CLogsEventInfoManager::BrandIconIndexAvailable( 
                                                    TUint32 aServiceId ) const
    {
    _LOG("CLogsEventInfoManager::BrandIconIndexAvailable(): begin")

    TBool result( EFalse );
    TInt brandIndex( KErrNotFound );
    if ( GetBrandIconIndex( aServiceId, brandIndex )
        && brandIndex > KInitialIndexValue )
        {
        result = ETrue;
        }

#ifdef _DEBUG
    if ( result )
        {
        _LOG("CLogsEventInfoManager::BrandIconIndexAvailable(): end ...")
        _LOG("...BrandIndex => available")
        }
    else
        {
        _LOG("CLogsEventInfoManager::BrandIconIndexAvailable(): end ...")
        _LOG("...BrandIndex => NOT available")
        }
#endif

    return result;
    }

