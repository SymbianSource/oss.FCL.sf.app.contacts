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
* Description:  Contacts Model store contact store implementation.
*
*/


// INCLUDES
#include "CContactStore.h"

// VPbkCntModel
#include "CFieldFactory.h"
#include "CFieldTypeMap.h"
#include "CAsyncContactOperation.h"
#include "CContact.h"
#include "CContactLink.h"
#include "CContactBookmark.h"
#include "CContactView.h"
#include "CFilteredContactView.h"
#include "CFilteredGroupView.h"
#include "CGroupView.h"
#include "CContactStoreDomain.h"
#include "CFieldInfo.h"
#include "CContactRetriever.h"
#include "CDeleteContactsOperation.h"
#include "CCommitContactsOperation.h"
#include "CMatchPhoneNumberOperation.h"
#include "CFindOperation.h"
#include "CFindInTextDefOperation.h"
#include "CContactStoreInfo.h"
#include "CVPbkDiskSpaceCheck.h"
#include "CNamedRemoteViewHandle.h"
#include "CSortOrderAcquirerList.h"
#include "VoiceTagSupport.h"
#include "CContactLink.h"
#include "COwnContactLinkOperation.h"
#include <vpbkcntmodelres.rsg>

// Virtual Phonebook
#include <VPbkError.h>
#include <MVPbkContactStoreObserver.h>
#include <TVPbkContactStoreUriPtr.h>
#include <CVPbkContactStoreUri.h>
#include <CVPbkAsyncOperation.h>
#include <RLocalizedResourceFile.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkContactViewDefinition.h>
#include <VPbkContactView.hrh>
#include <CVPbkContactStoreProperties.h>
#include <CVPbkLocalVariationManager.h>
#include <CVPbkFieldTypeSelector.h>
#include <VPbkUtils.h>
#include <TVPbkContactStoreUriPtr.h>
#include <VPbkDataCaging.hrh>
#include <VPbkContactStoreUris.h>
#include <MVPbkSingleContactLinkOperationObserver.h>

// Contacts Model
#include <cntdb.h>
#include <cntitem.h>
#include <cntmodel.rsg>

// System includes
#include <barsc.h>
#include <barsread.h>
#include <featmgr.h>
#include <shareddataclient.h>
#include <coemain.h>

// Debugging headers
#include <VPbkDebug.h>


namespace VPbkCntModel {

#ifdef _DEBUG
_LIT( KContactStorePanicCat, "VPbkCntModel_CContactStore" );
void ContactStorePanic( TContactStorePanic aPanic )
	{
	User::Panic( KContactStorePanicCat, aPanic);
	}
#endif // _DEBUG

/// Unnamed namespace for local definitions
namespace {

// LOCAL
_LIT(KCntResFile, "cntmodel.rsc");
_LIT(KCntExtResFile, "VPbkCntModelRes.rsc");

// CONSTANTS
const TInt KDefaultArrayGranularity = 4;
const TInt KDiskSpaceForDbOpening = 140 * 1024; // 140 kB space for db opening

inline void SendEventL(MVPbkContactStoreObserver* aObserver,
                       void (MVPbkContactStoreObserver::*aEventFunc)(MVPbkContactStore&),
                       MVPbkContactStore& aStore)
    {
    (aObserver->*aEventFunc)(aStore);
    }

inline void SendEventL(
        MVPbkContactStoreObserver* aObserver,
        void (MVPbkContactStoreObserver::*aEventFunc)(MVPbkContactStore&,TVPbkContactStoreEvent),
        MVPbkContactStore& aStore,
        TVPbkContactStoreEvent aEvent)
    {
    (aObserver->*aEventFunc)(aStore, aEvent);
    }

inline void SendErrorEvent(MVPbkContactStoreObserver* aObserver,
                           void (MVPbkContactStoreObserver::*aEventFunc)(MVPbkContactStore&,TInt),
                           MVPbkContactStore& aStore, TInt aError)
    {
    (aObserver->*aEventFunc)(aStore, aError);
    }

void SendEventL
        (RPointerArray<MVPbkContactStoreObserver>& aObservers,
        void (MVPbkContactStoreObserver::*aEventFunc)(MVPbkContactStore&),
        MVPbkContactStore& aStore)
    {
    for (TInt i = aObservers.Count() - 1; i >= 0; --i)
        {
        SendEventL(aObservers[i], aEventFunc, aStore);
        }
    }

void SendEventL
        (RPointerArray<MVPbkContactStoreObserver>& aObservers,
        void (MVPbkContactStoreObserver::*aEventFunc)(MVPbkContactStore&,TVPbkContactStoreEvent),
        MVPbkContactStore& aStore,
        TVPbkContactStoreEvent aEvent)
    {
    for (TInt i = aObservers.Count() - 1; i >= 0; --i)
        {
        SendEventL(aObservers[i], aEventFunc, aStore, aEvent);
        }
    }

void SendErrorEvent
        (RPointerArray<MVPbkContactStoreObserver>& aObservers,
        void (MVPbkContactStoreObserver::*aEventFunc)(MVPbkContactStore&,TInt),
        MVPbkContactStore& aStore, TInt aError)
    {
    for (TInt i = aObservers.Count() - 1; i >= 0; --i)
        {
        SendErrorEvent(aObservers[i], aEventFunc, aStore, aError);
        }
    }

TBool UpdateSystemTemplateFieldL(CContactItem& aSystemTemplate,
        const CFieldInfo& aFieldInfo )
    {
    TBool updated = EFalse;
    // Scan system template field set for the field
    CContactItemFieldSet& fieldSet = aSystemTemplate.CardFields();
    const TInt fieldCount = fieldSet.Count();
    TInt i = 0;
    for (; i < fieldCount; ++i)
        {
        CContactItemField& sysTemplateField = fieldSet[i];
        if (aFieldInfo.IsEqualType(sysTemplateField))
            {
            // Field was found, just check the label
            if (!aFieldInfo.IsEqualLabel(sysTemplateField))
                {
                sysTemplateField.SetLabelL(aFieldInfo.FieldName());
                updated = ETrue;
                }
            break;
            }
        }

    if ( i == fieldCount )
        {
        // Field was not found, add it
        CContactItemField* field = aFieldInfo.CreateFieldL();
        CleanupStack::PushL( field );
        fieldSet.AddL( *field );
        CleanupStack::Pop( field );
        updated = ETrue;
        }

    return updated;
    }

TBool UpdateSystemTemplateFieldsL(CContactItem& aSystemTemplate, const CFieldsInfo& aFieldsInfo)
    {
    TBool updated = EFalse;
    const TInt fieldInfoCount = aFieldsInfo.Count();

    for (TInt i = 0; i < fieldInfoCount; ++i)
        {
        if (UpdateSystemTemplateFieldL(aSystemTemplate, *aFieldsInfo.At(i)))
            {
            updated = ETrue;
            }
        }
    return updated;
    }


TVPbkContactStoreEvent MapDbEventToStoreEvent(TContactDbObserverEvent aEvent)
    {
    TVPbkContactStoreEvent result(TVPbkContactStoreEvent::ENullEvent, NULL);

    switch (aEvent.iType)
        {
        case EContactDbObserverEventContactAdded:
            {
            result.iEventType = TVPbkContactStoreEvent::EContactAdded;
            break;
            }
        case EContactDbObserverEventContactDeleted:
        case EContactDbObserverEventOwnCardDeleted:
            {
            result.iEventType = TVPbkContactStoreEvent::EContactDeleted;
            break;
            }
	    case EContactDbObserverEventContactChanged:
	    case EContactDbObserverEventSpeedDialsChanged:
        case EContactDbObserverEventOwnCardChanged:
            {
            result.iEventType = TVPbkContactStoreEvent::EContactChanged;
            break;
            }
	    case EContactDbObserverEventGroupAdded:
            {
            result.iEventType = TVPbkContactStoreEvent::EGroupAdded;
            break;
            }
        case EContactDbObserverEventGroupDeleted:
            {
            result.iEventType = TVPbkContactStoreEvent::EGroupDeleted;
            break;
            }
        case EContactDbObserverEventGroupChanged:
            {
            result.iEventType = TVPbkContactStoreEvent::EGroupChanged;
            break;
            }
	    case EContactDbObserverEventUnknownChanges:
            {
            result.iEventType = TVPbkContactStoreEvent::EUnknownChanges;
            break;
            }
	    case EContactDbObserverEventBackupBeginning:
            {
            result.iEventType = TVPbkContactStoreEvent::EStoreBackupBeginning;
            break;
            }
	    case EContactDbObserverEventRestoreBeginning:
            {
            result.iEventType = TVPbkContactStoreEvent::EStoreRestoreBeginning;
            break;
            }
	    case EContactDbObserverEventBackupRestoreCompleted:
            {
            result.iEventType = TVPbkContactStoreEvent::EStoreBackupRestoreCompleted;
            break;
            }
        case EContactDbObserverEventCurrentItemDeleted:         // FALLTHROUGH
	    case EContactDbObserverEventCurrentItemChanged:         // FALLTHROUGH
	    case EContactDbObserverEventCurrentDatabaseChanged:     // FALLTHROUGH
	    case EContactDbObserverEventRestoreBadDatabase:         // FALLTHROUGH
	    case EContactDbObserverEventSortOrderChanged:           // FALLTHROUGH
	    case EContactDbObserverEventPreferredTemplateChanged:   // FALLTHROUGH
	    case EContactDbObserverEventRecover:                    // FALLTHROUGH
	    case EContactDbObserverEventRollback:                   // FALLTHROUGH
	    case EContactDbObserverEventTablesClosed:               // FALLTHROUGH
	    case EContactDbObserverEventTablesOpened:               // FALLTHROUGH
	    case EContactDbObserverEventTemplateChanged:            // FALLTHROUGH
	    case EContactDbObserverEventTemplateDeleted:            // FALLTHROUGH
	    case EContactDbObserverEventTemplateAdded:              // FALLTHROUGH
        case EContactDbObserverEventNull:                       // FALLTHROUGH
	    case EContactDbObserverEventUnused:                     // FALLTHROUGH
        default:
            {
            result.iEventType = TVPbkContactStoreEvent::ENullEvent;
            break;
            }
        }

    return result;
    }

}  // unnamed namespace

// --------------------------------------------------------------------------
// CContactStoreOpenOperation
// --------------------------------------------------------------------------
//
class CContactStoreOpenOperation : public CActive
{
public: // construction & destruction

