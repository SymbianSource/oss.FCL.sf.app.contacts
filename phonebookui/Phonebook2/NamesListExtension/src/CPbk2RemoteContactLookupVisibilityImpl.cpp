/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDE FILES
#include "CPbk2RemoteContactLookupVisibilityImpl.h"
// pbk2
#include <Phonebook2PrivateCRKeys.h>

// General
#include <centralrepository.h>

namespace
    {
#ifdef _DEBUG
enum TPanicCode
    {   
    EPanicCorruptCenRepKey = 0,
    EPanicVariationFlagUpdateFailed,
    EPanicObserverAlreadyExists        
    };

/// Unnamed namespace for local definitions
    void Panic(TInt aReason)
        {
        _LIT(KPanicText, "Pbk2AddFavVisImpl");
        User::Panic(KPanicText, aReason);
        }
#endif // _DEBUG    
    } // namespace


CPbk2RemoteContactLookupVisibilityImpl* CPbk2RemoteContactLookupVisibilityImpl::NewL(
	    TInt aCmdId)	    
	{
	CPbk2RemoteContactLookupVisibilityImpl* self =
	    new (ELeave) CPbk2RemoteContactLookupVisibilityImpl( aCmdId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	return self;
	}

CPbk2RemoteContactLookupVisibilityImpl::CPbk2RemoteContactLookupVisibilityImpl(
        TInt aCmdId) :
            CActive( CActive::EPriorityIdle ),
            iVariation( EVisibilityAlwaysON ),
            iPreviousVisibility( EFalse ),
            iCmdId( aCmdId )
	{
	}
	
void CPbk2RemoteContactLookupVisibilityImpl::ConstructL()
    {
    CActiveScheduler::Add(this);
    ReadRemoteContactLookupVisibilityKeyL();
    }
    
CPbk2RemoteContactLookupVisibilityImpl::~CPbk2RemoteContactLookupVisibilityImpl()
    {
    Cancel();
    }

void CPbk2RemoteContactLookupVisibilityImpl::ReadRemoteContactLookupVisibilityKeyL()
    {
    //TODO (if needed at all for RCL)
    }    

void CPbk2RemoteContactLookupVisibilityImpl::SetRemoteContactLookupVisibilityKeyOFFL()
    {
    //TODO (if needed at all for RCL)    
    }   

TVariationFlag CPbk2RemoteContactLookupVisibilityImpl::Variation()
    {
    return iVariation;
    }
    
void CPbk2RemoteContactLookupVisibilityImpl::NotifyVisibilityChange()
    {
    }

TBool CPbk2RemoteContactLookupVisibilityImpl::CalculateVisibilityL(const TInt aTopCount, const TInt aContactCount)   
    {
    return iPreviousVisibility;    
    }    

void CPbk2RemoteContactLookupVisibilityImpl::SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver)
    {
    // If removing observer, then call cancel
    if(iObserver && !aObserver)
        {
        Cancel();
        }
        
    iObserver = aObserver;
    }

void CPbk2RemoteContactLookupVisibilityImpl::PrepareAsyncVisibilityCallback()
    {
    if(IsActive())  //TODO: remove this?
	    {
        Cancel();  
        }
    
    if(!IsActive() && iObserver) 
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();        
        }
    }
    
void CPbk2RemoteContactLookupVisibilityImpl::DoCancel()
    {
    // do nothing
    }

void CPbk2RemoteContactLookupVisibilityImpl::RunL()
    {
    if(iStatus == KErrNone)
        {
        // no leave here, therefore, RunError() is not overridden
        // inform observer about the current visibility, whether on or off
        iObserver->CmdItemVisibilityChanged( iCmdId, iPreviousVisibility );     
        }
    }
    
TInt CPbk2RemoteContactLookupVisibilityImpl::RunError(TInt /*aError*/)
    {
    // observer callback left!! This should not have happened
    return KErrNone;
    }
    
TBool CPbk2RemoteContactLookupVisibilityImpl::Visibility()
    {
    return iPreviousVisibility;
    }

void CPbk2RemoteContactLookupVisibilityImpl::SetVisibility(TBool aVisibility)
    {
    if ( iPreviousVisibility != aVisibility )
        {
        iPreviousVisibility = aVisibility; 
        PrepareAsyncVisibilityCallback();
        }
    }
    
// end of file
