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
* Description:  CContactStore implementation
*
*/


// INCLUDE FILES
#include "contactstore.h"
#include "contactstoredomain.h"
#include "contactstoreinfo.h"
#include "contactstoreproperties.h"
#include "contactstoreuri.h"
#include "contact.h"
#include "ldapcontact.h"
#include "contactlink.h"
#include "contactretriever.h"
#include "contactview.h"
#include "supportedfieldtypes.h"

#include "ldapfinderui.h"       // Finder UI
#include "fieldtypemappings.h"  // Field type mapping
#include "ldapfieldtype.h"      // LDAP Field type
#include <barsc.h>              // For resource
#include <barsread.h>           // For resource reader
#include <mvpbkfieldtype.h>     // Field type list
#include <ldapstoreres.rsg>     // Field type mapping resource

#include <cvpbkcontactviewdefinition.h>
#include <mvpbkcontactstoreobserver.h>
#include <mvpbkcontactoperationfactory.h>

// CONSTANTS
_LIT(KLDAPDriveC,"c:");
_LIT(KLDAPDriveZ,"z:");
_LIT(KLDAPResourceFilesDir,"\\resource\\");
_LIT(KLDAPStoreResFile,"LDAPStoreRes.rsc");

// -----------------------------------------------------------------------------
// LDAPStoreInitializeL
// Store initialized notify callback
// -----------------------------------------------------------------------------
//
static TInt LDAPStoreInitializeL(TAny* aThis)
    {
    if (aThis)
        {
        return static_cast<LDAPStore::CContactStore*>(aThis)->StoreInitializeL();
        }
    return EFalse;
    }