    /**
     * Open native contact database operation. Started automatically.
     *
     * @param aStore Store that will receive events when operation is complete 
     * @param aReplace replace contact database (instead of open)
     */
    CContactStoreOpenOperation( CContactStore& aStore, TBool aReplace );
    
    ~CContactStoreOpenOperation();

private: // From CActive
    void DoCancel();
    void RunL();
    TInt RunError( TInt aError );

private: // data types

    /// Operation states
    enum TStates
        {
        // Operation: Replace default store
        // Next states: [EStateDone]
        EStateReplaceStore,
        
        // Operation: Open default store
        // Next states: [EStateStoreOpened]
        EStateOpenStore,
        
        // Result: State when store has been opened (success or failure) 
        // Next states: [EStateStoreOpenedAfterError, EStateDone]
        EStateStoreOpened,
        
        // Result: State after second attempt to open store (first have failed).
        // Next states: [EStateDone]
        EStateStoreOpenedAfterError,
        
        // Result: DB has been opened successfully. Report it to store. 
        // Next states: -
        EStateDone,
        };

private: // implementation
    
    /**
     * Move to next state asynchronously
     */
    void IssueRequest( TStates aNextState, TInt aError = KErrNone );
    
    /**
     * Handle state EStateStoreOpened
     */
    void HandleStoreOpenedL( TInt aError );
    
private: // data
    
    /// Active state
    TStates iState;
    /// Ref: Contact store
    CContactStore& iStore;
    /// Own: Native contact database
    CContactDatabase* iDb;
    /// Own: Native contact database open operation
    CContactOpenOperation* iOperation;
};

// --------------------------------------------------------------------------
// CContactStoreOpenOperation::~CContactStoreOpenOperation
// --------------------------------------------------------------------------
//
CContactStoreOpenOperation::~CContactStoreOpenOperation()
    {
    Cancel();
    delete iDb;
    delete iOperation;
    }

// --------------------------------------------------------------------------
// CContactStoreOpenOperation::DoCancel
// --------------------------------------------------------------------------
//
void CContactStoreOpenOperation::DoCancel()
    {
    delete iOperation;
    iOperation = NULL;
    }

// --------------------------------------------------------------------------
// CContactStoreOpenOperation::RunL
// --------------------------------------------------------------------------
//
void CContactStoreOpenOperation::RunL()
    {
    switch( iState )
        {
        case EStateReplaceStore:
            {
            iDb = CContactDatabase::ReplaceL( 
                iStore.StoreProperties().Name().UriDes() );
            IssueRequest( EStateDone );
            break;
            }
        case EStateOpenStore:
            {
            iOperation = CContactDatabase::Open( 
                iStore.StoreProperties().Name().UriDes(), iStatus );
            SetActive();
            iState = EStateStoreOpened;
            break;
            }
        case EStateStoreOpened:
            {
            HandleStoreOpenedL( iStatus.Int() );
            break;
            }
        case EStateStoreOpenedAfterError:
            {
            User::LeaveIfError( iStatus.Int() ); // report errors immediately
            iDb = iOperation->TakeDatabase();
            IssueRequest( EStateDone );
            break;
            }
        case EStateDone:
            {
            CContactDatabase* db = iDb;
            iDb = NULL; // give ownership
            CContactStore& store = iStore;
            
            TRAPD( err, store.StoreOpenedL( db ) );
            // NOTE! Don't touch members after this call.
            // This instance could be deleted
            if( err )
                {
                store.StoreOpenFailed( err );
                }
            break;
            }
        default:
            {
            // unknown state
            __ASSERT_DEBUG( EFalse,
                ContactStorePanic( EPreCond_OpenOperationState ) );
            iStore.StoreOpenFailed( KErrUnknown );
            }
        }
    }

// --------------------------------------------------------------------------
// CContactStoreOpenOperation::RunError
// --------------------------------------------------------------------------
//
TInt CContactStoreOpenOperation::RunError( TInt aError )
    {
    iStore.StoreOpenFailed( aError );
    return KErrNone;
    }

// --------------------------------------------------------------------------
// CContactStoreOpenOperation::CContactStoreOpenOperation
// --------------------------------------------------------------------------
//
CContactStoreOpenOperation::CContactStoreOpenOperation( 
    CContactStore& aStore, TBool aReplace ) : 
    CActive( EPriorityLow ),
    iStore( aStore )
    {
    CActiveScheduler::Add( this );
    IssueRequest( aReplace ? EStateReplaceStore : EStateOpenStore ); // start working
    }

// --------------------------------------------------------------------------
// CContactStoreOpenOperation::IssueRequest
// --------------------------------------------------------------------------
//
void CContactStoreOpenOperation::IssueRequest( TStates aNextState, TInt aError )
    {
    if( !IsActive() )
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, aError );
        SetActive();
        iState = aNextState; 
        }
    }

