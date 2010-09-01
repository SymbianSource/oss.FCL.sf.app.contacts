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


#ifndef C_CLOGSEVENTINFOMANAGER_H
#define C_CLOGSEVENTINFOMANAGER_H


#include <e32base.h>

class TLogsEventInfo;

NONSHARABLE_CLASS(CLogsEventInfoManager) : public CBase 
    {
friend class ut_clogsuicontrolextension;
public:
        
    static CLogsEventInfoManager* NewL();
    static CLogsEventInfoManager* NewLC();

    /**
     * Destructor.
     */        
    ~CLogsEventInfoManager();
    
    /**
     * Creates a TLogsEventInfo entry and adds it the manager's array of
     * TLogsEventInfo entries. Before that it is checked whether the entry 
     * already exists to avoid duplicates. TLogsEventInfo entries with
     * the same service id are considered to be duplicates. 
     * Leaves with system-wide error code in case of an error.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     */
    void AddEventEntryL( TUint32 aServiceId );
    
    /**
     * The brand icon index for a log event with the specified service id 
     * is retrieved and assigned to 'aIndex'.
     *
     * @since S60 v3.2
     * @param aServiceId Id of requested settings entry
     * @param aIndex the index of the brand icon of the specified service
     * @return True, if index for the specified serviceId was found and set,
     *         False otherwise
     */ 
    TBool GetBrandIconIndex( TUint32 aServiceId, TInt& aIndex ) const;

    /**
     * Returns the event info entry at the specified index in the
     * iEventInfoArray array.
     *
     * @since S60 v3.2
     * @param aIndex a index
     * @return pointer to a event info object or null if the index
     *         position is not valid
     */ 
    TLogsEventInfo* GetEventInfoEntryByIndex( TInt aIndex ) const;
    
    /**
     * Returns the number of event info objects.
     *
     * @since S60 v3.2
     * @return number of event info objects
     */ 
    TInt Count() const;
    
    /**
     * Check whether the branding icon index for the specified service
     * id is already available or not.
     *
     * @since S60 v3.2
     * @param aServiceId service id
     * @return True, if the branding icon index for the specified service
     *         id is available, False otherwise
     */ 
    TBool BrandIconIndexAvailable( TUint32 aServiceId ) const;

    /**
     * Sets the brand icon index of the event info entry with a 
     * certain service id to 'aBrandingIconIndex'. If no event
     * info entry could be found, 'KErrNotFound' is returned.
     * If setting succeeded 'KErrNone' is returned.
     *
     * @since S60 v3.2
     * @param aServiceId Id of requested settings entry
     * @param aBrandingIconIndex the brand icon index to be set
     * @return KErrNotFound, if no TLogsEventInfo object with the
     *         specified service id could be found, KErrNone otherwise
     */ 
    TInt SetBrandIndex( TUint32 aServiceId, TInt aBrandingIconIndex );    
    
    
private:
    
    void ConstructL();
    
    /**
     * Constructor.
     */
    CLogsEventInfoManager();

    /**
     * Determines whether the manager contains already an entry for the 
     * specified service id.
     *
     * @since S60 v3.2
     * @param aServiceId the service id
     * @return Erue, if the manager contains already an entry with the
     *         specified service id, False otherwise
     */ 
    TBool EntryExists( TUint32 aServiceId );    
    
    /**
     * Searches the event info array for an entry with the specified service id
     * and returns the pointer to this entry or NULL if no such entry can be 
     * found.
     *
     * @since S60 v3.2
     * @param aServiceId Id of requested settings entry
     * @return Pointer to the entry with the specified service id or NULL if 
     *         no such entry can be found in the iEventInfoArray array
     */ 
    TLogsEventInfo* GetEventInfoEntry( TUint32 aServiceId ) const;

  
private: // data   
    
    /**
     * The array that contains the event info entries
     * Own.
     */  
    RPointerArray<TLogsEventInfo> iEventInfoArray;
    
    };
        
#endif // C_CLOGSEVENTINFOMANAGER_H
