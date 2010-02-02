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
* Description: 
*
*/


#ifndef VPBKENGUTILS_VPBKSENDEVENTUTILITY_H
#define VPBKENGUTILS_VPBKSENDEVENTUTILITY_H


// INCLUDES
#include <e32cmn.h>

// FORWARD DECLARATIONS

namespace VPbkEng {

// -----------------------------------------------------------------------------
// SendEventToObservers
// -----------------------------------------------------------------------------
//
template<typename RefParam1, typename Observer, typename NotifyFunc>
void SendEventToObservers( RefParam1& aRefParam1,
        RPointerArray<Observer>& aObservers, NotifyFunc aNotifyFunc )
    {
    Observer* obs = NULL;
    for ( TInt i = aObservers.Count() - 1; i >= 0; --i )
        {
        if ( i < aObservers.Count() && obs != aObservers[i] )
            {
            obs = aObservers[i];
            (obs->*aNotifyFunc)( aRefParam1 );
            }
        }
    }

// -----------------------------------------------------------------------------
// SendEventToObservers
// -----------------------------------------------------------------------------
//
template<typename RefParam1, typename ValParam2, typename RefParam3,
    typename Observer, typename NotifyFunc>
void SendEventToObservers( RefParam1& aRefParam1, ValParam2 aValParam2,
        RefParam3& aRefParam3, RPointerArray<Observer>& aObservers,
        NotifyFunc aNotifyFunc )
    {
    Observer* obs = NULL;
    for ( TInt i = aObservers.Count() - 1; i >= 0; --i )
        {
        if ( i < aObservers.Count() && obs != aObservers[i] )
            {
            obs = aObservers[i];
            (obs->*aNotifyFunc)( aRefParam1, aValParam2,
                    aRefParam3 );
            }
        }
    }


// -----------------------------------------------------------------------------
// CopyPointerArrayL
// -----------------------------------------------------------------------------
//
template<typename ObjectTypeName>
void CopyPointerArrayL( RPointerArray<ObjectTypeName>& aSource,
        RPointerArray<ObjectTypeName>& aTarget )
    {
    const TInt count = aSource.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        aTarget.AppendL( aSource[i] );
        }
    }

// -----------------------------------------------------------------------------
// SendViewEventToObserversL
// See and use SendViewEventToObservers if possible
// -----------------------------------------------------------------------------
//
template<typename NotifyFunc, typename Observer, typename RefParam1>
void SendViewEventToObserversL( RefParam1& aRefParam1,
        RPointerArray<Observer>& aObservers, NotifyFunc aNotifyFunc )
    {
    RPointerArray<Observer> tmp;
    CleanupClosePushL( tmp );

    CopyPointerArrayL( aObservers, tmp );

    const TInt count = tmp.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        if ( aObservers.Find( tmp[i] ) != KErrNotFound )
            {
            (tmp[i]->*aNotifyFunc)( aRefParam1 );
            }
        }

    CleanupStack::PopAndDestroy(); // tmp
    }

// -----------------------------------------------------------------------------
// SendViewEventToObserversL
// See and use SendViewEventToObservers if possible
// -----------------------------------------------------------------------------
//

template<typename NotifyFunc, typename Observer, typename RefParam1,
    typename ValParam2, typename RefParam3>
void SendViewEventToObserversL( RefParam1& aRefParam1, ValParam2 aValParam2,
        RefParam3& aRefParam3, RPointerArray<Observer>& aObservers,
        NotifyFunc aNotifyFunc )
    {
    RPointerArray<Observer> tmp;
    CleanupClosePushL( tmp );

    CopyPointerArrayL( aObservers, tmp );

    const TInt count = tmp.Count();
    for ( TInt i = count - 1; i >= 0; --i )
        {
        if ( aObservers.Find( tmp[i] ) != KErrNotFound )
            {
            (tmp[i]->*aNotifyFunc)( aRefParam1, aValParam2, aRefParam3 );
            }
        }

    CleanupStack::PopAndDestroy(); // tmp
    }


// -----------------------------------------------------------------------------
// SendViewEventToObservers
//
// Usage:
// CMyView::HandleNativeViewReady()
//     {
//     ...
//     SendViewEventToObservers( *this, iObservers,
//          &MVPbkContactViewObserver::ContactViewReady,
//          &MVPbkContactViewObserver::ContactViewError )
//     }
//
// The function slightly bound to MVPbkContactViewBase::ContactViewError for
// error handling. If that doesn't suite your needs then use
// SendViewEventToObserversL and handle error yourself.
// -----------------------------------------------------------------------------
//
template<typename NotifyFunc, typename ErrorFunc, typename Observer,
    typename RefParam1>
void SendViewEventToObservers( RefParam1& aRefParam1,
        RPointerArray<Observer>& aObservers, NotifyFunc aNotifyFunc,
        ErrorFunc aErrorFunc )
    {
    TRAPD( result,
        SendViewEventToObserversL( aRefParam1, aObservers, aNotifyFunc ) );
    if ( result != KErrNone )
        {
        TBool errorNotified = EFalse;
        SendEventToObservers( aRefParam1, result, errorNotified,
            aObservers, aErrorFunc );
        }
    }

// -----------------------------------------------------------------------------
// SendViewEventToObservers
//
// Usage:
// CMyView::HandleNativeViewContactAdded(TInt aIndex, TContactId aId)
//     {
//     ...
//     SendViewEventToObservers( *this, aIndex, *iLink, iObservers,
//          &MVPbkContactViewObserver::ContactAddedToView,
//          &MVPbkContactViewObserver::ContactViewError )
//     }
//
// The function slightly bound to MVPbkContactViewBase::ContactViewError for
// error handling. If that doesn't suite your needs then use
// SendViewEventToObserversL and handle error yourself.
// -----------------------------------------------------------------------------
//
template<typename NotifyFunc, typename ErrorFunc, typename Observer,
    typename RefParam1, typename ValParam2, typename RefParam3>
void SendViewEventToObservers( RefParam1& aRefParam1, ValParam2 aValParam2,
        RefParam3& aRefParam3, RPointerArray<Observer>& aObservers,
        NotifyFunc aNotifyFunc, ErrorFunc aErrorFunc )
    {
    TRAPD( result, SendViewEventToObserversL( aRefParam1, aValParam2,
            aRefParam3, aObservers, aNotifyFunc ) );
    if ( result != KErrNone )
        {
        TBool errorNotified = EFalse;
        SendEventToObservers( aRefParam1, result, errorNotified,
            aObservers, aErrorFunc );
        }
    }
} // namespace VPbkEng


#endif  // VPBKENGUTILS_VPBKSENDEVENTUTILITY_H
// End of file