// --------------------------------------------------------------------------
// CContactStoreOpenOperation::HandleStoreOpenedL
// --------------------------------------------------------------------------
//
void CContactStoreOpenOperation::HandleStoreOpenedL( TInt aError )
    {
    if( aError == KErrNone )
        {
        // Store opened succesfully
        iDb = iOperation->TakeDatabase();
        IssueRequest( EStateDone );
        }
    else // some error happened
        {
        delete iOperation;
        iOperation = NULL;
        
        const TDesC& dbName = iStore.StoreProperties().Name().UriDes();
        TInt creating = EFalse;
        
        if( aError == KErrNotFound )
            {
            // Database not found --> Creating it
            creating = ETrue;
            TRAP( aError, iDb = CContactDatabase::CreateL( dbName ) );
            }
    
        if ( KErrDiskFull == aError )
            {
            // Try to release space for contact db opening
            if( iStore.RequestFreeDiskSpaceLC( KDiskSpaceForDbOpening ) )
                {
                aError = KErrNone;
                if( creating )
                    {
                    // Databse was not found --> Creating it
                    iDb = CContactDatabase::CreateL( dbName );
                    }
                else
                    {
                    // Opening database
                    iOperation = CContactDatabase::Open( dbName, iStatus );
                    SetActive();
                    iState = EStateStoreOpenedAfterError;
                    }
                CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC
                }
            }
        
        // leave on unhandled error
        User::LeaveIfError( aError );
        
        if( iDb )
            {
            IssueRequest( EStateDone );
            }
        }
    }

// --------------------------------------------------------------------------
// CContactStore::CContactStore
// --------------------------------------------------------------------------
//
CContactStore::CContactStore( CContactStoreDomain& aStoreDomain ) :
        iStoreDomain( aStoreDomain )
    {
    }

// --------------------------------------------------------------------------
// CContactStore::~CContactStore
// --------------------------------------------------------------------------
//
CContactStore::~CContactStore()
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::~CContactStore(0x%x)"), this);

    iNamedViewContainer.ResetAndDestroy();
    CloseSystemTemplate(iContactDb);
    iObservers.Close();
    delete iStoreInfo;
    delete iDbNotifier;
    delete iContactDb;
    delete iAsyncContactOperation;
    delete iAsyncOpenOp;
    delete iStoreURI;
    delete iFieldFactory;
    delete iFieldsInfo;
    delete iProperties;
    delete iDiskSpaceCheck;
    delete iOpenOperation;
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CContactStore::NewL
// --------------------------------------------------------------------------
//
CContactStore* CContactStore::NewL
        (const TVPbkContactStoreUriPtr& aURI,
        CContactStoreDomain& aStoreDomain)
    {
    CContactStore* self = new(ELeave) CContactStore(aStoreDomain);
    CleanupStack::PushL(self);
    self->ConstructL(aURI);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CContactStore::ConstructL
// --------------------------------------------------------------------------
//
inline void CContactStore::ConstructL(
        const TVPbkContactStoreUriPtr& aURI)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::ConstructL(%S)"), &aURI.UriDes());

    FeatureManager::InitializeLibL();
    iProperties = CVPbkContactStoreProperties::NewL();
    iStoreURI = CVPbkContactStoreUri::NewL(aURI);
    iProperties->SetName(iStoreURI->Uri());

    // open and read the contacts model resource file cntmodel.rsc
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC(iStoreDomain.FsSession(),
        KVPbkRomFileDrive, KDC_CONTACTS_RESOURCE_DIR, KCntResFile);

    TResourceReader reader;
    reader.SetBuffer( resFile.AllocReadLC(R_CNTUI_NEW_FIELD_DEFNS) );
    // create the fields info structure from the contacts model fields
    iFieldsInfo = CFieldsInfo::NewL(reader);
    CleanupStack::PopAndDestroy(2); // R_CNTUI_NEW_FIELD_DEFNS buffer, resFile

    // Extend the contacts model golden template with variant fields
    DoAddFieldTypesL();

    iAsyncOpenOp =
        CVPbkAsyncObjectOperation<MVPbkContactStoreObserver>::NewL();
    iAsyncContactOperation = CAsyncContactOperation::NewL(*this);

    iStoreInfo = CContactStoreInfo::NewL(*this, iStoreDomain);

    // Get contact store location drive for disk space check
    const TPtrC ptr( iStoreURI->Uri().Component
        ( TVPbkContactStoreUriPtr::EContactStoreUriStoreDrive ) );
    TInt drive( EDriveA ); // c is usually the default location
    User::LeaveIfError( iStoreDomain.FsSession().CharToDrive
        ( ptr[0], drive) );
    iDiskSpaceCheck = VPbkEngUtils::CVPbkDiskSpaceCheck::NewL
            ( iStoreDomain.FsSession(), drive );

    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::ConstructL end"));
    }

// --------------------------------------------------------------------------
// CContactStore::FieldTypeMap
// --------------------------------------------------------------------------
//
const CFieldTypeMap& CContactStore::FieldTypeMap() const
    {
    return iStoreDomain.FieldTypeMap();
    }

// --------------------------------------------------------------------------
// CContactStore::FieldFactory
// --------------------------------------------------------------------------
//
const CFieldFactory& CContactStore::FieldFactory() const
    {
    return *iFieldFactory;
    }

// --------------------------------------------------------------------------
// CContactStore::MasterFieldTypeList
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CContactStore::MasterFieldTypeList() const
    {
    return iStoreDomain.MasterFieldTypeList();
    }

// --------------------------------------------------------------------------
// CContactStore::CreateFieldLC
// --------------------------------------------------------------------------
//
CContactItemField* CContactStore::CreateFieldLC
        (const MVPbkFieldType& aFieldType) const
    {
    return iFieldFactory->CreateFieldLC(aFieldType);
    }

// --------------------------------------------------------------------------
// CContactStore::ReadContactL
// --------------------------------------------------------------------------
//
void CContactStore::ReadContactL
        (TContactItemId aContactId, MVPbkContactObserver& aObserver)
    {
    iAsyncContactOperation->PrepareL
        (MVPbkContactObserver::EContactRead, aContactId, aObserver);
    iAsyncContactOperation->Execute();
    }

// --------------------------------------------------------------------------
// CContactStore::LockContactL
// --------------------------------------------------------------------------
//
void CContactStore::LockContactL
        (const CContact& aContact, MVPbkContactObserver& aObserver)
    {
    iAsyncContactOperation->PrepareL
        (MVPbkContactObserver::EContactLock, aContact, aObserver);
    iAsyncContactOperation->Execute();
    }

