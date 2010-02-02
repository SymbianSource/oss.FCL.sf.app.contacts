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
* Description:  Phonebook 2 phased service observer interface.
*
*/


#ifndef MPBK2SERVICEPHASEOBSERVER_H
#define MPBK2SERVICEPHASEOBSERVER_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class MPbk2ServicePhase;

// CLASS DECLARATION

/**
 * Phonebook 2 phased service observer interface.
 */
class MPbk2ServicePhaseObserver
    {
    public: // Interface

        /**
         * Phase complete callback.
         * Proceeds to next phase.
         *
         * @param aPhase    The calling phase.
         */
        virtual void NextPhase(
                MPbk2ServicePhase& aPhase ) = 0;

        /**
         * Phase complete callback.
         * Proceeds to previous phase.
         *
         * @param aPhase    The calling phase.
         */
        virtual void PreviousPhase(
                MPbk2ServicePhase& aPhase ) = 0;

        /**
         * Cancel callback.
         *
         * @param aPhase    The calling phase.
         */
        virtual void PhaseCanceled(
                MPbk2ServicePhase& aPhase ) = 0;

        /**
         * Abort callback.
         *
         * @param aPhase    The calling phase.
         */
        virtual void PhaseAborted(
                MPbk2ServicePhase& aPhase ) = 0;

        /**
         * Error callback.
         *
         * @param aPhase    The calling phase.
         * @param aErrorCode    Error code.
         */
        virtual void PhaseError(
                MPbk2ServicePhase& aPhase,
                TInt aErrorCode ) = 0;

        /**
         * Asks a permission to exit.
         *
         * @param aCommandId    Exit command id.
         * @return  ETrue to exit, EFalse to not to exit.
         */
        virtual TBool PhaseOkToExit(
                MPbk2ServicePhase& aPhase,
                TInt aCommandId ) = 0;

        /**
         * Accept callback.
         *
         * @param aMarkedEntries    Number of marked entries.
         * @param aLinkData         Lastly marked contact packed
         *                          in contact link buffer.
         */
        virtual void PhaseAccept(
                MPbk2ServicePhase& aPhase,
                const TDesC8& aMarkedEntries,
                const TDesC8& aLinkData ) = 0;

    protected: // Disabled functions
        virtual ~MPbk2ServicePhaseObserver()
            {};
    };

#endif // MPBK2SERVICEPHASEOBSERVER_H

// End of File
