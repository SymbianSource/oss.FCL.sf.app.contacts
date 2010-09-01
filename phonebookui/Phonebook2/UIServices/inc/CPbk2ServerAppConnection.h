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
* Description:  Phonebook 2 UI Service server application connection.
*
*/


#ifndef CPBK2SERVERAPPCONNECTION_H
#define CPBK2SERVERAPPCONNECTION_H

//  INCLUDES
#include "RPbk2UIService.h"
#include <AknServerApp.h>
#include <Pbk2ServerAppIPC.h>

// FORWARD DECLARATIONS
class CApaServerAppExitMonitor;
class CPbk2ExitServiceMonitor;
class CPbk2AcceptServiceMonitor;
class MPbk2ConnectionObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 UI Service server application connection.
 * Responsible for managing the connection to Phonebook 2 Application Server.
 */
class CPbk2ServerAppConnection : public CActive,
                                 private MAknServerAppExitObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Connection observer.
         * @return  A new instance of this clas.
         */
        static CPbk2ServerAppConnection* NewL(
                MPbk2ConnectionObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ServerAppConnection();

    public: // Interface

        /**
         * Launches assign service.
         *
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aDataPackage              Buffer containing the
         *                                  data to assign.
         * @param aAssignInstructions       Buffer for assign instructions.
         */
        void LaunchAssignL(
                HBufC8* aConfigurationPackage,
                HBufC8* aDataPackage,
                HBufC8* aAssignInstructions );

        /**
         * Launches attribute assign service.
         *
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aAttributePackage         The attribute to assign.
         * @param aAssignInstructions       Assign instructions.
         */
        void LaunchAttributeAssignL(
                HBufC8* aConfigurationPackage,
                TPbk2AttributeAssignData& aAttributePackage,
                HBufC8* aAssignInstructions );

        /**
         * Launches attribute unassign service.
         *
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aAttributePackage         The attribute to unassign.
         * @param aAssignInstructions       Assign instructions.
         */
        void LaunchAttributeUnassignL(
                HBufC8* aConfigurationPackage,
                TPbk2AttributeAssignData aAttributePackage,
                HBufC8* aAssignInstructions );

        /**
         * Cancels assign service.
         */
        void CancelAssign();

        /**
         * Launches fetch service.
         *
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aFetchInstructions        Buffer for fetch instructions.
         */
        void LaunchFetchL(
                HBufC8* aConfigurationPackage,
                HBufC8* aFetchInstructions );

        /**
         * Cancels fetch service.
         */
        void CancelFetch();

    private: // From CActive
        void RunL();
        TInt RunError(
                TInt aError );
        void DoCancel();

    private: // From MAknServerAppExitObserver
        void HandleServerAppExit(
                TInt aReason );

    private: // Implementation
        CPbk2ServerAppConnection(
                MPbk2ConnectionObserver& aObserver );
        void ConnectL();
        void GetResultsL();
        void DisconnectMonitors();
        void DisconnectServerApplication();

    private: // Data
        /// Own: Cancel information
        TBool iCanceled;
        /// Ref: Connection observer
        MPbk2ConnectionObserver& iObserver;
        /// Own: Application server service
        RPbk2UIService iPbk2AppService;
        /// Own: Exit application server monitor
        CApaServerAppExitMonitor* iExitAppServerMonitor;
        /// Own: Exit service monitor
        CPbk2ExitServiceMonitor* iExitServiceMonitor;
        /// Own: Accept service monitor
        CPbk2AcceptServiceMonitor* iAcceptServiceMonitor;
        /// Own: Results buffer for contact link array
        HBufC8* iArrayResults;
        /// Own: Results buffer for field content
        HBufC* iFieldResults;
        /// Own: Extra result data
        TInt iExtraResultData;
    };

#endif // CPBK2SERVERAPPCONNECTION_H

// End of File
