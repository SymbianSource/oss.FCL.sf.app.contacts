/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef CVPBKSIMPHONE_H
#define CVPBKSIMPHONE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSimPhone.h>
#include <VPbkSimServerCommon.h> // TVPbkSimContactEventData
#include "RVPbkSimStore.h"

// CLASS DECLARATIONS

NONSHARABLE_CLASS(CVPbkSimPhone) : public CActive, public MVPbkSimPhone
    {
    public:
        static CVPbkSimPhone* NewL();
        
        ~CVPbkSimPhone();
        
    public: // interface
        /**
        * Adds an observer for the store
        * @param aObserver a new observer
        */
        void AddObserverL(MVPbkSimPhoneObserver& aObserver);

        /**
        * Removes an observer
        * @param aObserver the observer to be removed
        */
        void RemoveObserver(MVPbkSimPhoneObserver& aObserver);
        
    public: // From CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();
    
    public: // From MVPbkSimPhone
        void OpenL( MVPbkSimPhoneObserver& aObserver );
        void Close( MVPbkSimPhoneObserver& aObserver );
        TBool USimAccessSupported() const;
        TUint32 ServiceTable() const;
        TFDNStatus FixedDialingStatus() const;    
    
    private: // Implementation
        CVPbkSimPhone();
        void ConstructL();
        
    private: // data
        /// Own: The sim store subsession
        RVPbkSimStore iSimStore;
        /// Ref: the observers of the phone
        RPointerArray<MVPbkSimPhoneObserver> iObservers;
        /// Own: event data
        TVPbkSimContactEventData iEventData;
    };
    
#endif // CVPBKSIMPHONE_H

// End of file
