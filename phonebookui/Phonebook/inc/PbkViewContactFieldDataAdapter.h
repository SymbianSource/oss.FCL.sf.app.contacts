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
*     CViewContact <-> MPbkFieldDataArray adapters.
*
*/


#ifndef __PbkViewContactFieldDataAdapter_H__
#define __PbkViewContactFieldDataAdapter_H__

// INCLUDES
#include "MPbkFieldData.h"

// FORWARD DECLARATIONS
class CViewContact;
class RContactViewSortOrder;


// CLASS DECLARATIONS

/**
 * Adapter CViewContact -> MPbkFieldDataArray.
 *
 * @see TViewContactFieldData
 * @see CViewContact
 * @see MPbkFieldDataArray
 */
class TPbkViewContactFieldDataArray : public MPbkFieldDataArray
    {
    public:  // Interface
        /**
         * Adapts a view contact contents to MPbkFieldDataArray interface.
         * 
         * @param aViewContact  view contact to adapt
         * @param aSortOrder    sort order of the view where aViewContact
         *                      is from.
         * @precond aViewContact.FieldCount() == aSortOrder.Count()
         */
        IMPORT_C TPbkViewContactFieldDataArray
            (const CViewContact& aViewContact, 
            const RContactViewSortOrder& aSortOrder);

        /**
         * Destructor.
         */
        IMPORT_C ~TPbkViewContactFieldDataArray();

    public:  // from MPbkFieldDataArray
        TInt PbkFieldCount() const;
        MPbkFieldData& PbkFieldAt(TInt aIndex);
        const MPbkFieldData& PbkFieldAt(TInt aIndex) const;

    private:  // Implementation
        TPbkFieldId FieldId(TInt aIndex) const;

        /**
         * Adapter CViewContact field -> MPbkFieldData.
         *
         * @see TPbkViewContactFieldDataArray
         * @see MPbkFieldData
         */
        class TFieldData : public MPbkFieldData
            {
            public:  // Interface
			   /**
				* Constructor.
				*/
				TFieldData();
			   /**
				* Set.
				* @param aFieldId field id
				* @param aText text
				*/
                void Set(TPbkFieldId aFieldId, const TDesC& aText);

            public:  // from MPbkFieldData
                TStorageType PbkFieldType() const;
                TPbkFieldId PbkFieldId() const;
                TPtrC PbkFieldText() const;
                TTime PbkFieldTime() const;

            private:  // Data
				/// Own: field id
                TPbkFieldId iFieldId;
				/// Own: text
                TPtrC iText;
            };

    private:  // Data
        /// Ref: the adapted view contact
        const CViewContact& iViewContact;
        /// Ref: sort order of the contact view where iViewContact is from
        const RContactViewSortOrder& iSortOrder;
        /// Own: last requested field
        mutable TFieldData iField;
    };


#endif // __PbkViewContactFieldDataAdapter_H__

// End of File
