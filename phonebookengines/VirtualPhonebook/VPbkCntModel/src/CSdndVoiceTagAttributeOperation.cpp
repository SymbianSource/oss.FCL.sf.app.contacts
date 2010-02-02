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



#include "csdndvoicetagattributeoperation.h"
#include "ccontactstore.h"
#include "tcontactfield.h"
#include "ccontact.h"
#include "ccontactfilter.h"

#include <cvpbkvoicetagattribute.h>

#include <mvpbkstorecontactfield.h>
#include <mvpbkcontactattributemanager.h>
#include <mvpbkcontactattribute.h>
#include <cvpbkasyncoperation.h>
#include <cvpbkasynccallback.h>
#include <cvpbkcontactmanager.h>
#include <mvpbkcontactstorelist.h>
#include <vpbkstoreuriliterals.h>
#include <tvpbkcontactstoreuriptr.h>
#include <mvpbkcontactfindobserver.h>

// From Contacts Model
#include <cntitem.h>

namespace VPbkCntModel {

#define KVoiceTagValue      KUidContactsVoiceDialFieldValue

CSdndVoiceTagAttributeOperation::CSdndVoiceTagAttributeOperation(
        TOperationMode aOperationMode,
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        MVPbkSetAttributeObserver& aObserver ) :
    iOperationMode( aOperationMode ),
    iContactStore( aContactStore ),
    iField( &aField ),
    iSetObserver( &aObserver )
    {
    }

CSdndVoiceTagAttributeOperation::CSdndVoiceTagAttributeOperation(
        TOperationMode aOperationMode,
        CContactStore& aContactStore,
        MVPbkContactFindObserver& aObserver ) :
    iOperationMode( aOperationMode ),
    iContactStore( aContactStore ),
    iFindObserver( &aObserver )
    {
    }

void CSdndVoiceTagAttributeOperation::ConstructL(
        const CVPbkVoiceTagAttribute& aAttribute)
    {
    iAsyncOperation = new ( ELeave ) VPbkEngUtils::CVPbkAsyncOperation;
    iAttribute = static_cast<CVPbkVoiceTagAttribute*>(aAttribute.CloneLC() );
    CleanupStack::Pop();
    }

CSdndVoiceTagAttributeOperation* CSdndVoiceTagAttributeOperation::NewSetLC(
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        const CVPbkVoiceTagAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver )
    {
    CSdndVoiceTagAttributeOperation* self = new( ELeave ) 
    CSdndVoiceTagAttributeOperation( ESet, aContactStore, aField, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aAttribute );
    return self;
    }

CSdndVoiceTagAttributeOperation* CSdndVoiceTagAttributeOperation::NewRemoveLC(
        CContactStore& aContactStore,
        MVPbkStoreContactField& aField,
        const CVPbkVoiceTagAttribute& aAttribute,
        MVPbkSetAttributeObserver& aObserver )
    {
    CSdndVoiceTagAttributeOperation* self = new ( ELeave ) 
            CSdndVoiceTagAttributeOperation( ERemove, aContactStore, 
                                             aField, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aAttribute );
    return self;
    }

CSdndVoiceTagAttributeOperation* CSdndVoiceTagAttributeOperation::NewListLC(
        CContactStore& aContactStore,
        const CVPbkVoiceTagAttribute& aAttribute,
        MVPbkContactFindObserver& aObserver )
    {
    CSdndVoiceTagAttributeOperation* self = new ( ELeave ) 
        CSdndVoiceTagAttributeOperation( EList, aContactStore, aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aAttribute );
    return self;
    }

CSdndVoiceTagAttributeOperation::~CSdndVoiceTagAttributeOperation()
    {
    delete iContactFilter;
    delete iAsyncOperation;
    delete iAttribute;
    }

void CSdndVoiceTagAttributeOperation::StartL()
    {
    if ( iOperationMode == EList )
        {
        VPbkEngUtils::MAsyncCallback* callback =
            VPbkEngUtils::CreateAsyncCallbackLC(
                *this,
                &CSdndVoiceTagAttributeOperation::DoListOperationL,
                &CSdndVoiceTagAttributeOperation::ListOperationError,
                *iFindObserver);
        iAsyncOperation->CallbackL( callback );
        CleanupStack::Pop( callback );
        }
    else
        {
        VPbkEngUtils::MAsyncCallback* callback =
            VPbkEngUtils::CreateAsyncCallbackLC(
                *this,
                &CSdndVoiceTagAttributeOperation::DoSetOperationL,
                &CSdndVoiceTagAttributeOperation::SetOperationError,
                *iSetObserver );
        iAsyncOperation->CallbackL( callback );
        CleanupStack::Pop( callback );
        }
    }

void CSdndVoiceTagAttributeOperation::DoListOperationL(
        MVPbkContactFindObserver& /*aObserver*/)
    {
    delete iContactFilter;
    iContactFilter = NULL;
    
    iContactFilter = CContactFilter::NewL( *this, iContactStore );
    iContactFilter->StartL();
    }

void CSdndVoiceTagAttributeOperation::ListOperationError(
        MVPbkContactFindObserver& aObserver,
        TInt aError )
    {
    aObserver.FindFailed( aError );
    }

void CSdndVoiceTagAttributeOperation::DoSetOperationL(
        MVPbkSetAttributeObserver& aObserver)
    {
    if ( &iField->ContactStore() == &iContactStore )
        {
        CContact& contact = static_cast<CContact&>(iField->ParentContact() );
        TContactField& field = static_cast<TContactField&>( *iField );

        if (iOperationMode == ESet )
            {
	        TFieldType fieldType = TFieldType::Uid( KVoiceTagValue );
	        if ( fieldType == KNullUid )
		        {
		        User::Leave( KErrNotSupported );
		        }
            // Add specified type to field
		    field.NativeField()->AddFieldTypeL( fieldType) ;
            }
        else if ( iOperationMode == ERemove )
            {
	        TFieldType fieldType = TFieldType::Uid( KVoiceTagValue );
	        if ( fieldType == KNullUid )
		        {
		        User::Leave( KErrNotSupported );
		        }

            // Remove specified type from field
            field.NativeField()->RemoveFieldType(fieldType);
            }
        }

    aObserver.AttributeOperationComplete( *this );
    }

void CSdndVoiceTagAttributeOperation::SetOperationError(
        MVPbkSetAttributeObserver& aObserver,
        TInt aError )
    {
    aObserver.AttributeOperationFailed( *this, aError );
    }

void CSdndVoiceTagAttributeOperation::Cancel()
    {
    iAsyncOperation->Purge();
    }

TBool CSdndVoiceTagAttributeOperation::IsIncluded( MVPbkStoreContact& aContact )
	{
	TBool result( EFalse );
	CContact& contact = static_cast<CContact&>( aContact );
	CContactItemFieldSet& fields = contact.NativeContact()->CardFields();
    const TInt count = fields.Count();

    for ( TInt i = 0; i < count && !result; ++i )
        {
		TFieldType fieldType = TFieldType::Uid(KVoiceTagValue );
        if ( fields[i].ContentType().ContainsFieldType(fieldType) )
            {
            result = ETrue;
            break;
        	}
        }
    return result;
	}

void CSdndVoiceTagAttributeOperation::FilteringDoneL( MVPbkContactLinkArray* 
                                                        aLinkArray )
	{
	iFindObserver->FindCompleteL( aLinkArray );
	}

void CSdndVoiceTagAttributeOperation::FilteringError( TInt aError )
	{
	iFindObserver->FindFailed(aError);
	}

} // namespace VPbkCntModel

// End of File
