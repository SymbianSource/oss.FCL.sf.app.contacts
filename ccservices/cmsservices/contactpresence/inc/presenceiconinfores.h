/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Presence in pbonebook
*
*/


#ifndef PRESENCEICONINFORES_H
#define PRESENCEICONINFORES_H

#include <e32std.h>
#include <mcontactpresenceinfo.h>


/**
 * CPresenceIconInfoRes
 *
 * Presence in phonebook
 *
 * @lib contactpresence
 * @since s60 v5.0
 */
NONSHARABLE_CLASS( CPresenceIconInfoRes ) : public CBase, public MContactPresenceInfo
    {
public:

    /**
     * Constructor.
     */
    static CPresenceIconInfoRes* NewL(
        const TDesC8& aBrandId,
        const TDesC8& aElementId,
        const TDesC& aServiceName,
        TInt aOpId);

    virtual ~CPresenceIconInfoRes();  
   
public:  // from base class MContactPresenceInfo 
        
    TPtrC8 BrandId();
    
    /**
     * Element id accessor
     * @return Element id in branding server
     */     
    TPtrC8 ElementId();
    
    /**
     * Service Name accessor
     * @return  Service Name
     */     
    TPtrC ServiceName(); 
    
    TInt OpId();

private:

    /**
     *  constructor
     */
    CPresenceIconInfoRes( TInt aOpId );

    /**
     *  constructor
     */
    void ConstructL(     
        const TDesC8& aBrandId,
        const TDesC8& aElementId,
        const TDesC& aServiceName );

     

private: // Data
    
    /*
     * Temporary save of brand id
     * OWN
     */
    HBufC8* iBrandId;
    
    /*
     * Temporary save of element id
     * OWN
     */
    HBufC8* iElementId;  
    
    /*
     * Temporary save of service name
     * OWN
     */
    HBufC* iServiceName; 
    
    TInt iOpId;
    
    };



#endif // PRESENCEICONINFORES_H
