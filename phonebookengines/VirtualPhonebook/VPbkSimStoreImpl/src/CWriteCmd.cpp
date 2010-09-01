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
* Description:  A command that writes a contact to (U)SIM card using
*                ETel RMobilePhoneBookStore Write
*
*/



// INCLUDE FILES
#include "CWriteCmd.h"

#include <VPbkDebug.h>
#include "CBasicStore.h"
#include "MVPbkSimContactObserver.h"
#include "CETelStoreNotification.h"
#include "VPbkSimStoreImplError.h"

namespace VPbkSimStoreImpl {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWriteCmd::CWriteCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWriteCmd::CWriteCmd( CBasicStore& aStore, TInt& aSimIndex, 
    MVPbkSimContactObserver& aObserver )
    :   CWriteCmdBase( aStore, aSimIndex, aObserver ),
        iStore( aStore )
    {
    }

// -----------------------------------------------------------------------------
// CWriteCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CWriteCmd::ConstructL( const TDesC8& aData )
    {
    BaseConstructL();
    iData = aData.AllocL();
    }

// -----------------------------------------------------------------------------
// CWriteCmd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWriteCmd* CWriteCmd::NewL( CBasicStore& aStore, const TDesC8& aData, 
    TInt& aSimIndex, MVPbkSimContactObserver& aObserver )
    {
    CWriteCmd* self = new( ELeave ) CWriteCmd( aStore, aSimIndex, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aData );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CWriteCmd::~CWriteCmd()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CWriteCmd::Execute
// -----------------------------------------------------------------------------
//
void CWriteCmd::Execute()
    {
    if ( iSimIndex == KVPbkSimStoreFirstFreeIndex )
        {
        // Handle getting first index by ourselves to avoid problems with
        // TSY functionality.
        iSimIndex = iStore.Contacts().FirstEmptySlot();
        }
        
        
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneBookStore::Write h%d request slot=%d"),
        iStore.ETelStore().SubSessionHandle(),iSimIndex);
        
    if ( iSimIndex == KErrNotFound )
        {
        // The cache is full, leave KErrOverflow. In MM ETel the core
        // error would be KErrOverflow and 
        // extended error KErrMMEtelMaxReached in the same situation.
        CompleteCommandAsynchronously( KErrOverflow );
        }
    else
        {
        iStore.ETelStore().Write( iStatus, *iData, iSimIndex );    
        CWriteCmdBase::Activate();
        }
    }  
} // namespace VPbkSimStoreImpl
//  End of File  
