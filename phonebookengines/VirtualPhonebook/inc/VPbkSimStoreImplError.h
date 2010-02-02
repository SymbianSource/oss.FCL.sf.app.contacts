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



#ifndef VPBKSIMSTOREIMPLERROR_H
#define VPBKSIMSTOREIMPLERROR_H

#ifdef _DEBUG

//  INCLUDES
#include <e32std.h>
#include "VPbkSimStoreImplError.h"

/**
* VPbkSimStore internal error codes and functions for
* debug code
*/
namespace VPbkSimStoreImpl
    {
    /// Panic category for Virtual Phonebook engine
    _LIT( KPanicCategory, "VPbkSimStoreImpl" );

    /**
    * Panic codes for the VPbkSimStore
    */
    enum TPanicCode
        {
        EInvalidVPbkToETelTypeConversion = 1,
        EReadingFromETelFailed = 2,
        EInvalidSimIndex = 3,
        ECContactArrayAddOrReplaceL_NullSimContact = 4,
        ESimStoreNotSupportedYet = 5,
        EPreCond_CMultipleReadCmd_EnlargeBufferL = 6,
        EInvalidSortedIndex = 7,
        EMemoryRunOutInSortKeyArrayAppend = 8,
        EViewArrayNotUpToDate = 9,
        EZeroFieldCount = 10,
        ECommandObserverAlreadySetInETelNotification = 11,
        EInvalidETelContactForFieldCount = 12,
        EPreCond_CMultipleReadCmd_IsBufferSizeInSane = 13,
        EPreCondCCompositeCmdBaseAddSubCommandL = 14,
        EPreCondCSequentialCompositeCmdExecute = 15,
        EPreCondCParallelCompositeCmdExecute = 16,
        EPreCondCCacheStatusCmdExecute = 17,
        EPreCondCCacheStatusCmdAddObserverL = 18,
        EPreCondCGetInfoCmdExecute = 19,
        EPreCondCGetInfoCmdAddObserverL = 20,
        EPreCondCServiceTableCmdExecute = 21,
        EPreCondCServiceTableCmdAddObserverL = 22,
        EPreCondCMultipleReadCmdAddObserverL = 23,
        EPreCondCMultipleReadCmdExecute = 24,
        EPreCondCCacheReadyNotificationCmdExecute = 25,
        EPreCondCCacheReadyNotificationCmdAddObserverL = 26,
        EPreCondCSingleReadBaseExecute = 27,
        EPreCondCSingleReadBaseAddObserverL = 28,
        EPreCondCGet3GPBInfoCmdExecute = 29,
        EPreCondCGet3GPBInfoCmdAddObserverL = 30
        // Check E_FREE_PANIC_CODEs first before creating a new one
        };

    /**
     * Called if an unrecoverable error is detected.
     * Stops the thread with User::Panic.
     */
    inline void Panic( TPanicCode aPanicCode );
    }

inline void VPbkSimStoreImpl::Panic( TPanicCode aPanicCode )
    {
    User::Panic( KPanicCategory, aPanicCode );
    }

#endif // _DEBUG

#endif      // VPBKSIMSTOREIMPLERROR_H
            
// End of File
