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
* Description:  The contact manager for accessing different stores.
*                Initialization: a client gives an array of store URIs (Virtual
*                offers some URIs, see VPbkStoreUriLiterals.h) when creating
*                a manager. The client uses store list to open all stores.
*
*/


// INCLUDES
#include "CVPbkContactManager.h"

// Virtual Phonebook
#include "CVPbkFieldType.h"
#include "CVPbkFieldTypeList.h"
#include "CVPbkContactStoreDomainList.h"
#include "CVPbkCompositeContactView.h"
#include "CVPbkCopyContactsOperation.h"
#include "CVPbkFoldingContactView.h"
#include "CVPbkContactAttributeManager.h"
#include "MVPbkContactStoreLoader.h"
#include "VPbkCompositeContactViewFactory.h"

#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkContactView.h>
#include <MVPbkContactOperation.h>
#include <vpbkeng.rsg>
#include <VPbkDataCaging.hrh>
#include <CVPbkContactLinkArray.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactCopier.h>
#include <MVPbkContactOperationFactory.h>
#include <MVPbkContactOperationFactory2.h>
// System includes
#include <s32buf.h>
#include <s32mem.h>
#include <barsc.h>
#include <bautils.h>
#include <barsread.h>
#include <ecom/ecom.h>
#include <RLocalizedResourceFile.h>

#ifdef _DEBUG
namespace {
    enum TPanic
        {
        EPreCond_CreateContactViewLC,
        EUnknownViewType_CreateContactViewLC,
        ENo_Factory_Extension_FindL
        };
        
    void Panic( TPanic aPanic )
        {
        _LIT(KPanicCat, "CVPbkContactManager");
        User::Panic( KPanicCat, aPanic );
        }       
}
#endif // _DEBUG


// --------------------------------------------------------------------------
// IsPlaceHolderCompositeView
// Returns ETrue if aViewDefinition is a composite that has no children
// --------------------------------------------------------------------------
//
TBool IsPlaceHolderCompositeView( 
        const CVPbkContactViewDefinition& aViewDefinition )
    {
    return ( aViewDefinition.Type() == EVPbkCompositeView &&
             aViewDefinition.SubViewCount() == 0 );
    }
    
/**
 * A loader class that uses CVPbkContactManager for loading stores
 */
NONSHARABLE_CLASS(CVPbkContactManager::CContactStoreLoader) 
    :   public CBase, 
        public MVPbkContactStoreLoader
    {
    public:
        CContactStoreLoader( CVPbkContactManager& aContactManager );
        
    // From base class MVPbkContactStoreLoader
        MVPbkContactStoreList& ContactStoreListL() const;
        void LoadContactStoreL( const TVPbkContactStoreUriPtr& aURI );
        
    private:
        CVPbkContactManager& iContactManager;
    };

CVPbkContactManager::CContactStoreLoader::CContactStoreLoader( 
        CVPbkContactManager& aContactManager )
        :   iContactManager( aContactManager )
    {
    }
    
MVPbkContactStoreList& 
        CVPbkContactManager::CContactStoreLoader::ContactStoreListL() const
    {
    return iContactManager.ContactStoresL();
    }
    
void CVPbkContactManager::CContactStoreLoader::LoadContactStoreL( 
        const TVPbkContactStoreUriPtr& aURI )
    {
    iContactManager.LoadContactStoreL( aURI );
    }
        
