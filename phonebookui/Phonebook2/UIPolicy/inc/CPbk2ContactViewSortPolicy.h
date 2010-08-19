/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact view sort policy.
*
*/


#ifndef CPBK2CONTACTVIEWSORTPOLICY_H
#define CPBK2CONTACTVIEWSORTPOLICY_H

//  INCLUDES
#include <CVPbkContactViewSortPolicy.h>
#include "TPbk2SortOrderFieldMapper.h"

// FORWARD DECLARATIONS
class CSortUtil;
class MVPbkFieldTypeList;
class CVPbkFieldTypeSelector;
class CPbk2SortKeyArray;

// CLASS DESCRIPTION

/**
 * Phonebook 2 contact view sort policy.
 * Responsible for implementing Virtual Phonebook defined contact
 * view sort policy by following the rules specied for contact
 * sorting in UI level specifications.
 */
class CPbk2ContactViewSortPolicy : public CVPbkContactViewSortPolicy
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         *
         * @param aParam    Virtual Phonebook sort policy parameter.
         * @return  A new instance of this class.
         */
        static CPbk2ContactViewSortPolicy* NewL(
                TParam* aParam );

        /**
         * Destructor.
         */
        ~CPbk2ContactViewSortPolicy();

    private: // From CVPbkContactViewSortPolicy
        void SetSortOrderL(
                const MVPbkFieldTypeList& aSortOrder );
        void SortStartL();
        void SortCompleted();
        TInt CompareContacts(
                const MVPbkViewContact& aLhs,
                const MVPbkViewContact& aRhs );
        TInt CompareItems(
                const MVPbkSortKeyArray& aLhs,
                const MVPbkSortKeyArray& aRhs ) const;

    private: // Implementation
        CPbk2ContactViewSortPolicy();
        void ConstructL(
                TParam* aParam );
        TInt PostProcessResult(
                TInt aSortUtilResult );
        /**
         * Get the index of top contact field in sort order list
         */
        TInt GetTopContactFieldIndex();
        /** 
         * Set sortkey array item from one field
         */
        void SetSortKey(
        		CPbk2SortKeyArray* aSortKeyArray,
        		const MVPbkBaseContactField* aField,
        		TInt aIndex );

    private: // Data
        /// Own: Sort Util to forward sorting requests
        CSortUtil* iSortUtil;
        /// Own: Field type selector for pronunciation fields
        CVPbkFieldTypeSelector* iPronunciationFieldSelector;
        /// Own: sort key array for left hand side
        CPbk2SortKeyArray* iLeftSortKeyArray;
        /// Own: sort key array for right hand side
        CPbk2SortKeyArray* iRightSortKeyArray;
        /// Own: Sort order of the view that this sort policy is related to
        const MVPbkFieldTypeList* iSortOrder;
        /// Own: Contact field mapper for mapping
        /// contact fields to sort order fields
        TPbk2SortOrderFieldMapper iFieldMapper;
        /// Own: Indicates succesfull initialization
        TBool iSortPolicyParameterReceived;
        /// Index of top contact field in iSortOrder
        TInt iTopContactId;
    };

#endif // CPBK2CONTACTVIEWSORTPOLICY_H

// End of File
