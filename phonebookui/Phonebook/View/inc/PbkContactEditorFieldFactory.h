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
*     Phonebook Contact editor field factory class.
*
*/


#ifndef __PbkContactEditorFieldFactory_H__
#define __PbkContactEditorFieldFactory_H__

// INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class MPbkContactEditorField;
class TPbkContactItemField;
class MPbkContactEditorUiBuilder;
class CPbkIconInfoContainer;
class CPbkContactItem;

// CLASS DECLARATION

/**
 * Phonebook Contact editor field factory class. 
 */
NONSHARABLE_CLASS(PbkContactEditorFieldFactory)
    {
    public:  // destructor
        /**
         * Factory for creating contac editor fields.
         * @param aField field which is created
         * @param aUiBuilder UI builder
		 * @param aIconInfoContainer field icon container
		 * @return a contact editor field.
         */
        static MPbkContactEditorField* CreateFieldL(
                TPbkContactItemField& aField,
                MPbkContactEditorUiBuilder& aUiBuilder,
				CPbkIconInfoContainer& aIconInfoContainer);

    };

#endif // __PbkContactEditorFieldFactory_H__

// End of File
