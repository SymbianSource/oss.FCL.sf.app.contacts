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
*    BCard export property.
*
*/


#ifndef __CBCARDEXPORTPROPERTY_H__
#define __CBCARDEXPORTPROPERTY_H__

//  INCLUDES
#include <e32base.h>        // CBase
#include "PbkFields.hrh"    // TPbkVersitStorageType

// FORWARD DECLARATIONS
class TResourceReader;
class CPbkFieldInfo;
class CPbkFieldsInfo;

// CLASS DECLARATION

/**
 * Phonebook vCard property.
 */
NONSHARABLE_CLASS(CBCardExportProperty) : public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class and intializes it from 
         * resources.
		 * @param aReader reference to resources
		 * @param aFieldsInfo fields info reference
         */
        static CBCardExportProperty* NewLC(
            TResourceReader& aReader, 
            const CPbkFieldsInfo& aFieldsInfo);

        /**
         * Destructor.
         */
        ~CBCardExportProperty();

    public:  // Implementation
         const TDesC8& Name() const;
         TPbkVersitStorageType StorageType() const;
         TInt InfoCount() const;
         const CPbkFieldInfo* InfoAt(TInt aIndex) const;
        
    private:  // Implementation
        CBCardExportProperty();
        void ConstructL(TResourceReader& aReader, const CPbkFieldsInfo& fieldsInfo);

    private:  // Data
		/// Own: name
        HBufC8*                         iName;
		/// Own: storage type
        TPbkVersitStorageType           iStorageType;
		/// Own: field info array
        RPointerArray<CPbkFieldInfo>    iFieldInfos;
    };


#endif // __CBCARDEXPORTPROPERTY_H__

// End of File
