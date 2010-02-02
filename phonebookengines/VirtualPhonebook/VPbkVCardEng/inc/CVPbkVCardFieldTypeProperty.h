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
* Description:  
*
*/


#ifndef CVPBKVCARDFIELDTYPEPROPERTY_H
#define CVPBKVCARDFIELDTYPEPROPERTY_H

// INCLUDES
#include "VPbkVCard.hrh"
#include <e32base.h>
#include <MVPbkFieldType.h>
#include <TVPbkFieldTypeParameters.h>
#include "TVPbkVCardFieldTypeMapping.h"

// FORWARD DECLARATIONS
class TResourceReader;
namespace VPbkEngUtils
    {
    class CTextStore;
    }

// CLASS DECLARATIONS

NONSHARABLE_CLASS( TVPbkVCardIndexedProperty )
    {
    public:  // Constructors and destructor
        /**
         * Initializes a new instance of this class from a VCARD_INDEXED_PROPERTY 
         * resource.
         */
        void InitializeL
            ( TResourceReader& aResReader,
              VPbkEngUtils::CTextStore& aTextStore );

    public: // New functions

        TInt Index() const;
        TVPbkVCardFieldTypeMapping Property() const;
        const MVPbkFieldType* 
            FindMatch(const MVPbkFieldTypeList& aFieldTypeList) const;

    private: // Data
        TInt                        iIndex;
        TVPbkVCardFieldTypeMapping  iProperty;
    };

/**
 * Virtual Phonebook field type.
 */
NONSHARABLE_CLASS(CVPbkVCardFieldTypeProperty) : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class from a VCARD_FIELDTYPE_PROPERTY 
         * resource.
         */
        static CVPbkVCardFieldTypeProperty* NewLC
            ( TResourceReader& aResReader, 
              VPbkEngUtils::CTextStore& aTextStore );

        ~CVPbkVCardFieldTypeProperty();

    public: 
        const TVPbkVCardIndexedProperty& IndexedPropertyAt( TInt aIndex ) const;
        const TDesC8& Name() const;
        TVPbkVersitStorageType StorageType() const;
        TInt Count() const;

    private:  // Implementation
        CVPbkVCardFieldTypeProperty();
        void ConstructL( TResourceReader& aResReader, 
              VPbkEngUtils::CTextStore& aTextStore );

    private:  // Data
        HBufC8*                         iName;
        TVPbkVersitStorageType          iStorageType;
        RArray<TVPbkVCardIndexedProperty>   iIndexedProperties;
    };
    
#endif // CVPBKVCARDFIELDTYPEPROPERTY_H
//End of file