// --------------------------------------------------------------------------
// CContactStore::ReadAndLockContactL
// --------------------------------------------------------------------------
//
void CContactStore::ReadAndLockContactL
        (TContactItemId aContactId, MVPbkContactObserver& aObserver)
    {
    iAsyncContactOperation->PrepareL
        (MVPbkContactObserver::EContactReadAndLock, aContactId, aObserver);
    iAsyncContactOperation->Execute();
    }

// --------------------------------------------------------------------------
// CContactStore::DeleteContactL
// --------------------------------------------------------------------------
//
void CContactStore::DeleteContactL
        (TContactItemId aContactId, MVPbkContactObserver& aObserver)
    {
    iAsyncContactOperation->PrepareL
        (MVPbkContactObserver::EContactDelete, aContactId, aObserver);
    iAsyncContactOperation->Execute();
    }

// --------------------------------------------------------------------------
// CContactStore::CommitContactL
// --------------------------------------------------------------------------
//
void CContactStore::CommitContactL(
        const CContact& aContactItem,
        MVPbkContactObserver& aObserver)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::CommitContactL(0x%x,0x%x)"),
        &aContactItem, &aObserver);

    iDiskSpaceCheck->DiskSpaceCheckL();

    // verify sync field content
    VPbkUtils::VerifySyncronizationFieldL(
        iStoreDomain.FsSession(),
        StoreProperties().SupportedFields(),
        const_cast<CContact&>(aContactItem));

    iAsyncContactOperation->PrepareL
        (MVPbkContactObserver::EContactCommit, aContactItem, aObserver);
    iAsyncContactOperation->Execute();
    }

// --------------------------------------------------------------------------
// CContactStore::CreateLinkLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CContactStore::CreateLinkLC(TContactItemId aContactId) const
    {
    MVPbkContactLink* ret = NULL;
    // if  aContactId is KGoldenTemplateId then it is probably new contact
    // which is not commited to contactdb, return null then
    if ( aContactId != KGoldenTemplateId &&
         aContactId != KNullContactId )
        {
        // Contact store constness casted away, no reason to be const
        ret = CContactLink::NewLC(const_cast<CContactStore&>(*this), aContactId);
        }
    return ret;
    }

// --------------------------------------------------------------------------
// CContactStore::ContactDestroyed
// --------------------------------------------------------------------------
//
void CContactStore::ContactDestroyed(CContactItem* aContactItem, TBool aCloseContact )
    {
    // Release any lock on contact.
    if (aContactItem)
        {
        // Cancel async operation if any
        iAsyncContactOperation->Cancel( aContactItem );

        //iContactDb may be already null if store configuration changed by
        //removing the current store from store configuartion
        if ( aCloseContact && iContactDb)
        	{
	        // CloseContactL doesn't leave despite its name.
	        iContactDb->CloseContactL(aContactItem->Id());
        	}
        }
    }

// --------------------------------------------------------------------------
// CContactStore::MatchContactStore
// --------------------------------------------------------------------------
//
TBool CContactStore::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    if (iStoreURI->Uri().Compare(
        TVPbkContactStoreUriPtr(aContactStoreUri),
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CContactStore::MatchContactStoreDomain
// --------------------------------------------------------------------------
//
TBool CContactStore::MatchContactStoreDomain(const TDesC& aContactStoreDomain) const
    {
    if (iStoreURI->Uri().Compare(
        aContactStoreDomain,
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType) == 0)
        {
        return ETrue;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateBookmarkLC
// --------------------------------------------------------------------------
//
MVPbkContactBookmark* CContactStore::CreateBookmarkLC(
        TContactItemId aContactId) const
    {
    // Contact store constness casted away, no reason to be const
    return CContactBookmark::NewLC( aContactId,
        const_cast<CContactStore&>(*this) );
    }

// --------------------------------------------------------------------------
// CContactStore::StoreProperties
// --------------------------------------------------------------------------
//
const MVPbkContactStoreProperties& CContactStore::StoreProperties() const
    {
    return *iProperties;
    }

// --------------------------------------------------------------------------
// CContactStore::OpenL
// --------------------------------------------------------------------------
//
void CContactStore::OpenL(
        MVPbkContactStoreObserver& aObserver)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::OpenL(0x%x)"), &aObserver);

    AddObserverL(aObserver);
    CVPbkAsyncObjectCallback<MVPbkContactStoreObserver>* openCallback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this,
            &CContactStore::DoOpenL,
            TBool(EFalse),
            &CContactStore::OpenError,
            aObserver);
    iAsyncOpenOp->CallbackL(openCallback);
    CleanupStack::Pop( openCallback );
    }

// --------------------------------------------------------------------------
// CContactStore::ReplaceL
// --------------------------------------------------------------------------
//
void CContactStore::ReplaceL(MVPbkContactStoreObserver& aObserver)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::ReplaceL(0x%x)"), &aObserver);

    AddObserverL(aObserver);
    CVPbkAsyncObjectCallback<MVPbkContactStoreObserver>* openCallback =
        VPbkEngUtils::CreateAsyncObjectCallbackLC(
            *this,
            &CContactStore::DoOpenL,
            TBool(ETrue),
            &CContactStore::OpenError,
            aObserver);
    iAsyncOpenOp->CallbackL(openCallback);
    CleanupStack::Pop(openCallback);
    }

// --------------------------------------------------------------------------
// CContactStore::Close
// --------------------------------------------------------------------------
//
void CContactStore::Close( MVPbkContactStoreObserver& aObserver)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::Close(0x%x)"), &aObserver);

    // Cancel open callback, If not active then it does nothing.
    iAsyncOpenOp->CancelCallback( &aObserver );
    // Remove the observer from the observer list.
    RemoveObserver(aObserver);
    if (iObservers.Count() == 0)
        {
        iNamedViewContainer.ResetAndDestroy();
        // Zero observers left, clean the resources.
        iAsyncOpenOp->Purge();
        delete iFieldFactory;
        iFieldFactory = NULL;
        delete iDbNotifier;
        iDbNotifier = NULL;
        delete iContactDb;
        iContactDb = NULL;
        // Set iGroupsFetched to false.
        // Make sure iGroupIds is updated again after store is opened again.
        iStoreInfo->ResetGroupsFetched();
        }
    }

