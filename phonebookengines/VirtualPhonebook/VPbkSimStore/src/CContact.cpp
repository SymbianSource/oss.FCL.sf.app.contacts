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
* Description:  A contact adapter between VPbk framework and VPbkSimStoreImpl
*
*/


// INCLUDES
#include "CContact.h"

#include "CContactStore.h"
#include "CFieldTypeMappings.h"
#include "CViewContact.h"
#include "MVPbkContactObserver.h"
#include "CSupportedFieldTypes.h"
#include "CContactOperationCallback.h"
#include "VPbkSimStoreError.h"

#include <CVPbkAsyncOperation.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkSimContact.h>
#include <MVPbkSimCntStore.h>
#include <MVPbkContactViewBase.h>
#include <MVPbkContactStoreProperties.h>
#include <RVPbkStreamedIntArray.h>
#include <TVPbkSimStoreProperty.h>
#include <VPbkSimCntFieldTypes.hrh>
#include <MVPbkSimStoreOperation.h>
#include <VPbkError.h>
#include <MVPbkStoreContactProperties.h>

namespace {

// MODULE DATA STRUCTURES
enum TContactFlags
    {
    KNewContact = 1
    };

// ============================= LOCAL FUNCTIONS ===============================

MVPbkContactObserver::TContactOp ConvertContactOperation(
    MVPbkSimContactObserver::TEvent aEvent )
    {
    MVPbkContactObserver::TContactOp op = 
        MVPbkContactObserver::EContactOperationUnknown;
    switch ( aEvent )
        {
        case MVPbkSimContactObserver::EDelete:
            {
            op = MVPbkContactObserver::EContactDelete;
            break;
            }
        case MVPbkSimContactObserver::ESave:
            {
            op = MVPbkContactObserver::EContactCommit;
            break;
            }
        default:
            {
            op = MVPbkContactObserver::EContactOperationUnknown;
            break;
            }
        }
    return op;
    }

TInt MaxNumberOfFieldsInContact( TVPbkSimCntFieldType aType,
        TVPbkUSimStoreProperty& aUsimProp )
    {
    TInt result = 0;
    switch ( aType )
        {
        case EVPbkSimReading: // FALLTHROUGH
        case EVPbkSimNickName: // FALLTHROUGH
        case EVPbkSimName:
            {
            ++result; // only one name field can exist
            break;
            }
        case EVPbkSimEMailAddress:
            {
            if (aUsimProp.iMaxNumOfEmails != KVPbkSimStorePropertyUndefined)
                {
                result = aUsimProp.iMaxNumOfEmails;
                }
            break;
            }
        case EVPbkSimGsmNumber: // FALLTHROUGH
        case EVPbkSimAdditionalNumber:
            {
            ++result; // always at least one number
            if ( aUsimProp.iMaxNumOfAnrs != KVPbkSimStorePropertyUndefined )
                {
                result += aUsimProp.iMaxNumOfAnrs;
                }
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    return result;
    }
}

namespace VPbkSimStore {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContact::CContact
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CContact::CContact( CContactStore& aParentStore ) 
:   iParentStore( aParentStore )
    {
    }

// -----------------------------------------------------------------------------
// CContact::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CContact::ConstructL( CVPbkSimContact& aSimContact, 
    TBool aIsNewContact )
    {
    if ( aIsNewContact )
        {
        iFlags.Set( KNewContact );
        }
    iFields.SetContact( *this, aSimContact );
    iAsyncOp = new( ELeave ) VPbkEngUtils::CVPbkAsyncOperation;
    }

// -----------------------------------------------------------------------------
// CContact::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CContact* CContact::NewL( CContactStore& aParentStore,
    CVPbkSimContact* aSimContact, TBool aIsNewContact )
    {
    CContact* self = new ( ELeave ) CContact( aParentStore );
    CleanupStack::PushL(self);
    self->ConstructL( *aSimContact, aIsNewContact );
    CleanupStack::Pop( self );
    // Take ownership after construction
    self->iSimContact = aSimContact;
    return self;
    }

// Destructor
CContact::~CContact()
    {
    delete iStoreOperation;
    delete iAsyncOp;
    delete iSimContact;
    }

// -----------------------------------------------------------------------------
// CContact::ParentObject
// -----------------------------------------------------------------------------
//
MVPbkObjectHierarchy& CContact::ParentObject() const
    {
    return iParentStore;
    }

// -----------------------------------------------------------------------------
// CContact::ConstFields
// -----------------------------------------------------------------------------
//
const MVPbkStoreContactFieldCollection& CContact::Fields() const
    {
    return iFields;
    }

// -----------------------------------------------------------------------------
// CContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CContact::IsSame( const MVPbkStoreContact& aOtherContact ) const
    {
    if ( &ParentStore() == &aOtherContact.ParentStore() )
        {
        const CContact& otherCnt = static_cast<const CContact&>( aOtherContact );
        return otherCnt.SimContact().SimIndex() == iSimContact->SimIndex();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContact::CreateLinkLC
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CContact::CreateLinkLC() const
    {
    return iParentStore.CreateLinkLC( iSimContact->SimIndex() );
    }

// -----------------------------------------------------------------------------
// CContact::DeleteL
// -----------------------------------------------------------------------------
//
void CContact::DeleteL( MVPbkContactObserver& aObserver ) const
    {
    if ( iStoreOperation )
        {
        User::Leave( KErrInUse );
        }
            
    // From the client point of view the MVPbkStoreContact is constant but
    // implementation needs a non const contact.
    RVPbkStreamedIntArray indexArray;
    CleanupClosePushL( indexArray );
    indexArray.AppendIntL(iSimContact->SimIndex() );
    iStoreOperation = iParentStore.NativeStore().DeleteL( indexArray, 
        const_cast<CContact&>(*this));
    iObserver = &aObserver;
    CleanupStack::PopAndDestroy(); // indexArray 
    }

// -----------------------------------------------------------------------------
// CContact::MatchContactStore
// -----------------------------------------------------------------------------
//    
TBool CContact::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iParentStore.MatchContactStore(aContactStoreUri);
    }

// -----------------------------------------------------------------------------
// CContact::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//    
TBool CContact::MatchContactStoreDomain(const TDesC& aContactStoreDomain) const
    {
    return iParentStore.MatchContactStoreDomain(aContactStoreDomain);
    }

// -----------------------------------------------------------------------------
// CContact::CreateBookmarkLC
// -----------------------------------------------------------------------------
//
MVPbkContactBookmark* CContact::CreateBookmarkLC() const
    {
    return iParentStore.CreateBookmarkLC( iSimContact->SimIndex() );
    }
    
// -----------------------------------------------------------------------------
// CContact::ParentStore
// -----------------------------------------------------------------------------
//
MVPbkContactStore& CContact::ParentStore() const
    {
    return iParentStore;
    }

// -----------------------------------------------------------------------------
// CContact::Fields
// -----------------------------------------------------------------------------
//
MVPbkStoreContactFieldCollection& CContact::Fields()
    {
    return iFields;
    }

// -----------------------------------------------------------------------------
// CContact::CreateFieldLC
// -----------------------------------------------------------------------------
//
MVPbkStoreContactField* CContact::CreateFieldLC(
    const MVPbkFieldType& aFieldType ) const
    {
    if ( !iParentStore.SupportedFieldTypes().ContainsSame( aFieldType ) )
        {
        // According to contact API the function must leave if 
        // the type is invalid
        User::Leave( KErrNotSupported );
        }
    TVPbkSimCntFieldType simType = 
        iParentStore.FieldTypeMappings().Match( aFieldType );
    
    __ASSERT_DEBUG( simType != EVPbkSimUnknownType,
        VPbkSimStore::Panic( ESimFieldTypeNotFound ) );

    if ( simType == EVPbkSimGsmNumber || 
         simType == EVPbkSimAdditionalNumber )
        {
        // EVPbkSimGsmNumber and EVPbkSimAdditionalNumber maps to same
        // VPbk field type. A sim contact can have only one EVPbkSimGsmNumber
        // field type and possibly many EVPbkSimAdditionalNumber types 
        // depending on the USIM store.
        // However, if SIM card doesn't support additional number it is allowed
        // to create as many EVPbkSimGsmNumber fields as client wants. This
        // is needed to enable temporary contacts that holds multiple numbers.
        CVPbkSimContact::TFieldLookup lookup = 
            iSimContact->FindField( EVPbkSimGsmNumber );
        if ( lookup.EndOfLookup() || 
             !( iParentStore.SimStoreCapabilities() & 
                VPbkSimStoreImpl::KAdditionalNumUsed ))
            {
            simType = EVPbkSimGsmNumber;
            }
        else
            {
            simType = EVPbkSimAdditionalNumber;
            }
        }

    CVPbkSimCntField* field = iSimContact->CreateFieldLC( simType );
    TContactNewField* fieldWrapper = new( ELeave ) TContactNewField( field );
    fieldWrapper->SetParentContact( const_cast<CContact&>( *this ) );
    CleanupStack::Pop( field );
    CleanupDeletePushL( fieldWrapper );
    return fieldWrapper;
    }

// -----------------------------------------------------------------------------
// CContact::AddFieldL
// -----------------------------------------------------------------------------
//
TInt CContact::AddFieldL( MVPbkStoreContactField* aField )
    {
    __ASSERT_ALWAYS( aField, VPbkError::Panic( VPbkError::ENullContactField ) );
    // Test that the client gives this contact's field and doesn't pass
    // an existing field of this contact as  a new one
    __ASSERT_ALWAYS( &aField->ParentContact() == this &&
        aField != iFields.FieldPointer(), 
        VPbkError::Panic( VPbkError::EInvalidContactField ) );
    
    // Now the wrapper is casted back
    TContactNewField* fieldWrapper = static_cast<TContactNewField*>( aField );
    // Takes ownership of the field from the wrapper
    CVPbkSimCntField* field = fieldWrapper->SimField();
    CleanupStack::PushL( field );
    // Add sim field to sim contact. Contact owns the field now.
    iSimContact->AddFieldL( field );
    CleanupStack::Pop( field );
    // After this the function must not leave because client has created
    // the fieldWrapper with CreateFieldLC and it's in the CleanupStack
    // Client pops the fieldWrapper after this function
    delete fieldWrapper;
    // The field is appended to the array -> return the last field index.
    return iSimContact->FieldCount() - 1;
    }

// -----------------------------------------------------------------------------
// CContact::RemoveField
// -----------------------------------------------------------------------------
//
void CContact::RemoveField( TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iFields.FieldCount(), 
        VPbkError::Panic(VPbkError::EInvalidFieldIndex) );
    __ASSERT_ALWAYS( !iParentStore.StoreProperties().ReadOnly(),
        VPbkError::Panic(VPbkError::EInvalidAccessToReadOnlyContact ) );

    iSimContact->DeleteField( aIndex );
    }

// -----------------------------------------------------------------------------
// CContact::RemoveAllFields
// -----------------------------------------------------------------------------
//
void CContact::RemoveAllFields()
    {
    __ASSERT_ALWAYS( !iParentStore.StoreProperties().ReadOnly(),
        VPbkError::Panic(VPbkError::EInvalidAccessToReadOnlyContact ) );

    iSimContact->DeleteAllFields();
    }

// -----------------------------------------------------------------------------
// CContact::LockL
// -----------------------------------------------------------------------------
//
void CContact::LockL( MVPbkContactObserver& aObserver ) const
    {
    MVPbkContactObserver::TContactOpResult opResult;
    opResult.iExtension = NULL;
    opResult.iOpCode = MVPbkContactObserver::EContactLock;
    opResult.iStoreContact = NULL;
    
    CContactOperationCallback* callBack = 
        new( ELeave ) CContactOperationCallback( opResult, aObserver, 
        KErrNone );
    CleanupStack::PushL( callBack );
    iAsyncOp->CallbackL( callBack );
    CleanupStack::Pop( callBack );
    iLocked = ETrue;
    }

// -----------------------------------------------------------------------------
// CContact::CommitL
// -----------------------------------------------------------------------------
//
void CContact::CommitL( MVPbkContactObserver& aObserver ) const
    {
    if ( iLocked || iFlags.IsSet( KNewContact ))
        {
        if ( iStoreOperation )
            {
            User::Leave( KErrInUse );
            }
        // From the client point of view the MVPbkStoreContact is constant but
        // implementation needs a non const contact.
        iStoreOperation = iSimContact->SaveL( const_cast<CContact&>( *this ));
        iObserver = &aObserver;
        }
    else
        {
        // Virtual Phonebook API demands that contact must be locked before
        // CommitL. Sim Store has to behave according to that and 
        // complete with KErrAccessDenied.
        MVPbkContactObserver::TContactOpResult opResult;
        opResult.iExtension = NULL;
        opResult.iOpCode = MVPbkContactObserver::EContactCommit;
        opResult.iStoreContact = NULL;
        CContactOperationCallback* callBack = 
            new( ELeave ) CContactOperationCallback( 
            opResult, aObserver, KErrAccessDenied );
        CleanupStack::PushL( callBack );
        iAsyncOp->CallbackL( callBack );
        CleanupStack::Pop( callBack );
        }
    }

// -----------------------------------------------------------------------------
// CContact::GroupsJoinedLC
// -----------------------------------------------------------------------------
//
MVPbkContactLinkArray* CContact::GroupsJoinedLC() const
    {
    return CVPbkContactLinkArray::NewLC();
    }
   
// -----------------------------------------------------------------------------
// CContact::Group
// -----------------------------------------------------------------------------
//
MVPbkContactGroup* CContact::Group()
    {
    return NULL;
    }    

// -----------------------------------------------------------------------------
// CContact::MaxNumberOfFieldL
// -----------------------------------------------------------------------------
//
TInt CContact::MaxNumberOfFieldL( const MVPbkFieldType& aType ) const
    {
    TInt res = 0;
    TVPbkSimCntFieldType nativeType = 
        iParentStore.FieldTypeMappings().Match( aType );
    if ( nativeType != EVPbkSimUnknownType )
        {
        TVPbkUSimStoreProperty usimProp;
        User::LeaveIfError(iParentStore.NativeStore().GetUSimStoreProperties( 
            usimProp ));
        res = MaxNumberOfFieldsInContact( nativeType, usimProp );
        }
    return res;
    }

// -----------------------------------------------------------------------------
// CContact::ContactEventComplete
// -----------------------------------------------------------------------------
//
void CContact::ContactEventComplete( TEvent aEvent, 
    CVPbkSimContact* /*aContact*/ )
    {
    __ASSERT_DEBUG( iObserver, Panic(EPreCond_CContact_ContactEventComplete));
    
    delete iStoreOperation;
    iStoreOperation = NULL;
    
    MVPbkContactObserver::TContactOpResult vpbkOpResult;
    vpbkOpResult.iStoreContact = NULL;
    vpbkOpResult.iExtension = NULL;
    vpbkOpResult.iOpCode = ConvertContactOperation( aEvent );
    
    MVPbkContactObserver* observer = iObserver;
    ResetContactOperationState();
    observer->ContactOperationCompleted( vpbkOpResult );
    }

// -----------------------------------------------------------------------------
// CContact::ContactEventError
// -----------------------------------------------------------------------------
//
void CContact::ContactEventError( TEvent aEvent, 
    CVPbkSimContact* /*aContact*/, TInt aError )
    {
    __ASSERT_DEBUG( iObserver, Panic(EPreCond_CContact_ContactEventError));
    
    delete iStoreOperation;
    iStoreOperation = NULL;
    
    MVPbkContactObserver* observer = iObserver;
    ResetContactOperationState();
    MVPbkContactObserver::TContactOp op = ConvertContactOperation( aEvent );
    observer->ContactOperationFailed( op, aError, EFalse );
    } 

// -----------------------------------------------------------------------------
// CContact::ResetContactOperationState
// -----------------------------------------------------------------------------
//
void CContact::ResetContactOperationState()
    {
    iObserver = NULL;
    }

TAny* CContact::StoreContactExtension(TUid aExtensionUid)
{
    
    if( aExtensionUid == KMVPbkStoreContactExtension2Uid )
		return static_cast<MVPbkStoreContact2*>( this );
    return NULL;
}


	
MVPbkStoreContactProperties* CContact::PropertiesL() const
    {
    //sim  store doesn't support any of MVPbkStoreContactProperties functions
    return NULL;
    }

void CContact::SetAsOwnL( MVPbkContactObserver& /*aObserver*/ ) const
	{
	// own link is not supported in sim store    
	User::Leave( KErrNotSupported );
	}

} // namespace VPbkSimStore

// end of file

