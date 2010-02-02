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
* Description:  A command that queries 3G ADN store information from ETel 
*                custom API
*
*/



// INCLUDE FILES
#include "CGet3GPBInfoCmd.h"

#include "CUsimAdnStore.h"
#include "VPbkSimStoreImplError.h"

// From VPbkEng
#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGet3GPBInfoCmd::CGet3GPBInfoCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGet3GPBInfoCmd::CGet3GPBInfoCmd( CUsimAdnStore& aStore )
    :   CActive( EPriorityStandard ),
        iStore( aStore )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CGet3GPBInfoCmd::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGet3GPBInfoCmd* CGet3GPBInfoCmd::NewLC( CUsimAdnStore& aStore )
    {
    CGet3GPBInfoCmd* self = 
        new( ELeave ) CGet3GPBInfoCmd( aStore );
    CleanupStack::PushL( self );
    return self;
    }

    
// Destructor
CGet3GPBInfoCmd::~CGet3GPBInfoCmd()
    {
    CancelCmd();
    }

// -----------------------------------------------------------------------------
// CGet3GPBInfoCmd::Execute
// -----------------------------------------------------------------------------
//
void CGet3GPBInfoCmd::Execute()
    {
    __ASSERT_DEBUG( iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCGet3GPBInfoCmdExecute ) );
        
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::Get3GPBInfo h%d request"),
        iStore.CustomAPI().SubSessionHandle());
    iStore.CustomAPI().Get3GPBInfo( iStatus, iStore.Get3GPBInfo() );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CGet3GPBInfoCmd::AddObserverL
// -----------------------------------------------------------------------------
//
void CGet3GPBInfoCmd::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCGet3GPBInfoCmdAddObserverL ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CGet3GPBInfoCmd::CancelCmd
// -----------------------------------------------------------------------------
//
void CGet3GPBInfoCmd::CancelCmd()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CGet3GPBInfoCmd::RunL
// -----------------------------------------------------------------------------
//
void CGet3GPBInfoCmd::RunL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::Get3GPBInfo h%d completed %d"),
        iStore.CustomAPI().SubSessionHandle(), iStatus.Int() );
            
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: T3GPBInfo: iMaxLenEmail %d, iMaxLenSne %d,\
        iMaxLenAnr %d, iMaxLenGroupName %d, iMaxNumEmail %d, iMaxNumSne %d,\
        iMaxNumAnr %d, iMaxNumGroupName %d"),
        iStore.Get3GPBInfo().iMaxLenEmail,
        iStore.Get3GPBInfo().iMaxLenSne,
        iStore.Get3GPBInfo().iMaxLenAnr,
        iStore.Get3GPBInfo().iMaxLenGroupName,
        iStore.Get3GPBInfo().iMaxNumEmail,
        iStore.Get3GPBInfo().iMaxNumSne,
        iStore.Get3GPBInfo().iMaxNumAnr,
        iStore.Get3GPBInfo().iMaxNumGroupName );
    
    if ( iStatus.Int() == KErrNone )
        {
        iObserver->CommandDone( *this );
        }
    else
        {
        iObserver->CommandError( *this, iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CGet3GPBInfoCmd::DoCancel
// -----------------------------------------------------------------------------
//
void CGet3GPBInfoCmd::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMmCustomAPI::Get3GPBInfo h%d Cancel"),
        iStore.CustomAPI().SubSessionHandle());
    iStore.CustomAPI().CancelAsyncRequest( EGet3GPBInfoIPC );
    }
} // namespace VPbkSimStoreImpl

//  End of File  
