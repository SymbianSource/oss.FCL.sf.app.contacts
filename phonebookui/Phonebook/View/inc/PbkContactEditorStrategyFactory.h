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
*     Contact editor strategy factory.
*
*/


#ifndef __PbkContactEditorStrategyFactory_H__
#define __PbkContactEditorStrategyFactory_H__

//  INCLUDES
#include <e32def.h>

//  FORWARD DECLARATIONS
class MPbkContactEditorStrategy;
class CPbkContactEngine;
class CPbkContactItem;

//  CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook Contact editor strategy factory.
 */
NONSHARABLE_CLASS(PbkContactEditorStrategyFactory)
    {
    public:  // Constructors
		/**
		 * Creates a strategy object for a contact editor.
		 * @param aEngine Phonebook engine
		 * @param aContactItem contact item
		 * @param aIsNewContact ETrue if the contact is new
		 */
		static MPbkContactEditorStrategy* CreateL(
                CPbkContactEngine& aEngine,
		        CPbkContactItem& aContactItem,
                TBool aIsNewContact);
    };

#endif // __PbkContactEditorStrategyFactory_H__

// End of File
