/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contacts handler for client API test suite
*
*/


//SYSTEM INCLUDES
#include <utf.h>
#include <e32std.h>
#include <badesca.h>
#include <cvpbkcontactmanager.h>
#include <cvpbkcontactstoreuriarray.h>
#include <cvpbkcontactfielditerator.h>
#include <cvpbkfieldtyperefslist.h>
#include <cvpbkcontactlinkarray.h>
#include <tvpbkcontactstoreuriptr.h>
#include <mvpbkcontactoperationbase.h>
#include <mvpbkcontactstorelist.h>
#include <mvpbkcontactstore.h>
#include <mvpbkstorecontact.h>
#include <MVPbkContactFieldData.h>
#include <mvpbkcontactfieldtextdata.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <mvpbkcontactstoreproperties.h>
#include <mvpbkContactLink.h>
#include <MVPbkContactGroup.h>
#include <vpbkeng.rsg>
#include <vpbkcontactstoreuris.h>
#include <tvpbkwordParsercallbackParam.h>
#include <tvpbkcontactstoreuriptr.h>
#include <CVPbkContactIdConverter.h>
#include <barsread.h>
#include <coemain.h>

//USER INCLUDES
#include "pstestcontactshandler.h"

_LIT(KDummyPhoneNumber,"9900211");

// ---------------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------------
CPcsTestContactsHandler* CPcsTestContactsHandler::NewL(TBool SimStoreOnly)
{
	CPcsTestContactsHandler* self = new ( ELeave ) CPcsTestContactsHandler();
	CleanupStack::PushL( self );
	self->ConstructL(SimStoreOnly);
	CleanupStack::Pop( self );
	
	return self;
}

// ---------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------
//
CPcsTestContactsHandler::CPcsTestContactsHandler() : CActive( CActive::EPriorityStandard ),
													 iAtLeastOneStoreReady(EFalse),
													 iStoreEventToWaitFor(KErrNotFound),
													 iExpectedStoreEventReceived(ETrue)
{
	CActiveScheduler::Add( this );
}

// ---------------------------------------------------------------------------------
// 2nd phase constructor
// ---------------------------------------------------------------------------------
void CPcsTestContactsHandler::ConstructL(TBool SimStoreOnly)
{
	iOperationComplete = EFalse;

	// initialize virtual phonebook with phonebook and sim contact uris.
	CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
	if(SimStoreOnly)
	{
		uriArray->AppendL( TVPbkContactStoreUriPtr( VPbkContactStoreUris::SimGlobalAdnUri() ) );
	}
	else
	{
		uriArray->AppendL( TVPbkContactStoreUriPtr(VPbkContactStoreUris::DefaultCntDbUri()));
		//The below  line is not actually reqired, but kept here for code coverage
		//uriArray->AppendL( TVPbkContactStoreUriPtr( VPbkContactStoreUris::SimGlobalAdnUri() ) );
	}

    // Create the contact manager
	iContactManager = CVPbkContactManager::NewL(*uriArray);
	CleanupStack::PopAndDestroy(uriArray); 
	
	// Create the active scheduler wait
	iWait = new( ELeave )CActiveSchedulerWait(); 

	// Open al the stores
	iContactManager->ContactStoresL().OpenAllL( *this );
	if(SimStoreOnly)
	{
		iStore = iContactManager->ContactStoresL().Find( TVPbkContactStoreUriPtr( VPbkContactStoreUris::SimGlobalAdnUri() ) );
	}
	else
	{
		 iStore = iContactManager->ContactStoresL().Find( TVPbkContactStoreUriPtr( VPbkContactStoreUris::DefaultCntDbUri() ) ); 	
	}

	iStore->OpenL( *this ); 
	
	// Wait until the stores are open
	while(!iOpenComplete)
    {	
		if( !(iWait->IsStarted()) ) 
		iWait->Start();
    }
    
	iContactsCreatedLinks = CVPbkContactLinkArray::NewL();
	iContactLinksToAddToGroup = CVPbkContactLinkArray::NewL();
}

// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------

