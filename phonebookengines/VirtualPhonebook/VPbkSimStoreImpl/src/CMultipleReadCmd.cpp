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
* Description:  A class that can read multiple contacts from ETel store
*                Uses ETel RMobilePhoneBookStore Read
*
*/



// INCLUDE FILES
#include "CMultipleReadCmd.h"

#include "CVPbkETelCntConverter.h"
#include "CVPbkSimContactBuf.h"
#include "CVPbkSimCntField.h"
#include "VPbkSimStoreImplError.h"
#include "CBasicStore.h"
#include "CContactArray.h"
#include "VPbkDebug.h"

namespace VPbkSimStoreImpl {

// CONSTANTS

// Define some default size of one contact for buffer creation
const TInt KDefaultSizeForOneContact = 256;
// Define a maximum size for the read buffer. 20 contacts at one request is
// a pretty good base for maximum.
const TInt KMaxSizeForBuffer = 20 * KDefaultSizeForOneContact;

// ============================= LOCAL FUNCTIONS ===============================

namespace {
// -----------------------------------------------------------------------------
// Return the number of indexes
// -----------------------------------------------------------------------------
//
inline TInt AmountOfIndexes( TInt aLastIndex, TInt aFirstIndex )
    {
    return aLastIndex - aFirstIndex + 1;
    }
} // unnamed namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMultipleReadCmd::CMultipleReadCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMultipleReadCmd::CMultipleReadCmd( CBasicStore& aBasicStore, 
        TInt aFirstIndexToRead, TInt aLastIndexToRead )
        :   CActive( EPriorityStandard ),
            iBasicStore( aBasicStore ),
            iFirstIndexToRead( aFirstIndexToRead ),
            iLastIndexToRead( aLastIndexToRead ),
            iIndexToRead( KVPbkSimStoreInvalidETelIndex ),
            iBufferPtr( 0, NULL )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::CMultipleReadCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMultipleReadCmd::CMultipleReadCmd( CBasicStore& aBasicStore, 
        TInt& aIndexToRead )
        :   CActive( EPriorityStandard ),
            iBasicStore( aBasicStore ),
            iFirstIndexToRead( KVPbkSimStoreInvalidETelIndex ),
            iLastIndexToRead( KVPbkSimStoreInvalidETelIndex ),
            iIndexToRead( aIndexToRead ),
            iBufferPtr( 0, NULL )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::ConstructL()
    {
    iCntConverter = CVPbkETelCntConverter::NewL();
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMultipleReadCmd* CMultipleReadCmd::NewLC( CBasicStore& aBasicStore )
    {
    CMultipleReadCmd* self = new( ELeave ) CMultipleReadCmd( aBasicStore, 
        KVPbkSimStoreFirstETelIndex, KVPbkSimStoreInvalidETelIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMultipleReadCmd* CMultipleReadCmd::NewLC( CBasicStore& aBasicStore,    
        TInt aFirstIndexToRead, TInt aLastIndexToRead )
    {
    CMultipleReadCmd* self = new( ELeave ) CMultipleReadCmd( aBasicStore, 
            aFirstIndexToRead, aLastIndexToRead );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMultipleReadCmd* CMultipleReadCmd::NewLC( CBasicStore& aBasicStore, 
        TInt& aIndexToRead )
    {
    CMultipleReadCmd* self = new( ELeave ) CMultipleReadCmd( aBasicStore, 
        aIndexToRead );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// Destructor
CMultipleReadCmd::~CMultipleReadCmd()
    {
    CancelCmd();
    delete iCntConverter;
    delete iBuffer;
    iCntArray.ResetAndDestroy();
    iCntArray.Close();
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::Execute
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::Execute()
    {
    __ASSERT_DEBUG( iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCMultipleReadCmdExecute ) );
        
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CMultipleReadCmd::Execute Request") );        
        
    if ( iIndexToRead != KVPbkSimStoreInvalidETelIndex )
        {
        iFirstIndexToRead = iIndexToRead;
        iLastIndexToRead = iIndexToRead;
        }
    else if ( iLastIndexToRead == KVPbkSimStoreInvalidETelIndex )
        {
        iLastIndexToRead = iBasicStore.ETelStoreInfo().iTotalEntries;
        }

    // Define the size of the buffer. ETel iTotalEntries can not be trusted.
    TInt amountOfIndexes = 
        AmountOfIndexes( iLastIndexToRead, iFirstIndexToRead );
    if ( amountOfIndexes > 0 )
        {
        // Create a buffer according to amount of possible contacts to read.
        // There must be a limit for the size.
        TInt size = Min( KMaxSizeForBuffer, 
            amountOfIndexes * KDefaultSizeForOneContact );
        TRAPD( result, iBuffer = HBufC8::NewL( size ) );
        if ( result != KErrNone )
            {
            DoDummyRequest( result );
            }
        else
            {
            iBufferPtr.Set( iBuffer->Des() );
            DoRequest();
            }
        }
    else
        {
        // There was probably a problem with ETel iTotalEntries.
        // Behave same as iTotalEntries would be zero. Completing with
        // KErrNotFound from ETel means that there were no contacts
        // in given index range.
        DoDummyRequest( KErrNotFound );
        }
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::AddObserverL
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::AddObserverL( MVPbkSimCommandObserver& aObserver )
    {
    __ASSERT_DEBUG( !iObserver,
        VPbkSimStoreImpl::Panic( EPreCondCMultipleReadCmdAddObserverL ) );
    iObserver = &aObserver;
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::CancelCmd
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::CancelCmd()
    {
    CActive::Cancel();
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::RunL
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::RunL()
    {
    TInt result = iStatus.Int();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneBooStore::Read h%d complete %d"),
        iBasicStore.ETelStore().SubSessionHandle(), result);

    switch ( result )
        {
        case KErrNone:
            {
            // Convert from ETel to own contacts
            ConvertContactsL();
            // Check that was the buffer big enough
            if ( WasBufferBigEnough() )
                {
                // Contacts were found -> update contact array
                UpdateContactArrayL();
                }
            // Buffer wasn't big enough -> Check that it's not growing
            // for ever.
            else if ( !IsBufferSizeInSane() )
                {
                // Make buffer bigger and read again
                EnlargeBufferL();
                }
            else
                {
                // Stop trying -> there is something wrong in ETel 
                result = KErrOverflow;
                }
            
            // Stop if error
            if ( result != KErrNone )
                {
                iObserver->CommandError( *this, result );
                }
            // If all the indexes have been read and we are done.
            else if ( AmountOfIndexes( 
                        iLastIndexToRead, iFirstIndexToRead ) <= 0 )
                {
                iObserver->CommandDone( *this );
                }
            // There are still contacts to read
            else
                {
                DoRequest();
                }
            break;
            }
        case KErrNotFound:
            {
            // If nothing found update the array and complete command.
            iBasicStore.Contacts().Delete( iFirstIndexToRead, iLastIndexToRead );
            iObserver->CommandDone( *this );
            break;
            }
        default:
            {
            iObserver->CommandError( *this, result );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::DoCancel
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::DoCancel()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneBooStore::Read h%d Cancel"),
        iBasicStore.ETelStore().SubSessionHandle());
    iBasicStore.ETelStore().CancelAsyncRequest( EMobilePhoneStoreRead );
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::RunError
// -----------------------------------------------------------------------------
//
TInt CMultipleReadCmd::RunError( TInt aError )
    {
    iObserver->CommandError( *this, aError );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CMultipleReadCmd::DoDummyRequest
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::DoDummyRequest( TInt aResult )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CMultipleReadCmd dummy request"));
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, aResult );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::DoRequest
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::DoRequest()
    {
    ResetBuffer();

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: RMobilePhoneBooStore::Read h%d request\
        firstslot=%d,lastslot=%d"),
        iBasicStore.ETelStore().SubSessionHandle(),
        iFirstIndexToRead, iLastIndexToRead);

    iBasicStore.ETelStore().Read( iStatus, iFirstIndexToRead, 
        AmountOfIndexes( iLastIndexToRead, iFirstIndexToRead ), iBufferPtr );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::IsBufferSizeInSane
// -----------------------------------------------------------------------------
//
TBool CMultipleReadCmd::IsBufferSizeInSane()
    {
    __ASSERT_DEBUG( iBuffer, 
        Panic( EPreCond_CMultipleReadCmd_IsBufferSizeInSane ) );
        
    if ( iBuffer->Size() >= KMaxSizeForBuffer )
        {
        return ETrue;
        }
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CMultipleReadCmd::EnlargeBufferL
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::EnlargeBufferL()
    {
    __ASSERT_DEBUG( iBuffer, 
        Panic( EPreCond_CMultipleReadCmd_EnlargeBufferL ) );
        
    const TInt doubleFactor = 2;
    TInt newSize = iBufferPtr.MaxSize() * doubleFactor;
    
    HBufC8* buffer = HBufC8::NewL( newSize );
    delete iBuffer;
    iBuffer = buffer;
    iBufferPtr.Set( iBuffer->Des() );
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::ConvertContactsL
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::ConvertContactsL()
    {    
    // Make sure that there are no old contacts
    iCntArray.ResetAndDestroy();
    
    if ( iBufferPtr.Length() > 0 )
        {
        iCntConverter->ConvertFromETelToVPbkSimContactsL( 
            iCntArray, iBufferPtr, iBasicStore );
        }
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::WasBufferBigEnough
// -----------------------------------------------------------------------------
//
TBool CMultipleReadCmd::WasBufferBigEnough()
    {
    // ConvertContactsL must be called before this. The logic is that if
    // ETel retuned KErrNone from Read request but there are not contacts
    // in iBuffer then the buffer was too small.
    return iCntArray.Count() > 0;
    }
    
// -----------------------------------------------------------------------------
// CMultipleReadCmd::UpdateContactArrayL
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::UpdateContactArrayL()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimStoreImpl: CMultipleReadCmd::UpdateContactArrayL "));
    
    __ASSERT_DEBUG( iCntArray.Count() > 0, Panic( EReadingFromETelFailed ) );

    // Get amount of contacts
    TInt cntCount = iCntArray.Count();
    for ( TInt i = cntCount - 1; i >= 0; --i )
        {
        // Check the biggest sim index that was read
        TInt simIndex = iCntArray[i]->SimIndex();
        if ( simIndex >= iFirstIndexToRead )
            {
            // iFirstIndexToRead must be the first sim index that was not read.
            iFirstIndexToRead = simIndex + 1; 
            }

        // Add contacts to contact array. Ownership changes.
        iBasicStore.Contacts().AddOrReplaceL( iCntArray[i] );
        iCntArray.Remove( i );
        }
    }

// -----------------------------------------------------------------------------
// CMultipleReadCmd::ResetBuffer
// -----------------------------------------------------------------------------
//
void CMultipleReadCmd::ResetBuffer()
    {
    iBufferPtr.FillZ( iBufferPtr.MaxLength() );
    iBufferPtr.Zero();
    }
} // namespace VPbkSimStoreImpl
//  End of File  
