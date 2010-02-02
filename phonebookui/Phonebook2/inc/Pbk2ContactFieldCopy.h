/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 utility for copying fields between contacts.
*
*/


#ifndef PBK2CONTACTFIELDCOPY_H
#define PBK2CONTACTFIELDCOPY_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MPbk2ContactNameFormatter;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class MVPbkFieldType;

// CLASS DECLARATION

/**
 * Phonebook 2 utility for copying fields between contacts.
 */
class Pbk2ContactFieldCopy
    {
    public: // Interface

        /**
         * Copies title fields from the source contact to the target
         * contact. The field is not copied if the contact already contains
         * a field of the same type.
         *
         * @param aSourceContact    Contact whose title is copied.
         * @param aTargetContact    Target contact.
         * @param aNameFormatter    Name formatter for title field
         *                          selection.
         */
        IMPORT_C static void CopyTitleFieldsL(
                const MVPbkStoreContact& aSourceContact,
                MVPbkStoreContact& aTargetContact,
                MPbk2ContactNameFormatter& aNameFormatter );

        /**
         * Creates a new field to the target contact. Copies the data
         * and the label from the source contact.
         *
         * @param aSourceField      The field to copy
         * @param aType             The type of the field to create.
         * @param aTargetContact    The contact into which the new field
         *                          is added.
         */
        IMPORT_C static void CopyFieldL(
                const MVPbkStoreContactField& aSourceField,
                const MVPbkFieldType& aType,
                MVPbkStoreContact& aTargetContact );

    private: // Disabled functions
        Pbk2ContactFieldCopy()
                {}
    };

#endif // PBK2CONTACTFIELDCOPY_H

// End of File
