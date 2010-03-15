/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Active object to read all the contacts from a 
*                particular data store. Handles add/ modify/ delete of contacts.
*
*/

// SYSTEM INCLUDES
#include <barsread.h>
#include <coemain.h>
#include <utf.h>
#include <e32std.h>
#include <badesca.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <CVPbkContactFieldIterator.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactLinkArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactLink.h>
#include <VPbkEng.rsg>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkContactViewBase.h>
#include <VPbkContactView.hrh>
#include <CVPbkContactIdConverter.h>
#include <centralrepository.h>
#include <MVPbkContactGroup.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactLinkArray.h>
#include <CVPbkSortOrder.h>
#include <data_caging_path_literals.hrh>
#include <bautils.h>

// USER INCLUDES
#include "cpcscontactstore.h"
#include "CPsData.h"
#include "CPcsDebug.h" 
#include "CPcsDefs.h"
#include <contactsort.rsg>

// CONSTANTS
const TInt KSimStoreOffset            = -5000;

_LIT(KResourceFileName, "contactsort.rsc");
_LIT(KPcsViewPrefix,"PCSView_");

// FORWARD DECLARATION

// ============================== MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------------
// 2 phase construction
// ---------------------------------------------------------------------------------
CPcsContactStore* CPcsContactStore::NewL(CVPbkContactManager&  aContactManager,
                                         MDataStoreObserver& aObserver,
                                         const TDesC& aUri)
{
	PRINT ( _L("Enter CPcsContactStore::NewL") );

	CPcsContactStore* self = new ( ELeave ) CPcsContactStore( );
	CleanupStack::PushL( self );
	
	self->ConstructL(aContactManager,aObserver,aUri);
	CleanupStack::Pop( self );
	
	PRINT ( _L("End CPcsContactStore::NewL") );
	return self;
}

// ---------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------
CPcsContactStore::CPcsContactStore():
	CActive( CActive::EPriorityLow),
    iAllContactLinksCount(0),
    iFetchedContactCount(0),
    iContactViewReady(EFalse)
{
    PRINT ( _L("Enter CPcsContactStore::CPcsContactStore") );
    CActiveScheduler::Add( this );
	PRINT ( _L("End CPcsContactStore::CPcsContactStore") );
}


// ---------------------------------------------------------------------------------
// CPcsContactStore::ConstructL() 
// 2nd phase constructor
// ---------------------------------------------------------------------------------
void CPcsContactStore::ConstructL(CVPbkContactManager&  aContactManager,
                                  MDataStoreObserver& aObserver,
                                  const TDesC& aUri)
{
    PRINT ( _L("Enter CPcsContactStore::ConstructL") );
    
	iContactManager  = &aContactManager;
	iObserver = &aObserver;
	
	iUri = HBufC::NewL(aUri.Length());
	iUri->Des().Copy(aUri);
	
	// create containers for holding the sim data      
    iSimContactItems = CVPbkContactLinkArray::NewL();
   
	
	// for creating sort order
	iSortOrder = CVPbkFieldTypeRefsList::NewL();
	
	// Read the fields to cache from the central repository
	ReadFieldsToCacheFromCenrepL();
	
    // Local timer for block fetch delay
    iTimer.CreateLocal();
    
    // Initial state
	iNextState = ECreateView;
	IssueRequest();
	
	PRINT ( _L("End CPcsContactStore::ConstructL") );
}

// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------
CPcsContactStore::~CPcsContactStore() 
{
	PRINT ( _L("Enter CPcsContactStore::~CPcsContactStore") );
	
	delete iContactViewBase;
	iContactViewBase = NULL;
	
	delete iSortOrder;
	iSortOrder = NULL;
	
    delete iSimContactItems;
    iSimContactItems = NULL ;
     
	delete iWait;
	iWait = NULL;
	
	delete iUri;
	iUri = NULL;
		
	iTimer.Cancel();
    iTimer.Close();
	iFieldsToCache.Close();	
	
	iFs.Close();
	
	if(IsActive())
	{
		Deque();
	}
	
	PRINT ( _L("End CPcsContactStore::~CPcsContactStore") );
}


