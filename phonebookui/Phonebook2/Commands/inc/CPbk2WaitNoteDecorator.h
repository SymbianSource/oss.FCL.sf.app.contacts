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
* Description:  Wait note process decorator.
*
*/


#ifndef CPBK2WAITNOTEDECORATOR_H
#define CPBK2WAITNOTEDECORATOR_H

//  INCLUDES
#include <e32base.h>
#include "MPbk2ProcessDecorator.h"
#include <AknProgressDialog.h>

//  CLASS DECLARATION

/**
 * Phonebook 2 progress dialog process decorator.
 */
NONSHARABLE_CLASS(CPbk2WaitNoteDecorator) : 
        public CBase,
        public MPbk2ProcessDecorator,
        public MProgressDialogCallback
    {
    public: // Construction
        /**
         * Creates a new instance of this class.
         * @param aResourceId the resource id for the dialog used
         * @param aVisibilityDelay ETrue if the delay is used, EFalse otherwise
         * @return a new instance of this class
         */
        static CPbk2WaitNoteDecorator* NewL(
                TInt aResourceId,
                TBool aVisibilityDelay);

        /**
         * Destructor.
         */
        ~CPbk2WaitNoteDecorator();

    private: // From MPbk2ProcessDecorator
        void ProcessStartedL(TInt aSteps);
        void ProcessAdvance(TInt aIncrement);
        void ProcessStopped();
        void SetObserver(MPbk2ProcessDecoratorObserver& aObserver);

    private: // From MProgressDialogCallback
        void DialogDismissedL(TInt aButtonId);

    private: // Implementation
        CPbk2WaitNoteDecorator(
            TInt aResourceId,
            TBool aVisibilityDelay);
        void CreateWaitNoteL();

    private: // Data
        /// Own: progress note dialog
        CAknProgressDialog* iWaitDialog;
        /// Own: resource id of the dialog to use
        TInt iDialogResource;
        /// Ref: observer
        MPbk2ProcessDecoratorObserver* iObserver;
        /// Own: indicates whether the visibility delay is used
        TBool iVisibilityDelayOff;
    };

#endif // CPBK2WAITNOTEDECORATOR_H

// End of File
