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
* Description: 
*
*/


#include "CPbk2StoreConfiguration.h"

// Phonebook 2
#include "CPbk2StoreConfigurationImpl.h"

// Virtual Phonebook
#include <TVPbkContactStoreUriPtr.h>

// Debugging headers
#include <Pbk2Debug.h>


// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::CPbk2StoreConfiguration
// --------------------------------------------------------------------------
//
CPbk2StoreConfiguration::CPbk2StoreConfiguration()
    {
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::~CPbk2StoreConfiguration
// --------------------------------------------------------------------------
//
CPbk2StoreConfiguration::~CPbk2StoreConfiguration()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfiguration::~CPbk2StoreConfiguration(0x%x)"), this);

    delete iImplementation;
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfiguration::ConstructL()
    {
    iImplementation = CPbk2StoreConfigurationImpl::NewL();
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreConfiguration* CPbk2StoreConfiguration::NewL()
    {
    CPbk2StoreConfiguration* self = new(ELeave) CPbk2StoreConfiguration;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::CurrentConfigurationL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkContactStoreUriArray*
        CPbk2StoreConfiguration::CurrentConfigurationL() const
    {
    return iImplementation->CurrentConfigurationL();
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::SearchStoreConfigurationL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkContactStoreUriArray*
        CPbk2StoreConfiguration::SearchStoreConfigurationL()
    {
    return iImplementation->SearchStoreConfigurationL();
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::SupportedStoreConfigurationL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkContactStoreUriArray*
        CPbk2StoreConfiguration::SupportedStoreConfigurationL() const
    {
    return iImplementation->SupportedStoreConfigurationL();
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::DefaultSavingStoreL
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkContactStoreUriPtr
        CPbk2StoreConfiguration::DefaultSavingStoreL() const
    {
    return iImplementation->DefaultSavingStoreL();
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::AddContactStoreURIL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::AddContactStoreURIL
        (TVPbkContactStoreUriPtr aURI)
    {
    iImplementation->AddContactStoreURIL( aURI );
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::RemoveContactStoreURIL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::RemoveContactStoreURIL
        (TVPbkContactStoreUriPtr aURI)
    {
    iImplementation->RemoveContactStoreURIL( aURI );
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::AddObserverL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::AddObserverL
        ( MPbk2StoreConfigurationObserver& aObserver )
    {
    iImplementation->AddObserverL( aObserver );
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::RemoveObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::RemoveObserver
        ( MPbk2StoreConfigurationObserver& aObserver )
    {
    iImplementation->RemoveObserver( aObserver );
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::AddDefaultSavingStoreObserverL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::AddDefaultSavingStoreObserverL
        ( MPbk2DefaultSavingStoreObserver& aObserver )
    {
    iImplementation->AddDefaultSavingStoreObserverL( aObserver );
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::RemoveDefaultSavingStoreObserver
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::RemoveDefaultSavingStoreObserver
        ( MPbk2DefaultSavingStoreObserver& aObserver )
    {
    iImplementation->RemoveDefaultSavingStoreObserver( aObserver );
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::AddSupportedContactStoreURIL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::AddSupportedContactStoreURIL
        ( TVPbkContactStoreUriPtr aURI )
    {
    iImplementation->AddSupportedContactStoreURIL( aURI );
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfiguration::RemoveSupportedContactStoreURIL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreConfiguration::RemoveSupportedContactStoreURIL
        ( TVPbkContactStoreUriPtr aURI )
    {
    iImplementation->RemoveSupportedContactStoreURIL( aURI );
    }
            
// End of File
