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
* Description:  A phone class that supports SAT refresh
*
*/



// INCLUDE FILES
#include "csatphone.h"

#include "cphone.h"
#include "vpbksimstorecommon.h"
#include "csatrefreshnotifier.h"
#include "vpbksimstoreimplerror.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSatPhone::CSatPhone
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CSatPhone::CSatPhone()
    {
    }

// -----------------------------------------------------------------------------
// CSatPhone::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CSatPhone::ConstructL()
    {
    iSatNotifier = VPbkSimStoreImpl::CSatRefreshNotifier::NewL();
    iPhone = new( ELeave ) VPbkSimStoreImpl::CPhone(*iSatNotifier);
    }

// -----------------------------------------------------------------------------
// CSatPhone::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSatPhone* CSatPhone::NewL()
    {
    CSatPhone* self = new(ELeave) CSatPhone;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CSatPhone::~CSatPhone()
    {
    delete iPhone;
    delete iSatNotifier;
    }

// -----------------------------------------------------------------------------
// CSatPhone::OpenL
// -----------------------------------------------------------------------------
//  
void CSatPhone::OpenL( MVPbkSimPhoneObserver& aObserver )
    {
    iPhone->OpenL( aObserver );
    }

// -----------------------------------------------------------------------------
// CSatPhone::Close
// -----------------------------------------------------------------------------
//
void CSatPhone::Close( MVPbkSimPhoneObserver& aObserver )
    {
    iPhone->Close( aObserver );
    }

// -----------------------------------------------------------------------------
// CSatPhone::USimAccessSupported
// -----------------------------------------------------------------------------
//
TBool CSatPhone::USimAccessSupported() const
    {
    return iPhone->USimAccessSupported();
    }

// -----------------------------------------------------------------------------
// CSatPhone::ServiceTable
// -----------------------------------------------------------------------------
//
TUint32 CSatPhone::ServiceTable() const
    {
    return iPhone->ServiceTable();
    }

// -----------------------------------------------------------------------------
// CSatPhone::FixedDialingStatus
// -----------------------------------------------------------------------------
//
MVPbkSimPhone::TFDNStatus CSatPhone::FixedDialingStatus() const
    {
    return iPhone->FixedDialingStatus();
    }
} // VPbkSimStoreImpl
//  End of File  
