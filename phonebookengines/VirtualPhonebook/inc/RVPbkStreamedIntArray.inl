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


// INLINE FUNCTIONS

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::RVPbkStreamedIntArray
// -----------------------------------------------------------------------------
//
inline RVPbkStreamedIntArray::RVPbkStreamedIntArray()
    {
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::Close
// -----------------------------------------------------------------------------
//
inline void RVPbkStreamedIntArray::Close()
    {
    iIntArray.Close();
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::AppendIntL
// -----------------------------------------------------------------------------
//
inline void RVPbkStreamedIntArray::AppendIntL( TInt aInt )
    {
    iIntArray.AppendL( aInt );
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::Find
// -----------------------------------------------------------------------------
//
inline TInt RVPbkStreamedIntArray::Find( TInt aInt )
    {
    return iIntArray.Find( aInt );
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::operator[]
// -----------------------------------------------------------------------------
//
inline TInt RVPbkStreamedIntArray::operator[]( TInt aIndex ) const
    {
    return iIntArray[aIndex];
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::Count
// -----------------------------------------------------------------------------
//
inline TInt RVPbkStreamedIntArray::Count() const
    {
    return iIntArray.Count();
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedIntArray::Reset
// -----------------------------------------------------------------------------
//
inline void RVPbkStreamedIntArray::Reset()
    {
    iIntArray.Reset();
    }


// -----------------------------------------------------------------------------
// RVPbkStreamedEnumArray::RVPbkStreamedEnumArray
// -----------------------------------------------------------------------------
//
template<class T>
inline RVPbkStreamedEnumArray<T>::RVPbkStreamedEnumArray()
    {
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedEnumArray::operator[]
// -----------------------------------------------------------------------------
//
template<class T>
inline T RVPbkStreamedEnumArray<T>::operator[]( 
    TInt aIndex ) const
    {
    return static_cast<T>( RVPbkStreamedIntArray::operator[](aIndex) );
    }

// -----------------------------------------------------------------------------
// RVPbkStreamedEnumArray::AppendL
// -----------------------------------------------------------------------------
//
template<class T>
inline void RVPbkStreamedEnumArray<T>::AppendL( T aEnum )
    {
    RVPbkStreamedIntArray::AppendIntL( aEnum );
    }

// -----------------------------------------------------------------------------
// RVPbkSimFieldTypeArray::RVPbkSimFieldTypeArray
// -----------------------------------------------------------------------------
//
inline RVPbkSimFieldTypeArray::RVPbkSimFieldTypeArray()
    {
    }
            
// End of File
