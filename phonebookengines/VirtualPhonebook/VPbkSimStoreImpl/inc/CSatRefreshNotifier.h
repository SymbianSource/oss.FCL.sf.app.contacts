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
* Description:  A notifier that gets refresh events from the SAT server
*
*/



#ifndef VPBKSIMSTOREIMPL_CSATREFRESHNOTIFIER_H
#define VPBKSIMSTOREIMPL_CSATREFRESHNOTIFIER_H

//  INCLUDES
#include <e32base.h>
#include <msatrefreshobserver.h>
#include "MSimRefreshObject.h"

namespace VPbkSimStoreImpl {

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  A notifier that gets refresh events from the SAT server
*
*/
NONSHARABLE_CLASS(CSatRefreshNotifier) : public CBase,
                            private MSatRefreshObserver,
                            private MSimRefreshCompletion
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CSatRefreshNotifier* NewL();
        
        /**
        * Destructor.
        */
        virtual ~CSatRefreshNotifier();

    public: // New functions
        
        /**
        * Opens session to SAT server and activates notification
        * if refresh object has files that are not already listened to.
        *
        * @param aSimRefreshObject the object that needs to listen to refresh.
        */
        void ActivateL( MSimRefreshObject& aSimRefreshObject );
        
        /**
        * Deactivates listening refresh.
        *
        * @param aSimRefreshObject the object that doesn't need 
        *                          notification anymore.
        */
        void DeActivate( MSimRefreshObject& aSimRefreshObject );
        
        /**
        * Returns ETrue if notifier has been activated
        * @return ETrue if notifier has been activated
        */
        TBool IsActive();
        
    private:  // From MSatRefreshObserver
        
        TBool AllowRefresh( TSatRefreshType aType, 
            const TSatRefreshFiles& aFiles );
        void Refresh( TSatRefreshType aType, const TSatRefreshFiles& aFiles );
    
    private:    // From MSimRefreshCompletion
        void SatRefreshCompleted( MSimRefreshObject& aSimRefreshObject,
            TBool aReadAdditionalFiles );
            
    private:

        /**
        * C++ default constructor.
        */
        CSatRefreshNotifier();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        /// Own: a C-class that owns SAT session
        class CSatSession;
        CSatSession* iSatSession;
        /// An array of observers. Instances are not owned.
        RPointerArray<MSimRefreshObject> iObjects;
        /// The number of object that have completed refresh
        TInt iRefreshedObjects;
        /// ETrue if some object has read other than SAT-defined EFs.
        TBool iAdditionalEFsRead;        
    };
} // namespace VPbkSimStoreImpl
#endif      // VPBKSIMSTOREIMPL_CSATREFRESHNOTIFIER_H
            
// End of File
