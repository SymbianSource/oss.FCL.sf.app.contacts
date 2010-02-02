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
* Description:  A command that queries (U)SIM service table and checks
*                if the store is supported. Updates the data in CVPbkPhone
*                so this command is needs to run only once.
*
*/



// INCLUDE FILES
#include "CServiceTableCmd.h"

#include <VPbkDebug.h>
#include <etelmm.h>
#include "CPhone.h"
#include "VPbkSimStoreImplError.h"

namespace VPbkSimStoreImpl {

// ============================= LOCAL FUNCTIONS ===============================

namespace {

#ifdef VPBK_ENABLE_DEBUG_PRINT

void PrintETelServiceTable( RMobilePhone::TMobilePhoneServiceTableV1& aTable )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: ETel iServices1To8: %b"), 
        aTable.iServices1To8 );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: ETel iServices9To16: %b"), 
        aTable.iServices9To16 );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: ETel iServices17To24: %b"), 
        aTable.iServices17To24 );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: ETel iServices25To32: %b"), 
        aTable.iServices25To32 );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: ETel iServices33To40: %b"), 
        aTable.iServices33To40 );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: ETel iServices41To48: %b"), 
        aTable.iServices41To48 );
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: ETel iServices49To56: %b"), 
        aTable.iServices49To56 );
    }

#endif // VPBK_ENABLE_DEBUG_PRINT


// -----------------------------------------------------------------------------
// ConvertUST. 
// Converts USIM service table
// -----------------------------------------------------------------------------
//
inline void ConvertUST( RMobilePhone::TMobilePhoneServiceTableV1& aTable,
    TUint32& aServiceTable )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST"));

#ifdef VPBK_ENABLE_DEBUG_PRINT  
    PrintETelServiceTable( aTable );
#endif // VPBK_ENABLE_DEBUG_PRINT
    
    aServiceTable = 0;
    // ADN is always supported
    aServiceTable |= MVPbkSimPhone::KVPbkCapsADNSupported;

    if (  aTable.iServices1To8 & RMobilePhone::KUstLocalPhBk )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST, local Pbk supported"));
        }
        
    if ( aTable.iServices1To8 & RMobilePhone::KUstSDN )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST, SDN supported"));
        aServiceTable |= MVPbkSimPhone::KVPbkCapsSDNSupported;
        }
    else
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST, SDN not supported"));
        aServiceTable &= ~MVPbkSimPhone::KVPbkCapsSDNSupported;
        }
        
    if ( aTable.iServices1To8 & RMobilePhone::KUstFDN )    
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST, FDN supported"));
        aServiceTable |= MVPbkSimPhone::KVPbkCapsFDNSupported;
        }
    else
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST, FDN not supported"));
        aServiceTable &= ~MVPbkSimPhone::KVPbkCapsFDNSupported;
        }
        
    if( aTable.iServices17To24 & RMobilePhone::KUstMSISDN )    
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST, ON supported"));
        aServiceTable |= MVPbkSimPhone::KVPbkCapsONSupported;
        }
    else
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertUST, ON not supported"));
        aServiceTable &= ~MVPbkSimPhone::KVPbkCapsONSupported;
        }
    }

