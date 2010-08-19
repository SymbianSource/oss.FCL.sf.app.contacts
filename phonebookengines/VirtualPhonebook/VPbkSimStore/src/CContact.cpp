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
#include <CVPbkSimCntField.h>
#include <featmgr.h>
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

const TInt KDefinedAnrFieldTypeCount = 3;   // count of defined additional number types

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
        case EVPbkSimAdditionalNumberLast:    // the EVPbkSimAdditionalNumber
            {
            ++result; // always at least one number
            if ( aUsimProp.iMaxNumOfAnrs != KVPbkSimStorePropertyUndefined )
                {
                if ( !FeatureManager::FeatureSupported(
                             KFeatureIdFfTdClmcontactreplicationfromphonebooktousimcard ) )
                    {
                    result += aUsimProp.iMaxNumOfAnrs;
                    }
                else
                    {
                    if ( aUsimProp.iMaxNumOfAnrs - KDefinedAnrFieldTypeCount > 0 )
                        {
                        result += (aUsimProp.iMaxNumOfAnrs - KDefinedAnrFieldTypeCount);
                        }
                    }
                }
            break;
            }
        case EVPbkSimAdditionalNumber1:
            result = aUsimProp.iMaxNumOfAnrs >= 1 ? 1 : 0;   // according the max number of anrs.
            break;
        case EVPbkSimAdditionalNumber2:
            result = aUsimProp.iMaxNumOfAnrs >= 2 ? 1 : 0;
            break;
        case EVPbkSimAdditionalNumber3:
            result = aUsimProp.iMaxNumOfAnrs >= 3 ? 1 : 0;	
            break;
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

_LIT( KEmptyData, "+" );    //the empty data, modifiy this string to keep its a special string.
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
    if( FeatureManager::FeatureSupported( 
                        KFeatureIdFfTdClmcontactreplicationfromphonebooktousimcard ) )
        {
        RemoveAllEmptyFields( aSimContact );    //  remove the empty contacts where added before save.
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
// CContact::RemoveAllEmptyFields
// -----------------------------------------------------------------------------
//
void CContact::RemoveAllEmptyFields( CVPbkSimContact& aSimContact )
    {
    TInt i = aSimContact.FieldCount() - 1 ;
    while( i >= 0 )
        {
		CVPbkSimCntField& cntField = aSimContact.FieldAt( i );
		TVPbkSimCntFieldType simCntType = cntField.Type();
        if( simCntType ==  EVPbkSimGsmNumber
		    || simCntType == EVPbkSimAdditionalNumber1
			|| simCntType == EVPbkSimAdditionalNumber2
			|| simCntType == EVPbkSimAdditionalNumber3
			|| simCntType == EVPbkSimAdditionalNumberLast )
        	{
			if( cntField. Data().Compare( KEmptyData ) == 0 )
				{
				aSimContact.DeleteField( i );
				}
        	}
        i --;
        }
    }

// -----------------------------------------------------------------------------
// CContact::FillWithEmptyFieldsL
// -----------------------------------------------------------------------------
//
void CContact::FillWithEmptyFieldsL() const
    {
    RPointerArray<CVPbkSimCntField> & contactFieldArray = iSimContact->FieldArray();
    TInt i = contactFieldArray.Count() - 1 ;
	while( i >= 0 ) // remove all empty content.
		{
		CVPbkSimCntField& cntField = iSimContact->FieldAt( i );
		TVPbkSimCntFieldType type = cntField.Type();
		if( cntField. Data().Length() == 0 )
			{
			iSimContact->DeleteField( i );
			}
		i --;
		}
	
    if( contactFieldArray.Count() == 0 )  // no un-empty fields.
        {
        return;
        }
    CVPbkSimContact::TFieldLookup lookupAdnNumber = 
                       iSimContact->FindField( EVPbkSimAdditionalNumber );  
    if( lookupAdnNumber.EndOfLookup())  // if there is no additional number in the contact then no need to add placeholder
    	{
		return;
    	}
    RPointerArray<CVPbkSimCntField> tempFieldArray;
    CleanupClosePushL( tempFieldArray );
    // mappings 
    CFieldTypeMappings & mappings = iParentStore.FieldTypeMappings();
    // supported types.
    const CSupportedFieldTypes& supportedTypes = iParentStore.SupportedFieldTypes();

    // check all supported field types in the fields list. If not exist created new.
    // if data length is 0, set data to empty data.
    for( int i = 0; i < supportedTypes.FieldTypeCount(); i ++ )
        {
        const MVPbkFieldType& fieldType = supportedTypes.FieldTypeAt( i );
        TVPbkSimCntFieldType simCntType = mappings.Match( fieldType );
        if( simCntType ==  EVPbkSimGsmNumber
		    || simCntType == EVPbkSimAdditionalNumber1
			|| simCntType == EVPbkSimAdditionalNumber2
			|| simCntType == EVPbkSimAdditionalNumber3
			|| simCntType == EVPbkSimAdditionalNumberLast )
            {
            CVPbkSimCntField * field = NULL;
            CVPbkSimContact::TFieldLookup lookup = 
                   iSimContact->FindField( simCntType );

            if( lookup.EndOfLookup() )
                {
                field= iSimContact->CreateFieldLC( simCntType );
                field->SetDataL( KEmptyData );
                tempFieldArray.Append( field );
                CleanupStack::Pop();
                }
             else
                {
                field = contactFieldArray[lookup.Index()];
                if( field->Data().Length() == 0 )
                    {
                    field->SetDataL( KEmptyData );
                    }
                if( simCntType == EVPbkSimAdditionalNumber1 
                    || simCntType == EVPbkSimAdditionalNumber2
                    || simCntType == EVPbkSimAdditionalNumber3 )
                    {
                    contactFieldArray.Remove( lookup.Index() );
                    tempFieldArray.AppendL( field );
                    }
                }
            }
        }
    TInt j = contactFieldArray.Count() - 1;
    while( j >= 0 )  //  EVPbkSimAdditionalNumberLast type field will append at last.
        {
        if( contactFieldArray[j]->Type() == EVPbkSimAdditionalNumberLast )
            {
            tempFieldArray.AppendL( contactFieldArray[ j ] );
            contactFieldArray.Remove( j );
            }
        j --;
        }
    for( int i = 0; i < tempFieldArray.Count(); i ++ )
        {
        contactFieldArray.AppendL( tempFieldArray[i]);
        }
    tempFieldArray.Reset();
    CleanupStack::Pop();
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
         simType == EVPbkSimAdditionalNumberLast )  //the same field type as EVPbkSimGsmNumber
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
            simType = EVPbkSimAdditionalNumberLast; 
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
        if( FeatureManager::FeatureSupported( KFeatureIdFfTdClmcontactreplicationfromphonebooktousimcard ) )
            {
            FillWithEmptyFieldsL();
            }
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
    
    // remove filled placeholder fields.
    if( vpbkOpResult.iOpCode == MVPbkContactObserver::EContactCommit )
    	{
		RemoveAllEmptyFields( *iSimContact );
    	}
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
    
    // remove filled placeholder fields.
    if( op == MVPbkContactObserver::EContactCommit )
    	{
		RemoveAllEmptyFields( *iSimContact );
    	}
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

