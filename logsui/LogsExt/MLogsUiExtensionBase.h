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
*    Abstract interface for creating logs extensions.
*
*
*/


#ifndef __MLogsUiExtensionBase_H
#define __MLogsUiExtensionBase_H


// INCLUDES
#include <e32base.h>


// CLASS DECLARATION

/**
 * @internal This interface is internal to Logs.
 *
 * Base interface for Logs UI extensions.
 */
class MLogsUiExtensionBase
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
        virtual ~MLogsUiExtensionBase() =0;

    private:
        friend void Release(MLogsUiExtensionBase*);
        static void CleanupRelease(TAny* aObj);

        /**
         * Implement to release this object and any resources it owns.
         */
        virtual void DoRelease() =0;
    };

inline void MLogsUiExtensionBase::PushL()
    { 
    CleanupStack::PushL(TCleanupItem(CleanupRelease,this)); 
    }

inline MLogsUiExtensionBase::~MLogsUiExtensionBase() 
    {
    }

/**
 * Releases aObj.
 */
inline void Release(MLogsUiExtensionBase* aObj)
    {
    if (aObj)
        {
        aObj->DoRelease();
        }
    }

inline void MLogsUiExtensionBase::CleanupRelease(TAny* aObj)
    { 
    Release(static_cast<MLogsUiExtensionBase*>(aObj)); 
    }

#endif


// End of File
