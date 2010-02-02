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
* Description:  A parameter class for the store
*
*/



#ifndef VPBKSIMSTOREIMPL_TSTOREPARAMS_H
#define VPBKSIMSTOREIMPL_TSTOREPARAMS_H

//  INCLUDES
#include <e32std.h>
#include "VPbkSimStoreCommon.h"

// FORWARD DECLARATIONS
class CVPbkSimStateInformation;

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS
class CPhone;
class CSatRefreshNotifier;
class CBtSapNotification;

// CLASS DECLARATION

/**
*  A parameter class for the store
*
*/
NONSHARABLE_CLASS( TStoreParams )
    {
    public:  // Constructors and destructor

        /**
        * C++ constructor.
        * @param aId the store identifier
        * @param aPhone the phone
        * @param aSatNotifier a notifier for SAT refresh
        */
        inline TStoreParams( TVPbkSimStoreIdentifier aId, CPhone& aPhone, 
            CSatRefreshNotifier& aSatNotifier,
            CBtSapNotification& aBtSapNotifier,
            CVPbkSimStateInformation& aSimStateInfo,
            TInt aSystemPhoneNumberMaxLength ); 
        
    public:    // Data
        /// The store identifier
        TVPbkSimStoreIdentifier iId;
        /// Ref: the phone
        CPhone& iPhone;
        /// Ref: a notifier for SAT refresh
        CSatRefreshNotifier& iSatNotifier;
        /// Ref: a notifier for bluetooth SIM Access profile
        CBtSapNotification& iBtSapNotifier;
        /// Ref: SIM card state information
        CVPbkSimStateInformation& iSimStateInfo;
        /// Ref: the max length from CentRep
        TInt iSystemPhoneNumberMaxLength;
    };

// INLINE FUNCTIONS
inline TStoreParams::TStoreParams( 
        TVPbkSimStoreIdentifier aId, 
        CPhone& aPhone, 
        CSatRefreshNotifier& aSatNotifier,
        CBtSapNotification& aBtSapNotifier,
        CVPbkSimStateInformation& aSimStateInfo,
        TInt aSystemPhoneNumberMaxLength )
        :   iId( aId ),
            iPhone( aPhone ),
            iSatNotifier( aSatNotifier ),
            iBtSapNotifier( aBtSapNotifier ),
            iSimStateInfo(aSimStateInfo),
            iSystemPhoneNumberMaxLength( aSystemPhoneNumberMaxLength )
    {
    }
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_TSTOREPARAMS_H
            
// End of File
