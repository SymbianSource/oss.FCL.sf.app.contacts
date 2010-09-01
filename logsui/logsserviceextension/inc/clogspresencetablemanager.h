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


#ifndef C_CLOGSPRESENCETABLEMANAGER_H
#define C_CLOGSPRESENCETABLEMANAGER_H

#include <e32base.h>

#include "logwrap.h"

class CLogsPresenceStatusTableEntry;
class CLogsPresenceIndexTableEntry;
class CLogsExtLogIdPresIdMapping;
class MLogsModel;

/**
 *  Used to collect and to keep track of various presence related 
 *  information for the extension.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CLogsPresenceTableManager) : public CBase
    {
    friend class ut_clogsuicontrolextension;
public:

    friend class ut_clogspresencetablemanager;    
    
    /**
     * Two-phased constructor.
     * Creates a new CLogsPresenceTableManager.
     *
     * @since S60 v3.2
     */        
    static CLogsPresenceTableManager* NewL();
    
    /**
     * Two-phased constructor.
     * Creates a new CLogsPresenceTableManager.
     *
     * @since S60 v3.2
     */
    static CLogsPresenceTableManager* NewLC();

    /**
     * Destructor.
     *
     * @since S60 v3.2
     */        
    ~CLogsPresenceTableManager();
    
    
    /**
     * Gets the index of a certain icon in the listbox's icon array. The index
     * is found by: 
     * a) looking up the presence status of the given presentity from the 
     *    status table (which contains CLogsStatusTableEntry objects)
     * b) using the status found in step a) to retrieve the index from the 
     *    index table (which contains CLogsIndexTableEntry objects)
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId the presentity id (depends on the particular 
     *        service, e.g.'sip:Max.And.Moritz@WilhelmBusch.edu' or similar)
     * @param aIndex the variable the index will be stored to in case it was
     *        found, aIndex will not be modified in case of an error. 
     * @return KErrNone, if the index information for the specified service 
     *         and aPresentityId could be retrieved; 
     *         KErrNotReady, if a corresponding index table entry exists,
     *           but the index information was not updated yet (fetching
     *           might be still ongoing)
     *         KErrNotFound, if no corresponding index table entry exists
     *         KErrGeneral, otherwise          
     */ 
    TInt GetIndex( const TUint32 aServiceId, 
                   const TDesC& aPresentityId, 
                   TInt& aIndex );
    
    /**
     * Sets the status of a certain status entry of the status table.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId the presentity id
     * @param aPresenceStatus the presence status to be set
     * @param aShowStatus status display flag
     * @return KErrNone if setting the value succeeded; 
     *         system-wide errorcode otherwise
     */    
    TInt SetStatusInStatusTable( const TUint32 aServiceId,
                                 const TDesC& aPresentityId,
                                 const TDesC& aPresenceStatus,
                                 TBool aShowStatus );
                
    /**
     * Sets the index of a certain index entry of the index table.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresenceStatus the presence status 
     * @param aIndex the index to be set
     * @return KErrNone if setting the value succeeded; 
     *         system-wide errorcode otherwise
     */    
    TInt SetIndexInIndexTable( const TUint32 aServiceId,
                               const TDesC& aPresenceStatus,
                               const TInt aIndex );

    /**
     * Determines whether the icon index for the specified presentity
     * of the specified service is already available or not.
     *
     * Note: If the index is available then the status of the presentity
     * has been retrieved and the icon has been added to the icon array list
     * already. The status icon is ready to be displayed. If the index is not
     * available yet, the icon has not been added and is not ready to be 
     * displayed.
     *
     * @param aServiceId service id
     * @param aPresentityId the presentity id     
     * @return ETrue, if the index is available, EFalse otherwise
     */
    TBool PresenceIconIndexAvailable( const TUint32 aServiceId,
                                      const TDesC& aPresentityId );

    /**
     * Gets the index of a certain icon in the listbox's icon array from the
     * index table. 
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresenceStatus the presence status (e.g. 'busy' or similar)
     * @param aIndex the variable the index will be stored to in case it was
     *        found, aIndex will not be modified in case of an error
     * @return KErrNone, if the index information could be retrieved (means 
     *         that an index entry with aServiceId and aPresenceStatus exists
     *         in the table; however, the value of the index of that entry can
     *         still be 'KErrNotFound' if the value was not changed after the 
     *         entry was created, since KErrNotFound is the initial value for 
     *         the index of a index entry); 
     *         KErrNotFound otherwise.
     *         
     */ 
    TInt GetIndexFromIndexTable( const TUint32 aServiceId, 
                                 const TDesC& aPresenceStatus, 
                                 TInt& aIndex );

    /**
     * Creates a new CLogsStatusTableEntry object and adds it to the manager's
     * status table (which is iPresenceStatusTable). Before adding an entry to
     * the table it is checked whether the entry already exists in the table 
     * to avoid duplicates. Entries with the same service id and presentity id
     * are considered to be duplicates.
     * Note: Since the presence status might not being known when adding the
     *       entry it is possible to leave it unspecified.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId the presentity id (depends on the particular 
     * @param aShowStatus status display flag
     *        service, e.g.'sip:Max.And.Moritz@WilhelmBusch.edu' or similar)
     * @return KErrNone, if the creation and insertation was successful;
     *         system-wide errorcode otherwise.
     */
    TInt AddEntryToStatusTable( const TUint32 aServiceId, 
                                const TDesC& aPresentityId,
                                TBool aShowStatus );    
    
    /**
     * Creates a new CLogsIndexTableEntry object and adds it to the manager's
     * index table (which is iPresenceIndexTable). Before adding an entry to
     * the table it is checked whether the entry alraedy exists in the table 
     * to avoid duplicates. Entries with the same service id and presence
     * status are considered to be duplicates.
     * Note: Since the idnex might not being known when adding the
     *       entry it is possible to leave it unspecified.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId the presentity id (depends on the particular 
     *        service, e.g.'sip:Max.And.Moritz@WilhelmBusch.edu' or similar)
     * @return KErrNone, if the creation and insertation was successful;
     *         system-wide errorcode otherwise.
     */
    TInt AddEntryToIndexTable( const TUint32 aServiceId, 
                               const TDesC& aPresenceStatus );    

    /**
     * Returns the number of entries in the status table.
     *
     * @since S60 v3.2
     * @return number of entries in the status table
     */
    TInt StatusTableEntriesCount();
    
    /**
     * Adds another mapping to the collection of mappings 
     * (Log Id is mapped to a presentity id).
     *
     * @since S60 v3.2
     * @param aLogId a log id
     * @param aPresentityId a presentity id
     */
    void AddMappingL( const TLogId aLogId, const TDesC& aPresentitytId );

    /**
     * Returns the presentity id of a mapping using the log id to retrieve
     * the mapping.
     *
     * @since S60 v3.2
     * @param aLogId a log id
     * @return the presentity id or NULL if no mapping was found for the 
     *         specified lod id.
     */   
    const TDesC* RetrievePresentityId( const TLogId aLogId );

    /**
     * Removes unnecessary mappings by comparing the log ids of the current 
     * view to the log ids in the array of mappings. Those mappings which 
     * contain a log id that cannot be found from 'aModel' will be deleted.
     *
     * @since S60 v3.2
     * @param aModel the data model that contains the
     *        information about the log event entries in the current view.
     */    
    void RemoveUnneededMappings( MLogsModel& aModel );
    
    /**
     * Sets the status of a certain status entry of the status table to the 
     * initial value ( = presense not displayed ) .
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId a presentity id
     * @param aShowStatus status display flag
     * @return KErrNone in case the status entry was reset successfully;
     *         system-wide error code otherwise
     */
    TInt UpdateStatusTableEntry( const TUint32 aServiceId,
                                const TDesC& aPresentityId,
                                TBool aShowStatus );
    
    /**
     * Sets the status of all status entries of the status table to the initial 
     * value.
     */
    void ResetStatusTableEntries();
    
    /**
     *  Sets state of this class to its initial state
     * 
     */
    void ResetState();
    
