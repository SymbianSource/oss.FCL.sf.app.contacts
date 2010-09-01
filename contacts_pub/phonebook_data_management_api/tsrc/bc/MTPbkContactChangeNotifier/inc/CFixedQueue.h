/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
*       Provides methods for test dlls.
*
*/








#ifndef __CFixedQueue_H__
#define __CFixedQueue_H__

// INCLUDES
#include <e32base.h>

// CONSTANTS
_LIT(KFixedQueuePanicText, "CFixedQueue<T>");


// CLASS DECLARATION

/**
 * Fixed size queue.
 * @param T type of objects in the queue. T must define default constructor
 *          and assignment operator.
 */
template<class T>
class CFixedQueue : public CBase
    {
    public:  // Constructors and destructor
        CFixedQueue();

        /**
         * Initializes a new fixed queue.
         * @param aSize maximum size of the queue.
         * @precond aSize > 0
         */
        void ConstructL(TInt aSize);

        ~CFixedQueue();

    public:  // Interface
        /**
         * Returns the object at the head of this queue.
         * @precond !IsEmpty()
         */
        T& Head() const;

        /**
         * Pops the head off this queue. Does not deallocate memory.
         * @precond !IsEmpty()
         */
        void PopHead();

        /**
         * Returns true if this queue is empty
         */
        TBool IsEmpty() const;

        /**
         * Returns the number of objects in the queue.
         */
        TInt Count() const;

        /**
         * Pushes an object at the tail of the queue.
         * @return ETrue if succesful, EFalse if queue run out of space.
         */ 
        TBool Push(const T& aObj);

        /**
         * Empties this queue. Does not deallocate memory.
         */
        void Reset();

        void __DbgTestInvariant() const;

    private:  // Implementation
        enum TPanicCode
            {
            EPanicPreCond_ConstructL = 1,
            EPanicPreCond_Head,
            EPanicPreCond_PopHead,
            EPanicPreCond_Push,
            EPanicInvariant
            };
        static void Panic(TInt aReason);

    private:  // Data
        /// Own: circural queue array
        T* iQueueArray;
        /// Own: maximum size of event queue
        TInt iQueueSize;
        /// Own: number of objects currently in the queue
        TInt iCount;
        /// Ref: head of queue
        T* iQueueHead;
        /// Ref: tail of queue
        T* iQueueTail;
    };

template<class T> inline
void CFixedQueue<T>::Panic(TInt aReason)
    {
    User::Panic(KFixedQueuePanicText,aReason);
    }

template<class T> inline
CFixedQueue<T>::CFixedQueue()
    {
    // CBase::operator new resets memebers
    __TEST_INVARIANT;
    }

template<class T> inline
void CFixedQueue<T>::ConstructL(TInt aSize)
    {
    __TEST_INVARIANT;
    __ASSERT_DEBUG(!iQueueArray, Panic(EPanicPreCond_ConstructL));
    __ASSERT_DEBUG(aSize > 0, Panic(EPanicPreCond_ConstructL));
    iQueueArray = new(ELeave) T[aSize];
    iQueueSize = aSize;
    iCount = 0;
    iQueueHead = iQueueTail = iQueueArray;
    __TEST_INVARIANT;
    }

template<class T>
CFixedQueue<T>::~CFixedQueue()
    {
    __TEST_INVARIANT;
    delete [] iQueueArray;
    }

template<class T> inline
T& CFixedQueue<T>::Head() const
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(iCount > 0, Panic(EPanicPreCond_Head));
    return *iQueueHead;
    }

template<class T> inline
void CFixedQueue<T>::PopHead()
    {
    __TEST_INVARIANT;
    __ASSERT_ALWAYS(iCount > 0, Panic(EPanicPreCond_PopHead));
    if (++iQueueHead == iQueueArray+iQueueSize)
        {
        iQueueHead = iQueueArray;
        }
    --iCount;
    }

template<class T> inline
TBool CFixedQueue<T>::IsEmpty() const
    {
    __TEST_INVARIANT;
    return (iCount==0);
    }

template<class T> inline
TInt CFixedQueue<T>::Count() const
    {
    __TEST_INVARIANT;
    return iCount;
    }

template<class T> inline
TBool CFixedQueue<T>::Push(const T& aObj)
    {
    __TEST_INVARIANT;
    if (iCount < iQueueSize)
        {
        *iQueueTail = aObj;
        if (++iQueueTail == iQueueArray+iQueueSize)
            {
            iQueueTail = iQueueArray;
            }
        ++iCount;
        __TEST_INVARIANT;
        return ETrue;
        }
    return EFalse;
    }

template<class T> inline
void CFixedQueue<T>::Reset()
    {
    __TEST_INVARIANT;
    iQueueTail = iQueueHead;
    iCount = 0;
    __TEST_INVARIANT;
    }

template<class T>
void CFixedQueue<T>::__DbgTestInvariant() const
    {
    if (!iQueueArray)
        {
        // Not initialised
        __ASSERT_DEBUG(iQueueSize==0 && iCount==0, Panic(EPanicInvariant));
        __ASSERT_DEBUG(!iQueueHead && !iQueueTail, Panic(EPanicInvariant));
        }
    else
        {
        // Initialised
        __ASSERT_DEBUG(iQueueSize >= 1, Panic(EPanicInvariant));
        __ASSERT_DEBUG(iCount >= 0 && iCount <= iQueueSize, Panic(EPanicInvariant));
        __ASSERT_DEBUG((iQueueHead >= iQueueArray) && (iQueueHead < iQueueArray+iQueueSize), 
            Panic(EPanicInvariant));
        __ASSERT_DEBUG((iQueueTail >= iQueueArray) && (iQueueTail < iQueueArray+iQueueSize), 
            Panic(EPanicInvariant));
        }
    }


#endif // __CFixedQueue_H__

// End of File
