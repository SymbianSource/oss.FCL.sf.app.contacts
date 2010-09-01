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
*     Contact Info View group members navigation stategy factory.
*
*/


#ifndef __PbkContactInfoNavigationStrategyFactory_H__
#define __PbkContactInfoNavigationStrategyFactory_H__


//  INCLUDES
#include <e32base.h>  // Memory entry views super class
#include <cntdef.h>

//  FORWARD DECLARATIONS
class MPbkContactInfoNavigationStrategy;
class MPbkContactInfoNavigationCallback;
class CPbkContactEngine;

//  CLASS DECLARATION

/**
 * Phonebook Contact Info navigation strategy class factory. 
 */
class PbkContactInfoNavigationStrategyFactory
    {
    public:  // Constructors and destructor
        /**
         * Creates and returns a new object of this class.
         * @param aEngine Phonebook contact engine
         * @param aCallback callback interface for changing viewed contact
         * @param aParentContactId contact id of the parent of this contact. 
         *              KNullContactId if no parent.
         */
        static MPbkContactInfoNavigationStrategy* CreateL
                (CPbkContactEngine& aEngine,
                MPbkContactInfoNavigationCallback& aCallback,
                TContactItemId aParentContactId);

    };

#endif // __PbkContactInfoNavigationStrategyFactory_H__
      
// End of File
