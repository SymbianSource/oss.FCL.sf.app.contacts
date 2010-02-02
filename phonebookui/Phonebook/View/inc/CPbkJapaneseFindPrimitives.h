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
*      MPbkFindPrimitives implementation for european Phonebook.
*
*/


#ifndef __CPbkJapaneseFindPrimitives_H__
#define __CPbkJapaneseFindPrimitives_H__

//  INCLUDES
#include <cntviewbase.h>
#include <MPbkFindPrimitives.h>

// FORWARD DECLARATIONS
class MPbkContactNameFormat;
class CViewContact;
class MPbkFieldDataArray;


// CLASS DECLARATION

/**
 * MPbkFindPrimitives implementation for european Phonebook.
 */
NONSHARABLE_CLASS(CPbkJapaneseFindPrimitives) : 
        public CBase, 
        public MPbkFindPrimitives
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aNameFormatter contact name formatter
         */
        static CPbkJapaneseFindPrimitives* NewL(MPbkContactNameFormat& aNameFormatter);

        /**
         * Destructor.
         */
        ~CPbkJapaneseFindPrimitives();

    private:  // from MPbkFindPrimitives
        void SetContactViewSortOrderL(const RContactViewSortOrder& aSortOrder);
        TBool IsFindMatchL
            (const CViewContact& aViewContact, const TDesC& aFindText);
        void GetInitialMatchesL
            (CContactViewBase& aView,
            const TDesC& aFindText,
            RPointerArray<CViewContact>& aMatchedContacts);
        TBool MatchesInitialFindTextL(const TDesC& aText);
        TBool MatchesInitialFindTextL(const CViewContact& aViewContact);

    private:  // Implementation
        CPbkJapaneseFindPrimitives(MPbkContactNameFormat& aNameFormatter);
        TBool IsMatchL(const MPbkFieldDataArray& aContactData, const TDesC& aFindText);
        TBool IsMatchL(const CViewContact& aViewContact, const TDesC& aFindText);
        static TBool IsWordSeparator(TChar aCh);
        void StoreInitialFindTextL(const TDesC& aInitialFindText);
        TPtrC InitialFindText() const;
        TBool DoMatchesInitialFindText(const TDesC& aText) const;

    private:  // Data
		/// Ref: contact name formatter
        MPbkContactNameFormat& iNameFormatter;
		/// Own: view sort order
        RContactViewSortOrder iViewSortOrder;
		/// Own: find text
        HBufC* iFindText;
		/// Own: field text
        HBufC* iFieldText;
		/// Own: initial find text
        HBufC* iInitialFindText;
    };

#endif // __CPbkJapaneseFindPrimitives_H__
            
// End of File