CPcsTestContactsHandler::~CPcsTestContactsHandler() 
{

	delete iWait;
    iWait = NULL;
    
    if(iContactsCreatedLinks)
        {
        iContactsCreatedLinks->ResetAndDestroy();
        delete iContactsCreatedLinks;
        }
    if(iContactLinksToAddToGroup)
        {
        iContactLinksToAddToGroup->ResetAndDestroy();
        delete iContactLinksToAddToGroup;
        }
//plese note you shouldn't close stores before cleaning links arrays    
	if ( iContactManager )
	{
	    TRAP_IGNORE(iContactManager->ContactStoresL().CloseAll( *this ));
 		delete iContactManager;
		iContactManager = NULL;	
	}

	if(iOp)
	{
		 delete iOp ;
		 iOp = NULL;
	}

}

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver.
// ---------------------------------------------------------------------------
void CPcsTestContactsHandler::OpenComplete()
{
    iOpenComplete = ETrue;
    if( iWait->IsStarted() )
     {
 	    iWait->AsyncStop();
     }
}
    
// ---------------------------------------------------------------------------
// // Call back method. Called if the store is available
// ---------------------------------------------------------------------------
//
void CPcsTestContactsHandler::StoreReady(MVPbkContactStore& /*aStore*/)
{
	iAtLeastOneStoreReady = ETrue;
}

// ---------------------------------------------------------------------------
// Call back method. Called if the store is not available
// ---------------------------------------------------------------------------
void CPcsTestContactsHandler::StoreUnavailable(
        MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/)
{
    // The store is not available 
    // So do nothing ....
}

// ---------------------------------------------------------------------------
// Handles the events received from the contact store.
// ---------------------------------------------------------------------------
void CPcsTestContactsHandler::HandleStoreEventL(MVPbkContactStore& /*aContactStore*/, 
                TVPbkContactStoreEvent aStoreEvent)
{

    if ( aStoreEvent.iEventType == iStoreEventToWaitFor )
        {
        iExpectedStoreEventReceived = ETrue;
        }

	switch (aStoreEvent.iEventType) 
	{
		case TVPbkContactStoreEvent::EContactAdded:
		{
			// If a group is added, then we need to add this contact to the group
		    if(iGroupAdded)
		    {
		    	MVPbkContactLink* temp = aStoreEvent.iContactLink->CloneLC();
		    	iContactLinksToAddToGroup->AppendL(temp);
		    	CleanupStack::Pop();
		    }
			break;
		}
		
		case TVPbkContactStoreEvent::EContactDeleted:
		case TVPbkContactStoreEvent::EContactChanged:
		{
			break;
		}
		default:
			break;
	}
	StopSchedulerIfNothingToWaitFor();
}

// ---------------------------------------------------------------------------------
// This fucntion creates a new contact in the phonebook with the specified 
// vpbkIds and a defualt Phone Number
// Callback functions: ContactOperationCompleted() and ContactOperationFailed()
// ---------------------------------------------------------------------------------    
void CPcsTestContactsHandler::CreateOneContactL(cacheData& aInputCacheData)
{
	//wait for operation completion
    iOperationComplete = EFalse;
	//wait for confirmation that contact was added
    iExpectedStoreEventReceived = EFalse;
    iStoreEventToWaitFor = TVPbkContactStoreEvent::EContactAdded;
    
    fieldTypeRefList = CVPbkFieldTypeRefsList::NewL();
    CleanupStack::PushL(fieldTypeRefList);
    
    //Check if the input is correct
    if( aInputCacheData.vpbkId.Count() != aInputCacheData.data.Count())
    {
    	User::Leave(KErrGeneral);
    }
    
    // Add the vpbkIds from the data to the fieldTypeRefList
    for(TInt i=0; i < aInputCacheData.vpbkId.Count();i++)
    {
    	fieldTypeRefList->AppendL(*iContactManager->FieldTypes().Find( aInputCacheData.vpbkId[i]));
    }
    
    // Add the mobile phone also. (Basically for code coverage reasons)
	fieldTypeRefList->AppendL(*iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN ) );

	// Create the contact
    iStoreContact = iStore->CreateNewContactLC();

	// Add data to respective ids
	for(TInt i=0; i < aInputCacheData.vpbkId.Count();i++)
    {
	 	MVPbkStoreContactField* dataField = iStoreContact->CreateFieldLC( fieldTypeRefList->FieldTypeAt( i ) );
		(MVPbkContactFieldTextData::Cast(dataField->FieldData())).SetTextL( *(aInputCacheData.data[i]) );	
		iStoreContact->AddFieldL( dataField );
		CleanupStack::Pop(); // dataField
	}
	
    // We have hardcoded the phone number, since this field is not used for search. 
    // This field has been added for code coverage improvment
	MVPbkStoreContactField* PhoneNumberfield = iStoreContact->CreateFieldLC( fieldTypeRefList->FieldTypeAt(aInputCacheData.vpbkId.Count() ) );
	(MVPbkContactFieldTextData::Cast(PhoneNumberfield->FieldData())).SetTextL( KDummyPhoneNumber);	
	iStoreContact->AddFieldL( PhoneNumberfield );
	CleanupStack::Pop(); // PhoneNumberfield
	
	// Commit the contact data
    iStoreContact->CommitL( *this );		
	CleanupStack::Pop(); // iStoreContact
	CleanupStack::Pop(); // fieldTypeRefList

	if( !(iWait->IsStarted()) ) 
	{
		iWait->Start();
   	}
	delete fieldTypeRefList;
	fieldTypeRefList = NULL;
}