private:   
    
    /**
     * Returns the number of entries in the index table.
     *
     * @since S60 v3.2
     * @return number of entries in the index table
     */
    TInt IndexTableEntriesCount();
    
    /**
     * Gets the status of a certain entry from the status table.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId the presentity id
     * @return the presence status or NULL if the value could not 
     *         be retrieved or if the internal 'ShowStatus' flag 
     *         of the status table entry is not set.
     */ 
    TDesC* GetStatusFromStatusTable( const TUint32 aServiceId, 
                                     const TDesC& aPresentityId );                                   
    /**
     * Determines whether a status entry exists for the given arguments.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresentityId the presentity id
     * @return ETrue, if a status entry with the given arguments exists;
     *         EFalse otherwise
     */     
    TBool StatusEntryExists( const TUint32 aServiceId,
                             const TDesC& aPresentityId );

    /**
     * Determines whether a index entry exists for the given arguments.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @param aPresenceStatus the presence status (e.g. 'busy' or similar)
     * @return ETrue, if a index entry with the given arguments exists;
     *         EFalse otherwise
     */    
    TBool IndexEntryExists( const TUint32 aServiceId,
                            const TDesC& aPresenceStatus );
                            
    /**
     * Updates the presentity id an existing mapping.
     * 
     * @since S60 v3.2
     * @param aLogId a unique log id
     * @param aPresentityId the presentity id
     */
    void UpdateMappingL( const TLogId aLogId, 
                         const TDesC& aPresentitytId );

    
    /**
     * Creates a new mapping with the specified arguments.
     *
     * @since S60 v3.2
     * @param aLogId a unique log id
     * @param aPresentityId the presentity id
     */
    void AppendMappingL( const TLogId aLogId, 
                         const TDesC& aPresentitytId );

    /**
     * Checks if one of the log events in 'aModel' has the same LogId as
     * the one that is specified.
     *
     * @since S60 v3.2
     * @param aModel the data model that contains the
     *        information about the log event entries in the current view.
     * @param aLogId a unique log id
     * @return ETrue, if aLogId can be found in aModel; EFalse otherwise.
     */
    TBool IsLogIdInMappingArray( MLogsModel& aModel, TLogId aLogId );
    
    
    /**
     * Removes a mapping from the array that contains the pointer
     * of all the mappings. This includes the deletion of the object 
     * whose pointer was stored in the mapping array.
     *
     * @since S60 v3.2
     * @param aLogId a unique log id
     */
    void RemoveMapping( TLogId aLogId );
                         
private:
    
    /**
     * Symbian second phase constructor.
     */
    void ConstructL();
    
    /**
     * Constructor.
     */
    CLogsPresenceTableManager();

  
private: // data   

    /**
     * Collection of status table entries.
     * Own.
     */  
    RPointerArray<CLogsPresenceStatusTableEntry> iPresenceStatusTable;  

    /**
     * Collection of index table entries.
     * Own.
     */  
    RPointerArray<CLogsPresenceIndexTableEntry> iPresenceIndexTable;  
    
    /**
     * Collection of mappings.
     * Own.
     */
    RPointerArray<CLogsExtLogIdPresIdMapping> iLogIdPresIdMappingArray;
    };
    
#endif // C_CLOGSPRESENCETABLEMANAGER_H
