/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook2 find delay
*
*/


#ifndef CPBK2FINDDELAY_H
#define CPBK2FINDDELAY_H

//  INCLUDES
#include <e32base.h>


// FORWARD DECLARATIONS
class MPbk2FindDelayObserver;

/**
 * Phonebook 2 find delay
 */
NONSHARABLE_CLASS(CPbk2FindDelay) : public CTimer
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer for the delay
         * @return A new instance of this class.
         */
        static CPbk2FindDelay* NewL( 
            MPbk2FindDelayObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2FindDelay();

    private: // From CTimer       
        void RunL();

    private:  // implementation
        CPbk2FindDelay( 
            MPbk2FindDelayObserver& aObserver );
        void ConstructL();

    private: // data
        // Ref:
        MPbk2FindDelayObserver& iObserver;
    };

#endif // CPBK2FINDDELAY_H
            
// End of File
