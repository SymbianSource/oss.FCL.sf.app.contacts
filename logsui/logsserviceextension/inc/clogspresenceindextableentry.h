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



#ifndef C_CLOGSPRESENCEINDEXTABLEENTRY_H
#define C_CLOGSPRESENCEINDEXTABLEENTRY_H


#include <e32base.h>


/**
 *  Encapsulates service id, presence status and the index of the presence 
 *  status icon in the listbox's icon array.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CLogsPresenceIndexTableEntry) : public CBase
    {

public:
    
    
    /**
     * Two-phased constructor.
     * @param aServiceId a service id
     * @param aPresenceStatusString a presence status string e.g."open"
     * @param aPresenceIconIndex a icon index
     */
     static CLogsPresenceIndexTableEntry* NewL( 
        const TUint32 aServiceId, 
        const TDesC& aPresenceStatusString,
        TInt aPresenceIconIndex = KErrNotFound );
        
    /**
     * Two-phased constructor.
     * @param aServiceId a service id
     * @param aPresenceStatusString a presence status string e.g."open"
     * @param aPresenceIconIndex a icon index
     */
    static CLogsPresenceIndexTableEntry* NewLC( 
        const TUint32 aServiceId, 
        const TDesC& aPresenceStatusString,
        TInt aPresenceIconIndex = KErrNotFound );
    
    /**
     * Destructor
     */
    ~CLogsPresenceIndexTableEntry();


    /**
     * Returns the service id of this index table entry.
     *
     * @since S60 v3.2
     * @return service id
     */
    TUint32 ServiceId() const;
    
    /**
     * Returns the presence status string of this index table entry.
     *
     * @since S60 v3.2
     * @return presence status string, e.g."busy"
     */
    TDesC& PresenceStatusString() const;    
    
    /**
     * Returns the presence icon index of this index table entry.
     *
     * @since S60 v3.2
     * @return KErrNone, if the index is different from KErrNotFound (which
     * is the initial value); KErrNotFound otherwise 
     */    
    TInt PresenceIconIndex() const;

    /**
     * Sets the presence icon index of this index table entry 
     *
     * @since S60 v3.2
     * @param aPresenceIconIndex the presence status icon index
     * @return KErrNone, if setting the index was successful
     *         KErrArgument, if the aPresenceIconIndex is negative
     */ 
    TInt SetPresenceIconIndex( TInt aPresenceIconIndex );
    
    
private:
       
    /**
     * Constructor
     *
     * @since S60 v3.2
     */
    CLogsPresenceIndexTableEntry( const TUint32 aServiceId );


    /**
     * Symbian Second Phase Constructor.
     *
     * @since S60 v3.2
     */
    void ConstructL( const TDesC& aPresenceStatusString,
                     const TInt aPresenceIconIndex );


    /**
     * Returns the presence status of this index table entry.
     *
     * @since S60 v3.2
     * @return presence status
     */
    TInt PresenceStatus() const;
    

private: // data

    /**
     * Service Id.
     */
    TUint32 iServiceId;

    /**
     * The Presence status in a string representation.
     */
    HBufC* iPresenceStatusString; 
    
    /**
     * Index of the icon of the presence status 'iPresenceStatus' 
     * of service with 'iServiceId' in the listbox's icon array.
     */
    TInt iPresenceIconIndex;
    
    };


#endif // C_CLOGSPRESENCEINDEXTABLEENTRY_H
