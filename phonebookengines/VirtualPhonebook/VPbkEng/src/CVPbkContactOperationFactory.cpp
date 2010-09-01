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
* Description:  An implementation of multioperation factory
*                that uses contact store list for creating suboperations
*                from stores
*
*/


#include <cvpbkcontactoperationfactory.h>

#include <CVPbkBatchOperation.h>
#include <CVPbkContactFindOperation.h>
#include <CVPbkContactFindFromStoresOperation.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CVPbkContactOperationFactory::CVPbkContactOperationFactory
// ---------------------------------------------------------------------------
//
CVPbkContactOperationFactory::CVPbkContactOperationFactory( 
        MVPbkContactStoreList& aStoreList )
        :   iStores( aStoreList )
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactOperationFactory::ConstructL
// ---------------------------------------------------------------------------
//
void CVPbkContactOperationFactory::ConstructL()
    {
    }

// ---------------------------------------------------------------------------
// CVPbkContactOperationFactory::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CVPbkContactOperationFactory* CVPbkContactOperationFactory::NewL( 
        MVPbkContactStoreList& aStoreList )
    {
    CVPbkContactOperationFactory* self = 
        new( ELeave ) CVPbkContactOperationFactory( aStoreList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CVPbkContactOperationFactory::~CVPbkContactOperationFactory
// ---------------------------------------------------------------------------
//
CVPbkContactOperationFactory::~CVPbkContactOperationFactory()
    {
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactOperationFactory::CreateContactRetrieverL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactOperationFactory::CreateContactRetrieverL(
        const MVPbkContactLink& aLink, 
        MVPbkSingleContactOperationObserver& aObserver )
    {
    // roll the link to each concrete store until one of them recognizes
    // the store part as his. This store then returns a contact or a null indicator.
    MVPbkContactOperation* operation = NULL;

    const TInt storeCount = iStores.Count();
    for ( TInt i = 0; i < storeCount; ++i )
        {
        operation = iStores.At(i).CreateContactRetrieverL( aLink, aObserver );
        if ( operation )
            {
            break;
            }
        }
                
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactOperationFactory::CreateDeleteContactsOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactOperationFactory::CreateDeleteContactsOperationL(
        const MVPbkContactLinkArray& aContactLinks, 
        MVPbkBatchOperationObserver& aObserver)
    {
    CVPbkBatchOperation* operation = CVPbkBatchOperation::NewLC( aObserver );

    const TInt storeCount = iStores.Count();
    for ( TInt i = 0; i < storeCount; ++i )
        {
        MVPbkContactOperation* subOperation = 
            iStores.At(i).CreateDeleteContactsOperationL( aContactLinks, 
            *operation );
        if ( subOperation )
            {
            CleanupDeletePushL( subOperation );
            operation->AddSubOperationL( subOperation );
            CleanupStack::Pop(); // subOperation
            }        
        }
    
    CleanupStack::Pop( operation );
    // If there are no suboperations then an empty operation is returned
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactOperationFactory::CreateCommitContactsOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactOperationFactory::CreateCommitContactsOperationL(
        const TArray<MVPbkStoreContact*>& aContacts,
        MVPbkBatchOperationObserver& aObserver )
    {
    CVPbkBatchOperation* operation = CVPbkBatchOperation::NewLC( aObserver );

    const TInt storeCount = iStores.Count();
    for ( TInt i = 0; i < storeCount; ++i )
        {
        MVPbkContactOperation* subOperation = 
            iStores.At(i).CreateCommitContactsOperationL( aContacts, 
            *operation );
        if ( subOperation )
            {
            CleanupDeletePushL( subOperation );
            operation->AddSubOperationL( subOperation );
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop( operation );
    // If there are no suboperations then an empty operation is returned
    return operation;    
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactOperationFactory::CreateMatchPhoneNumberOperationL
// ---------------------------------------------------------------------------
//
MVPbkContactOperation* CVPbkContactOperationFactory::CreateMatchPhoneNumberOperationL(
        const TDesC& aPhoneNumber, 
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver )
    {
    CVPbkContactFindOperation* operation = 
        CVPbkContactFindOperation::NewLC( aObserver );
    
    const TInt storeCount = iStores.Count();
    for ( TInt i = 0; i < storeCount; ++i )
        {
        MVPbkContactOperation* subOperation = 
            iStores.At(i).CreateMatchPhoneNumberOperationL( aPhoneNumber, 
            aMaxMatchDigits, *operation );
        if ( subOperation )
            {
            CleanupDeletePushL( subOperation );
            operation->AddSubOperationL( subOperation );
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop( operation );
    // If there are no suboperations then an empty operation is returned
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactOperationFactory::CreateFindOperationL
// ---------------------------------------------------------------------------
//    
MVPbkContactOperation* CVPbkContactOperationFactory::CreateFindOperationL(
        const TDesC& aSearchString, 
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver )
    {
    CVPbkContactFindOperation* operation = 
        CVPbkContactFindOperation::NewLC( aObserver );
    
    const TInt storeCount = iStores.Count();
    for (TInt i = 0; i < storeCount; ++i)
        {
        MVPbkContactOperation* subOperation = 
            iStores.At(i).CreateFindOperationL( aSearchString, aFieldTypes, 
                *operation );
        if ( subOperation )
            {
            CleanupDeletePushL( subOperation );
            operation->AddSubOperationL( subOperation );
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop( operation );
    // If there are no suboperations then an empty operation is returned
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactOperationFactory::CreateFindOperationL
// ---------------------------------------------------------------------------
//    
MVPbkContactOperation* CVPbkContactOperationFactory::CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack )
    {
    CVPbkContactFindFromStoresOperation* operation = 
        CVPbkContactFindFromStoresOperation::NewLC( aObserver );
    
    const TInt storeCount = iStores.Count();
    for (TInt i = 0; i < storeCount; ++i)
        {
        MVPbkContactOperation* subOperation = 
            iStores.At(i).CreateFindOperationL( aSearchStrings, aFieldTypes, 
                *operation, aWordParserCallBack );
        if ( subOperation )
            {
            CleanupDeletePushL( subOperation );
            operation->AddSubOperationL( subOperation );
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop( operation );
    // If there are no suboperations then an empty operation is returned
    return operation;
    }

MVPbkContactOperation* CVPbkContactOperationFactory::CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack,
                const CDesC16ArrayFlat& aStoreEntriesArray  )
    {
    CVPbkContactFindFromStoresOperation* operation = 
        CVPbkContactFindFromStoresOperation::NewLC( aObserver );
    
    const TInt storeCount = iStores.Count();
    for (TInt i = 0; i < storeCount; ++i)
        {
        
	        TInt pos = KErrNotFound;
	        TInt err = aStoreEntriesArray.Find( iStores.At(i).StoreProperties().Uri().UriDes(), pos );
	        
	        if ( err == KErrNone && pos != KErrNotFound )
	        {
		        MVPbkContactOperation* subOperation = 
		            iStores.At(i).CreateFindOperationL( aSearchStrings, aFieldTypes, 
		                *operation, aWordParserCallBack );
		        
		        if ( subOperation )
		            {
		            CleanupDeletePushL( subOperation );
		            operation->AddSubOperationL( subOperation );
		            CleanupStack::Pop(); // subOperation
		            }
	        }
		
        }
        
        
    CleanupStack::Pop( operation );
    // If there are no suboperations then an empty operation is returned
    return operation;
    }

// ---------------------------------------------------------------------------
// From class MVPbkContactOperationFactory
// CVPbkContactOperationFactory::CreateCompressStoresOperationL
// ---------------------------------------------------------------------------
//    
MVPbkContactOperation* CVPbkContactOperationFactory::CreateCompressStoresOperationL(
	    MVPbkBatchOperationObserver& aObserver )
	{
    CVPbkBatchOperation* operation = CVPbkBatchOperation::NewLC( aObserver );

    const TInt storeCount = iStores.Count();
    for ( TInt i = 0; i < storeCount; ++i )
        {
        MVPbkContactOperation* subOperation = 
            iStores.At(i).CreateCompressStoresOperationL( *operation );
        if ( subOperation )
            {
            CleanupDeletePushL( subOperation );
            operation->AddSubOperationL( subOperation );
            CleanupStack::Pop(); // subOperation
            }
        }

    CleanupStack::Pop( operation );
    // If there are no suboperations then an empty operation is returned
    return operation;
	}


TAny* CVPbkContactOperationFactory::ContactOperationFactoryExtension( TUid aExtensionUid )
	{
	if ( aExtensionUid == KMVPbkContactOperationFactory2Uid )
		{
		return static_cast<MVPbkContactOperationFactory2*>( this );
		}
	return NULL;
	}