// ---------------------------------------------------------------------------------
// This fucntion deletes all the created contacts
// ---------------------------------------------------------------------------------    

void CPcsTestContactsHandler::DeleteAllCreatedContactsL()
{
	if(iContactsCreatedLinks->Count())
	{
	    iOperationComplete = EFalse;
	    // Delete all the contacts
		iOp = iContactManager->DeleteContactsL( *iContactsCreatedLinks, *this );
		while(!iOperationComplete)
	    {	
			if( !(iWait->IsStarted()) ) 
			iWait->Start();
	    }
	}
}    

// ---------------------------------------------------------------------------------
// This fucntion adds a group 
// ---------------------------------------------------------------------------------    

void CPcsTestContactsHandler::AddGroupL(const TDesC& aGroupName)
{
	iGroupAdded = iStore->CreateNewContactGroupLC();
	iGroupAdded->SetGroupLabelL( aGroupName );
	iGroupAdded->CommitL(*this);
	CleanupStack::Pop();    
}

// ---------------------------------------------------------------------------------
// From CActive
// Handles an active object's request completion event.
// ---------------------------------------------------------------------------------
void CPcsTestContactsHandler::RunL() 
{
	if(!iOperationComplete)
	{
	    TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );
		SetActive();
	}
	else
	{
        StopSchedulerIfNothingToWaitFor();
	}
}

// ---------------------------------------------------------------------------------
//  If the RunL function leaves,
// then the active scheduler calls RunError() to handle the leave.
// ---------------------------------------------------------------------------------
TInt CPcsTestContactsHandler::RunError(TInt /*aError*/) 
{
	return KErrNone;
}

// ---------------------------------------------------------------------------------
// From CActive
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------------
void CPcsTestContactsHandler::DoCancel() 
{
    // Not supported
}

// ---------------------------------------------------------------------------------
// Stops main active scheduler loop if stop conditions are fullfiled
// ---------------------------------------------------------------------------------
void CPcsTestContactsHandler::StopSchedulerIfNothingToWaitFor()
    {
     if( iOperationComplete && iExpectedStoreEventReceived )
        {
        CActiveScheduler::Stop();
        }
    }

// ---------------------------------------------------------------------------------
// Called when a contact operation has succesfully completed.
// Callback for CommitL
// ---------------------------------------------------------------------------------    
void CPcsTestContactsHandler::ContactOperationCompleted(TContactOpResult /*aResult*/)
{    
	iOperationComplete = ETrue;
	delete iStoreContact;
	iStoreContact = NULL;
	
	if( iWait->IsStarted() )
	{
		iWait->AsyncStop();
	}

	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	SetActive();
}

// ---------------------------------------------------------------------------------
// Called when a contact operation has failed
// Callback for CommitL
// ---------------------------------------------------------------------------------    
void CPcsTestContactsHandler::ContactOperationFailed
           (TContactOp /*aOpCode*/, TInt /*aErrorCode*/, TBool /*aErrorNotified*/)
{
	iOperationComplete = ETrue;
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	SetActive();
}

// ---------------------------------------------------------------------------------
// Called when one step of the operation is completed.
// Callback for DeleteContactsL, 
// ---------------------------------------------------------------------------------    
void CPcsTestContactsHandler::StepComplete(MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/ )
{
}