// ---------------------------------------------------------------------------------
// Handles addition/deletion/modification of contacts
// ---------------------------------------------------------------------------------
void CPcsContactStore::HandleStoreEventL(MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent)
{
   	PRINT ( _L("Enter CPcsContactStore::HandleStoreEventL") );

	switch (aStoreEvent.iEventType) 
	{
		case TVPbkContactStoreEvent::EContactAdded:
		{
		    PRINT ( _L("Add contact/group event received") );
		    iContactManager->RetrieveContactL( *(aStoreEvent.iContactLink),
	                                           *this);
			break;
		}
		
        case TVPbkContactStoreEvent::EUnknownChanges:
            {
            iObserver->RemoveAll( *iUri );
            iContactViewReady = EFalse;
            iContactViewBase->AddObserverL(*this);
            }
            break;

		case TVPbkContactStoreEvent::EContactDeleted:
		case TVPbkContactStoreEvent::EContactChanged:
		case TVPbkContactStoreEvent::EGroupDeleted:
		case TVPbkContactStoreEvent::EGroupChanged:
		{
		    if ( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactChanged ) 
		    {
		    	PRINT ( _L("Change contact/group event received") );
		    }
		    else 
		    {
		    	PRINT ( _L("Delete contact/group event received") );
		    }
			
			CVPbkContactIdConverter* converter = NULL;
			
		    TRAPD ( err, converter = CVPbkContactIdConverter::NewL( aContactStore ) );

	        TInt32 contactId(-1);
	        
	        if ( err == KErrNotSupported )
	        {
	            // sim domain  
	            // Pass the sim observer string in the next line
	            TInt contactLocation =  iSimContactItems->Find(*aStoreEvent.iContactLink);	
				if( KErrNotFound != contactLocation)
				{
				    // We are not removing from the array cache. If you try to fetch, 
				    // then it will give an error			       
				    TInt index = CreateCacheIDfromSimArrayIndex (contactLocation);
				    
	                iObserver->RemoveData(*iUri,index );
		       	} 	               	           
		               	           
	        }
	        else
	        {
	           // cntdb domain
	           // Get contact id by mapping the link
	           contactId = converter->LinkToIdentifier(*(aStoreEvent.iContactLink));
	           
	           // Remove the contact
		       iObserver->RemoveData(*iUri, contactId);
		    }
		    
		    delete converter;
		    converter = NULL;
		    
	        if (( aStoreEvent.iEventType == TVPbkContactStoreEvent::EContactChanged ) ||
	            ( aStoreEvent.iEventType == TVPbkContactStoreEvent::EGroupChanged ) )
	        {
	  		    // Add the contact
	  		   iContactManager->RetrieveContactL( *(aStoreEvent.iContactLink),
	  		                                                           *this );
	        }
	        	  		  
			break;
		}
	}

	PRINT ( _L("End CPcsContactStore::HandleStoreEventL") );
}

// ---------------------------------------------------------------------------
//  Returns ths database URI
// ---------------------------------------------------------------------------
TDesC& CPcsContactStore::GetStoreUri()
{
  return *iUri;
}

// ---------------------------------------------------------------------------
//  Callback Method. Called when one Retrieve operation is complete
// --------------------------------------------------------------------------- 
void CPcsContactStore::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation, MVPbkStoreContact* aContact)
{    
    iFetchedContactCount++;

			       
	// Handle the fetched contact....
	TRAPD(err, HandleRetrievedContactL(aContact) );
	if( err != KErrNone)
	{
		iObserver->UpdateCachingStatus(*iUri, err);
	}
	
	MVPbkContactOperationBase* Opr = &aOperation;
	if ( Opr )
	{
		delete Opr;
		Opr = NULL;
	}
	
	// Update the iNextState variable to proper state
	if( iFetchedContactCount == iAllContactLinksCount )
	{
	    // Fetch complete
	    // Update the caching status
		iObserver->UpdateCachingStatus(*iUri, ECachingComplete);
		iNextState = EComplete;
		IssueRequest();
	}
	else if ( (iFetchedContactCount % KLinksToFetchInOneGo) == 0 )
	{
	    // Fetch next block
		iNextState = EFetchContactBlock;
		IssueRequest();
	}
	
}
    
