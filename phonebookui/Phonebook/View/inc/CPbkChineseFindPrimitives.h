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
*    Chinese find primitives implementation for Phonebook.
*
*/


#ifndef __CPbkChineseFindPrimitives_H__
#define __CPbkChineseFindPrimitives_H__

//  INCLUDES
#include <cntviewbase.h>
#include "MPbkFindPrimitives.h"

// FORWARD DECLARATIONS
class CFindUtil;
class MPbkContactNameFormat;
class CPbkViewContactNameFormatter;


// CLASS DECLARATION

/**
 * Chinese find primitives implementation.
 */
NONSHARABLE_CLASS(CPbkChineseFindPrimitives) : 
        public CBase, public MPbkFindPrimitives
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aNameFormatter contact name formatter
         */
        static CPbkChineseFindPrimitives* NewL
            (MPbkContactNameFormat& aContactNameFormatter);
		
        /**
         * Destructor.
         */
        ~CPbkChineseFindPrimitives();
        
    public:  // from MPbkFindPrimitives
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
		CPbkChineseFindPrimitives();
        void ConstructL(MPbkContactNameFormat& aContactNameFormatter);
        const TDesC& ContactTitleL(const CViewContact& aViewContact);
        static TBool IsWordSeparator(TChar aCh);
        TBool IsMatchL(const TDesC& aTargetText, const TDesC& aFindText);

    private:  // Data
		/// Own: find util
		CFindUtil* iFindUtil;
		/// Ref: contact name formatter
        CPbkViewContactNameFormatter* iViewContactNameFormatter;
		/// Own: view sort order
        RContactViewSortOrder iContactViewSortOrder;
		/// Own: initial find text
        HBufC* iInitialFindText;
    };

#endif // __CPbkChineseFindPrimitives_H__

// End of File
