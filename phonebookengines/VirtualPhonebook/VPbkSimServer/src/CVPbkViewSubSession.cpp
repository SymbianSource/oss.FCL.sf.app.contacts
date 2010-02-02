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
* Description:  A sim store subsession class.
*
*/



// INCLUDE FILES
#include "CVPbkViewSubSession.h"

#include "VPbkSimServerCommon.h"
#include <RVPbkStreamedIntArray.h>
#include <MVPbkSimCntStore.h>
#include <MVPbkSimCntView.h>
#include <CVPbkSimContact.h>
#include <CVPbkSimFieldTypeFilter.h>
#include "VPbkSimServerOpCodes.h"
#include "SimServerErrors.h"
#include "SimServerInternal.h"
#include "CVPbkSimServerEventQueue.h"
#include <VPbkIPCPackage.h>
#include <MVPbkSimStoreOperation.h>
#include <s32mem.h> // RDesWriteStream
#include <badesca.h> // CDesCArray

// Debugging headers
#include <VPbkDebug.h>


// CONSTANTS

// This is some sensible limit for the checking the size of the
// sortarray.
const TInt KVPbkViewEventGranularity = 10;
const TInt KVPbkMaxViewEvents = 10 * KVPbkViewEventGranularity;
const TInt KGranularity( 2 );

// ============================= LOCAL FUNCTIONS ============================

namespace {

// --------------------------------------------------------------------------
// CompleteViewNotification
// --------------------------------------------------------------------------
//
void CompleteViewNotification( const RMessage2& aMsg, 
    TVPbkSimContactEventData& aEventData, TInt aResult )
    {    
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "VPbkSimServer: complete view notification event %d data %d opdata %d"),
        aEventData.iEvent,aEventData.iData,aEventData.iOpData );
    TPckg<TVPbkSimContactEventData> pckg( aEventData );
    TInt result = aMsg.Write( KVPbkSlot0, pckg );
    if ( result != KErrNone )
        {
        VPbkSimSrvUtility::CompleteRequest( aMsg, result );
        }
    else
        {
        VPbkSimSrvUtility::CompleteRequest( aMsg, aResult );
        }
    }

