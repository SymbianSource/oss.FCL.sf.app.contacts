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
#include "CPbk2RemoteContactLookupVisibility.h"
#include "CPbk2RemoteContactLookupVisibilityImpl.h"

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
        _LIT(KPanicText, "CPbk2SrchRem-vis");
        User::Panic(KPanicText, aReason);
        }
#endif // _DEBUG
    } // namespace

CPbk2RemoteContactLookupVisibility::CPbk2RemoteContactLookupVisibility(MPbk2ApplicationServices& aAppServices) : 
    iApplicationServices(aAppServices)
	{
	}

CPbk2RemoteContactLookupVisibility::~CPbk2RemoteContactLookupVisibility()
    {
    delete iImpl;
    
    StopObservingStoreConfiguration();        
    }

void CPbk2RemoteContactLookupVisibility::ConstructL(
        TInt aCmdId)
    {
    iImpl = CPbk2RemoteContactLookupVisibilityImpl::NewL(aCmdId);
    StartL();
    }

CPbk2RemoteContactLookupVisibility* CPbk2RemoteContactLookupVisibility::NewL(        
    TInt aCmdId, MPbk2ApplicationServices& aAppServices)
	{
	CPbk2RemoteContactLookupVisibility* self =
	    new (ELeave) CPbk2RemoteContactLookupVisibility(aAppServices);
    CleanupStack::PushL( self );
    self->ConstructL(aCmdId);
    CleanupStack::Pop( self );
	return self;
	}
    
void CPbk2RemoteContactLookupVisibility::StartL()    
    {
    //TODO (if needed at all for RCL)    
    }
    
void CPbk2RemoteContactLookupVisibility::StartObservingStoreConfigurationL()
    {
    CPbk2StoreConfiguration &storeConfig = 
    iApplicationServices.StoreConfiguration();
    
    storeConfig.AddObserverL(*this);
    }
    
void CPbk2RemoteContactLookupVisibility::StopObservingStoreConfiguration()
    {
    CPbk2StoreConfiguration &storeConfig = iApplicationServices.StoreConfiguration();
    
    storeConfig.RemoveObserver(*this);
    }
        
void CPbk2RemoteContactLookupVisibility::NotifyVisibilityChange()
    {
    iImpl->NotifyVisibilityChange();                     
    }
    
void CPbk2RemoteContactLookupVisibility::ConfigurationChanged()
    {
    Reset();
    }

void CPbk2RemoteContactLookupVisibility::ConfigurationChangedComplete()
    {
    TRAP_IGNORE( HandleConfigurationChangedL() );    
    // inform the client in case of error or not    
    NotifyVisibilityChange(); 
    }

void CPbk2RemoteContactLookupVisibility::HandleConfigurationChangedL()
    {
    Reset();
    StartL();
    }
    
void CPbk2RemoteContactLookupVisibility::Reset()
    {
    //TODO (if needed at all for RCL)
    }    
    
void CPbk2RemoteContactLookupVisibility::SetVisibilityObserver(MPbk2CmdItemVisibilityObserver* aObserver)
    {
    iImpl->SetVisibilityObserver(aObserver);
    }

void CPbk2RemoteContactLookupVisibility::SetVisibility(TBool aVisibility)
    {
    iImpl->SetVisibility( aVisibility );
    }

TBool CPbk2RemoteContactLookupVisibility::Visibility()
    {
    return iImpl->Visibility();
    }

    
// end of file
