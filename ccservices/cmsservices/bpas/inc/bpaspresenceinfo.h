/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Presence information holder
*
*/



#ifndef C_BPASPRESENCEINFO_H
#define C_BPASPRESENCEINFO_H

// #include <presencefeeder.h> // only for availability enums
#include <mpresencebuddyinfo2.h>


/**
 * bpas presence info class. It is created only if a buddy's presence contains
 * both availability text and availability enumeration values. 
 *
 *  @lib bpas.lib
 *  @since S60 v5.0
 */
class CBPASPresenceInfo: public CBase
    {

public: // From MBPASPresenceInfo


    /**
     * Symbian constructors. Not exported because only used inside bpas.
     *
     * @param aAvailabilityText availability text value
     * @param aAvailabilityEnum availability enumeration value
     * @return CBPASPresenceInfo* newly created object
     */
    static CBPASPresenceInfo* NewL(const TDesC& aAvailabilityText,
            MPresenceBuddyInfo2::TAvailabilityValues aAvailabilityEnum);
    static CBPASPresenceInfo* NewLC(const TDesC& aAvailabilityText,
            MPresenceBuddyInfo2::TAvailabilityValues aAvailabilityEnum);
    
    /**
     * Public destructor. MBPASPresenceInfo object
     * can be deleted through this interface.
     */
    virtual ~CBPASPresenceInfo();
    

    /**
     * Get presence availability text attribute from presence info
     *
     * @return presence availability text
     */    
    IMPORT_C const TPtrC AvailabilityText( ) const;
     

    /**
     * Get presence availability enum attribute from presence info
     *
     * @return presence availability enumeration value
     */    
    IMPORT_C MPresenceBuddyInfo2::TAvailabilityValues AvailabilityEnum( ) const;


private:
 
    /**
     * C++ and Symbian second phase constructors
     *
     */    
    CBPASPresenceInfo(MPresenceBuddyInfo2::TAvailabilityValues aAvailabilityEnum);
    void ConstructL(const TDesC& aAvailabilityText);

private:

    // Availability text value owned
    HBufC*      iAvailabilityText;
    
    // Availability enumeration value owned
    MPresenceBuddyInfo2::TAvailabilityValues iAvailabilityEnum;// add not defined value

    };


#endif // C_BPASPRESENCEINFO_H
