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



#ifndef VPBKSIMSTOREIMPL_CSDNSTORE_H
#define VPBKSIMSTOREIMPL_CSDNSTORE_H

//  INCLUDES
#include "CBasicStore.h"

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  An ADN store that interferes with active FDN
*
*/
NONSHARABLE_CLASS(CSdnStore) : public CBasicStore
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * @param aParams the store parameters
        * @return a new instance of this class
        */
        static CSdnStore* NewL( TStoreParams& aParams );
        
        /**
        * Destructor.
        */
        virtual ~CSdnStore();

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
        */
        TUint32 ElementaryFiles();
        
    private:

        /**
        * C++ default constructor.
        */
        CSdnStore( TStoreParams& aParams );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    };

} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CSDNSTORE_H
            
// End of File
