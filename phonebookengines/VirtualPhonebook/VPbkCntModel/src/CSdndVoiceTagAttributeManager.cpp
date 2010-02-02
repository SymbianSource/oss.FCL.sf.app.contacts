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
* Description:  Virtual phonebook speaker dependent voice dialing
*
*/



#include "csdndvoicetagattributemanager.h"
#include "csdndvoicetagattributeoperation.h"

// From Virtual Phonebook
#include <cvpbkcontactmanager.h>
#include <cvpbkvoicetagattribute.h>
#include <mvpbkcontactoperation.h>
#include <vpbkstoreuriliterals.h>
#include <mvpbkcontactstorelist.h>
#include <tvpbkcontactstoreuriptr.h>
#include <mvpbkstorecontact.h>
#include <cvpbkcontactfieldcollection.h>
#include <cvpbkvoicetagattribute.h>

// From VPbkCntModel
#include "ccontactstore.h"
#include "ccontact.h"

// From Contacts Model
#include <cntitem.h>


namespace VPbkCntModel {

#define KVoiceTagValue      KUidContactsVoiceDialFieldValue  

/**
 * Factory function.
 */
CVoiceTagAttributeManager* CVoiceTagAttributeManager::NewL(
        TAny* aParam)
    {
    return CSdndVoiceTagAttributeManager::NewL( aParam );
    }

/**
 * SDND voice tag attribute manager.
 */
CSdndVoiceTagAttributeManager::CSdndVoiceTagAttributeManager(
        CVPbkContactManager& aContactManager ) :
    iContactManager( aContactManager )
    {
    }

inline void CSdndVoiceTagAttributeManager::ConstructL()
    {
    }

CSdndVoiceTagAttributeManager* CSdndVoiceTagAttributeManager::NewL( TAny* aParam )
    {
    TParam& param = *static_cast<TParam*>(aParam);
    CSdndVoiceTagAttributeManager* self = new ( ELeave ) 
                   CSdndVoiceTagAttributeManager( param.iContactManager );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CSdndVoiceTagAttributeManager::~CSdndVoiceTagAttributeManager()
    {
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::ListContactsL(
        TUid /*aAttributeType*/,
        MVPbkContactFindObserver& /*aObserver*/ )
    {
    __ASSERT_DEBUG( EFalse, User::Leave( KErrNotSupported ) );
    return NULL;
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::ListContactsL(
        const MVPbkContactAttribute& /*aAttribute*/,
        MVPbkContactFindObserver& /*aObserver*/ )
    {
    __ASSERT_DEBUG( EFalse, User::Leave( KErrNotSupported ) ); 
    return NULL;
    }
    
MVPbkContactOperation* CSdndVoiceTagAttributeManager::CreateListContactsOperationL(
            TUid aAttributeType, 
            MVPbkContactFindObserver& aObserver )
    {
    MVPbkContactOperation* result = NULL;

    if ( aAttributeType == CVPbkVoiceTagAttribute::Uid() )
        {
        CVPbkVoiceTagAttribute* attribute = CVPbkVoiceTagAttribute::NewL();
        CleanupStack::PushL( attribute );
        result = CreateListContactsOperationL( *attribute, aObserver );
        CleanupStack::PopAndDestroy( attribute );
        }

    return result;   
    }

MVPbkContactOperation* CSdndVoiceTagAttributeManager::CreateListContactsOperationL(
        const MVPbkContactAttribute& aAttribute,
        MVPbkContactFindObserver& aObserver )
    {
    MVPbkContactOperation* result = NULL;

    if ( aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        const CVPbkVoiceTagAttribute& attribute =
                static_cast<const CVPbkVoiceTagAttribute&>( aAttribute );
        MVPbkContactStore* store = 
               iContactManager.ContactStoresL().Find(KVPbkDefaultCntDbURI() );
        if ( store )
            {
            CContactStore& cntStore = *static_cast<CContactStore*>( store );
            result = CSdndVoiceTagAttributeOperation::NewListLC(
                cntStore,
                attribute,
                aObserver);
            result->StartL();                
            CleanupStack::Pop();
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }

    return result;  
    }
    
TBool CSdndVoiceTagAttributeManager::HasContactAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContact& aContact ) const
    {
    TBool result( EFalse );

    if (dynamic_cast<const CContact*>( &aContact ) &&
        aAttributeType == CVPbkVoiceTagAttribute::Uid())    
        {        
        const TInt count(aContact.Fields().FieldCount() );
        for ( TInt i = 0; i < count; ++i )
            {
            const MVPbkStoreContactField& field = aContact.Fields().FieldAt( i );
            if ( HasFieldAttributeL( aAttributeType, field ) )
                {
                result = ETrue;
                break;
                }
            }
        }
    return result;
    }

TBool CSdndVoiceTagAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContact& aContact ) const
    {
    TBool result = EFalse;

    if ( aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        result = HasContactAttributeL( aAttribute.AttributeType(), aContact );
        }

    return result;
    }

TBool CSdndVoiceTagAttributeManager::HasFieldAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContactField& aField ) const
    {
    TBool result = EFalse;
    
    if ( dynamic_cast<const TContactField*>(&aField) &&
        aAttributeType == CVPbkVoiceTagAttribute::Uid() )
        {
        MVPbkContactStore* store = iContactManager.ContactStoresL().Find(KVPbkDefaultCntDbURI());
        if ( store )
            {
            CContactStore* cntStore = static_cast<CContactStore*>( store );
            if (&aField.ContactStore() == cntStore)
                {
                // Loop through all defaults and check if field contains
                // that type.
               	const TContactField& field = 
               	        static_cast<const TContactField&>( aField );
				TFieldType fieldType = TFieldType::Uid( KVoiceTagValue );
                if ( field.NativeField()->ContentType().ContainsFieldType( fieldType ) )
                    {
                    result = ETrue;
                    }
                }
            }
        }
    return result;
    }

TBool CSdndVoiceTagAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContactField& aField ) const
    {   
    TBool result = EFalse;

    if ( aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        result = HasFieldAttributeL( aAttribute.AttributeType(), aField );
        }

    return result;
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::SetContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/ )
    {
    MVPbkContactOperationBase* result = NULL;

    if ( aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        User::Leave( KErrNotSupported );
        }

    return result;
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::SetFieldAttributeL(
        MVPbkStoreContactField& aField,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver )
    {
    MVPbkContactOperation* result = NULL;

    if ( dynamic_cast<TContactField*>( &aField ) &&
        aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        const CVPbkVoiceTagAttribute& attribute =
                static_cast<const CVPbkVoiceTagAttribute&>( aAttribute );
        MVPbkContactStore* store = iContactManager.ContactStoresL().Find( KVPbkDefaultCntDbURI() );
        if (store)
            {
            VPbkCntModel::CContactStore& cntStore = 
                        *static_cast<CContactStore*>( store );
            result = CSdndVoiceTagAttributeOperation::NewSetLC(
                    cntStore, aField, attribute, aObserver );
            result->StartL();
            CleanupStack::Pop(); // result
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }

    return result;
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/ )
    {
    MVPbkContactOperationBase* result = NULL;

    if ( aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        User::Leave( KErrNotSupported );
        }

    return result;
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& /*aObserver*/ )
    {
    MVPbkContactOperationBase* result = NULL;

    if ( aAttributeType == CVPbkVoiceTagAttribute::Uid() )
        {
        User::Leave( KErrNotSupported );
        }

    return result;
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& aField,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver )
    {
    MVPbkContactOperation* result = NULL;

    if ( dynamic_cast<TContactField*>( &aField ) &&
        aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        const CVPbkVoiceTagAttribute& attribute =
                static_cast<const CVPbkVoiceTagAttribute&>( aAttribute );

        MVPbkContactStore* store = 
                iContactManager.ContactStoresL().Find( KVPbkDefaultCntDbURI() );
        if ( store )
            {
            VPbkCntModel::CContactStore& cntStore = 
                        *static_cast<CContactStore*>( store );
            result = CSdndVoiceTagAttributeOperation::NewRemoveLC(
                    cntStore, aField, attribute, aObserver );
            result->StartL();
            CleanupStack::Pop(); // result
            }
        else
            {
            User::Leave( KErrNotSupported );
            }
        }

    return result;
    }

MVPbkContactOperationBase* CSdndVoiceTagAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& aField,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& aObserver)
    {
    MVPbkContactOperationBase* result = NULL;

    if ( dynamic_cast<TContactField*>(&aField) &&
        aAttributeType == CVPbkVoiceTagAttribute::Uid() )
        {
        CVPbkVoiceTagAttribute* attribute = CVPbkVoiceTagAttribute::NewL();
        CleanupStack::PushL( attribute );
        result = RemoveFieldAttributeL( aField, *attribute, aObserver );
        CleanupStack::PopAndDestroy( attribute );
        }
    return result;
    }
    
MVPbkStoreContactFieldCollection* CSdndVoiceTagAttributeManager::FindFieldsWithAttributeLC(
        TUid aAttributeType,
        MVPbkStoreContact& aContact ) const
    {    
    CVPbkContactFieldCollection* result = NULL;
    
    if ( dynamic_cast<CContact*>( &aContact ) &&
        aAttributeType == CVPbkVoiceTagAttribute::Uid() )
        {
        result = CVPbkContactFieldCollection::NewLC( aContact );
        const TInt count = aContact.Fields().FieldCount();
        for ( TInt i = 0; i < count; ++i )
            {
            MVPbkStoreContactField& field = aContact.Fields().FieldAt( i );
            if ( HasFieldAttributeL(aAttributeType, field) )
                {
                MVPbkStoreContactField* clone = field.CloneLC();
                result->AppendL( clone );
                CleanupStack::Pop(); // clone
                }
            }        
        }

    return result;    
    }        
        
MVPbkStoreContactFieldCollection* CSdndVoiceTagAttributeManager::FindFieldsWithAttributeLC(
        const MVPbkContactAttribute& aAttribute,
        MVPbkStoreContact& aContact ) const
    {
    CVPbkContactFieldCollection* result = NULL;
    
    if ( dynamic_cast<CContact*>(&aContact) &&
        aAttribute.AttributeType() == CVPbkVoiceTagAttribute::Uid() )
        {
        result = CVPbkContactFieldCollection::NewLC( aContact );
        const TInt count = aContact.Fields().FieldCount();
        for ( TInt i = 0; i < count; ++i )
            {
            MVPbkStoreContactField& field = aContact.Fields().FieldAt( i );
            if ( HasFieldAttributeL(aAttribute, field) )
                {
                MVPbkStoreContactField* clone = field.CloneLC();
                result->AppendL( clone );
                CleanupStack::Pop(); // clone
                }
            }
        }
    return result;
    }    

} // namespace VPbkCntModel

// End of File
