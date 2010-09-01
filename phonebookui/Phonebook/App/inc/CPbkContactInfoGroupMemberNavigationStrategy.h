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
*     Contact Info View group members navigation stategy.
*
*/


#ifndef __CPbkContactInfoGroupMemberNavigationStrategy_H__
#define __CPbkContactInfoGroupMemberNavigationStrategy_H__


//  INCLUDES
#include <e32base.h>
#include "CPbkContactInfoBaseNavigationStrategy.h"


//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CAknNavigationDecorator;
class CContactGroupView;
class MPbkContactInfoNavigationCallback;


//  CLASS DECLARATION

/**
 * Phonebook Contact Info group members navigation strategy class. 
 */
class CPbkContactInfoGroupMemberNavigationStrategy : 
        public CPbkContactInfoBaseNavigationStrategy
    {
    public:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         */
        static CPbkContactInfoGroupMemberNavigationStrategy* NewL(
                CPbkContactEngine& aEngine,
                MPbkContactInfoNavigationCallback& aCallback,
                TContactItemId aGroupId);

        /**
         * Destructor.
         */
        ~CPbkContactInfoGroupMemberNavigationStrategy();

    private: // Implementation
        CPbkContactInfoGroupMemberNavigationStrategy(
                CPbkContactEngine& aEngine,
                MPbkContactInfoNavigationCallback& aCallback);
        void ConstructL(TContactItemId aGroupId);

    };

#endif // __CPbkContactInfoGroupMemberNavigationStrategy_H__
      
// End of File
