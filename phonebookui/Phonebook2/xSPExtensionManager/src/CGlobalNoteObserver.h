/*
* Copyright (c) 2002 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Generic global note events observer.
*
*/


#ifndef C_GLOBAL_NOTE_OBSERVER_H
#define C_GLOBAL_NOTE_OBSERVER_H

//  INCLUDES
#include <e32base.h>

// CONSTANTS
// None

// MACROS
// None

// DATA TYPES
// None

// FUNCTION PROTOTYPES
// None

// FORWARD DECLARATIONS
class MGlobalNoteObserver;

// CLASS DECLARATION

/**
* Active object that tracks global note events.
*/
NONSHARABLE_CLASS(CGlobalNoteObserver) : public CActive
    {
    public:
        /**
         * Standard Symbian two phased constructor.
         * @param aObserver The observer implementation that is interested of
         *          the global note events.
         * @leave Leaves with KErrArgument if aObserver is null.
         * @return Newly constructed object.
         */
        static CGlobalNoteObserver* NewL( MGlobalNoteObserver* aObserver );

        /**
         * Destructor
         */
        virtual ~CGlobalNoteObserver();

    private:
        /**
         * Standard C++ constructor
         * @param aObserver The observer implementation that is interested of
         *          the global note events. Leaves with KErrArgument if
         *          aObserver is null.
         */
        CGlobalNoteObserver( MGlobalNoteObserver* aObserver );

        /**
         * Standard Symbian 2nd phase constructor.
         */
        void ConstructL();
 
    private: // From CActive
        void RunL();
        TInt RunError( TInt aError );
        void DoCancel();

    private:
        // Reference
        MGlobalNoteObserver* iObserver;
    };

#endif  // C_GLOBAL_NOTE_OBSERVER_H
            
// End of File

