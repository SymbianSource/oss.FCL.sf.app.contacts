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
* Description:  Phonebook 2 UI Service IPC client.
*
*/


#ifndef RPBK2UISERVICE_H
#define RPBK2UISERVICE_H

// INCLUDES
#include <AknServerApp.h>
#include <Pbk2ServerAppIPC.h>

// CLASS DECLARATION

/**
 * Phonebook 2 UI Service IPC client.
 * Responsible for handling IPC traffic with Phonebook 2 Application Server.
 */
class RPbk2UIService : public RAknAppServiceBase
    {
    public: // Constructor

        /**
         * Constructor.
         */
        RPbk2UIService();

    public: // Interface

        /**
         * Launches assign service.
         *
         * @param aStatus                   Request status.
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aDataPackage              Buffer containing the
         *                                  data to assign.
         * @param aAssignInstructions       Buffer for assign instructions.
         * @param aCanceled                 Cancel information.
         */
        void LaunchAssignL(
                TRequestStatus& aStatus,
                HBufC8* aConfigurationPackage,
                HBufC8* aDataPackage,
                HBufC8* aAssignInstructions,
                TBool& aCanceled );

        /**
         * Launches attribute assign service.
         *
         * @param aStatus                   Request status.
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aAttributePackage         The attribute to assign.
         * @param aAssignInstructions       Buffer for assign instructions.
         * @param aCanceled                 Cancel information.
         */
        void LaunchAttributeAssignL(
                TRequestStatus& aStatus,
                HBufC8* aConfigurationPackage,
                TPbk2AttributeAssignData& aAttributePackage,
                HBufC8* aAssignInstructions,
                TBool& aCanceled );

        /**
         * Launches attribute unassign service.
         *
         * @param aStatus                   Request status.
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aAttributePackage         The attribute to unassign.
         * @param aAssignInstructions       Buffer for assign instructions.
         * @param aCanceled                 Cancel information.
         */
        void LaunchAttributeUnassignL(
                TRequestStatus& aStatus,
                HBufC8* aConfigurationPackage,
                TPbk2AttributeAssignData aAttributePackage,
                HBufC8* aAssignInstructions,
                TBool& aCanceled );

        /**
         * Launches fetch service.
         *
         * @param aStatus                   Request status.
         * @param aConfigurationPackage     Buffer containing
         *                                  configuration data.
         * @param aAssignInstructions       Buffer for fetch instructions.
         * @param aCanceled                 Cancel information.
         */
        void LaunchFetchL(
                TRequestStatus& aStatus,
                HBufC8* aConfigurationPackage,
                HBufC8* aFetchInstructions,
                TBool& aCanceled );

        /**
         * Cancels assign service.
         *
         * @param aCanceled                 Cancel information.
         */
        void CancelAssign(
                TBool& aCanceled );


        /**
         * Cancels fetch service.
         *
         * @param aCanceled                 Cancel information.
         */
        void CancelFetch(
                TBool& aCanceled );

        /**
         * Retrieves results.
         *
         * @param aArrayResults  Buffer where the contact link array results
         *                       will be fetched into.
         * @param aFieldResults  Buffer where the field content results
         *                       will be fetched into.
         */
        void GetResultsL(
                HBufC8& aArrayResults,
                TInt& aExtraData,
                HBufC& aFieldResults );

        /**
         * Retrieves result size.
         *
         * @param aArrayResultSize  Buffer where the contact link array size
         *                          will be fetched into.
         * @param aFieldResultSize  Buffer where the field content size
         *                          will be fetched into.
         */
        void GetResultSizeL(
                TInt& aArrayResultSize,
                TInt& aFieldResultSize );

        /**
         * Requests exit notification.
         *
         * @param aStatus           Request status.
         * @param aExitCommandId    Id of the exit command.
         */
        void ExitRequestL(
                TRequestStatus& aStatus,
                TInt& aExitCommandId );

        /**
         * Cancels exit notification request.
         */
        void CancelExitRequest();

        /**
         * Sends exit command to the service.
         *
         * @param aAcceptOutput     Indicates whether the exit is
         *                          acceptable.
         * @param aExitCommandId    Exit command id.
         */
        void ExitServiceL(
                TBool aAcceptOutput,
                TInt aExitCommandId );

        /**
         * Requests accept notification.
         *
         * @param aStatus           Request status.
         * @param aNumContacts      Number of contacts.
         * @param aContactLink      Buffer for contact links.
         */
        void AcceptRequestL(
                TRequestStatus& aStatus,
                TInt& aNumContacts,
                HBufC8& aContactLink );

        /**
         * Cancels accept notification request.
         */
        void CancelAcceptRequest();

        /**
         * Sends accept command to the service.
         *
         * @param aAcceptOutput     Indicates whether to accept or not.
         * @param aContactLink      Buffer for contact links, can be NULL.
         */
        void AcceptServiceL(
                TBool aAcceptOutput,
                HBufC8* aContactLink );

    private: // From RAknAppServiceBase
        TUid ServiceUid() const;

    private: // Data
        /// Own: Configuration package
        TPtr8 iConfigurationPackagePtr;
        /// Own: Data package
        TPtr8 iDataPackagePtr;
        /// Own: Instructions
        TPtr8 iInstructionsPtr;
        /// Own: Results size for contant link array
        TPtr8 iResultArraySizePtr;
        /// Own: Results for contact links
        TPtr8 iArrayResultsPtr;
        /// Own: Cancel information
        TPtr8 iCanceledPtr;
        /// Own: Id of the exit command
        TPtr8 iExitCommandIdPtr;
        /// Own: Number of contacts
        TPtr8 iCountPtr;
        /// Own: Contact link
        TPtr8 iContactLinkPtr;
        /// Own: Results size for field content
        TPtr8 iResultFieldSizePtr;
        /// Own: Results for field content
        TPtr16 iFieldResultsPtr;

    };

#endif // RPBK2UISERVICE_H

// End of File