// --------------------------------------------------------------------------
// CContactStore::CreateNewContactLC
// --------------------------------------------------------------------------
//
MVPbkStoreContact* CContactStore::CreateNewContactLC()
    {
    TRAPD( err, iDiskSpaceCheck->DiskSpaceCheckL() );
    if ( err != KErrNone )
        {
        CCoeEnv::Static()->HandleError( err );
        User::Leave(err);
        }
    CContactItem* newContact = CContactCard::NewLC();
    CContact* result = CContact::NewL(*this, newContact, ETrue);
    CleanupStack::Pop(newContact);
    CleanupStack::PushL(result);
    return result;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateNewContactGroupLC
// --------------------------------------------------------------------------
//
MVPbkContactGroup* CContactStore::CreateNewContactGroupLC()
    {
    // Check that store has been opened
    __ASSERT_DEBUG( iContactDb,
        ContactStorePanic( EPreCond_CreateNewContactGroupLC ) );

    iDiskSpaceCheck->DiskSpaceCheckL();
    CContactItem* newGroup = iContactDb->CreateContactGroupLC();
    TContactItemId groupId = newGroup->Id();
    CleanupStack::PopAndDestroy(); // newGroup

    CContactItem* group = iContactDb->OpenContactLX(groupId);
    CleanupStack::PushL(group);
    CContact* result = CContact::NewL(*this, group);
    CleanupStack::Pop(2); // group, lock
    CleanupStack::PushL(result);

    return result;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateViewLC
// --------------------------------------------------------------------------
//
MVPbkContactView* CContactStore::CreateViewLC(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::CreateViewLC(0x%x)"), &aSortOrder);

    CViewBase* result = NULL;
    if (aViewDefinition.SortPolicy() == EVPbkUnsortedContactView)
        {
        // Contact model doesn't have a view that is not sorted.
        User::Leave( KErrArgument );
        }
    else if (aViewDefinition.Type() == EVPbkContactsView)
        {
        if ( !aViewDefinition.FieldTypeFilter() && !aViewDefinition.ContactSelector() )
            {
            // View without filtering
            result = CContactView::NewLC( aViewDefinition, aObserver,
                    *this, aSortOrder );
            }
        else
            {
            // A filtered view
            result = CFilteredContactView::NewLC( aViewDefinition,
                    aObserver, *this, aSortOrder, iStoreDomain.FsSession() );
            }
        }
    else // aViewDefinition.Type() == EVPbkGroupsView
        {
        if ( !aViewDefinition.FieldTypeFilter() &&
        		!aViewDefinition.FlagIsOn( EVPbkExcludeEmptyGroups ) )
            {
            // View without filtering
            result = CGroupView::NewLC( aViewDefinition, aObserver, *this,
                aSortOrder );
            }
        else
            {
            // A filtered view
            result = CFilteredGroupView::NewLC( aViewDefinition,
                    aObserver, *this, aSortOrder, iStoreDomain.FsSession() );
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CContactStore::ContactGroupsLC
// --------------------------------------------------------------------------
//
MVPbkContactLinkArray* CContactStore::ContactGroupsLC() const
    {
    // Check that store has been opened
    __ASSERT_DEBUG( iContactDb,
        ContactStorePanic( EPreCond_ContactGroupsLC ) );

    CVPbkContactLinkArray* result = CVPbkContactLinkArray::NewLC();

    CContactIdArray* groups = iContactDb->GetGroupIdListL();
    if (groups)
        {
        CleanupStack::PushL(groups);
        const TInt count = groups->Count();
        for (TInt i = 0; i < count; ++i)
            {
            MVPbkContactLink* link = CreateLinkLC((*groups)[i]);
            result->AppendL(link);
            CleanupStack::Pop(); // link
            }
        CleanupStack::PopAndDestroy(); // groups
        }

    return result;
    }

// --------------------------------------------------------------------------
// CContactStore::StoreInfo
// --------------------------------------------------------------------------
//
const MVPbkContactStoreInfo& CContactStore::StoreInfo() const
    {
    return *iStoreInfo;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateLinkFromInternalsLC
// --------------------------------------------------------------------------
//
MVPbkContactLink* CContactStore::CreateLinkFromInternalsLC(
        RReadStream& aStream) const
    {
    // Contact store constness casted away, no reason to be const
    return CContactLink::NewLC(const_cast<CContactStore&>(*this), aStream);
    }


// --------------------------------------------------------------------------
// CContactStore::ContactStoreExtension
// --------------------------------------------------------------------------
//
TAny* CContactStore::ContactStoreExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KMVPbkContactStoreExtension2Uid )
        {
        return static_cast<MVPbkContactStore2*>( this );
        }

    return NULL;
    }


// --------------------------------------------------------------------------
// CContactStore::OwnContactLinkL
// --------------------------------------------------------------------------
//
MVPbkContactOperationBase* CContactStore::OwnContactLinkL(
		   MVPbkSingleContactLinkOperationObserver& aObserver) const
	{
    __ASSERT_DEBUG( iContactDb,
        ContactStorePanic( EPreCond_OwnContactLinkL ) );

	MVPbkContactLink* ownContactLink = NULL;
	TContactItemId ownCardId = iContactDb->OwnCardId();

	if ( ownCardId != KNullContactId )
		{
		ownContactLink = CContactLink::NewLC(
				const_cast<CContactStore&>(*this),
				ownCardId );
		}

    // if ownCard is KNullContactId, than return ownContactLink with NULL, async
	MVPbkContactOperationBase* result = new (ELeave) COwnContactLinkOperation(
			ownContactLink,
			aObserver );

	if ( ownCardId != KNullContactId )
		{
		CleanupStack::Pop(); //contact link
		}

	return result;
	}

// --------------------------------------------------------------------------
// CContactStore::SetOwnContactL
// --------------------------------------------------------------------------
//
void CContactStore::SetAsOwnL(
        const CContact& aContactItem,
        MVPbkContactObserver& aObserver )
	{
    iAsyncContactOperation->PrepareL
        (MVPbkContactObserver::EContactSetOwn, aContactItem, aObserver);
    iAsyncContactOperation->Execute();
	}

// --------------------------------------------------------------------------
// CContactStore::StoreOpenedL
// --------------------------------------------------------------------------
//
void CContactStore::StoreOpenedL( CContactDatabase* aDB )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("VPbkCntModel: CContactStore::StoreOpenedL"));
    
    CleanupStack::PushL( aDB );
    
    delete iOpenOperation;
    iOpenOperation = NULL;

    if( !iGoldenTemplateUpdated )
        {
        iSysTemplate = aDB->OpenContactL(aDB->TemplateId());
        // set contact model template label texts according to localisation
        // must be done in order for contact field labels to be correct after language change
        if( UpdateSystemTemplateFieldsL( *iSysTemplate, *iFieldsInfo ) )
            {
            aDB->CommitContactL( *iSysTemplate );
            iGoldenTemplateUpdated = ETrue;
            }
        CloseSystemTemplate( aDB );
    
        // reopen the contact database if the golden template has been changed
        if( iGoldenTemplateUpdated )
            {
            // template updated -> reopen store
            CleanupStack::PopAndDestroy( aDB );
            iOpenOperation = new(ELeave) CContactStoreOpenOperation( *this, EFalse );
            return;
            }
        }

    // read the system template and create the field factory accordingly
    iSysTemplate = aDB->ReadContactL(aDB->TemplateId());
    iFieldFactory = CFieldFactory::NewL(iStoreDomain.FieldTypeMap(),
        iSysTemplate, MasterFieldTypeList(), iStoreDomain.FsSession() );
    iSysTemplate = NULL;

    // set supported fields for this store
    iProperties->SetSupportedFields(*iFieldFactory);
    SetStaticPropertiesL();

    iDbNotifier = CContactChangeNotifier::NewL(*aDB, this);
    CleanupStack::Pop(aDB);
    iContactDb = aDB;

    // Activate remote views so that contacts server will build
    // the view as soon as possible when system boots.
    // This is trapped because it's not wanted to block
    // the store opening if the view building failed.
    TRAP_IGNORE( DoActivateRemoteViewsL() );

    // Send event to all observers. This means that multiple pending open requests
    // are all signalled as complete here and we can cancel all other open operations.
    SendEventL(iObservers, &MVPbkContactStoreObserver::StoreReady, *this);
    iAsyncOpenOp->Purge();
    }

// --------------------------------------------------------------------------
// CContactStore::StoreOpenFailed
// --------------------------------------------------------------------------
//
void CContactStore::StoreOpenFailed( TInt aError )
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("VPbkCntModel: CContactStore::StoreOpenFailed(%d)"), aError);
    
    delete iOpenOperation;
    iOpenOperation = NULL;
    
    SendErrorEvent(iObservers, &MVPbkContactStoreObserver::StoreUnavailable,
        *this, aError );
    }

