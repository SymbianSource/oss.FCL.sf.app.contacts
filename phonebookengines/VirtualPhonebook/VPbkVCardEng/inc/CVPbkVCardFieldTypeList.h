/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Field type list for mapping vCard field types to 
                 generic field type list.
*
*/


#ifndef CVPBKVCARDFIELDTYPELIST_H
#define CVPBKVCARDFIELDTYPELIST_H

// INCLUDE FILES
#include <e32base.h>
#include <MVPbkFieldType.h>
#include <VPbkFieldType.hrh>

// FORWARD DECLARATIONS
class RContactViewSortOrder;
class MVPbkFieldType;
class MVPbkFieldTypeList;
class CVPbkVCardFieldTypeProperty;

// FORWARD DECLARATIONS
class CFieldTypeMap;

/**
 * Field type list for mapping vCard field types to generic field type list.
 */
NONSHARABLE_CLASS(CVPbkVCardFieldTypeList) : 
        public CBase,
        public MVPbkFieldTypeList
    {
    public:
        static CVPbkVCardFieldTypeList* NewLC( 
            const TArray<CVPbkVCardFieldTypeProperty*> aArray,
            const MVPbkFieldTypeList& aMasterFieldTypeList );
        ~CVPbkVCardFieldTypeList();

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
        CVPbkVCardFieldTypeList();
        void ConstructL( const TArray<CVPbkVCardFieldTypeProperty*> aArray,
            const MVPbkFieldTypeList& aMasterFieldTypeList );

    private: // Data
        ///Own: field type array
        RPointerArray<MVPbkFieldType> iFieldTypes;
        ///Own: max match priority
        TInt iMaxMatchPriority;
    };


#endif // CVPBKVCARDFIELDTYPELIST_H
//End of file

