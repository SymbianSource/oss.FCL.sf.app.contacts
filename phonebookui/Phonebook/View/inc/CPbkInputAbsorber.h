/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Control which absorbs all keyboard and CBA events.
*
*/


#ifndef __CPbkInputAbsorber_H__
#define __CPbkInputAbsorber_H__

//  INCLUDES
#include <coecntrl.h>   // CCoeControl
#include <eikcmobs.h>   // MEikCommandObserver

// FORWARD DECLARATIONS
class CEikAppUi;
class CEikButtonGroupContainer;

// CLASS DECLARATION

/**
 * Control which absorbs all keyboard and CBA events.
 */
NONSHARABLE_CLASS(CPbkInputAbsorber) : 
        public CCoeControl, 
        private MEikCommandObserver
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aCbaResource  softkeys to use. Default is empty softkeys.
         */
        static CPbkInputAbsorber* NewL(TInt aCbaResource=0);

        /**
         * Creates a new instance of this class.
         * @param aCbaResource  softkeys to use. Default is empty softkeys.
         */
        static CPbkInputAbsorber* NewLC(TInt aCbaResource=0);

        /**
         * Destructor.
         */
        ~CPbkInputAbsorber();

    public: // New functions
        /**
         * Sets observer aCmdObserver for softkey commands.
         */
        void SetCommandObserver(MEikCommandObserver* aCmdObserver);

        /**
         * Waits until StopWait() is called or this object is destroyed.
         */
        void Wait();

        /**
         * Makes Wait() return.
         */
        void StopWait();

        /**
         * Returns ETrue if Wait() is called.
         */
        TBool IsWaiting() const;

    private:  // from CCoeControl
	    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);

    private:  // from MEikCommandObserver
    	void ProcessCommandL(TInt aCommandId);

    private: // Implementation
        CPbkInputAbsorber();
        void InitControlL();
        void CaptureEventsL();
        void InitCbaL(TInt aCbaResource);	
        void ConstructL(TInt aCbaResource);

    private:    // Data
		/// Ref: Application UI reference
        CEikAppUi* iAppUi;
		/// Own: button group container
        CEikButtonGroupContainer* iCba;
		/// Ref: softkey command observer
        MEikCommandObserver* iCmdObserver;
		/// Own: active scheduler waiter
		CActiveSchedulerWait* iWait;
    };

#endif // __CPbkInputAbsorber_H__
            
// End of File
