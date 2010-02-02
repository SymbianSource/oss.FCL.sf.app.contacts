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
* Description:  Contains list of field types
*
*/


#ifndef CVPBKFIELDTYPELIST_H
#define CVPBKFIELDTYPELIST_H

// INCLUDES
#include <e32base.h>
#include <CVPbkFieldType.h>

// FORWARD DECLARATIONS
class RResourceFile;
class TResourceReader;
namespace VPbkEngUtils
    {
    class CTextStore;
    }


// CLASS DECLARATIONS

/**
 * A collection of Virtual Phonebook field types.
 */
class CVPbkFieldTypeList : public CBase, public MVPbkFieldTypeList
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class from a VPBK_FIELD_TYPE_LIST
         * resource.
         *
         * @param aResFile      Resource file.
         * @param aResReader    Resource reader pointed
         *                      to VPBK_FIELD_TYPE_LIST resource.
         * @return  A new instance of this class.
         */
        IMPORT_C static CVPbkFieldTypeList* NewL(
                RResourceFile& aResFile,
                TResourceReader& aResReader );

        /**
         * Destructor.
         */
        ~CVPbkFieldTypeList();

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
        CVPbkFieldTypeList();
        void ConstructL(
                RResourceFile& aResFile,
                TResourceReader& aResReader );

    private: // Data
        /// Own: X- Versit type storage
        VPbkEngUtils::CTextStore* iTextStore;
        /// Own: Array of field types
        RPointerArray<CVPbkFieldType> iFieldTypes;
        /// Own: Max match priority
        TInt iMaxMatchPriority;
    };

#endif // CVPBKFIELDTYPELIST_H

//End of File
