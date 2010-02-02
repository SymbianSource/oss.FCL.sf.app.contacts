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
* Description:   Progress dialog process decorator.
*
*/


#ifndef CPBK2PROGRESSDIALOGDECORATOR_H
#define CPBK2PROGRESSDIALOGDECORATOR_H

//  INCLUDES
#include <e32base.h>
#include "MPbk2ProcessDecorator.h"
#include <AknProgressDialog.h>

//  FORWARD DECLARATIONS
class CEikProgressInfo;

//  CLASS DECLARATION

/**
 * Phonebook 2 progress dialog process decorator.
 */

NONSHARABLE_CLASS(CPbk2ProgressDialogDecorator) : 
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
        static CPbk2ProgressDialogDecorator*
            NewL(TInt aResourceId, TBool aVisibilityDelay);

        /**
         * Destructor.
         */
        ~CPbk2ProgressDialogDecorator();

    private: // From MPbk2ProcessDecorator
        void ProcessStartedL(TInt aSteps);
        void ProcessAdvance(TInt aIncrement);
        void ProcessStopped();
        void SetObserver(MPbk2ProcessDecoratorObserver& aObserver);

    private: // From MProgressDialogCallback
        void DialogDismissedL(TInt aButtonId);

    private: // Implementation
        CPbk2ProgressDialogDecorator(TInt aResourceId, TBool aVisibilityDelay);
        void CreateProgressNoteL(TInt aFinalValue);

    private: // Data
        /// Own: progress note dialog
        CAknProgressDialog* iProgressDialog;
        /// Own: progress dialog info
        CEikProgressInfo* iProgressInfo;
        /// Own: resource id of the dialog to use
        TInt iDialogResource;
        /// Ref: observer
        MPbk2ProcessDecoratorObserver* iObserver;
        /// Own: indicates whether the visibility delay is used
        TBool iVisibilityDelayOff;
    };

#endif // CPBK2PROGRESSDIALOGDECORATOR_H

// End of File
