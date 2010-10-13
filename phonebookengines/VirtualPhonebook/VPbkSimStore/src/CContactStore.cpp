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
* Description:  The virtual phonebook store implementation
*
*/


// INCLUDES
#include "CContactStore.h"

#include "CContactView.h"
#include "CContact.h"
#include "CRemoteStore.h"
#include "CFieldTypeMappings.h"
#include "CContactStoreDomain.h"
#include "CCommitContactsOperation.h"
#include "CDeleteContactsOperation.h"
#include "CMatchPhoneNumberOperation.h"
#include "CFindOperation.h"
#include "CFindWithParserOperation.h"
#include "CContactRetriever.h"
#include "CSupportedFieldTypes.h"
#include "CContactLink.h"
#include "CContactOperationCallback.h"
#include "CContactStoreInfo.h"

#include "VPbkSimStoreError.h"
#include "VPbkStoreUriLiterals.h"

#include <CVPbkContactStoreUri.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreObserver.h>
#include <CVPbkSimContact.h>
#include <TVPbkSimStoreProperty.h>
#include <CVPbkContactStoreProperties.h>
#include <VPbkError.h>
#include <VPbkSimStoreTemplateFunctions.h>
#include <CVPbkAsyncCallback.h>

namespace VPbkSimStore {

// LOCAL
namespace {

// CONSTANTS

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// GetStoreIdentifier
// Converts Virtual phonebook store name to sim store identifier
// -----------------------------------------------------------------------------
//
TVPbkSimStoreIdentifier GetStoreIdentifier( const TDesC& aStoreName )
    {
    TVPbkSimStoreIdentifier identifier = EVPbkSimAdnGlobalStore;
    if ( aStoreName.Compare( KVPbkSimGlobalAdnURI ) == 0 )
        {
        identifier = EVPbkSimAdnGlobalStore;
        }
    else if ( aStoreName.Compare( KVPbkSimGlobalSdnURI ) == 0 )
        {
        identifier = EVPbkSimSdnGlobalStore;
        }
    else if ( aStoreName.Compare( KVPbkSimGlobalFdnURI ) == 0 )
        {
        identifier = EVPbkSimFdnGlobalStore;
        }
    else if ( aStoreName.Compare( KVPbkSimGlobalOwnNumberURI ) == 0 )
        {
        identifier = EVPbkSimONStore;
        }
    else
        {
        VPbkSimStore::Panic( VPbkSimStore::EStoreNameMappingFailed );
        }
    return identifier;
    }
}  // unnamed namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactStore::CContactStore
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CContactStore::CContactStore( CContactStoreDomain& aStoreDomain )
:   iStoreDomain( aStoreDomain ),
    iStoreState( EStoreNotOpen )
    {
    }

// -----------------------------------------------------------------------------
// CContactStore::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CContactStore::ConstructL( const TVPbkContactStoreUriPtr& aURI )
    {
    iProperties = CVPbkContactStoreProperties::NewL();
    iStoreURI = CVPbkContactStoreUri::NewL( aURI );
    iProperties->SetName( iStoreURI->Uri() );
    iAsyncOpenOp =
        CVPbkAsyncObjectOperation<MVPbkContactStoreObserver>::NewL();

    iStoreInfo = CContactStoreInfo::NewL( *this );
    iNativeStore = CRemoteStore::NewL( GetStoreIdentifier(
                    iStoreURI->Uri().UriDes() ) );
    }

// -----------------------------------------------------------------------------
// CContactStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStore* CContactStore::NewL( const TVPbkContactStoreUriPtr& aURI,
    CContactStoreDomain& aStoreDomain )
    {
    CContactStore* self = new ( ELeave ) CContactStore( aStoreDomain );
    CleanupStack::PushL( self );
    self->ConstructL( aURI );
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CContactStore::~CContactStore()
    {
    iStoreObservers.Close();
    delete iStoreInfo;
    delete iSupportedFieldTypes;
    delete iAsyncOpenOp;
    delete iStoreURI;
    if ( iNativeStore )
        {
        iNativeStore->Close( *this );
        delete iNativeStore;
        }
    delete iProperties;
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
// CContactStore::MasterFieldTypeList
// -----------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CContactStore::MasterFieldTypeList() const
    {
    return iStoreDomain.MasterFieldTypeList();
    }

// -----------------------------------------------------------------------------
// CContactStore::NativeStore
// -----------------------------------------------------------------------------
//
MVPbkSimCntStore& CContactStore::NativeStore()
    {
    return *iNativeStore;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateLinkLC
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CContactStore::CreateLinkLC( TInt aSimIndex ) const
    {
    MVPbkContactLink* ret = NULL;
    if ( aSimIndex != KVPbkSimStoreFirstFreeIndex )
        {
        // Contact store constness casted away, no reason to be const
        ret = CContactLink::NewLC( const_cast<CContactStore&>(*this), aSimIndex );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CContactStore::ReadContactL
// -----------------------------------------------------------------------------
//
CContact* CContactStore::ReadContactL( TInt aSimIndex )
    {
    __ASSERT_DEBUG( iStoreState != EStoreNotOpen,
        VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );

    // Read sim contact synchronously
    const TDesC8* etelCnt = iNativeStore->AtL( aSimIndex );
    if( !etelCnt )
        {
        User::Leave( KErrNotFound );
        }
    CVPbkSimContact* simCnt =
        CVPbkSimContact::NewLC( *etelCnt, *iNativeStore );
    // Create vpbk contact
    CContact* cnt = CContact::NewL( *this, simCnt );
    CleanupStack::Pop( simCnt );
    return cnt;
    }

// -----------------------------------------------------------------------------
// CContactStore::MatchContactStore
// -----------------------------------------------------------------------------
//
TBool CContactStore::MatchContactStore( const TDesC& aContactStoreUri ) const
    {
    if ( iStoreURI->Uri().Compare(
        TVPbkContactStoreUriPtr( aContactStoreUri ),
        TVPbkContactStoreUriPtr::EContactStoreUriAllComponents ) == 0 )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStore::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//
TBool CContactStore::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain ) const
    {
    if ( iStoreURI->Uri().Compare(
        aContactStoreDomain,
        TVPbkContactStoreUriPtr::EContactStoreUriStoreType ) == 0 )
        {
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateBookmarkLC
// -----------------------------------------------------------------------------
//
MVPbkContactBookmark* CContactStore::CreateBookmarkLC( TInt aSimIndex ) const
    {
    __ASSERT_DEBUG( iStoreState != EStoreNotOpen,
        VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );
    // Bookmark is implemented as a link in this store
    // Contact store constness casted away, no reason to be const
    return CContactLink::NewLC( const_cast<CContactStore&>(*this), aSimIndex );
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateCommitContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateCommitContactsOperationL(
    const TArray<MVPbkStoreContact*>& aContacts,
    MVPbkBatchOperationObserver& aObserver )
    {

    RArray<MVPbkStoreContact*> simContacts;
    CleanupClosePushL( simContacts );
    const TInt count = aContacts.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( &aContacts[i]->ParentStore() == this )
            {
            simContacts.AppendL( aContacts[i] );
            }
        }

    CCommitContactsOperation* cntOp = NULL;
    if ( simContacts.Count() > 0 )
        {
        __ASSERT_DEBUG( iStoreState != EStoreNotOpen,
            VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );
        if (iStoreState == EStoreOpen )
            {
            cntOp = CCommitContactsOperation::NewL( *this, aObserver,
                simContacts.Array() );
            }

        }
    CleanupStack::PopAndDestroy(); // simContacts
    return cntOp;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateContactRetrieverL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateContactRetrieverL(
    const MVPbkContactLink& aLink,
    MVPbkSingleContactOperationObserver& aObserver )
    {

    if ( CContactLink::IsValid( *this, aLink ) )
        {
        __ASSERT_DEBUG( iStoreState != EStoreNotOpen,
            VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );
        if ( iStoreState == EStoreOpen )
            {
            const CContactLink& link = static_cast<const CContactLink&>( aLink );
            return CContactRetriever::NewL( *this, link.SimIndex(), aObserver );
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateDeleteContactsOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateDeleteContactsOperationL(
    const MVPbkContactLinkArray& aContactLinks,
    MVPbkBatchOperationObserver& aObserver )
    {

    MVPbkContactOperation* operation = NULL;
    if ( !iProperties->ReadOnly() )
        {
        const TInt count = aContactLinks.Count();
        const TInt granularity = 20;
        RArray<TInt> indexes( granularity );
        CleanupClosePushL( indexes );
        for ( TInt i = 0; i < count; ++i )
            {
            if ( &aContactLinks.At(i).ContactStore() == this )
                {
                const CContactLink& link =
                    static_cast<const CContactLink&>( aContactLinks.At(i) );
                indexes.AppendL( link.SimIndex() );
                }

            }

        if ( indexes.Count() > 0 )
            {
            __ASSERT_DEBUG( iStoreState != EStoreNotOpen,
                VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );
            if ( iStoreState == EStoreOpen )
                {
                operation = CDeleteContactsOperation::NewL( *this, aObserver,
                    indexes.Array() );
                }
            }
        CleanupStack::PopAndDestroy(); // indexes
        }
    return operation;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateMatchPhoneNumberOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateMatchPhoneNumberOperationL(
    const TDesC& aPhoneNumber, TInt aMaxMatchDigits,
    MVPbkContactFindObserver& aObserver )
    {

    if ( iStoreState == EStoreOpen )
        {
        return CMatchPhoneNumberOperation::NewL( aPhoneNumber,
            aMaxMatchDigits, aObserver, *this );
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateFindOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateFindOperationL(
        const TDesC& aSearchString,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindObserver& aObserver)
    {

    if ( iStoreState == EStoreOpen )
        {
        return CFindOperation::NewL( *this, aObserver, aSearchString,
            aFieldTypes );
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateFindOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateFindOperationL(
        const MDesC16Array& aSearchStrings,
        const MVPbkFieldTypeList& aFieldTypes,
        MVPbkContactFindFromStoresObserver& aObserver,
        const TCallBack& aWordParserCallBack )
    {

    if ( iStoreState == EStoreOpen )
        {
        return CFindWithParserOperation::NewL( *this, aSearchStrings,
            aFieldTypes, aObserver, aWordParserCallBack );
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateCompressStoresOperationL
// -----------------------------------------------------------------------------
//
MVPbkContactOperation* CContactStore::CreateCompressStoresOperationL(
		MVPbkBatchOperationObserver& /*aObserver*/)
	{
	return NULL;
	}

// -----------------------------------------------------------------------------
// CContactStore::StoreProperties
// -----------------------------------------------------------------------------
//
const MVPbkContactStoreProperties& CContactStore::StoreProperties() const
    {
    return *iProperties;
    }

// -----------------------------------------------------------------------------
// CContactStore::OpenL
// -----------------------------------------------------------------------------
//
void CContactStore::OpenL( MVPbkContactStoreObserver& aObserver )
    {
    switch ( iStoreState )
        {
        case EStoreNotOpen:
            {
            // Close first to ensure that store doesn't register twice.
            iNativeStore->Close( *this );
            iNativeStore->OpenL( iStoreDomain.SecurityInformation(), *this );
            break;
            }
        case EStoreOpen:
        case EStoreNotAvailable:
        default:
            {
            CVPbkAsyncObjectCallback<MVPbkContactStoreObserver>* callback =
                VPbkEngUtils::CreateAsyncObjectCallbackLC(
                    *this,
                    &CContactStore::DoOpenL,
                    &CContactStore::DoOpenError,
                    aObserver);
            iAsyncOpenOp->CallbackL( callback );
            CleanupStack::Pop( callback );
            break;
            }
        }

    // adds aObserver to list of observers if it isn't there already, thus
    // avoids clients registering multiple times
    if ( iStoreObservers.Find(&aObserver) == KErrNotFound )
        {
        iStoreObservers.AppendL( &aObserver );
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::ReplaceL
// -----------------------------------------------------------------------------
//
void CContactStore::ReplaceL(MVPbkContactStoreObserver& aObserver)
    {
    OpenL( aObserver );
    }

// -----------------------------------------------------------------------------
// CContactStore::Close
// -----------------------------------------------------------------------------
//
void CContactStore::Close(MVPbkContactStoreObserver& aObserver)
    {
    iAsyncOpenOp->CancelCallback( &aObserver );

    const TInt pos = iStoreObservers.Find( &aObserver );
    if ( pos != KErrNotFound )
        {
        iStoreObservers.Remove( pos );
        }

    if ( iStoreObservers.Count() == 0 )
        {
        iNativeStore->Close( *this );
        iAsyncOpenOp->Purge();
        iStoreState = EStoreNotOpen;
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateNewContactLC
// -----------------------------------------------------------------------------
//
MVPbkStoreContact* CContactStore::CreateNewContactLC()
    {
    __ASSERT_DEBUG( iStoreState != EStoreNotOpen,
        VPbkError::Panic( VPbkError::EAccessOfUnopenedStore ) );

    CVPbkSimContact* simCnt = CVPbkSimContact::NewLC( *iNativeStore );
    CContact* cnt = CContact::NewL( *this, simCnt, ETrue );
    CleanupStack::Pop( simCnt );
    CleanupStack::PushL( cnt );
    return cnt;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateNewContactGroupLC
// -----------------------------------------------------------------------------
//
MVPbkContactGroup* CContactStore::CreateNewContactGroupLC()
    {
    User::Leave(KErrNotSupported);
    return NULL;
    }

// -----------------------------------------------------------------------------
// CContactStore::CreateViewLC
// -----------------------------------------------------------------------------
//
MVPbkContactView* CContactStore::CreateViewLC(
        const CVPbkContactViewDefinition& aViewDefinition,
        MVPbkContactViewObserver& aObserver,
        const MVPbkFieldTypeList& aSortOrder )
    {
    return CContactView::NewLC( aObserver, *this, aSortOrder, aViewDefinition );
    }

// -----------------------------------------------------------------------------
// CContactStore::ContactGroupsLC
// -----------------------------------------------------------------------------
//
MVPbkContactLinkArray* CContactStore::ContactGroupsLC() const
    {
    return CVPbkContactLinkArray::NewLC();
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
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CContactStore::CreateLinkFromInternalsLC(
        RReadStream& aStream) const
    {
    // Contact store constness casted away, no reason to be const
    return CContactLink::NewLC( const_cast<CContactStore&>(*this), aStream );
    }



// -----------------------------------------------------------------------------
// CContactStore::StoreReady
// -----------------------------------------------------------------------------
//
void CContactStore::StoreReady( MVPbkSimCntStore& /*aStore*/ )
    {
    TVPbkGsmStoreProperty gsmProperties;
    TInt res = iNativeStore->GetGsmStoreProperties( gsmProperties );

    if ( res == KErrNone )
        {
        delete iSupportedFieldTypes;
        iSupportedFieldTypes = NULL;
        TRAP( res, iSupportedFieldTypes = CSupportedFieldTypes::NewL(
            iStoreDomain.FieldTypeMappings(), gsmProperties ) );
        iSimStoreCapabilities = gsmProperties.iCaps;
        }

    if ( res != KErrNone )
        {
        iStoreState = EStoreNotAvailable;
        VPbkSimStoreImpl::SendObserverMessageRV( iStoreObservers,
            &MVPbkContactStoreObserver::StoreUnavailable, *this, res );
        }
    else
        {
        iProperties->SetSupportedFields( *iSupportedFieldTypes );
        // After sim store is opened get read-only information and set properties
        SetStaticProperties( gsmProperties );
        iStoreState = EStoreOpen;
        VPbkSimStoreImpl::SendObserverMessageR( iStoreObservers,
            &MVPbkContactStoreObserver::StoreReady, *this );
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::StoreReady
// -----------------------------------------------------------------------------
//
void CContactStore::StoreError( MVPbkSimCntStore& /*aStore*/, TInt aError )
    {
    iStoreState = EStoreNotAvailable;
    VPbkSimStoreImpl::SendObserverMessageRV( iStoreObservers,
        &MVPbkContactStoreObserver::StoreUnavailable, *this, aError );
    }

// -----------------------------------------------------------------------------
// CContactStore::StoreNotAvailable
// -----------------------------------------------------------------------------
//
void CContactStore::StoreNotAvailable( MVPbkSimCntStore& /*aStore*/ )
    {
    iStoreState = EStoreNotAvailable;
    VPbkSimStoreImpl::SendObserverMessageRV( iStoreObservers,
        &MVPbkContactStoreObserver::StoreUnavailable, *this,
        KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactStore::StoreContactEvent
// -----------------------------------------------------------------------------
//
void CContactStore::StoreContactEvent( TEvent aEvent, TInt aSimIndex )
    {
    CContactLink* link = NULL;
    TRAPD( result,
        {
        link = CContactLink::NewLC( *this, aSimIndex );
        CleanupStack::Pop( link );
        });
    if ( result != KErrNone )
        {
        // First param not used in StoreError()
        StoreError( *iNativeStore, result );
        return;
        }

    switch ( aEvent )
        {
        case EContactAdded:
            {
            TVPbkContactStoreEvent event(TVPbkContactStoreEvent::EContactAdded, link);
            VPbkSimStoreImpl::SendObserverMessageRV( iStoreObservers,
                &MVPbkContactStoreObserver::HandleStoreEventL, *this, event);
            break;
            }
        case EContactDeleted:
            {
            TVPbkContactStoreEvent event(TVPbkContactStoreEvent::EContactDeleted, link);
            VPbkSimStoreImpl::SendObserverMessageRV( iStoreObservers,
                &MVPbkContactStoreObserver::HandleStoreEventL, *this, event);
            break;
            }
        case EContactChanged:
            {
            TVPbkContactStoreEvent event(TVPbkContactStoreEvent::EContactChanged, link);
            VPbkSimStoreImpl::SendObserverMessageRV( iStoreObservers,
                &MVPbkContactStoreObserver::HandleStoreEventL, *this, event);
            break;
            }
        default:
            {
            __ASSERT_DEBUG( EFalse,
                VPbkSimStore::Panic( VPbkSimStore::EUnknownSimStoreEvent ) );
            VPbkSimStoreImpl::SendObserverMessageRV( iStoreObservers,
                &MVPbkContactStoreObserver::StoreUnavailable, *this,
                KErrUnknown );
            break;
            }
        }

    delete link;
    }

// -----------------------------------------------------------------------------
// CContactStore::DoOpenL
// -----------------------------------------------------------------------------
//
void CContactStore::DoOpenL( MVPbkContactStoreObserver& aObserver )
    {
    if ( iStoreState == EStoreOpen )
        {
        aObserver.StoreReady( *this );
        }
    else
        {
        aObserver.StoreUnavailable( *this, KErrNotSupported );
        }
    }

// -----------------------------------------------------------------------------
// CContactStore::DoOpenError
// -----------------------------------------------------------------------------
//
void CContactStore::DoOpenError( MVPbkContactStoreObserver& aObserver,
        TInt aError )
    {
    aObserver.StoreUnavailable( *this, aError );
    }

// -----------------------------------------------------------------------------
// CContactStore::SetStaticProperties
// -----------------------------------------------------------------------------
//
void CContactStore::SetStaticProperties( TVPbkGsmStoreProperty& aProperties )
    {
    // SIM resides in device -> local store
    iProperties->SetBooleanProperty(
        CVPbkContactStoreProperties::EPropertyLocal, ETrue );
    // In some devices SIM can be also removed from the device run time
    // -> removable store
    iProperties->SetBooleanProperty(
        CVPbkContactStoreProperties::EPropertyRemovable, ETrue );
    // If client changes the SIM store the changes are saved persistently
    // -> persistent store.
    iProperties->SetBooleanProperty(
        CVPbkContactStoreProperties::EPropertyPersistent, ETrue );

    if ( !( aProperties.iCaps & VPbkSimStoreImpl::KWriteAccess ) )
        {
        iProperties->SetBooleanProperty(
            CVPbkContactStoreProperties::EPropertyReadOnly, ETrue );
        }
    }

// --------------------------------------------------------------------------
// CContactStore::ContactStoreDomainFsSession
// --------------------------------------------------------------------------
//
RFs& CContactStore::ContactStoreDomainFsSession()
    {
    return iStoreDomain.FsSession();
    }

} // namespace VPbkSimStore

