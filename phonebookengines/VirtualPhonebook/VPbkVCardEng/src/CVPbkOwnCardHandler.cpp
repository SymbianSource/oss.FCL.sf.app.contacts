/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler to manage owncard property during import and export
*
*/


#include "CVPbkOwnCardHandler.h"
#include <MVPbkContactStore.h>
#include <MVPbkContactStore2.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactStoreProperties2.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactIdConverter.h>
#include <MVPbkStoreContact2.h>

_LIT(KSelfValue,"1");
_LIT8(KSelf,"X-SELF");

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::CVPbkOwnCardHandler
// ----------------------------------------------------------------------------
CVPbkOwnCardHandler::CVPbkOwnCardHandler(CVPbkVCardData& aData):
	iData(aData)
    {
    iContactLink = NULL;
 	iContact = NULL;
    }

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::NewL
// ----------------------------------------------------------------------------
CVPbkOwnCardHandler* CVPbkOwnCardHandler::NewL(CVPbkVCardData& aData)
    {
    CVPbkOwnCardHandler* self = new (ELeave) CVPbkOwnCardHandler(aData);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::ConstructL
// ----------------------------------------------------------------------------
 void CVPbkOwnCardHandler::ConstructL()
    {
    iWait = new( ELeave )CActiveSchedulerWait();
 	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::~CVPbkOwnCardHandler
// ----------------------------------------------------------------------------
CVPbkOwnCardHandler::~CVPbkOwnCardHandler()
    {
     if ( iWait )
		{
    	if( iWait->IsStarted() )
    		iWait->AsyncStop();
		delete iWait;
		iWait = NULL;
	    }
	    delete iContactLink;
	    delete iContact;
    }

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::DoInitL
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::DoInitL(MVPbkContactStore& aStore )
    {
    MVPbkContactStoreProperties& storeProperties = const_cast<MVPbkContactStoreProperties&>( aStore.StoreProperties() );
    MVPbkContactStoreProperties2* storeProperties2 = 
        reinterpret_cast<MVPbkContactStoreProperties2*>
            ( storeProperties.ContactStorePropertiesExtension
                ( KMVPbkContactStorePropertiesExtension2Uid ) );

    if( !storeProperties2 )
        {
        return;
        }

    TBool supportsOwnContact = storeProperties2->SupportsOwnContact();

    if( supportsOwnContact )
        {
        MVPbkContactStore2* contactStoreExtension = 
            reinterpret_cast<MVPbkContactStore2*>
                ( aStore.ContactStoreExtension
                    ( KMVPbkContactStoreExtension2Uid ) );

        if( contactStoreExtension )
            {
            contactStoreExtension->OwnContactLinkL(*this);
            }
        }


	//Wait for the retrieval of OwnContactLinkL to complete/fail
	if( ! (iWait->IsStarted()) && supportsOwnContact)
	    {
	    	iWait->Start();
	    }
	  }

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::VPbkSingleContactLinkOperationComplete
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::VPbkSingleContactLinkOperationComplete(
	MVPbkContactOperationBase& aOperation,
	MVPbkContactLink* aContactLink )
	{

	TRAP_IGNORE(CloneContactLinkL(aContactLink));

	MVPbkContactOperationBase* operation = &aOperation;
	if ( operation )
		{
		delete operation;
		operation = NULL;
		}
	if( iWait->IsStarted() )
	    iWait->AsyncStop();
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::VPbkSingleContactLinkOperationComplete
// ----------------------------------------------------------------------------

void CVPbkOwnCardHandler::CloneContactLinkL(MVPbkContactLink* aContactLink)
{
	iContactLink = aContactLink->CloneLC();
	CleanupStack::Pop();
}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::VPbkSingleContactLinkOperationFailed
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::VPbkSingleContactLinkOperationFailed(
	MVPbkContactOperationBase& aOperation,
	TInt /*aError*/ )
	{
	MVPbkContactOperationBase* operation = &aOperation;
	if ( operation )
		{
		delete operation;
		operation = NULL;
		}
	if( iWait->IsStarted() )
    	iWait->AsyncStop();
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::GetselfValueLL
// ----------------------------------------------------------------------------
CParserPropertyValue* CVPbkOwnCardHandler::GetselfValueL()
	{
	TPtrC text( KSelfValue);
	CParserPropertyValueHBufC* value = CParserPropertyValueHBufC::NewL( text );
	return value;
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::CreateXSelfPropertyL
// ----------------------------------------------------------------------------
CParserProperty* CVPbkOwnCardHandler::CreateXSelfPropertyL()
	{
	CParserPropertyValue* value = GetselfValueL();
	CVPbkVCardParserParamArray* params = NULL;

	TPtrC8 text( KSelf);
	CParserProperty* property =
	        CParserProperty::NewL( *value,text, params);

	return property;
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::VPbkSingleContactOperationComplete
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::VPbkSingleContactOperationComplete(
    MVPbkContactOperationBase& aOperation,
    MVPbkStoreContact* aContact)
	{
	iContact= aContact;

	MVPbkContactOperationBase* operation = &aOperation;
	if ( operation )
		{
		delete operation;
		operation = NULL;
		}

	MVPbkStoreContact2* tempContact = 
	    reinterpret_cast<MVPbkStoreContact2*>
	        (iContact->StoreContactExtension
	            (KMVPbkStoreContactExtension2Uid));
	TRAP_IGNORE(tempContact->SetAsOwnL(*this));
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::VPbkSingleContactOperationFailed
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::VPbkSingleContactOperationFailed
	(MVPbkContactOperationBase& aOperation, TInt /*aError*/)
	{
	MVPbkContactOperationBase* operation = &aOperation;
	if ( operation )
		{
		delete operation;
		operation = NULL;
		}

	if( iWait->IsStarted() )
		iWait->AsyncStop();
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::IsOwnContactL
// Check if the contact being exported is the owncard
// ----------------------------------------------------------------------------
TBool CVPbkOwnCardHandler::IsOwnContactL(const MVPbkStoreContact* aContact)
{
	if(iContactLink)
		{
		TBool same = iContactLink->IsSame(*aContact->CreateLinkLC());
		CleanupStack::PopAndDestroy();
		return same;
		}
	return EFalse;
}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::SetAsOwnContactL
// Retrieve the contact and set it as owncontact
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::SetAsOwnContactL(const MVPbkContactLink& aContactLink)
	{
	iData.GetContactManager().RetrieveContactL( aContactLink, *this );
	//wait for SetAsOwnL operation to complete
	if( ! (iWait->IsStarted()) )
	    {
	    	iWait->Start();
	    }
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::ContactOperationCompleted
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::ContactOperationCompleted(TContactOpResult aResult)
	{
	delete aResult.iStoreContact;
	if( iWait->IsStarted() )
    	iWait->AsyncStop();
	}

// ----------------------------------------------------------------------------
// CVPbkOwnCardHandler::ContactOperationFailed
// ----------------------------------------------------------------------------
void CVPbkOwnCardHandler::ContactOperationFailed
    (TContactOp /*aOpCode*/, TInt /*aErrorCode*/, TBool /*aErrorNotified*/)
    {
    if( iWait->IsStarted() )
    	iWait->AsyncStop();
    }
