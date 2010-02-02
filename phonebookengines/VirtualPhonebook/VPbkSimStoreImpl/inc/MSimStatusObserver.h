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
* Description:  An observer for bluetooth SIM Access Profile changes
*
*/


#ifndef VPBKSIMSTOREIMPL_MSIMSTATUSOBSERVER_H
#define VPBKSIMSTOREIMPL_MSIMSTATUSOBSERVER_H

//  INCLUDES
#include <e32std.h>

namespace VPbkSimStoreImpl {


// CLASS DECLARATION

/**
*  An observer for bluetooth SIM Access Profile changes
*
*/
NONSHARABLE_CLASS( MSimStatusObserver )
    {
    public: // New functions
        
        /**
        * Called when SIM status has changed to OK
        */
        virtual void SimStatusOk() = 0;

        /**
        * Called when SIM status has changed to NOT OK.
        */
        virtual void SimStatusNotOk() = 0;
        
        /**
        * Called when SIM status has changed to UNINITIALIZED.
        */
        virtual void SimStatusUninitialized() = 0;
        
        /**
        * Called when notifcation ends to error
        *
        * @param aError a system wide error code
        */
        virtual void SimStatusNotificationError( TInt aError ) = 0;

    protected:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MSimStatusObserver() {}        
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_MSIMSTATUSOBSERVER_H
            
// End of File
