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
* Description:  Phonebook 2 compress policy manager interface.
*
*/


#ifndef MPBK2COMPRESSPOLICYMANAGER_H
#define MPBK2COMPRESSPOLICYMANAGER_H

// CLASS DECLARATION

/**
 * Phonebook 2 compress policy manager interface.
 */
class MPbk2CompressPolicyManager
    {
    public: // Interface

        /**
         * Destructor
         */
        virtual ~MPbk2CompressPolicyManager()
                {}

        /**
         * Start compressing of all the stores.
         */
        virtual void CompressStores() = 0;

        /**
         * Cancel compress operation.
         */
        virtual void CancelCompress() = 0;

        /**
         * Start explicitly all policies.
         */
        virtual void StartAllPolicies() = 0 ;

        /**
         * Stop all policies.
         */
        virtual void StopAllPolicies() = 0;
    };

#endif // MPBK2COMPRESSPOLICYMANAGER_H

// End of File
