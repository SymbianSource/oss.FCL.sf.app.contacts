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
* Description:  
*
*/


#ifndef MPBK2IMAGEFIELDOBSERVER_H
#define MPBK2IMAGEFIELDOBSERVER_H

// INCLUDES


// FORWARD DECLATIONS


// CLASS DECLARATION

/**
 * Phonebook 2 contact editor image field observer
 */
class MPbk2ImageFieldObserver
    {
    public: // Interface

        /**
         * Called when image has bee loaded.
         * Returns system error code
         */
        virtual TInt ImageLoadingComplete() = 0;


        /**
         * Called when image loading failed.
         * Returns system error code
         */
        virtual TInt ImageLoadingFailed() = 0;


        /**
         * Called when image loading was cancelled.
         * Returns system error code
         */
        virtual TInt ImageLoadingCancelled() = 0;



    };

#endif // MPBK2IMAGEFIELDOBSERVER

// End of File
