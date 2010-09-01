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
* Description:  
*
*/


#ifndef CMSONESERVICESTATUS_H_
#define CMSONESERVICESTATUS_H_


#include <e32base.h>
#include "mpresencetrafficlightsobs.h"


class MVPbkContactLink;

/**
 *  Branded presence icons handling class.
 *
 *  @lib cmsserver.exe
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS(CCmsOneServiceStatus):  public CBase, public MPresenceServiceIconInfo
    {

public: 

    /**
     * Symbian constructors.
     *
     * @param aCallback callback method for notifications
     * @return CCmsBrandedIcons
     */
    static CCmsOneServiceStatus* NewL(  
            const TDesC& aService,            
            const TDesC8& aBrandId,
            TInt aBrandLanguage,
            TBool aIsAvailable);

    /**
     * Public destructor. 
     */
    virtual ~CCmsOneServiceStatus();
    
// new methods

    void SetAvailability( TBool aIsAvailable );
    
    void SetElementL( const TDesC8& aElementId );    
    
    TPtrC8 ElementId();
    
    TBool Availability( );
        
    TBool IsSameService( const TDesC& aService );
    
    TPtrC ServiceName();  
    
    TPtrC8 BrandId();     
    
    TInt BrandLanguage();
                
private:
 
    CCmsOneServiceStatus( TBool aIsAvailable, TInt aBrandLanguage  );            
    
    void ConstructL( const TDesC& aService, const TDesC8& aBrandId );
    
// new methods
       
        
private:

    TBool iPresenceAvailable;
    HBufC* iService;    
    HBufC8* iBrandId;
    HBufC8* iElementId;    
    TInt   iBrandLanguage;
    };

#endif /*CMSONESERVICESTATUS_H_*/