// --------------------------------------------------------------------------
// Internalize find strings
// --------------------------------------------------------------------------
//   
MDesCArray* InternalizeFindStringsL( const RMessage2& aMessage )
    {
    CDesCArrayFlat* array = new(ELeave)CDesCArrayFlat( KGranularity );
    CleanupStack::PushL( array );
    
    TInt length = aMessage.GetDesLengthL( KVPbkSlot0 );
    if ( length > 0 )
        {
        HBufC8* data = HBufC8::NewLC( length );
        TPtr8 ptr = data->Des();
        aMessage.ReadL( KVPbkSlot0, ptr );
        
        RDesReadStream readStream( ptr );    
        CleanupClosePushL( readStream );
        
        const TInt count( readStream.ReadInt16L() );
        for ( TInt i(0); i < count; ++i )
            {
            TInt length( readStream.ReadInt16L() );
            HBufC* buffer = HBufC::NewLC( length );            
            TPtr ptrBuffer = buffer->Des();
            readStream.ReadL( ptrBuffer, length );
            array->AppendL( *buffer );
            CleanupStack::PopAndDestroy( buffer );                        
            }
        
        CleanupStack::PopAndDestroy(); // readStream
        CleanupStack::PopAndDestroy( data );
        }
    
    CleanupStack::Pop( array );
    return array;
    }

} // unnamed namespace

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CVPbkViewSubSession::CVPbkViewSubSession
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CVPbkViewSubSession::CVPbkViewSubSession( MVPbkSimCntStore& aStore ) :
        iStore( aStore )
    {
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::~CVPbkViewSubSession
// Destructor.
// --------------------------------------------------------------------------
//
CVPbkViewSubSession::~CVPbkViewSubSession()
    {
    AssertNoActiveAsyncRequests();
    delete iMatchOperation;
    delete iMatchingResults;
    delete iFindStrings;
    delete iEventQueue;
    CloseAndDestroyView();
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CVPbkViewSubSession* CVPbkViewSubSession::NewL( const RMessage2& aMessage,
                                                MVPbkSimCntStore& aStore )
    {
    CVPbkViewSubSession* self = new( ELeave ) CVPbkViewSubSession( aStore );
    CleanupClosePushL( *self );
    self->ConstructL( aMessage );
    CleanupStack::Pop();
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ConstructL
// Symbian 2nd phase constructor can leave.
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ConstructL( const RMessage2& aMessage )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CVPbkViewSubSession::ConstructL begin"));

    iEventQueue = CVPbkSimServerEventQueue::NewL(
        KVPbkViewEventGranularity, KVPbkMaxViewEvents);
        
    RVPbkSimFieldTypeArray sortOrder;
    CleanupClosePushL( sortOrder );
    VPbkSimSrvUtility::ReadFieldTypesL( sortOrder, aMessage, KVPbkSlot0 );

    TVPbkSimViewConstructionPolicy constructionPolicy = 
        static_cast<TVPbkSimViewConstructionPolicy>( aMessage.Int2() );
    
    // Read contact view name and filter from the separate package
    TInt length = aMessage.GetDesLengthL( KVPbkSlot1 );
    if ( length > 0 )
        {
        HBufC8* ipcData = HBufC8::NewLC( length );
        TPtr8 ptr = ipcData->Des();
        aMessage.ReadL( KVPbkSlot1, ptr );
        
        RDesReadStream readStream( ptr );
        readStream.PushL();

        HBufC* viewName = NULL;
        VPbkEngUtils::VPbkIPCPackage::InternalizeL( viewName, readStream );
        CleanupStack::PushL( viewName );

        HBufC8* filterBuffer = NULL;
        VPbkEngUtils::VPbkIPCPackage::InternalizeL
            ( filterBuffer, readStream );

        CVPbkSimFieldTypeFilter* filter = NULL;
        if ( filterBuffer && filterBuffer->Length() > 0 )
            {
            CleanupStack::PushL( filterBuffer );
			filter = new ( ELeave ) CVPbkSimFieldTypeFilter;
            CleanupStack::PushL( filter );

            // Internalize from stream
            TPtr8 filterPtr = filterBuffer->Des();
            filter->InternalizeL( filterPtr );

            CleanupStack::Pop( filter );
            CleanupStack::PopAndDestroy(); // filterBuffer
            }
       
        // Create a view (filter is taken)
        iView = iStore.CreateViewL(
        	sortOrder, constructionPolicy, *viewName, filter );
        
        CleanupStack::PopAndDestroy( viewName );
        CleanupStack::PopAndDestroy( &readStream );
        CleanupStack::PopAndDestroy( ipcData );
        }
    else
        {
        // Create an unamed non-filtered view
        iView = iStore.CreateViewL(
        	sortOrder, constructionPolicy, KNullDesC, NULL );
        }

    CleanupStack::PopAndDestroy(); // sortOrder
    iView->OpenL( *this );

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING(
        "CVPbkViewSubSession::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ServiceL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case EVPbkSimSrvViewEventNotification:
            {
            SaveViewEventNotification( aMessage );
            break;
            }
        case EVPbkSimSrvViewCount:
            {
            ReturnContactCountL( aMessage );
            break;
            }
        case EVPbkSimSrvGetViewContact:
            {   
            GetViewContactL( aMessage );
            break;
            }
        case EVPbkSimSrvChangeViewSortOrder:
            {
            ChangeSortOrderL( aMessage );
            break;
            }
        case EVPbkSimSrvFindViewIndex:
            {
            FindViewIndexL( aMessage );
            break;
            }
        case EVPbkSimSrvContactMatchingPrefix:
            {
            ContactMatchingPrefixL( aMessage );
            break;
            }
        case EVPbkSimSrvContactMathingResult:
            {
            ContactMatchingResultL( aMessage );
            break;
            }
        case EVPbkSimSrvViewSortOrderSize:
            {
            SortOrderSizeL( aMessage );
            break;
            }
        case EVPbkSimSrvViewSortOrder:
            {
            SortOrderL( aMessage );
            break;
            }
        default:
            {
            VPbkSimServer::PanicClient( aMessage, 
                VPbkSimServer::EInvalidOpCode );
            break;
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::CancelRequestL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::CancelRequestL( const RMessage2& aMessage )
    {
    TInt opCode = KErrUnknown;
    TPckg<TInt> pckg( opCode );
    aMessage.ReadL( KVPbkSlot0, pckg );

    switch ( opCode )
        {        
        case EVPbkSimSrvViewEventNotification:
            {
            CancelViewEventNotification();
            break;
            }
        case EVPbkSimSrvContactMatchingPrefix:
            {
            CancelContactMatching();
            break;
            }
        default:
            {
            VPbkSimServer::PanicClient( aMessage, 
                VPbkSimServer::EInvalidCancelOpCode );
            break;
            }
        }
    VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ViewReady
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ViewReady( MVPbkSimCntView& /*aView*/ )
    {
    TVPbkSimContactEventData event;
    event.iEvent = EVPbkSimViewOpen;
    event.iData = 0;
    event.iOpData = 0;
    HandleEvent( event );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ViewError
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ViewError( MVPbkSimCntView& /*aView*/, TInt aError )
    {
    // If the view has an error then this subsession must not use it anymore
    
    // Complete the match request if it's active.
    CompleteAsyncContactMatching( aError );
    
    // Close the view
    CloseAndDestroyView();
    
    // Complete the view notification message.
    TVPbkSimContactEventData event;
    event.iEvent = EVPbkSimViewError;
    event.iData = aError;
    event.iOpData = 0;
    HandleEvent( event );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ViewNotAvailable
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ViewNotAvailable( MVPbkSimCntView& /*aView*/ )
    {
    // Complete the match request if it's active.
    CompleteAsyncContactMatching( KErrCancel );
    
    TVPbkSimContactEventData event;
    event.iEvent = EVPbkSimViewNotAvailable;
    event.iData = 0;
    event.iOpData = 0;
    HandleEvent( event );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ViewContactEvent
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ViewContactEvent( TEvent aEvent, TInt aIndex, 
    TInt aSimIndex )
    {
    TVPbkSimEvent viewEvent = EVPbkSimUnknown; 
    switch ( aEvent )
        {
        case EContactDeleted:
            {
            viewEvent = EVPbkSimContactDeleted;
            break;
            }
        case EContactAdded:
            {
            viewEvent = EVPbkSimContactAdded;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    TVPbkSimContactEventData event;
    event.iEvent = viewEvent;
    event.iData = aIndex;
    event.iOpData = (TInt16) aSimIndex;
    HandleEvent( event );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ViewFindCompleted
// --------------------------------------------------------------------------
//    
void CVPbkViewSubSession::ViewFindCompleted( 
        MVPbkSimCntView& aSimCntView,
        const RVPbkStreamedIntArray& aSimIndexArray )
    {        
    TRAPD( result, ViewFindCompletedL( aSimCntView, aSimIndexArray ) );
    CompleteAsyncContactMatching( result );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ViewFindError
// --------------------------------------------------------------------------
//    
void CVPbkViewSubSession::ViewFindError( 
        MVPbkSimCntView& /*aSimCntView*/, 
        TInt aError )    
    {
    // There was an error during match request. Complete the match request
    // with the error. View can still be used.
    CompleteAsyncContactMatching( aError );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::SaveViewEventNotification
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::SaveViewEventNotification( 
    const RMessage2& aMessage )
    {
    // First check if the notification is already set
    if ( iNotificationMsg.Handle() )
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrAlreadyExists );
        }
    // Second: check if there is an error waiting
    else if ( iErrorEvent.iEvent == EVPbkSimViewError )
        {
        CompleteViewNotification( aMessage, iErrorEvent, iErrorEvent.iData );
        ResetErrorEvent();
        }
    // Third: check if there is an event waiting
    else if ( !iEventQueue->IsEmpty() )
        {
        CompleteViewNotification( aMessage, iEventQueue->FirstEvent(), 
            KErrNone );
        iEventQueue->Pop();
        }   
    // Save notification message
    else
        {
        iNotificationMsg = aMessage;
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ReturnContactCount
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ReturnContactCountL( const RMessage2& aMessage )
    {
    if ( iView )
        {
        TInt count = iView->CountL();
        TPckg<TInt> pckg( count );
        aMessage.WriteL( KVPbkSlot0, pckg );
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );    
        }
    else
        {
        // Client has ignored an error and still using the view.
        VPbkSimServer::PanicClient( aMessage,
            VPbkSimServer::ECVPbkViewSubSession_ReturnContactCountL );
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::GetViewContactL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::GetViewContactL( const RMessage2& aMessage )
    {
    if ( iView )
        {
        TInt viewIndex = aMessage.Int0();
        TInt clientBufLength = aMessage.GetDesMaxLengthL( KVPbkSlot1 );

        MVPbkSimContact& cnt = iView->ContactAtL( viewIndex );
        const TDesC8& etelCnt = cnt.ETelContactL();
        if ( clientBufLength < etelCnt.Length() )
            {
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrOverflow );
            }
        else
            {
            aMessage.WriteL( KVPbkSlot1, etelCnt );
            VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
            }
        }
    else
        {
        // Client has ignored an error and still using the view.
        VPbkSimServer::PanicClient( aMessage,
            VPbkSimServer::ECVPbkViewSubSession_GetViewContactL );
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ChangeSortOrderL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ChangeSortOrderL( const RMessage2& aMessage )
    {
    if ( iView )
        {
        RVPbkSimFieldTypeArray sortOrder;
        CleanupClosePushL( sortOrder );
        VPbkSimSrvUtility::ReadFieldTypesL( sortOrder, aMessage, KVPbkSlot0 );
        iView->ChangeSortOrderL( sortOrder );
        CleanupStack::PopAndDestroy(); //sortOrder 
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
        }
    else
        {
        // Client has ignored an error and still using the view.
        VPbkSimServer::PanicClient( aMessage,
            VPbkSimServer::ECVPbkViewSubSession_GetViewContactL );
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::FindViewIndexL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::FindViewIndexL( const RMessage2& aMessage )
    {
    if ( iView )
        {
        TInt viewIndex = iView->MapSimIndexToViewIndexL( aMessage.Int0() );
        TPckg<TInt> pckg( viewIndex );
        aMessage.WriteL( KVPbkSlot1, pckg );
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
        }
    else
        {
        VPbkSimServer::PanicClient( aMessage,
            VPbkSimServer::ECVPbkViewSubSession_GetViewContactL );
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::CancelViewEventNotification
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::CancelViewEventNotification()
    {
    if ( iNotificationMsg.Handle() )
        {
        SetErrorEvent( KErrCancel );
        CompleteViewNotification( iNotificationMsg, iErrorEvent, KErrCancel );
        ResetErrorEvent();
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ContactMatchingPrefixL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ContactMatchingPrefixL( const RMessage2& aMessage )
    {
    // First check if the matching is already set
    if ( iMatchingMsg.Handle() )
        {
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrAlreadyExists );
        
        __ASSERT_DEBUG( iFindStrings && iMatchOperation, 
            VPbkSimServer::PanicClient( aMessage,
            VPbkSimServer::ECVPbkViewSubSession_InvalidActiveMatchState));
        }    
    // Save matching message
    else
        {
        __ASSERT_DEBUG( !iFindStrings && !iMatchOperation, 
            VPbkSimServer::PanicClient( aMessage,
            VPbkSimServer::ECVPbkViewSubSession_InvalidNonActiveMatchState));
            
        iMatchingMsg = aMessage;
        iFindStrings = InternalizeFindStringsL( aMessage );
        iMatchOperation = iView->ContactMatchingPrefixL( *iFindStrings, *this );       
        }    
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::CancelContactMatching
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::CancelContactMatching()
    {
    CompleteAsyncContactMatching( KErrCancel );
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::CompleteAsyncContactMatching
// --------------------------------------------------------------------------
//    
void CVPbkViewSubSession::CompleteAsyncContactMatching( TInt aResult )
    {
    delete iMatchOperation;
    iMatchOperation = NULL;
    delete iFindStrings;
    iFindStrings = NULL;
            
    VPbkSimSrvUtility::CompleteRequest( iMatchingMsg, aResult );
    }
    
// --------------------------------------------------------------------------
// CVPbkViewSubSession::ContactMatchingResultL
// --------------------------------------------------------------------------
//    
void CVPbkViewSubSession::ContactMatchingResultL( 
        const RMessage2& aMessage )    
    {
    if ( iMatchingResults )
        {
        TPtr8 ptr = iMatchingResults->Des();    
        aMessage.WriteL( KVPbkSlot0, ptr );
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );    
        }
    else
        {        
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrGeneral );    
        }    
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::SortOrderSizeL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::SortOrderSizeL( const RMessage2& aMessage )
    {
    if ( iView )
        {
        TInt size = iView->SortOrderL().ExternalizedSize();
        TPckg<TInt> pckg( size );
        aMessage.WriteL( KVPbkSlot0, pckg );
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
        }
    else
        {
        // A view error has been occured.
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrAbort );
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::SortOrderL
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::SortOrderL( const RMessage2& aMessage )
    {
    if ( iView )
        {
        HBufC8* buf = ExternalizeArrayLC( iView->SortOrderL() );
        TPtr8 ptr( buf->Des() );
        // Leave KErrOverflow if too small buffer from client
        aMessage.WriteL( KVPbkSlot0, ptr );
        CleanupStack::PopAndDestroy( buf );
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrNone );
        }
    else
        {
        // A view error has been occured.
        VPbkSimSrvUtility::CompleteRequest( aMessage, KErrAbort );
        }
    }
    
// --------------------------------------------------------------------------
// CVPbkViewSubSession::HandleEvent
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::HandleEvent( TVPbkSimContactEventData& aEventData )
    {
    if ( iNotificationMsg.Handle() )
        {
        // There is a waiting notification message
        if ( aEventData.iEvent == EVPbkSimViewError )
            {
            // View is not in valid state anymore. Complete with error
            CompleteViewNotification( iNotificationMsg, aEventData, 
                aEventData.iData );
            }
        else
            {
            CompleteViewNotification( iNotificationMsg, aEventData, KErrNone );
            }
        }
    else
        {
        // Save the event to the queue
        if ( iEventQueue->IsFull() )
            {
            // Queue is full, set an error that will be returned when next
            // request comes.
            SetErrorEvent( KErrOverflow );
            }
        else
            {
            // Push the event to the queue for waiting notification message
            TInt result = iEventQueue->Push( aEventData );
            if ( result != KErrNone )
                {
                SetErrorEvent( result );
                }
            }
        }
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::SetErrorEvent
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::SetErrorEvent( TInt aError )
    {
    iErrorEvent.iEvent = EVPbkSimViewError;
    iErrorEvent.iData = aError;
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ResetErrorEvent
// --------------------------------------------------------------------------
//
void CVPbkViewSubSession::ResetErrorEvent()
    {
    iErrorEvent.iEvent = EVPbkSimUnknown;
    iErrorEvent.iData = 0;
    iErrorEvent.iOpData = 0;
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::ViewFindCompletedL
// --------------------------------------------------------------------------
//    
void CVPbkViewSubSession::ViewFindCompletedL( 
        MVPbkSimCntView& /*aSimCntView*/,
        const RVPbkStreamedIntArray& aSimIndexArray )
    {
    if ( iMatchingResults )
        {
        delete iMatchingResults;
        iMatchingResults = NULL;
        }
    
    iMatchingResults = HBufC8::NewL( aSimIndexArray.ExternalizedSize() );
    TPtr8 ptr = iMatchingResults->Des();
    RDesWriteStream stream( ptr );    
    CleanupClosePushL( stream );
    
    aSimIndexArray.ExternalizeL( stream );
    
    CleanupStack::PopAndDestroy(); //stream  
                
    TPckg<TInt> pckg( aSimIndexArray.ExternalizedSize() );
    iMatchingMsg.WriteL( KVPbkSlot1, pckg );        
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::CloseAndDestroyView
// --------------------------------------------------------------------------
//    
void CVPbkViewSubSession::CloseAndDestroyView()
    {
    if ( iView )
        {
        iView->Close( *this );
        }
    delete iView;
    iView = NULL;
    }

// --------------------------------------------------------------------------
// CVPbkViewSubSession::AssertNoActiveAsyncRequests
// --------------------------------------------------------------------------
//    
void CVPbkViewSubSession::AssertNoActiveAsyncRequests()
    {
    // Check that client has canceled the notification message.
    if ( iNotificationMsg.Handle() )
        {
        VPbkSimServer::PanicClient( iNotificationMsg,
            VPbkSimServer::ECVPbkViewSubSession_Destructor_EventMessage );
        }
    
    if ( iMatchingMsg.Handle() )
        {
        VPbkSimServer::PanicClient( iMatchingMsg,
            VPbkSimServer::ECVPbkViewSubSession_Destructor_Match );
        }
    }
//  End of File  
