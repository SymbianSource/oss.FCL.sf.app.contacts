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
#include "CPbk2AddFavoritesVisibilityImpl.h"
// Pbk2
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


CPbk2AddFavoritesVisibilityImpl* CPbk2AddFavoritesVisibilityImpl::NewL(
	    TInt aCmdId)	    
	{
	CPbk2AddFavoritesVisibilityImpl* self =
	    new (ELeave) CPbk2AddFavoritesVisibilityImpl( aCmdId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
	return self;
	}

CPbk2AddFavoritesVisibilityImpl::CPbk2AddFavoritesVisibilityImpl(
        TInt aCmdId) :
            CActive( CActive::EPriorityIdle ),
            iVariation( EVisibilityAlwaysOFF ),
            iPreviousVisibility( EFalse ),
            iCmdId( aCmdId )
	{
	}
	
void CPbk2AddFavoritesVisibilityImpl::ConstructL()
    {
    CActiveScheduler::Add(this);
    ReadAddFavoritiesVisibilityKeyL();
    }
    
CPbk2AddFavoritesVisibilityImpl::~CPbk2AddFavoritesVisibilityImpl()
    {
    Cancel();
    }

void CPbk2AddFavoritesVisibilityImpl::ReadAddFavoritiesVisibilityKeyL()
    {
    // Read the variation flag
    CRepository* repository = NULL;
    
    // Read local variation flags
    // if didn't leave, repository is not NULL
    repository = CRepository::NewL( TUid::Uid( KCRUidPhonebook ) );
    CleanupStack::PushL( repository );
    
    TInt variationFlag = KErrNotFound; // EVisibilityUndefined
    User::LeaveIfError(repository->Get( KPhonebookAddFavoritiesVisibility, variationFlag ));
        
    TBool validVal = ((variationFlag >= EVisibilityAlwaysOFF) && 
                      (variationFlag <= EVisibilityAlwaysON));
    
    // on hw urel builds, if cenrep key is corrupt, we wont show
    // the promotion
    if(!validVal)
        {
        variationFlag = KErrNotFound; // EVisibilityUndefined        
        }
        
    iVariation = static_cast<TVariationFlag>(variationFlag); 
    CleanupStack::PopAndDestroy( repository );       
    }    

void CPbk2AddFavoritesVisibilityImpl::SetAddFavoritiesVisibilityKeyOFFL()
    {
    // Read the variation flag
    CRepository* repository = NULL;
    
    // Read local variation flags
    repository = CRepository::NewL( TUid::Uid( KCRUidPhonebook ) );
    CleanupStack::PushL( repository );
    
    TInt variationFlag = EVisibilityAlwaysOFF;        
    User::LeaveIfError(repository->Set(KPhonebookAddFavoritiesVisibility, variationFlag));
    CleanupStack::PopAndDestroy( repository );            
    }   

TVariationFlag CPbk2AddFavoritesVisibilityImpl::Variation()
    {
    return iVariation;
    }
    
void CPbk2AddFavoritesVisibilityImpl::NotifyVisibilityChange
    (const TInt aTopCount, const TInt aContactCount)
    {
    TBool newVisibility = EFalse;
    
    if( iVisibilityOkToShow ) 
        {                     
        TRAPD(err, newVisibility = CalculateVisibilityL(aTopCount, aContactCount));
        if(err)
            {
            // Setting cenrep key failed.. Turn visibility off & let
            // client know if needed
            newVisibility = EFalse;	
            __ASSERT_DEBUG((err == KErrNone), Panic(EPanicVariationFlagUpdateFailed));
            }
        }

    // Finally, there must be a change from previous visibility value
    // to new one to inform the observer.
    if ( iPreviousVisibility != newVisibility )
        {
        PrepareAsyncVisibilityCallback();
        }

    iNewVisibility = newVisibility;    	
    }

TBool CPbk2AddFavoritesVisibilityImpl::CalculateVisibilityL(const TInt aTopCount, const TInt aContactCount)   
    {
    TBool newVisibility = EFalse;
      
    switch(iVariation)    
        {
            case EVisibilityUntilFirstFavorite:
                {
                // if EVisibilityUntilFirstFavorite, then 
                // check if TC feature is ever used
                if(aTopCount > 0)
                    {
                    // Top functionality is used. Set it OFF
                    newVisibility = EFalse;
                    SetAddFavoritiesVisibilityKeyOFFL();
                    iVariation = EVisibilityAlwaysOFF;                    
                    }
                else 
                    {
                    // TC not used yet by user
                    newVisibility = ETrue;
                    }
                }
                break; 
                
            case EVisibilityAlwaysON:
                {
                // when there are no top contacts
                newVisibility = (aTopCount == 0);
                }   
                break;                 
                
            case EVisibilityAlwaysOFF:
                {         
                newVisibility = EFalse;            			
                }
            	break;
            			
            default:             
                {
				// on hw urel builds, if cenrep key is corrupt, we wont show
    			// the promotion                	
                newVisibility = EFalse;    			
                __ASSERT_DEBUG(EFalse, Panic(EPanicCorruptCenRepKey));
                }
        }
    
    // There must be at least 1 contact to be able 
    // to show the promotion
    newVisibility &= (aContactCount > 0);

    return newVisibility;    
    }    

void CPbk2AddFavoritesVisibilityImpl::SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver)
    {
    // If removing observer, then call cancel
    if(iObserver && !aObserver)
        {
        Cancel();
        }
        
    iObserver = aObserver;
    }

void CPbk2AddFavoritesVisibilityImpl::PrepareAsyncVisibilityCallback()
    {
    if(!IsActive() && iObserver)
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        SetActive();        
        }
    }
    
void CPbk2AddFavoritesVisibilityImpl::DoCancel()
    {
    // do nothing
    }

void CPbk2AddFavoritesVisibilityImpl::RunL()
    {
    if(iStatus == KErrNone)
        {
        // no leave here, therefore, RunError() is not overridden
        // inform observer about the current visibility, whether on or off
        iPreviousVisibility = iNewVisibility;
        iObserver->CmdItemVisibilityChanged( iCmdId, iNewVisibility );        
        }
    }
    
TInt CPbk2AddFavoritesVisibilityImpl::RunError(TInt /*aError*/)
    {
    // observer callback left!! This should not have happened
    return KErrNone;
    }
    
TBool CPbk2AddFavoritesVisibilityImpl::Visibility()
    {
    return iPreviousVisibility;
    }

void CPbk2AddFavoritesVisibilityImpl::SetVisibility(TBool aVisibility)
    {
    iVisibilityOkToShow = aVisibility;
    }
    
// end of file
