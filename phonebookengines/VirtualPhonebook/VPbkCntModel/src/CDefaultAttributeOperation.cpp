/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  The virtual phonebook default attribute operation
*
*/


#include "CDefaultAttributeOperation.h"
#include "CContactStore.h"
#include "TContactField.h"
#include "CContact.h"
#include "VPbkCntModelUid.h"
#include "CContactFilter.h"

#include <CVPbkDefaultAttribute.h>

#include <MVPbkStoreContactField.h>
#include <MVPbkContactAttributeManager.h>
#include <MVPbkContactAttribute.h>
#include <CVPbkAsyncOperation.h>
#include <CVPbkAsyncCallback.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStoreList.h>
#include <VPbkStoreUriLiterals.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactFindObserver.h>

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
} // unnamed namespace

CDefaultAttributeOperation::CDefaultAttributeOperation(
        TOperationMode aOperationMode,
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        MVPbkSetAttributeObserver& aObserver) :
    iOperationMode(aOperationMode),
    iContactStore(aContactStore),
    iField(&aField),
    iSetObserver(&aObserver)
    {
    }

CDefaultAttributeOperation::CDefaultAttributeOperation(
        TOperationMode aOperationMode,
        CContactStore& aContactStore,
        MVPbkContactFindObserver& aObserver) :
    iOperationMode(aOperationMode),
    iContactStore(aContactStore),
    iFindObserver(&aObserver)
    {
    }

void CDefaultAttributeOperation::ConstructL(
        const CVPbkDefaultAttribute& aAttribute)
    {
    iAsyncOperation = new(ELeave) VPbkEngUtils::CVPbkAsyncOperation;
    iAttribute = static_cast<CVPbkDefaultAttribute*>(aAttribute.CloneLC());
    CleanupStack::Pop();
    }

CDefaultAttributeOperation* CDefaultAttributeOperation::NewSetLC(
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        const CVPbkDefaultAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    CDefaultAttributeOperation* self = new(ELeave) CDefaultAttributeOperation(
            ESet, aContactStore, aField, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aAttribute);
    return self;
    }

CDefaultAttributeOperation* CDefaultAttributeOperation::NewRemoveLC(
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        const CVPbkDefaultAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver)
    {
    CDefaultAttributeOperation* self = new(ELeave) CDefaultAttributeOperation(
            ERemove, aContactStore, aField, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aAttribute);
    return self;
    }

CDefaultAttributeOperation* CDefaultAttributeOperation::NewListLC(
        CContactStore& aContactStore,
        const CVPbkDefaultAttribute& aAttribute,
        MVPbkContactFindObserver& aObserver)
    {
    CDefaultAttributeOperation* self = new(ELeave) CDefaultAttributeOperation(
            EList, aContactStore, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aAttribute);
    return self;
    }

CDefaultAttributeOperation::~CDefaultAttributeOperation()
    {
    delete iContactFilter;
    delete iAsyncOperation;
    delete iAttribute;
    }

void CDefaultAttributeOperation::StartL()
    {
    if (iOperationMode == EList)
        {
        VPbkEngUtils::MAsyncCallback* callback =
            VPbkEngUtils::CreateAsyncCallbackLC(
                *this,
                &CDefaultAttributeOperation::DoListOperationL,
                &CDefaultAttributeOperation::ListOperationError,
                *iFindObserver);
        iAsyncOperation->CallbackL(callback);
        CleanupStack::Pop(callback);
        }
    else
        {
        VPbkEngUtils::MAsyncCallback* callback =
            VPbkEngUtils::CreateAsyncCallbackLC(
                *this,
                &CDefaultAttributeOperation::DoSetOperationL,
                &CDefaultAttributeOperation::SetOperationError,
                *iSetObserver);
        iAsyncOperation->CallbackL(callback);
        CleanupStack::Pop(callback);
        }
    }

void CDefaultAttributeOperation::DoListOperationL(
        MVPbkContactFindObserver& /*aObserver*/)
    {
    delete iContactFilter;
    iContactFilter = NULL;
    
    iContactFilter = CContactFilter::NewL(*this, iContactStore);
    iContactFilter->StartL();
    }

void CDefaultAttributeOperation::ListOperationError(
        MVPbkContactFindObserver& aObserver,
        TInt aError)
    {
    aObserver.FindFailed(aError);
    }

void CDefaultAttributeOperation::DoSetOperationL(
        MVPbkSetAttributeObserver& aObserver)
    {
    if (&iField->ContactStore() == &iContactStore)
        {
        CContact& contact = static_cast<CContact&>(iField->ParentContact());
        TContactField& field = static_cast<TContactField&>(*iField);

        if (iOperationMode == ESet)
            {
	        TFieldType fieldType = FindFieldTypeForProperty(iAttribute->DefaultType());
	        if (fieldType == KNullUid)
		        {
		        User::Leave(KErrNotSupported);
		        }

		    field.NativeField()->AddFieldTypeL(fieldType);
            }
        else if (iOperationMode == ERemove)
            {
            if (iAttribute->DefaultType() == CVPbkDefaultAttribute::KDefaultTypeNotDefined)
                {
                // If field contains any default type, remove it
                for (TInt n = 0;
                	fieldTypeMap[n].iProperty != EVPbkLastDefaultType;
                	++n)
                	{
					TFieldType fieldType = TFieldType::Uid(fieldTypeMap[n].iUid);
                    if (field.NativeField()->ContentType().ContainsFieldType(fieldType))
                        {
                        field.NativeField()->RemoveFieldType(fieldType);
                        }
                	}
                }
            else
                {
		        TFieldType fieldType = FindFieldTypeForProperty(iAttribute->DefaultType());
		        if (fieldType == KNullUid)
			        {
			        User::Leave(KErrNotSupported);
			        }

                // Remove specified type from field
                field.NativeField()->RemoveFieldType(fieldType);
                }
            }
        }

    aObserver.AttributeOperationComplete(*this);
    }

void CDefaultAttributeOperation::SetOperationError(
        MVPbkSetAttributeObserver& aObserver,
        TInt aError)
    {
    aObserver.AttributeOperationFailed(*this, aError);
    }

void CDefaultAttributeOperation::Cancel()
    {
    iAsyncOperation->Purge();
    }

TBool CDefaultAttributeOperation::IsIncluded(MVPbkStoreContact& aContact)
	{
	TBool result(EFalse);
	CContact& contact = static_cast<CContact&>(aContact);
	CContactItemFieldSet& fields = contact.NativeContact()->CardFields();
    const TInt count = fields.Count();

    for (TInt i = 0; i < count && !result; ++i)
        {
		if (iAttribute->DefaultType() == CVPbkDefaultAttribute::KDefaultTypeNotDefined)
			{
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
		else
			{
			TFieldType fieldType = FindFieldTypeForProperty(iAttribute->DefaultType());
	        if (fields[i].ContentType().ContainsFieldType(fieldType))
	            {
	            result = ETrue;
	            break;
	        	}
			}
        }
    return result;
	}

void CDefaultAttributeOperation::FilteringDoneL(MVPbkContactLinkArray* aLinkArray)
	{
	iFindObserver->FindCompleteL(aLinkArray);
	}

void CDefaultAttributeOperation::FilteringError(TInt aError)
	{
	iFindObserver->FindFailed(aError);
	}

} // namespace VPbkCntModel

// End of File
