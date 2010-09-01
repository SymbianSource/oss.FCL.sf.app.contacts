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
* Description:  A virtual phonebook view contact implementation
*
*/



// INCLUDE FILES
#include "CViewContact.h"

#include <MVPbkFieldType.h>
#include <CVPbkSimCntField.h>
#include <CVPbkSimContact.h>
#include "CContactStore.h"
#include "CFieldTypeMappings.h"
#include "CContactView.h"
#include "CContact.h"
#include "CContactOperationCallback.h"
#include "CRemoteStore.h"
#include "VPbkSimStoreError.h"

namespace VPbkSimStore {

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TViewContactFieldData::TViewContactFieldData
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TViewContactFieldData::TViewContactFieldData( const TDesC& aData )
:   iData( aData )
    {
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::IsEmpty
// -----------------------------------------------------------------------------
//
TBool TViewContactFieldData::IsEmpty() const
    {
    return iData.Length() == 0;
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::CopyL
// -----------------------------------------------------------------------------
//
void TViewContactFieldData::CopyL( const MVPbkContactFieldData& /*aFieldData*/ )
    {
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::Text
// -----------------------------------------------------------------------------
//
TPtrC TViewContactFieldData::Text() const
    {
    return iData;
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::SetTextL
// -----------------------------------------------------------------------------
//
void TViewContactFieldData::SetTextL( const TDesC& aText )
    {
    iData.Set( aText );
    }

// -----------------------------------------------------------------------------
// TViewContactFieldData::SetTextL
// -----------------------------------------------------------------------------
//
TInt TViewContactFieldData::MaxLength() const
    {
    return iData.Length();
    }

// -----------------------------------------------------------------------------
// TViewContactField::TViewContactField
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TViewContactField::TViewContactField( CViewContact& aParentContact,
    const MVPbkFieldType& aFieldType, const TDesC& aData )
:   iParentContact( aParentContact ),
    iFieldType( aFieldType ),
    iFieldData( aData )
    {
    }

// -----------------------------------------------------------------------------
// TViewContactField::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& TViewContactField::ParentContact() const
    {
    return iParentContact;
    }

// -----------------------------------------------------------------------------
// TViewContactField::MatchFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TViewContactField::MatchFieldType( 
    TInt /*aMatchPriority*/ ) const
    {
    return &iFieldType;
    }

// -----------------------------------------------------------------------------
// TViewContactField::BestMatchingFieldType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* TViewContactField::BestMatchingFieldType() const
    {
    return &iFieldType;
    }

// -----------------------------------------------------------------------------
// TViewContactField::ConstFieldData
// -----------------------------------------------------------------------------
//
const MVPbkContactFieldData& TViewContactField::FieldData() const
    {
    return iFieldData;
    }

// -----------------------------------------------------------------------------
// TViewContactField::IsSame
// -----------------------------------------------------------------------------
//
TBool TViewContactField::IsSame( 
    const MVPbkBaseContactField& aOther ) const
    {
    if ( &aOther.ParentContact() == &ParentContact() &&
         &aOther.FieldData() == &FieldData() )
        {
        return ETrue;
        }
    return EFalse;
    }


// Destructor
CViewContactFieldCollection::~CViewContactFieldCollection()
    {
    iFields.Close();
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::SetParentContact
// -----------------------------------------------------------------------------
//
void CViewContactFieldCollection::SetParentContact( 
    CViewContact& aParentContact )
    {
    iParentContact = &aParentContact;
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::ResetFields
// -----------------------------------------------------------------------------
//
void CViewContactFieldCollection::ResetFields()
    {
    iFields.Reset();
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::AppendFieldL
// -----------------------------------------------------------------------------
//
void CViewContactFieldCollection::AppendFieldL( TViewContactField& aNewField )
    {
    iFields.AppendL( aNewField );
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::ParentContact
// -----------------------------------------------------------------------------
//
MVPbkBaseContact& CViewContactFieldCollection::ParentContact() const
    {
    return *iParentContact;
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::FieldCount
// -----------------------------------------------------------------------------
//
TInt CViewContactFieldCollection::FieldCount() const
    {
    return iFields.Count();
    }

// -----------------------------------------------------------------------------
// CViewContactFieldCollection::FieldAt
// -----------------------------------------------------------------------------
//
const MVPbkBaseContactField& CViewContactFieldCollection::FieldAt( 
    TInt aIndex ) const
    {
    return iFields[aIndex];
    }

// -----------------------------------------------------------------------------
// CViewContact::CViewContact
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CViewContact::CViewContact( CContactView& aView, 
    const MVPbkFieldTypeList& aSortOrder )
    :   iView( aView ),
        iSortOrder( &aSortOrder )
    {
    }

// -----------------------------------------------------------------------------
// CViewContact::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CViewContact::ConstructL()
    {
    iAsyncOp = new( ELeave ) VPbkEngUtils::CVPbkAsyncOperation;
    iFieldCollection.SetParentContact( *this );
    }
    
// -----------------------------------------------------------------------------
// CViewContact::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CViewContact* CViewContact::NewL( CContactView& aView,
    const MVPbkFieldTypeList& aSortOrder )
    {
    CViewContact* self = new( ELeave ) CViewContact( aView, aSortOrder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CViewContact::~CViewContact()
    {
    delete iStoreOperation;
    delete iAsyncOp;    
    }

// -----------------------------------------------------------------------------
// CViewContact::SetSimContactL
// -----------------------------------------------------------------------------
//
void CViewContact::SetSimContactL( MVPbkSimContact& aSimContact )
    {
    // View contact must have the same fields as in the sort order that was
    // given by VPbk client.
    iFieldCollection.ResetFields();
    CFieldTypeMappings& mappings = iView.Store().FieldTypeMappings();
    TInt count = aSimContact.FieldCount();
    TInt typeCount = iSortOrder->FieldTypeCount();
    // Loop all field types in the sort order and try to find fields
    // from the sim contact.
    for ( TInt i = 0; i < typeCount; ++i )
        {
        const CVPbkSimCntField* simField = NULL;
        const MVPbkFieldType& sortOrderType = iSortOrder->FieldTypeAt( i );
        for ( TInt j = 0; j < count; ++j )
            {
            const MVPbkFieldType* vpbkType = mappings.Match( *iSortOrder, 
                aSimContact.ConstFieldAt( j ).Type() );
            if ( vpbkType && vpbkType == &sortOrderType )
                {
                simField = &aSimContact.ConstFieldAt( j );
                break;
                }
            }

        if ( simField )
            {
            // SIM field was found for the sort order type
            TViewContactField field( *this, sortOrderType, simField->Data() );
            iFieldCollection.AppendFieldL( field );
            }
        else
            {
            // SIM field was NOT found for the sort order type. Set empty data,
            TViewContactField field( *this, sortOrderType, KNullDesC );
            iFieldCollection.AppendFieldL( field );
            }
        }

    iSimContact = &aSimContact;
    }

// -----------------------------------------------------------------------------
// CViewContact::SimIndex
// -----------------------------------------------------------------------------
//
TInt CViewContact::SimIndex() const
    {
    if ( iSimContact )
        {
        return iSimContact->SimIndex();
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CViewContact::SetSortOrder
// -----------------------------------------------------------------------------
//
void CViewContact::SetSortOrder( const MVPbkFieldTypeList& aSortOrder )
    {
    iSortOrder = &aSortOrder;
    }

// -----------------------------------------------------------------------------
// CViewContact::NativeContact
// -----------------------------------------------------------------------------
//
const MVPbkSimContact* CViewContact::NativeContact() const
    {
    return iSimContact;
    }
    
// -----------------------------------------------------------------------------
// CViewContact::View
// -----------------------------------------------------------------------------
//
CContactView& CViewContact::View() const
    {
    return iView;
    }

// -----------------------------------------------------------------------------
// CViewContact::Fields
// -----------------------------------------------------------------------------
//
const MVPbkBaseContactFieldCollection& CViewContact::Fields() const
    {
    return iFieldCollection;
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame( const MVPbkStoreContact& aOtherContact ) const
    {
    if ( &iView.ContactStore() == &aOtherContact.ContactStore() )
        {
        const CVPbkSimContact& simContact = 
            static_cast<const CContact&>( aOtherContact ).SimContact();
        return SimIndex() == simContact.SimIndex();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame( const MVPbkViewContact& aOtherContact ) const
    {
    return aOtherContact.IsSame(*this, &iView.ContactStore());
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame(const MVPbkViewContact& aOtherContact, 
                           const MVPbkContactStore* aContactStore) const
    {
    if (aContactStore == &iView.ContactStore())
        {
        return static_cast<const CViewContact&>(aOtherContact).SimIndex() == 
            SimIndex();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CViewContact::IsSame
// -----------------------------------------------------------------------------
//
TBool CViewContact::IsSame(const MVPbkStoreContact& aOtherContact, 
                           const MVPbkContactStore* aContactStore) const
    {
    if (aContactStore == &iView.ContactStore())
        {
        const CVPbkSimContact& simContact = 
            static_cast<const CContact&>( aOtherContact ).SimContact();
        return SimIndex() == simContact.SimIndex();
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CViewContact::CreateLinkLC
// -----------------------------------------------------------------------------
//
MVPbkContactLink* CViewContact::CreateLinkLC() const
    {
    return iView.Store().CreateLinkLC( SimIndex() );
    }

// -----------------------------------------------------------------------------
// CViewContact::DeleteL
// -----------------------------------------------------------------------------
//
void CViewContact::DeleteL( MVPbkContactObserver& aObserver ) const
    {
    if ( iStoreOperation )
        {
        User::Leave( KErrInUse );
        }
        
    // From the client point of view the MVPbkStoreContact is constant but
    // implementation needs a non const contact.
    RVPbkStreamedIntArray indexArray;
    CleanupClosePushL( indexArray );
    indexArray.AppendIntL( SimIndex() );
    iStoreOperation = iView.Store().NativeStore().DeleteL( indexArray, 
        const_cast<CViewContact&>(*this));
    iObserver = &aObserver;
    CleanupStack::PopAndDestroy(); // indexArray 
    }

// -----------------------------------------------------------------------------
// CViewContact::MatchContactStore
// -----------------------------------------------------------------------------
//        
TBool CViewContact::MatchContactStore(const TDesC& aContactStoreUri) const
    {
    return iView.MatchContactStore(aContactStoreUri);
    }

// -----------------------------------------------------------------------------
// CViewContact::MatchContactStoreDomain
// -----------------------------------------------------------------------------
//        
TBool CViewContact::MatchContactStoreDomain(
        const TDesC& aContactStoreDomain) const
    {
    return iView.MatchContactStoreDomain(aContactStoreDomain);
    }

// -----------------------------------------------------------------------------
// CViewContact::CreateBookmarkLC
// -----------------------------------------------------------------------------
//
MVPbkContactBookmark* CViewContact::CreateBookmarkLC() const
    {
    return iView.Store().CreateBookmarkLC( SimIndex() );
    }
    
// -----------------------------------------------------------------------------
// CViewContact::ParentView
// -----------------------------------------------------------------------------
//
MVPbkContactViewBase& CViewContact::ParentView() const
    {
    return iView;
    }

// -----------------------------------------------------------------------------
// CViewContact::ParentView
// -----------------------------------------------------------------------------
//
void CViewContact::ReadL( MVPbkContactObserver& aObserver ) const
    {
    MVPbkContactObserver::TContactOpResult opResult;
    opResult.iExtension = NULL;
    opResult.iOpCode = MVPbkContactObserver::EContactRead;
    opResult.iStoreContact = iView.Store().ReadContactL( SimIndex() );
    
    CreateReadCallbackL( aObserver, opResult );
    }

// -----------------------------------------------------------------------------
// CViewContact::ReadAndLockL
// -----------------------------------------------------------------------------
//
void CViewContact::ReadAndLockL( MVPbkContactObserver& aObserver ) const
    {
    MVPbkContactObserver::TContactOpResult opResult;
    opResult.iExtension = NULL;
    opResult.iOpCode = MVPbkContactObserver::EContactReadAndLock;
    CContact* cnt = iView.Store().ReadContactL( SimIndex() );
    cnt->SetLock( ETrue );
    opResult.iStoreContact = cnt;
    
    CreateReadCallbackL( aObserver, opResult );
    }

// -----------------------------------------------------------------------------
// CViewContact::Expandable
// -----------------------------------------------------------------------------
//
MVPbkExpandable* CViewContact::Expandable() const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CViewContact::ContactEventComplete
// -----------------------------------------------------------------------------
//
void CViewContact::ContactEventComplete( TEvent /*aEvent*/, 
    CVPbkSimContact* /*aContact*/ )
    {
    __ASSERT_DEBUG( iObserver, 
        Panic(EPreCond_CViewContact_ContactEventComplete));
    
    delete iStoreOperation;
    iStoreOperation = NULL;
    
    MVPbkContactObserver::TContactOpResult vpbkOpResult;
    vpbkOpResult.iStoreContact = NULL;
    vpbkOpResult.iExtension = NULL;
    vpbkOpResult.iOpCode = MVPbkContactObserver::EContactDelete;
    
    MVPbkContactObserver* observer = iObserver;
    iObserver = NULL;
    observer->ContactOperationCompleted( vpbkOpResult );
    }

// -----------------------------------------------------------------------------
// CViewContact::ContactEventError
// -----------------------------------------------------------------------------
//
void CViewContact::ContactEventError( TEvent /*aEvent*/, 
    CVPbkSimContact* /*aContact*/, TInt aError )
    {
    __ASSERT_DEBUG( iObserver, Panic(EPreCond_CViewContact_ContactEventError));
    
    delete iStoreOperation;
    iStoreOperation = NULL;
    
    MVPbkContactObserver* observer = iObserver;
    iObserver = NULL;
    MVPbkContactObserver::TContactOp op = MVPbkContactObserver::EContactDelete;
    observer->ContactOperationFailed( op, aError, EFalse );
    } 

// -----------------------------------------------------------------------------
// CViewContact::CreateReadCallbackL
// -----------------------------------------------------------------------------
//
void CViewContact::CreateReadCallbackL( MVPbkContactObserver& aObserver,
    MVPbkContactObserver::TContactOpResult& aOpResult ) const
    {
    CleanupDeletePushL( aOpResult.iStoreContact );
    CContactOperationCallback* callBack = 
        new( ELeave ) CContactOperationCallback( aOpResult, aObserver, 
        KErrNone );
    CleanupStack::Pop( aOpResult.iStoreContact );
    CleanupStack::PushL( callBack );
    iAsyncOp->CallbackL( callBack );
    CleanupStack::Pop( callBack );
    }
} // namespace VPbkSimStore
//  End of File  
