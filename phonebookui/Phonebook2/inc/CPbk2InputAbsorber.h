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
* Description:  Phonebook 2 input absorber.
*
*/


#ifndef CPBK2INPUTABSORBER_H
#define CPBK2INPUTABSORBER_H

//  INCLUDES
#include <coecntrl.h>   // CCoeControl
#include <eikcmobs.h>   // MEikCommandObserver

// FORWARD DECLARATIONS
class CEikAppUi;
class CEikButtonGroupContainer;

// CLASS DECLARATION

/**
 * Phonebook 2 input absorber.
 * A control which absorbs all the keyboard and CBA events.
 */
class CPbk2InputAbsorber : public CCoeControl,
                           private MEikCommandObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aCbaResource  Resource id of softkeys to use.
         *                      Defaults to empty softkeys.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2InputAbsorber* NewL(
                TInt aCbaResource = 0 );

        /**
         * Creates a new instance of this class.
         *
         * @param aCbaResource  Resource id of softkeys to use.
         *                      Defaults to empty softkeys.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2InputAbsorber* NewLC(
                TInt aCbaResource = 0 );

        /**
         * Destructor.
         */
        ~CPbk2InputAbsorber();

    public: // Interface

        /**
         * Sets an observer for the softkey commands.
         *
         * @param aCmdObserver  The observer to set.
         */
        IMPORT_C void SetCommandObserver(
                MEikCommandObserver* aCmdObserver );

        /**
         * Waits until StopWait() is called or this object is destroyed.
         */
        IMPORT_C void Wait();

        /**
         * Makes Wait() return.
         */
        IMPORT_C void StopWait();

        /**
         * Returns ETrue if Wait() is called.
         */
        IMPORT_C TBool IsWaiting() const;

    private: // From CCoeControl
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );

    private: // From MEikCommandObserver
        void ProcessCommandL(
                TInt aCommandId );

    private: // Implementation
        CPbk2InputAbsorber();
        void ConstructL(
                TInt aCbaResource );
        void InitControlL();
        void CaptureEventsL();
        void InitCbaL(
                TInt aCbaResource );

    private: // Data
        /// Ref: Application UI reference
        CEikAppUi* iAppUi;
        /// Own: Button group container
        CEikButtonGroupContainer* iCba;
        /// Ref: Softkey command observer
        MEikCommandObserver* iCmdObserver;
        /// Own: Active scheduler waiter
        CActiveSchedulerWait* iWait;
    };

#endif // CPBK2INPUTABSORBER_H

// End of File
