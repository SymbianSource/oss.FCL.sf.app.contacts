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
* Description:  Phonebook 2 UI Services connection observer interface.
*
*/


#ifndef MPBK2CONNECTIONOBSERVER_H
#define MPBK2CONNECTIONOBSERVER_H

// INCLUDES
#include "Pbk2UIServices.hrh"

// CLASS DECLARATION

/**
 * Phonebook 2 UI Services connection observer interface.
 */
class MPbk2ConnectionObserver
    {
    public: // Interface

        /**
         * Operation complete callback.
         *
         * @param aContactLinks     Links to the contacts that were
         *                          assigned some data.
         * @param aExtraResultData  Additional data
         * @param aField            Field content of the selected contacts.
         */
        virtual void OperationCompleteL(
                const TDesC8& aContactLinks, TInt aExtraResultData,
                const TDesC& aField ) = 0;

        /**
         * Operation canceled callback.
         */
        virtual void OperationCanceledL() = 0;

        /**
         * Operation error callback.
         *
         * @param   aError      Error code.
         */
        virtual void OperationErrorL(
                TInt aError ) = 0;

        /**
         * Ok to exit callback.
         *
         * @param aCommandId        Id of the exit command.
         * @param aExitParameter    Indicates whether to exit from
         *                          server application or from
         *                          client process.
         * @return  ETrue if it is ok to exit.
         */
        virtual TBool OkToExitL(
                TInt aCommandId,
                TPbk2ExitCommandParams aExitParameter ) = 0;

        /**
         * Accept selection verify callback.
         *
         * @param aNumberOfSelectedContacts     Number of selected contacts.
         * @param aContactLink                  Link to the contact currently
         *                                      being selected.
         * @return  ETrue if selection is accepted.
         */
        virtual TBool AcceptSelectionL(
                TInt aNumberOfSelectedContacts,
                HBufC8& aContactLink ) = 0;

    protected: // Disabled functions
        ~MPbk2ConnectionObserver()
            {}
    };

#endif // MPBK2CONNECTIONOBSERVER_H

// End of File