// --------------------------------------------------------------------------
// CVPbkContactManager::CVPbkContactManager
// --------------------------------------------------------------------------
//
inline CVPbkContactManager::CVPbkContactManager( TSecurityInfo aSecurityInfo, RFs* aFs)
: iSecurityInfo( aSecurityInfo )
    {
    if (aFs)
        {
        iFs = *aFs;
        }
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::~CVPbkContactManager
// --------------------------------------------------------------------------
//
CVPbkContactManager::~CVPbkContactManager()
    {
    delete iStoreLoader;
    delete iAttributeManager;
    delete iContactStoreDomains;
    delete iFieldTypes;
    iOwnFs.Close();
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkContactManager* CVPbkContactManager::NewL(
        const CVPbkContactStoreUriArray& aURIList,
        RFs* aFs /*= NULL*/)
    {
    TSecurityInfo secInfo;
    secInfo.SetToCurrentInfo();
    CVPbkContactManager* self = new(ELeave) CVPbkContactManager( secInfo, aFs);
    CleanupStack::PushL(self);
    self->ConstructL(aURIList);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkContactManager* CVPbkContactManager::NewL(
        TSecurityInfo aSecurityInfo,
        const CVPbkContactStoreUriArray& aURIList,
        RFs* aFs /*= NULL*/)
    {
    CVPbkContactManager* self = new(ELeave) CVPbkContactManager( aSecurityInfo, aFs);
    CleanupStack::PushL(self);
    self->ConstructL(aURIList);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::ConstructL
// --------------------------------------------------------------------------
//
inline void CVPbkContactManager::ConstructL
        (const CVPbkContactStoreUriArray& aURIList)
    {
    if (iFs.Handle() == KNullHandle)
        {
        User::LeaveIfError(iOwnFs.Connect());
        iFs = iOwnFs;
        }

    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC( iFs, KVPbkRomFileDrive,
        KDC_RESOURCE_FILES_DIR, KVPbkDefResFileName );
    HBufC8* fieldTypesBuf = resFile.AllocReadLC(R_VPBK_GLOBAL_FIELD_TYPES);
    TResourceReader resReader;
    resReader.SetBuffer(fieldTypesBuf);
    iFieldTypes = CVPbkFieldTypeList::NewL(resFile, resReader);
    CleanupStack::PopAndDestroy(2, &resFile);

    iContactStoreDomains =
        CVPbkContactStoreDomainList::NewL( aURIList, *iFieldTypes, iSecurityInfo );
    
    iStoreLoader = new( ELeave ) CContactStoreLoader( *this );
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::FieldTypes
// --------------------------------------------------------------------------
//
EXPORT_C const MVPbkFieldTypeList& CVPbkContactManager::FieldTypes() const
    {
    return *iFieldTypes;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::ContactStoresL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactStoreList& CVPbkContactManager::ContactStoresL()
    {
    return *iContactStoreDomains;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::LoadContactStoreL
// --------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactManager::LoadContactStoreL
        (const TVPbkContactStoreUriPtr& aURI)
    {
    if (!iContactStoreDomains->Find(aURI))
        {
        CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();
        uriList->AppendL(aURI);

        iContactStoreDomains->LoadPluginStoreDomainsL( *uriList, *iFieldTypes, 
                                                       iSecurityInfo );

        CleanupStack::PopAndDestroy(); // uriList
        }
    }
    
// --------------------------------------------------------------------------
// CVPbkContactManager::ContactAttributeManagerL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactAttributeManager&
        CVPbkContactManager::ContactAttributeManagerL()
    {
    if (!iAttributeManager)
        {
        iAttributeManager = CVPbkContactAttributeManager::NewL(*this);
        }

    return *iAttributeManager;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::CreateContactViewLC
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactViewBase* CVPbkContactManager::CreateContactViewLC(
        MVPbkContactViewObserver& aObserver,
        const CVPbkContactViewDefinition& aViewDefinition, 
        const MVPbkFieldTypeList& aSortOrder) const
    {
    MVPbkContactViewBase* result = NULL;
    
    switch ( aViewDefinition.Type() )
        {
        case EVPbkContactsView: // FALLTHROUGH
        case EVPbkGroupsView:
            {
            // Create a leaf view from the store plug-in.
            MVPbkContactStore* store = FindStore( aViewDefinition.Uri() );
            if ( !store )
                {
                User::Leave( KErrNotFound );
                }
            result = store->CreateViewLC( aViewDefinition, aObserver, 
                aSortOrder );
            break;
            }
        case EVPbkFoldingView:
            {
            // Create a folding view. Folding view can then
            // be expanded to its subviews.
            result = CVPbkFoldingContactView::NewLC( aObserver, 
                aViewDefinition, *this, aSortOrder ); 
            break;
            }
        case EVPbkCompositeView:
            {
            result = CreateOptimizedCompositeViewLC( aObserver,
                    aViewDefinition, aSortOrder );
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse, Panic ( EUnknownViewType_CreateContactViewLC ));
            CleanupStack::PushL( result ); // LC semantics
            break;
            }
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::RetrieveContactL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::RetrieveContactL(
        const MVPbkContactLink& aLink,
        MVPbkSingleContactOperationObserver& aObserver) const
    {
    MVPbkContactOperation* operation = 
        iContactStoreDomains->CreateContactRetrieverL( aLink, aObserver );
    
    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::CreateLinksLC
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLinkArray* CVPbkContactManager::CreateLinksLC(
        const TDesC8& aPackedLinks) const
    {
    RDesReadStream readStream( aPackedLinks );
    readStream.PushL();
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC(
        readStream, *iStoreLoader );
    CleanupStack::Pop( linkArray );
    CleanupStack::PopAndDestroy( &readStream );
    CleanupStack::PushL( linkArray );
    return linkArray;    
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::CreateLinksLC
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactLinkArray* CVPbkContactManager::CreateLinksLC(
        RReadStream& aStream) const
    {    
    CVPbkContactLinkArray* linkArray = CVPbkContactLinkArray::NewLC(
        aStream, *iStoreLoader );
    return linkArray;
    }
    
// --------------------------------------------------------------------------
// CVPbkContactManager::DeleteContactsL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::DeleteContactsL(
        const MVPbkContactLinkArray& aContactLinks, 
        MVPbkBatchOperationObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        iContactStoreDomains->CreateDeleteContactsOperationL(
            aContactLinks, aObserver);

    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }
    
    return operation;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::CommitContactsL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::CommitContactsL(
        const TArray<MVPbkStoreContact*>& aContacts, 
        MVPbkBatchOperationObserver& aObserver)
    {
    MVPbkContactOperation* operation =
        iContactStoreDomains->CreateCommitContactsOperationL(
            aContacts, aObserver);

    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }
    
    return operation;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::CopyContactsL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::CopyContactsL(
        const MVPbkContactLinkArray& aContactLinks,
        MVPbkContactStore* aTargetStore,
        MVPbkBatchOperationObserver& aObserver)
    {
    MVPbkContactOperation* operation = CVPbkCopyContactsOperation::NewLC(
            CVPbkContactCopier::EVPbkSimpleContactCopy,
            *this,
            aContactLinks,
            aTargetStore,
            NULL,
            aObserver);
    operation->StartL();
    CleanupStack::Pop(); // operation
    return operation;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::MatchPhoneNumberL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::MatchPhoneNumberL(
        const TDesC& aPhoneNumber, 
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* operation = 
        iContactStoreDomains->CreateMatchPhoneNumberOperationL(
            aPhoneNumber, aMaxMatchDigits, aObserver);

    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }
    
// --------------------------------------------------------------------------
// CVPbkContactManager::FindL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::FindL(
        const TDesC& aSearchString,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver)
    {
     MVPbkContactOperation* operation = 
     (static_cast<MVPbkContactOperationFactory*>(iContactStoreDomains))->CreateFindOperationL(
            aSearchString, aFieldTypes, aObserver); 

    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::FindL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::FindL(
        const MDesC16Array& aSearchStrings, 
        const MVPbkFieldTypeList& aFieldTypes, 
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack )
    {
     MVPbkContactOperation* operation = 
     (static_cast<MVPbkContactOperationFactory*>(iContactStoreDomains))->CreateFindOperationL(
            aSearchStrings, aFieldTypes, aObserver, aWordParserCallBack ); 
    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;
    }

EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::FindL(
        const MDesC16Array& aSearchStrings, 
        const MVPbkFieldTypeList& aFieldTypes, 
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack, const CDesC16ArrayFlat& aStoreEntriesArray )
 {
    MVPbkContactOperation* operation = NULL;
    
    MVPbkContactOperationFactory2* factoryExtension = reinterpret_cast<MVPbkContactOperationFactory2*>
    	(iContactStoreDomains->ContactOperationFactoryExtension(KMVPbkContactOperationFactory2Uid) );
    
    // Safeguard to avoid factory extension initialization. It is a programming error if 
    // factory extension does not exist.
    __ASSERT_DEBUG(factoryExtension, Panic( ENo_Factory_Extension_FindL ) );

    operation = 
        factoryExtension->CreateFindOperationL(
            aSearchStrings, aFieldTypes, aObserver, aWordParserCallBack ,aStoreEntriesArray );
 
    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }


    return operation;
    }
                
// --------------------------------------------------------------------------
// CVPbkContactManager::CompressStoresL
// --------------------------------------------------------------------------
//
EXPORT_C MVPbkContactOperationBase* CVPbkContactManager::CompressStoresL(
        		MVPbkBatchOperationObserver& aObserver)
	{
    MVPbkContactOperation* operation = 
    	iContactStoreDomains->CreateCompressStoresOperationL(aObserver);

    if (operation)
        {
        CleanupDeletePushL(operation);
        operation->StartL();
        CleanupStack::Pop(); // operation
        }

    return operation;	
	}

// --------------------------------------------------------------------------
// CVPbkContactManager::FsSession
// --------------------------------------------------------------------------
//
EXPORT_C RFs& CVPbkContactManager::FsSession()
    {
    return iFs;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::FindStore
// --------------------------------------------------------------------------
//
MVPbkContactStore* CVPbkContactManager::FindStore
        ( const TVPbkContactStoreUriPtr& aURI ) const
    {
    const TInt count = iContactStoreDomains->Count();
    for (TInt i = 0; i < count; ++i)
        {
        MVPbkContactStore& store = iContactStoreDomains->At(i);
        if (store.StoreProperties().Uri().Compare(aURI, 
                TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
            {
            return &store;
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::CreateOptimizedCompositeViewLC
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CVPbkContactManager::CreateOptimizedCompositeViewLC(
        MVPbkContactViewObserver& aObserver,
        const CVPbkContactViewDefinition& aViewDefinition, 
        const MVPbkFieldTypeList& aSortOrder) const
    {
    // First the placeholder composites must be reduced from the count
    // so that the actual number of subviews are known.
    TInt subViewCount = aViewDefinition.SubViewCount();
    TInt actualSubViewCount = subViewCount;
    for ( TInt i = 0; i < subViewCount; ++i )
        {
        if ( IsPlaceHolderCompositeView( aViewDefinition.SubViewAt( i ) ) )
            {
            --actualSubViewCount;
            }
        }
    
    MVPbkContactViewBase* result = NULL;
    const TInt oneSubview = 1;
    if ( actualSubViewCount == oneSubview )
        {
        // Create the one subview directly using aObserver.
        // Loop must be used to ignore placeholders.
        for ( TInt i = 0; i < subViewCount && !result; ++i )
            {
            result = CreateContactViewLC( aObserver, 
                aViewDefinition.SubViewAt( i ), aSortOrder );
            if ( !result )
                {
                CleanupStack::Pop( result );
                }
            }
        }
    else
        {
        // many subviews
        result = CreateCompositeViewLC( aObserver, aViewDefinition, 
                aSortOrder );
        }
    return result;
    }

// --------------------------------------------------------------------------
// CVPbkContactManager::CreateCompositeViewLC
// --------------------------------------------------------------------------
//
MVPbkContactViewBase* CVPbkContactManager::CreateCompositeViewLC(
        MVPbkContactViewObserver& aObserver,
        const CVPbkContactViewDefinition& aViewDefinition, 
        const MVPbkFieldTypeList& aSortOrder) const
    {
    MVPbkContactViewBase* result = NULL;
    const TInt subViewCount = aViewDefinition.SubViewCount();
    const TInt noSubviews = 0;
            
    if ( subViewCount == noSubviews )
        {
        // The view is used as a placeholder.
        // lets not create anything there
        CleanupStack::PushL( result ); // LC semantics
        }
    else
        {
        // Create a composite and its subviews
        CVPbkCompositeContactView* compositeView = 
            VPbkCompositeContactViewFactory::CreateViewLC(
                aObserver, aViewDefinition.SortPolicy(), 
                aSortOrder, *iFieldTypes, const_cast<RFs&>(iFs) );

        compositeView->SetViewId(aViewDefinition.Id());
        for ( TInt i = 0; i < subViewCount; ++i )
            {
            const CVPbkContactViewDefinition& subViewDef = 
                aViewDefinition.SubViewAt(i);
            if ( aViewDefinition.SortPolicy() == EVPbkSortedContactView &&
                 subViewDef.SortPolicy() == EVPbkUnsortedContactView )
                {
                // It's not possible to use unsorted subviews.
                // All subviews must be sorted so that they can be merged
                User::Leave(KErrArgument);
                }
            MVPbkContactViewBase* subView = CreateContactViewLC(
                *compositeView, subViewDef, aSortOrder );
            if (subView)
                {
                compositeView->AddSubViewL(subView, subViewDef.Id());
                if ( subView->Type() == EVPbkCompositeView )
                    {
                    // Composite creates another composite as its
                    // subview. The view can be safely casted because
                    // CVPbkCompositeContactView is VPbkEng internal
                    // and EVPbkCompositeView type view is always
                    // derived from CVPbkCompositeContactView.
                    static_cast<CVPbkCompositeContactView*>( 
                        subView )->ApplyInternalCompositePolicyL();
                    }
                }
            CleanupStack::Pop(); // subView
            }
        result = compositeView;        
        }
    return result;
    }
// End of File
