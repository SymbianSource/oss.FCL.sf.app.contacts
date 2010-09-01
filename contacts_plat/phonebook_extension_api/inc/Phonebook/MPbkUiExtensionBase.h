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
*    Abstract interface for creating phone book extensions.
*
*
*/


#ifndef __MPbkUiExtensionBase_H__
#define __MPbkUiExtensionBase_H__


// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * Base interface for Phonebook UI extensions.
 */
class MPbkUiExtensionBase
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
        virtual ~MPbkUiExtensionBase() =0;

    private:
        friend void Release(MPbkUiExtensionBase*);
        static void CleanupRelease(TAny* aObj);

        /**
         * Implement to release this object and any resources it owns.
         */
        virtual void DoRelease() =0;
    };

inline void MPbkUiExtensionBase::PushL()
    { 
    CleanupStack::PushL(TCleanupItem(CleanupRelease,this)); 
    }

inline MPbkUiExtensionBase::~MPbkUiExtensionBase() 
    {
    }

/**
 * Releases aObj.
 */
inline void Release(MPbkUiExtensionBase* aObj)
    {
    if (aObj)
        {
        aObj->DoRelease();
        }
    }

inline void MPbkUiExtensionBase::CleanupRelease(TAny* aObj)
    { 
    Release(static_cast<MPbkUiExtensionBase*>(aObj)); 
    }

#endif // __MPbkUiExtensionBase_H__

// End of File
