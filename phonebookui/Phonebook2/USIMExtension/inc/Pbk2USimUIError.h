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
* Description:  Contains the panic function and codes for this component
*
*/



#ifndef PBK2USIMUIERROR_H
#define PBK2USIMUIERROR_H

#ifdef _DEBUG

//  INCLUDES
#include <e32std.h>

/**
* VPbkSimStore internal error codes and functions for
* debug code
*/
namespace Pbk2USimUI
    {
    /// Panic category for Virtual Phonebook engine
    _LIT( KPanicCategory, "Pbk2USimUI" );

    /**
    * Panic codes for the VPbkSimStore
    */
    enum TPanicCode
        {
        EPostCond_CPsu2ViewManager_StoreL           = 1,
        EPostCond_OpenAndActivateViewL              = 2 
        };

    /**
     * Called if an unrecoverable error is detected.
     * Stops the thread with User::Panic.
     */
    inline void Panic(TPanicCode aPanicCode);
    }

inline void Pbk2USimUI::Panic(TPanicCode aPanicCode)
    {
    User::Panic( KPanicCategory, aPanicCode );
    }

#endif // _DEBUG

#endif      // PBK2USIMUIERROR_H
            
// End of File
