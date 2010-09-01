/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CContactStoreDomain implementation
*
*/


// INCLUDE FILES
#include "contactstoredomain.h"
#include "contactstorelist.h"
#include "contactstore.h"
#include "ldapuid.h"
#include "ldapfinderui.h"

#include <barsc.h>              // For resource
#include <barsread.h>           // For resource reader
#include <mvpbkfieldtype.h>     // Field type list
#include "fieldtypemappings.h"  // Field type mapping
#include <ldapstoreres.rsg>     // Field type mapping resource

// CONSTANTS
_LIT(KLDAPDriveZ,"z:");
_LIT(KLDAPResourceFilesDir,"\\resource\\");
_LIT(KLDAPStoreResFile,"LDAPStoreRes.rsc");

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CleanupResetAndDestroy
// Cleanup finder ui plugin implementations
// -----------------------------------------------------------------------------
//
void CleanupResetAndDestroy(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
        }
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CContactStoreDomain
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactStoreDomain::CContactStoreDomain
(
    const MVPbkFieldTypeList& aMasterFieldTypeList
)
:   iMasterFieldTypeList     (aMasterFieldTypeList)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::ConstructL
// CContactStoreDomain constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::ConstructL
(
    const CVPbkContactStoreUriArray& aURIList,
    const TDesC& aURIDomain
)
    {
    // Domain descriptor
    iDomainDescriptor = aURIDomain.AllocL();

    // Field types
    iFieldTypeMappings = CFieldTypeMappings::NewL(iMasterFieldTypeList);

    // Initialize field mappings
    InitializeFieldMappingL();

    // Finder ui for all stores
    LoadFinderUIL();

    // Create the store list that creates the stores
    iContactStoreList = CContactStoreList::NewL(aURIList,*this);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::InitializeFieldMappingL
// Initialize contact model mappings to versit properties
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::InitializeFieldMappingL()
    {
    // For resource reading
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
   
    // Resource file name
    TFileName res_name(KLDAPDriveZ);
    res_name.Append(KLDAPResourceFilesDir);
    res_name.Append(KLDAPStoreResFile);

    // Load Versit field type mappings
    RResourceFile res_file;

    // Open resource
    res_file.OpenL(fs,res_name);
    CleanupClosePushL(res_file);

    // Resource reader
    TResourceReader reader;
    // Versit types
    reader.SetBuffer(res_file.AllocReadLC(R_VERSIT_FIELD_TYPE_MAPPINGS));
    // Read versit types
    iFieldTypeMappings->InitVersitMappingsL(reader);
    CleanupStack::PopAndDestroy();  //R_VERSIT_FIELD_TYPE_MAPPINGS

    // Non versit types
    reader.SetBuffer(res_file.AllocReadLC(R_NONVERSIT_FIELD_TYPE_MAPPINGS));
    // Read non versit types
    iFieldTypeMappings->InitNonVersitMappingsL(reader);
    CleanupStack::PopAndDestroy();  //R_NONVERSIT_FIELD_TYPE_MAPPINGS

    // Close resource
    res_file.Close();

    // Resource file
    CleanupStack::PopAndDestroy();
    // File server
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::LoadFinderUIL
// Initialize finder ui
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::LoadFinderUIL()
    {
    // Find implementations
    RImplInfoPtrArray implementations;
    REComSession::ListImplementationsL(
            TUid::Uid(KLDAPFinderInterfaceUID),implementations);

    // Push implmentations
    CleanupStack::PushL(TCleanupItem(CleanupResetAndDestroy,&implementations));

    // Shoud be only one
    const TInt count = implementations.Count();
    if (count > 0)
        {
        // Implementation
        CImplementationInformation* infop = implementations[count - 1];
        CleanupStack::PushL(infop);
        implementations.Remove(count - 1);

        // Load finder
        iFinder = CLDAPFinderUI::NewL(infop->ImplementationUid());

        // Pop info
        CleanupStack::PopAndDestroy(infop);
        }

     // Implementations
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::NewLC
// CContactStoreDomain two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreDomain* CContactStoreDomain::NewLC
(
    const CVPbkContactStoreUriArray& aURIList, 
    const TDesC& aURIDomain,
    const MVPbkFieldTypeList& aMasterFieldTypeList
)
    {
    CContactStoreDomain* self = new (ELeave)CContactStoreDomain(aMasterFieldTypeList);
    CleanupStack::PushL(self);
    self->ConstructL(aURIList,aURIDomain);
    return self;
    }
// -----------------------------------------------------------------------------
// CContactStoreDomain::NewL
// CContactStoreDomain two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreDomain* CContactStoreDomain::NewL(TAny* aParam)
    {
    TStoreDomainParam* param = static_cast<TStoreDomainParam*>(aParam);
    CContactStoreDomain* self = CContactStoreDomain::NewLC(
        param->iURIList,*param->iURIDomain,*param->iFieldTypeList);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::~CContactStoreDomain
// CContactStoreDomain Destructor
// -----------------------------------------------------------------------------
//
CContactStoreDomain::~CContactStoreDomain()
    {
    // Store domain
    if (iDomainDescriptor)
        {
        delete iDomainDescriptor;
        iDomainDescriptor = NULL;
        }
    // Field type mappings
    if (iFieldTypeMappings)
        {
        delete iFieldTypeMappings;
        iFieldTypeMappings = NULL;
        }
    // Store list
    if (iContactStoreList)
        {
        delete iContactStoreList;
        iContactStoreList = NULL;
        }
    // Finder UI
    if (iFinder)
        {
        delete iFinder;
        iFinder = NULL;
        }
    }
// -----------------------------------------------------------------------------
//                  CContactStoreDomain public methods
// -----------------------------------------------------------------------------
// CContactStoreDomain::FieldTypeMappings
// Returns the field type mappings between ldap and phonebook.
// -----------------------------------------------------------------------------
//
CFieldTypeMappings& CContactStoreDomain::FieldTypeMappings()
    {
    return *iFieldTypeMappings;
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::MasterFieldTypeList
// Returns the master field type list.
// -----------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CContactStoreDomain::MasterFieldTypeList() const
    {
    return iMasterFieldTypeList;
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::FinderUI
// Returns the finder ui.
// -----------------------------------------------------------------------------
//
CLDAPFinderUI& CContactStoreDomain::FinderUI()
    {
    return *iFinder;
    }

// -----------------------------------------------------------------------------
//                  CVPbkContactStoreDomain implmentation
// -----------------------------------------------------------------------------
// CContactStoreDomain::UriDomain
// Returns the URI domain.
// -----------------------------------------------------------------------------
//
const TDesC& CContactStoreDomain::UriDomain() const
    {
    return *iDomainDescriptor;
    }
// -----------------------------------------------------------------------------
// CContactStoreDomain::LoadContactStoreL
// Loads a new contact store implementation to this domain.
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::LoadContactStoreL(const TVPbkContactStoreUriPtr& aUri)
    {
    // Already in list
    if (! iContactStoreList->Find(aUri))
        {
        // New store
        CContactStore* store = CContactStore::NewLC(aUri,*this);
        // Add to list
        iContactStoreList->AppendL(store);
        CleanupStack::Pop(store);
        }
    }

// -----------------------------------------------------------------------------
//                      MVPbkContactStoreList implmentation
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
MVPbkContactStore& CContactStoreDomain::At(TInt aIndex) const
    {
    return iContactStoreList->At(aIndex);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::Find
// -----------------------------------------------------------------------------
//
MVPbkContactStore* CContactStoreDomain::Find(
    const TVPbkContactStoreUriPtr& aUri) const
    {
    return iContactStoreList->Find(aUri);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::OpenAllL
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::OpenAllL(MVPbkContactStoreListObserver& aObserver)
    {
    iContactStoreList->OpenAllL(aObserver);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CloseAll
// -----------------------------------------------------------------------------
//
void CContactStoreDomain::CloseAll(MVPbkContactStoreListObserver& aObserver)
    {
    iContactStoreList->CloseAll(aObserver);
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactOperationFactory implementation
// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateContactRetrieverL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateContactRetrieverL
( 
    const MVPbkContactLink& aLink, 
    MVPbkSingleContactOperationObserver& aObserver
)
    {
    return iContactStoreList->CreateContactRetrieverL(aLink,aObserver);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateDeleteContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateDeleteContactsOperationL
(
    const MVPbkContactLinkArray& aContactLinks,
    MVPbkBatchOperationObserver& aObserver
)
    {
    return iContactStoreList->CreateDeleteContactsOperationL(aContactLinks,aObserver);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateDeleteContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateCommitContactsOperationL
(
    const TArray<MVPbkStoreContact*>& aContacts,
    MVPbkBatchOperationObserver& aObserver
)
    {
    return iContactStoreList->CreateCommitContactsOperationL(aContacts,aObserver);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateMatchPhoneNumberOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStoreDomain::CreateMatchPhoneNumberOperationL
(
    const TDesC& aPhoneNumber,
    TInt aMaxMatchDigits,
    MVPbkContactFindObserver& aObserver
)
    {
    return iContactStoreList->CreateMatchPhoneNumberOperationL(aPhoneNumber,
                                                    aMaxMatchDigits,aObserver);
    }

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateFindOperationL
// -----------------------------------------------------------------------------
//  
MVPbkContactOperation* CContactStoreDomain::CreateFindOperationL
(
    const TDesC& aSearchString,
    const MVPbkFieldTypeList& aFieldTypes,
    MVPbkContactFindObserver& aObserver
)
    {
    return iContactStoreList->CreateFindOperationL(aSearchString,aFieldTypes,
                                                                    aObserver);
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
    // TODO 
    //return iContactStoreList->CreateFindOperationL(aSearchString,aFieldTypes, aObserver);
    return NULL;
    }
    

// -----------------------------------------------------------------------------
// CContactStoreDomain::CreateCompressStoresOperationL
// -----------------------------------------------------------------------------
//  
MVPbkContactOperation* CContactStoreDomain::CreateCompressStoresOperationL
(
    MVPbkBatchOperationObserver& /* aObserver */
)
    {
    // LDAP Store is remote
    return NULL;
    }

} // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------