// --------------------------------------------------------------------------
// CContactStore::RequestFreeDiskSpaceLC
// --------------------------------------------------------------------------
//
TBool CContactStore::RequestFreeDiskSpaceLC( TInt aSpace )
    {
    RSharedDataClient* sharedDataClient = iStoreDomain.SharedDataClient();
    if( sharedDataClient )
        {
        // Try to release space for contact db opening
        sharedDataClient->RequestFreeDiskSpaceLC( aSpace );
        return ETrue;
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateContactRetrieverL
// --------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateContactRetrieverL(
        const MVPbkContactLink& aLink,
        MVPbkSingleContactOperationObserver& aObserver)
    {
    // test if aLink is in fact a instance of this class
    CContactRetriever* operation = NULL;
    if (CContactLink::Matches(StoreProperties().Uri(), aLink))
        {
        if ( IsOpened() )
            {
            operation = CContactRetriever::NewLC(aLink, *this, aObserver);
            CleanupStack::Pop();
            }
        else
            {
            VPBK_DEBUG_PRINT( VPBK_DEBUG_STRING(
                "VPbkCntModel: ContactStore not opened" ) );
            User::Leave( KErrArgument );
            }
        }

    return operation;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateDeleteContactsOperationL
// --------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateDeleteContactsOperationL(
                const MVPbkContactLinkArray& aContactLinks,
                MVPbkBatchOperationObserver& aObserver)
    {
    CVPbkContactLinkArray* links = CVPbkContactLinkArray::NewLC();

    const TInt linkCount = aContactLinks.Count();
    for (TInt i = 0; i < linkCount; ++i)
        {
        const MVPbkContactLink& link = aContactLinks.At(i);
        if (CContactLink::Matches(StoreProperties().Uri(), link))
            {
            MVPbkContactLink* newLink = link.CloneLC();
            links->AppendL(newLink);
            CleanupStack::Pop(); // newLink
            }
        }

    MVPbkContactOperation* operation = NULL;
    if (links->Count() > 0)
        {
        __ASSERT_DEBUG( IsOpened(),
            VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );
        if ( IsOpened() )
            {
            operation = CDeleteContactsOperation::NewL(*this, *links, aObserver,
                                                       iStoreDomain.SharedDataClient(),
                                                       *iDiskSpaceCheck );
            }
        }
    CleanupStack::PopAndDestroy(); // links

    return operation;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateCommitContactsOperationL
// --------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateCommitContactsOperationL(
        const TArray<MVPbkStoreContact*>& aContacts,
        MVPbkBatchOperationObserver& aObserver)
    {
    CArrayPtrFlat<CContact>* contacts =
        new(ELeave) CArrayPtrFlat<CContact>(KDefaultArrayGranularity);
    CleanupStack::PushL(contacts);

    const TInt contactCount = aContacts.Count();
    for (TInt i = 0; i < contactCount; ++i)
        {
        MVPbkStoreContact* contact = aContacts[i];
        if (&contact->ParentStore() == this)
            {
            contacts->AppendL(static_cast<CContact*>(contact));

            // Validate syncronization field content
            VPbkUtils::VerifySyncronizationFieldL(
                iStoreDomain.FsSession(),
                StoreProperties().SupportedFields(),
                *(aContacts[i]));
            }
        }

    MVPbkContactOperation* operation = NULL;
    if (contacts->Count() > 0)
        {
        __ASSERT_DEBUG( IsOpened(),
            VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );
        if ( IsOpened() )
            {
            operation = CCommitContactsOperation::NewL(*this, contacts->Array(), aObserver);
            }
        }
    CleanupStack::PopAndDestroy(); // contacts

    return operation;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateMatchPhoneNumberOperationL
// --------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateMatchPhoneNumberOperationL(
        const TDesC& aPhoneNumber,
        TInt aMaxMatchDigits,
        MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* operation = NULL;
    if ( IsOpened() )
        {
        operation = CMatchPhoneNumberOperation::NewL(
            *this, aPhoneNumber, aMaxMatchDigits, aObserver);
        }
    return operation;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateFindOperationL
// --------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateFindOperationL(
        const TDesC& aSearchString,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* operation = NULL;
    if ( IsOpened() )
        {
        operation = CFindOperation::NewL(
            *this, aSearchString, aFieldTypes, aObserver);
        }
    return operation;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateFindOperationL
// --------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateFindOperationL(
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver,
        const TCallBack& aWordParserCallBack )
    {
    MVPbkContactOperation* operation = NULL;
    if ( IsOpened() )
        {
        operation = CFindInTextDefOperation::NewL( *this, aSearchStrings,
            aFieldTypes, aObserver, aWordParserCallBack );
        }
    return operation;
    }

// --------------------------------------------------------------------------
// CContactStore::CreateCompressStoresOperationL
// --------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateCompressStoresOperationL(
	MVPbkBatchOperationObserver& /*aObserver*/)
	{
	// Contacts model handles compress by itself nowadays
	return NULL;
	}

// --------------------------------------------------------------------------
// CContactStore::AddObserverL
// --------------------------------------------------------------------------
//
void CContactStore::AddObserverL(MVPbkContactStoreObserver& aObserver)
    {
    // adds aObserver to list of observers if it isnt there already, thus
    // avoids clients registering multiple times
    if (iObservers.Find(&aObserver) == KErrNotFound)
        {
        iObservers.AppendL( &aObserver );
        }
    }

// --------------------------------------------------------------------------
// CContactStore::RemoveObserver
// --------------------------------------------------------------------------
//
void CContactStore::RemoveObserver(MVPbkContactStoreObserver& aObserver)
    {
    const TInt pos = iObservers.Find(&aObserver);
    if (pos != KErrNotFound)
        {
        iObservers.Remove(pos);
        }
    }

// --------------------------------------------------------------------------
// CContactStore::IsOpened
// --------------------------------------------------------------------------
//
TBool CContactStore::IsOpened() const
    {
    if ( iContactDb )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// --------------------------------------------------------------------------
// CContactStore::HandleDatabaseEventL
// --------------------------------------------------------------------------
//
void CContactStore::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
    {
    // Note: prints Symbian event and contact id
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
    ("VPbkCntModel:CContactStore(0x%x)::HandleDatabaseEventL %d, cntId=%d"),
    this, aEvent.iType, aEvent.iContactId );

    TVPbkContactStoreEvent mappedEvent = MapDbEventToStoreEvent(aEvent);

    if (aEvent.iContactId != KNullContactId)
        {
        mappedEvent.iContactLink = CreateLinkLC(aEvent.iContactId);
        }

    if (mappedEvent.iEventType != TVPbkContactStoreEvent::ENullEvent)
        {
        SendEventL(iObservers, &MVPbkContactStoreObserver::HandleStoreEventL,
            *this, mappedEvent);
        }

    // Backup handling. Send unavailable event when backup/restore begins
    // and ready event when it completes.
    switch( mappedEvent.iEventType )
        {
        case TVPbkContactStoreEvent::EStoreRestoreBeginning: // FALLTHROUGH
        case TVPbkContactStoreEvent::EStoreBackupBeginning:
            {
            SendErrorEvent( iObservers,
                &MVPbkContactStoreObserver::StoreUnavailable, *this,
                KErrAccessDenied );
            break;
            }
        case TVPbkContactStoreEvent::EStoreBackupRestoreCompleted:
            {
            SendEventL( iObservers, &MVPbkContactStoreObserver::StoreReady,
                *this );
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if ( mappedEvent.iContactLink )
        {
        CleanupStack::PopAndDestroy(); // mappedEvent.iContactLink;
        }
    }

// --------------------------------------------------------------------------
// CContactStore::DoOpenL
// --------------------------------------------------------------------------
//
void CContactStore::DoOpenL(MVPbkContactStoreObserver& aObserver, TBool aReplace)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("CContactStore::DoOpenL(0x%x)"), &aObserver);

    if (!iContactDb)
        {
        // start opening store if it's not already pending
        if( !iOpenOperation )
            {
            iOpenOperation = new(ELeave) CContactStoreOpenOperation( *this, EFalse );
            }
        }
    else
        {
        // In case the contact database is already open, we come here
        SendEventL(&aObserver, &MVPbkContactStoreObserver::StoreReady, *this);
        }
    }

