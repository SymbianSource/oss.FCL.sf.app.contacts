/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
*    Abstract Base interface for other Extension Manager interfaces.
*
*/


#ifndef __MXSPBASE_H__
#define __MXSPBASE_H__


// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * Base interface for other Extension Manager interfaces.
 */
class MxSPBase
    {
    public:
        /**
         * Pushes this object on the cleanup stack.
         */
        void PushL();

    protected:
        /**
         * Destructor.
         */
        virtual ~MxSPBase() =0;

    private:
        /**
         * Releases the given object
         *
         * @param aObj object to be released
         */
        friend void Release( MxSPBase* aObj ); // CSI: 36 #
        
        /**
         * Releases the given object
         *
         * @param aObj object to be released
         */        
        static void CleanupRelease( TAny* aObj );

        /**
         * Implement to release this object and any resources it owns.
         */
        virtual void DoRelease() =0;
    };

inline void MxSPBase::PushL()
    {
    CleanupStack::PushL(TCleanupItem(CleanupRelease,this));
    }

inline MxSPBase::~MxSPBase()
    {
    }

/**
 * Releases aObj.
 */
inline void Release(MxSPBase* aObj)
    {
    if (aObj)
        {
        aObj->DoRelease();
        }
    }

inline void MxSPBase::CleanupRelease(TAny* aObj)
    {
    Release(static_cast<MxSPBase*>(aObj));
    }

#endif // __MXSPBASE_H__

// End of File
