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
* Description:  An operation that writes a contact to (U)SIM card using
*                ETel RMobilePhoneBookStore Write
*
*/



#ifndef VPBKSIMSTOREIMPL_CWRITECMD_H
#define VPBKSIMSTOREIMPL_CWRITECMD_H

//  INCLUDES
#include "CWriteCmdBase.h"

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CBasicStore;

// CLASS DECLARATION

/**
*  An operation that writes a contact to (U)SIM card
*/
NONSHARABLE_CLASS(CWriteCmd) : public CWriteCmdBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        *
        * @param the store for making the ETel request
        * @param aData the data containing the ETel contact
        * @param aSimIndex the index to write the contact or KFirstFreeSimIndex
        *       ETel updates this index if it's KFirstFreeSimIndex
        * @param aObserver an observer to notify after command is done
        * @return a new instance of this class
        */
        static CWriteCmd* NewL( CBasicStore& aStore, const TDesC8& aData, 
            TInt& aSimIndex, MVPbkSimContactObserver& aObserver );
        
        /**
        * Destructor.
        */
        virtual ~CWriteCmd();

    public: // New functions

        /**
        * Starts this operation
        */
        void Execute();
    
    private:    // Construction

        /**
        * C++ constructor
        */
        CWriteCmd( CBasicStore& aStore, TInt& aSimIndex,
            MVPbkSimContactObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( const TDesC8& aData );

    private:    // New functions
        /// Handles error case
        void HandleError( TInt aError );
        /// Removes this instance from the ETel notification
        void RemoveNotificationObserver();
        
    private:    // Data
        /// the store for making the ETel request
        CBasicStore& iStore;
        /// the data containing the ETel contact
        HBufC8* iData;
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CWRITECMD_H
            
// End of File