// ---------------------------------------------------------------------------
//  Callback Method.Called when one Retrieve operation fails.
// ---------------------------------------------------------------------------
void CPcsContactStore::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
{
  	PRINT ( _L("Enter CPcsContactStore::VPbkSingleContactOperationFailed") );
	iFetchedContactCount++;

	// Update the iNextState variable to proper state
	if( iFetchedContactCount == iAllContactLinksCount )
	{
		// Fetch complete
		iObserver->UpdateCachingStatus(*iUri, ECachingComplete);
		iNextState = EComplete;
		IssueRequest();
	}
	else if ( (iFetchedContactCount % KLinksToFetchInOneGo) == 0 ) 
	{
		// Fetch next block
		iNextState = EFetchContactBlock;
		IssueRequest();
	}
	
	PRINT ( _L("End CPcsContactStore::VPbkSingleContactOperationFailed") );
}

// ---------------------------------------------------------------------------
// Handles the operations for a single contact after it is fetched
// ---------------------------------------------------------------------------
void CPcsContactStore::HandleRetrievedContactL(MVPbkStoreContact* aContact)
{    
    // Fill the contact link
	MVPbkContactLink* tmpLink = aContact->CreateLinkLC();
		
    // If the link is null, then it is not put on the cleanup stack,
    // so we need not pop in such a case
    if( NULL == tmpLink )
    {
        delete aContact;
        aContact = NULL;
    	return;
    }
   
    // Recover the URI
    HBufC* storeUri = HBufC::NewL(aContact->ParentStore().StoreProperties().Uri().UriDes().Length());
    storeUri->Des().Copy(aContact->ParentStore().StoreProperties().Uri().UriDes());
        			
	CPsData *phoneContact = CPsData::NewL();

	
	// Fill the contact id
	CVPbkContactIdConverter* converter = NULL;
	TRAPD ( err, converter = CVPbkContactIdConverter::NewL( aContact->ParentStore() ) );
	
	if ( err == KErrNotSupported )
	{
		// simdb domain	
		PRINT ( _L("SIM domain data received") );
			
		TInt tempIndex =  iSimContactItems->Find(*tmpLink);	
		
		if( KErrNotFound == tempIndex)

		    {
		    	tempIndex = iSimContactItems->Count();
	        iSimContactItems->AppendL(tmpLink);
	      }
	    //Create a dummy sim index and set it
	    TInt simIndex = CreateCacheIDfromSimArrayIndex(tempIndex);
	    phoneContact->SetId(simIndex);
	    
	    // Set the contact link 
	    HBufC8* extnInfo = tmpLink->PackLC();
	    phoneContact->SetDataExtension(extnInfo);
	    CleanupStack::Pop();//extnInfo		      
	    CleanupStack::Pop(); // tmpLink
	}
	else 
	{
	    // cntdb domain
		TInt32 contactId = converter->LinkToIdentifier(*tmpLink);
		phoneContact->SetId(contactId);		
		CleanupStack::PopAndDestroy(); // tmpLink
	}
   	 
	// Take the ownership
	aContact->PushL();

	MVPbkContactGroup* myContactGroup= aContact->Group();
	
	// The retrieved contact can be a contact item or a contact group, Handle accordingly
	if ( NULL == myContactGroup )
	{
		// The fetched contact item (and not a contact group.)
		GetDataForSingleContactL( *aContact, phoneContact );
		//Add the data to the relevent cache through the observer
		iObserver->AddData(*storeUri, phoneContact);
	}
	else
	{  
	    // Fetch the group name 
	    HBufC* groupName = HBufC::NewL(myContactGroup->GroupLabel().Length());
	    groupName->Des().Copy(myContactGroup->GroupLabel());
	    TInt grpArrayIndex = -1; 
	    for(TInt i =0; i  <iFieldsToCache.Count(); i++)
	    {
	    	if(iFieldsToCache[i] == R_VPBK_FIELD_TYPE_LASTNAME)
         
			 {
			 grpArrayIndex = i;
			 	
			 }
			 phoneContact->SetDataL(i,KNullDesC);
	    }
	    if(grpArrayIndex != -1)
	    {
	    	phoneContact->SetDataL(grpArrayIndex,groupName->Des());
	    	storeUri->Des().Copy(KVPbkDefaultGrpDbURI);	    
	    
	
	    
	    
        // Check for the contact in the group.
	    MVPbkContactLinkArray* contactsContainedInGroup = myContactGroup->ItemsContainedLC();
        for(TInt i = 0; i < contactsContainedInGroup->Count(); i++)
		{
			TInt grpContactId = converter->LinkToIdentifier(contactsContainedInGroup->At(i));
			phoneContact->AddIntDataExtL(grpContactId);
		}
		CleanupStack::PopAndDestroy(); // contactsContainedInGroup
 
 		
 		//Add the data to the relevent cache through the observer
	    iObserver->AddData(*storeUri, phoneContact);
	 	}
	    else
	    {
	    	//We do not add anything here since Lastname does not exists in cenrep
	    	delete 	phoneContact;
	    	phoneContact = NULL;
	    }
	    delete groupName;
	    groupName = NULL;
	}
	
	delete converter;
	converter = NULL;

	delete storeUri;
    storeUri = NULL;
    
	CleanupStack::PopAndDestroy(aContact); // aContact
}
 
    
// ---------------------------------------------------------------------------
// Fetches the data from a particular contact 
// --------------------------------------------------------------------------- 
void CPcsContactStore::GetDataForSingleContactL( MVPbkBaseContact& aContact,
                                                 CPsData* aPhoneData )
{      

    for(TInt i =0; i < iFieldsToCache.Count(); i++)
    {
	    aPhoneData->SetDataL(i, KNullDesC);
		AddContactFieldsL( aContact, iFieldsToCache[i], aPhoneData);			
    }
	
}

