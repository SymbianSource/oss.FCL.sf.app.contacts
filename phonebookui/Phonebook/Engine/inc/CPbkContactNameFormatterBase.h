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
*      Phonebook contact name formatting base class
*
*/


#ifndef __CPBKCONTACTNAMEFORMATTERBASE_H__
#define __CPBKCONTACTNAMEFORMATTERBASE_H__

// INCLUDES
#include <MPbkContactNameFormat.h>


// FORWARD DECLARATIONS
class CContactViewBase;
class RContactViewSortOrder;
class MPbkFieldDataArray;


// CLASS DECLARATION

/**
 * Phonebook contact name formatting base class.
 */
NONSHARABLE_CLASS(CPbkContactNameFormatterBase) :
        public CBase,
        public MPbkContactNameFormat
    {
    protected:  // destructor
        /**
         * Constructor.
         *
         * @param aUnnamedText  text to use for unnamed contacts.
         */
        CPbkContactNameFormatterBase();

        /**
         * Destructor.
         */
        ~CPbkContactNameFormatterBase(); 

    public: // from MPbkContactNameFormat
        HBufC* GetContactTitleOrNullL
            (const MPbkFieldDataArray& aContactData) const;
        HBufC* GetContactTitleL
            (const MPbkFieldDataArray& aContactData) const;
        const TDesC& UnnamedText() const;
        void GetContactTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const = 0;
        TInt ContactTitleLength
            (const MPbkFieldDataArray& aContactData) const;
        TBool IsTitleField
            (TPbkFieldId aFieldId) const;
        HBufC* FormatNameDescriptorL
            (const TDesC& aTitle) const;
        const MPbkFieldDataArray& FilterContactTitleFields
            (const MPbkFieldDataArray& aContactData) const = 0;
        void GetContactListTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const = 0;
        TInt ContactListTitleLength
            (const MPbkFieldDataArray& aContactData) const;
        const MPbkFieldDataArray& FilterContactFieldsForFind
            (const MPbkFieldDataArray& aContactData) const = 0;


    protected: // interface        
        /**
         * Returns true if a space is needed between aLastName
         * and aFirstName in the formatted name.
         */
        virtual TBool RequiresSpaceBetweenNames
            (const TDesC& aLastName, const TDesC& aFirstName) const;

    protected: // implementation
        void BaseConstructL(const TDesC& aUnnamedText);
        TInt TrimNameLength(TInt aTitleType, const TDesC& aName) const;
        TInt CalculateContactTitleLength
            (const MPbkFieldDataArray& aContactData,
            TInt aTitleType) const;

    protected: // data members
        /// Ref: unnamed text
        HBufC* iUnnamedText;

    };

#endif // __CPBKCONTACTNAMEFORMATTERBASE_H__

// End of File
