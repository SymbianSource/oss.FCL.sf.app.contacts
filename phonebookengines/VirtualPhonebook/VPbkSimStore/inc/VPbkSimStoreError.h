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



#ifndef VPBKSIMSTOREERROR_H
#define VPBKSIMSTOREERROR_H

//  INCLUDES
#include <e32std.h>

/**
* VPbkSimStore internal error codes and functions for
* debug code
*/
namespace VPbkSimStore
    {
    /// Panic category for Virtual Phonebook engine
    _LIT( KPanicCategory, "VPbkSimStore" );

    /**
    * Panic codes for the VPbkSimStore
    */
    enum TPanicCode
        {
        /// Invalid index was usedERemoteStoreOpenAlreadyCalled
        EStoreAlreadyExists = 1,
        EZeroSortOrder = 2,
        E_FREE_PANIC_CODE = 3,
        ERemoteViewObserverAlreadySet = 4,
        EContactObserverNotSet = 5,
        EUnknownSimViewEvent = 6,
        EUnknownViewEventFromServer = 7,
        EFieldTypeMapForViewContactFieldFailed = 8,
        EZeroCommitContacts = 9,
        ESimFieldTypeNotFound = 10,
        EZeroIndexesInDeleteOperation = 11,
        EPreCond_DeleteContactsOperationNextCycle = 12,
        EStoreNameMappingFailed = 13,
        EPreCond_CContact_ContactEventComplete = 14,
        EPreCond_CContact_ContactEventError = 15,
        EPreCond_CViewContact_ContactEventComplete = 16,
        EPreCond_CViewContact_ContactEventError = 17,
        EPreCond_CRemoteStore_SaveL = 18,
        EPreCond_CRemoteStore_DeleteL = 19,
        EUnknownSimStoreEvent = 20,
        EStoreObserverArrayMalfunction = 21
        };

    /**
     * Called if an unrecoverable error is detected.
     * Stops the thread with User::Panic.
     */
    inline void Panic( TPanicCode aPanicCode );
    }

inline void VPbkSimStore::Panic( TPanicCode aPanicCode )
    {
    User::Panic( KPanicCategory, aPanicCode );
    }

#endif      // VPBKSIMSTOREERROR_H
            
// End of File