// -----------------------------------------------------------------------------
// LDAPStoreReady
// Store ready notify callback
// -----------------------------------------------------------------------------
//
static TInt LDAPStoreReady(TAny* aThis)
    {
    if (aThis)
        {
        static_cast<LDAPStore::CContactStore*>(aThis)->StoreReady();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContactStore::CContactStore
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactStore::CContactStore(CContactStoreDomain& aStoreDomain)
:   iStoreDomain(aStoreDomain)
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CContactStore::ConstructL
// CContactStore constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContactStore::ConstructL(const TVPbkContactStoreUriPtr& aURI)
    {
    // Store properties
    iProperties = CContactStoreProperties::NewL();

    // Store uri - pure nonsense, but required
    iStoreURI = CContactStoreUri::NewL(aURI);

    // Set name, actually whole uri
    iProperties->SetName(iStoreURI->Uri());

    // Store information
    iStoreInfo = CContactStoreInfo::NewL(*this);

    // Settings loaded with store name
    iSettings = CLDAPSettings::NewL(iProperties->Name().UriDes());

    // Initialiaze field mappings of this store
    InitializeFieldMappingL();
    }

// -----------------------------------------------------------------------------
// CContactStore::NewLC
// CContactStore two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStore* CContactStore::NewLC(const TVPbkContactStoreUriPtr& aURI,
                                    CContactStoreDomain& aStoreDomain)
    {
    CContactStore* self = new (ELeave)CContactStore(aStoreDomain);
    CleanupStack::PushL(self);
    self->ConstructL(aURI);
    return self;
    }
// -----------------------------------------------------------------------------
// CContactStore::NewL
// CContactStore two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStore* CContactStore::NewL(const TVPbkContactStoreUriPtr& aURI,
                                   CContactStoreDomain& aStoreDomain)
    {
    CContactStore* self = CContactStore::NewLC(aURI,aStoreDomain);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CContactStore::~CContactStore
// CContactStore Destructor
// -----------------------------------------------------------------------------
//
CContactStore::~CContactStore()
    {
    // Close observers
    iStoreObservers.Close();

    // Callback
    if (iIdle)
        {
        // Cancel all pending operations
        iIdle->Cancel();
        delete iIdle;
        iIdle = NULL;
        }

    // Store directory
    if (iDirectory)
        {
        delete iDirectory;
        iDirectory = NULL;
        }
    // Settings
    if (iSettings)
        {
        delete iSettings;
        iSettings = NULL;
        }
    // Store URI
    if (iStoreURI)
        {
        delete iStoreURI;
        iStoreURI = NULL;
        }
    // Store properties
    if (iProperties)
        {
        delete iProperties;
        iProperties = NULL;
        }
    // Store information
    if (iStoreInfo)
        {
        delete iStoreInfo;
        iStoreInfo = NULL;
        }

    // Supported fields
    if (iSupportedFieldTypes)
        {
        delete iSupportedFieldTypes;
        iSupportedFieldTypes = NULL;
        }

    // Field type list
    iFieldTypes.Close();

    // Clear contacts
    iContacts.ResetAndDestroy();
    iContacts.Close();
    }

// -----------------------------------------------------------------------------
//                      CContactStore public methods
// -----------------------------------------------------------------------------
// CContactStore::AddObserverL
// -----------------------------------------------------------------------------
//
void CContactStore::AddObserverL(MVPbkContactStoreObserver& aObserver)
    {
    // Already in observers
    TInt find = iStoreObservers.Find(&aObserver);
    if (find == KErrNotFound)
        {
        // Add observer
        iStoreObservers.AppendL(&aObserver);
        }
    }
// -----------------------------------------------------------------------------
// CContactStore::RemoveObserver
// -----------------------------------------------------------------------------
//
void CContactStore::RemoveObserver(MVPbkContactStoreObserver& aObserver)
    {
    const TInt pos = iStoreObservers.Find(&aObserver);
    if (pos != KErrNotFound)
        {
        iStoreObservers.Remove(pos);
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateContactL
// Creates new contact from index.
// -----------------------------------------------------------------------------
//
CContact* CContactStore::CreateContactL(TInt aIndex)
    {
    CContact* cp = NULL;
    // Index in range
    if (aIndex >= 0 && aIndex < iContacts.Count())
        {        
        // New contact
        cp = CContact::NewL(*this,ContactAt(aIndex).Contact(),EFalse);
        }
    return cp;
    }

// -----------------------------------------------------------------------------
// CContactStore::ContactCount
// -----------------------------------------------------------------------------
//
TInt CContactStore::ContactCount()
    {
    return iContacts.Count();    
    }

// -----------------------------------------------------------------------------
// CContactStore::ContactAt
// -----------------------------------------------------------------------------
//
CContact& CContactStore::ContactAt(TInt aIndex)
    {
    return *iContacts[aIndex];
    }

// -----------------------------------------------------------------------------
// CContactStore::FieldTypeMappings
// -----------------------------------------------------------------------------
//
CFieldTypeMappings& CContactStore::FieldTypeMappings()
    {
    return iStoreDomain.FieldTypeMappings();
    }

// -----------------------------------------------------------------------------
// CContactStore::IsInitialized
// Returns true if store is initialized.
// -----------------------------------------------------------------------------
//
TBool CContactStore::IsInitialized() {
    return iState == EInitialized;
}

// -----------------------------------------------------------------------------
// CContactStore::IsOpen
// Returns true if store is open.
// -----------------------------------------------------------------------------
//
TBool CContactStore::IsOpen()
    {
    return iState == EOpened;
    }

// -----------------------------------------------------------------------------
// CContactStore::State
// Returns store state.
// -----------------------------------------------------------------------------
//
CContactStore::TState CContactStore::State()
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CContactStore::MatchContactStore
// -----------------------------------------------------------------------------
//
TBool CContactStore::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    if (iStoreURI->Uri().Compare(TVPbkContactStoreUriPtr(aContactStoreUri), 
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }
    
// -----------------------------------------------------------------------------
// CContactStore::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//
TBool CContactStore::MatchContactStoreDomain(const TDesC& aContactStoreDomain) const
    {
    if (iStoreURI->Uri().Compare(aContactStoreDomain, 
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStore::Name
// Returns store name
// -----------------------------------------------------------------------------
//
TPtrC CContactStore::Name()
    {
    return (iSettings) ? iSettings->Name() : TPtrC(KNullDesC);
    }

// -----------------------------------------------------------------------------
// CContactStore::StoreInitializeL
// Initializes store
// -----------------------------------------------------------------------------
//
TBool CContactStore::StoreInitializeL()
    {
    TBool status = ETrue;
    // Just change state
    if (iState == EIdle)
        {
        iState = EInitializing;
        }
    else
    // Start initializing
    if (iState == EInitializing)
        {
        // Old directory
        if (iDirectory)
            {
            delete iDirectory;
            iDirectory = NULL;
            }
        // New directory
        iDirectory = LDAPEngine::CDirectory::NewL();
        // Add observer
        iDirectory->AddObserverL(*this);
        // Configure
        iDirectory->Configure(*iSettings);
        // Set field types
        iDirectory->SetSearchAttributesL(iFieldTypes);
        // Supported field types
        InitializeSupportedFieldsL();
        // Stop initializing
        status = EFalse;
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CContactStore::StoreReady
// -----------------------------------------------------------------------------
//
void CContactStore::StoreReady()
    {
    // Notify all - notice the reverse order
    for (TInt loop = iStoreObservers.Count() - 1; loop >= 0 ;loop--)
        {
        // Notify
        StoreReady(*iStoreObservers[loop]);
        }
    }

// -----------------------------------------------------------------------------
//                      CContactStore private methods
// -----------------------------------------------------------------------------
// CTestEngineAppUi::InitializeFieldMappingL
// Initialiaze store field type mappings
// -----------------------------------------------------------------------------
void CContactStore::InitializeFieldMappingL()
    {  
    // For resource reading
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    // Default resource file name
    TFileName res_name(KLDAPDriveZ);
    res_name.Append(KLDAPResourceFilesDir);
    res_name.Append(KLDAPStoreResFile);

    // Customized resource file
    TPtrC cus_name = iSettings->ResourceFile();
    if (cus_name.Length() > 0)
        {
        // Custom resource name
        TFileName cus_res;

        // Parse name
        TParse name_parser;
        name_parser.Set(cus_name,NULL,NULL);

        // Has drive
        TPtrC drive = name_parser.Drive();
        if (drive.Length() > 0)
            {
            // Add drive
            cus_res.Append(drive);
            }
        else
            {
            // Add default C drive
            cus_res.Append(KLDAPDriveC);
            }

        // Path present
        if (name_parser.PathPresent())
            {
            // Add path
            cus_res.Append(name_parser.Path());
            }
        else
            {
            // Add default
            cus_res.Append(KLDAPResourceFilesDir);
            }

        // Add name
        cus_res.Append(name_parser.NameAndExt());
        if (fs.IsValidName(cus_res))
            {
            // File attributes
            TUint attr = 0;
            TInt status = fs.Att(cus_res,attr);
            if (status == KErrNone)
                {
                // Replace name
                res_name.Copy(cus_res);
                }
            }
        }

    // Load field type mappings
    RResourceFile res_file;
    // Open resource
    res_file.OpenL(fs,res_name);

    // Check that resource has mapping list
    if (! res_file.OwnsResourceId(R_LDAP_FIELD_MAPPING_LIST))
        {
        // Close resource
        res_file.Close();
        // Custom resource
        if (cus_name.Length() > 0)
            {
            // Default resource
            res_name.Copy(KLDAPDriveZ);
            res_name.Append(KLDAPResourceFilesDir);
            res_name.Append(KLDAPStoreResFile);
            // Open resource
            res_file.OpenL(fs,res_name);
            }
        else
            {
            // File server
            CleanupStack::PopAndDestroy();
            return;
            }
        }

    // Push to close
    CleanupClosePushL(res_file);

    // Resource reader
    TResourceReader reader;
    // Search attribute list
    reader.SetBuffer(res_file.AllocReadLC(R_LDAP_FIELD_MAPPING_LIST));

    // Read field mapping list
    TInt count = reader.ReadInt16();
    while (count-- > 0)
        {
        // Read entries
        iFieldTypes.AppendL(TLDAPFieldType(reader));
        }

    // Cleanup - R_LDAP_FIELD_MAPPING_LIST
    CleanupStack::PopAndDestroy();

    // Close resource
    res_file.Close();

    // Resource file
    CleanupStack::PopAndDestroy();
    // File server
    CleanupStack::PopAndDestroy();
    }

// -----------------------------------------------------------------------------
// CTestEngineAppUi::InitializeSupportedFieldsL
// Initialiaze store supported field types
// -----------------------------------------------------------------------------
void CContactStore::InitializeSupportedFieldsL()
    {
    // Clear supported fields
    if (iSupportedFieldTypes)
        {
        delete iSupportedFieldTypes;
        iSupportedFieldTypes = NULL;
        }

    // Supported fields
    iSupportedFieldTypes = CSupportedFieldTypes::NewL(iFieldTypes,
                                iStoreDomain.FieldTypeMappings());
    // Set supported fields
    iProperties->SetSupportedFields(*iSupportedFieldTypes);
    }

// -----------------------------------------------------------------------------
// CContactStore::StoreReady
// -----------------------------------------------------------------------------
//
void CContactStore::StoreReady(MVPbkContactStoreObserver& aObserver)
    {
    aObserver.StoreReady(*this);
    }

// -----------------------------------------------------------------------------
// CContactStore::RetrieveContactsL
// Creates contact records from directory.
// -----------------------------------------------------------------------------
//
void CContactStore::RetrieveContactsL()
    {
    // Clear old results
    iContacts.ResetAndDestroy();
    // Results count
    TInt count = (iDirectory) ? iDirectory->Records() : 0;
    if (count > 0)
        {
        // Make new results
        for (TInt loop = 0;loop < count;loop++)
            {
            // LDAP contact
            CLDAPContact* lcp = iDirectory->CreateContactL(loop);
            if (lcp)
                {
                // Push to stack
                CleanupStack::PushL(lcp);

                // New contact
                CContact* cp = CContact::NewLC(*this,lcp);
                cp->SetIndex(loop);

                // Add to contacts
                iContacts.AppendL(cp);

                // Pop stack
                CleanupStack::Pop(cp);
                CleanupStack::Pop(lcp);
                }
            } // End of loop over results

        // Can release results
        if (iDirectory->CanReleaseResults())
            {
            // Clear results
            iDirectory->ReleaseResults();
            }
        else
            {
            // Set release results
            iDirectory->SetCanReleaseResults(ETrue);
            }
        }
    }

// -----------------------------------------------------------------------------
//              MVPbkContactOperationFactory implementation
// -----------------------------------------------------------------------------
// CContactStore::CreateContactRetrieverL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateContactRetrieverL( 
    const MVPbkContactLink& aLink,
    MVPbkSingleContactOperationObserver& aObserver
)
    {
    // Contact link is from this store
    if (this == &aLink.ContactStore())
        {
        // Cast to contact link
        const CContactLink& link = static_cast<const CContactLink&>(aLink);
        // Contact retriever
        return CContactRetriever::NewL(*this,link.Index(),aObserver);
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateDeleteContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateDeleteContactsOperationL(
    const MVPbkContactLinkArray& /* aContactLinks */, 
    MVPbkBatchOperationObserver& /* aObserver */
)
    {
    // LDAP Store is read only
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateCommitContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateCommitContactsOperationL(
    const TArray<MVPbkStoreContact*>& /* aContacts */,
    MVPbkBatchOperationObserver& /* aObserver */
)
    {
    // LDAP Store is read only
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateMatchPhoneNumberOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateMatchPhoneNumberOperationL(
    const TDesC&           /* aPhoneNumber */,
    TInt               /*  aMaxMatchDigits */,
    MVPbkContactFindObserver& /* aObserver */
)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateFindOperationL
// Creates find operation.
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateFindOperationL(
    const TDesC&            /* aSearchString */,
    const MVPbkFieldTypeList& /* aFieldTypes */,
    MVPbkContactFindObserver&   /* aObserver */ )
    {
    return NULL;
    }

MVPbkContactOperation* CContactStore::CreateFindOperationL(
                const MDesC16Array& aSearchStrings, 
                const MVPbkFieldTypeList& aFieldTypes, 
                MVPbkContactFindFromStoresObserver& aObserver, 
                const TCallBack& aWordParserCallBack)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateCompressStoresOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateCompressStoresOperationL(
    MVPbkBatchOperationObserver& /*aObserver*/
)
    {
    // Remote store
    return NULL;
    }

// -----------------------------------------------------------------------------
//                      MVPbkContactStore implementation
// -----------------------------------------------------------------------------
// CContactStore::StoreProperties
// Returns store properties.
// -----------------------------------------------------------------------------
//
const MVPbkContactStoreProperties& CContactStore::StoreProperties() const
    {
    return *iProperties;
    }

// -----------------------------------------------------------------------------
// CContactStore::OpenL
// Opens this store asynchronously.
// -----------------------------------------------------------------------------
//
void CContactStore::OpenL(MVPbkContactStoreObserver& aObserver)
    {
    // Add observers
    AddObserverL(aObserver);

    // State is idle
    if (iState == EIdle)
        {
        // Start initializing
        iState = EInitializing;
        // Clear for new notifier
        if (iIdle)
            {
            // Cancel pending operation
            iIdle->Cancel();
            delete iIdle;
            iIdle = NULL;
            }
        // Create initializer
        iIdle = CIdle::NewL(CActive::EPriorityStandard);
        // Initialized notifier
        TCallBack callback(LDAPStoreInitializeL,this);
        // Start
        iIdle->Start(callback);
        }
    else
        {
        // Still active, observer will be notified
        if (iIdle && iIdle->IsActive())
            return;

        // Clear for new notifier
        if (iIdle)
            {
            // Cancel pending operation
            iIdle->Cancel();
            delete iIdle;
            iIdle = NULL;
            }

        // Create store ready notifier
        iIdle = CIdle::NewL(CActive::EPriorityStandard);
        // Store ready notifier
        TCallBack callback(LDAPStoreReady,this);
        // Start
        iIdle->Start(callback);
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::ReplaceL
// Replaces an existing store and opens it asynchronously.
// -----------------------------------------------------------------------------
//
void CContactStore::ReplaceL(MVPbkContactStoreObserver& aObserver)
    {
    OpenL(aObserver);
    }
// -----------------------------------------------------------------------------
// CContactStore::Close
// Closes this contact store from a single observer.
// -----------------------------------------------------------------------------
//
void CContactStore::Close(MVPbkContactStoreObserver& aObserver)
    {
    // Remove observer
    RemoveObserver(aObserver);
    // Last one switch the lights off
    if (iStoreObservers.Count() == 0)
        {
        // Close connection
        if (iDirectory && iDirectory->IsConnected())
            {
            iDirectory->Shutdown();
            }
        iState = EClosed;
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateNewContactLC
// Creates new contact associated to this store.
// -----------------------------------------------------------------------------
//
MVPbkStoreContact* CContactStore::CreateNewContactLC()
    {
    // Read only
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateNewContactGroupLC
// Creates contact group associated to this store.
// -----------------------------------------------------------------------------
//
MVPbkContactGroup* CContactStore::CreateNewContactGroupLC()
    {
    // No groups
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateViewLC
// Creates contact view.
// -----------------------------------------------------------------------------
//
MVPbkContactView* CContactStore::CreateViewLC(
    const CVPbkContactViewDefinition& aViewDefinition,
    MVPbkContactViewObserver& aObserver,
    const MVPbkFieldTypeList& aSortOrder
)
    {
    // View type
    TVPbkContactViewType type = aViewDefinition.Type();
    // Contact view
    MVPbkContactView* vp = NULL;
    // Folding view
    if (type == EVPbkFoldingView &&
        aViewDefinition.FlagIsOn(EVPbkContactViewFolding))
        {
        vp = CContactView::NewLC(aObserver,*this,aSortOrder,type);
        }
    else
        {
        // Clear old results
        iContacts.ResetAndDestroy();

        // Finder
        CLDAPFinderUI& finder = iStoreDomain.FinderUI();
        finder.ExecuteL(*iDirectory);
        iState = ESearching;

        // Contact view
        type = EVPbkContactsView;
        vp = CContactView::NewLC(aObserver,*this,aSortOrder,type);
        }
    return vp;
    }

// -----------------------------------------------------------------------------
// CContactStore::ContactGroupsLC
// Returns contact groups from this store.
// -----------------------------------------------------------------------------
//    
MVPbkContactLinkArray* CContactStore::ContactGroupsLC() const
    {
    // No groups
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::StoreInfo
// -----------------------------------------------------------------------------
//
const MVPbkContactStoreInfo& CContactStore::StoreInfo() const
    {
    return *iStoreInfo;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateLinkFromInternalsLC
// Creates contact link from packed stream.
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CContactStore::CreateLinkFromInternalsLC(
    RReadStream& aStream) const
    {
    // Read index and type id
    TInt index = aStream.ReadInt32L();
    TInt type  = aStream.ReadInt32L();
    // New contact link
    return CContactLink::NewLC(const_cast<CContactStore&>(*this),index,type);
    }

// -----------------------------------------------------------------------------
//                      MDirectoryObserver implementation
// -----------------------------------------------------------------------------
// CContactStore::ActionDoneL
// This method is called when action ends
// -----------------------------------------------------------------------------
//
void CContactStore::ActionDoneL(
    LDAPEngine::CDirectory& /* aDirectory */,
    LDAPEngine::TActionType aAction,
    TInt aStatus, TPtrC8 // aErrorMsg
)
    {
    // Action was successful
    if (aStatus == KErrNone)
        {
        // Connected
        if (aAction == LDAPEngine::EConnect)
            {
            iState = EOpened;
            }
        else
        // Secured connection
        if (aAction == LDAPEngine::ESecureConnection)
            {
            }
        else
        // Authenticated
        if (aAction == LDAPEngine::EAuthenticate)
            {
            }
        else
        // Search done
        if (aAction == LDAPEngine::ESearchAction)
            {
            // Contacts from directory
            RetrieveContactsL();
            // Search done
            iState = ERetrieved;
            }
        else
        // Logout
        if (aAction == LDAPEngine::ELogout)
            {
            }
        else
        // Disconnect, shutdown
        if (aAction == LDAPEngine::EDisconnect)
            {
            iState = EIdle;
            }
        }
    else

    // Limits exceeded warnings
    if (aStatus == LDAPEngine::ETimeLimitExceeded ||
        aStatus == LDAPEngine::ESizeLimitExceeded)
        {
        // Search done
        if (aAction == LDAPEngine::ESearchAction)
            {
            // Contacts from directory
            RetrieveContactsL();
            // Search done
            iState = ERetrieved;
            }
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::ConnectionStateChangedL
// This is notified when directory connection state changes.
// -----------------------------------------------------------------------------
//
void CContactStore::ConnectionStateChangedL(
    LDAPEngine::CDirectory& /* aDirectory */,
    LDAPEngine::TConnectionState aState
)
    {
    // According state
    switch (aState)
        {
        case LDAPEngine::EIdle:
            break;

        // Directory configured
        case LDAPEngine::EConfigured:
            // Previous state was
            if (iState == EIdle || iState == EInitializing)
                {
                // New state
                iState = EInitialized;
                // Notify store ready
                StoreReady();
                }
            break;

        // Intermediate connecting states are irrelevant
        case LDAPEngine::ELookingUp:
        case LDAPEngine::EConnecting:
        case LDAPEngine::EConnected:
        case LDAPEngine::ESecuring:
        case LDAPEngine::ESecured:
            break;

        // Authentication states
        case LDAPEngine::EAuthenticating:
            break;
        case LDAPEngine::EAuthenticated:
            break;

        case LDAPEngine::ESending:
            break;
        case LDAPEngine::ESent:
            break;
        case LDAPEngine::EReceiving:
            iState = ERetrieving;
            break;
        case LDAPEngine::EReceived:
            break;
        case LDAPEngine::EWaiting:
            break;

        case LDAPEngine::ESearching:
            iState = ESearching;
            break;
        case LDAPEngine::ESearchDone:
            break;

        // Directory is closing down
        case LDAPEngine::EUnbinding:
        case LDAPEngine::EUnbound:
        case LDAPEngine::EClosing:
            iState = EClosing;
            break;

        case LDAPEngine::EClosed:
            iState = EClosed;
            break;

        case LDAPEngine::EShutdown:
            iState = EIdle;
            break;

        default:
            break;
        } // End of switch state
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
