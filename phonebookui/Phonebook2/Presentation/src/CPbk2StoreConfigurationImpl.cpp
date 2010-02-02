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


// INCLUDE FILES
#include "CPbk2StoreConfigurationImpl.h"

// Phonebook2
#include "CPbk2StoreConfigurationMonitor.h"
#include "CPbk2DefaultSavingStoreMonitor.h"
#include <Phonebook2InternalCRKeys.h>

// Virtual Phonebook
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactStoreUriArray.h>

// System includes
#include <centralrepository.h>

// Debugging headers
#include <Pbk2Debug.h>

// Unnamed namespace for local definitions
namespace {

// Definition for contact initial contact store uri size
const TInt KInitialStoreUriSize = 1;

} // namespace


// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::CPbk2StoreConfigurationImpl
// --------------------------------------------------------------------------
//
CPbk2StoreConfigurationImpl::CPbk2StoreConfigurationImpl()
    {
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::~CPbk2StoreConfigurationImpl
// --------------------------------------------------------------------------
//
CPbk2StoreConfigurationImpl::~CPbk2StoreConfigurationImpl()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::~CPbk2StoreConfigurationImpl(0x%x)"), this);

    iObservers.Reset();
    iDefaultStoreObservers.Reset();
    delete iDefaultSavingStoreMonitor;
    delete iConfigurationMonitor;
    delete iRepository;
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::NewL
// --------------------------------------------------------------------------
//
CPbk2StoreConfigurationImpl* CPbk2StoreConfigurationImpl::NewL()
    {
    CPbk2StoreConfigurationImpl* self = new(ELeave) CPbk2StoreConfigurationImpl;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::ConstructL()
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::ConstructL(0x%x)"), this);

    iRepository = CRepository::NewL
        (TUid::Uid(KCRUidPhonebookStoreConfiguration));

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::ConstructL(0x%x) Repository created"), this);

    iConfigurationMonitor = CPbk2StoreConfigurationMonitor::NewL
        ( *iRepository, *this );

    iDefaultSavingStoreMonitor = CPbk2DefaultSavingStoreMonitor::NewL
        ( *iRepository, *this );

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::ConstructL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::CurrentConfigurationL
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray*
        CPbk2StoreConfigurationImpl::CurrentConfigurationL() const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::CurrentConfigurationL(0x%x)"), this);

    CVPbkContactStoreUriArray* result = CVPbkContactStoreUriArray::NewLC();

    RArray<TUint32> configurationKeys;
    CleanupClosePushL(configurationKeys);

    iRepository->FindL(KPhonebookCurrentConfigurationPartialKey,
            KPhonebookCurrentConfigurationMask,
            configurationKeys);

    HBufC* buffer = HBufC::NewLC(KInitialStoreUriSize);
    const TInt keyCount = configurationKeys.Count();
    for (TInt i = 0; i < keyCount; ++i)
        {
        TPtr ptr = buffer->Des();
        ptr.Zero();
        TInt actualSize = 0;
        TInt ret = iRepository->Get(configurationKeys[i], ptr, actualSize);
        if (ret == KErrOverflow)
            {
            CleanupStack::PopAndDestroy(); // buffer
            buffer = HBufC::NewLC(actualSize);
            ptr.Set(buffer->Des());
            User::LeaveIfError(iRepository->Get(configurationKeys[i], ptr));
            }
        else
            {
            User::LeaveIfError(ret);
            }
        if( !result->IsIncluded( TVPbkContactStoreUriPtr( ptr ) ) )  // Only append if the uri is not yet included
            {
            result->AppendL( ptr );
            }

        }
    CleanupStack::PopAndDestroy(); // buffer

    CleanupStack::PopAndDestroy(); // configurationKeys
    CleanupStack::Pop(result); // result

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::CurrentConfigurationL(0x%x) end"), this);

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::SearchStoreConfigurationL
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray*
        CPbk2StoreConfigurationImpl::SearchStoreConfigurationL()
    {
    CVPbkContactStoreUriArray* result = CVPbkContactStoreUriArray::NewLC();

    // The default saving store shall be appended to the result array
    // as an first element, when it becomes available

    // Finally append the current Phonebook configuration for searching
    CVPbkContactStoreUriArray* currentConfiguration = CurrentConfigurationL();
    CleanupStack::PushL(currentConfiguration);
    result->AppendAllL(*currentConfiguration);
    CleanupStack::PopAndDestroy(currentConfiguration);
    CleanupStack::Pop(result);
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::SupportedStoreConfigurationL
// --------------------------------------------------------------------------
//
CVPbkContactStoreUriArray*
        CPbk2StoreConfigurationImpl::SupportedStoreConfigurationL() const
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::SupportedStoreConfigurationL(0x%x)"), this);

    CVPbkContactStoreUriArray* result = CVPbkContactStoreUriArray::NewLC();

    RArray<TUint32> configurationKeys;
    CleanupClosePushL( configurationKeys );

    User::LeaveIfError( iRepository->FindL(
                            KPhonebookSupportedConfigurationPartialKey,
                            KPhonebookSupportedConfigurationMask,
                            configurationKeys ) );

    HBufC* buffer = HBufC::NewLC(KInitialStoreUriSize);
    const TInt keyCount = configurationKeys.Count();
    for (TInt i = 0; i < keyCount; ++i)
        {
        TPtr ptr = buffer->Des();
        ptr.Zero();
        TInt actualSize = 0;
        TInt ret = iRepository->Get(configurationKeys[i], ptr, actualSize);
        if (ret == KErrOverflow)
            {
            CleanupStack::PopAndDestroy(); // buffer
            buffer = HBufC::NewLC(actualSize);
            ptr.Set(buffer->Des());
            User::LeaveIfError(iRepository->Get(configurationKeys[i], ptr));
            }
        else
            {
            User::LeaveIfError(ret);
            }
        result->AppendL(ptr);
        }
    CleanupStack::PopAndDestroy(); // buffer

    CleanupStack::PopAndDestroy(); // configurationKeys
    CleanupStack::Pop(result); // result

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::SupportedStoreConfigurationL(0x%x) end"), this);

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::DefaultSavingStoreL
// --------------------------------------------------------------------------
//
TVPbkContactStoreUriPtr
        CPbk2StoreConfigurationImpl::DefaultSavingStoreL() const
    {
    return iDefaultSavingStoreMonitor->DefaultSavingStoreL();
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::AddContactStoreURIL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::AddContactStoreURIL
        (TVPbkContactStoreUriPtr aURI)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::AddContactStoreURIL(0x%x)"), this);

    RArray<TUint32> configurationKeys;
    CleanupClosePushL(configurationKeys);

    iRepository->FindL(
            KPhonebookCurrentConfigurationPartialKey,
            KPhonebookCurrentConfigurationMask,
            configurationKeys);

    if ( AlreadyRegisteredL( aURI, configurationKeys ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    // Find the first free key starting from
    // KPhonebookCurrentConfigurationFirstKey
    TBool keyFound = EFalse;
    TUint32 newKey = KPhonebookCurrentConfigurationFirstKey;
    for (TInt i = KPhonebookCurrentConfigurationFirstKey;
        i < KPhonebookCurrentConfigurationLastKey && !keyFound; ++i)
        {
        if (configurationKeys.Find(i) == KErrNotFound)
            {
            newKey = i;
            keyFound = ETrue;
            }
        }

    if (!keyFound)
        {
        User::Leave(KErrOverflow);
        }

    TInt err = iRepository->Create(newKey, aURI.UriDes());
    User::LeaveIfError(err);

    CleanupStack::PopAndDestroy(); // configurationKeys

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::AddContactStoreURIL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::RemoveContactStoreURIL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::RemoveContactStoreURIL
        (TVPbkContactStoreUriPtr aURI)
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::RemoveContactStoreURIL(0x%x)"), this);

    RArray<TUint32> configurationKeys;
    CleanupClosePushL(configurationKeys);

    iRepository->FindEqL(
            KPhonebookCurrentConfigurationPartialKey,
            KPhonebookCurrentConfigurationMask,
            aURI.UriDes(),
            configurationKeys);

    const TInt keyCount = configurationKeys.Count();
    for (TInt i = 0; i < keyCount; ++i)
        {
        iRepository->Delete(configurationKeys[i]);
        }
    CleanupStack::PopAndDestroy(); // configurationKeys

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::RemoveContactStoreURIL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::AddObserverL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::AddObserverL
        (MPbk2StoreConfigurationObserver& aObserver)
    {
     iObservers.Append( &aObserver );
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::RemoveObserver
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::RemoveObserver
        (MPbk2StoreConfigurationObserver& aObserver)
    {
    TInt index = iObservers.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::AddDefaultSavingStoreObserverL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::AddDefaultSavingStoreObserverL
        ( MPbk2DefaultSavingStoreObserver& aObserver )
    {
    TInt index = iDefaultStoreObservers.Find(&aObserver);
    if (index == KErrNotFound)
        {
        // Insert to the beginning so that the observers will get the
        // events in the order they have registere
        // This has to be done because events are sent from last to first
        // in the ConfigurationChanged function
        iDefaultStoreObservers.InsertL(&aObserver, 0);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::RemoveDefaultSavingStoreObserver
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::RemoveDefaultSavingStoreObserver
        ( MPbk2DefaultSavingStoreObserver& aObserver )
    {
    TInt index = iDefaultStoreObservers.Find(&aObserver);
    if (index != KErrNotFound)
        {
        iDefaultStoreObservers.Remove(index);
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::AddSupportedContactStoreURIL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::AddSupportedContactStoreURIL
        ( TVPbkContactStoreUriPtr aURI )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::AddSupportedContactStoreURIL(0x%x)"), this);

    RArray<TUint32> configurationKeys;
    CleanupClosePushL(configurationKeys);

    iRepository->FindL(
            KPhonebookSupportedConfigurationPartialKey,
            KPhonebookSupportedConfigurationMask,
            configurationKeys);

    if ( AlreadyRegisteredL( aURI, configurationKeys ) )
        {
        User::Leave( KErrAlreadyExists );
        }

    // Find the first free key starting from
    // KPhonebookSupportedConfigurationFirstKey
    TBool keyFound = EFalse;
    TUint32 newKey = KPhonebookSupportedConfigurationFirstKey;
    for (TInt i = KPhonebookSupportedConfigurationFirstKey;
        i < KPhonebookSupportedConfigurationLastKey && !keyFound; ++i)
        {
        if (configurationKeys.Find(i) == KErrNotFound)
            {
            newKey = i;
            keyFound = ETrue;
            }
        }

    if (!keyFound)
        {
        User::Leave(KErrOverflow);
        }

    TInt err = iRepository->Create(newKey, aURI.UriDes());
    User::LeaveIfError(err);

    CleanupStack::PopAndDestroy(); // configurationKeys

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::AddSupportedContactStoreURIL(0x%x) end"), this);
    }
    
// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::RemoveSupportedContactStoreURIL
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::RemoveSupportedContactStoreURIL
        ( TVPbkContactStoreUriPtr aURI )
    {
    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::RemoveSupportedContactStoreURIL(0x%x)"), this);

    RArray<TUint32> configurationKeys;
    CleanupClosePushL(configurationKeys);

    iRepository->FindEqL(
            KPhonebookSupportedConfigurationPartialKey,
            KPhonebookSupportedConfigurationMask,
            aURI.UriDes(),
            configurationKeys);

    const TInt keyCount = configurationKeys.Count();
    for (TInt i = 0; i < keyCount; ++i)
        {
        iRepository->Delete(configurationKeys[i]);
        }
    CleanupStack::PopAndDestroy(); // configurationKeys

    PBK2_DEBUG_PRINT(PBK2_DEBUG_STRING
        ("CPbk2StoreConfigurationImpl::RemoveSupportedContactStoreURIL(0x%x) end"), this);
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::ConfigurationChanged
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::ConfigurationChanged()
    {
    for (TInt i = iObservers.Count() - 1; i >= 0; --i)
        {
        iObservers[i]->ConfigurationChanged();
        }

    for (TInt i = iObservers.Count() - 1; i >= 0; --i)
        {
        iObservers[i]->ConfigurationChangedComplete();
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::ConfigurationChangedComplete
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::ConfigurationChangedComplete()
    {
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::SavingStoreChanged
// --------------------------------------------------------------------------
//
void CPbk2StoreConfigurationImpl::SavingStoreChanged()
    {
    for (TInt i = iDefaultStoreObservers.Count() - 1; i >= 0; --i)
        {
        iDefaultStoreObservers[i]->SavingStoreChanged();
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreConfigurationImpl::AlreadyRegisteredL
// --------------------------------------------------------------------------
//
TBool CPbk2StoreConfigurationImpl::AlreadyRegisteredL
        ( TVPbkContactStoreUriPtr aURI, RArray<TUint32> aKeys )
    {
    TBool already = EFalse;

    HBufC* buffer = HBufC::NewLC(KInitialStoreUriSize);
    const TInt keyCount = aKeys.Count();
    for (TInt j = 0; j < keyCount; ++j)
        {
        TPtr ptr = buffer->Des();
        ptr.Zero();
        TInt actualSize = 0;
        TInt ret = iRepository->Get(aKeys[j], ptr, actualSize);
        if (ret == KErrOverflow)
            {
            CleanupStack::PopAndDestroy(); // buffer
            buffer = HBufC::NewLC(actualSize);
            ptr.Set(buffer->Des());
            User::LeaveIfError(iRepository->Get(aKeys[j], ptr));
            }

        TVPbkContactStoreUriPtr uri( *buffer );
        if ( uri.Compare( aURI,
            TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) == 0 )
            {
            already = ETrue;
            break;
            }
        }

    CleanupStack::PopAndDestroy(); // buffer

    return already;
    }
    
// End of File
