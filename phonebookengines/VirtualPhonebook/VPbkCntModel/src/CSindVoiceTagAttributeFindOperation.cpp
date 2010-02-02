/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contact model store Voice tag attribute operation.
*
*/


#include "csindvoicetagattributefindoperation.h"
#include "ccontactstore.h"
#include "ccontactlink.h"

#include <cvpbkcontactmanager.h>
#include <mvpbkcontactfindobserver.h>
#include <cvpbkcontactfieldcollection.h>
#include <mvpbkcontactattributemanager.h>
#include <mvpbkfieldtype.h>
#include <vpbkeng.rsg>
#include <cvpbkcontactfielditerator.h>
#include <vpbkstoreuriliterals.h>

// From VPbkCntModel
#include "ccontact.h"

// From Phonebook
#include <pbkfields.hrh>

// System includes
#include <vuivoicerecogdefs.h>  // KVoiceDialContext

// From Contacts Model
#include <cntitem.h>
#include "vpbkcntmodeluid.h"

namespace VPbkCntModel {

namespace {

// LOCAL CONSTANTS AND MACROS
const TInt KVoiceTagFieldIdElement = 1;

#ifdef _DEBUG
enum TPanicCode
    {
    EPreCond_GetContextCompleted,
    EPreCond_MapTaggedFields
    };

void Panic( TPanicCode aPanicCode )
    {
    _LIT(KPanicText, "CSindVoiceTagAttributeFindOperation");
    User::Panic(KPanicText, aPanicCode);
    };
#endif // _DEBUG

/**
 * Custom cleanup item.
 *
 * @param aObj  Pointer to item pushed on to the stack.
 */
void CleanupResetAndDestroy( TAny* aObj )
    {
    if ( aObj )
        {
        static_cast<CArrayPtr<MNssTag> *>( aObj )->ResetAndDestroy();
        }
    }

	    
} // unnamed namespace

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::CSindVoiceTagAttributeFindOperation
// --------------------------------------------------------------------------
//            
CSindVoiceTagAttributeFindOperation::CSindVoiceTagAttributeFindOperation(
        CContactStore& aContactStore,
        MNssContextMgr& aNssContextManager,
        MNssTagMgr& aNssTagManager,
        MVPbkStoreContact& aStoreContact,
        CVPbkContactManager& aContactManager,
        MVPbkMultiAttributePresenceObserver& aObserver) :
    iContactStore( aContactStore ),
    iNssContextManager( aNssContextManager ),
    iNssTagManager( aNssTagManager ),
    iStoreContact( aStoreContact ),
    iContactManager( aContactManager ),
    iOperationObserver( aObserver )
    {
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::ConstructL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeFindOperation::ConstructL()
    {
    iVoiceTagFields = CVPbkContactFieldCollection::NewLC(iStoreContact);
    CleanupStack::Pop();
    StartL();
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::NewListLC
// --------------------------------------------------------------------------
//            
CSindVoiceTagAttributeFindOperation* CSindVoiceTagAttributeFindOperation::NewFindLC(
        CContactStore& aContactStore,
        MNssContextMgr& aNssContextManager,
        MNssTagMgr& aNssTagManager,
        MVPbkStoreContact& aStoreContact,
        CVPbkContactManager& aContactManager,
        MVPbkMultiAttributePresenceObserver& aObserver)
    {
    CSindVoiceTagAttributeFindOperation* self = 
        new(ELeave) CSindVoiceTagAttributeFindOperation( aContactStore, 
            aNssContextManager, aNssTagManager, aStoreContact, 
            aContactManager, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::~CSindVoiceTagAttributeFindOperation
// --------------------------------------------------------------------------
//            
CSindVoiceTagAttributeFindOperation::~CSindVoiceTagAttributeFindOperation()
    {
    Cancel();
    delete iVoiceTagFields;
    delete iNssContext;
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::StartL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeFindOperation::StartL()
    {
    // Get the voice tag context asynchronously. 
    // See completes in GetContextCompleted.
    TInt res = iNssContextManager.GetContext( this, KVoiceDialContext );
    if ( res != TNssVasCoreConstant::EVasSuccess )
        {
        // NSS error codes are positive. Transform to system wide error
        // codes.
        if ( res >= 0 )
            {
            res = KErrUnknown;
            }
        User::Leave( res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::Cancel
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeFindOperation::Cancel()
    {
    iNssContextManager.CancelGetContext();
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::GetContextCompleted
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeFindOperation::GetContextCompleted( 
        MNssContext* aContext )
    {
    __ASSERT_DEBUG( !iNssContext, Panic( EPreCond_GetContextCompleted ));
    delete iNssContext;
    iNssContext = aContext;
    
    // Context acquired. Voice tags can be checked now.
    TRAPD( res, GetTagL() );
    if ( res != KErrNone )
        {
        iOperationObserver.AttributePresenceOperationFailed( *this, res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::GetContextListCompleted
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeFindOperation::GetContextListCompleted( 
        MNssContextListArray* /*aContextList*/ )
    {
    // Not used by this operation
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::GetContextFailed
// --------------------------------------------------------------------------
//        
void CSindVoiceTagAttributeFindOperation::GetContextFailed( 
        TNssGetContextClientFailCode aFailCode )
    {
    // transform Nss voice tag error codes to system wide error codes and
    // signal observer
    TInt res = KErrUnknown;
    switch ( aFailCode )
        {
        case MNssGetContextClient::EVASDBItemNotFound:
            {
            res = KErrNotFound;
            break;
            }
        case MNssGetContextClient::EVASDBNoMemory:
            {
            res = KErrNoMemory;
            break;
            }
        case MNssGetContextClient::EVASDBDiskFull:
            {
            res = KErrDiskFull;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }
    
    iOperationObserver.AttributePresenceOperationFailed( *this, res );
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::GetTagListCompleted
// --------------------------------------------------------------------------
//    
void CSindVoiceTagAttributeFindOperation::GetTagListCompleted( 
        MNssTagListArray* aTagList )
    {
    TRAPD( res, HandleGetTagListCompletedL( aTagList ) );
    if ( res != KErrNone )
        {
        iOperationObserver.AttributePresenceOperationFailed( *this, res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::GetTagFailed
// --------------------------------------------------------------------------
//
void CSindVoiceTagAttributeFindOperation::GetTagFailed( 
        TNssGetTagClientFailCode aFailCode )
    {    
    TInt res = KErrNone;    
    switch ( aFailCode )
        {
        case MNssGetTagClient::EVASDBItemNotFound:
            {
            // Tag not found for the iCurrentId, signal client completion
            CVPbkContactFieldCollection* results = iVoiceTagFields;
            iVoiceTagFields = NULL;
            iOperationObserver.AttributePresenceOperationComplete( *this, results );
            break;
            }
        case MNssGetTagClient::EVASDBNoMemory:
            {
            res = KErrNoMemory;
            break;
            }
        case MNssGetTagClient::EVASDBDiskFull:
            {
            res = KErrDiskFull;
            break;
            }
        default:
            {
            res = KErrUnknown;
            break;
            }
        }
    
    if ( res != KErrNone )
        {
        iOperationObserver.AttributePresenceOperationFailed( *this, res );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::HandleGetTagListCompletedL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeFindOperation::HandleGetTagListCompletedL( 
        MNssTagListArray* aTagList )
    {
    if ( aTagList )
        {
        CleanupStack::PushL( TCleanupItem ( CleanupResetAndDestroy, aTagList ) );
        TBool hasVoiceTag = aTagList->Count() > 0;
        if ( hasVoiceTag )
            {
            MapTaggedFieldsL( aTagList );
            }
        // signal client completion
        CVPbkContactFieldCollection* results = iVoiceTagFields;
        iVoiceTagFields = NULL;
        iOperationObserver.AttributePresenceOperationComplete( *this, results );

        // cleanup
        CleanupStack::PopAndDestroy(); // aTagList
        delete aTagList;
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::GetTagL
// --------------------------------------------------------------------------
//            
void CSindVoiceTagAttributeFindOperation::GetTagL()
    {
    // get contact id
    const CContact& contact = static_cast<const CContact&>(iStoreContact);
    TContactItemId cid = contact.NativeContact()->Id();

    const TInt position = 0;
    // This is asynchronous operation if it returns KErrNone.
    // Otherwise it's synchronous.
    const TInt res = iNssTagManager.GetTagList( this, iNssContext,
        (TInt) cid, position );
    if ( res != KErrNone )
        {
        // The current id didn't have voice tag. Signal client completion.
        CVPbkContactFieldCollection* results = iVoiceTagFields;
        iVoiceTagFields = NULL;
        iOperationObserver.AttributePresenceOperationComplete( *this, results );
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::MapTaggedFieldsL
// --------------------------------------------------------------------------
//     
void CSindVoiceTagAttributeFindOperation::MapTaggedFieldsL(
        MNssTagListArray* aTagList )
    {
    // Count check prevent double calling this function
    __ASSERT_DEBUG( iVoiceTagFields && 
                    iVoiceTagFields->FieldCount() == 0, 
                    Panic(EPreCond_MapTaggedFields) );

    TBool thisContactHasDefaultParameter = EFalse;
                        
    // Check has the user set the default calling number parameter
    // to certain field. If the default calling number is set, 
    // the voice tagged field is the default field.
    const TInt fieldCount = iStoreContact.Fields().FieldCount();
    for ( TInt i(0); i < fieldCount; ++i )
        {
        MVPbkStoreContactField& field = 
            iStoreContact.Fields().FieldAt( i );

        TInt defaultFieldIndex = DefaultCallNumberNativeFieldIndexL(
            dynamic_cast<MVPbkStoreContact&>(field.ParentContact()));
        if ( defaultFieldIndex != KErrNotFound )
            {
            TInt nativeFieldIndex =
                (dynamic_cast<const TContactField*>(
                    &field))->NativeFieldIndex();
            if ( defaultFieldIndex == nativeFieldIndex )
                {
                thisContactHasDefaultParameter = ETrue;
                iVoiceTagFields->AppendL( field.CloneLC() );
                CleanupStack::Pop(); // CloneLC
                }
            }
        }

    // This contact do not have default calling number. Let's check 
    // then the other fields 
    if ( !thisContactHasDefaultParameter )
        {
        // Get tag from first index
        MNssTag* tag = aTagList->At( 0 );
        if ( tag )
            {
            // RRD data contains contactid as first element 
            // and field id as second element
            const TInt voiceTaggedFieldId = 
                tag->RRD()->IntArray()->At( KVoiceTagFieldIdElement );

            for ( TInt i(0); i < fieldCount; ++i )
                {
                MVPbkStoreContactField& field = 
                    iStoreContact.Fields().FieldAt( i );
                TInt fieldTypeResId( 0 );
                
                TPtrC ptr = field.FieldLabel(); // FOR DEBUG                        
                
                if ( DoesFieldIdMatchMVPbkFieldType( 
                        voiceTaggedFieldId,
                        field.BestMatchingFieldType(),
                        fieldTypeResId ) && 
                     IsFirstFieldOfTypeL( field, fieldTypeResId )   
                        )
                    {                    
                    iVoiceTagFields->AppendL( field.CloneLC() );
                    CleanupStack::Pop(); // CloneLC
                    }               
                }                
            }
        }
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::DoesFieldIdMatchMVPbkFieldType
// --------------------------------------------------------------------------
//       
TBool CSindVoiceTagAttributeFindOperation::DoesFieldIdMatchMVPbkFieldType(
          const TInt aFieldId,
          const MVPbkFieldType* aFieldType,
          TInt& aFieldTypeResId)  const
    {
    const MVPbkFieldType* vpbkFieldType = NULL;
    TBool result = EFalse;

    switch(aFieldId)
        {
        case EPbkFieldIdPhoneNumberGeneral:
            {
            aFieldTypeResId = R_VPBK_FIELD_TYPE_LANDPHONEGEN;
            break;
            }
        case EPbkFieldIdPhoneNumberHome:
            {
            aFieldTypeResId = R_VPBK_FIELD_TYPE_LANDPHONEHOME;
            break;
            }
        case EPbkFieldIdPhoneNumberWork:
            {
            aFieldTypeResId = R_VPBK_FIELD_TYPE_LANDPHONEWORK;
            break;
            }
        case EPbkFieldIdPhoneNumberMobile:
            {
            aFieldTypeResId = R_VPBK_FIELD_TYPE_MOBILEPHONEGEN;
            break;
            }
        default:
            {
            // Do nothing
            break;
            }
        }

    if (aFieldTypeResId)
        {
        vpbkFieldType = iContactManager.FieldTypes().Find( aFieldTypeResId );
        }

    if ( vpbkFieldType == aFieldType )
        {
        result = ETrue;
        }
    else if ( aFieldId == EPbkFieldIdPhoneNumberMobile )
        {
        // the NssVas API returns only one type of mobile number and there are
        // three different kind of mobile number fields (general, home and work)
        // let's go all those through: general has already checked.
        vpbkFieldType =
            iContactManager.FieldTypes().Find( R_VPBK_FIELD_TYPE_MOBILEPHONEHOME );

        if ( vpbkFieldType == aFieldType )
            {
            aFieldTypeResId = R_VPBK_FIELD_TYPE_MOBILEPHONEHOME;
            result = ETrue;
            }
        else
            {
            vpbkFieldType =
                iContactManager.FieldTypes().Find( R_VPBK_FIELD_TYPE_MOBILEPHONEWORK );
            if ( vpbkFieldType == aFieldType )
                {
                aFieldTypeResId = R_VPBK_FIELD_TYPE_MOBILEPHONEWORK;
                result = ETrue;
                }
            }
        }
    
    return result;
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::IsFirstFieldOfTypeL
// --------------------------------------------------------------------------
//
TBool CSindVoiceTagAttributeFindOperation::IsFirstFieldOfTypeL(
            const MVPbkStoreContactField& aField,
            TInt aFieldTypeResId ) const
    {
    TBool result = EFalse;

    const MVPbkFieldType* fieldType =
        dynamic_cast<const MVPbkFieldType*>(aField.BestMatchingFieldType());

    MVPbkStoreContact* storeContact =
        dynamic_cast<MVPbkStoreContact*>(&aField.ParentContact());

    MVPbkStoreContactFieldCollection* collection =
        dynamic_cast<MVPbkStoreContactFieldCollection*>(&storeContact->Fields());

    // Cannot be assumed which mobile field has the voice tag attached.
    // The order, the NssVas API uses is the following:
    // 1. mobile
    // 2. mobile (home)
    // 3. mobile (work)
    // All the mobile (XXX) fields are gone through to
    // find out which one has the voice tag.
    if ( aFieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEGEN ||
         aFieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEHOME ||
         aFieldTypeResId == R_VPBK_FIELD_TYPE_MOBILEPHONEWORK )
         {
         TBool hasMobileGenField = EFalse;
         TBool hasMobileHomeField = EFalse;

         // check does the "mobile" match with given field
         fieldType =
            iContactManager.FieldTypes().Find(R_VPBK_FIELD_TYPE_MOBILEPHONEGEN);

         CVPbkContactFieldTypeIterator* iterator =
            CVPbkContactFieldTypeIterator::NewLC(*fieldType, *collection);

         if ( iterator->HasNext() )
            {
            hasMobileGenField = ETrue;
            MVPbkStoreContactField* field = iterator->Next();
            if ( field->IsSame(aField) )
                {
                result = ETrue;
                }
            }
         CleanupStack::PopAndDestroy(iterator);

         if ( !result && !hasMobileGenField)
            {
            // check does the "mobile (home)" match with given field
            fieldType =
                iContactManager.FieldTypes().Find(R_VPBK_FIELD_TYPE_MOBILEPHONEHOME);
            iterator =
                CVPbkContactFieldTypeIterator::NewLC(*fieldType, *collection);
            if ( iterator->HasNext() )
                {
                hasMobileHomeField = ETrue;
                MVPbkStoreContactField* field = iterator->Next();
                if ( field->IsSame(aField) )
                    {
                    result = ETrue;
                    }
                }
            CleanupStack::PopAndDestroy( iterator );
            if ( !result && !hasMobileGenField && !hasMobileHomeField)
                {
                // check does the "mobile (work)" match with given field
                fieldType =
                    iContactManager.FieldTypes().Find(R_VPBK_FIELD_TYPE_MOBILEPHONEWORK);
                iterator =
                    CVPbkContactFieldTypeIterator::NewLC(*fieldType, *collection);
                if ( iterator->HasNext() )
                    {
                    MVPbkStoreContactField* field = iterator->Next();
                    if ( field->IsSame(aField) )
                        {
                        result = ETrue;
                        }
                    }
                CleanupStack::PopAndDestroy( iterator );
                }
            }
        }

    // All other fields (general, home and work) are checked in this branch:
    else
        {
        CVPbkContactFieldTypeIterator* iterator =
            CVPbkContactFieldTypeIterator::NewLC(*fieldType, *collection);

        if ( iterator->HasNext() )
            {
            MVPbkStoreContactField* field = iterator->Next();
            if ( field->IsSame(aField) )
                {
                result = ETrue;
                }
            }
        CleanupStack::PopAndDestroy(iterator);
        }
        
    return result;
    }

// --------------------------------------------------------------------------
// CSindVoiceTagAttributeFindOperation::DefaultCallNumberNativeFieldIndexL
// --------------------------------------------------------------------------
//
TInt CSindVoiceTagAttributeFindOperation::DefaultCallNumberNativeFieldIndexL(
        const MVPbkStoreContact& aContact) const
    {
    TInt result = KErrNotFound;
    MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                   KVPbkDefaultCntDbURI());
    if (store)
        {
        CContactStore* cntStore = static_cast<CContactStore*>(store);
        if (&aContact.ContactStore() == cntStore)
            {
            const CContact& contact = static_cast<const CContact&>(aContact);
            CContactItemFieldSet& fields = contact.NativeContact()->CardFields();
            const TInt count = fields.Count();
            for (TInt i = 0; i < count; ++i)
                {
                // Loop through all defaults and check if field contains
                // that type. If any field of contact contains any default
                // we can break.
                TFieldType fieldType = TFieldType::Uid(KIntVPbkDefaultFieldPref);
                if (fields[i].ContentType().ContainsFieldType(fieldType))
                    {
                    result = i;
                    break;
                    }
                }
            }
        }
        
    return result;
    }

} // namespace VPbkCntModel

// End of File
