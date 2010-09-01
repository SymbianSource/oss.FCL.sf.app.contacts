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
* Description:  Used to collect various presence related information for the
*                extension.
*
*/



// INCLUDE FILES
#include "clogspresencetablemanager.h"
#include "clogspresenceindextableentry.h"
#include "clogspresencestatustableentry.h"
#include "clogsextlogidpresidmapping.h"
#include "MLogsEventGetter.h"
#include "logsextconsts.h"
#include "MLogsModel.h"
#include "simpledebug.h"

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CLogsPresenceTableManager* CLogsPresenceTableManager::NewL()
    {
    CLogsPresenceTableManager* self = CLogsPresenceTableManager::NewLC();
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//    
CLogsPresenceTableManager* CLogsPresenceTableManager::NewLC()
    {
    CLogsPresenceTableManager* self = new( ELeave ) CLogsPresenceTableManager;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//    
CLogsPresenceTableManager::~CLogsPresenceTableManager()
    {
    _LOG("CLogsPresenceTableManager::~CLogsPresenceTableManager: begin")
   
    iPresenceStatusTable.ResetAndDestroy();
    iPresenceIndexTable.ResetAndDestroy();
    iLogIdPresIdMappingArray.ResetAndDestroy();
    
    _LOG("CLogsPresenceTableManager::~CLogsPresenceTableManager: end")
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//    
void CLogsPresenceTableManager::ConstructL()
    {
    _LOG("CLogsPresenceTableManager::ConstructL(): begin")    
    _LOG("CLogsPresenceTableManager::ConstructL(): end")
    }


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogsPresenceTableManager::CLogsPresenceTableManager():
    iPresenceStatusTable( KPresenceTableManagerGranularity ),
    iPresenceIndexTable( KPresenceTableManagerGranularity ),
    iLogIdPresIdMappingArray( KLogIdPresIdMappingArrayGranularity )
    {    
    _LOG("CLogsPresenceTableManager::CLogsPresenceTableManager(): begin")
    _LOG("CLogsPresenceTableManager::CLogsPresenceTableManager(): end")
    }    



// ---------------------------------------------------------------------------
//  Gets the index of a certain icon in the listbox's icon array.
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceTableManager::GetIndex( 
    const TUint32 aServiceId, 
    const TDesC& aPresentityId, 
    TInt& aIndex )
    {    
    _LOG("CLogsPresenceTableManager::GetIndex: begin")
    TInt error( KErrNotFound );    
    
    TDesC* status = GetStatusFromStatusTable( aServiceId, aPresentityId );
    
    if ( status )
        {        
        TInt index( KErrNotFound );
        error = GetIndexFromIndexTable( aServiceId, *status, index );
        
        if ( KErrNone == error )
            {            
            if ( index >= 0 )
                {
                aIndex = index;
                error = KErrNone;
                }            
            else 
                {
                error = KErrNotFound;
                }            
            }
        else if ( KErrNotFound == error )
            {
            error = KErrNotReady;
            }
        }

    _LOGP("CLogsPresenceTableManager::GetIndex: end (error==%d)",error)    
    
    return error;
    }
  
// ---------------------------------------------------------------------------
// Gets the index of a certain icon in the listbox's icon array from the
// index table. 
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceTableManager::GetIndexFromIndexTable( 
    const TUint32 aServiceId, 
    const TDesC& aPresenceStatusString, 
    TInt& aIndex )
    {
    _LOG("CLogsPresenceTableManager::GetIndexFromIndexTable: begin")    
    TInt error( KErrNotFound );
    
    for ( TInt i( 0 ); i < IndexTableEntriesCount(); i++ )
        {
        CLogsPresenceIndexTableEntry* entry = iPresenceIndexTable[i];
        if ( entry 
            && entry->ServiceId() == aServiceId 
            && entry->PresenceStatusString() == aPresenceStatusString )
            {
            aIndex = entry->PresenceIconIndex();
            error = KErrNone;
            }        
        }    

    _LOGP("CLogsPresenceTableManager::GetIndexFromIndexTable: end (error=%d)",
        error)        

    return error;       
    }

// ---------------------------------------------------------------------------
// Gets the status of a status entry from the status table. 
// ---------------------------------------------------------------------------
//
TDesC* CLogsPresenceTableManager::GetStatusFromStatusTable( 
    const TUint32 aServiceId, 
    const TDesC& aPresentityId )
    {
    _LOG("CLogsPresenceTableManager::GetStatusFromStatusTable: begin")        

    TDesC* presenceStatus = NULL;
    
    _LOGP("CLogsPresenceTableManager::GetStatusFromStatusTable: count=%d",
        StatusTableEntriesCount())
    TBool found( EFalse );
    for ( TInt i( 0 ); !found && i < StatusTableEntriesCount(); i++ )
        {
        CLogsPresenceStatusTableEntry* entry = iPresenceStatusTable[i];
        _LOGP("CLogsPresenceTableManager::GetStatusFromStatusTable: i=%d",i)        
        if ( entry
            && entry->ServiceId() == aServiceId 
            && entry->PresentityId() == aPresentityId
            && entry->DisplayPresence() 
            && entry->PresenceStatusString() )
            {
            _LOG("CLogsPresenceTableManager::GetStatusFromStatusTable:...")
            _LOGPP("...INSIDE IF i=%d presenceString=%S",
                i , entry->PresenceStatusString() )                   
            
            presenceStatus = entry->PresenceStatusString();
            found = ETrue;            
            }                    
        }
        
    return presenceStatus;
    }
    

// ---------------------------------------------------------------------------
// Creates a new CLogsStatusTableEntry object and adds it to the manager's
// status table (which is iPresenceStatusTable). Before adding an entry to
// the table it is checked whether the entry already exists in the table 
// to avoid duplicates. Entries with the same service id and presentity id
// are considered to be duplicates.
// Note: Since the presence status might not being known when adding the
//       entry it is possible to leave it unspecified.
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceTableManager::AddEntryToStatusTable( 
    const TUint32 aServiceId, 
    const TDesC& aPresentityId,
    TBool aShowStatus )
    {
    _LOG("CLogsPresenceTableManager::AddEntryToStatusTable: begin")        
    TInt error( KErrNotFound );
    
    // if entry does not exist -> create it.
    if ( !StatusEntryExists( aServiceId, aPresentityId ) )
        {
        CLogsPresenceStatusTableEntry* entry = NULL;
        TRAP( error, entry = 
                        CLogsPresenceStatusTableEntry::NewL(
                                                           aServiceId,
                                                           aPresentityId,
                                                           aShowStatus ); );
        
        if ( KErrNone == error )
            {
            error = iPresenceStatusTable.Append( entry );
            if ( KErrNone != error )
                {
                delete entry;
                entry = NULL;
                error = KErrNoMemory;
                }            
            }        
        }
    // if entry already exists -> dont create it again.
    else 
        {
        //entry exists already
        error = KErrAlreadyExists;
        }

    _LOGP("CLogsPresenceTableManager::AddEntryToStatusTable: end (error=%d)",
        error )                
    return error;
    } 
    


// ---------------------------------------------------------------------------
// Creates a new CLogsIndexTableEntry object and adds it to the manager's
// index table (which is iPresenceIndexTable). Before adding an entry to
// the table it is checked whether the entry already exists in the table 
// to avoid duplicates. Entries with the same service id and presence
// status are considered to be duplicates.
// Note: Since the index might not being known when adding the
//       entry it is possible to leave it unspecified.
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceTableManager::AddEntryToIndexTable( 
    const TUint32 aServiceId, 
    const TDesC& aPresenceStatusString )
    {
    _LOGP("CLogsPresenceTableManager::AddEntryToIndexTable: aServiceId=%d", 
            aServiceId )
            
    TInt error( KErrNotFound );

    // if entry does not exist -> create it.
    TBool entryexists( ETrue );
    entryexists = IndexEntryExists( aServiceId, aPresenceStatusString );
    if ( !entryexists )
        {        
        CLogsPresenceIndexTableEntry* entry = NULL;
        TRAP( error, 
              entry = CLogsPresenceIndexTableEntry::NewL( aServiceId, 
                                                    aPresenceStatusString ) );
        if ( KErrNone == error )
            {
            error = iPresenceIndexTable.Append( entry );
            if ( KErrNone != error )
                {
                delete entry;
                entry = NULL;
                error = KErrNoMemory;
                }            
            }        
        }
    // if entry already exists -> dont create it again.
    else 
        {
        //entry exists already
        error = KErrAlreadyExists;
        }
    
    _LOGP("CLogsPresenceTableManager::AddEntryToIndexTable: end (error==%d)",
            error)                    
    return error;
    }


// ---------------------------------------------------------------------------
// Check whether the presence icon index is available or not.
// ---------------------------------------------------------------------------
//
TBool CLogsPresenceTableManager::PresenceIconIndexAvailable( 
    const TUint32 aServiceId,
    const TDesC& aPresentityId )
    {
    _LOG("CLogsPresenceTableManager::PresenceIconIndexAvailable: begin")        
    TBool IsIndexAvailable( EFalse );
    
    TInt index( KErrNotFound );
    TInt error ( GetIndex( aServiceId, aPresentityId, index ) );    
    
    switch( error )
        {
        case KErrNone:
            {
            if ( index >= 0 )
                {
                IsIndexAvailable = ETrue;
                }            
            break;
            }
        case KErrNotReady:
        case KErrNotFound:
        default:
            {
            IsIndexAvailable = EFalse;
            }        
        }

    if ( IsIndexAvailable )
        {
        _LOG("CLogsPresenceTableManager::PresenceIconIndexAvailable: end...")
        _LOG("... PresenceIndex => available")
        }
    
    else
        {
        _LOG("CLogsPresenceTableManager::PresenceIconIndexAvailable: end...")
        _LOG("... PresenceIndex => NOT available")
        }
    


    return IsIndexAvailable;
    }

  
// ---------------------------------------------------------------------------
// Sets the status of a certain status entry of the status table.
// ---------------------------------------------------------------------------
//   
TInt CLogsPresenceTableManager::SetStatusInStatusTable( 
    const TUint32 aServiceId,
    const TDesC& aPresentityId,
    const TDesC& aPresenceStatusString,
    TBool aShowStatus )
    {
    _LOG("CLogsPresenceTableManager::SetStatusInStatusTable: begin")        
    TInt error( KErrGeneral );
    TBool found( EFalse );
    
    for ( TInt i( 0 ); !found && i < StatusTableEntriesCount(); i++ )
        {
        CLogsPresenceStatusTableEntry* entry = iPresenceStatusTable[i];
        if ( entry 
            && entry->ServiceId() == aServiceId 
            && entry->PresentityId() == aPresentityId )
            {
            found = ETrue;
            entry->SetDisplayPresence( aShowStatus );
            error = entry->SetPresenceStatusString( aPresenceStatusString );
            }        
        }
    
    _LOGP("CLogsPresenceTableManager::SetStatusInStatusTable: end (error=%d)",
            error)                 
    return error;
    }
            

// ---------------------------------------------------------------------------
// Sets the status of a certain status entry of the status table to the 
// initial value. Presense status is not displayed.
// ---------------------------------------------------------------------------
//   
TInt CLogsPresenceTableManager::UpdateStatusTableEntry( 
    const TUint32 aServiceId,
    const TDesC& aPresentityId,
    TBool aShowStatus )
    {
    _LOG("CLogsPresenceTableManager::ResetStatusTableEntry: begin")        
    TInt error( KErrNotFound );
    TBool found( EFalse );
    
    for ( TInt i( 0 ); !found && i < StatusTableEntriesCount(); i++ )
        {
        CLogsPresenceStatusTableEntry* entry = iPresenceStatusTable[i];
        if ( entry 
            && entry->ServiceId() == aServiceId 
            && entry->PresentityId() == aPresentityId )
            {            
            entry->SetDisplayPresence( aShowStatus );
            found = ETrue;
            error = KErrNone;
            }        
        }
    
    _LOGP("CLogsPresenceTableManager::ResetStatusTableEntry: end (error==%d)",
            error)                 
    return error;
    }


// ---------------------------------------------------------------------------
// Sets the status of all status entries of the status table to the initial 
// value.
// ---------------------------------------------------------------------------
//   
void CLogsPresenceTableManager::ResetStatusTableEntries()
    {
    _LOG("CLogsPresenceTableManager::ResetStatusTableEntries: begin")        
    
    for ( TInt i( 0 ); i < StatusTableEntriesCount(); i++ )
        {
        CLogsPresenceStatusTableEntry* entry = iPresenceStatusTable[i];
        if ( entry )
            {            
            entry->ResetPresenceStatusString();
            entry->SetDisplayPresence( EFalse );
            }        
        }
    
    _LOG("CLogsPresenceTableManager::ResetStatusTableEntries: end")                 
    }

// ---------------------------------------------------------------------------
// Sets the presence icon index values to initial values.
// ---------------------------------------------------------------------------
//  
void CLogsPresenceTableManager::ResetState()
    {
    _LOG("CLogsPresenceTableManager::ResetIndexTableEntries: begin")  
    iPresenceIndexTable.ResetAndDestroy();
    iPresenceStatusTable.ResetAndDestroy();
    iLogIdPresIdMappingArray.ResetAndDestroy();
    _LOG("CLogsPresenceTableManager::ResetIndexTableEntries: end")  
    }
 
 
// ---------------------------------------------------------------------------
// Sets the index of a certain index entry of the index table.
// ---------------------------------------------------------------------------
//     
TInt CLogsPresenceTableManager::SetIndexInIndexTable( 
    const TUint32 aServiceId,
    const TDesC& aPresenceStatusString,
    const TInt aIndex )
    {
    _LOG("CLogsPresenceTableManager::SetIndexInIndexTable: begin")        
    TInt error( KErrGeneral );
    TBool found( EFalse );
    
    for ( TInt i( 0 ); !found && i < IndexTableEntriesCount() ; i++ )
        {
        CLogsPresenceIndexTableEntry* entry = iPresenceIndexTable[i];
        if ( entry 
            && entry->ServiceId() == aServiceId 
            && entry->PresenceStatusString() == aPresenceStatusString )
            {
            found = ETrue;            
            error = entry->SetPresenceIconIndex( aIndex );            
            }        
        }
    
    _LOGP("CLogsPresenceTableManager::SetIndexInIndexTable: end (error==%d)",
            error)
    return error; 
    }

// ---------------------------------------------------------------------------
// Checks the existance of a status entry with the specified arguments.
// ---------------------------------------------------------------------------
//
TBool CLogsPresenceTableManager::StatusEntryExists( 
        const TUint32 aServiceId,
        const TDesC& aPresentityId )
    {
    _LOG("CLogsPresenceTableManager::StatusEntryExists: begin")        
    TBool exists( EFalse );
    
    for ( TInt i( 0 ); !exists && i < StatusTableEntriesCount(); i++ )
        {
        CLogsPresenceStatusTableEntry* entry = iPresenceStatusTable[i];
        if ( entry 
            && entry->ServiceId() == aServiceId 
            && entry->PresentityId() == aPresentityId )
            {
            _LOG("CLogsPresenceTableManager::StatusEntryExists: entry exists")
            exists = ETrue;            
            }
        }
    
    _LOG("CLogsPresenceTableManager::StatusEntryExists: end")
    return exists;     
    }


// ---------------------------------------------------------------------------
// Checks the existance of a index entry with the specified arguments.
// ---------------------------------------------------------------------------
//
TBool CLogsPresenceTableManager::IndexEntryExists( 
        const TUint32 aServiceId,
        const TDesC& aPresenceStatus )
    {
    _LOG("CLogsPresenceTableManager::IndexEntryExists: begin")        
    TBool exists( EFalse );
    
    for ( TInt i( 0 ); !exists && i < IndexTableEntriesCount() ; i++ )
        {
        CLogsPresenceIndexTableEntry* entry = iPresenceIndexTable[i];
        if ( entry 
            && entry->ServiceId() == aServiceId 
            && entry->PresenceStatusString() == aPresenceStatus )
            {
            _LOG("CLogsPresenceTableManager::IndexEntryExists: entry exists")
            exists = ETrue;            
            }
        }
    
    _LOG("CLogsPresenceTableManager::IndexEntryExists: end")
    return exists;     
    }


// ---------------------------------------------------------------------------
// Returns the number of entries in the status table.
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceTableManager::StatusTableEntriesCount()
    {
    _LOGP("CLogsPresenceTableManager::StatusTableEntriesCount(): count=%d", 
        iPresenceStatusTable.Count())    
    return iPresenceStatusTable.Count();
    }
    
    
// ---------------------------------------------------------------------------
// Returns the number of entries in the index table.
// ---------------------------------------------------------------------------
//
TInt CLogsPresenceTableManager::IndexTableEntriesCount()
    {
    _LOGP("CLogsPresenceTableManager::IndexTableEntriesCount()=%d", 
        iPresenceIndexTable.Count())
    return iPresenceIndexTable.Count();    
    }


// ---------------------------------------------------------------------------
// Adds a mapping (Log Id is mapped to a presentity id).
// ---------------------------------------------------------------------------
//   
void CLogsPresenceTableManager::AddMappingL( const TLogId aLogId, 
                                             const TDesC& aPresentityId )
    {    
    _LOG("CLogsPresenceTableManager::AddMappingL(): begin")
    
    // if a mapping for aLogId already exists and the two pres ids of both
    // mappings are different, only then update the existing mapping with the
    // new pres id   
    
    const TDesC* presId = NULL;
    presId = RetrievePresentityId( aLogId );
    
    if ( presId && ( *presId != aPresentityId ) ) //update mapping
        {
        _LOG("CLogsPresenceTableManager::AddMappingL():...")
        _LOG("... update existing mapping")
        UpdateMappingL( aLogId, aPresentityId );
        }
    else if( !presId ) // add mapping
        {
        _LOG("CLogsPresenceTableManager::AddMappingL(): create new mapping")
        AppendMappingL( aLogId, aPresentityId );
        }
    
    _LOGP("CLogsPresenceTableManager::RetrievePresentityId(): #mappings=%d",
        iLogIdPresIdMappingArray.Count() )
    
    _LOG("CLogsPresenceTableManager::AddMappingL(): end")
    }
    
    
// ---------------------------------------------------------------------------
// Updates the presentity id an existing mapping.
// ---------------------------------------------------------------------------
//
void CLogsPresenceTableManager::UpdateMappingL( const TLogId aLogId, 
                                                const TDesC& aPresentityId )
    {
    _LOG("CLogsPresenceTableManager::UpdateMappingL(): begin")    
    
    // delete the mapping with aLogId    
    TBool mappingFound( EFalse );
    CLogsExtLogIdPresIdMapping* mapping = NULL;
    for ( TInt i( 0 ); 
          i < iLogIdPresIdMappingArray.Count() && !mappingFound; 
          i++ )
        {
        mapping = iLogIdPresIdMappingArray[i];
        if ( mapping && ( mapping->LogId() == aLogId ) )
            {
            delete mapping;
            mapping = NULL;
            iLogIdPresIdMappingArray.Remove( i );
            mappingFound = ETrue;
            }
        }
    iLogIdPresIdMappingArray.Compress();
    
    // create a new mapping
    AppendMappingL( aLogId, aPresentityId );
    
    _LOG("CLogsPresenceTableManager::UpdateMappingL(): end")        
    }


// ---------------------------------------------------------------------------
// Creates a new mapping with the specified arguments.
// ---------------------------------------------------------------------------
//
void CLogsPresenceTableManager::AppendMappingL( const TLogId aLogId, 
                                                const TDesC& aPresentitytId )
    {
    _LOG("CLogsPresenceTableManager::AppendMappingL(): begin")    
    
    CLogsExtLogIdPresIdMapping* mapping = 
        CLogsExtLogIdPresIdMapping::NewLC( aLogId, aPresentitytId );         
    iLogIdPresIdMappingArray.AppendL( mapping );
    CleanupStack::Pop( mapping );

    _LOG("CLogsPresenceTableManager::AppendMappingL(): end")    
    }


    
// ---------------------------------------------------------------------------
// Returns the presentity id of a mapping using the log id to retrieve the
// mapping.
// ---------------------------------------------------------------------------
//
const TDesC* CLogsPresenceTableManager::RetrievePresentityId( 
        const TLogId aLogId )
    {
    _LOG("CLogsPresenceTableManager::RetrievePresentityId(): begin")
        
    const TDesC* result = NULL;
    CLogsExtLogIdPresIdMapping* mapping = NULL;
    for ( TInt i( 0 ); i < iLogIdPresIdMappingArray.Count(); i++ )
        {
        mapping = iLogIdPresIdMappingArray[i];
        if ( mapping && ( mapping->LogId() == aLogId ) )
            {
            result = mapping->PresentityId();
            }
        }
    
    _LOG("CLogsPresenceTableManager::RetrievePresentityId(): end")        
    return result;    
    }    
   

// ---------------------------------------------------------------------------
// Removes unnecessary mappings by comparing the log ids of the current 
// view to the log ids in the array of mappings. Those mappings which 
// contain a log id that cannot be found from 'aModel' will be deleted.
// ---------------------------------------------------------------------------
// 
void CLogsPresenceTableManager::RemoveUnneededMappings( MLogsModel& aModel )
    {
    _LOG("CLogsPresenceTableManager::RemoveUnneededMappings(): begin")
    
    CLogsExtLogIdPresIdMapping* mapping = NULL;
    _LOG("CLogsPresenceTableManager::RemoveUnneededMappings():...")
    _LOGP("... #mappingsBeforeCleanup=%d",iLogIdPresIdMappingArray.Count() )  
      
    for ( TInt i( 0 ); i < iLogIdPresIdMappingArray.Count(); i++ )
        {
        mapping = iLogIdPresIdMappingArray[i];        
        if ( mapping && !IsLogIdInMappingArray( aModel, mapping->LogId() ) )
            {
            RemoveMapping( mapping->LogId() );
            }            
        }
        
    _LOG("CLogsPresenceTableManager::RemoveUnneededMappings():...")
    _LOGP("... #mappingsAfterCleanup=%d",iLogIdPresIdMappingArray.Count() )
    
    _LOG("CLogsPresenceTableManager::RemoveUnneededMappings(): end")
    }


// ---------------------------------------------------------------------------
// Checks if one of the log events in 'aModel' has the same LogId as the one 
// that is specified.
// ---------------------------------------------------------------------------
// 
TBool CLogsPresenceTableManager::IsLogIdInMappingArray( MLogsModel& aModel, 
                                                        TLogId aLogId )
    {
    _LOG("CLogsPresenceTableManager::IsLogIdInMappingArray(): begin")    
    
    TBool isLogIdInMappingArray( EFalse );
    TBool found( EFalse );
    
    for ( TInt i( 0 ); i < aModel.Count() && !found; i++ )
        {
        if ( aModel.At( i ) && aLogId == aModel.At( i )->LogId() )
            {
            isLogIdInMappingArray = ETrue;
            found = ETrue;            
            }        
        }
    
    _LOG("CLogsPresenceTableManager::IsLogIdInMappingArray(): end")    
    return isLogIdInMappingArray;    
    }


// ---------------------------------------------------------------------------
// Removes a mapping from the array that contains the pointer of all the 
// mappings. This includes the deletion of the object whose pointer was 
// stored in the mapping array.
// ---------------------------------------------------------------------------
// 
void CLogsPresenceTableManager::RemoveMapping( TLogId aLogId )
    {
    _LOG("CLogsPresenceTableManager::RemoveMapping(): begin ")    
    
    _LOG("CLogsPresenceTableManager::RemoveMapping(): remove mapping...")
    _LOGP("... with logId=%d", aLogId )
    
    CLogsExtLogIdPresIdMapping* mapping = NULL;
    TBool deleted( EFalse );
    for ( TInt i( 0 ); i < iLogIdPresIdMappingArray.Count(); i++ )
        {
        mapping = iLogIdPresIdMappingArray[i];
        if ( mapping && ( mapping->LogId() == aLogId ) && !deleted )
            {
            iLogIdPresIdMappingArray.Remove( i );
            iLogIdPresIdMappingArray.Compress();
            delete mapping;
            mapping = NULL;
            deleted = ETrue;
            _LOG("CLogsPresenceTableManager::RemoveMapping(): mapping...")
            _LOGP("... with logId=%d deleted from mapping array", aLogId )
            }
        }
    
    _LOG("CLogsPresenceTableManager::RemoveMapping(): end")    
    }


