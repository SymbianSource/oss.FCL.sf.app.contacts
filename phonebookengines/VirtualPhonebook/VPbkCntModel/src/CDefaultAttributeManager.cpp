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
* Description:  The virtual phonebook default attribute manager
*
*/



#include "CDefaultAttributeManager.h"
#include "CDefaultAttributeOperation.h"

// From Virtual Phonebook
#include <CVPbkContactManager.h>
#include <MVPbkContactOperation.h>
#include <VPbkStoreUriLiterals.h>
#include <MVPbkContactStoreList.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactFieldCollection.h>
#include <CVPbkDefaultAttribute.h>

// From VPbkCntModel
#include "CContactStore.h"
#include "CContact.h"
#include "VPbkCntModelUid.h"

// From Contacts Model
#include <cntitem.h>


namespace VPbkCntModel {

namespace {

	struct TFieldTypePair
	    {
	    TInt iProperty;
	    TInt iUid;
	    };

	static const TFieldTypePair fieldTypeMap[] =
	    {
	    /// default phone number field type
	    {EVPbkDefaultTypePhoneNumber, KIntVPbkDefaultFieldPref},
	    /// default video number field type
	    {EVPbkDefaultTypeVideoNumber, KIntVPbkDefaultFieldVideo},
	    /// default sms field type
	    {EVPbkDefaultTypeSms, KIntVPbkDefaultFieldSms},
	    /// default mms field type
	    {EVPbkDefaultTypeMms, KIntVPbkDefaultFieldMms},
	    /// default email field type
	    {EVPbkDefaultTypeEmail, KIntVPbkDefaultFieldPref},
	    /// default email over sms field type
	    {EVPbkDefaultTypeEmailOverSms, KIntVPbkDefaultFieldEmailOverSms},
	    /// default poc field type
	    {EVPbkDefaultTypePOC, KIntVPbkDefaultFieldPoc},
	    /// default voip field type
	    {EVPbkDefaultTypeVoIP, KIntVPbkDefaultFieldVoip},
        /// default chat field type
        {EVPbkDefaultTypeChat, KIntVPbkDefaultFieldChat},
        /// default url field type
        {EVPbkDefaultTypeOpenLink, KIntVPbkDefaultFieldOpenLink},


	        
	    // Always the last one
	    {EVPbkLastDefaultType, 0}
	    };

	TFieldType FindFieldTypeForProperty(TVPbkDefaultType aProperty)
	    {
	    for (TInt i = 0; fieldTypeMap[i].iProperty != EVPbkLastDefaultType;
	         ++i)
	        {
	        if (aProperty == fieldTypeMap[i].iProperty)
	            {
	            return TFieldType::Uid(fieldTypeMap[i].iUid);
	            }

	        }
	    return KNullUid;
	    }  
	    
