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
* Description:  An observer API for ETel store events
*
*/



#ifndef VPBKSIMSTOREIMPL_METELSTORENOTIFICATIONOBSERVER_H
#define VPBKSIMSTOREIMPL_METELSTORENOTIFICATIONOBSERVER_H

//  INCLUDES
#include <e32std.h>

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  An observer API for ETel store events
*
*/
NONSHARABLE_CLASS( METelStoreNotificationObserver )
    {
    public: // New functions
        
        /**
        * Called when ETel store notification arrives
        * @param aSimIndex the index that has been changed
        * @param aEvent the ETel store event bit field
        * @return ETrue if the class handles the update
        */
        virtual TBool ETelStoreChanged( TInt aSimIndex, TUint32 aEvents ) = 0;

        /**
        * Called when ETel store notification is completed with error
        * @param aError the error code received from ETel
        * @return ETrue if the error is handled
        */
        virtual TBool ETelStoreChangeError( TInt aError ) = 0;

    protected:    // Destruction

        /**
        * Destructor.
        */
        virtual ~METelStoreNotificationObserver() {}
    };
}   // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_METELSTORENOTIFICATIONOBSERVER_H
            
// End of File
