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
* Description:  Phonebook 2 duplicate contact finder observer interface.
*
*/


#ifndef MPBK2DUPLICATECONTACTOBSERVER_H
#define MPBK2DUPLICATECONTACTOBSERVER_H

// INCLUDES
#include <e32def.h>

// CLASS DECLARATION

/**
 * Phonebook 2 duplicate contact finder observer interface.
 */
class MPbk2DuplicateContactObserver
    {
    public: // Interface

        /**
         * Called after the duplicate find is done. The results
         * are in the given array.
         */
        virtual void DuplicateFindComplete() = 0;

        /**
         * Called if there is a leave during the find.
         *
         * @param aError    A system wide error code.
         */
        virtual void DuplicateFindFailed(
                TInt aError ) = 0;

    protected: // Disabled functions
        ~MPbk2DuplicateContactObserver()
                {}
    };


#endif // MPBK2DUPLICATECONTACTOBSERVER_H
