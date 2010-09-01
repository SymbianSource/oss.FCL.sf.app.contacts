/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Field type list for mapping contact model view sort order 
                 to generic field type list.
*
*/


#ifndef CFIELDTYPELIST_H
#define CFIELDTYPELIST_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkFieldType.h>
#include <VPbkFieldType.hrh>

// FORWARD DECLARATIONS
class RContactViewSortOrder;
class MVPbkFieldType;
class MVPbkFieldTypeList;

namespace VPbkCntModel {

// FORWARD DECLARATIONS
class CFieldTypeMap;

/**
 * Field type list for mapping contact model view sort order to generic field type list.
 */
NONSHARABLE_CLASS( CFieldTypeList ): 
        public CBase,
        public MVPbkFieldTypeList
    {
    public:
        static CFieldTypeList* NewLC(const RContactViewSortOrder& aSortOrder,
                                     const CFieldTypeMap& aFieldTypeMap);
        ~CFieldTypeList();

    public: // From MVPbkFieldTypeList
        TInt FieldTypeCount() const;
        const MVPbkFieldType& FieldTypeAt(TInt aIndex) const;
        TBool ContainsSame(const MVPbkFieldType& aFieldType) const;
        TInt MaxMatchPriority() const;
        const MVPbkFieldType* FindMatch
            (const TVPbkFieldVersitProperty& aMatchProperty,
            TInt aMatchPriority) const;
        const MVPbkFieldType* FindMatch
            (TVPbkNonVersitFieldType aNonVersitType) const;
        const MVPbkFieldType* Find(TInt aFieldTypeResId) const;
        
    private: // Implementation
        CFieldTypeList();
        void ConstructL(const RContactViewSortOrder& aSortOrder, 
                        const CFieldTypeMap& aFieldTypeMap);

    private: // Data
        ///Own: Field types array
        RPointerArray<MVPbkFieldType> iFieldTypes;
        ///Own: max match priority
        TInt iMaxMatchPriority;
    };

} // namespace VPbkCntModel

#endif // CFIELDTYPELIST_H

//End of file
