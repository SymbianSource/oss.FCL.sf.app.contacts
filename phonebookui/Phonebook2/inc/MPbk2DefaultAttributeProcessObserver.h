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
* Description:  Phonebook 2 memory entry default attribute
*              : process observer interface.
*
*/


#ifndef MPBK2DEFAULTATTRIBUTEPROCESSOBSERVER_H
#define MPBK2DEFAULTATTRIBUTEPROCESSOBSERVER_H

// CLASS DECLARATION

/**
 * Phonebook 2 memory entry default attribute process observer interface.
 */
class MPbk2DefaultAttributeProcessObserver
    {
    public: // Interface

        /**
         * Notifies attribute process completed succesfully.
         */
        virtual void AttributeProcessCompleted() = 0;

        /**
         * Notifies attribute process failed.
         *
         * @param aErrorCode    Error code.
         */
        virtual void AttributeProcessFailed(
                TInt aErrorCode ) = 0;

    protected: // Disabled functions
        ~MPbk2DefaultAttributeProcessObserver()
                {}
    };

#endif // MPBK2DEFAULTATTRIBUTEPROCESSOBSERVER_H

// End of File
