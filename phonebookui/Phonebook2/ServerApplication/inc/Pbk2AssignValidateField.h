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
* Description:  Phonebook 2 assign service field validator.
*
*/


#ifndef PBK2ASSIGNVALIDATEFIELD_H
#define PBK2ASSIGNVALIDATEFIELD_H

// INCLUDES
#include <e32base.h>
#include <f32file.h>

// FORWARD DECLARATIONS
class CPbk2PresentationContact;
class MVPbkFieldType;

// CLASS DECLARATION

/**
 * Phonebook 2 assign service field validator.
 * Responsible for:
 * - inspecting the given contact for its support capabilities
 *   for the given field type
 */
class Pbk2AssignValidateField
    {
    public: // Interface

        /**
         * Checks how the given field is used in the given contact.
         *
         * @param aContact      The contact to inspect.
         * @param aFieldType    The field type to inspect.
         * @param aFsSession    File server session.
         * @param aFieldIndex   Store index of the matching field if the
         *                      contact contains field(s) of the given type.
         * @return  - KErrNone if contact contains field of given type but
         *            there are no restrictions to usage of that field type.
         *          - KErrNotFound if the contact does not contain
         *            any fields of the given type.
         *          - KErrAlreadyExists if the contact contains one
         *            field of the given field type and the maximum
         *            number of fields of that type is one.
         *          - KErrNotSupported if the contact contains maximum
         *            amount of fields of the given field type and the
         *            maximum number of fields of that type is greater
         *            than one.
         */
        static TInt ValidateFieldTypeUsageInContactL(
                    CPbk2PresentationContact& aContact,
                    const MVPbkFieldType& aFieldType,
                    RFs& aFsSession,
                    TInt& aFieldIndex );
    };

#endif // PBK2ASSIGNVALIDATEFIELD_H

// End of File
