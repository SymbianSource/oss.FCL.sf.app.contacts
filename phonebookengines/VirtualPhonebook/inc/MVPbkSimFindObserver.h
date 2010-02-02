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
* Description:  An observer API for find results
*
*/



#ifndef MVPBKSIMFINDOBSERVER_H
#define MVPBKSIMFINDOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkSimCntStore;
class RVPbkStreamedIntArray;

// CLASS DECLARATION

/**
*  An observer API for find results
*
*/
class MVPbkSimFindObserver
    {
    public: // New functions
        
        /**
        * Called after find is completed
        * @param aStore the store into which results applies
        * @param aSimIndexArray an array sim indexes that matched.
        *        If array is empty there is no matches
        */
        virtual void FindCompleted( MVPbkSimCntStore& aStore,
            const RVPbkStreamedIntArray& aSimIndexArray ) = 0;

        /**
        * Called if there was an error
        * @param aStore the store into which results applies
        * @param aError the error value
        */
        virtual void FindError( MVPbkSimCntStore& aStore, TInt aError ) = 0;

    protected: 

        /**
        * Destructor
        */
        virtual ~MVPbkSimFindObserver() {}
    };

#endif      // MVPBKSIMFINDOBSERVER_H
            
// End of File
