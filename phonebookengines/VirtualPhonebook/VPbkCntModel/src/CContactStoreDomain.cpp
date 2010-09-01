/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Maps a Contact Model databases to a virtual Phonebook contact.
*
*/


// INCLUDES
#include "CContactStoreDomain.h"

#include "CFieldTypeMap.h"
#include "CAsyncContactOperation.h"
#include "CContact.h"
#include "CContactLink.h"
#include "CContactView.h"
#include "CContactStore.h"

#include <CVPbkContactStoreList.h>
#include <cvpbkcontactoperationfactory.h>
#include <CVPbkContactStoreUri.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkAsyncOperation.h>
#include <MVPbkContactStoreObserver.h>
#include <RLocalizedResourceFile.h>
#include <VPbkUtil.h>
#include <VPbkDataCaging.hrh>
#include <VPbkCntModelRes.rsg>
#include <VPbkStoreUriLiterals.h>

#include <cntdb.h>
#include <cntitem.h>
#include <barsc.h>
#include <barsread.h>
#include <shareddataclient.h>

namespace VPbkCntModel {

// LOCAL
namespace {

// CONSTANTS
_LIT(KVPbkCntModelResFile, "VPbkCntModelRes.rsc");

}   // unnamed namespace

CContactStoreDomain::CContactStoreDomain(const MVPbkFieldTypeList& aMasterFieldTypeList) :
    iMasterFieldTypeList(aMasterFieldTypeList)
    {
    }

inline void CContactStoreDomain::ConstructL(
        const CVPbkContactStoreUriArray& aURIList,
        const TDesC& aURIDomain,
        CFieldTypeMap* aFieldTypeMap)
    {
    User::LeaveIfError(iFs.Connect());

    RProcess process;
    if ( process.HasCapability(ECapabilityReadDeviceData, ECapabilityWriteDeviceData, 
            __PLATSEC_DIAGNOSTIC_STRING("Virtual Phonebook client missing capabilities to \
            free disk space during compress/delete operations")) )
        {
    	// Shared data objects have to created after connecting RFs,
    	// but before reading resources    
        iSharedDataClient = new (ELeave) RSharedDataClient;
        User::LeaveIfError( iSharedDataClient->Connect() );    
        }

    if (!aFieldTypeMap)
        {
        iTextStore = new(ELeave) VPbkEngUtils::CTextStore;

        // load Versit field type mappings
        VPbkEngUtils::RLocalizedResourceFile resFile;

        resFile.OpenLC(iFs, KVPbkRomFileDrive, KDC_RESOURCE_FILES_DIR, KVPbkCntModelResFile);

        iFieldTypeMap = VPbkCntModel::CFieldTypeMap::NewL(iMasterFieldTypeList, *iTextStore);
        TResourceReader resReader;
        // initialize the field type map with versit name mapping data
        resReader.SetBuffer(resFile.AllocReadLC(R_VERSIT_NAME_FIELD_TYPE_MAPPINGS));
        iFieldTypeMap->InitVersitNameMappingsL(resReader);
        CleanupStack::PopAndDestroy();  // R_VERSIT_NAME_FIELD_TYPE_MAPPINGS buffer

        // initialize the field type map with versit parameter mapping data
        resReader.SetBuffer(resFile.AllocReadLC(R_VERSIT_PARAM_FIELD_TYPE_MAPPINGS));
        iFieldTypeMap->InitVersitParameterMappingsL(resReader);
        CleanupStack::PopAndDestroy();  // R_VERSIT_PARAM_FIELD_TYPE_MAPPINGS buffer

        // initialize the field type map with non versit field type mapping data
        resReader.SetBuffer(resFile.AllocReadLC(R_NONVERSIT_FIELD_TYPE_MAPPINGS));
        iFieldTypeMap->InitNonVersitMappingsL(resReader);
        CleanupStack::PopAndDestroy(2);
        }

    // Create the store list
    iContactStoreList = CVPbkContactStoreList::NewL();
    // Load all Contacts Model stores
    TVPbkContactStoreUriPtr defaultDb( KVPbkDefaultCntDbURI );
    const TPtrC domain = defaultDb.Component(
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType );
    const TInt uriCount = aURIList.Count();
    for (TInt i = 0; i < uriCount; ++i)
        {
        if ( aURIList[i].Compare( domain, 
            TVPbkContactStoreUriPtr::EContactStoreUriStoreType ) == 0 )
            {
            LoadContactStoreL( aURIList[i] );
            }
        }
    
    // Create multioperation factory
    iOperationFactory = CVPbkContactOperationFactory::NewL( 
        *iContactStoreList );
                
    iDomainDescriptor = aURIDomain.AllocL();

    // Take ownership of aFieldTypeMap in the end as described in the header.
    if (aFieldTypeMap)
        {
        iFieldTypeMap = aFieldTypeMap;
        }
    }

CContactStoreDomain* CContactStoreDomain::NewL
        (const CVPbkContactStoreUriArray& aURIList,
        const TDesC& aURIDomain,
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        CFieldTypeMap* aFieldTypeMap)
    {
    CContactStoreDomain* self = new(ELeave) CContactStoreDomain(aMasterFieldTypeList);
    CleanupStack::PushL(self);
    self->ConstructL(aURIList, aURIDomain, aFieldTypeMap);
    CleanupStack::Pop(self);
    return self;
    }

CContactStoreDomain* CContactStoreDomain::NewL(
        TAny* aParam)
    {
    TStoreDomainParam* param = static_cast<TStoreDomainParam*>(aParam);
    return NewL(param->iURIList, *param->iURIDomain, *param->iFieldTypeList, NULL);
    }

CContactStoreDomain::~CContactStoreDomain()
    {
    delete iDomainDescriptor;
    delete iOperationFactory;
    delete iContactStoreList;
    delete iFieldTypeMap;
    delete iTextStore;

    if ( iSharedDataClient )
        {
        iSharedDataClient->Close();
        }    
    delete iSharedDataClient;

    iFs.Close();
    }

const TDesC& CContactStoreDomain::UriDomain() const
    {
    return *iDomainDescriptor;
    }

void CContactStoreDomain::LoadContactStoreL(const TVPbkContactStoreUriPtr& aUri)
    {
    if (!Find(aUri))
        {
        CContactStore* store = CContactStore::NewL(aUri, *this);       
        CleanupStack::PushL(store);
        iContactStoreList->AppendL(store);
        CleanupStack::Pop(store);
        }
    }

TInt CContactStoreDomain::Count() const
    {
    return iContactStoreList->Count();
    }

MVPbkContactStore& CContactStoreDomain::At(TInt aIndex) const
    {
    return iContactStoreList->At(aIndex);
    }

MVPbkContactStore* CContactStoreDomain::Find(const TVPbkContactStoreUriPtr& aUri) const
    {
    return iContactStoreList->Find(aUri);
    }

void CContactStoreDomain::OpenAllL(MVPbkContactStoreListObserver& aObserver)
    {
    iContactStoreList->OpenAllL(aObserver);
    }

void CContactStoreDomain::CloseAll(MVPbkContactStoreListObserver& aObserver)
    {
    iContactStoreList->CloseAll(aObserver);
    }

MVPbkContactOperation* CContactStoreDomain::CreateContactRetrieverL
        (const MVPbkContactLink& aLink,
        MVPbkSingleContactOperationObserver& aObserver)
    {
    // test if aLink is in fact a instance of this domain
    MVPbkContactOperation* operation = NULL;
    if (CContactLink::Matches(*iDomainDescriptor, aLink,
            TVPbkContactStoreUriPtr::EContactStoreUriStoreType))
        {
        operation = iOperationFactory->CreateContactRetrieverL(aLink, aObserver);
        }
    return operation;
    }

MVPbkContactOperation* CContactStoreDomain::CreateDeleteContactsOperationL(
        const MVPbkContactLinkArray& aContactLinks,
        MVPbkBatchOperationObserver& aObserver)
    {
    return iOperationFactory->CreateDeleteContactsOperationL( aContactLinks, 
        aObserver);
    }

MVPbkContactOperation* CContactStoreDomain::CreateCommitContactsOperationL(
        const TArray<MVPbkStoreContact*>& aContacts,
        MVPbkBatchOperationObserver& aObserver)
    {
    return iOperationFactory->CreateCommitContactsOperationL( aContacts, 
        aObserver );
    }

MVPbkContactOperation* CContactStoreDomain::CreateMatchPhoneNumberOperationL(
        const TDesC& aPhoneNumber,
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver)
    {
    return iOperationFactory->CreateMatchPhoneNumberOperationL( aPhoneNumber,
        aMaxMatchDigits, aObserver );
    }

MVPbkContactOperation* CContactStoreDomain::CreateFindOperationL(
        const TDesC& aSearchString,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver)
    {
    return iOperationFactory->CreateFindOperationL( aSearchString, 
        aFieldTypes, aObserver );
    }

MVPbkContactOperation* CContactStoreDomain::CreateFindOperationL(
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver,
        const TCallBack& aWordParserCallBack )
    {
    return iOperationFactory->CreateFindOperationL( aSearchStrings,
        aFieldTypes, aObserver, aWordParserCallBack );
    }


TAny* CContactStoreDomain::ContactOperationFactoryExtension( TUid aExtensionUid )
	{
	if ( aExtensionUid == KMVPbkContactOperationFactory2Uid )
		{
		return static_cast<MVPbkContactOperationFactory2*>( this );
		}
	return NULL;
	}


MVPbkContactOperation* CContactStoreDomain::CreateFindOperationL(
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver,
        const TCallBack& aWordParserCallBack,
        const CDesC16ArrayFlat& aStoreEntriesArray )
    {
    return iOperationFactory->CreateFindOperationL( aSearchStrings,
        aFieldTypes, aObserver, aWordParserCallBack, aStoreEntriesArray );
    }

MVPbkContactOperation* CContactStoreDomain::CreateCompressStoresOperationL(
        MVPbkBatchOperationObserver& aObserver)
    {
    return iOperationFactory->CreateCompressStoresOperationL( aObserver );
    }

} // namespace VPbkCntModel
//End of file
