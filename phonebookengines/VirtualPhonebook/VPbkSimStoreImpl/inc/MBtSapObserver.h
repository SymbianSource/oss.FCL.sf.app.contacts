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


#ifndef VPBKSIMSTOREIMPL_MBTSAPOBSERVER_H
#define VPBKSIMSTOREIMPL_MBTSAPOBSERVER_H

//  INCLUDES
#include <e32std.h>

namespace VPbkSimStoreImpl {


// CLASS DECLARATION

/**
*  An observer for bluetooth SIM Access Profile changes
*
*/
NONSHARABLE_CLASS( MBtSapObserver )
    {
    public: // New functions
        
        /**
        * Called when bluetooth SIM access profile is activated
        */
        virtual void BtSapActivated() = 0;

        /**
        * Called when bluetooth SIM access profile is deactived
        */
        virtual void BtSapDeactivated() = 0;
        
        /**
        * Called when notifcation ends to error
        *
        * @param aError a system wide error code
        */
        virtual void BtSapNotificationError( TInt aError ) = 0;

    protected:  // Constructors and destructor
        
        /**
        * Destructor.
        */
        virtual ~MBtSapObserver() {}        
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_MBTSAPOBSERVER_H
            
// End of File
