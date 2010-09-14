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
* Description:  Phonebook 2 field ordering manager.
*
*/


#ifndef CPBK2FIELDORDERINGMANAGER_H
#define CPBK2FIELDORDERINGMANAGER_H

// INCLUDES
#include <e32base.h> // CBase

// FORWARD DECLARATIONS
class MVPbkFieldType;
class TResourceReader;

// CLASS DECLARATIONS

NONSHARABLE_CLASS( CPbk2FieldOrderingManager )
:   public CBase
    {
    public:
        /**
         * Static constructor
         *
         * @param aReader a resource reader to the array of 
         *        PBK2_FIELDTYPE_PRESENTATION_ORDER structures.
         */
        static CPbk2FieldOrderingManager* NewL( TResourceReader& aReader );

        /**
         * Destructor
         */
        ~CPbk2FieldOrderingManager();

    public: // Interface
        /**
         * Returns field ordering of aFieldType based on feature manager.
         * @param aFieldType A field type which order will be resolved.
         * @return Ordering of the aFieldType.
         */
        TInt OrderingItem( const MVPbkFieldType& aFieldType );

        /**
         * Returns add field ordering of aFieldType based on feature manager.
         * @param aFieldType A field type which order will be resolved.
         * @return Add ordering of the aFieldType.
         */
        TInt AddItemOrdering( const MVPbkFieldType& aFieldType );

    private: // Implementation
        CPbk2FieldOrderingManager();
        void ConstructL( TResourceReader& aReader );
        TInt ItemIndex( TInt aFieldTypeResId );

    private: // Data
        class TFieldTypeOrder
        {
        public:
            /**
             * @param aReader a resource reader to the structure
             * PBK2_FIELDTYPE_PRESENTATION_ORDER
             */
            TFieldTypeOrder( TResourceReader& aReader );
            
            /**
             * Field type resource id from vpbkeng.rsg
             */
            TInt iFieldTypeResId;
            
            /**
             * The number that specifies the location of this field type
             * in contact presentation (defines the order of fields). 
             */
            TInt8 iOrderingItem;
            
            /**
             * The number that specifies the location of this field type
             * in the UI when adding an new fieldtype to the contact.
             */
            TInt8 iAddItemOrdering;
        };
        
        /// Own: an array of ordering items
        RArray<TFieldTypeOrder> iOrderingItems;
    };

#endif // CPBK2FIELDORDERINGMANAGER_H

// End of file
