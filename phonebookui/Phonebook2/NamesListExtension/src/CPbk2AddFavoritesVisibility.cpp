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
#include "CPbk2AddFavoritesVisibility.h"
#include "CPbk2AddFavoritesVisibilityImpl.h"

// Pbk2
#include <MPbk2ContactViewSupplier.h>
#include <MPbk2ContactViewSupplier2.h>
#include <MVPbkContactViewBase.h>
#include <CPbk2StoreConfiguration.h>
#include <VPbkStoreUriDefinitions.hrh>
#include <MPbk2ApplicationServices.h>

// Virtual Phonebook
#include <VPbkContactStoreUris.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>


namespace
    {
#ifdef _DEBUG
enum TPanicCode
    {
    EPanicContactViewError = 0,
    EPanicUnknownView
    };

/// Unnamed namespace for local definitions
    void Panic(TInt aReason)
        {
        _LIT(KPanicText, "CPbk2AddFav-vis");
        User::Panic(KPanicText, aReason);
        }
#endif // _DEBUG
    } // namespace

CPbk2AddFavoritesVisibility::CPbk2AddFavoritesVisibility(MPbk2ApplicationServices& aAppServices)
    : iTcCount(KErrNotFound), iAllContactsCount(KErrNotFound), iApplicationServices(aAppServices)
	{
	}

CPbk2AddFavoritesVisibility::~CPbk2AddFavoritesVisibility()
    {
    delete iImpl;
    
    if ( iTopContactsView )
        {
        iTopContactsView->RemoveObserver( *this );
        }

    if ( iAllContactsView )
        {
        iAllContactsView->RemoveObserver( *this );
        }

    StopObservingStoreConfiguration();        
    }

void CPbk2AddFavoritesVisibility::ConstructL(
        TInt aCmdId)
    {
    iImpl = CPbk2AddFavoritesVisibilityImpl::NewL(aCmdId);
    StartL();
    }

CPbk2AddFavoritesVisibility* CPbk2AddFavoritesVisibility::NewL(        
    TInt aCmdId, MPbk2ApplicationServices& aAppServices)
	{
	CPbk2AddFavoritesVisibility* self =
	    new (ELeave) CPbk2AddFavoritesVisibility(aAppServices);
    CleanupStack::PushL( self );
    self->ConstructL(aCmdId);
    CleanupStack::Pop( self );
	return self;
	}
    
void CPbk2AddFavoritesVisibility::StartL()    
    {
    switch(iImpl->Variation())
        {
        case EVisibilityUntilFirstFavorite:
        case EVisibilityAlwaysON:
            {
        	if ( !ObserveContactViewsL() )
                {
                NotifyVisibilityChange();
                }

            // to prevent double addition of the same observer
            StopObservingStoreConfiguration();        		
        	StartObservingStoreConfigurationL();                                
            }
         case EVisibilityAlwaysOFF: // fallthrough
         default:            
            {
            // do nothing
            }                                
            break;
        }    
    }
    
TBool CPbk2AddFavoritesVisibility::ObserveContactViewsL()
    {
    MPbk2ContactViewSupplier2* viewSupplierExtension = 
	    reinterpret_cast<MPbk2ContactViewSupplier2*>(
	        iApplicationServices.ViewSupplier().
	            MPbk2ContactViewSupplierExtension(
                    KMPbk2ContactViewSupplierExtension2Uid ));
   
    TBool res = EFalse;
    if ( viewSupplierExtension != NULL )
        {
    	iTopContactsView = 
            viewSupplierExtension->TopContactsViewL();
        }
        
    if( iTopContactsView )
        {
        iTopContactsView->AddObserverL(*this);

    	iAllContactsView = 
        iApplicationServices.ViewSupplier().AllContactsViewL();
        iAllContactsView->AddObserverL( *this );
        res = ETrue;        
        }
    return res; // observing both contact views or not
    }

void CPbk2AddFavoritesVisibility::StartObservingStoreConfigurationL()
    {
    CPbk2StoreConfiguration &storeConfig = 
    iApplicationServices.StoreConfiguration();
    
    storeConfig.AddObserverL(*this);
    }
    
void CPbk2AddFavoritesVisibility::StopObservingStoreConfiguration()
    {
    CPbk2StoreConfiguration &storeConfig = iApplicationServices.StoreConfiguration();
    
    storeConfig.RemoveObserver(*this);
    }
        
