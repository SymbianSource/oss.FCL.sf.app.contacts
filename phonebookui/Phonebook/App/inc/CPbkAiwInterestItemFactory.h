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
*       Creates Phonebook's AIW interest items.
*
*/


#ifndef __CPbkAiwInterestItemFactory_H__
#define __CPbkAiwInterestItemFactory_H__

//  INCLUDES
#include <e32base.h>
#include <MPbkAiwInterestItemFactory.h>


//  CLASS DECLARATION 

/**
 * Creates phonebook commands.
 */
class CPbkAiwInterestItemFactory : public CBase, 
                           public MPbkAiwInterestItemFactory
    {
    public: // Constructor and destructor
        /**
         * Static constructor.
         * @return a new CPbkAiwInterestItemFactory object
         */
        static CPbkAiwInterestItemFactory* NewL();

        /**
         * Destructor.
         */
        ~CPbkAiwInterestItemFactory();

    public: // From MPbkAiwInterestItemFactory
        MPbkAiwInterestItem* CreateInterestItemL(
            TInt aInterestId,
            CAiwServiceHandler& aServiceHandler,
            CPbkContactEngine& aEngine);

    private: // Implementation
        CPbkAiwInterestItemFactory();

    private: // Data
        ///Own:
        MPbkAiwInterestItem* iCallInterest;
        ///Own:
        MPbkAiwInterestItem* iPocInterest;
        
        MPbkAiwInterestItem* iSyncMlInterest;
    };

#endif // __CPbkAiwInterestItemFactory_H__

// End of File
