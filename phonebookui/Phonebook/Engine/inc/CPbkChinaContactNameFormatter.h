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
*      Concrete Phonebook contact name formatting interface for
*       APAC region variant.
*
*/


#ifndef __CPBKCHINACONTACTNAMEFORMATTER_H__
#define __CPBKCHINACONTACTNAMEFORMATTER_H__

// INCLUDES
#include "CPbkContactNameFormatterBase.h"
#include "TPbkTitleFieldFilter.h"
#include <cntviewbase.h>

// FORWARD DECLARATIONS


// CLASS DECLARATION

/// Maximum of 2 fields (last name + first name) are used to format a title
const TInt KApacMaxTitleFields = 2;

/**
 * Phonebook contact name formatting implementation class. This class 
 * encapsulates contact name formatting rules in Series60 Phonebook 
 * application.
 */
NONSHARABLE_CLASS(CPbkChinaContactNameFormatter) :
        public CPbkContactNameFormatterBase
    {
    public:  // constructor and destructor
        /**
         * Constructor.
         *
         * @param aUnnamedText  text to use for unnamed contacts.
         */
        static CPbkChinaContactNameFormatter* NewL
            (const TDesC& aUnnamedText, const RContactViewSortOrder& aSortOrder);

        /**
         * Destructor.
         */
        ~CPbkChinaContactNameFormatter(); 

    public: // from MPbkContactNameFormat
        void GetContactTitle 
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const;
        const MPbkFieldDataArray& FilterContactTitleFields
            (const MPbkFieldDataArray& aContactData) const;
        void GetContactListTitle
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const;
        const MPbkFieldDataArray& FilterContactFieldsForFind
            (const MPbkFieldDataArray& aContactData) const;

    private: // from CPbkContactNameFormatterBase
        TBool RequiresSpaceBetweenNames(const TDesC& aLastName, const TDesC& aFirstName) const;

    private: // implementation
        CPbkChinaContactNameFormatter();
        void ConstructL(const TDesC& aUnnamedText, const RContactViewSortOrder& aSortOrder);
        void DoGetContactTitle 
            (const MPbkFieldDataArray& aContactData, TDes& aTitle) const;


    private: // data members
        typedef TPbkTitleFieldFilter<KApacMaxTitleFields> TTitleFieldFilter;
        /// Own: title field filter
        TTitleFieldFilter* iTitleFieldFilter;
        /// Ref: views contact sort order
        RContactViewSortOrder iSortOrder;

    };


#endif // __CPBKCHINACONTACTNAMEFORMATTER_H__

// End of File
