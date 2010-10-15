/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for presence cache client.
*
*/

#include <e32std.h>
#include <s32buf.h>

#include <mpresencebuddyinfo2.h>
#include <presencecachewritehandler2.h>
#include <presencecachereadhandler2.h>

#include "presenceobjecthelpers.h"
#include "presencecachedefs2.h"
#include "cacheobjecthelpers.h"
#include "presencecacheclientnotification.h"
#include "presencecacheclient.h"

//Include Cache server namespace
using namespace NCacheSrv2;


// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// CPresenceCacheClient::NewL()
// ---------------------------------------------------------------------------
//
EXPORT_C CPresenceCacheClient* CPresenceCacheClient::NewL()
    {
    CPresenceCacheClient* self = new( ELeave ) CPresenceCacheClient();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::CreateReaderL()
// ---------------------------------------------------------------------------
//
EXPORT_C MPresenceCacheReader2* MPresenceCacheReader2::CreateReaderL()
    {
    return CPresenceCacheClient::NewL();
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::CreateWriterL()
// ---------------------------------------------------------------------------
//
EXPORT_C MPresenceCacheWriter2* MPresenceCacheWriter2::CreateWriterL()
    {
    return CPresenceCacheClient::NewL();
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::~CPresenceCacheClient()
// ---------------------------------------------------------------------------
//
CPresenceCacheClient::~CPresenceCacheClient()
    {
    delete iPresInfoPack;
    delete iPresInfoPack16;    
    delete iNotifyHandler;    
    Cancel();
    RSessionBase::Close();
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::CPresenceCacheClient()
// ---------------------------------------------------------------------------
//
CPresenceCacheClient::CPresenceCacheClient(): CActive( EPriorityStandard ), 
    iWriteHandler(NULL),
    iReadHandler(NULL),
    iAsyncReq(NRequest::ENoRequestMade)
    {
        
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::ConstructL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheClient::ConstructL()
    {
    User::LeaveIfError( Connect() );
    CActiveScheduler::Add( this );  
    }
    
// -----------------------------------------------------------------------------
// CPresenceCacheClient::Connect()
// Connect to the server, attempting to start it if necessary
// -----------------------------------------------------------------------------
//
TInt CPresenceCacheClient::Connect()
    {
    TInt retry=2;
    for (;;)
        {
        TInt r = CreateSession( NName::KSymbianServer,
                                Version(),
                                NRequest::KMsgSlotCount );      
        // Continue if there was error but caused from server not being 
        // started yet, in which case server start will be tried later in the loop.
        // Otherwise return KErrNone or one of the system wide error codes.
        if (r!=KErrNotFound && r!=KErrServerTerminated)    
            return r;
        //
        // Decreace retry counter and abort if too many retries already
        if (--retry==0)    
            return r;
        //
        // Try to start server. If no error or the error was caused  
        // from server already started, continue to next iteration when 
        // the session creation will be tried again.
        r=StartServer();
        if (r!=KErrNone && r!=KErrAlreadyExists)      
            return r;
            
        }  
    }
          
// ----------------------------------------------------
// CPresenceCacheClient::StartServer
// ----------------------------------------------------
//
TInt CPresenceCacheClient::StartServer()
    {  
    // Start the server process. Simultaneous launching
    // of two such processes should be detected when the second one attempts to
    // create the server object, failing with KErrAlreadyExists.
    //
    RProcess server;
    TInt r=server.Create( NName::KExecutable, KNullDesC );  
    if (r!=KErrNone) 
        return r;
    TRequestStatus stat;
    server.Rendezvous(stat);
    if (stat!=KRequestPending) 
        server.Kill(0);     // abort startup  
    else 
        server.Resume();    // logon OK - start the server
    User::WaitForRequest(stat);     // wait for start or death
    // we can't use the 'exit reason' if the server panicked as this
    // is the panic 'reason' and may be '0' which cannot be distinguished
    // from KErrNone
    r=(server.ExitType()==EExitPanic) ? KErrGeneral : stat.Int();
    // here it may return KErrAlreadyExists or KErrNone
    server.Close();    
    return r;
    }
    
// -----------------------------------------------------------------------------
// CPresenceCacheClient::Version()
// Gets the version number.
// -----------------------------------------------------------------------------
//
TVersion CPresenceCacheClient::Version() const
    {
    return( TVersion( NVersion::KMajor,
                      NVersion::KMinor,
                      NVersion::KBuild ) );
    }
 
// ---------------------------------------------------------------------------
// CPresenceCacheClient::PresenceInfoLC()
// ---------------------------------------------------------------------------
//    
MPresenceBuddyInfo2* CPresenceCacheClient::PresenceInfoLC(const TDesC& aIdentity)
    {
    MPresenceBuddyInfo2*  presInfo(NULL);  
    
    if ( !ValidateXspId( aIdentity ) )    
        return presInfo;
    
    TInt sizeOfPresInfo(0);
    TPckgBuf<TInt> sizePckg;
                
    // Package message arguments before sending to the server
    TIpcArgs args;
    args.Set(1, &aIdentity);    
    args.Set(2, &sizePckg);    
        
    // We will get either size(+ve) or error code(+ve) after this call
    TInt err = SendReceive( NRequest::EPrepReadPresenceInfoSync, args );
    
    if ( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }
    sizeOfPresInfo = sizePckg();
        
    if ( sizeOfPresInfo && (err==KErrNone) ) // If found
        {
        presInfo = MPresenceBuddyInfo2::NewLC();    
        HBufC8* presInfoDes = HBufC8::NewLC(sizeOfPresInfo);                        
        TPtr8 ptrBuf( presInfoDes->Des() );                        
                               
        SendReceive( NRequest::EGetLastPreparedPacket, TIpcArgs(&ptrBuf) );
        
        TCacheObjectPacker< MPresenceBuddyInfo2 >::UnPackL( *presInfo, *presInfoDes );
        CleanupStack::PopAndDestroy(presInfoDes);
        }
            
    return presInfo;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::BuddyCountInAllServices()
// ---------------------------------------------------------------------------
//    
TInt CPresenceCacheClient::BuddyCountInAllServices()
    {
    TInt buddyCount(0);
    TPckgBuf<TInt> buddyCountPckg;
                      
    // Package message arguments before sending to the server
    TIpcArgs args(&buddyCountPckg);
        
    // This call waits for the server to complete the request before
    // proceeding.
    TInt err = SendReceive( NRequest::EBuddyCountInAllServices, args );
    buddyCount = buddyCountPckg();
    
    return (err<0) ? err  : buddyCount;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::BuddyCountInService()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::BuddyCountInService(const TDesC& aServiceName)
    {
    TInt buddyCount(0);
    TPckgBuf<TInt> buddyCountPckg;
    
    // Package message arguments before sending to the server
    TIpcArgs args(&buddyCountPckg);
    args.Set(1, &aServiceName);
        
    // This call waits for the server to complete the request before
    // proceeding.
    TInt err = SendReceive( NRequest::EBuddyCountInService, args );
    buddyCount = buddyCountPckg();
    
    return (err<0) ? err  : buddyCount;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::ServicesCount()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::ServicesCount()
    {
    TInt serviceCount(0);
    TPckgBuf<TInt> serviceCountPckg;
    
    // Package message arguments before sending to the server
    TIpcArgs args(&serviceCountPckg);
        
    // This call waits for the server to complete the request before
    // proceeding.
    TInt err = SendReceive( NRequest::EGetServiceCount, args );
    serviceCount = serviceCountPckg();
    
    return (err<0) ? err  : serviceCount;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::AllBuddiesPresenceInService()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::AllBuddiesPresenceInService(
    const TDesC& aServiceName, 
    MPresenceCacheReadHandler2* aHandler)
    {
    if (IsActive())
        return KErrNotReady;
    
    if (!aHandler || (aServiceName.Length()==NULL) )
        return KErrArgument;
    else
        iReadHandler = aHandler;
    
    delete iPresInfoPack16;
    iPresInfoPack16 = NULL;   
    TRAPD( err, iPresInfoPack16 = aServiceName.AllocL());
    if ( err )
        {
        return err;
        }
    
    // Package message arguments before sending to the server
    TIpcArgs args(iPresInfoPack16);    
        
    // This call is async
    SendReceive( NRequest::EPrepReadAllBuddiesPresenceInService, args, iStatus );
    
    iAsyncReq = NRequest::EPrepReadAllBuddiesPresenceInService;
    
    SetActive(); 
    
    return KErrNone;        
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::SubscribePresenceBuddyChangeL
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::SubscribePresenceBuddyChangeL(
    const TDesC& aIdentity)
    {
    if ( !ValidateXspId(aIdentity ))
        return KErrArgument;

    if ( !iNotifyClient )
    	{
    	return KErrNotReady;
    	}
    
    if ( !iNotifyHandler )
    	{
    	iNotifyHandler = CPresenceCacheClientNotification::NewL( *this );  	    	
    	}
    
    TInt ret = iNotifyHandler->SubscribePresenceBuddyChangeL( aIdentity );
    return ret; 
    }  
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::UnSubscribePresenceBuddyChangeL
// ---------------------------------------------------------------------------
//
void CPresenceCacheClient::UnSubscribePresenceBuddyChangeL(
                        const TDesC& aIdentity)
    {
    if (!ValidateXspId( aIdentity ))
        return;
    
    if ( iNotifyHandler )
    	{
    	iNotifyHandler->UnsubscribePresenceBuddyChangeL( aIdentity);  	    	
    	} 
    }      
                                           
// ---------------------------------------------------------------------------
// CPresenceCacheClient::SetObserverForSubscribedNotifications
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::SetObserverForSubscribedNotifications(
    MPresenceCacheReadHandler2* aHandler)
    {
    if ( !aHandler )
    	{
    	return KErrArgument;
    	}
    iNotifyClient = aHandler;
    return KErrNone;               
    }                                          
                                             
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::HandlePreparedAllBuddiesPresenceL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheClient::HandlePreparedAllBuddiesPresenceL(TInt aError)
    {
    TInt status = aError; // size comes in status
    
    if (!iReadHandler)
        {
        // this is possible in cancel situation        
        return;
        }
        
    RPointerArray<MPresenceBuddyInfo2> tempArray;
    CleanupClosePushL( tempArray );
    
    if (status>0) // if anything found
        {   
        HBufC8* buddyInfosDes = HBufC8::NewLC(status);                        
        TPtr8 ptrBuf( buddyInfosDes->Des() );                        
                               
        status = SendReceive( NRequest::EGetLastAsyncPreparedPacket, TIpcArgs(&ptrBuf) );
        
        TRAPD( err, TPresenceArrayPacker::UnPackArrayL( tempArray, ptrBuf ));
        if ( err )
            {
            tempArray.ResetAndDestroy();
            }
        CleanupStack::PopAndDestroy(buddyInfosDes);
        }
    
    // Thos takes ownership of the elements in the array
    iReadHandler->HandlePresenceReadL(status, tempArray);
    CleanupStack::PopAndDestroy( &tempArray  ); // Close the array       
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::WritePresenceL()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::WritePresenceL(
    const MPresenceBuddyInfo2* aPresenceBuddyInfo)
    {
    if (!aPresenceBuddyInfo)
        return KErrArgument;
    
    if (!ValidateXspId( aPresenceBuddyInfo->BuddyId() ))
        return KErrArgument;
        
    HBufC8* presInfoPack = PackBuddyPresenceInfoLC( aPresenceBuddyInfo );
    
    // Package message arguments before sending to the server
    TIpcArgs args(presInfoPack);
                                                            
       
    // This call waits for the server to complete the request before
    // proceeding.
    TInt err = SendReceive( NRequest::EWritePresenceInfoSync, args );
    
    CleanupStack::PopAndDestroy(presInfoPack);        
    
    return err;
    
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::DeletePresenceL()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::DeletePresenceL( const TDesC& aIdentity)
    {   
    // Package message arguments before sending to the server
    TIpcArgs args( &aIdentity );
       
    // This call waits for the server to complete the request before
    // proceeding.
    TInt err = SendReceive( NRequest::EDeletePresence, args ); 
    
    return err;
    }
    

// ---------------------------------------------------------------------------
// CPresenceCacheClient::CancelWrite()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::CancelWrite()
    {
    TInt ret(KErrNotFound);
    if(IsActive())
        {
        Cancel();
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::CancelRead()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::CancelRead()
    {
    TInt ret(KErrNotFound);
    if(IsActive())
        {
        Cancel();
        ret = KErrNone;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::DeleteService()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::DeleteService(const TDesC& aServiceName)
    {
    return SendReceive( NRequest::EDeleteService, TIpcArgs(&aServiceName) );
    }
    


// ---------------------------------------------------------------------------
// CPresenceCacheClient::NewBuddyPresenceInfoLC()
// ---------------------------------------------------------------------------
//  

MPresenceBuddyInfo2* CPresenceCacheClient::NewBuddyPresenceInfoLC()
    {
    return MPresenceBuddyInfo2::NewLC();
    }    

                                                
// ---------------------------------------------------------------------------
// CPresenceCacheClient::WritePresenceL()
// ---------------------------------------------------------------------------
//
TInt CPresenceCacheClient::WriteMultiplePresenceL(
    const RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfos,
    MPresenceCacheWriteHandler2* aHandler)
    {
    if (IsActive())
        return KErrNotReady;
        
    delete iPresInfoPack;
    iPresInfoPack = NULL;    
    iPresInfoPack = PackPresenceBuddyListLC(aPresenceBuddyInfos);
    CleanupStack::Pop(iPresInfoPack); 
    
    // Package message arguments before sending to the server
    TIpcArgs args(iPresInfoPack);
    args.Set(1,aPresenceBuddyInfos.Count());
                                                            
    // This is async call to server.
    SendReceive( NRequest::EWritePresenceInfoAsync, args, iStatus);
  
    iAsyncReq = NRequest::EWritePresenceInfoAsync;    
    if (aHandler)
        {
        iWriteHandler = aHandler;
        }           
    SetActive();
           
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::ValidateXspId()
// ---------------------------------------------------------------------------
//
TBool CPresenceCacheClient::ValidateXspId(const TDesC& aXspId)
    {
    TBool ret(EFalse);
    _LIT(KColon, ":");
    TInt pos = aXspId.Find(KColon);
    if(pos>0) // if colon found and there is something before colon, i.e. xsp id
        {
        ret = ETrue;
        }
    return ret;
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::RunL()
// ---------------------------------------------------------------------------
//
void CPresenceCacheClient::RunL()
    {
    
    TInt origStatus = iStatus.Int();
    switch (iAsyncReq)
        {
        case NRequest::EWritePresenceInfoAsync:
            if(iWriteHandler) // if client had requested the call back
                iWriteHandler->HandlePresenceWriteL(origStatus);
            iAsyncReq = NRequest::ENoRequestMade;
            break;

        case NRequest::EPrepReadAllBuddiesPresenceInService:
            HandlePreparedAllBuddiesPresenceL(origStatus);
            iAsyncReq = NRequest::ENoRequestMade;
            break;
        
        case NRequest::ENoRequestMade:
        default:
            break;
        }   
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::DoCancel()
// ---------------------------------------------------------------------------
//
void CPresenceCacheClient::DoCancel()
    {
    iAsyncReq = NRequest::ENoRequestMade;
    iReadHandler = NULL;
    iWriteHandler = NULL;
    SendReceive( NRequest::ECancelAsyncOperation);
    // in CPresenceCacheClientNotification SendReceive( NRequest::ECancelWaitingForNotification)
    }
    
// ---------------------------------------------------------------------------
// CPresenceCacheClient::PackBuddyPresenceInfoLC()
// ---------------------------------------------------------------------------
//
HBufC8* CPresenceCacheClient::PackBuddyPresenceInfoLC(
    const MPresenceBuddyInfo2* aBuddyPresInfo)
    {
    HBufC8* pack(NULL);
    
    if ( aBuddyPresInfo )
        {        
        pack = TCacheObjectPacker< const MPresenceBuddyInfo2>::PackL( *aBuddyPresInfo );
        CleanupStack::PushL( pack );
        }

    return pack;
    }

// ---------------------------------------------------------------------------
// CPresenceCacheClient::PackPresenceBuddyListLC()
// ---------------------------------------------------------------------------
//
HBufC8* CPresenceCacheClient::PackPresenceBuddyListLC(const RPointerArray<MPresenceBuddyInfo2>& aList)
    {
    HBufC8* pack(NULL);    
    pack = TPresenceArrayPacker::PackArrayL( aList ); 
    CleanupStack::PushL( pack );        
    return pack;
    }
    
// end of file
