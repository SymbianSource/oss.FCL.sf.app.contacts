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
* Description:  A SIM contact view subsession.
*
*/



// INCLUDE FILES
#include "RVPbkSimCntView.h"

// VPbkSimServer
#include <VPbkSimServerOpCodes.h>
#include <VPbkSimStoreTemplateFunctions.h>

// VPbkSimStoreImpl
#include <RVPbkStreamedIntArray.h>
#include <CVPbkSimFieldTypeFilter.h>
#include <RVPbkSimStore.h>

// VPbkEngUtils
#include <VPbkIPCPackage.h>

// System includes
#include <s32mem.h>

// CONSTANTS

// Initial size for the buffer used to retrieve a view contact
const TInt KInitialReadBufSize = 128;
// Maximum size for the buffer used to retrieve a view contact
const TInt KMaxReadBufSize = 4 * KInitialReadBufSize;
// Double factor
const TInt KTwice = 2;
// Max attempts to get sort order.
const TInt KMaxSortOrderAttempts = 5;

// ============================= LOCAL FUNCTIONS ============================

namespace {

// --------------------------------------------------------------------------
// ExternalizeSortOrderLC
// --------------------------------------------------------------------------
//
HBufC8* ExternalizeSortOrderLC( const RVPbkSimFieldTypeArray& aSortOrder )
    {
    TInt size = aSortOrder.ExternalizedSize();
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 ptr( buf->Des() );
    RDesWriteStream stream( ptr );
    CleanupClosePushL( stream );
    stream << aSortOrder;
    ptr.SetLength( size );
    CleanupStack::PopAndDestroy(); // stream
    return buf;
    }

// --------------------------------------------------------------------------
// CountBufferSize
// --------------------------------------------------------------------------
// 
TInt CountBufferSize( const MDesCArray& aFindStrings )
    {
    TInt size( sizeof(TInt) );
    const TInt count( aFindStrings.MdcaCount() );
    for ( TInt i(0); i < count; ++i )
        {
        size += sizeof(TInt);
        size += aFindStrings.MdcaPoint( i ).Size();
        }
    return size;        
    }
    
// --------------------------------------------------------------------------
// ExternalizeFindStringsL
// --------------------------------------------------------------------------
//    
void ExternalizeFindStringsL( 
        HBufC8*& aBuffer,
        const MDesCArray& aFindStrings )
    {
    // Count needed buffer size
    const TInt size( CountBufferSize( aFindStrings ) );
    
    // check if we have buffer
    if ( aBuffer )
        {
        // check if the buffer is big enough
        TPtr8 ptrBuf( aBuffer->Des() );
        if ( ptrBuf.MaxSize() < size  )
            {
            delete aBuffer;
            aBuffer = NULL;
            aBuffer = HBufC8::NewL( size * KTwice );
            }        
        }
    else
        {
        aBuffer = HBufC8::NewL( size * KTwice );
        }
    
    TPtr8 ptr( aBuffer->Des() );
    RDesWriteStream stream( ptr );
    CleanupClosePushL( stream );
    
    const TInt count( aFindStrings.MdcaCount() );    
    stream.WriteInt16L( count );
    for( TInt i(0); i < count; ++i )
        {
        stream.WriteInt16L( aFindStrings.MdcaPoint( i ).Length() );
        stream.WriteL( aFindStrings.MdcaPoint( i ) );
        }
    
    CleanupStack::PopAndDestroy(); // stream    
    }
}

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// RVPbkSimCntView::RVPbkSimCntView
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
EXPORT_C RVPbkSimCntView::RVPbkSimCntView() :
        iViewEvent( NULL, 0 ),
        iContactPtr( NULL, 0 ),
        iMatchingBufferSize( NULL, 0 )
    {
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::OpenL
// --------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimCntView::OpenL( RVPbkSimStore& aStore, 
        const RVPbkSimFieldTypeArray& aSortOrder, 
        TVPbkSimViewConstructionPolicy aConstructionPolicy, 
        const TDesC& aViewName,
        CVPbkSimFieldTypeFilter* aFilter )
    {
    if ( !aStore.Handle() )
        {
        User::Leave( KErrArgument );
        }

    if ( !iContactBuf )
        {
        iContactBuf = HBufC8::NewL( KInitialReadBufSize );
        iContactPtr.Set( iContactBuf->Des() );
        }

    HBufC8* sortOrderbuf = ExternalizeSortOrderLC( aSortOrder );
    HBufC8* filterBuf = NULL;
    if ( aFilter )
        {
        filterBuf = aFilter->ExternalizeLC();
        }
    else
        {
        CleanupStack::PushL( filterBuf );
        }

    HBufC8* ipcPackage =
        SerializeParametersL( &aViewName, filterBuf );
	CleanupStack::PushL( ipcPackage );

    User::LeaveIfError( CreateSubSession( aStore, EVPbkSimSrvOpenView, 
        TIpcArgs( sortOrderbuf, ipcPackage, aConstructionPolicy ) ) );
    //ipcPackage,filterBuf,sortOrderbuf
    CleanupStack::PopAndDestroy( 3, sortOrderbuf );
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::Close
// --------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimCntView::Close()
    {
    delete iMatchingBuffer;
    iMatchingBuffer = NULL;
    delete iContactBuf;
    iContactBuf = NULL;
    CloseSubSession( EVPbkSimSrvCloseView );
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::CancelAsyncRequest
// --------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimCntView::CancelAsyncRequest( TInt aReqToCancel )
    {
    TPckg<TInt> opCode( aReqToCancel );
    SendReceive( EVPbkSimSrvCancelAsyncRequest, TIpcArgs( &opCode ) );
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::ListenToViewEvents
// --------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimCntView::ListenToViewEvents( TRequestStatus& aStatus,
    TVPbkSimContactEventData& aEvent )
    {
    TPckg<TVPbkSimContactEventData> pckg( aEvent );
    iViewEvent.Set( pckg );
    SendReceive( EVPbkSimSrvViewEventNotification, TIpcArgs( &iViewEvent ), 
        aStatus );
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::CountL
// --------------------------------------------------------------------------
//
EXPORT_C TInt RVPbkSimCntView::CountL() const
    {
    TInt count = KErrNotFound;
    TPckg<TInt> pckg( count );
    User::LeaveIfError( 
        SendReceive( EVPbkSimSrvViewCount, TIpcArgs( &pckg ) ) );
    return count;
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::ContactAtL
// --------------------------------------------------------------------------
//
EXPORT_C TPtr8 RVPbkSimCntView::ContactAtL( TInt aIndex )
    {
    TInt result = KErrNone;
    do
        {
        result = SendReceive( EVPbkSimSrvGetViewContact, 
            TIpcArgs( aIndex, &iContactPtr ) );
        if ( result == KErrNone )
            {
            return iContactPtr;
            }
        else if ( result == KErrOverflow )
            {
            EnlargeBufferOrLeaveL();
            }
        else
            {
            User::Leave( result );
            }
        } while ( result == KErrOverflow );
        
    return iContactPtr;
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::ChangeSortOrderL
// --------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimCntView::ChangeSortOrderL( 
    const RVPbkSimFieldTypeArray& aSortOrder )
    {
    HBufC8* buf = ExternalizeSortOrderLC( aSortOrder );
    User::LeaveIfError( SendReceive( EVPbkSimSrvChangeViewSortOrder, 
        TIpcArgs( buf ) ) );
    CleanupStack::PopAndDestroy( buf );
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::FindViewIndexL
// --------------------------------------------------------------------------
//
EXPORT_C TInt RVPbkSimCntView::FindViewIndexL( TInt aSimIndex )
    {
    TInt viewIndex = KErrNotFound;
    TPckg<TInt> pckg( viewIndex );
    User::LeaveIfError( SendReceive( EVPbkSimSrvFindViewIndex, 
        TIpcArgs( aSimIndex, &pckg ) ) );
    return viewIndex;
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::ContactMatchingPrefixL
// --------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimCntView::ContactMatchingPrefixL(
        const MDesCArray& aFindStrings,
        TInt& aResultBufferSize,
        TRequestStatus& aStatus )
    {
    TPckg<TInt> resultSize( aResultBufferSize );
    iMatchingBufferSize.Set( resultSize );     
        
    ExternalizeFindStringsL( iMatchingBuffer, aFindStrings );
    SendReceive( EVPbkSimSrvContactMatchingPrefix, 
                 TIpcArgs( iMatchingBuffer, &iMatchingBufferSize ), 
                 aStatus ) ;
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::ContactMatchingResultL
// --------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimCntView::ContactMatchingResultL( 
        TPtr8& aSimMatchResultBufPtr )
    {
    User::LeaveIfError( SendReceive( EVPbkSimSrvContactMathingResult, 
        TIpcArgs( &aSimMatchResultBufPtr ) ) );
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::SortOrderL
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* RVPbkSimCntView::SortOrderL() const
    {
    // Use loop because there is too requests and after getting the
    // size of the needed buffer it can be that some other client
    // changes the sort order before our second request.
    HBufC8* sortOrder = NULL;
    TInt res = KErrOverflow;
    TInt attempts = 0;
    while ( res == KErrOverflow && attempts < KMaxSortOrderAttempts )
        {
        delete sortOrder;
        // Get first the size
        TInt size = 0;
        TPckg<TInt> pckg( size );
        User::LeaveIfError( SendReceive( EVPbkSimSrvViewSortOrderSize, 
                TIpcArgs( &pckg ) ) );
        // Create a buffer for sort order.
        sortOrder = HBufC8::NewL( size );
        TPtr8 ptr( sortOrder->Des() );
        // Get sort order
        res = SendReceive( EVPbkSimSrvViewSortOrder, TIpcArgs( &ptr ) );
        ++attempts;
        }
        
    User::LeaveIfError( res );
    
    return sortOrder;
    }
    
// --------------------------------------------------------------------------
// RVPbkSimCntView::EnlargeBufferOrLeaveL
// --------------------------------------------------------------------------
//
void RVPbkSimCntView::EnlargeBufferOrLeaveL()
    {
    TInt newLength = 2 * iContactPtr.MaxLength();
    if ( newLength > KMaxReadBufSize )
        {
        User::Leave( KErrOverflow );
        }

    VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iContactBuf, iContactPtr, 
        newLength );
    }

// --------------------------------------------------------------------------
// RVPbkSimCntView::SerializeParametersL
// --------------------------------------------------------------------------
//
inline HBufC8* RVPbkSimCntView::SerializeParametersL(
        const TDesC* aViewName, const HBufC8* aFilterBuffer ) const
    {
    TInt packageLength = 0;
	packageLength += VPbkEngUtils::VPbkIPCPackage::CountPackageSize
        ( aViewName );
	packageLength += VPbkEngUtils::VPbkIPCPackage::CountPackageSize
        ( aFilterBuffer );

    HBufC8* ipcPackage = HBufC8::NewL( packageLength );
    TPtr8 bufferPtr( ipcPackage->Des() );
    RDesWriteStream writeStream( bufferPtr  );
    writeStream.PushL();

    // Important to externalize everything, even the NULL pointers
    VPbkEngUtils::VPbkIPCPackage::ExternalizeL( aViewName, writeStream );
    VPbkEngUtils::VPbkIPCPackage::ExternalizeL( aFilterBuffer, writeStream );

    writeStream.CommitL();
    
    CleanupStack::PopAndDestroy( &writeStream );

    return ipcPackage;
    }



//  End of File  
