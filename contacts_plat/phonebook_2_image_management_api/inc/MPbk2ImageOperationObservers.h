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
* Description:  Phonebook 2 image manager observer interface.
*
*/


#ifndef MPBK2IMAGEOPERATIONOBSERVERS_H
#define MPBK2IMAGEOPERATIONOBSERVERS_H

//  INCLUDES
#include <e32def.h>

//  FORWARD DECLARATIONS
class MPbk2ImageOperation;
class CFbsBitmap;

// CLASS DECLARATION

/**
 * Observer interface for CPbk2ImageManager get operation.
 * Only one of the event callbacks is called once by the operation.
 *
 * @see CPbk2ImageManager::GetImageAsyncL
 */
class MPbk2ImageGetObserver
    {
    public: // Interface

        /**
         * Called when image loading is completed.
         *
         * @param aOperation    The completed operation.
         * @param aBitmap       The loaded bitmap. Callee is responsible of
         *                      the bitmap.
         */
        virtual void Pbk2ImageGetComplete(
                MPbk2ImageOperation& aOperation,
                CFbsBitmap* aBitmap ) = 0;

        /**
         * Called if the image loading fails.
         *
         * @param aOperation    The failed operation.
         * @param aError        Error code of the failure.
         */
        virtual void Pbk2ImageGetFailed(
                MPbk2ImageOperation& aOperation,
                TInt aError ) = 0;

    protected: // Disable functions
        ~MPbk2ImageGetObserver()
                {}
    };

/**
 * Observer interface for CPbkI2mageManager set operations.
 * Only one of the event callbacks is called once by the operation.
 *
 * @see CPbk2ImageManager::SetImageAsyncL
 */
class MPbk2ImageSetObserver
    {
    public: // Interface

        /**
         * Called when image setting is completed.
         *
         * @param aOperation    The completed operation.
         */
        virtual void Pbk2ImageSetComplete(
                MPbk2ImageOperation& aOperation ) = 0;

        /**
         * Called if the image setting fails.
         *
         * @param aOperation    The failed operation.
         * @param aError        Error code of the failure.
         */
        virtual void Pbk2ImageSetFailed(
                MPbk2ImageOperation& aOperation,
                TInt aError ) = 0;

    protected: // Disabled functions
        ~MPbk2ImageSetObserver()
                {}
    };

#endif // MPBK2IMAGEOPERATIONOBSERVERS_H

// End of File
