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
* Description:  An observer API for find view
*
*/



#ifndef MVPBKSIMVIEWFINDOBSERVER_H
#define MVPBKSIMVIEWFINDOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkSimCntView;
class RVPbkStreamedIntArray;

// CLASS DECLARATION

/**
*  An observer API for find view
*
*/
class MVPbkSimViewFindObserver
    {
    public: // New functions
        
        /**
        * Called after find is completed
        * @param aSimCntView the view into which results applies
        * @param aSimIndexArray an array sim indexes that matched.
        *        If array is empty there is no matches
        */
        virtual void ViewFindCompleted( MVPbkSimCntView& aSimCntView,
            const RVPbkStreamedIntArray& aSimIndexArray ) = 0;

        /**
        * Called if there was an error
        * @param aSimCntView the view into which results applies
        * @param aError the error value
        */
        virtual void ViewFindError( MVPbkSimCntView& aSimCntView, TInt aError ) = 0;

    protected:

        /**
        * Destructor
        */
        virtual ~MVPbkSimViewFindObserver() {}
    };

#endif      // MVPBKSIMVIEWFINDOBSERVER_H
            
// End of File
