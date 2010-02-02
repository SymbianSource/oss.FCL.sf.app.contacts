/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*       xSP View client-side API.
*
*/


// INCLUDE FILES
#include "xSPViewServices.h"
#include "xSPViewServices_priv.h"

// System includes
#include <w32std.h>
#include <apgtask.h>
#include <apacmdln.h>
#include <apgcli.h>
#include <e32svr.h>


// ================= MEMBER FUNCTIONS =======================

EXPORT_C TInt RxSPViewServices::Open()
    {
    if( Handle() != KNullHandle ) return KErrAlreadyExists;
    
    TInt err = CreateSession(KxSPServer, Version());
    if(err == KErrNotFound)
        {
        TRAP(err, StartPhonebookL());
        if(err == KErrNone)
            {
            err = CreateSession(KxSPServer, Version());
            }
        }

    return err;
    }

EXPORT_C void RxSPViewServices::Close()
    {
    RSessionBase::Close();
    }

TVersion RxSPViewServices::Version() const
    {
    return TVersion(KxSPViewServerMajor, KxSPViewServerMinor, KxSPViewServerBuild);
    }

// Show the tab view from plugin aEComID with ID aOriginalViewID.
EXPORT_C TInt RxSPViewServices::Activate(TUint32 aEComID, TInt aOriginalViewID)
    {
    TInt err = Open();

    if( err == KErrAlreadyExists || err == KErrNone )
        {
        TBool close( EFalse );
        if( err == KErrNone )
        	{
        	close = ETrue;
        	}
        
        err = SendReceive( EActivateView1, TIpcArgs( aEComID, aOriginalViewID ) );
        
        if( close )
        	{
        	Close();
        	}        
        }

    return err;    
    }

// Opens the detail view for a contact item
EXPORT_C void RxSPViewServices::ActivateL( TUint32 aEComID,
                                           TInt aOriginalViewID,
                                           MVPbkContactLink* aContactLink,
                                           TInt aFocusedField )
    {
    CPbk2ViewState* pbkViewParam = CPbk2ViewState::NewLC();
    pbkViewParam->SetFocusedContact(aContactLink);

    pbkViewParam->SetFocusedFieldIndex(aFocusedField);
    HBufC8* paramBuf = pbkViewParam->PackLC();

    TPtr8 ptr = paramBuf->Des();

    User::LeaveIfError(Activate(aEComID, aOriginalViewID, ptr));

    CleanupStack::PopAndDestroy(2);  // paramBuf, pbkViewParam
    }

// Opens the detail view for a contact item. The contact item is specified in the HBuf object,
// which is contructed from either CPbk2ViewState on S60 3.2 or CPbkViewState.
EXPORT_C TInt RxSPViewServices::Activate(TUint32 aEComID, TInt aOriginalViewID, TPtr8& aParamBuf)
    {
    TInt err = Open();
    
    if( err == KErrAlreadyExists || err == KErrNone )
        {
        TBool close( EFalse );
        if( err == KErrNone )
        	{
        	close = ETrue;
        	}
        	
        err = SendReceive( EActivateView2, TIpcArgs( aEComID, aOriginalViewID, &aParamBuf ) );
        
        if( close )
        	{
        	Close();
        	}
        }

    return err;
    }

EXPORT_C void RxSPViewServices::Activate( TUint32 aEComID,
                                          TInt aOriginalViewID,
                                          TPtr8& aParamBuf,
                                          TRequestStatus& aStatus )
    {
    SendReceive(EActivateView2Async, TIpcArgs(aEComID, aOriginalViewID, &aParamBuf), aStatus);
    }
    
EXPORT_C TInt RxSPViewServices::CancelActivate() const
	{
	TInt err = SendReceive( ECancelActivateView2Async );
	return err;
	}    
    
EXPORT_C RxSPViewServices::RxSPViewServices() :  
											RSessionBase(),
											iViewCount( NULL, 0, 0 ),
											iPackedViewDataBufferLength( NULL, 0, 0 )
    {
    }
	
EXPORT_C TInt RxSPViewServices::Open( TInt aAsyncMessageSlots,
        									TIpcSessionType aType,
        									TRequestStatus& aStatus )
	{
	if( Handle() != KNullHandle ) return KErrAlreadyExists;
    
    TInt err = CreateSession(KxSPServer,
    							Version(),
    							aAsyncMessageSlots,
    							aType,
    							NULL,
    							&aStatus);
    if(err == KErrNotFound)
        {
        TRAP(err, StartPhonebookL());
        if(err == KErrNone)
            {
            err = CreateSession(KxSPServer,
            					Version(),
    							aAsyncMessageSlots,
    							aType,
    							NULL,
    							&aStatus);
            }
        }

    return err;
	}
	    
