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



#ifndef C_CLOGSPRESENCESTATUSTABLEENTRY_H
#define C_CLOGSPRESENCESTATUSTABLEENTRY_H

#include <e32base.h>

/**
 *  Encapsulates the service id, presentity id and the presence status.
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CLogsPresenceStatusTableEntry) : public CBase
    {

public:    

    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2     
     * @param aServiceId a service id
     * @param aPresentityId a presentity id
     * @param aDisplayStatus status display flag
     * @param aPresenceStatusString a presence status string e.g."open"
     */
    static CLogsPresenceStatusTableEntry* NewL(
        const TUint32 aServiceId, 
        const TDesC& aPresentityId,
        TBool aDisplayStatus,
        TDesC* aPresenceStatusString = NULL );
        
    /**
     * Two-phased constructor.
     *
     * @since S60 v3.2     
     * @param aServiceId a service id
     * @param aPresentityId a presentity id
     * @param aDisplayStatus status display flag
     * @param aPresenceStatusString a presence status string e.g."open"
     */
    static CLogsPresenceStatusTableEntry* NewLC(
        const TUint32 aServiceId, 
        const TDesC& aPresentityId, 
        TBool aDisplayStatus,
        TDesC* aPresenceStatusString = NULL );

    /**
     * Destructor
     *
     * @since S60 v3.2     
     */    
    ~CLogsPresenceStatusTableEntry();
    
        
    /**
     * Returns the service id of this status table entry.
     *
     * @since S60 v3.2
     * @return service id
     */
    TUint32 ServiceId() const;        
    
    /**
     * Returns the presence display status of this status table entry.
     *
     * @since S60 v3.2
     * @return ETrue, if the presence status should be displayed in UI, 
     *         EFalse otherwise.
     */
    TBool DisplayPresence();
    
    /**
     * Sets the presence display status flag of this status table entry.
     *
     * @since S60 v3.2     
     * @param aDisplayStatus status flag
     */
    void SetDisplayPresence( TBool aDisplayStatus );
    
    /**
     * Returns the presentity id of this status table entry.
     *
     * @since S60 v3.2
     * @return presentity id
     */
    const TDesC& PresentityId();
    
    /**
     * Returns the presence status of this status table entry.
     * Ownership is not transferred. 
     *
     * @since S60 v3.2
     * @return The presence status of this status table entry or NULL
     *         if the value was not set.Owbership is not transferred.
     */    
    TDesC* PresenceStatusString();
    
    /**
     * Sets the presence status string of this status table entry.
     *
     * @since S60 v3.2
     * @param aPresenceStatusString the new presence status string
     * @return KErrNone, if setting was successful and KErrNoMemory
     *         in case an error occurred.
     */  
    TInt SetPresenceStatusString( const TDesC& aPresenceStatusString );
    
    /**
     * Resets the presence status string of this status table entry.
     */
    void ResetPresenceStatusString();
    
private:

    /**
     * Constructor
     *
     * @since S60 v3.2
     * @param aServiceId a service id
     * @param aDisplayStatus status display flag
     */
    CLogsPresenceStatusTableEntry( const TUint32 aServiceId, 
                                   TBool aDisplayStatus );

    /**
     * Symbian Second Phase Constructor.
     *
     * @since S60 v3.2     
     * @param aPresentityId a presentity id     
     * @param aPresenceStatus a presence status e.g."open"     
     */       
    void ConstructL( const TDesC& aPresentityId,
                     TDesC* aPresenceStatus );


private: // data

    /**
     * Service Id
     */
    TUint32 iServiceId;

    /**
     * The PresentityId
     * Own.
     */
    HBufC* iPresentityId;

    /**
     * The Presence status in an integer representation.
     * Own.
     */    
    HBufC* iPresenceStatusString;
    
    /**
     * The Presence status shown in UI or not.
     * Own.
     */    
    TBool iDisplayStatus;

    };


#endif // C_CLOGSPRESENCESTATUSTABLEENTRY_H
