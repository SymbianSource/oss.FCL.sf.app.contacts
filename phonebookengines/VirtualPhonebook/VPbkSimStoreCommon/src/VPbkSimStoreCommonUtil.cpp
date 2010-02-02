/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Utility functions for for all SIM components
*
*/

#include "VPbkSimStoreCommonUtil.h"

#include <PhonebookInternalCRKeys.h>
#include <centralrepository.h>

// 48 is the default from Phonebook UI specification
const TInt KVPbkDefaultSystemMaxPhoneNumberLength = 48;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// VPbkSimStoreCommonUtil::VPbkSimStoreCommonUtil
// ---------------------------------------------------------------------------
//
VPbkSimStoreCommonUtil::VPbkSimStoreCommonUtil()
    {
    }

// ---------------------------------------------------------------------------
// VPbkSimStoreCommonUtil::SystemMaxPhoneNumberLengthL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt VPbkSimStoreCommonUtil::SystemMaxPhoneNumberLengthL()
    {
    TInt systemMaxPhoneNumberLength = KVPbkDefaultSystemMaxPhoneNumberLength;
    CRepository* rep = NULL;
    TRAPD( res, rep = CRepository::NewL( TUid::Uid( 
        KCRUidPhonebookInternalConfig ) ) );
    if ( res == KErrNone )
        {
        CleanupStack::PushL( rep );
        User::LeaveIfError( rep->Get( KPhonebookNumberEditorMaxLengthKey, 
            systemMaxPhoneNumberLength ) );
        CleanupStack::PopAndDestroy( rep );
        }
    else if ( res != KErrNotFound )
        {
        // It's ok if the CenRep was not found -> then we use
        // KDefaultSystemMaxPhoneNumberLength.
        // But this must leave if it was some other error.
        User::Leave( res );
        }
    return systemMaxPhoneNumberLength;
    }
