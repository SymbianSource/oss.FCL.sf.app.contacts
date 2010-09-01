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
*    vCard data field <-> UID mapping class.
*
*/


#ifndef __CBCARDFIELDUIDMAPPING_H__
#define __CBCARDFIELDUIDMAPPING_H__

//  INCLUDES
#include <e32base.h>    // CBase

// FORWARD DECLARATIONS
class TResourceReader;

// CLASS DECLARATION

/**
 * vCard data field <-> UID mapping.
 */
NONSHARABLE_CLASS(CBCardFieldUidMapping) : 
        public CBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class and initializes it from 
         * a PBK_NAME_UID_MAPPING resource.
		 * @param aReader reference to resources
         */
        static CBCardFieldUidMapping* NewL(TResourceReader& aReader);

        /**
         * Destructor.
         */
        ~CBCardFieldUidMapping();

    public:  // Types
        /**
         * vCard data field structure.
         */
        struct TVcardField
            {
            /// vCard property name
            TPtrC8 iName;
            /// vCard property sub field index
            TInt  iFieldIndex;

            /// Default constructor
            TVcardField();
            /// Constructor
            TVcardField(const TDesC8& aName, TInt aFieldIndex=0);
            };

        /**
         * vCard data field <-> UID mapping.
         */
        struct TMapping
            {
            TVcardField iVcardField;
            TUid iUid;
            };

    public:  // Interface
        /**
         * Returns the number of mappings held by this object.
         */
        TInt Count() const;

        /**
         * Returns the aIndex:th mapping held by this object.
         */
        TMapping operator[](TInt aIndex) const;

        /**
         * Finds a mapping for a vCard field.
         *
         * @param aVcardField   vCard field to search a mapping for.
         * @return  index of the mapping (for operator[]) or KErrNotFound if
         *          not found.
         * @see operator[]
         */
        TInt Find(const TVcardField& aVcardField) const;

        /**
         * Finds a mapping with an UID.
         *
         * @param aUid  UID to search for.
         * @return  index of the mapping (for operator[]) or KErrNotFound if
         *          not found.
         * @see operator[]
         */
        TInt Find(const TUid& aUid) const;

        /**
         * Finds the name of the field with an UID.
         *
         * @param aUid  UID to search for.
         * @return  The name of the mapping or KNullDesC if
         *          not found.
         * @see operator[]
         */
        TPtrC8 FindName(const TUid& aUid) const;
        
        /**
         * Finds the index of the field with an UID.
         *
         * @param aUid  UID to search for.
         * @return  The index of the mapping or KErrNotFound if
         *          not found.
         * @see operator[]
         */
        TInt FindIndex(const TUid& aUid) const;


    private:  // Implementation
        CBCardFieldUidMapping();
        void ConstructL(TResourceReader& aReader);


    private:  // Data
        struct TMappingImpl;
		/// Own: array of mappings
        RArray<TMappingImpl> iMappings;
    };


// INLINE FUNCTIONS
inline CBCardFieldUidMapping::TVcardField::TVcardField()
    {
    }

inline CBCardFieldUidMapping::TVcardField::TVcardField
        (const TDesC8& aName, TInt aFieldIndex/*=0*/) :
    iName(aName), iFieldIndex(aFieldIndex)
    {
    }

#endif // __CBCARDFIELDUIDMAPPING_H__

// End of File