// ---------------------------------------------------------------------------------
// CPcsTestContactsHandler::StepComplete() 
// Called when one step of the operation fails.
// Callback for DeleteContactsL, 
// ---------------------------------------------------------------------------------    
TBool CPcsTestContactsHandler::StepFailed(MVPbkContactOperationBase& /*aOperation*/, TInt /*aStepSize*/, TInt /*aError*/ )
{
	return ETrue;
}

// ---------------------------------------------------------------------------------
// Called when operation is completed.
// This is called when all steps are executed. If EFalse is returned
// in StepFailed() call this function is NOT called.
// Callback for DeleteContactsL, 
// ---------------------------------------------------------------------------------    
void CPcsTestContactsHandler::OperationComplete(MVPbkContactOperationBase& aOperation )
{
	iOperationComplete = ETrue;

//	iDeleteComplete = ETrue;
	if( iWait->IsStarted() )
	{
		iWait->AsyncStop();
	}

	iOp = &aOperation ;         
	if( iOp  )
	{
		delete iOp;
		iOp = NULL;
	}
	
	delete iContactLinksToDelete;
	iContactLinksToDelete = NULL;

	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	SetActive();
}

// ---------------------------------------------------------------------------------
// This function deltes contacts with the id specified
// ---------------------------------------------------------------------------------    
void CPcsTestContactsHandler::DeleteContactsWithIdsL(RArray<TInt>& aContactIdArray)
{
	CVPbkContactIdConverter* converter = NULL;
    TRAPD ( err, converter = CVPbkContactIdConverter::NewL( *iStore ) );
    
	if ( err != KErrNotSupported )
    {
     	iContactLinksToDelete = CVPbkContactLinkArray::NewL();
		for(TInt i =0;i < aContactIdArray.Count(); i++)
		{
			// get the contact link
			MVPbkContactLink* contactLink = NULL;
			contactLink = converter->IdentifierToLinkLC(aContactIdArray[i]);
			CleanupStack::Pop();
	     	
	     	MVPbkContactLink* temp = contactLink->CloneLC();
		    iContactLinksToDelete->AppendL(temp);
		    CleanupStack::Pop();
		}
		
		// Delete the contacts
		iOp = iContactManager->DeleteContactsL( *iContactLinksToDelete, *this );
		while(!iOperationComplete)
	    {	
			if( !(iWait->IsStarted()) ) 
			iWait->Start();
	    }
    }
    
    // Delete the converter
    if(converter)
	{
		delete converter;
		converter = NULL;
	} 
}

// -----------------------------------------------------------------------------
// Tests if Id returns correct vpbklink
// -----------------------------------------------------------------------------

TBool CPcsTestContactsHandler::TestVpbkLinkForIdL(CPSRequestHandler& aPsClientHandler,const CPsClientData& aSearchResult)
{
	TBool ret = EFalse;
	// Get the link from Ps engine
	MVPbkContactLink* tmpLink1 = aPsClientHandler.ConvertToVpbkLinkLC( aSearchResult,*iContactManager );
	
	// Get the link from the manager
	CVPbkContactIdConverter* converter = NULL;
    TRAPD ( err, converter = CVPbkContactIdConverter::NewL( *iStore ) );
    MVPbkContactLink* tmpLink2 = NULL;
	if ( err != KErrNotSupported )
    {
    	// get the contact link
		tmpLink2 = converter->IdentifierToLinkLC(aSearchResult.Id());
		ret =  (tmpLink1->IsSame(*tmpLink2));
		CleanupStack::Pop(2);
	}
	else
	{
		CleanupStack::PopAndDestroy();
		ret = EFalse;
	}
	
	// Cleanup
	delete converter;
	converter = NULL;
	
	return ret;
}

// -----------------------------------------------------------------------------
// CPcsTestContactsHandler::AddCreatedContactsToGroup()
// -----------------------------------------------------------------------------
void CPcsTestContactsHandler::AddCreatedContactsToGroup()
{
    if(iGroupAdded)
    {
        for(TInt i=0;i<iContactLinksToAddToGroup->Count();i++ )
            iGroupAdded->AddContactL(iContactLinksToAddToGroup->At(i));
    }
}
// End of file