EXPORT_C TInt RxSPViewServices::GetViewCount( TInt& aCount ) const
	{
	TPckg<TInt> countBuf( aCount );
	TInt err = SendReceive( EGetViewCount, TIpcArgs( &countBuf ) );
	return err;
	}
	
EXPORT_C TInt RxSPViewServices::GetViewCount( TInt& aCount, TRequestStatus& aStatus )
	{
	iViewCount.Set(reinterpret_cast<TUint8*>(&aCount), sizeof(aCount), sizeof(aCount));
	SendReceive( EGetViewCountAsync, TIpcArgs( &iViewCount ), aStatus );	
	return KErrNone;
	}
	
EXPORT_C TInt RxSPViewServices::CancelGetViewCount() const
	{
	TInt err = SendReceive( ECancelGetViewCountAsync );
	return err;
	}
	
EXPORT_C TInt RxSPViewServices::GetPackedViewDataBufferLength( TInt aIndex, 
														TInt& aLength ) const
	{
	TPckg<TInt> lengthBuf( aLength );
	TInt err = SendReceive( EGetViewDataPackLength, TIpcArgs( aIndex, &lengthBuf ) );
	return err;
	}																
	
EXPORT_C TInt RxSPViewServices::GetPackedViewDataBufferLength( TInt aIndex,
														TInt& aLength,
														TRequestStatus& aStatus )
	{
	iPackedViewDataBufferLength.Set(reinterpret_cast<TUint8*>(&aLength), 
										sizeof(aLength),
										sizeof(aLength));
	SendReceive( EGetViewDataPackLengthAsync, 
										TIpcArgs( aIndex, &iPackedViewDataBufferLength ), 
										aStatus );	
	return KErrNone;
	}															

EXPORT_C TInt RxSPViewServices::CancelGetPackedViewDataBufferLength() const
	{
	TInt err = SendReceive( ECancelGetViewDataPackLengthAsync );
	return err;
	}		
		
EXPORT_C TInt RxSPViewServices::GetPackedViewData( TInt aIndex,
											TDes8& aPackedViewData ) const
	{
	TInt err = SendReceive( EGetViewData, TIpcArgs( aIndex, &aPackedViewData ) );			
	return err;
	}												
										
EXPORT_C TInt RxSPViewServices::GetPackedViewData( TInt aIndex,
											TDes8& aPackedViewData,
											TRequestStatus& aStatus ) 
	{		
	SendReceive( EGetViewDataAsync, TIpcArgs( aIndex, &aPackedViewData ), aStatus );	
	return KErrNone;
	}
	
EXPORT_C TInt RxSPViewServices::CancelGetPackedViewData() const
	{
	TInt err = SendReceive( ECancelGetViewDataAsync );
	return err;
	}    
												      

// Starts Phonebook in the background if it is not already running.
void RxSPViewServices::StartPhonebookL()
    {
    RMutex mutex;
    TInt err = mutex.CreateGlobal(KxSPServer);
    if(err == KErrAlreadyExists)
        {
        err = mutex.OpenGlobal(KxSPServer);
        }
    User::LeaveIfError(err);
    CleanupClosePushL(mutex);

    mutex.Wait();

    RWsSession ws;
    User::LeaveIfError(ws.Connect());
    CleanupClosePushL(ws);
    TApaTaskList list(ws);
    TUid uid = { KUid };
    TApaTask task(list.FindApp(uid));
    if(!task.Exists())
        {
        // Phonebook is not running => we are going to launch it.
        RSemaphore semaphore;
        User::LeaveIfError(semaphore.CreateGlobal(KxSPServer, 0));
        CleanupClosePushL(semaphore);

        // Launch Phonebook
        CApaCommandLine* cmd = CApaCommandLine::NewLC();
        cmd->SetExecutableNameL(KPhonebookExe);
        cmd->SetCommandL(EApaCommandBackground);
        RApaLsSession ls;
        User::LeaveIfError(ls.Connect());
        CleanupClosePushL(ls);
        User::LeaveIfError(ls.StartApp(*cmd));

        // Wait here until server has started or timeout occurs.
        semaphore.Wait(KTimeoutActivate);
        CleanupStack::PopAndDestroy(3); // ls, cmd, semaphore
        }
    CleanupStack::PopAndDestroy(); // ws

    mutex.Signal();
    CleanupStack::PopAndDestroy(); // mutex
    }

// End of file.
