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
* Description:  Phonebook 2 application server service interface.
*
*/


#ifndef MPBK2APPSERVICE_H
#define MPBK2APPSERVICE_H

// FORWARD DECLARATIONS
class RMessage2;

// CLASS DECLARATION

/**
 * Phonebook 2 application server service interface.
 */
class MPbk2AppService
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2AppService()
                {}

        /**
         * Launches the service.
         *
         * @param aMessage  The message object containing further info.
         */
        virtual void LaunchServiceL(
                const RMessage2& aMessage ) = 0;

        /**
         * Cancels the service.
         *
         * @param aMessage  The message object containing further info.
         */
        virtual void CancelService(
                const RMessage2& aMessage ) = 0;

        /**
         * Gets results of the service.
         *
         * @param aMessage  The message object the results are written into.
         */
        virtual void GetResultsL(
                const RMessage2& aMessage ) = 0;

        /**
         * Gets result size.
         *
         * @param aMessage  The message object the size is written into.
         */
        virtual void GetResultSizeL(
                const RMessage2& aMessage ) = 0;

        /**
         * Notifies the service that the client side has processed the
         * exit notification. The answer is in the given message,
         * the service will inspect the message and decide whether to
         * exit or not.
         *
         * @param aMessage  The message containing further info.
         */
        virtual void TryExitServiceL(
                const RMessage2& aMessage ) = 0;

        /**
         * Notifies the service that the client side has processed the
         * accept notification. The answer is in the given message,
         * the service will inspect the message and decide whether to
         * accept or not.
         *
         * @param aMessage  The message containing further info.
         */
        virtual void TryAcceptServiceL(
                const RMessage2& aMessage ) = 0;
    };

#endif // MPBK2APPSERVICE_H

// End of File
