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
* Description:  The sim store subsession class
*
*/



// INCLUDE FILES
#include "RVPbkSimStore.h"

#include <VPbkSimServerCommon.h>
#include <VPbkSimServerOpCodes.h>
#include <TVPbkSimStoreProperty.h>
#include "VPbkSimServerStarter.h"
#include <RVPbkStreamedIntArray.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <s32mem.h> // RDesStream

// CONSTANTS
// The max amount attempts to connect to server
const TInt KVPbkMaxConnectionAttempts = 5;
const TInt KVPbkInitialIndexBufferSize = 10;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// RVPbkSimStore::RVPbkSimStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C RVPbkSimStore::RVPbkSimStore()
:   iEvent( NULL, 0 ),
    iStoreError( NULL, 0 ),
    iSavedSimIndex( NULL, 0 )
    {
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::ConnectToServerL
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::ConnectToServerL()
    {
    TInt res = KErrUnknown;
    TBool sessionCreated = EFalse;
    // Try to connect to server.
    for ( TInt i = 0; !sessionCreated && i < KVPbkMaxConnectionAttempts; ++i )
        {
        // Reserve message slots according to amount of asynchronous operations.
        // Add one for synchronous operations (Cancel is synchronous too).
        TInt slots = KVPbkSimSrvAsyncOperations + 1;
        res = CreateSession( KVPbkSimServerName, 
            TVersion( KVPbkSimServerMajorVersion, 
            KVPbkSimServerMinorVersion,
            KVPbkSimServerBuildVersion ), 
            slots );

        if ( res == KErrNone )
            {
            sessionCreated = ETrue;
            }

        switch ( res )
            {
            case KErrNotFound: // FALLTHROUGH
            case KErrServerTerminated:
                {
                // If server didn't exist start it
                res = VPbkSimServerStarter::Start();
                break;
                }
            default:
                {
                break;
                }
            }
        }

    User::LeaveIfError( res );
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::Close
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::Close()
    {
    delete iIndexBuffer;
    iIndexBuffer = NULL;
    if ( Handle() )
        {
        SendReceive( EVPbkSimSrvCloseStore );
        }
    RSessionBase::Close();
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::OpenL
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::OpenL( const TSecurityInfo& aSecurityInfo, 
                                    TVPbkSimStoreIdentifier aIdentifier )
    {
    /// A package for the store identifier
    TPckg<TSecurityInfo> secPckg( aSecurityInfo );
    TPckg<TVPbkSimStoreIdentifier> pckg( aIdentifier );
    User::LeaveIfError( 
        SendReceive( EVPbkSimSrvOpenStore, TIpcArgs( &pckg, &secPckg ) ) );
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::CancelAsyncRequest
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::CancelAsyncRequest( TInt aReqToCancel )
    {
    TPckg<TInt> opCode( aReqToCancel );
    SendReceive( EVPbkSimSrvCancelAsyncRequest, TIpcArgs( &opCode ) );
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::ListenToViewEvents
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::ListenToStoreEvents( TRequestStatus& aStatus,
    TVPbkSimContactEventData& aEvent )
    {
    TPckg<TVPbkSimContactEventData> pckg( aEvent );
    iEvent.Set( pckg );
    SendReceive( EVPbkSimSrvStoreEventNotification, TIpcArgs( &iEvent ), 
        aStatus );
    }    

// -----------------------------------------------------------------------------
// RVPbkSimStore::ReadLC
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* RVPbkSimStore::ReadLC( TInt aSimIndex )
    {
    TInt length = KErrUnknown;
    TPckg<TInt> lengthPckg( length );
    
    // Get the length and load contact in the server side
    TInt result = SendReceive( EVPbkSimSrvGetSizeAndLoadContact, 
        TIpcArgs( aSimIndex, &lengthPckg ) );

    HBufC8* etelContact = NULL;
    if ( result == KErrNone && length > 0 )
        {
        // Create a buffer for contact.
        etelContact = HBufC8::NewLC( length );
        TPtr8 ptr( etelContact->Des() );
        // Read the loaded contact
        result = SendReceive( EVPbkSimSrvGetContact, TIpcArgs( &ptr ) );
        }
    
    if ( result != KErrNone && result != KErrNotFound )
        {
        User::Leave( result );
        }
    
    return etelContact;
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::SaveL
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::SaveL( TRequestStatus& aStatus, const TDesC8& aData, 
    TInt& aSimIndex )
    {
    TPckg<TInt> index( aSimIndex );
    iSavedSimIndex.Set( index );

    SendReceive( EVPbkSimSrvSaveContact, 
        TIpcArgs( &aData, &iSavedSimIndex ), aStatus );
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::Delete
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::DeleteL( TRequestStatus& aStatus, 
                                     RVPbkStreamedIntArray& aSimIndexes )
    {
    TInt neededBufLength = aSimIndexes.ExternalizedSize();
    if ( !iIndexBuffer )
        {
        iIndexBuffer = HBufC8::NewL( KVPbkInitialIndexBufferSize );
        }        
    TPtr8 ptr( iIndexBuffer->Des() );
    VPbkSimStoreImpl::CheckAndUpdateBufferSizeL( iIndexBuffer, 
        ptr, neededBufLength );
            
    ptr.Zero();
    RDesWriteStream stream( ptr );
    CleanupClosePushL( stream );
    stream << aSimIndexes;
    CleanupStack::PopAndDestroy(); // stream
    ptr.SetLength( neededBufLength );

    SendReceive( EVPbkSimSrvDeleteContact, TIpcArgs( iIndexBuffer ), aStatus );
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::GetGsmStoreProperties
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RVPbkSimStore::GetGsmStoreProperties( 
    TVPbkGsmStoreProperty& aGsmProperties ) const
    {
    TPckg<TVPbkGsmStoreProperty> pckg( aGsmProperties );
    if ( Handle() )
    	{   	
    	return SendReceive( EVPbkSimSrvGsmStoreProperties, TIpcArgs( &pckg ) );
    	}
    else
    	{
    	return KErrServerTerminated;
    	}
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::GetUSimStoreProperties
// -----------------------------------------------------------------------------
//
EXPORT_C TInt RVPbkSimStore::GetUSimStoreProperties( 
    TVPbkUSimStoreProperty& aUSimProperties ) const
    {
    TPckg<TVPbkUSimStoreProperty> pckg( aUSimProperties );
    return SendReceive( EVPbkSimSrvUSimStoreProperties, TIpcArgs( &pckg ) );
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::MatchPhoneNumber
//
// EVPbkSimSrvMatchPhoneNumber and EVPbkSimSrvFind must use the same location
// for aSimIndexBuffer
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::MatchPhoneNumber( TRequestStatus& aStatus,
    const TDesC& aPhoneNumber, TInt aMaxMatchDigits, TDes8& aSimIndexBuffer )
    {
    SendReceive( EVPbkSimSrvMatchPhoneNumber, 
        TIpcArgs( &aPhoneNumber, aMaxMatchDigits, &aSimIndexBuffer ), 
        aStatus );
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::Find
//
// EVPbkSimSrvMatchPhoneNumber and EVPbkSimSrvFind must use the same location
// for aSimIndexBuffer
// -----------------------------------------------------------------------------
//
EXPORT_C void RVPbkSimStore::Find( TRequestStatus& aStatus,
    const TDesC& aStringToFind, const TDesC8& aFieldTypes,
    TDes8& aSimIndexBuffer )
    {
    SendReceive( EVPbkSimSrvFind, 
        TIpcArgs( &aStringToFind, &aFieldTypes, &aSimIndexBuffer ), aStatus );
    }
  
// -----------------------------------------------------------------------------
// RVPbkSimStore::OpenPhoneL
// -----------------------------------------------------------------------------
//    
EXPORT_C void RVPbkSimStore::OpenPhoneL()
    {
    SendReceive(EVPbkSimSrvOpenPhone);
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::ClosePhone
// -----------------------------------------------------------------------------
//    
EXPORT_C void RVPbkSimStore::ClosePhone()
    {
    SendReceive(EVPbkSimSrvClosePhone);
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::USimAccessSupported
// -----------------------------------------------------------------------------
//    
EXPORT_C TBool RVPbkSimStore::USimAccessSupported() const
    {
    TBool retval(EFalse);
    TPckg<TBool> retvalPckg( retval );
    SendReceive(EVPbkSimSrvUSimAccessSupported, TIpcArgs(&retvalPckg));        
    return retval;
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::ServiceTable
// -----------------------------------------------------------------------------
//    
EXPORT_C TUint32 RVPbkSimStore::ServiceTable() const
    {
    TUint32 serviceTable(0);
    TPckg<TUint32> serviceTablePckg( serviceTable );
    SendReceive(EVPbkSimSrvServiceTable, TIpcArgs(&serviceTablePckg));    
    return serviceTable;
    }

// -----------------------------------------------------------------------------
// RVPbkSimStore::FixedDialingStatus
// -----------------------------------------------------------------------------
//    
EXPORT_C MVPbkSimPhone::TFDNStatus RVPbkSimStore::FixedDialingStatus() const  
    {
    MVPbkSimPhone::TFDNStatus status;
    TPckg<MVPbkSimPhone::TFDNStatus> statusPckg(status);
    SendReceive(EVPbkSimSrvFixedDiallingStatus, TIpcArgs(&statusPckg));
    return status;
    }
    
//  End of File  
