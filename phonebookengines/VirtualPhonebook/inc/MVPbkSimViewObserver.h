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
* Description:  An observer API for view events
*
*/



#ifndef MVPBKSIMVIEWOBSERVER_H
#define MVPBKSIMVIEWOBSERVER_H

//  INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MVPbkSimCntView;

// CLASS DECLARATION

/**
*  An observer API for view events
*
*/
class MVPbkSimViewObserver
    {
    public: // Types

        /**
        * SIM contact events
        */
        enum TEvent
            {
            /// Unknown operation
            EUnknown = 0,
            /// A contact was added to the view
            EContactAdded,
            /// A contact was deleted from the view
            EContactDeleted,
            };

    public: // New functions
        
        /**
        * Called after the view is succesfully created
        * @param aView the view that was created
        */
        virtual void ViewReady( MVPbkSimCntView& aView ) = 0;

        /**
        * Called if there was an error in view creation
        * @param aView the view that fail to open
        * @param aError a system wide error code for failure
        */
        virtual void ViewError( MVPbkSimCntView& aView, TInt aError ) = 0;

        /**
        * Called if the view is not available. E.g. SAT refresh happens.
        * @param aView the view that is not available
        */
        virtual void ViewNotAvailable( MVPbkSimCntView& aView ) = 0;

        /**
        * Called when there is changes in the view
        * @param aEvent the view event
        * @param the view index that changed
        * @param the changed contact sim index
        */
        virtual void ViewContactEvent( TEvent aEvent, TInt aIndex, 
            TInt aSimIndex ) = 0;

    protected:    // Destruction

        /**
        * Destructor.
        */
        virtual ~MVPbkSimViewObserver() {}
    };

#endif      // MVPBKSIMVIEWOBSERVER_H
            
// End of File