// ---------------------------------------------------------------------------
// Add the data from contact fields
// --------------------------------------------------------------------------- 
void CPcsContactStore::AddContactFieldsL( MVPbkBaseContact& aContact,
                                          TInt afieldtype,
                                          CPsData *aPhoneData)
{
	const MVPbkFieldType*  myContactDataField = 
		iContactManager->FieldTypes().Find( afieldtype );

	CVPbkBaseContactFieldTypeIterator* itr = 
		CVPbkBaseContactFieldTypeIterator::NewLC( *myContactDataField, 
		aContact.Fields() );	
			
	// Iterate through each of the data fields
	while ( itr->HasNext() )
	{
		const MVPbkBaseContactField* field = itr->Next();

	    if ( ( field->FieldData()).DataType() == EVPbkFieldStorageTypeText )
		{
			const MVPbkContactFieldTextData& data = 
				MVPbkContactFieldTextData::Cast( field->FieldData() );
			
			//If the field exist in iFieldsToCache, then set it
			for( TInt i = 0 ; i< iFieldsToCache.Count(); i++)
			{
				if(afieldtype == iFieldsToCache[i])
				{	
                    // Check if the field has any data entry. If so
                    // concatenate the next data to the existing one.
                    // A unit seperator is used to show that these are
                    // two entries.
                    HBufC* previousData = aPhoneData->Data(i);
                    if( (previousData != NULL) && (previousData->Des().Length()> 1) )
                    {
                        HBufC* newData = HBufC::NewLC(data.Text().Length() + previousData->Des().Length() + 5);
                        TPtr newDataPtr(newData->Des());
                        newDataPtr.Append(previousData->Des());
                        newDataPtr.Append(KSpaceCharacter);
                        newDataPtr.Append(KUnitSeparator);
                        newDataPtr.Append(KSpaceCharacter);
                        newDataPtr.Append(data.Text());
                        aPhoneData->SetDataL(i,*newData);
                        CleanupStack::PopAndDestroy(); // newData
                    }
                    else
                    {
                        aPhoneData->SetDataL(i,data.Text());
                    }
				}			
			}					
		}
	}
	
	CleanupStack::PopAndDestroy( itr ); 
}

