/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A list of store domains.
*
*/


/// 

// INCLUDES
#include "CVPbkContactStoreDomainList.h"
#include <CVPbkContactStoreDomain.h>
#include <MVPbkContactStoreListObserver.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkStoreUid.h>
#include <VPbkPrivateUid.h>
#include <VPbkError.h>
#include <MVPbkContactStore.h>
#include <CVPbkBatchOperation.h>
#include <CVPbkContactFindOperation.h>
#include <CVPbkContactLinkArray.h>
#include <MVPbkContactStoreProperties.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactFindOperation.h>
#include <CVPbkContactFindFromStoresOperation.h>
#include <CVPbkEmptySingleContactOperation.h>

namespace 
    {
    void CleanupResetAndDestroy(TAny* aObj)
        {
        if (aObj)
            {
            static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
            }
        }
    
    void OpenStoresOneByOneL( CVPbkContactStoreDomain& aStoreDomain, 
            MVPbkContactStoreObserver& aObserver )
        {
        const TInt storeCount = aStoreDomain.Count();
        for ( TInt i = 0; i < storeCount; ++i )
            {
            aStoreDomain.At( i ).OpenL( aObserver );
            }
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::CVPbkContactStoreDomainList
// ---------------------------------------------------------------------------
//
inline CVPbkContactStoreDomainList::CVPbkContactStoreDomainList() :
    iStoreDomains( 1 ) // granularity
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::ConstructL
// ---------------------------------------------------------------------------
//
inline void CVPbkContactStoreDomainList::ConstructL
        (const CVPbkContactStoreUriArray& aURIList, 
        const MVPbkFieldTypeList& aFieldTypeList,
        const TSecurityInfo& aSecurityInfo )
    {
    LoadPluginStoreDomainsL( aURIList, aFieldTypeList, aSecurityInfo );
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::NewL
// ---------------------------------------------------------------------------
//
CVPbkContactStoreDomainList* CVPbkContactStoreDomainList::NewL
        (const CVPbkContactStoreUriArray& aURIList, 
        const MVPbkFieldTypeList& aFieldTypeList,
        const TSecurityInfo& aSecurityInfo)
    {
    CVPbkContactStoreDomainList* self = new(ELeave) CVPbkContactStoreDomainList;
    CleanupStack::PushL(self);
    self->ConstructL(aURIList, aFieldTypeList, aSecurityInfo );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::~CVPbkContactStoreDomainList
// ---------------------------------------------------------------------------
//
CVPbkContactStoreDomainList::~CVPbkContactStoreDomainList()
    {
    delete iAsyncCallBack;
    iStoreDomains.ResetAndDestroy();
    iStoresToOpen.Close();
    // all plug-ins have been destroyed -> call FinalClose
    REComSession::FinalClose();
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::AppendL
// ---------------------------------------------------------------------------
//
void CVPbkContactStoreDomainList::AppendL(CVPbkContactStoreDomain* aStore)
    {
    iStoreDomains.AppendL(aStore);
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::DomainCount
// ---------------------------------------------------------------------------
//
TInt CVPbkContactStoreDomainList::DomainCount() const
    {
    return iStoreDomains.Count();
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::LoadPluginStoreDomainsL
// ---------------------------------------------------------------------------
//
void CVPbkContactStoreDomainList::LoadPluginStoreDomainsL(
        const CVPbkContactStoreUriArray& aURIList, 
        const MVPbkFieldTypeList& aFieldTypeList,
        const TSecurityInfo& aSecurityInfo )
    {
    RImplInfoPtrArray implementations;
    CleanupStack::PushL(
        TCleanupItem( CleanupResetAndDestroy, &implementations ) );

    TEComResolverParams resolverParams;    
    resolverParams.SetWildcardMatch(ETrue);
    _LIT8(KPbkExtension, KVPbkStorePluginDataString);
    resolverParams.SetDataType(KPbkExtension);
    
    REComSession::ListImplementationsL(
        TUid::Uid( KVPbkContactStoreDomainInterfaceUID ), resolverParams, 
        implementations );
        
    const TInt count = implementations.Count();
    for (TInt i = count-1; i >= 0; --i)
        {
        CImplementationInformation* implInfo = implementations[i];
        CleanupStack::PushL(implInfo);
        implementations.Remove(i);

        // associate URIs with plugin implementations
        LoadImplementationStoreDomainsL( aURIList, 
                                         *implInfo, 
                                         aFieldTypeList, 
                                         aSecurityInfo );
        CleanupStack::PopAndDestroy(implInfo);
        }
    CleanupStack::PopAndDestroy(); // implementations
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactStoreDomainList::CreateContactRetrieverL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactStoreDomainList::CreateContactRetrieverL(
        const MVPbkContactLink& aLink,
        MVPbkSingleContactOperationObserver& aObserver)
    {
    // roll the link to each concrete store until one of them recognizes
    // the store part as his. This store then returns a contact or a null indicator.
    MVPbkContactOperation* result = NULL;

    for (TInt i = 0; i < iStoreDomains.Count(); ++i)
        {
        result = iStoreDomains[i]->CreateContactRetrieverL( aLink, 
            aObserver );
        if ( result )
            {
            break;
            }
        }
        
    if ( !result )
        {
        // Create dummy async operation 
        result = CVPbkEmptySingleContactOperation::NewL( aObserver, KErrNotFound );
        }

    return result;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactStoreDomainList::CreateDeleteContactsOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactStoreDomainList::CreateDeleteContactsOperationL(
        const MVPbkContactLinkArray& aContactLinks, 
        MVPbkBatchOperationObserver& aObserver )
    {
    CVPbkBatchOperation* operation = CVPbkBatchOperation::NewLC( aObserver );
    
    const TInt domainCount = iStoreDomains.Count();
    for (TInt i = 0; i < domainCount; ++i)
        {
        MVPbkContactOperation* subOperation = iStoreDomains[i]->
            CreateDeleteContactsOperationL( aContactLinks, *operation );
        if (subOperation)
            {
            CleanupDeletePushL(subOperation);
            operation->AddSubOperationL(subOperation);
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop(operation);
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactStoreDomainList::CreateCommitContactsOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactStoreDomainList::CreateCommitContactsOperationL(
        const TArray<MVPbkStoreContact*>& aContacts,
        MVPbkBatchOperationObserver& aObserver )
    {
    CVPbkBatchOperation* operation = 
        CVPbkBatchOperation::NewLC( aObserver );
    
    const TInt domainCount = iStoreDomains.Count();
    for (TInt i = 0; i < domainCount; ++i)
        {
        MVPbkContactOperation* subOperation = 
            iStoreDomains[i]->CreateCommitContactsOperationL( aContacts, 
                *operation );
        if ( subOperation )
            {
            CleanupDeletePushL(subOperation);
            operation->AddSubOperationL(subOperation);
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop(operation);
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactStoreDomainList::CreateMatchPhoneNumberOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactStoreDomainList::CreateMatchPhoneNumberOperationL(
        const TDesC& aPhoneNumber, 
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkContactFindOperation* operation = 
        CVPbkContactFindOperation::NewLC( aObserver );
    
    const TInt domainCount = iStoreDomains.Count();
    for (TInt i = 0; i < domainCount; ++i)
        {
        MVPbkContactOperation* subOperation = 
            iStoreDomains[i]->CreateMatchPhoneNumberOperationL( aPhoneNumber, 
                aMaxMatchDigits, *operation );
        if ( subOperation )
            {
            CleanupDeletePushL( subOperation );
            operation->AddSubOperationL( subOperation );
            CleanupStack::Pop(); // subOperation
            }
        }
    
    CleanupStack::Pop(operation);
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactStoreDomainList::CreateFindOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactStoreDomainList::CreateFindOperationL(
        const TDesC& aSearchString,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver)
    {
    CVPbkContactFindOperation* operation = 
        CVPbkContactFindOperation::NewLC(aObserver);
    
    // If there are no field types, do not create find suboperations.
    if ( aFieldTypes.FieldTypeCount() > 0 )
        {
        const TInt domainCount = iStoreDomains.Count();
        for (TInt i = 0; i < domainCount; ++i)
            {
            MVPbkContactOperation* subOperation = 
                (static_cast<MVPbkContactOperationFactory*>(iStoreDomains[i]))->CreateFindOperationL(
                    aSearchString, aFieldTypes, *operation);
            if (subOperation)
                {
                CleanupDeletePushL(subOperation);
                operation->AddSubOperationL(subOperation);
                CleanupStack::Pop(); // subOperation
                }
            }        
        }
    
    CleanupStack::Pop(operation);
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactStoreDomainList::CreateFindOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactStoreDomainList::CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack )
    {
    CVPbkContactFindFromStoresOperation* operation = 
        CVPbkContactFindFromStoresOperation::NewLC( aObserver );
    
    // If there are no field types, do not create find suboperations.
    if ( aFieldTypes.FieldTypeCount() > 0 )
        {
        const TInt domainCount = iStoreDomains.Count();
        for (TInt i = 0; i < domainCount; ++i)
            {
            
            // added cast to avoid ambiguos access to CreateFindOperationL
            MVPbkContactOperation* subOperation = 
                (static_cast<MVPbkContactOperationFactory*>(iStoreDomains[i]))->CreateFindOperationL(
                    aSearchStrings, aFieldTypes, *operation, aWordParserCallBack );  
			
			if ( subOperation )
                {
                CleanupDeletePushL( subOperation );
                operation->AddSubOperationL( subOperation );
                CleanupStack::Pop(); // subOperation
                }
            }    
        }
        
    CleanupStack::Pop(operation);
    return operation;
    }


TAny* CVPbkContactStoreDomainList::ContactOperationFactoryExtension( TUid aExtensionUid )
	{
	if ( aExtensionUid == KMVPbkContactOperationFactory2Uid  )
		{
		return static_cast<MVPbkContactOperationFactory2*>(this);
		}
	return NULL;
	}

MVPbkContactOperation* CVPbkContactStoreDomainList::CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack, const CDesC16ArrayFlat& aStoreEntriesArray )
    {
    CVPbkContactFindFromStoresOperation* operation = 
        CVPbkContactFindFromStoresOperation::NewLC( aObserver );
    
    // If there are no field types, do not create find suboperations.
    if ( aFieldTypes.FieldTypeCount() > 0 )
        {
        const TInt domainCount = iStoreDomains.Count();
        for (TInt i = 0; i < domainCount; ++i)
            {
                    MVPbkContactOperation* subOperation = 
    	            (reinterpret_cast<MVPbkContactOperationFactory2*>
    	            	(iStoreDomains[i]->ContactOperationFactoryExtension( KMVPbkContactOperationFactory2Uid )))->
    	            		CreateFindOperationL(
        	            		aSearchStrings, aFieldTypes, *operation, aWordParserCallBack,aStoreEntriesArray ); 
            	if ( subOperation )
                	{
                	CleanupDeletePushL( subOperation );
                	operation->AddSubOperationL( subOperation );
                	CleanupStack::Pop(); // subOperation
                	}
             }    
        }
        
    CleanupStack::Pop(operation);
    return operation;
    }
// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactStoreDomainList::CreateCompressStoresOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactStoreDomainList::CreateCompressStoresOperationL(
        MVPbkBatchOperationObserver& aObserver)
	{
    CVPbkBatchOperation* operation = CVPbkBatchOperation::NewLC( aObserver );
    
    const TInt domainCount = iStoreDomains.Count();
    for (TInt i = 0; i < domainCount; ++i)
        {
        MVPbkContactOperation* subOperation = iStoreDomains[i]->
                            CreateCompressStoresOperationL(*operation);
        if (subOperation)
            {
            CleanupDeletePushL(subOperation);
            operation->AddSubOperationL(subOperation);
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop(operation);

    return operation;	
	}

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreDomainList::Count
// ---------------------------------------------------------------------------
//    
TInt CVPbkContactStoreDomainList::Count() const
    {
    TInt storeCount = 0;
    TInt domainCount = iStoreDomains.Count();
    for (TInt i = 0; i < domainCount; ++i)
        {
        storeCount += iStoreDomains[i]->Count();
        }
    return storeCount;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreDomainList::At
// ---------------------------------------------------------------------------
//    
MVPbkContactStore& CVPbkContactStoreDomainList::At(TInt aIndex) const
    {
    MVPbkContactStore* ret = NULL;
    TInt totalStores = 0;
    TInt domainCount = iStoreDomains.Count();
    // Make a conversion from universal store list index 
    // to domain[index]:storedomain[index]
    for (TInt i = 0; i < domainCount; ++i)
        {
        TInt storeCount = iStoreDomains[i]->Count();
        if (aIndex < totalStores + storeCount)
            {
            ret = &iStoreDomains[i]->At(aIndex - totalStores);
            break;
            }
        totalStores += storeCount;
        }

    __ASSERT_DEBUG(ret, VPbkError::Panic(VPbkError::EInvalidStoreIndex));
    return *ret;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreDomainList::Find
// ---------------------------------------------------------------------------
//    
MVPbkContactStore* CVPbkContactStoreDomainList::Find(
        const TVPbkContactStoreUriPtr& aUri ) const
    {
    TInt count = Count();
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkContactStore& store = At(i); 
        if (aUri.Compare(store.StoreProperties().Uri(), 
               TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
            {
            return &store;
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreDomainList::OpenAllL
// ---------------------------------------------------------------------------
//    
void CVPbkContactStoreDomainList::OpenAllL(
        MVPbkContactStoreListObserver& aObserver)
    {
    if (iListObserver)
        {
        User::Leave(KErrInUse);
        }

    iStoresToOpen.Reset();
    iListObserver = &aObserver;
    // Open all stores from all domains
    const TInt domainCount = iStoreDomains.Count();
    for (TInt domain = 0; domain < domainCount; ++domain)
        {
        CVPbkContactStoreDomain* storeDomain = iStoreDomains[domain];
        storeDomain->OpenAllL( *this );
        // OpenAllL can not be called for client because otherwise all the
        // domains would call OpenComplete for aObserver. OpenComplete must be
        // called only one time when all domains are ready.
        OpenStoresOneByOneL( *storeDomain, aObserver );
        const TInt count = storeDomain->Count();
        for (TInt i = 0; i < count; ++i)
            {
            // Save stores for counting callbacks            
            iStoresToOpen.AppendL(&storeDomain->At(i));
            }
        }
        
    if ( iStoresToOpen.Count() == 0 )
        {
        // If there is no stores to open run idle object once to complete
        // OpenAllL with OpenComplete.
        if ( !iAsyncCallBack )
            {
            iAsyncCallBack = CIdle::NewL( CActive::EPriorityStandard );
            }
        iAsyncCallBack->Start( TCallBack( AsyncSignalOpenComplete, this ) );
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreList
// CVPbkContactStoreDomainList::CloseAll
// ---------------------------------------------------------------------------
//    
void CVPbkContactStoreDomainList::CloseAll(
        MVPbkContactStoreListObserver& aObserver)
    {
    const TInt domainCount = iStoreDomains.Count();
    if ( iListObserver == &aObserver )
        {
        // The observer is the same that called OpenAll -> the list must
        // stop listening to stores
        for (TInt domain = 0; domain < domainCount; ++domain)
            {
            CVPbkContactStoreDomain* storeDomain = iStoreDomains[domain];
            storeDomain->CloseAll( *this );
            storeDomain->CloseAll( aObserver );
            }
        iStoresToOpen.Reset();
        iListObserver = NULL;
        }
    else
        {
        // The observer is not the same that called OpenAll -> call CloseAll
        // for stores for that observer, but don't stop the opening 
        // because the observer that called OpenAll is still expecting the
        // OpenComplete event
        // 
        for (TInt domain = 0; domain < domainCount; ++domain)
            {
            CVPbkContactStoreDomain* storeDomain = iStoreDomains[domain];
            storeDomain->CloseAll( aObserver );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver
// CVPbkContactStoreDomainList::StoreReady
// ---------------------------------------------------------------------------
//    
void CVPbkContactStoreDomainList::StoreReady(MVPbkContactStore& aContactStore)
    {
    // Do not call iListObserver->StoreReady() because 
    // it receives the notification directly
    TInt index = iStoresToOpen.Find(&aContactStore);
    if (index != KErrNotFound)
        {
        iStoresToOpen.Remove(index);
        }

    if (iStoresToOpen.Count() == 0)
        {
        SignalOpenComplete();
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver
// CVPbkContactStoreDomainList::StoreUnavailable
// ---------------------------------------------------------------------------
//    
void CVPbkContactStoreDomainList::StoreUnavailable(
    MVPbkContactStore& aContactStore, TInt /*aReason*/)
    {
    // Do not call iListObserver->StoreUnavailable() because 
    // it receives the notification directly
    TInt index = iStoresToOpen.Find(&aContactStore);
    if (index != KErrNotFound)
        {
        iStoresToOpen.Remove(index);
        }

    if (iStoresToOpen.Count() == 0)
        {
        SignalOpenComplete();
        }
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver
// CVPbkContactStoreDomainList::HandleStoreEventL
// ---------------------------------------------------------------------------
//    
void CVPbkContactStoreDomainList::HandleStoreEventL(
        MVPbkContactStore& /*aContactStore*/, 
        TVPbkContactStoreEvent /*aStoreEvent*/)
    {
    // Only opening stores is interesting for this class
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver
// CVPbkContactStoreDomainList::OpenComplete
// ---------------------------------------------------------------------------
//    
void CVPbkContactStoreDomainList::OpenComplete()
    {
    // The needed information is gathered in StoreReady and StoreUnavailable
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::LoadImplementationStoreDomainsL
// ---------------------------------------------------------------------------
//
void CVPbkContactStoreDomainList::LoadImplementationStoreDomainsL(
        const CVPbkContactStoreUriArray& aURIList, 
        const CImplementationInformation& aImplInfo,
        const MVPbkFieldTypeList& aFieldTypeList, 
        const TSecurityInfo& aSecurityInfo )
    {
    // create a copy of the ECom opaque data descriptor
    const TInt length = aImplInfo.OpaqueData().Length();
    HBufC* opaqueBuf = HBufC::NewLC(length);
    TPtr opaque = opaqueBuf->Des();
    opaque.Copy(aImplInfo.OpaqueData()); 
    
    for (TInt i = 0; i < aURIList.Count(); ++i)
        {
        TVPbkContactStoreUriPtr uri = aURIList[i];
        const TPtrC domain = uri.Component(
                TVPbkContactStoreUriPtr::EContactStoreUriStoreType);

        // check if aImplInfo has the same domain as this one
        if (!opaque.CompareF(domain))
            {
            // check if domain already exists
            CVPbkContactStoreDomain* storeDomain = FindDomain(opaque);
            if (storeDomain)
                {
                // domain was already loaded -> just create a new store there
                storeDomain->LoadContactStoreL(uri);
                }
            else
                {
                // domain was not already loaded
                CVPbkContactStoreDomain::TStoreDomainParam param(aURIList, aSecurityInfo );
                param.iURIDomain = &opaque;
                param.iFieldTypeList = &aFieldTypeList;

                CVPbkContactStoreDomain* store = CVPbkContactStoreDomain::NewL
                    (aImplInfo.ImplementationUid(), param);
                CleanupStack::PushL(store);
                AppendL(store);
                CleanupStack::Pop(store);                
                }
            }            
        }
    CleanupStack::PopAndDestroy(opaqueBuf);
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::FindDomain
// ---------------------------------------------------------------------------
//
CVPbkContactStoreDomain* CVPbkContactStoreDomainList::FindDomain(
        const TDesC& aStoreDomain ) const
    {
    CVPbkContactStoreDomain* result = NULL;
    for (TInt i = 0; i < iStoreDomains.Count(); ++i)
        {
        if (!aStoreDomain.CompareF(iStoreDomains[i]->UriDomain()))
            {
            result = iStoreDomains[i];
            break;
            }
        }

    return result;
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::SignalOpenComplete
// ---------------------------------------------------------------------------
//    
void CVPbkContactStoreDomainList::SignalOpenComplete()
    {
    if ( iListObserver )
        {
        MVPbkContactStoreListObserver* observer = iListObserver;
        iListObserver = NULL;
        iStoresToOpen.Reset();
        // The list is not interested about the contact store events anymore
        CloseAll( *this );
        observer->OpenComplete();
        }
    }

// ---------------------------------------------------------------------------
// CVPbkContactStoreDomainList::AsyncSignalOpenComplete
// ---------------------------------------------------------------------------
//    
TInt CVPbkContactStoreDomainList::AsyncSignalOpenComplete( 
    TAny* aDomainList )
    {
    ((CVPbkContactStoreDomainList*)aDomainList)->SignalOpenComplete();
    return EFalse; // This is called just once
    }
    
// end of file



