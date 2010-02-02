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
*      Concrete Phonebook contact name formatting interface
*
*/


#ifndef     __CPBKCONTACTNAMEFORMATTER_H__
#define     __CPBKCONTACTNAMEFORMATTER_H__

// INCLUDES
#include "CPbkContactNameFormatterBase.h"
#include <cntviewbase.h>
#include "TPbkTitleFieldFilter.h"


// FORWARD DECLARATIONS
class CPbkSortOrderManager;

// CLASS DECLARATION

/// Maximum of 2 fields (last name + first name) are used to format a title
const TInt KMaxTitleFields = 2;

/**
 * Phonebook contact name formatting implementation class. This class 
 * encapsulates contact name formatting rules in Series60 Phonebook 
 * application.
 */
NONSHARABLE_CLASS(CPbkContactNameFormatter) :
        public CPbkContactNameFormatterBase
    {
    public:  // constructor and destructor
        /**
         * Constructor.
         *
         * @param aUnnamedText  text to use for unnamed contacts.
         * @param aEngine engine for formatting contacts
         */
        static CPbkContactNameFormatter* NewL(
				const TDesC& aUnnamedText,
				const CPbkSortOrderManager& aSortOrderManager);

        /**
         * Destructor.
         */
        ~CPbkContactNameFormatter(); 

    public: // from MPbkContactNameFormat
        void GetContactTitle 
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const;
        const MPbkFieldDataArray& FilterContactTitleFields
            (const MPbkFieldDataArray& aContactData) const;
        void GetContactListTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const;
        const MPbkFieldDataArray& FilterContactFieldsForFind
            (const MPbkFieldDataArray& aContactData) const;
        TInt ContactTitleLength
            (const MPbkFieldDataArray& aContactData) const;
        TInt ContactListTitleLength
            (const MPbkFieldDataArray& aContactData) const;   

    private: // implementation
        CPbkContactNameFormatter(const CPbkSortOrderManager& aSortOrderManager);
        void ConstructL(const TDesC& aUnnamedText);
        void TrimName(TInt aTitleType, const TDesC& aName, TDes& aTitle) const;
        void DoGetContactTitle 
            (const MPbkFieldDataArray& aContactData, TDes& aTitle,
            TInt aTitleType) const;

    private: // data members
        typedef TPbkTitleFieldFilter<KMaxTitleFields> TTitleFieldFilter;
        /// Own: title field filter
        TTitleFieldFilter* iTitleFieldFilter;
		/// Ref: engine used when formatting contacts
		const CPbkSortOrderManager& iSortOrderManager;
    };


#endif // __CPBKCONTACTNAMEFORMATTER_H__

// End of File
