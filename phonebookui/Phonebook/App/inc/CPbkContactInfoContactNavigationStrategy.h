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
*     Contact Info View all contacts navigation stategy.
*
*/


#ifndef __CPbkContactInfoContactNavigationStrategy_H__
#define __CPbkContactInfoContactNavigationStrategy_H__


//  INCLUDES
#include <e32base.h>
#include "CPbkContactInfoBaseNavigationStrategy.h"


//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CAknNavigationDecorator;
class CContactViewBase;
class CPbkContactItem;
class MPbkContactInfoNavigationCallback;


//  CLASS DECLARATION

/**
 * Phonebook Contact Info all contacts navigation strategy class. 
 */
class CPbkContactInfoContactNavigationStrategy : 
        public CPbkContactInfoBaseNavigationStrategy 
    {
    public:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         */
        static CPbkContactInfoContactNavigationStrategy* NewL(
                CPbkContactEngine& aEngine,
                MPbkContactInfoNavigationCallback& aCallback);

        /**
         * Destructor.
         */
        ~CPbkContactInfoContactNavigationStrategy();

    private: // Implementation
        CPbkContactInfoContactNavigationStrategy(
                CPbkContactEngine& aEngine,
                MPbkContactInfoNavigationCallback& aCallback);
        void ConstructL();

    };

#endif // __CPbkContactInfoContactNavigationStrategy_H__
      
// End of File
