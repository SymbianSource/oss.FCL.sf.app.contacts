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
* Description:  Phonebook 2 application server fetch service.
*
*/


#ifndef CPBK2FETCHSERVICE_H
#define CPBK2FETCHSERVICE_H

// INCLUDES
#include <e32base.h>
#include "MPbk2AppService.h"
#include "Pbk2ServerApp.hrh"
#include "MPbk2UiServiceObserver.h"

// FORWARD DECLARATIONS
class MPbk2UiService;

// CLASS DECLARATION

/**
 * Phonebook 2 application server fetch service.
 * Responsible for driving the fetch service. Selecting proper fetch
 * type and gathering fetch results afterwards.
 */
class CPbk2FetchService : public CBase,
                          public MPbk2AppService,
                          public MPbk2UiServiceObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aExitMsg      Exit message.
         * @param aAcceptMsg    Accept message.
         * @return  A new instance of this class.
         */
        static CPbk2FetchService* NewL(
                const RMessage2& aExitMsg,
                const RMessage2& aAcceptMsg );

        /**
         * Destructor.
         */
        ~CPbk2FetchService();

    public: // From MPbk2AppService
        void LaunchServiceL(
                const RMessage2& aMessage );
        void CancelService(
                const RMessage2& aMessage );
        void GetResultsL(
                const RMessage2& aMessage );
        void GetResultSizeL(
                const RMessage2& aMessage );
        void TryExitServiceL(
                const RMessage2& aMessage );
        void TryAcceptServiceL(
                const RMessage2& aMessage );

    public: // From MPbk2UiServiceObserver
        void ServiceComplete();
        void ServiceCanceled();
        void ServiceAborted();
        void ServiceError(
                TInt aErrorCode );
        void CompleteExitMessage(
                TInt aExitCommandId );
        void CompleteAcceptMsg(
                const TDesC8& aMarkedEntries,
                const TDesC8& aLinkData );

    private: // Implementation
        CPbk2FetchService(
                const RMessage2& aExitMsg,
                const RMessage2& aAcceptMsg );
        void LaunchEntryFetchL(
                const RMessage2& aMessage,
                TPbk2FetchType aFetchType );
        void LaunchItemFetchL(
                const RMessage2& aMessage,
                TPbk2FetchType aFetchType );
        TPbk2FetchType FetchTypeL(
                const RMessage2& aMessage );

    private: // Data
        /// Own: Fetch complete message
        RMessage2 iFetchCompleteMsg;
        /// Ref: Exit message
        const RMessage2& iExitMsg;
        /// Ref: Accept message
        const RMessage2& iAcceptMsg;
        /// Own: Ui service that performs the fetch
        MPbk2UiService* iUiService;
    };

#endif // CPBK2FETCHSERVICE_H

// End of File
