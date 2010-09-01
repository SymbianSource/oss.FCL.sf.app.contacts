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
*      Concrete Phonebook contact name formatting interface for japanese region.
*
*/


#ifndef     __CPBKJAPANCONTACTNAMEFORMATTER_H__
#define     __CPBKJAPANCONTACTNAMEFORMATTER_H__

// INCLUDES
#include "CPbkContactNameFormatterBase.h"
#include <cntviewbase.h>
#include "TPbkJapanTitleFieldFilter.h"
#include "TPbkJapanFindFieldFilter.h"

// FORWARD DECLARATIONS
class CPbkSortOrderManager;

// CLASS DECLARATION

/// Maximum of 2 fields (last name + first name) are used to format a title
const TInt KJapanMaxTitleFields = 2;
/// Maximum of 4 fields (last name + first name + their reading
/// counterparts) are used in Japanese find
const TInt KJapanMaxFindFields = 4;


/**
 * Phonebook contact name formatting implementation class. This class 
 * encapsulates contact name formatting rules in Series60 Phonebook 
 * application.
 */
NONSHARABLE_CLASS(CPbkJapanContactNameFormatter) :
        public CPbkContactNameFormatterBase
    {
    public:  // constructor and destructor
        /**
         * Constructor.
         *
         * @param aUnnamedText  text to use for unnamed contacts.
         * @param aEngine engine for formatting contacts
         */
        static CPbkJapanContactNameFormatter* NewL
            (const TDesC& aUnnamedText, const CPbkSortOrderManager& aSortOrderManager);

        /**
         * Destructor.
         */
        ~CPbkJapanContactNameFormatter(); 

    public: // from MPbkContactNameFormat
        void GetContactTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const;
        TInt ContactTitleLength
            (const MPbkFieldDataArray& aContactData) const;
        TBool IsTitleField
            (TPbkFieldId aFieldId) const;
        const MPbkFieldDataArray& FilterContactTitleFields
            (const MPbkFieldDataArray& aContactData) const;
        void GetContactListTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const;
        TInt ContactListTitleLength
            (const MPbkFieldDataArray& aContactData) const;

    private: // implementation
        CPbkJapanContactNameFormatter(const CPbkSortOrderManager& aSortOrderManager);
        void ConstructL(const TDesC& aUnnamedText);
        void DoGetContactTitle 
            (const MPbkFieldDataArray& aContactData, TDes& aTitle,
            TInt aTitleType) const;
        void TrimName(TInt aTitleType, const TDesC& aName, TDes& aTitle) const;
        TInt TrimNameLength(TInt aTitleType, const TDesC& aName) const;
        TInt CalculateContactTitleLength
            (const MPbkFieldDataArray& aContactData, TInt aTitleType) const;
        const MPbkFieldDataArray& FilterContactFieldsForFind
            (const MPbkFieldDataArray& aContactData) const;

    private: // data members
        typedef TPbkJapanTitleFieldFilter<KJapanMaxTitleFields> TTitleFieldFilter;
        typedef TPbkJapanFindFieldFilter<KJapanMaxFindFields> TFindFieldFilter;
        /// Own: title field filter
        TTitleFieldFilter* iTitleFieldFilter;
        /// Own: find field filter
        TFindFieldFilter* iFindFieldFilter;
		/// Ref: engine used when formatting contacts
		const CPbkSortOrderManager& iSortOrderManager;
    };


#endif // __CPBKJAPANCONTACTNAMEFORMATTER_H__

// End of File
