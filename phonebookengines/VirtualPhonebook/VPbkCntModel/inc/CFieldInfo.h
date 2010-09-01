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
* Description:  The virtual phonebook field info
*
*/


#ifndef CFIELDINFO_H
#define CFIELDINFO_H

// INCLUDE FILES
#include <e32base.h>
#include <cntdef.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CContactItemField;
class CContentType;

namespace VPbkCntModel {

NONSHARABLE_CLASS(CFieldInfo) : public CBase
    {
    public:
        static CFieldInfo* NewL(TResourceReader& aReader);
        ~CFieldInfo();

    public: // Interface
        const TDesC& FieldName() const;
        TBool IsEqualType(const CContactItemField& aField) const;
        TBool IsEqualLabel(const CContactItemField& aField) const;
        CContactItemField* CreateFieldL() const;        

    private: // Implementation
        void ConstructL(TResourceReader& aReader);
        CFieldInfo();
        TBool IsEqualFlags(const CContactItemField& aField) const;

    private: // Data
        /// Own: storage type (Text,Date,...)
        TStorageType iFieldStorageType;		// LONG fieldStorageType
		/// Own: content type
        CContentType* iContentType;			// LONG contactFieldType,
											// LONG vCardMapping,
											// STRUCT extraMapping[]
		/// Own: category (Home/Work)
        TInt iCategory;						// LONG category
		/// Own: field's default name and label
        HBufC* iFieldName;					// LTEXT fieldName
		/// Own: special flags
        TUint iFlags;						// LONG flags
	};

NONSHARABLE_CLASS(CFieldsInfo) : public CArrayPtrFlat<CFieldInfo>
    {
    public:
        static CFieldsInfo* NewL(TResourceReader& aReader);
        ~CFieldsInfo();

    public:
        /**
         * Appends field infos to the structure.
         */
        void AppendFieldsL(TResourceReader& aReader);

    private: // Implementation
        void ConstructL(TResourceReader& aReader);
        CFieldsInfo();
    };

inline const TDesC& CFieldInfo::FieldName() const
    {
    return *iFieldName;
    }

} // namespace

#endif // CFIELDINFO_H
//End of file
