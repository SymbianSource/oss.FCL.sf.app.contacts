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
* Description:  Phonebook 2 process decorator interface.
*
*/


#ifndef MPBK2PROCESSDECORATOR_H
#define MPBK2PROCESSDECORATOR_H

#include <e32def.h>

// CLASS DECLARATIONS

/**
 * Phonebook 2 process decorator observer interface.
 */
class MPbk2ProcessDecoratorObserver
    {
    public: // Interface

        /**
         * This function is called if the process is dismissed from
         * the decorator.
         *
         * @param aCancelCode   The reason for cancelling.
         */
        virtual void ProcessDismissed(
                TInt aCancelCode ) = 0;

    protected: // Disabled functions
        ~MPbk2ProcessDecoratorObserver()
                {}
    };

/**
 * Phonebook 2 process decorator interface.
 */
class MPbk2ProcessDecorator
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ProcessDecorator()
                {}

        /**
         * This function is called when the decorated process is started.
         *
         * @param aSteps    Number of steps in the whole process.
         */
        virtual void ProcessStartedL(
                TInt aSteps ) = 0;

        /**
         * This function is called when the decorated process advances
         * one or more steps.
         *
         * @param aIncrement    Number of steps that has been passed since
         *                      startup or last ProcessAdvance call.
         */
        virtual void ProcessAdvance(
                TInt aIncrement ) = 0;

        /**
         * This function is called when the decorated process is stopped.
         */
        virtual void ProcessStopped() = 0;

        /**
         * Sets an observer for this decorator.
         *
         * @param aObserver     The observer to set.
         */
        virtual void SetObserver(
                MPbk2ProcessDecoratorObserver& aObserver ) = 0;
    };

#endif // MPBK2PROCESSDECORATOR_H

// End of File
