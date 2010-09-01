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
* Description:  Phonebook 2 store info item.
*
*/


#ifndef CPBK2STOREINFOITEM_H
#define CPBK2STOREINFOITEM_H

//  INCLUDES
#include <e32base.h>
#include <bamdesca.h>
#include <Pbk2StoreInfo.hrh>

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATION

/**
 * Phonebook 2 store info item.
 */
NONSHARABLE_CLASS(CPbk2StoreInfoItem) : public CBase
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aReader   A resource reader pointed to a
         *                  PHONEBOOK2_STORE_INFO_ITEM resource.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2StoreInfoItem* NewLC(
                TResourceReader& aReader );

        /**
         * Destructor.
         */
        virtual ~CPbk2StoreInfoItem();

    public: // Interface

        /**
         * Returns the type of the item that is the link
         * to the function in Virtual Phonebook store info interface.
         *
         * @return  Type of the item.
         */
        IMPORT_C TInt ItemType() const;

        /**
         * Returns the item text.
         *
         * @return  Item text or KNullDesC.
         */
        IMPORT_C const TDesC& ItemTextSingular() const;

        /**
         * Returns the item text in plural form.
         *
         * @return  Item text in plural form or KNullDesC.
         */
        IMPORT_C const TDesC& ItemTextPlural() const;

        /**
         * Returns the item text type.
         *
         * @return  Item text type.
         */
        IMPORT_C TPbk2StoreInfoItemTextType TextType() const;

        /**
         * Returns item flags.
         *
         * @see Pbk2StoreProperty.hrh.
         * @return  Item flags.
         */
        IMPORT_C TUint32 Flags() const;

    private: // Implementation
        CPbk2StoreInfoItem();
        void ConstructL(
                TResourceReader& aReader );

    private: // Data
        /// Own: Info item type
        TInt iItemType;
        /// Own: Item text
        HBufC* iItemTextSingular;
        /// Own: Item text in plural form
        HBufC* iItemTextPlural;
        /// Own: Info item text type
        TPbk2StoreInfoItemTextType iTextType;
        /// Own: Flags
        TUint32 iFlags;
    };

#endif // CPBK2STOREINFOITEM_H

// End of File
