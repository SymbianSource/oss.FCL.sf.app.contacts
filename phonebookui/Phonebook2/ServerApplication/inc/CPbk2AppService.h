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
* Description:  Phonebook 2 application server service class.
*
*/


#ifndef CPBK2APPSERVICE_H
#define CPBK2APPSERVICE_H

// INCLUDES
#include <AknServerApp.h>

// FORWARD DECLARATIONS
class MPbk2AppService;

// CLASS DECLARATION

/**
 * Phonebook 2 application server service class.
 * Responsible for:
 * - selecting correct service by interpreting the IPC parameters
 * - performing security checks for the clients
 */
class CPbk2AppService : public CAknAppServiceBase
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2AppService* NewL();

        /**
         * Destructor.
         */
        ~CPbk2AppService();

    private: // From CAknAppServiceBase
        void ServiceL(
                const RMessage2& aMessage );
        CPolicyServer::TCustomResult SecurityCheckL(
                const RMessage2& aMsg,
                TInt& aAction,
                TSecurityInfo& aMissing );

    private: // Implementation
        CPbk2AppService();
        void StoreExitRequest(
                const RMessage2& aMessage );
        void CancelExitRequest(
                const RMessage2& aMessage );
        void StoreAcceptRequest(
                const RMessage2& aMessage );
        void CancelAcceptRequest(
                const RMessage2& aMessage );


    protected: // Data
        /// Own: Exit message used for quering ok to exit
        RMessage2 iExitMsg;
        /// Own: Accept message used for quering acceptance
        RMessage2 iAcceptMsg;
        /// Own: Fetch service
        MPbk2AppService* iFetchService;
        /// Own: Assign service
        MPbk2AppService* iAssignService;
    };

#endif // CPBK2APPSERVICE_H

// End of File
