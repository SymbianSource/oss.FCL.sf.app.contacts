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
*       Phonebook AIW interest item factory.
*
*/


#ifndef __MPbkAiwInterestItemFactory_H__
#define __MPbkAiwInterestItemFactory_H__

//  INCLUDES

//  FORWARD DECLARATIONS
class MPbkAiwInterestItem;
class CAiwServiceHandler;
class CPbkContactEngine;

//  CLASS DECLARATION 

/**
 * Phonebook command factory.
 */
class MPbkAiwInterestItemFactory
    {
    public:  // Interface
        /**
         * Factory function for creating an interest item.
         * @param aInterestId AIW interest id
         * @param aServiceHandler reference to AIW service handler
         * @param aEngine reference to Phonebook contact engine
         * @ret Phonebook AIW interest item
         * @see MPbkAiwInterestItem
         */
        virtual MPbkAiwInterestItem* CreateInterestItemL(
            TInt aInterestId,
            CAiwServiceHandler& aServiceHandler,
            CPbkContactEngine& aEngine) = 0;

	protected:
	    /**
		 * Protected virtual destructor.
		 */
		virtual ~MPbkAiwInterestItemFactory() { }
    };

#endif // __MPbkAiwInterestItemFactory_H__

// End of File
