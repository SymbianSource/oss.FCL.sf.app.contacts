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
* Description:  Phonebook 2 application server UI service interface.
*
*/


#ifndef MPBK2UISERVICE_H
#define MPBK2UISERVICE_H

// INCLUDES
#include <e32std.h>
#include "Pbk2ServerApp.hrh"

// FORWARD DECLARATIONS
class MVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 application server UI service interface.
 */
class MPbk2UiService
    {
    public: // Interface

        struct TServiceResults
        {
            MVPbkContactLinkArray* iLinkArray;
            TInt iExtraData;
            HBufC* iFieldContent;
        };

        /**
         * Destructor.
         */
        virtual ~MPbk2UiService()
                {}

        /**
         * Launches the service.
         */
        virtual void LaunchServiceL() = 0;

        /**
         * Cancels the service.
         */
        virtual void CancelService() = 0;

        /**
         * Accepts the delayed contacts as part of
         * the contact set used in the service.
         *
         * @param aContactLinkBuffer    Accepted contacts.
         */
        virtual void AcceptDelayedContactsL(
                const TDesC8& aContactLinkBuffer ) = 0;

        /**
         * Exits the service.
         *
         * @param aExitCommandId    The id of the exit command.
         */
        virtual void ExitServiceL(
                TInt aExitCommandId ) = 0;

        /**
         * Retrieves service results.
         *
         * @return  Contact links in an array.
         */
        virtual void ServiceResults(TServiceResults* aResults) const = 0;
    };

#endif // MPBK2UISERVICE_H

// End of File
