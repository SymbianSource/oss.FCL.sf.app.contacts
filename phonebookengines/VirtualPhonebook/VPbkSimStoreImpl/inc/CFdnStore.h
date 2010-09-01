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
* Description:  An FDN store
*
*/



#ifndef VPBKSIMSTOREIMPL_CFDNSTORE_H
#define VPBKSIMSTOREIMPL_CFDNSTORE_H

//  INCLUDES
#include "CCustomStore.h"

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  An FDN store
*
*/
NONSHARABLE_CLASS(CFdnStore) : public CCustomStore
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParams the store parameters
        * @return a new instance of this class
        */
        static CFdnStore* NewL( TStoreParams& aParams );
        
        /**
        * Destructor.
        */
        virtual ~CFdnStore();

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
        * From MSimRefreshObject
        */
        TUint32 ElementaryFiles();
        
    private:

        /**
        * C++ default constructor.
        */
        CFdnStore( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CFDNSTORE_H
            
// End of File