// --------------------------------------------------------------------------
// CContactStore::OpenError
// --------------------------------------------------------------------------
//
void CContactStore::OpenError(MVPbkContactStoreObserver& aObserver,
        TInt aError)
    {
    VPBK_DEBUG_PRINT(VPBK_DEBUG_STRING
        ("VPbkCntModel: CContactStore::OpenError(%d,0x%x)"), aError, &aObserver);

    SendErrorEvent(iObservers, &MVPbkContactStoreObserver::StoreUnavailable,
        *this, aError);
    }

// --------------------------------------------------------------------------
// CContactStore::SetStaticPropertiesL
// --------------------------------------------------------------------------
//
inline void CContactStore::SetStaticPropertiesL()
    {
    iProperties->SetBooleanProperty
        (CVPbkContactStoreProperties::EPropertyFieldLabelSupported, ETrue);
    iProperties->SetBooleanProperty
        (CVPbkContactStoreProperties::EPropertyDefaultsSupported, ETrue);
    iProperties->SetBooleanProperty
        (CVPbkContactStoreProperties::EPropertyVoiceTagsSupported,
         VoiceTagSupported() );
    iProperties->SetBooleanProperty
        (CVPbkContactStoreProperties::EPropertySpeedDialsSupported, ETrue);
    iProperties->SetBooleanProperty
        (CVPbkContactStoreProperties::EPropertyContactGroupsSupported, ETrue);
    iProperties->SetBooleanProperty
        (CVPbkContactStoreProperties::EPropertyOwnContactSupported, ETrue);
    // Contacts Model store is never remote store but always in device memory
    // or memory card.
    iProperties->SetBooleanProperty(
        CVPbkContactStoreProperties::EPropertyLocal, ETrue );
    // The store is always persistent because contacts are saved in data base.
    iProperties->SetBooleanProperty(
        CVPbkContactStoreProperties::EPropertyPersistent, ETrue );

    // For setting removable property we need the drive letter from the URI
    TDriveUnit driveUnit( iStoreURI->Uri().Component(
        TVPbkContactStoreUriPtr::EContactStoreUriStoreDrive ) );
    // Then the removable information can be received from TDriveInfo
    TVolumeInfo volInfo;
    User::LeaveIfError( iStoreDomain.FsSession().Volume(
        volInfo, driveUnit ) );
    if ( volInfo.iDrive.iDriveAtt & KDriveAttRemovable )
        {
        iProperties->SetBooleanProperty(
            CVPbkContactStoreProperties::EPropertyRemovable, ETrue );
        }
    }

// --------------------------------------------------------------------------
// CContactStore::DoAddFieldTypesL
// --------------------------------------------------------------------------
//
void CContactStore::DoAddFieldTypesL()
    {
    CVPbkLocalVariationManager* localVariation =
        CVPbkLocalVariationManager::NewL();
    CleanupStack::PushL(localVariation);

    // open the golden template extension resource file
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC( iStoreDomain.FsSession(),
        KVPbkRomFileDrive, KDC_RESOURCE_FILES_DIR, KCntExtResFile );

    // Nick name field
    if ( localVariation->LocallyVariatedFeatureEnabled(
            EVPbkLVAddNickNameField ) )
        {
        TResourceReader reader;
        reader.SetBuffer(
            resFile.AllocReadLC( R_CNTUI_ADD_NICK_NAME_FIELD_DEFNS) );
        iFieldsInfo->AppendFieldsL( reader );
        // R_CNTUI_ADD_NICK_NAME_FIELD_DEFNS buffer
        CleanupStack::PopAndDestroy();
        }

    // Video telephony fields
    if ( localVariation->LocallyVariatedFeatureEnabled(
            EVPbkLVAddVideoTelephonyFields) )
        {
        TResourceReader reader;
        reader.SetBuffer(
            resFile.AllocReadLC( R_CNTUI_ADD_VIDEO_TELEPHONY_FIELD_DEFNS) );
        iFieldsInfo->AppendFieldsL( reader );
        // R_CNTUI_ADD_VIDEO_TELEPHONY_FIELD_DEFNS buffer
        CleanupStack::PopAndDestroy();
        }

    // VOIP fields
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVVOIP ) )
        {
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC( R_CNTUI_ADD_VOIP_FIELD_DEFNS) );
        iFieldsInfo->AppendFieldsL( reader );
        CleanupStack::PopAndDestroy(); // R_CNTUI_ADD_VOIP_FIELD_DEFNS buffer
        }

    // PTT/POC field
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVPOC ) )
        {
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC( R_CNTUI_ADD_POC_FIELD_DEF ) );
        iFieldsInfo->AppendFieldsL( reader );
        CleanupStack::PopAndDestroy(); // R_CNTUI_ADD_POC_FIELD_DEF buffer
        }

    // Share View field
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVShareView ) )
        {
        TResourceReader reader;
        reader.SetBuffer(
            resFile.AllocReadLC( R_CNTUI_ADD_SHARE_VIEW_FIELD_DEF ) );
        iFieldsInfo->AppendFieldsL( reader );
        // R_CNTUI_ADD_SHARE_VIEW_FIELD_DEF buffer
        CleanupStack::PopAndDestroy();
        }


    // OLD VOIP fields for compatibility
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVVOIP ) )
        {
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC(
            R_CNTUI_ADD_OLD_VOIP_FIELD_DEFNS ) );
        iFieldsInfo->AppendFieldsL( reader );
        // R_CNTUI_ADD_OLD_VOIP_FIELD_DEFNS buffer
        CleanupStack::PopAndDestroy();
        }


    // OLD PTT/POC field for compatibility
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVPOC ) )
        {
        TResourceReader reader;
        reader.SetBuffer(
            resFile.AllocReadLC( R_CNTUI_ADD_OLD_POC_FIELD_DEF ) );
        iFieldsInfo->AppendFieldsL( reader );
        CleanupStack::PopAndDestroy(); // R_CNTUI_ADD_OLD_POC_FIELD_DEF buffer
        }


    // OLD Share View field for compatibility
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVShareView ) )
        {
        TResourceReader reader;
        reader.SetBuffer(
            resFile.AllocReadLC( R_CNTUI_ADD_OLD_SHARE_VIEW_FIELD_DEF ) );
        iFieldsInfo->AppendFieldsL( reader );
        // R_CNTUI_ADD_OLD_SHARE_VIEW_FIELD_DEF buffer
        CleanupStack::PopAndDestroy();
        }

    // Generic SIP field
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVAddSIPFields ) )
        {
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC( R_CNTUI_ADD_SIP_FIELD_DEF ) );
        iFieldsInfo->AppendFieldsL( reader );
        CleanupStack::PopAndDestroy(); // R_CNTUI_ADD_SIP_FIELD_DEF buffer
        }

    // COD fields
    if ( FeatureManager::FeatureSupported( KFeatureIdCallImagetext ) )
        {
        TResourceReader reader;
        reader.SetBuffer( resFile.AllocReadLC( R_CNTUI_ADD_COD_FIELD_DEFNS ) );
        iFieldsInfo->AppendFieldsL( reader );
        CleanupStack::PopAndDestroy(); // R_CNTUI_ADD_COD_FIELD_DEFNS buffer
	    }

    // MDO fields
    if ( localVariation->LocallyVariatedFeatureEnabled( EVPbkLVAddMDOFields ) )
        {
        TResourceReader reader;
        reader.SetBuffer(
            resFile.AllocReadLC( R_CNTUI_ADD_PREFIX_SUFFIX_FIELD_DEFNS ) );
        iFieldsInfo->AppendFieldsL( reader );
        // R_CNTUI_ADD_PREFIX_SUFFIX_FIELD_DEFNS buffer
        CleanupStack::PopAndDestroy();
        }

     // MegaOperator fields
    if ( localVariation->LocallyVariatedFeatureEnabled(
            EVPbkLVMegaOperatorFields ) )
        {
        TResourceReader reader;
        reader.SetBuffer(
            resFile.AllocReadLC( R_CNTUI_ADD_MEGAOP_FIELD_DEFNS ) );
        iFieldsInfo->AppendFieldsL( reader );
        CleanupStack::PopAndDestroy(); // R_CNTUI_ADD_MEGAOP_FIELD_DEFNS buffer
        }

    CleanupStack::PopAndDestroy(2); // resFile, localVariation
    }

