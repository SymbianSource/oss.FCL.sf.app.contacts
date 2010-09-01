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


#ifndef __CONTACTPRESENCEAPIHANDLER__
#define __CONTACTPRESENCEAPIHANDLER__

// INCLUDE FILES
#include <e32base.h>
#include <mcontactpresenceobs.h>

class CContactPresenceApiTester;

class CContactPresenceApiHandler : public CBase, public MContactPresenceObs
    {
    public:

        static CContactPresenceApiHandler* NewL( CContactPresenceApiTester& aTesterMain );
        
        ~CContactPresenceApiHandler();

    private:

        CContactPresenceApiHandler( CContactPresenceApiTester& aTesterMain );

        void ConstructL();
        
    private:  //From MContactPresenceObs
        
        /**
        * Receive contact's presence icon info.
        * @param aPackedLink a contact link    
        * @param aBrandId branding id
        * @param aElementId element id (AKA aImageId)
        */
        void ReceiveIconInfoL( const TDesC8& aPackedLink, const TDesC8& aBrandId, const TDesC8& aElementId );

        /**
        * Receive Icon File    
        * @param aBrandedBitmap bitmap
        * @param aMask mask
        * @param aChangeOwner if this value is changed to ETrue then the ownership of the params is transferred [OUT]      
        */
        void ReceiveIconFileL( const TDesC8& aBrandId, const TDesC8& aElementId, 
                               CFbsBitmap* aBrandedBitmap, CFbsBitmap* aMask );
        
        /**
        * Receive Icon File    
        * @param aBrandedBitmap bitmap
        * @param aMask mask
        * @param aChangeOwner if this value is changed to ETrue then the ownership of the params is transferred [OUT]      
        */
        void PresenceSubscribeError( const TDesC8& aContactLink, TInt aStatus );

        /**
        * Error occurred in the request
        * @param aOpId operation id to idetify the operation [IN]     
        * @param aStatus error status
        */
        void ErrorOccured( TInt aOpId, TInt aStatus ); 
        

        void ReceiveIconInfosL(
            const TDesC8& /*aPackedLink*/,
            RPointerArray <MContactPresenceInfo>& aInfoArray,
            TInt aOpId );          
        
    private:
    
        CContactPresenceApiTester&                iTesterMain;
        
    };
    
#endif
