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
* Description:  Phonebook 2 UI service observer interface.
*
*/


#ifndef MPBK2UISERVICEOBSERVER_H
#define MPBK2UISERVICEOBSERVER_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION

/**
 * Phonebook 2 UI service observer interface.
 */
class MPbk2UiServiceObserver
    {
    public: // Interface

        /**
         * Service complete callback.
         */
        virtual void ServiceComplete() = 0;

        /**
         * Service canceled callback.
         * Cancel indicates exiting the server application only.
         */
        virtual void ServiceCanceled() = 0;

        /**
         * Assign aborted callback.
         * Abort indicates exiting the server application
         * and the client process.
         */
        virtual void ServiceAborted() = 0;

        /**
         * Service error callback.
         *
         * @param aErrorCode    Error code
         */
        virtual void ServiceError(
                TInt aErrorCode ) = 0;

        /**
         * Completes exit message.
         *
         * @param aExitCommandId    Id of the exit command.
         */
        virtual void CompleteExitMessage(
                TInt aExitCommandId ) = 0;

        /**
         * Completes accept message.
         *
         * @param aMarkedEntries    Number of marked entries.
         * @param aLinkData         Lastly marked contact packed
         *                          in contact link buffer.
         */
        virtual void CompleteAcceptMsg(
                const TDesC8& aMarkedEntries,
                const TDesC8& aLinkData ) = 0;

    protected: // Disabled functions
        ~MPbk2UiServiceObserver()
            {};
    };

#endif // MPBK2UISERVICEOBSERVER_H

// End of File
