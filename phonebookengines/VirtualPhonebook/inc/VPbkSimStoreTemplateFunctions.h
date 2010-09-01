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
* Description:  Common templates for sim store
*
*/



#ifndef VPBKSIMSTORETEMPLATEFUNCTIONS_H
#define VPBKSIMSTORETEMPLATEFUNCTIONS_H

//  INCLUDES
#include <e32std.h>

namespace VPbkSimStoreImpl {

// -----------------------------------------------------------------------------
// CheckAndUpdateBufferSizeL
// Creates a buffer or enlarges it and sets the given pointer to it
// -----------------------------------------------------------------------------
//
template<class Buf, class BufPtr>
void CheckAndUpdateBufferSizeL( Buf*& aBuffer, BufPtr& aPtr,
    TInt aNewLength )
    {
    if ( !aBuffer )
        {
        aBuffer = Buf::NewL( aNewLength );
        aPtr.Set( aBuffer->Des() );
        }
    else
        {
        aPtr.Set( aBuffer->Des() );
        if ( aPtr.MaxLength() < aNewLength )
            {
            aBuffer = aBuffer->ReAllocL( aNewLength );
            aPtr.Set( aBuffer->Des() );
            }
        }
    }

// Function templates for calling observers
//
// Function naming convention:
// R: function takes reference argument
// V: function takes argument by value
//
// E.g In SendObserverMessageRVR the RVR means that the function takes
// a reference, a value and reference as an argument

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that doesn't take arguments
// -----------------------------------------------------------------------------
//
template<class Observer, class Function>
void SendObserverMessage(
    RPointerArray<Observer>& aObservers, Function aFunction)
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )();
        }
    }

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that take one reference argument
// -----------------------------------------------------------------------------
//
template<class Observer, class Function, class Param>
void SendObserverMessageR(
    RPointerArray<Observer>& aObservers, Function aFunction,
    Param& aParam )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam );
        }
    }

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that take a reference and a value
// -----------------------------------------------------------------------------
//
template<class Observer, class Function, class Param1, class Param2>
void SendObserverMessageRV(
    RPointerArray<Observer>& aObservers, Function aFunction,
    Param1& aParam1, Param2 aParam2 )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam1, aParam2 );
        }
    }

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that take two references
// -----------------------------------------------------------------------------
//
template<class Observer, class Function, class Param1, class Param2>
void SendObserverMessageRR(
    RPointerArray<Observer>& aObservers, Function aFunction,
    Param1& aParam1, Param2& aParam2 )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam1, aParam2 );
        }
    }

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that take two values
// -----------------------------------------------------------------------------
//
template<class Observer, class Function, class Param1, class Param2>
void SendObserverMessageVV(
    RPointerArray<Observer>& aObservers, Function aFunction,
    Param1 aParam1, Param2 aParam2 )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam1, aParam2 );
        }
    }

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that take three values
// -----------------------------------------------------------------------------
//
template<class Observer, class Function, class Param1, class Param2, class Param3>
void SendObserverMessageVVV(
    RPointerArray<Observer>& aObservers, Function aFunction,
    Param1 aParam1, Param2 aParam2, Param3 aParam3 )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam1, aParam2, aParam3 );
        }
    }

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that take a reference, value and reference
// -----------------------------------------------------------------------------
//
template<class Observer, class Function, class Param1, class Param2, class Param3>
void SendObserverMessageRVR(
    RPointerArray<Observer>& aObservers, Function aFunction,
    Param1& aParam1, Param2 aParam2, Param3& aParam3 )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam1, aParam2, aParam3 );
        }
    }

// -----------------------------------------------------------------------------
// SendObserverMessage
// For observer APIs that take a reference and two values
// -----------------------------------------------------------------------------
//
template<class Observer, class Function, class Param1, class Param2, class Param3>
void SendObserverMessageRVV(
    RPointerArray<Observer>& aObservers, Function aFunction,
    Param1& aParam1, Param2 aParam2, Param3 aParam3 )
    {
    const TInt count = aObservers.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        ( aObservers[i]->*aFunction )( aParam1, aParam2, aParam3 );
        }
    }

} // namespace VPbkSimStoreImpl

#endif      // VPBKSIMSTORETEMPLATEFUNCTIONS_H

// End of File