// --------------------------------------------------------------------------
// CContactStore::CloseSystemTemplate
// --------------------------------------------------------------------------
//
void CContactStore::CloseSystemTemplate(CContactDatabase* aContactDb)
    {
    if(iSysTemplate && aContactDb)
        {
        // according to contacts model documentation closecontactL
        // cannot leave, so this is safe.
        aContactDb->CloseContactL(iSysTemplate->Id());
        delete iSysTemplate;
        iSysTemplate = NULL;
        }
    }

// --------------------------------------------------------------------------
// CContactStore::OpenL
// --------------------------------------------------------------------------
//
CContactDatabase* CContactStore::OpenInternalL()
    {
    TBool creating = EFalse;

    CContactDatabase* db = NULL;

    TPtrC dbName( StoreProperties().Name().UriDes() );
    TRAPD( err, db = CContactDatabase::OpenL(dbName) );

    if ( KErrNotFound == err )
        {
        // Database not found --> Creating it
        creating = ETrue;
        TRAP( err, db = CContactDatabase::CreateL(dbName) );
        }

    RSharedDataClient* sharedDataClient = iStoreDomain.SharedDataClient();
    if ( KErrDiskFull == err && sharedDataClient )
        {
        // Try to release space for contact db opening
        sharedDataClient->RequestFreeDiskSpaceLC(KDiskSpaceForDbOpening);
        if (creating)
            {
            // Databse was not found --> Creating it
            db = CContactDatabase::CreateL(dbName);
            }
        else
            {
            // Opening database
            db = CContactDatabase::OpenL(dbName);
            }
        CleanupStack::PopAndDestroy();  // RequestFreeDiskSpaceLC
        }
    else
        {
        User::LeaveIfError(err);
        }
    return db;
    }

// --------------------------------------------------------------------------
// CContactStore::DoActivateRemoteViewsL
// --------------------------------------------------------------------------
//
void CContactStore::DoActivateRemoteViewsL()
    {
    // create the parameter object
    CVPbkSortOrderAcquirer::TSortOrderAcquirerParam param(
            MasterFieldTypeList() );
    // load ecom plugins to acquire the sort orders from the presentation
    // layer
    CSortOrderAcquirerList* ecomList = CSortOrderAcquirerList::NewLC(param);

    const TInt count = ecomList->Count();
    for ( TInt i = 0; i < count; ++i )
        {
        CVPbkSortOrderAcquirer& acquirer = ecomList->At( i );
        if ( acquirer.ApplySortOrderToStoreL( iStoreURI->Uri() ) )
            {
            RContactViewSortOrder cntSortOrder = CreateSortOrderL(
                acquirer.SortOrder() );
            CleanupClosePushL(cntSortOrder);
            // Info is always found because acquirer is from same
            // CSortOrderAcquirerList instance.
            const CImplementationInformation* info =
                    ecomList->FindInfo(acquirer);
            TContactViewPreferences viewPrefs = ConvertViewPrefsL(
                    info->OpaqueData());

            CNamedRemoteViewHandle* handle = CNamedRemoteViewHandle::NewLC();
            // create remote view
            handle->CreateRemoteViewL( info->DisplayName(), *iContactDb,
                    cntSortOrder, viewPrefs );
            // append handle to container
            iNamedViewContainer.AppendL( handle );
            CleanupStack::Pop( handle );
            CleanupStack::PopAndDestroy(); // cntSortOrder
            }
        }
    CleanupStack::PopAndDestroy( ecomList );
    }

// --------------------------------------------------------------------------
// CContactStore::CreateSortOrderL
// --------------------------------------------------------------------------
//
RContactViewSortOrder CContactStore::CreateSortOrderL(
        const MVPbkFieldTypeList& aSortOrder) const
    {
    RContactViewSortOrder sortOrder;
    CleanupClosePushL(sortOrder);

    const TInt count = aSortOrder.FieldTypeCount();
    for ( TInt i = 0; i < count; ++i )
        {
        const MVPbkFieldType& fieldType = aSortOrder.FieldTypeAt(i);
        // Ignores all field types that are not supported by this store
        const CContactItemField* field = iFieldFactory->FindField(
            fieldType);
        if (field)
            {
            sortOrder.AppendL(field->ContentType().FieldType(0));
            }
        }
    CleanupStack::Pop(); // sortOrder
    return sortOrder;
    }

// --------------------------------------------------------------------------
// CContactStore::ConvertViewPrefsL
// --------------------------------------------------------------------------
//
TContactViewPreferences CContactStore::ConvertViewPrefsL(const TDesC8& aData)
    {
    // convert character to TContactViewPreferences
    TLex8 lexer(aData);
    TUint32 output;
    User::LeaveIfError(lexer.Val(output, EDecimal));

    TContactViewPreferences prefs = static_cast<TContactViewPreferences>(
            output);
    return prefs;
    }

// --------------------------------------------------------------------------
// CContactStore::ContactStoreDomainFsSession
// --------------------------------------------------------------------------
//
RFs& CContactStore::ContactStoreDomainFsSession()
    {
    return iStoreDomain.FsSession();
    }

} /// namespace VPbkCntModel

// End of File
