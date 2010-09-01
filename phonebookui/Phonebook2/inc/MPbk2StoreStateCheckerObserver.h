/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 Store state checker observer
*
*/


#ifndef MPBK2STORESTATECHECKEROBSERVER_H
#define MPBK2STORESTATECHECKEROBSERVER_H

/**
 * Phonebook 2 Store state checker callback
 */
class MPbk2StoreStateCheckerObserver
    {
    public:
     enum TState
            {
            /// Store is available
            EStoreAvailable,
            /// Store is unavailables
            EStoreUnavailable
            };
            
    public:  // Interface

        /**
         * Store state callback
         *
         * @param aState  State of the store
         */
        virtual void StoreState( 
            MPbk2StoreStateCheckerObserver::TState aState ) = 0;

    };

#endif // MPBK2STORESTATECHECKEROBSERVER_H

// End of File
