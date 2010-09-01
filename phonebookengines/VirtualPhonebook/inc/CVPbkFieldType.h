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
* Description:  Field type
*
*/


#ifndef CVPBKFIELDTYPE_H
#define CVPBKFIELDTYPE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldTypeParameters.h>

// FORWARD DECLARATIONS
class RResourceFile;
class TResourceReader;
class TVPbkFieldVersitProperty;
namespace VPbkEngUtils
    {
    class CTextStore;
    }

// CLASS DECLARATIONS

/**
 * Virtual Phonebook field type.
 */
NONSHARABLE_CLASS(CVPbkFieldType) : public CBase, public MVPbkFieldType
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class from a VPBK_FIELD_TYPE 
         * resource.
         *
         * @param aResFile      Resource file.
         * @param aResReader    Resource reader pointed to
         *                      VPBK_FIELD_TYPE resource.
         * @param aTextStore    Store for field type TPtrs.
         * @return  A new instance of this class.
         */
        static CVPbkFieldType* NewLC(
                RResourceFile& aResFile,
                TResourceReader& aResReader, 
                VPbkEngUtils::CTextStore& aTextStore );

        /**
         * Destructor.
         */
        ~CVPbkFieldType();

    public: // From MVPbkFieldType
        TArray<TVPbkFieldVersitProperty> VersitProperties() const;
        const TVPbkFieldTypeParameters& ExcludedParameters() const;
        TVPbkNonVersitFieldType NonVersitType() const;
        TBool IsSame(
                const MVPbkFieldType& aOtherType ) const;
        TBool Matches(
                const TVPbkFieldVersitProperty& aMatchProperty,
                TInt aMatchPriority ) const;
        TInt FieldTypeResId() const;

    private:  // Implementation
        CVPbkFieldType();
        void ConstructL(
                RResourceFile& aResFile, 
                TResourceReader& aResReader,
                VPbkEngUtils::CTextStore& aTextStore );

    private: // Data
        /// Own: Field type resource id
        TInt iFieldTypeResId;
        /// Own: Field type versit properties
        RArray<TVPbkFieldVersitProperty> iVersitProperties;
        /// Own: Exclude parameters
        TVPbkFieldTypeParameters iExcludedTypeParameters;
        /// Own: Non-Versit type
        TVPbkNonVersitFieldType iNonVersitType;
    };
    
#endif // CVPBKFIELDTYPE_H

//End of File
