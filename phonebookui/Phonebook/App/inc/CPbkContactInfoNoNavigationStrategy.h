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
*     Contact Info View no navigation stategy.
*
*/


#ifndef __CPbkContactInfoNoNavigationStrategy_H__
#define __CPbkContactInfoNoNavigationStrategy_H__


//  INCLUDES
#include <e32base.h>
#include "MPbkContactInfoNavigationStrategy.h"


//  FORWARD DECLARATIONS


//  CLASS DECLARATION

/**
 * Phonebook Contact Info no navigation strategy class. 
 */
class CPbkContactInfoNoNavigationStrategy : 
        public CBase, 
        public MPbkContactInfoNavigationStrategy
    {
    public:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         */
        static CPbkContactInfoNoNavigationStrategy* NewL();

        /**
         * Destructor.
         */
        ~CPbkContactInfoNoNavigationStrategy();

    private: // from MPbkContactInfoNavigationStrategy
        TBool HandleCommandKeyL(const TKeyEvent& aKeyEvent, TEventCode aType);

    private: // Implementation
        CPbkContactInfoNoNavigationStrategy();

    };

#endif // __CPbkContactInfoNoNavigationStrategy_H__
      
// End of File