    TBool IsFieldType( const TContactField& aField, TUid aUid )
        {
        for (TInt i = 0; i < aField.NativeField()->
                ContentType().FieldTypeCount(); ++i )
            {
            if ( aField.NativeField()->ContentType().FieldType( i )  ==
                    aUid )
                {
                return ETrue;
                }
            }
        return EFalse;
        }    
	    
} // unnamed namespace

CDefaultAttributeManager::CDefaultAttributeManager(
        CVPbkContactManager& aContactManager) :
    iContactManager(aContactManager)
    {
    }

inline void CDefaultAttributeManager::ConstructL()
    {
    }

CDefaultAttributeManager* CDefaultAttributeManager::NewL(TAny* aParam)
    {
    TParam& param = *static_cast<TParam*>(aParam);
    CDefaultAttributeManager* self = new(ELeave) CDefaultAttributeManager(
        param.iContactManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

CDefaultAttributeManager::~CDefaultAttributeManager()
    {
    }

MVPbkContactOperationBase* CDefaultAttributeManager::ListContactsL(
        TUid /*aAttributeType*/,
        MVPbkContactFindObserver& /*aObserver*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return NULL;
    }

MVPbkContactOperationBase* CDefaultAttributeManager::ListContactsL(
        const MVPbkContactAttribute& /*aAttribute*/,
        MVPbkContactFindObserver& /*aObserver*/)
    {
    __ASSERT_DEBUG(EFalse, User::Leave(KErrNotSupported));
    return NULL;
    }
    
MVPbkContactOperation* CDefaultAttributeManager::CreateListContactsOperationL(
            TUid aAttributeType, 
            MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (aAttributeType == CVPbkDefaultAttribute::Uid())
        {
        CVPbkDefaultAttribute* attribute = CVPbkDefaultAttribute::NewL(
            CVPbkDefaultAttribute::KDefaultTypeNotDefined);
        CleanupStack::PushL(attribute);
        result = CreateListContactsOperationL(*attribute, aObserver);
        CleanupStack::PopAndDestroy(attribute);
        }

    return result;   
    }

MVPbkContactOperation* CDefaultAttributeManager::CreateListContactsOperationL(
    const MVPbkContactAttribute& aAttribute, 
    MVPbkContactFindObserver& aObserver)
    {
    MVPbkContactOperation* result = NULL;

    if (aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid())
        {
        
        const CVPbkDefaultAttribute& attribute =
                static_cast<const CVPbkDefaultAttribute&>(aAttribute);
                
        MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                   KVPbkDefaultCntDbURI());
        if (store)
            {
            CContactStore& cntStore = *static_cast<CContactStore*>(store);
            result = CDefaultAttributeOperation::NewListLC(
                cntStore,
                attribute,
                aObserver);
            result->StartL();                
            CleanupStack::Pop();
            }
        else
            {
            User::Leave(KErrNotSupported);
            }
        }

    return result;     
    }
    
TBool CDefaultAttributeManager::HasContactAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContact& aContact) const
    {
    TBool result = EFalse;
    
    if (aAttributeType == CVPbkDefaultAttribute::Uid())
        {
        const CContact* contact = dynamic_cast<const CContact*>(&aContact);
        if (contact)
            {
            CContactItemFieldSet& fields = contact->NativeContact()->CardFields();
            const TInt count = fields.Count();
            for (TInt i = 0; i < count && !result; ++i)
                {
                // Loop through all defaults and check if field contains
                // that type. If any field of contact contains any default
                // we can break.
                for (TInt n = 0;
                	fieldTypeMap[n].iProperty != EVPbkLastDefaultType;
                	++n)
                	{
					TFieldType fieldType = TFieldType::Uid(fieldTypeMap[n].iUid);
                    if (fields[i].ContentType().ContainsFieldType(fieldType))
                        {
                        result = ETrue;
                        break;
                        }
                	}
                }
            }
        }

    return result;
    }

TBool CDefaultAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContact& aContact) const
    {
    TBool result = EFalse;
    
    if (aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid())
        {
        // Make sure that aContact is from CntModel store
        const CContact* contact = dynamic_cast<const CContact*>(&aContact);
        if (contact)
            {
            const CVPbkDefaultAttribute& attribute =
                    static_cast<const CVPbkDefaultAttribute&>(aAttribute);
            if (attribute.DefaultType() == CVPbkDefaultAttribute::KDefaultTypeNotDefined)
                {
    	        // If default type is not defined, we try to match any default field
                result = HasContactAttributeL(aAttribute.AttributeType(), aContact);
                }
            else
                {
                // There is already dynamic_cast
                TFieldType fieldType = FindFieldTypeForProperty(attribute.DefaultType());
                CContactItemFieldSet& fields = contact->NativeContact()->CardFields();
                const TInt count = fields.Count();
                for (TInt i = 0; i < count && !result; ++i)
                    {
                    if (fields[i].ContentType().ContainsFieldType(fieldType))
                        {
                        result = ETrue;
                        break;
                        }
                	}
                }
            }
        }

    return result;
    }