// ---------------------------------------------------------------------------
// Fetches the data from the vpbk using the contact links 
// --------------------------------------------------------------------------- 
void CPcsContactStore::FetchlinksL()
{		

	PRINT1 ( _L("CPcsContactStore::Total contacts downloaded = %d"),
	          iFetchedContactCount );
	          						
    TInt blockCount = iFetchedContactCount + KLinksToFetchInOneGo;
    
    if( blockCount >= iAllContactLinksCount)
    	blockCount = iAllContactLinksCount;
    
	for(int cnt = iFetchedContactCount; cnt < blockCount; cnt++)
	{	
		// Retrieve the contact 
		MVPbkContactLink* tempLink =iContactViewBase->CreateLinkLC(cnt);
		iContactManager->RetrieveContactL( *tempLink, *this );
		CleanupStack::PopAndDestroy();
	}				

}

// ---------------------------------------------------------------------------
// Implements the view ready function of MVPbkContactViewObserver
// --------------------------------------------------------------------------- 
void CPcsContactStore::ContactViewReady(
                MVPbkContactViewBase& aView ) 
{
    PRINT ( _L("Enter CPcsContactStore::ContactViewReady") );
    iFs.Close();
    aView.RemoveObserver(*this);
    // Get the total number of contacts for this view
    TRAPD(err,iAllContactLinksCount = aView.ContactCountL());
    
    if(err != KErrNone)
    {
    	PRINT( _L("CPcsContactStore::ContactViewReady - Unable to obtain contact count"));
    	iNextState = EComplete;
   		IssueRequest();
   		return;
    }
    
    PRINT1 ( _L("Total number of contacts for this view: %d"), iAllContactLinksCount);
    if( iAllContactLinksCount == 0)
    {
        // No Contactsb to cache, hence update the status accordingly
    	iObserver->UpdateCachingStatus(*iUri, ECachingComplete);
    	iNextState = EComplete;
   		IssueRequest();
   		return;
    }
    iFetchedContactCount = 0;
    
    // Change the iNextState to fetch the contacts 
    if( iContactViewReady == EFalse)
    {
    	iObserver->UpdateCachingStatus(*iUri, ECachingInProgress);
    	iContactViewReady = ETrue;
    	iNextState = EFetchContactBlock;
   		IssueRequest();
    }
	
	PRINT ( _L("End CPcsContactStore::ContactViewReady") );
}

// ---------------------------------------------------------------------------
// Implements the view unavailable function of MVPbkContactViewObserver
// --------------------------------------------------------------------------- 
void CPcsContactStore::ContactViewUnavailable(
                MVPbkContactViewBase& /*aView*/ )  
{
    PRINT ( _L("Enter CPcsContactStore::ContactViewUnavailable") );
    // Update the caching status to complete
	iObserver->UpdateCachingStatus(*iUri, ECachingComplete);
    iFs.Close();
	PRINT ( _L("End CPcsContactStore::ContactViewUnavailable") );
}

// ---------------------------------------------------------------------------
// Implements the add contact function of MVPbkContactViewObserver
// --------------------------------------------------------------------------- 
void CPcsContactStore::ContactAddedToView(
            MVPbkContactViewBase& /*aView*/, 
            TInt /*aIndex*/, 
            const MVPbkContactLink& /*aContactLink*/ ) 
{
}

// ---------------------------------------------------------------------------
// Implements the remove contact function of MVPbkContactViewObserver
// --------------------------------------------------------------------------- 
void CPcsContactStore::ContactRemovedFromView(
                MVPbkContactViewBase& /*aView*/, 
                TInt /*aIndex*/, 
                const MVPbkContactLink& /*aContactLink*/ )  
{
}

// ---------------------------------------------------------------------------
// Implements the view error function of MVPbkContactViewObserver
// --------------------------------------------------------------------------- 
void CPcsContactStore::ContactViewError(
            MVPbkContactViewBase& /*aView*/, 
            TInt /*aError*/, 
            TBool /*aErrorNotified*/ )  
{
}

