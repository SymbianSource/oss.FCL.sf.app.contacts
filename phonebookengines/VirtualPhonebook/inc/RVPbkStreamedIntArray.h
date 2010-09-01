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
* Description:  TInt array that can be streamed
*
*/



#ifndef RVPBKSTREAMEDINTARRAY_H
#define RVPBKSTREAMEDINTARRAY_H

//  INCLUDES
#include <e32std.h>
#include "VPbkSimCntFieldTypes.hrh"

// FORWARD DECLARATIONS
class RWriteStream;
class RReadStream;
class RVPbkStreamedIntArray;

// FUNCTION PROTOTYPES

/// Returns a buffer containg the content of the array
IMPORT_C HBufC8* ExternalizeArrayLC( 
    const RVPbkStreamedIntArray& aFieldTypes );

// CLASS DECLARATION

/**
*  Serializable TInt array
*
*  @lib VPbkSimStoreImpl.lib
*/
class RVPbkStreamedIntArray
    {
    public:  // Constructors

        /**
        * C++ default constructor.
        */
        inline RVPbkStreamedIntArray();

    public: // New functions
        
        /**
        * Externalizes the instance
        * @param aWriteStream the stream to write to.
        */
        IMPORT_C void ExternalizeL( RWriteStream& aWriteStream ) const;

        /**
        * Returns the size of the data in the instance
        * @return the size of the data in the instance
        */
        IMPORT_C TInt ExternalizedSize() const;

        /**
        * Initializes the instance from the stream
        * @param aReadStream the stream to read from.
        */
        IMPORT_C void InternalizeL( RReadStream& aReadStream );

        /**
        * Deletes all the items from the array but doesn't free the memory
        */
        IMPORT_C void RemoveAll();

        /**
        * Closes this array
        */
        inline void Close();

                /**
        * Returns the amount of the types in the array
        * @return the amount of the types in the array
        */
	    inline TInt Count() const;

        /**
        * Resets the sort order
        */
        inline void Reset();

        /**
        * Appends a value to the array
        * @param aField the field type
        */
        inline void AppendIntL( TInt aInt );

        /**
        * Finds the given value from the array
        * @return index or KErrNotFound
        */
        inline TInt Find( TInt aInt );

        /**
        * Returns the type in the specfied index
        * @return the type in the specfied index.
        */
	    inline TInt operator[]( TInt aIndex ) const;
    
    private: // New functions

        // Prohibit copy constructor if not deriving from CBase.
        RVPbkStreamedIntArray( const RVPbkStreamedIntArray& );
        // Prohibit assigment operator if not deriving from CBase.
        RVPbkStreamedIntArray& operator=( 
            const RVPbkStreamedIntArray& );

    private: // Data
        RArray<TInt> iIntArray;
    };

/**
*  Streamed array for types that have a conversion to TInt
*
*  @lib VPbkSimStoreImpl.lib
*/
template<class T>
class RVPbkStreamedEnumArray : public RVPbkStreamedIntArray
    {
    public:  // Constructors
        
        /**
        * C++ default constructor.
        */
        inline RVPbkStreamedEnumArray();
        
    public: // New functions
        
        /**
        * Returns the type in the specfied index
        * @return the type in the specfied index.
        */
	    inline T operator[]( TInt aIndex ) const;

        /**
        * Appends an new value to the array
        * @param aEnum the new value
        */
        inline void AppendL( T aEnum );
    };


/**
*  Streamed sort order array for SIM field types
*
*  @lib VPbkSimStoreImpl.lib
*/
class RVPbkSimFieldTypeArray : 
    public RVPbkStreamedEnumArray<TVPbkSimCntFieldType>
    {
    public:  // Constructors
        
        /**
        * C++ default constructor.
        */
        inline RVPbkSimFieldTypeArray();
    };

#include "RVPbkStreamedIntArray.inl"
#endif      // RVPBKSTREAMEDINTARRAY_H
            
// End of File
