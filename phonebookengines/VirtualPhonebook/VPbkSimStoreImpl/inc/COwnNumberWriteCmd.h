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
* Description:  A write command for writing own number contacts
*
*/



#ifndef VPBKSIMSTOREIMPL_COWNNUMBERWRITECMD_H
#define VPBKSIMSTOREIMPL_COWNNUMBERWRITECMD_H

//  INCLUDES
#include "CWriteCmdBase.h"
#include <etelmm.h>

namespace VPbkSimStoreImpl {

// CLASS DECLARATION

/**
*  A write command for writing own number contacts
*
*/
NONSHARABLE_CLASS( COwnNumberWriteCmd ) : 
        public CBasicWriteCmd<RMobileONStore::TMobileONEntryV1>
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param the store for making the ETel request
        * @param aData the contact data in ETel format
        * @param aSimIndex the index to write the contact or KFirstFreeSimIndex
        *       ETel updates this index if it's KFirstFreeSimIndex
        * @param aObserver an observer to notify after command is done
        * @return a new instance of this class
        */
        static COwnNumberWriteCmd* NewL( CStoreBase& aStore, 
            const TDesC8& aData, TInt& aSimIndex, 
            MVPbkSimContactObserver& aObserver );
        
        /**
        * Destructor.
        */
        ~COwnNumberWriteCmd();

    private:

        /**
        * C++ constructor.
        */
        COwnNumberWriteCmd( CStoreBase& aStore, TInt& aSimIndex, 
            MVPbkSimContactObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aData );
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_COWNNUMBERWRITECMD_H
            
// End of File