// -----------------------------------------------------------------------------
// ConvertSST. 
// Converts SIM service table
// -----------------------------------------------------------------------------
//
inline void ConvertSST( RMobilePhone::TMobilePhoneServiceTableV1& aTable,
    TUint32& aServiceTable )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertSST"));
    
    // ADN is always supported
    aServiceTable |= MVPbkSimPhone::KVPbkCapsADNSupported;

    if ( aTable.iServices17To24 & RMobilePhone::KSstSDN )
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertSST, SDN supported"));
        aServiceTable |= MVPbkSimPhone::KVPbkCapsSDNSupported;
        }
    else
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertSST, SDN not supported"));
        aServiceTable &= ~MVPbkSimPhone::KVPbkCapsSDNSupported;
        }
        
    if ( aTable.iServices1To8 & RMobilePhone::KSstFDN )    
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertSST, FDN supported"));
        aServiceTable |= MVPbkSimPhone::KVPbkCapsFDNSupported;
        }
    else
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertSST, FDN not supported"));
        aServiceTable &= ~MVPbkSimPhone::KVPbkCapsFDNSupported;
        }
        
    if( aTable.iServices9To16 & RMobilePhone::KSstMSISDN )    
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertSST, ON supported"));
        aServiceTable |= MVPbkSimPhone::KVPbkCapsONSupported;
        }
    else
        {
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::ConvertSST, ON not supported"));
        aServiceTable &= ~MVPbkSimPhone::KVPbkCapsONSupported;
        }

    }
} // unnamed

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CServiceTableCmd::CServiceTableCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CServiceTableCmd::CServiceTableCmd( CPhone& aPhone, TUint32& aServiceTable )
    : CActive( EPriorityStandard ),
      iPhone( aPhone ),
      iVPbkServiceTable( aServiceTable ),
      iServiceTablePckg( iServiceTable )
    {
    CActiveScheduler::Add( this );
    }
    
// Destructor
CServiceTableCmd::~CServiceTableCmd()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::~CServiceTableCmd"));
    
    CancelCmd();
    }

// -----------------------------------------------------------------------------
// CServiceTableCmd::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CServiceTableCmd* CServiceTableCmd::NewLC( CPhone& aPhone, 
        TUint32& aServiceTable )
    {
    CServiceTableCmd* self = 
        new( ELeave ) CServiceTableCmd( aPhone, aServiceTable );
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CServiceTableCmd::Execute
// -----------------------------------------------------------------------------
//
void CServiceTableCmd::Execute()
    {
    __ASSERT_DEBUG( iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCServiceTableCmdExecute ) );
        
    RMobilePhone::TMobilePhoneServiceTable tableType = 
        RMobilePhone::ESIMServiceTable;
    iUSimSupported = iPhone.USimAccessSupported();
    if ( iUSimSupported )
        {
        tableType = RMobilePhone::EUSIMServiceTable;
        }

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhone::GetServiceTable h%d request,type=%d"), 
        iPhone.ETelPhone().SubSessionHandle(),tableType);
    iPhone.ETelPhone().GetServiceTable( iStatus, tableType, 
        iServiceTablePckg );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CServiceTableCmd::AddObserverL
// -----------------------------------------------------------------------------
//
void CServiceTableCmd::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCServiceTableCmdAddObserverL ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CServiceTableCmd::CancelCmd
// -----------------------------------------------------------------------------
//
void CServiceTableCmd::CancelCmd()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::CancelCmd"));
    
    Cancel();
    }

// -----------------------------------------------------------------------------
// CServiceTableCmd::RunL
// -----------------------------------------------------------------------------
//
void CServiceTableCmd::RunL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhone::GetServiceTable h%d complete %d"),
        iPhone.ETelPhone().SubSessionHandle(),iStatus.Int());

    if ( iStatus == KErrNone )
        {
        if ( iUSimSupported )
            {
            ConvertUST( iServiceTable, iVPbkServiceTable );
            }
        else
            {
            ConvertSST( iServiceTable, iVPbkServiceTable );
            }

        iObserver->CommandDone( *this );
        }
    else
        {
        iObserver->CommandError( *this, iStatus.Int() );
        }       
    }

// -----------------------------------------------------------------------------
// CServiceTableCmd::DoCancel
// -----------------------------------------------------------------------------
//
void CServiceTableCmd::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhone::GetServiceTable h%d Cancel"),
        iPhone.ETelPhone().SubSessionHandle());
    iPhone.ETelPhone().CancelAsyncRequest( EMobilePhoneGetServiceTable );
    }

// -----------------------------------------------------------------------------
// CServiceTableCmd::RunError
// -----------------------------------------------------------------------------
//    
TInt CServiceTableCmd::RunError( TInt aError )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CServiceTableCmd::RunError"));
        
    iObserver->CommandError( *this, aError );    
    
    return KErrNone;    
    }

} // namespace VPbkSimStoreImpl
//  End of File  
