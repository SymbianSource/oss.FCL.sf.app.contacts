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
* Description:  Phonebook 2 group operation observer
*
*/


#ifndef MPGUGROUPOPERATIONOBSERVER_H
#define MPGUGROUPOPERATIONOBSERVER_H

//  INCLUDES
#include <e32def.h>

// CLASS DECLARATION
class CPguGroupOperation;

/**
 * Phonebook 2 group operation observer.
 */
class MPguGroupOperationObserver
    {
    public: // Interface

        /**
         * Called after the group operation is completed
         *
         * @param aOperation Completed operation
         * @param aError KErrNone - success
         *               generic error code - Failure
         */
        virtual void GroupOperationComplete(
            const CPguGroupOperation* aOperation,
            TInt aError ) = 0;

    protected: // Protected functions
        virtual ~MPguGroupOperationObserver() {}
    };

#endif // MPGUGROUPOPERATIONOBSERVER_H

// End of File
