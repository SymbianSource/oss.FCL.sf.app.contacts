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
* Description:  Phonebook 2 compress policy interface.
*
*/


#ifndef MPBK2COMPRESSPOLICY_H
#define MPBK2COMPRESSPOLICY_H

// CLASS DECLARATION

/**
 * Phonebook 2 compress policy interface.
 */
class MPbk2CompressPolicy
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2CompressPolicy()
                {}

        /**
         * Starts the policy.
         */
        virtual void Start() = 0;

        /**
         * Cancels the policy activity.
         */
        virtual void Stop() = 0;
    };

#endif // MPBK2COMPRESSPOLICY_H

// End of File
