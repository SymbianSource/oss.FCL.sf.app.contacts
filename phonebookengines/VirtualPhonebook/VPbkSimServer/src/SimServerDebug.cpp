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
* Description:  A header for debug helpers
*
*/


//  INCLUDES
#include "SimServerDebug.h"

#include "SimServerErrors.h"
#include <VPbkSimServerOpCodes.h>

#if defined(_DEBUG) || defined(VPBK_ENABLE_DEBUG_PRINT)

namespace VPbkSimServer {
    
// CONSTANTS
_LIT( KVPbkSimSrvCancelAsyncRequest, "EVPbkSimSrvCancelAsyncRequest" );
_LIT( KVPbkSimSrvOpenStore, "EVPbkSimSrvOpenStore" );
_LIT( KVPbkSimSrvCloseStore, "EVPbkSimSrvCloseStore" );
_LIT( KVPbkSimSrvStoreEventNotification, "EVPbkSimSrvStoreEventNotification" );
_LIT( KVPbkSimSrvGetSizeAndLoadContact, "EVPbkSimSrvGetSizeAndLoadContact" );
_LIT( KVPbkSimSrvGetContact, "EVPbkSimSrvGetContact" );
_LIT( KVPbkSimSrvSaveContact, "EVPbkSimSrvSaveContact" );
_LIT( KVPbkSimSrvDeleteContact, "EVPbkSimSrvDeleteContact" );
_LIT( KVPbkSimSrvGsmStoreProperties, "EVPbkSimSrvGsmStoreProperties" );
_LIT( KVPbkSimSrvUSimStoreProperties, "EVPbkSimSrvUSimStoreProperties" );
_LIT( KVPbkSimSrvMatchPhoneNumber, "EVPbkSimSrvMatchPhoneNumber" );
_LIT( KVPbkSimSrvFind, "EVPbkSimSrvFind" );
_LIT( KVPbkSimSrvOpenView, "EVPbkSimSrvOpenView" );
_LIT( KVPbkSimSrvCloseView, "EVPbkSimSrvCloseView" );
_LIT( KVPbkSimSrvViewEventNotification, "EVPbkSimSrvViewEventNotification" );
_LIT( KVPbkSimSrvViewCount, "EVPbkSimSrvViewCount" );
_LIT( KVPbkSimSrvGetViewContact, "EVPbkSimSrvGetViewContact" );
_LIT( KVPbkSimSrvChangeViewSortOrder, "EVPbkSimSrvChangeViewSortOrder" );
_LIT( KVPbkSimSrvFindViewIndex, "EVPbkSimSrvFindViewIndex" );
_LIT( KVPbkSimSrvOpenPhone, "EVPbkSimSrvOpenPhone" );
_LIT( KVPbkSimSrvClosePhone, "EVPbkSimSrvClosePhone" );
_LIT( KVPbkSimSrvUSimAccessSupported, "EVPbkSimSrvUSimAccessSupported" );
_LIT( KVPbkSimSrvServiceTable, "EVPbkSimSrvServiceTable" );
_LIT( KVPbkSimSrvFixedDiallingStatus, "EVPbkSimSrvFixedDiallingStatus" );
_LIT( KVPbkSimSrvContactMatchingPrefix, "EVPbkSimSrvContactMatchingPrefix" );
_LIT( KVPbkSimSrvContactMathingResult, "EVPbkSimSrvContactMathingResult" );
_LIT( KVPbkSimSrvViewSortOrderSize, "EVPbkSimSrvViewSortOrderSize" );
_LIT( KVPbkSimSrvViewSortOrder, "EVPbkSimSrvViewSortOrder" );

/**
 * Returns the message id text for the opCode
 */
const TDesC& MessageInTextFormat( TInt aOpCode )
    {
    switch ( aOpCode )
        {
        case EVPbkSimSrvCancelAsyncRequest:
            {
            return KVPbkSimSrvCancelAsyncRequest;
            }
        case EVPbkSimSrvOpenStore:
            {
            return KVPbkSimSrvOpenStore;
            }
        case EVPbkSimSrvCloseStore:
            {
            return KVPbkSimSrvCloseStore;
            }
        case EVPbkSimSrvStoreEventNotification:
            {
            return KVPbkSimSrvStoreEventNotification;
            }
        case EVPbkSimSrvGetSizeAndLoadContact:
            {
            return KVPbkSimSrvGetSizeAndLoadContact;
            }
        case EVPbkSimSrvGetContact:
            {
            return KVPbkSimSrvGetContact;
            }
        case EVPbkSimSrvSaveContact:
            {
            return KVPbkSimSrvSaveContact;
            }
        case EVPbkSimSrvDeleteContact:
            {
            return KVPbkSimSrvDeleteContact;
            }
        case EVPbkSimSrvGsmStoreProperties:
            {
            return KVPbkSimSrvGsmStoreProperties;
            }
        case EVPbkSimSrvUSimStoreProperties:
            {
            return KVPbkSimSrvUSimStoreProperties;
            }
        case EVPbkSimSrvMatchPhoneNumber:
            {
            return KVPbkSimSrvMatchPhoneNumber;
            }
        case EVPbkSimSrvFind:
            {
            return KVPbkSimSrvFind;
            }
        case EVPbkSimSrvOpenView:
            {
            return KVPbkSimSrvOpenView;
            }
        case EVPbkSimSrvCloseView:
            {
            return KVPbkSimSrvCloseView;
            }
        case EVPbkSimSrvViewEventNotification:
            {
            return KVPbkSimSrvViewEventNotification;
            }
        case EVPbkSimSrvViewCount:
            {
            return KVPbkSimSrvViewCount;
            }
        case EVPbkSimSrvGetViewContact:
            {
            return KVPbkSimSrvGetViewContact;
            }
        case EVPbkSimSrvChangeViewSortOrder:
            {
            return KVPbkSimSrvChangeViewSortOrder;
            }
        case EVPbkSimSrvFindViewIndex:
            {
            return KVPbkSimSrvFindViewIndex;
            }
        case EVPbkSimSrvOpenPhone:
            {
            return KVPbkSimSrvOpenPhone;
            }
        case EVPbkSimSrvClosePhone:
            {
            return KVPbkSimSrvClosePhone;
            }
        case EVPbkSimSrvUSimAccessSupported:
            {
            return KVPbkSimSrvUSimAccessSupported;
            }
        case EVPbkSimSrvServiceTable:
            {
            return KVPbkSimSrvServiceTable;
            }
        case EVPbkSimSrvFixedDiallingStatus:
            {
            return KVPbkSimSrvFixedDiallingStatus;
            }                        
        case EVPbkSimSrvContactMatchingPrefix:
            {
            return KVPbkSimSrvContactMatchingPrefix;
            }
        case EVPbkSimSrvContactMathingResult:
            {
            return KVPbkSimSrvContactMathingResult;
            }
        case EVPbkSimSrvViewSortOrderSize:
            {
            return KVPbkSimSrvViewSortOrderSize;
            }
        case EVPbkSimSrvViewSortOrder:
            {
            return KVPbkSimSrvViewSortOrder;
            }
        default:
            {
            __ASSERT_ALWAYS( EFalse, Panic( EMissingOpCodeDebugText ) );
            return KNullDesC;
            }
        }
    }
} // namespace
#endif // _DEBUG

// End of File