void CPbk2AddFavoritesVisibility::NotifyVisibilityChange()
    {
    iImpl->NotifyVisibilityChange(iTcCount, iAllContactsCount);                     
    }
    
void CPbk2AddFavoritesVisibility::UpdateContactCount(
        MVPbkContactViewBase& aView ) 
    {
    TInt count = KErrNotFound;
    TRAP_IGNORE( count = aView.ContactCountL());
    SetContactCount( aView, count );
    }

inline void CPbk2AddFavoritesVisibility::SetContactCount(
        MVPbkContactViewBase& aView,
        TInt aCount ) 
    {
    if( &aView == iTopContactsView )
        {
        iTcCount = aCount;
        }
    else if( &aView == iAllContactsView )
        {
        iAllContactsCount = aCount;
        }       
    else
        {
        __ASSERT_DEBUG(EFalse, Panic(EPanicUnknownView));
        }
    }   

// --------------------------------------------------------------------------
// CPbk2AddFavoritesVisibility::ContactViewReady
// --------------------------------------------------------------------------
//
void CPbk2AddFavoritesVisibility::ContactViewReady(
        MVPbkContactViewBase& aView)
    {   
    UpdateContactCount( aView );
    NotifyVisibilityChange();
    }

// --------------------------------------------------------------------------
// CPbk2AddFavoritesVisibility::ContactViewUnavailable
// --------------------------------------------------------------------------
//
void CPbk2AddFavoritesVisibility::ContactViewUnavailable(
        MVPbkContactViewBase& aView )
    {
    SetContactCount( aView, KErrNotFound );
    NotifyVisibilityChange();            
    }

// --------------------------------------------------------------------------
// CPbk2AddFavoritesVisibility::ContactAddedToView
// --------------------------------------------------------------------------
//
void CPbk2AddFavoritesVisibility::ContactAddedToView(
        MVPbkContactViewBase& aView,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {          
    UpdateContactCount( aView );
    NotifyVisibilityChange();            
    }

// --------------------------------------------------------------------------
// CPbk2AddFavoritesVisibility::ContactRemovedFromView
// --------------------------------------------------------------------------
//
void CPbk2AddFavoritesVisibility::ContactRemovedFromView(
        MVPbkContactViewBase& aView,
        TInt /*aIndex*/,
        const MVPbkContactLink& /*aContactLink*/ )
    {
    UpdateContactCount( aView );
    NotifyVisibilityChange();            
    }

// --------------------------------------------------------------------------
// CPbk2AddFavoritesVisibility::ContactViewError
// --------------------------------------------------------------------------
//
void CPbk2AddFavoritesVisibility::ContactViewError(
        MVPbkContactViewBase& aView,
        TInt /*aError*/,
        TBool /*aErrorNotified*/ )
    {
    __ASSERT_DEBUG(EFalse, Panic(EPanicContactViewError));
    SetContactCount( aView, KErrNotFound );
    NotifyVisibilityChange();            
    }    
   
void CPbk2AddFavoritesVisibility::ConfigurationChanged()
    {
    Reset();
    }

void CPbk2AddFavoritesVisibility::ConfigurationChangedComplete()
    {
    TRAP_IGNORE( HandleConfigurationChangedL() );    
    // inform the client in case of error or not    
    NotifyVisibilityChange(); 
    }

void CPbk2AddFavoritesVisibility::HandleConfigurationChangedL()
    {
    Reset();
    StartL();
    }
    
void CPbk2AddFavoritesVisibility::Reset()
    {
    iTcCount = KErrNotFound;     
    iAllContactsCount = KErrNotFound;                 
    
    if(iTopContactsView)
        {
        iTopContactsView->RemoveObserver(*this);
        iTopContactsView = NULL;
        }
            
    if(iAllContactsView)
        {
        iAllContactsView->RemoveObserver(*this);
        iAllContactsView = NULL;
        }
    }    
    
void CPbk2AddFavoritesVisibility::SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver)
    {
    iImpl->SetVisibilityObserver(aObserver);
    }

void CPbk2AddFavoritesVisibility::SetVisibility(TBool aVisibility)
    {
    iImpl->SetVisibility( aVisibility );
    NotifyVisibilityChange();    
    }

TBool CPbk2AddFavoritesVisibility::Visibility()
    {
    return iImpl->Visibility();
    }
    
// end of file
