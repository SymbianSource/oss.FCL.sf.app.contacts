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
* Description:  A class that reads an array of PSU2_COPY_TO_SIM_FIELDTYPE_INFO
*                items
*
*/



#ifndef CPSU2COPYTOSIMFIELDINFOARRAY_H
#define CPSU2COPYTOSIMFIELDINFOARRAY_H

//  INCLUDES
#include <e32base.h>
#include "Pbk2USimUI.hrh"

// FORWARD DECLARATIONS
class TResourceReader;
class MVPbkFieldType;
class MVPbkFieldTypeList;
class RFs;
class CPsu2CopyToSimFieldtypeMapping;

// CLASS DECLARATION

class TPsu2CopyToSimFieldInfo
    {
    public:  // Constructors and destructor
        /**
         * C++ constructor
         *
         * @param aSourceType   Source field type
         * @param aSimType      Sim field type
         * @param aFlags        Flags
         * @param aBlockingError    Error code
         */
        TPsu2CopyToSimFieldInfo( const MVPbkFieldType& aSourceType,
            const MVPbkFieldType& aSimType, TUint8 aFlags, 
            TUint8 aBlockingError );

    public: // New functions

        /**
        * @return the field type that has a conversion to the sim type
        */
        inline const MVPbkFieldType& SourceType() const;

        /**
        * @return the sim type that must be used before saving to sim
        */
        inline const MVPbkFieldType& SimType() const;

        /**
        * @return ETrue if field can be truncated
        */
        inline TBool TruncationAllowed() const;
        
        /**
        * @param aBlockingError an error that is compared
        * @return ETrue if aBlockingError means 
        */
        inline TBool BlockedByError( TPsu2ErrorCode aBlockingError ) const;

    private:  // Data
        /// Ref: a field type that possible can be copied to the sim
        const MVPbkFieldType& iSourceType;
        /// Ref: the sim type for the iFieldType
        const MVPbkFieldType& iSimType;
        /// Field info flags
        TUint8 iFlags;
        /// A saving time error that tells that iSimType is not possible
        /// to save. See TPsu2ErrorCode
        TUint8 iBlockingError;
    };
    
/**
*  A class that reads an array of PSU2_COPY_TO_SIM_FIELDTYPE_INFO items
*
*/
class CPsu2CopyToSimFieldInfoArray : public CBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @param aMasterFieldTypeList list of all field types
        * @param aFs an open file system session for reading
        *   resources
        * @return a new instance of this class
        */
        static CPsu2CopyToSimFieldInfoArray* NewL(
            const MVPbkFieldTypeList& aMasterFieldTypeList,
            RFs& aFs );

        /**
        * Destructor.
        */
        virtual ~CPsu2CopyToSimFieldInfoArray();

    public: // New functions

        /**
        * @return the number of items in the array
        */
        inline TInt Count() const;

        /**
        * @param aIndex the index of the item in the array
        * @return a reference to item in given index
        */
        inline const TPsu2CopyToSimFieldInfo& operator[](TInt aIndex) const;

        
        /**
        * @param aSourceType the source field type
        * @return the info that for the aSourceType or NULL
        */
        const TPsu2CopyToSimFieldInfo* FindInfoForSourceType(
                const MVPbkFieldType& aSourceType );
        
        /**
        * @return the field type of the sim name field
        */
        const MVPbkFieldType& SimNameType() const;
        
        /**
        * @return the field type of SIM reading field
        */
        const MVPbkFieldType& LastNameReadingType() const;
        
        /**
        * @param aSourceType a type of the source contact field that has
        *        a conversion to sim type.
        * @return the corresponding sim type or NULL
        */
        const MVPbkFieldType* ConvertToSimType(const MVPbkFieldType& aSourceType);

        /**
        * @return ETrue if the fields of given field type must be truncated
        *         when data is too long
        */
        TBool TruncationAllowed( const MVPbkFieldType& aSimType );

        /**
        * @return The standard number keymap
        */
        const TDesC& NumberKeyMap() const;

    private:

        /**
        * C++ default constructor.
        */
        CPsu2CopyToSimFieldInfoArray(
            const MVPbkFieldTypeList& aMasterFieldTypeList);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL( RFs& aFs );
        void CreateMappingsL( CPsu2CopyToSimFieldtypeMapping& aCMapping );

    private:    // Data
        /// The list of all types
        const MVPbkFieldTypeList& iMasterFieldTypeList;
        /// Ref: the sim name field type
        const MVPbkFieldType* iSimNameType;
        /// Ref: the sim name field type
        const MVPbkFieldType* iSimReadingType;
        /// An array of field infos
        RArray<TPsu2CopyToSimFieldInfo> iInfoArray;
        /// An array for finding types that caused an error
        RPointerArray<const MVPbkFieldType> iMatchedTypes;
        /// Own: valid characters for number key map
        HBufC* iNumberKeyMap;
    };

// INLINE FUNCTIONS

// -----------------------------------------------------------------------------
// TPsu2CopyToSimFieldInfo::SourceType
// -----------------------------------------------------------------------------
//
inline const MVPbkFieldType& TPsu2CopyToSimFieldInfo::SourceType() const
    {
    return iSourceType;
    }

// -----------------------------------------------------------------------------
// TPsu2CopyToSimFieldInfo::SimType
// -----------------------------------------------------------------------------
//
inline const MVPbkFieldType& TPsu2CopyToSimFieldInfo::SimType() const
    {
    return iSimType;
    }

// -----------------------------------------------------------------------------
// TPsu2CopyToSimFieldInfo::TruncationAllowed
// -----------------------------------------------------------------------------
//
inline TBool TPsu2CopyToSimFieldInfo::TruncationAllowed() const
    {
    return iFlags & KPsu2AllowedToTruncate;
    }

// -----------------------------------------------------------------------------
// TPsu2CopyToSimFieldInfo::BlockedByError
// -----------------------------------------------------------------------------
//
inline TBool TPsu2CopyToSimFieldInfo::BlockedByError( 
        TPsu2ErrorCode aBlockingError ) const
    {
    return iBlockingError == aBlockingError;
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::Count
// -----------------------------------------------------------------------------
//
inline TInt CPsu2CopyToSimFieldInfoArray::Count() const
    {
    return iInfoArray.Count();
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::operator[]
// -----------------------------------------------------------------------------
//
inline const TPsu2CopyToSimFieldInfo&
    CPsu2CopyToSimFieldInfoArray::operator[](TInt aIndex) const
    {
    return iInfoArray[aIndex];
    }
#endif      // CPSU2COPYTOSIMFIELDINFOARRAY_H

// End of File
