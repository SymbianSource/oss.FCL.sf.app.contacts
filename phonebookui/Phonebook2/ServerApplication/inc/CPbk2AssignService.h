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
* Description:  Phonebook 2 application server assign service.
*
*/


#ifndef CPBK2ASSIGNSERVICE_H
#define CPBK2ASSIGNSERVICE_H

// INCLUDES
#include <e32base.h>
#include "MPbk2AppService.h"
#include "MPbk2UiServiceObserver.h"

// FORWARD DECLARATIONS
class MPbk2UiService;

// CLASS DECLARATION

/**
 * Phonebook 2 application server assign service.
 * Responsible for driving the assign service. Selecting proper assign
 * type and gathering assign results afterwards.
 */
class CPbk2AssignService : public CBase,
                           public MPbk2AppService,
                           public MPbk2UiServiceObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aExitMsg      Exit message.
         * @return  A new instance of this class.
         */
        static CPbk2AssignService* NewL(
                const RMessage2& aExitMsg );

        /**
         * Destructor.
         */
        ~CPbk2AssignService();

        void DoExitServiceL( TInt aExitId );

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
        CPbk2AssignService(
                const RMessage2& aExitMsg );
        void ExitApplicationL();

    private: // Data
        /// Own: Assign complete message
        RMessage2 iAssignCompleteMsg;
        /// Ref: Exit message
        const RMessage2& iExitMsg;
        /// Own: Ui service that performs the assign
        MPbk2UiService* iUiService;
    };

#endif // CPBK2ASSIGNSERVICE_H

// End of File