// ---------------------------------------------------------------------------
// Creates the contact view to fetch data from this store
// --------------------------------------------------------------------------- 
void CPcsContactStore::CreateContactFetchViewL()
{
	PRINT ( _L("Enter CPcsContactStore::CreateContactFetchViewL") );
	
	// Create the view definition
    CVPbkContactViewDefinition* viewDef = CVPbkContactViewDefinition::NewL();
	CleanupStack::PushL( viewDef );

  // Create the View Name for the view
  // The views are named as PCSView_<uri>		
	HBufC* viewName = HBufC::NewL(KBufferMaxLen);
	viewName->Des().Append(KPcsViewPrefix);
	viewName->Des().Append(iUri->Des());
	CleanupStack::PushL(viewName);

	// Set the Uri
	if ( iUri->Des().CompareC(KVPbkDefaultGrpDbURI) == 0)
	{	    
		// Special Handling required for Groups Data Store
		// Read the resource file and create the sort order
		// The sort order created is used in view creation.
		
		User::LeaveIfError( iFs.Connect() );
		TPtrC driveLetter = TParsePtrC( RProcess().FileName() ).Drive();
		TBuf<KMaxFileName> resourceFileName;
		RResourceFile iResourceFile;
		
		//Read the path of the resource file
		resourceFileName.Copy( driveLetter );
		resourceFileName.Append( KDC_RESOURCE_FILES_DIR );
		resourceFileName.Append( KResourceFileName );
		
		//Open the resource file
	  	BaflUtils::NearestLanguageFile( iFs, resourceFileName );
	    iResourceFile.OpenL( iFs, resourceFileName );
	  	iResourceFile.ConfirmSignatureL(0);
	

        //Read resource	             
        HBufC8* textbuffer = iResourceFile.AllocReadL(R_SORTORDER_LASTNAME);
        CleanupStack::PushL(textbuffer);
        
        // Set the resource information into the reader
        TResourceReader reader;
        reader.SetBuffer(textbuffer);
	
	    // Create sort order with only last name
	    CVPbkSortOrder* sortOrder = CVPbkSortOrder::NewL( reader, 
	                                                      iContactManager->FieldTypes() );

	    CleanupStack::PushL(sortOrder);
		
		// Set the groups type
		viewDef->SetUriL(KVPbkDefaultCntDbURI);
		viewDef->SetType( EVPbkGroupsView );

		// Set name for the view
		viewDef->SetNameL( *viewName );	
		
		// Create the contact view
		iContactViewBase = iContactManager->CreateContactViewLC( 
		                         *this, 
		                         *viewDef, 
		                         *sortOrder);

        CleanupStack::Pop(); // iContactViewBase
        CleanupStack::PopAndDestroy(sortOrder);	
        CleanupStack::PopAndDestroy(textbuffer);
    
    	// Close the resouce File
        iResourceFile.Close();                        
	
	}
	else
	{
	
		// Create sort order with the fields from cenrep
		CreateSortOrderL(iContactManager->FieldTypes());
		
		CVPbkSortOrder* sortOrderPhone = CVPbkSortOrder::NewL(*iSortOrder);
	    CleanupStack::PushL(sortOrderPhone);
	    
		// set contacts type
		viewDef->SetUriL(iUri->Des());
		viewDef->SetType( EVPbkContactsView );
			
		// Set name for the view
		viewDef->SetNameL( *viewName );	
		__LATENCY_MARK ( _L("CPcsContactStore::CreateContactFetchViewL===== create view start") );
    	iContactViewBase = iContactManager->CreateContactViewLC( 
		                         *this, 
		                         *viewDef, 
		                         *sortOrderPhone );
		__LATENCY_MARKEND ( _L("CPcsContactStore::CreateContactFetchViewL ==== create view end") );	                         
        
        CleanupStack::Pop(); // iContactViewBase	
        CleanupStack::PopAndDestroy(sortOrderPhone);	
	}		 
		                             
    CleanupStack::PopAndDestroy( viewName );	
    CleanupStack::PopAndDestroy( viewDef );	
    
    PRINT ( _L("End CPcsContactStore::CreateContactFetchViewL") );
}

