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
* Description:  Phonebook 2 group view sort order.
*
*/


#ifndef CPGUSORTORDER_H
#define CPGUSORTORDER_H

#include <e32base.h>
#include <MVPbkFieldType.h>

//  FORWARD DECLARATIONS
class CVPbkSortOrder;

// CLASS DECLARATION

/**
 * Phonebook 2 group view sort order.
 * Responsible for creating and owning group view's sort order.
 */
NONSHARABLE_CLASS(CPguSortOrder) : public CBase,
                                   public MVPbkFieldTypeList
    {
    public: // Construction and destruction

        /**
         * Creates a new intance of this class.
         *
         * @param aMasterFieldTypeList  Master field type list.
         * @return  A new instance of this class.
         */
        static CPguSortOrder* NewL(
                const MVPbkFieldTypeList& aMasterFieldTypeList );

        /**
         * Destructor.
         */
        ~CPguSortOrder();

    public: // From MVPbkFieldTypeList
        TInt FieldTypeCount() const;
        const MVPbkFieldType& FieldTypeAt(
                TInt aIndex ) const;
        TBool ContainsSame(
                const MVPbkFieldType& aFieldType ) const;
        TInt MaxMatchPriority() const;
        const MVPbkFieldType* FindMatch(
                const TVPbkFieldVersitProperty& aMatchProperty,
                TInt aMatchPriority ) const;
        const MVPbkFieldType* FindMatch(
                TVPbkNonVersitFieldType aNonVersitType ) const;
        const MVPbkFieldType* Find(
                TInt aFieldTypeResId ) const;

    private: // Implementation
        CPguSortOrder(
                const MVPbkFieldTypeList& aMasterFieldTypeList );
        void ConstructL();
        CVPbkSortOrder* CreateSortOrderL();

    private: // Data
        /// Ref: Master field type list
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        /// Own: Group view's sort order
        CVPbkSortOrder* iSortOrder;
    };

#endif // CPGUSORTORDER_H

// End of File