TBool CDefaultAttributeManager::HasFieldAttributeL(
        TUid aAttributeType,
        const MVPbkStoreContactField& aField) const
    {   
    TBool result = EFalse;
    
    // Make sure that aField is from CntModel store
    if (aAttributeType == CVPbkDefaultAttribute::Uid())
        {
        const TContactField* field = dynamic_cast<const TContactField*>(&aField);
        if ( field )
            {
            // Loop through all defaults and check if field contains
            // that type.
            for (TInt n = 0;
            	fieldTypeMap[n].iProperty != EVPbkLastDefaultType;
            	++n)
            	{
				TFieldType fieldType = TFieldType::Uid(fieldTypeMap[n].iUid);
                if (field->NativeField()->ContentType().ContainsFieldType(fieldType))
                    {
                    result = ETrue;
                    break;
                    }
            	}
            }
        }
    return result;
    }

TBool CDefaultAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& aAttribute,
        const MVPbkStoreContactField& aField) const
    {    
    TBool result = EFalse;
    
    if (aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid())
        {
        const TContactField* field = dynamic_cast<const TContactField*>(&aField);
        if ( field )
            {
            const CVPbkDefaultAttribute& attribute =
                    static_cast<const CVPbkDefaultAttribute&>( aAttribute );
            if ( attribute.DefaultType() == 
                        CVPbkDefaultAttribute::KDefaultTypeNotDefined )
                {
                result = HasFieldAttributeL( aAttribute.AttributeType(), 
                                                                aField );
                }
            else
                {
				TFieldType fieldType = 
				        FindFieldTypeForProperty( attribute.DefaultType() );
                // If checking for email default we have to check that field is email 
                // field, because default phone and email has same id 
                // KIntVPbkDefaultFieldPref Field has specified default
                if ( field->NativeField()->
                            ContentType().ContainsFieldType(fieldType) )
                    {                    
                    // Only Email field can be set as email default
                    if ( attribute.DefaultType() == EVPbkDefaultTypeEmail )
                        {
                        if ( IsFieldType( *field, KUidContactFieldEMail ) )
                            {
                            result = ETrue;
                            }
                        }
                    else if ( attribute.DefaultType() == EVPbkDefaultTypePhoneNumber )
                        {
                        // Phone number default and field is not email field
                        if  ( !IsFieldType( *field, KUidContactFieldEMail ) )
                            {
                            result = ETrue;
                            }
                        }
                    else
                        {
                        result = ETrue;
                        }
                    }
                }
            }
        }
    return result;
    }

MVPbkContactOperationBase* CDefaultAttributeManager::SetContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if ( aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid() )
        {
        User::Leave( KErrNotSupported );
        }

    return result;
    }

MVPbkContactOperationBase* CDefaultAttributeManager::SetFieldAttributeL(
        MVPbkStoreContactField& aField,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver )
    {
    MVPbkContactOperation* result = NULL;

    if ( aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid() )
        {
        TContactField* field = dynamic_cast<TContactField*>( &aField );
        if ( field )
            {
            const CVPbkDefaultAttribute& attribute =
                    static_cast<const CVPbkDefaultAttribute&>( aAttribute );
            if ( attribute.DefaultType() == 
                        CVPbkDefaultAttribute::KDefaultTypeNotDefined )
                {
                User::Leave( KErrArgument );
                }
            else
                {
                // Safe to cast because the field is contact model field
                CContactStore& store = static_cast<CContactStore&>(
                    aField.ParentObject().ContactStore() );
                result = CDefaultAttributeOperation::NewSetLC(
                    store, aField, attribute, aObserver );
                result->StartL();
                CleanupStack::Pop(); // result
                }
            }
        }

    return result;
    }

MVPbkContactOperationBase* CDefaultAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if ( aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid() )
        {
        User::Leave( KErrNotSupported );
        }

    return result;
    }

MVPbkContactOperationBase* CDefaultAttributeManager::RemoveContactAttributeL(
        const MVPbkContactLink& /*aContactLink*/,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& /*aObserver*/)
    {
    MVPbkContactOperationBase* result = NULL;

    if ( aAttributeType == CVPbkDefaultAttribute::Uid() )
        {
        User::Leave( KErrNotSupported );
        }

    return result;
    }

