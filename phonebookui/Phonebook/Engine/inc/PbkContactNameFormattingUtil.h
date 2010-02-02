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
*      Phonebook contact name formatting utility class
*
*/


#ifndef __PbkContactNameFormattingUtil_H__
#define __PbkContactNameFormattingUtil_H__

// INCLUDES
#include <e32std.h>
#include "PbkFields.hrh"


// FORWARD DECLARATIONS
class MPbkFieldDataArray;
class RContactViewSortOrder;


// CLASS DECLARATION

/**
 * Phonebook contact name formatting utility class.
 */
NONSHARABLE_CLASS(PbkContactNameFormattingUtil)
    {
    public: // static helper functions
        /**
         * Returns text of first field matching a field id.
         *
         * @param aContactData  field data array to search from.
         * @param aFieldId      field id to search for.
         * @return  matching field's text or empty string if
         *          field is not found or it does not contain any text.
         */     
        static TPtrC FieldText
            (const MPbkFieldDataArray& aContactData, TPbkFieldId aFieldId);

        /**
         * Returns true if first character of aText is space.
         * @precond aText.Length() > 0
         */
        static TBool FirstCharIsSpace(const TDesC& aText);

        /**
         * Returns true if last character of aText is space.
         * @precond aText.Length() > 0
         */
        static TBool LastCharIsSpace(const TDesC& aText);

        /**
         * Gets the phonebook field id corresponding to aIndex
         * in the sort order array.
         */
        static TPbkFieldId GetPbkFieldIdAt
            (const RContactViewSortOrder& aSortOrder, TInt aIndex);

    };

#endif // __PbkContactNameFormattingUtil_H__

// End of File
