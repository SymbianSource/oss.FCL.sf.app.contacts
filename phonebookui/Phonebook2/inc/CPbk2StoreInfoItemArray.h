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
* Description:  Phonebook 2 store info item array.
*
*/


#ifndef CPBK2STOREINFOITEMARRAY_H
#define CPBK2STOREINFOITEMARRAY_H

//  INCLUDES
#include <e32base.h>
#include <e32cmn.h>

// FORWARD DECLARATIONS
class TResourceReader;
class CPbk2StoreInfoItem;

// CLASS DECLARATION

/**
 * Phonebook 2 store info item array.
 */
NONSHARABLE_CLASS(CPbk2StoreInfoItemArray) : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aReader   A resource reader pointed to
         *                  an array of PHONEBOOK2_STORE_INFO_ITEMs.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreInfoItemArray* NewL(
                TResourceReader& aReader );

        /**
         * Destructor.
         */
        virtual ~CPbk2StoreInfoItemArray();

    public: // Interface

        /**
         * Returns the number of items in the array.
         *
         * @return  Number of items in the array.
         */
        IMPORT_C TInt Count() const;

        /**
         * Returns a store info item in given location.
         *
         * @return  Store info item in given location.
         */
        IMPORT_C const CPbk2StoreInfoItem& At(
                TInt aIndex ) const;

    private: // Implementation
        CPbk2StoreInfoItemArray();
        void ConstructL(
                TResourceReader& aReader );

    private: // Data
        /// Own: An array of store info items
        RPointerArray<CPbk2StoreInfoItem> iItems;
    };

#endif // CPBK2STOREINFOITEMARRAY_H

// End of File
