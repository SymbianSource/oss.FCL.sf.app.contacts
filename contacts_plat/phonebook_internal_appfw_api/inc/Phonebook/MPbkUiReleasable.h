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
*       Base interface for Phonebook UI releasable objects.
*
*/



#ifndef __MPbkUiReleasable_H__
#define __MPbkUiReleasable_H__


// INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
 * Base interface for Phonebook UI releasable objects.
 */
class MPbkUiReleasable
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
        virtual ~MPbkUiReleasable() =0;

    private:
        friend void Release(MPbkUiReleasable*);
        static void CleanupRelease(TAny* aObj);

        /**
         * Implement to release this object and any resources it owns.
         */
        virtual void DoRelease() =0;
    };

inline void MPbkUiReleasable::PushL()
    { 
    CleanupStack::PushL(TCleanupItem(CleanupRelease,this)); 
    }

inline MPbkUiReleasable::~MPbkUiReleasable() 
    {
    }

/**
 * Releases aObj.
 */
inline void Release(MPbkUiReleasable* aObj)
    {
    if (aObj)
        {
        aObj->DoRelease();
        }
    }

inline void MPbkUiReleasable::CleanupRelease(TAny* aObj)
    { 
    Release(static_cast<MPbkUiReleasable*>(aObj)); 
    }

#endif // __MPbkUiReleasable_H__

// End of File