MVPbkContactOperationBase* CDefaultAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& aField,
        const MVPbkContactAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver )
    {
    MVPbkContactOperation* result = NULL;
    
    if ( dynamic_cast<TContactField*>(&aField) )
        {        
        if ( aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid() )
            {
            const CVPbkDefaultAttribute& attribute =
                    static_cast<const CVPbkDefaultAttribute&>( aAttribute );

            MVPbkContactStore* store = iContactManager.ContactStoresL().Find(
                                                       KVPbkDefaultCntDbURI() );
            if ( store )
                {
                VPbkCntModel::CContactStore& cntStore = 
                            *static_cast<CContactStore*>( store );
                result = CDefaultAttributeOperation::NewRemoveLC(
                        cntStore, aField, attribute, aObserver );
                result->StartL();
                CleanupStack::Pop(); // result
                }
            else
                {
                User::Leave( KErrNotSupported );
                }
            }
        }
    return result;
    }

MVPbkContactOperationBase* CDefaultAttributeManager::RemoveFieldAttributeL(
        MVPbkStoreContactField& aField,
        TUid aAttributeType,
        MVPbkSetAttributeObserver& aObserver )
    {
    MVPbkContactOperationBase* result = NULL;
    
    if ( dynamic_cast<TContactField*>(&aField) )
        {        
        if ( aAttributeType == CVPbkDefaultAttribute::Uid() )
            {
            CVPbkDefaultAttribute* attribute = CVPbkDefaultAttribute::NewL(
                    CVPbkDefaultAttribute::KDefaultTypeNotDefined );
            CleanupStack::PushL( attribute );
            result = RemoveFieldAttributeL( aField, *attribute, aObserver );
            CleanupStack::PopAndDestroy( attribute );
            }
        }
    return result;
    }
    
MVPbkStoreContactFieldCollection* CDefaultAttributeManager::FindFieldsWithAttributeLC(
        TUid aAttributeType,
        MVPbkStoreContact& aContact ) const
    {
    CVPbkContactFieldCollection* result = NULL;
    
    if (dynamic_cast<CContact*>( &aContact ) &&
        aAttributeType == CVPbkDefaultAttribute::Uid() )
        {         
        result = CVPbkContactFieldCollection::NewLC(aContact);
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
        
MVPbkStoreContactFieldCollection* CDefaultAttributeManager::FindFieldsWithAttributeLC(
        const MVPbkContactAttribute& aAttribute,
        MVPbkStoreContact& aContact) const
    {
    CVPbkContactFieldCollection* result = NULL;
    
    if ( dynamic_cast<CContact*>( &aContact ) &&
        aAttribute.AttributeType() == CVPbkDefaultAttribute::Uid() )
        {        
        result = CVPbkContactFieldCollection::NewLC( aContact );
        const TInt count = aContact.Fields().FieldCount();
        for ( TInt i = 0; i < count; ++i )
            {
            MVPbkStoreContactField& field = aContact.Fields().FieldAt(i);
            if ( HasFieldAttributeL( aAttribute, field ) )
                {
                MVPbkStoreContactField* clone = field.CloneLC();
                result->AppendL( clone );
                CleanupStack::Pop(); // clone
                }
            }
        }
    return result;
    }  

MVPbkContactOperationBase* CDefaultAttributeManager::HasContactAttributeL(
        TUid /*aAttributeType*/, 
        const MVPbkStoreContact& /*aContact*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CDefaultAttributeManager::HasContactAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/, 
        const MVPbkStoreContact& /*aContact*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CDefaultAttributeManager::HasFieldAttributeL(
        TUid /*aAttributeType*/, 
        const MVPbkStoreContactField& /*aField*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CDefaultAttributeManager::HasFieldAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/, 
        const MVPbkStoreContactField& /*aField*/,
        MVPbkSingleAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CDefaultAttributeManager::FindFieldsWithAttributeL(
        TUid /*aAttributeType*/,
        MVPbkStoreContact& /*aContact*/,
        MVPbkMultiAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }

MVPbkContactOperationBase* CDefaultAttributeManager::FindFieldsWithAttributeL(
        const MVPbkContactAttribute& /*aAttribute*/,
        MVPbkStoreContact& /*aContact*/,
        MVPbkMultiAttributePresenceObserver& /*aObserver*/) const
    {
    User::Leave( KErrNotSupported );
    return NULL;    
    }
  
} // namespace VPbkCntModel

// End of File
