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
* Description:  A command that uses ETel RMobilePhoneStore GetInfo request
*
*/



// INCLUDE FILES
#include "CGetInfoCmd.h"

#include "CStoreBase.h"
#include "VPbkSimStoreImplError.h"

// From VPbkEng
#include <VPbkDebug.h>

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGetInfoCmd::CGetInfoCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CGetInfoCmd::CGetInfoCmd( CStoreBase& aStore ) 
    :   CActive( EPriorityStandard ),
        iStore( aStore )
    {
    CActiveScheduler::Add( this );
    }
    
// Destructor
CGetInfoCmd::~CGetInfoCmd()
    {
    CancelCmd();
    }

// -----------------------------------------------------------------------------
// CGetInfoCmd::NewLC
// -----------------------------------------------------------------------------
//
CGetInfoCmd* CGetInfoCmd::NewLC( CStoreBase& aStore )
    {
    CGetInfoCmd* self = new( ELeave ) CGetInfoCmd( aStore );
    CleanupStack::PushL( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGetInfoCmd::Execute
// -----------------------------------------------------------------------------
//
void CGetInfoCmd::Execute()
    {
    __ASSERT_DEBUG( iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCGetInfoCmdExecute ) );
        
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::GetInfo h%d request"),
        iStore.ETelStoreBase().SubSessionHandle());
    iStore.ETelStoreBase().GetInfo( iStatus, 
        iStore.ETelStoreInfoPackge() );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CGetInfoCmd::AddObserverL
// -----------------------------------------------------------------------------
//
void CGetInfoCmd::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCGetInfoCmdAddObserverL ) );
        
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CGetInfoCmd::CancelCmd
// -----------------------------------------------------------------------------
//
void CGetInfoCmd::CancelCmd()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CGetInfoCmd::RunL
// -----------------------------------------------------------------------------
//
void CGetInfoCmd::RunL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::GetInfo h%d completed %d"),
        iStore.ETelStoreBase().SubSessionHandle(), iStatus.Int() );
    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: TMobilePhoneStoreInfoV1: type %d, total %d,\
        used %d, caps %b, name %S"),
        iStore.ETelStoreInfo().iType,
        iStore.ETelStoreInfo().iTotalEntries,
        iStore.ETelStoreInfo().iUsedEntries,
        iStore.ETelStoreInfo().iCaps,
        &iStore.ETelStoreInfo().iName );

#ifdef VPBK_ENABLE_DEBUG_PRINT
    if ( iStore.ETelStoreInfo().ExtensionId() ==
         RMobilePhoneStore::KETelMobilePhonebookStoreV1 )
        {
        RMobilePhoneBookStore::TMobilePhoneBookInfoV1& info = 
            static_cast<RMobilePhoneBookStore::TMobilePhoneBookInfoV1&>(
                iStore.ETelStoreInfo());
        		
        VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
            "VPbkSimStoreImpl: TMobilePhoneBookInfoV1: iMaxNumLength %d,\
            iMaxTextLength %d, iLocation %d, iChangeCounter %d, iIdentity %S"),
            info.iMaxNumLength,
            info.iMaxTextLength,
            info.iLocation,
            info.iChangeCounter,
            &info.iIdentity );
        }
#endif // VPBK_ENABLE_DEBUG_PRINT

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
// CGetInfoCmd::DoCancel
// -----------------------------------------------------------------------------
//
void CGetInfoCmd::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneStore::GetInfo h%d Cancel"),
        iStore.ETelStoreBase().SubSessionHandle());
    iStore.ETelStoreBase().CancelAsyncRequest( EMobilePhoneStoreGetInfo );
    }

} // namespace VPbkSimStoreImpl
//  End of File  