// ---------------------------------------------------------------------------------
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------------
void CPcsContactStore::DoCancel() 
{    
}

// ---------------------------------------------------------------------------------
// The function is called by the active scheduler 
// ---------------------------------------------------------------------------------
void CPcsContactStore::RunL() 
{
	TRequestStatus timerStatus;
	iTimer.Cancel();
	
    switch( iNextState)
    {
	   	case ECreateView :
			CreateContactFetchViewL();
		    break;
   					      	
	   	case EFetchContactBlock:
	   		PRINT ( _L("Issuing the fetch request for next block") );
			FetchlinksL();
			
			// Delay the next fetch since contact fetch is CPU intensive,
			// this will give other threads a chance to use CPU
			iTimer.After( timerStatus, 100000); // 100 milliseconds
			User::WaitForRequest( timerStatus );
			break;
	   						
	   	case EComplete:
		    PRINT ( _L("Contacts Caching FINISHED") );
		    PRINT_BOOT_PERFORMANCE ( _L("Contacts Caching FINISHED") );
		    break;
    }
}

// ---------------------------------------------------------------------------------
// Called in case of any errros 
// ---------------------------------------------------------------------------------
TInt CPcsContactStore::RunError(TInt /*aError*/) 
{
	 PRINT ( _L(" Enter CPcsContactStore:: CPcsContactStore::RunError()") );

	 PRINT1 ( _L(" CPcsContactStore:: RunError().  Completing caching in contacts store %S with status ECachingCompleteWithErrors "), &(iUri->Des()));
   iObserver->UpdateCachingStatus(*iUri, ECachingCompleteWithErrors);
 	 PRINT ( _L(" End CPcsContactStore:: CPcsContactStore::RunError()") );
	return KErrNone;
}

// ---------------------------------------------------------------------------------
// Read the fields to cache from the central repository
// ---------------------------------------------------------------------------------
void CPcsContactStore::ReadFieldsToCacheFromCenrepL()
{
	CRepository *repository = CRepository::NewL( KCRUidPSContacts );

    // Read the data fields from cenrep
    for (TInt i(KCenrepFieldsStartKey); i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; i++ )
    {
	TInt fieldToCache (-1);
	    TInt err = repository->Get(i, fieldToCache );
	    
	    if ( KErrNone != err )
	    {
		    break;
	    }
	    if ( fieldToCache != 0 )
	    {
		    iFieldsToCache.Append(fieldToCache);
	    }
	    		
    }
    
    delete repository;
    
}

// ---------------------------------------------------------------------------------
// Creates a dummy id for the cache
// ---------------------------------------------------------------------------------

TInt CPcsContactStore::CreateCacheIDfromSimArrayIndex(TInt aSimId)
{
	return (KSimStoreOffset + aSimId);
}

// ---------------------------------------------------------------------------------
// Issues request to active object to call RunL method
// ---------------------------------------------------------------------------------
void CPcsContactStore::IssueRequest()
{
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	SetActive();
}
// Creates a sort order depending on the fields specified in the cenrep
// when calling this function pass the masterList (i.e list containing all the
// vpbk fields)
// ---------------------------------------------------------------------------------
void CPcsContactStore::CreateSortOrderL(const MVPbkFieldTypeList& aMasterList)
{
		
	TInt count = aMasterList.FieldTypeCount();
	
	// loop through the master list and find the types that are supported
	for(TInt i = 0; i < count; i++)
	{
		const MVPbkFieldType& fieldType = aMasterList.FieldTypeAt(i);
		TInt resourceId = fieldType.FieldTypeResId();
		// if the field type is supported
		// then append MVPbkFieldType to sortOrder
		for(TInt j =0; j < iFieldsToCache.Count(); j++)
		{
			if(iFieldsToCache[j] == resourceId)
			{
				iSortOrder->AppendL(fieldType);
			}
		}
		
		if(iSortOrder->FieldTypeCount() == iFieldsToCache.Count())
		{
			break;
		}
	}
	
}

// End of file
