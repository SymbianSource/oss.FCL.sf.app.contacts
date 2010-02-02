/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An ADN store
*
*/



#ifndef VPBKSIMSTOREIMPL_CADNSTORE_H
#define VPBKSIMSTOREIMPL_CADNSTORE_H

//  INCLUDES
#include "CCustomStore.h"
#include <MVPbkSimPhoneObserver.h>

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  An ADN store that interferes with active FDN
*
*/
NONSHARABLE_CLASS(CAdnStore) 
        : public CCustomStore,
          private MVPbkSimPhoneObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParams the store parameters
        * @return a new instance of this class
        */
        static CAdnStore* NewL( TStoreParams& aParams );
        
        /**
        * Destructor.
        */
        virtual ~CAdnStore();

    public: // Functions from base classes

        /**
        * From CStoreBase
        */
        const TDesC& ETelName() const;
        
        /**
        * From CStoreBase
        */
        void CompleteSATRefresh( 
            MSimRefreshCompletion& aSimRefreshCompletion );
            
        /**
        * From CStoreBase
        * 
        * Checks the FDN activity
        */
        TBool AllowOpeningL();
        
        /**
        * From MSimRefreshObject
        */
        TUint32 ElementaryFiles();
    
    private:    // Functions from base classes
    
        /**
        * From MVPbkSimPhoneObserver
        */ 
        void PhoneOpened( MVPbkSimPhone& aPhone );

        /**
        * From MVPbkSimPhoneObserver
        */ 
        void PhoneError( MVPbkSimPhone& aPhone, 
            TErrorIdentifier aIdentifier, TInt aError );
        
        /**
        * From MVPbkSimPhoneObserver
        */ 
        void ServiceTableUpdated( TUint32 aServiceTable );
        
        /**
        * From MVPbkSimPhoneObserver
        */ 
        void FixedDiallingStatusChanged(
            TInt aFDNStatus );
            
    private:

        /**
        * C++ default constructor.
        */
        CAdnStore( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // New functions
        void HandleFDNStatusChangeL();

    private:    // Data
    
    };

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CADNSTORE_H
            
// End of File
