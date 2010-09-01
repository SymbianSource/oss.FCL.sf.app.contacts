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
*     Phonebook Contact editor add field abstract class.
*
*/


#ifndef __MPbkContactEditorCreateField_H__
#define __MPbkContactEditorCreateField_H__

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbkContactEditorField;
class CPbkFieldInfo;

// CLASS DECLARATION

/**
 * Phonebook Contact editor add field abstract class. 
 */
class MPbkContactEditorCreateField
    {
    public:  // Interface
        /**
         * Creates a new field from the aFieldInfo type.
         */
        virtual MPbkContactEditorField& CreateFieldL
			(CPbkFieldInfo& aFieldInfo) = 0;
    };

#endif // __MPbkContactEditorCreateField_H__
            
// End of File
