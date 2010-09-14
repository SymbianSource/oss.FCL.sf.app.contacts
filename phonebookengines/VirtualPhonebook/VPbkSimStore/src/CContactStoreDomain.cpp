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
* Description:  Implements the sim store domain
*
*/



// INCLUDE FILES
#include "CContactStoreDomain.h"

#include "CFieldTypeMappings.h"
#include "CContactStore.h"

#include <CVPbkContactStoreList.h>
#include <cvpbkcontactoperationfactory.h>
#include <CVPbkContactStoreUriArray.h>
#include <MVPbkContactStore.h>
#include <RLocalizedResourceFile.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkDataCaging.hrh>
#include <vpbksimstoreres.rsg>
#include <VPbkStoreUriLiterals.h>
#include <VPbkUtil.h>

#include <barsc.h>
#include <barsread.h>
#include <featmgr.h>

namespace VPbkSimStore {

// CONSTANTS
_LIT(KResFile, "VPbkSimStoreRes.rsc");


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactStoreDomain::CContactStoreDomain
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CContactStoreDomain::CContactStoreDomain( 
    const MVPbkFieldTypeList& aMasterFieldTypeList, 
    const TSecurityInfo& aSecurityInfo )
    : iMasterFieldTypeList( aMasterFieldTypeList ), 
      iSecurityInfo( aSecurityInfo )
    {
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::ConstructL( 
    const CVPbkContactStoreUriArray& aURIList, const TDesC& aURIDomain )
    {
    iDomainDescriptor = aURIDomain.AllocL();

    // Load Versit field type mappings
    User::LeaveIfError( iFs.Connect() );
    
    // Create text store for versit property extension names
    iTextStore = new(ELeave) VPbkEngUtils::CTextStore;
    // Create the field type mapper
    iFieldTypeMappings = CFieldTypeMappings::NewL( iMasterFieldTypeList, *
        iTextStore );
    
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC( iFs, KVPbkRomFileDrive, KDC_RESOURCE_FILES_DIR, KResFile );
    FeatureManager::InitializeLibL();
    TResourceReader resReader;
    
    if ( !FeatureManager::FeatureSupported(
            KFeatureIdFfTdClmcontactreplicationfromphonebooktousimcard ) )
        {
        resReader.SetBuffer( resFile.AllocReadLC(
                R_VPBK_VERSIT_FIELD_TYPE_MAPPINGS ) );
        }
    else
        {
        resReader.SetBuffer( resFile.AllocReadLC(
                R_VPBK_USIM_VERSIT_FIELD_TYPE_MAPPINGS ) );
        }
    // Read versit types
    iFieldTypeMappings->InitVersitMappingsL( resReader );
    resReader.SetBuffer( resFile.AllocReadLC( 
        R_VPBK_NONVERSIT_FIELD_TYPE_MAPPINGS ) );
    // Read non versit types
    iFieldTypeMappings->InitNonVersitMappingsL( resReader );
    //R_VPBK_NONVERSIT_FIELD_TYPE_MAPPINGS, resFile, iFs
    CleanupStack::PopAndDestroy( 3 );

    // Create the store list
    iContactStoreList = CVPbkContactStoreList::NewL();
    // Load all SIM stores
    TVPbkContactStoreUriPtr defaultDb( KVPbkSimGlobalAdnURI );
    const TPtrC domain = defaultDb.Component(
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType );
    const TInt uriCount = aURIList.Count();
    for ( TInt i = 0; i < uriCount; ++i )
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
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreDomain* CContactStoreDomain::NewL
    ( const CVPbkContactStoreUriArray& aURIList,
    const TDesC& aURIDomain,
    const MVPbkFieldTypeList& aMasterFieldTypeList,
    const TSecurityInfo& aSecurityInfo )
    {
    CContactStoreDomain* self = 
        new( ELeave ) CContactStoreDomain( aMasterFieldTypeList, aSecurityInfo );
    CleanupStack::PushL( self );
    self->ConstructL( aURIList, aURIDomain );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreDomain* CContactStoreDomain::NewL( TAny* aParam )
    {
    TStoreDomainParam* param = static_cast<TStoreDomainParam*>( aParam );
    return NewL( param->iURIList, *param->iURIDomain, 
                 *param->iFieldTypeList, param->iSecurityInfo );
    }

// Destructor
CContactStoreDomain::~CContactStoreDomain()
    {
    delete iDomainDescriptor;
    delete iFieldTypeMappings;
    delete iOperationFactory;
    delete iContactStoreList;
    delete iTextStore;
    iFs.Close();
    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::UriDomain
// -----------------------------------------------------------------------------
//
const TDesC& CContactStoreDomain::UriDomain() const
    {
    return *iDomainDescriptor;
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::Count
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::LoadContactStoreL( 
        const TVPbkContactStoreUriPtr& aUri )
    {
    if ( !iContactStoreList->Find(aUri) )
        {
        CContactStore* store = CContactStore::NewL( aUri, *this );
        CleanupStack::PushL( store );
        iContactStoreList->AppendL( store );
        CleanupStack::Pop( store );
        }
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::Count
// -----------------------------------------------------------------------------
//
TInt CContactStoreDomain::Count() const
    {
    return iContactStoreList->Count();
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::At
// -----------------------------------------------------------------------------
//
MVPbkContactStore& CContactStoreDomain::At( TInt aIndex ) const
    {
    return iContactStoreList->At( aIndex );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::Find
// -----------------------------------------------------------------------------
//
MVPbkContactStore* CContactStoreDomain::Find(
        const TVPbkContactStoreUriPtr& aUri ) const
    {
    return iContactStoreList->Find( aUri );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::OpenAllL
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::OpenAllL( MVPbkContactStoreListObserver& aObserver )
    {
    iContactStoreList->OpenAllL( aObserver );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CloseAll
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::CloseAll( MVPbkContactStoreListObserver& aObserver )
    {
    iContactStoreList->CloseAll( aObserver );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateContactRetrieverL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateContactRetrieverL( 
    const MVPbkContactLink& aLink, 
    MVPbkSingleContactOperationObserver& aObserver )
    {
    return iOperationFactory->CreateContactRetrieverL( aLink, aObserver );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateDeleteContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateDeleteContactsOperationL(
    const MVPbkContactLinkArray& aContactLinks, 
    MVPbkBatchOperationObserver& aObserver )
    {
    return iOperationFactory->CreateDeleteContactsOperationL( aContactLinks, 
        aObserver );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateCommitContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateCommitContactsOperationL(
    const TArray<MVPbkStoreContact*>& aContacts,
    MVPbkBatchOperationObserver& aObserver )
    {
    return iOperationFactory->CreateCommitContactsOperationL(
        aContacts, aObserver );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateMatchPhoneNumberOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateMatchPhoneNumberOperationL(
    const TDesC& aPhoneNumber,
    TInt aMaxMatchDigits,
    MVPbkContactFindObserver& aObserver )
    {
    return iOperationFactory->CreateMatchPhoneNumberOperationL( aPhoneNumber, 
        aMaxMatchDigits, aObserver );
    }
    
// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateMatchPhoneNumberOperationL
// -----------------------------------------------------------------------------
//  
MVPbkContactOperation* CContactStoreDomain::CreateFindOperationL(
        const TDesC& aSearchString,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver)
    {
    return iOperationFactory->CreateFindOperationL( aSearchString, aFieldTypes,
        aObserver );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateFindOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateFindOperationL(
        const MDesC16Array& aSearchStrings, 
        const MVPbkFieldTypeList& aFieldTypes, 
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack )
    {
    return iOperationFactory->CreateFindOperationL( aSearchStrings, aFieldTypes,
        aObserver, aWordParserCallBack );
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateFindOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateFindOperationL(
        const MDesC16Array& aSearchStrings, 
        const MVPbkFieldTypeList& aFieldTypes, 
        MVPbkContactFindFromStoresObserver& aObserver, 
        const TCallBack& aWordParserCallBack,
        const CDesC16ArrayFlat& aStoreEntriesArray )
    {
    return (reinterpret_cast<MVPbkContactOperationFactory2*>
    	(iOperationFactory->ContactOperationFactoryExtension(KMVPbkContactOperationFactory2Uid)))->
    		CreateFindOperationL( aSearchStrings, aFieldTypes,
        		aObserver, aWordParserCallBack, aStoreEntriesArray );
    }

// CContactStoreDomain::CreateCompressStoresOperationL
// -----------------------------------------------------------------------------
//  
MVPbkContactOperation* CContactStoreDomain::CreateCompressStoresOperationL(
	MVPbkBatchOperationObserver& /*aObserver*/)
	{
	// Compress not supported in SIM store
	return NULL;
	}
TAny* CContactStoreDomain::ContactOperationFactoryExtension( TUid aExtensionUid )
	{
	if ( aExtensionUid == KMVPbkContactOperationFactory2Uid )
		{
		return static_cast<MVPbkContactOperationFactory2*>( this );
		}
	return NULL;
	}
	
} // namespace VPbkSimStore

//  End of File  
