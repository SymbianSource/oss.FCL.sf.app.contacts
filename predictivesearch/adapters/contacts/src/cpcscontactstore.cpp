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
#include <featmgr.h>
#include <CPbk2SortOrderManager.h>

// USER INCLUDES
#include "cpcscontactstore.h"
#include "CPsData.h"
#include "CPcsDebug.h" 
#include "CPcsDefs.h"
#include <contactsort.rsg>

// CONSTANTS
namespace {
const TInt KSimStoreOffset            = -5000;
const TInt KTimerInterval             = 100000;   // 100 milliseconds

_LIT(KResourceFileName, "contactsort.rsc");
_LIT(KPcsViewPrefix,"PCSView_");
}
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
    CActive( CActive::EPriorityLow ),
    iInitialContactCount( 0 ),
    iFetchBlockLowerNumber( 0 ),
    iFetchBlockUpperNumber( 0 ),
    iVPbkCallbackCount( 0 ),
    iContactViewReady( EFalse )
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
    
	iContactManager = &aContactManager;
	iObserver = &aObserver;
	
	iUri = aUri.AllocL();
	
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
	IssueRequest( 0 );
	
    FeatureManager::InitializeLibL();
    if( FeatureManager::FeatureSupported( KFeatureIdffContactsMycard ) )
        {
        iMyCardSupported = ETrue;
        }
    FeatureManager::UnInitializeLib();    

	PRINT ( _L("End CPcsContactStore::ConstructL") );
}

// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------
CPcsContactStore::~CPcsContactStore() 
{
	PRINT ( _L("Enter CPcsContactStore::~CPcsContactStore") );
	
	delete iContactViewBase;
	delete iSortOrder;
    delete iSimContactItems;   
	delete iWait;
	delete iUri;
	
	iTimer.Cancel();
    iTimer.Close();
	iFieldsToCache.Close();	
	
	iFs.Close();

	iInitialContactLinks.ResetAndDestroy();
    
	if(IsActive())
	{
		Deque();
	}
	
	delete iSortOrderMan;
	
	PRINT ( _L("End CPcsContactStore::~CPcsContactStore") );
}

// ---------------------------------------------------------------------------------
// Handles addition/deletion/modification of contacts
// ---------------------------------------------------------------------------------
void CPcsContactStore::HandleStoreEventL(MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent)
{
   	PRINT ( _L("Enter CPcsContactStore::HandleStoreEventL") );
   	
    PRINT2 ( _L("CPcsContactStore::HandleStoreEventL: URI=%S, event TVPbkContactStoreEventType::%d received"),
             &*iUri, aStoreEvent.iEventType);
   	        
    switch (aStoreEvent.iEventType) 
    {
		case TVPbkContactStoreEvent::EContactAdded:
        case TVPbkContactStoreEvent::EGroupAdded:
		{
		    // Observer will be notified once the cache update is complete
		    iOngoingCacheUpdate = ECacheUpdateContactAdded;
		                    
		    iContactManager->RetrieveContactL( *(aStoreEvent.iContactLink), *this );
			break;
		}

        case TVPbkContactStoreEvent::EUnknownChanges:
        {
            iObserver->RemoveAll( *iUri );      
            // To indicate that contact view is not ready when unknown changes happens 
            iContactViewReady = EFalse;
            iContactViewBase->AddObserverL(*this);
            break;
        }

		case TVPbkContactStoreEvent::EContactDeleted:
        case TVPbkContactStoreEvent::EGroupDeleted:
		case TVPbkContactStoreEvent::EContactChanged:
		case TVPbkContactStoreEvent::EGroupChanged:
		{
			CVPbkContactIdConverter* converter = NULL;
			
		    TRAPD ( err, converter = CVPbkContactIdConverter::NewL( aContactStore ) );

	        TInt32 contactId(-1);
	        
	        if ( err == KErrNotSupported )
	        {
	            // sim domain  
	            // Pass the sim observer string in the next line
	            TInt contactLocation = iSimContactItems->Find(*aStoreEvent.iContactLink);
				if( KErrNotFound != contactLocation)
				{
				    // We are not removing from the array cache. If you try to fetch, 
				    // then it will give an error
				    TInt index = CreateCacheIDfromSimArrayIndex( contactLocation );
				    
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
	            // Observer will be notified once the cache update is complete
	            iOngoingCacheUpdate = ECacheUpdateContactModified;
	            
	            // Add the contact
	            iContactManager->RetrieveContactL( *(aStoreEvent.iContactLink), *this );
	        }
	        else
	        {
	            // Inform observer immediately about contact removal
                iObserver->UpdateCachingStatus( *iUri, ECacheUpdateContactRemoved );
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
//  Check next state of state machine for contacts retrieval and perform
//  transition
// --------------------------------------------------------------------------- 
void CPcsContactStore::CheckNextState()
{
    PRINT5 ( _L("CPcsContactStore::CheckNextState(state %d) %d:(%d..%d)/%d"),
             iNextState, iVPbkCallbackCount,
             iFetchBlockLowerNumber, iFetchBlockUpperNumber, iInitialContactCount );

    if ( iNextState == EComplete )
    {
        return;
    }

    // If during the caching of the contacts initially in the view we get some
    // contact events for other contact operations from the user, then it can
    // happen at the end of the caching process that iCallbackCount > iAllContactLinksCount.
    // We assume that it can never be that at the end of the caching it results
    // iCallbackCount < iAllContactLinksCount.

    // Check if caching is complete
    if( iFetchBlockUpperNumber == iInitialContactCount       // All contacts initially in the view were fetched
        && iVPbkCallbackCount >= iInitialContactCount )        // We got at least the same amount of callbacks
    {
        iObserver->UpdateCachingStatus(*iUri, ECachingComplete);
        iNextState = EComplete;
        IssueRequest( 0 );
    }
    // We could have a number of callbacks that is the same of the fetched contacts
    // from the view even before we get the all the callbacks for the fetched contacts.
    // This can happen for instance if a contact is added by the user.
    // With the following condition it will happen just that the fetching of more
    // contacts from the initial view is happening before all the callbacks for
    // those are received.
    else if ( iVPbkCallbackCount >= iFetchBlockUpperNumber ) // Fetch next block
    {
        iObserver->UpdateCachingStatus(*iUri, ECachingInProgress);
        iNextState = EFetchContactBlock;
        // Delay the next fetch since contact fetch is CPU intensive,
        // this will give other threads a chance to use CPU
        IssueRequest( KTimerInterval ); // 100 milliseconds
    }
    else
    {
    // Otherwise, just pass through.
    }

}

// ---------------------------------------------------------------------------
//  Callback Method. Called when one Retrieve operation is complete
// --------------------------------------------------------------------------- 
void CPcsContactStore::VPbkSingleContactOperationComplete(
        MVPbkContactOperationBase& aOperation, MVPbkStoreContact* aContact)
{
    PRINT ( _L("Enter CPcsContactStore::VPbkSingleContactOperationComplete") );

    // We get this incremented even during add/del of contacts during caching
    iVPbkCallbackCount++;
    
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

	CheckNextState();

    PRINT ( _L("End CPcsContactStore::VPbkSingleContactOperationComplete") );
}
    
// ---------------------------------------------------------------------------
//  Callback Method. Called when one Retrieve operation fails.
// ---------------------------------------------------------------------------
void CPcsContactStore::VPbkSingleContactOperationFailed(
        MVPbkContactOperationBase& /*aOperation*/, TInt /*aError*/ )
{
  	PRINT ( _L("Enter CPcsContactStore::VPbkSingleContactOperationFailed") );

  	iVPbkCallbackCount++;

	CheckNextState();

	PRINT ( _L("End CPcsContactStore::VPbkSingleContactOperationFailed") );
}

// ---------------------------------------------------------------------------
// Handles the operations for a single contact after it is fetched
// ---------------------------------------------------------------------------
void CPcsContactStore::HandleRetrievedContactL(MVPbkStoreContact* aContact)
{    
    // Take the ownership
    aContact->PushL();

    if ( iMyCardSupported && IsMyCard( *aContact ) )
        {
        CleanupStack::PopAndDestroy( aContact );
        return;
        }
    
    // Fill the contact link
	MVPbkContactLink* tmpLink = aContact->CreateLinkLC();
	
    // If the link is null, then it is not put on the cleanup stack,
    // so we need not pop in such a case
    if ( NULL == tmpLink )
    {
        CleanupStack::PopAndDestroy( aContact );
        return;
    }

	CPsData* phoneContact = CPsData::NewL();

	// Fill the contact id
	CVPbkContactIdConverter* converter = NULL;
	TRAPD ( err, converter = CVPbkContactIdConverter::NewL( aContact->ParentStore() ) );
	
	if ( err == KErrNotSupported )
	{
		// simdb domain
		PRINT ( _L("CPcsContactStore::HandleRetrievedContactL: SIM domain data received") );
		
        // Set the contact link 
        HBufC8* extnInfo = tmpLink->PackLC();
        phoneContact->SetDataExtension(extnInfo);
        CleanupStack::Pop( extnInfo );

        // Get the index of the SIM contact
        TInt tempIndex =  iSimContactItems->Find(*tmpLink);	
		
		if ( KErrNotFound == tempIndex)
		    {
		    tempIndex = iSimContactItems->Count();
	        iSimContactItems->AppendL(tmpLink);
	        CleanupStack::Pop(); // tmpLink
	        }
		else
		    {
            CleanupStack::PopAndDestroy(); // tmpLink
		    }
		
	    //Create a dummy sim index and set it
	    TInt simIndex = CreateCacheIDfromSimArrayIndex(tempIndex);
	    phoneContact->SetId(simIndex);
	}
	else 
	{
	    // cntdb domain
		TInt32 contactId = converter->LinkToIdentifier(*tmpLink);
		phoneContact->SetId(contactId);
		CleanupStack::PopAndDestroy(); // tmpLink
	}
   	 
	MVPbkContactGroup* myContactGroup= aContact->Group();
	
	// The retrieved contact can be a contact item or a contact group, Handle accordingly
	if ( NULL == myContactGroup )
	{
		// The fetched contact item (and not a contact group.)
		GetDataForSingleContactL( *aContact, phoneContact );
	    // Recover the URI
	    HBufC* storeUri = aContact->ParentStore().StoreProperties().Uri().UriDes().AllocL();
		//Add the data to the relevent cache through the observer
		iObserver->AddData(*storeUri, phoneContact);
		delete storeUri;
	}
	else
	{  
	    // Fetch the group name 
	    HBufC* groupName = myContactGroup->GroupLabel().AllocLC();
	    TInt grpArrayIndex = -1; 
	    for (TInt i =0; i  <iFieldsToCache.Count(); i++)
	    {
	    	if (iFieldsToCache[i] == R_VPBK_FIELD_TYPE_LASTNAME)
			{
			    grpArrayIndex = i;
			}
			phoneContact->SetDataL(i, KNullDesC);
	    }
	    if (grpArrayIndex != -1)
	    {
	    	phoneContact->SetDataL(grpArrayIndex, *groupName);
	    
            // Check for the contact in the group.
            MVPbkContactLinkArray* contactsContainedInGroup = myContactGroup->ItemsContainedLC();
            for (TInt i = 0; i < contactsContainedInGroup->Count(); i++)
            {
                TInt grpContactId = converter->LinkToIdentifier(contactsContainedInGroup->At(i));
                phoneContact->AddIntDataExtL(grpContactId);
            }
            CleanupStack::PopAndDestroy(); // contactsContainedInGroup
            
            // Recover the URI
            HBufC* storeUri = KVPbkDefaultGrpDbURI().AllocL();
            
            //Add the data to the relevent cache through the observer
            iObserver->AddData(*storeUri, phoneContact);
            
            delete storeUri;
	 	}
	    else
	    {
	    	//We do not add anything here since Lastname does not exists in cenrep
	    	delete phoneContact;
	    	phoneContact = NULL;
	    }
	    CleanupStack::PopAndDestroy( groupName );
	}
	
	delete converter;
	converter = NULL;

	CleanupStack::PopAndDestroy(aContact);
	
	// Inform observer if this contact addition was a result of contact store event.
	if ( iOngoingCacheUpdate == ECacheUpdateContactModified ||
	     iOngoingCacheUpdate == ECacheUpdateContactAdded )
	    {
        iObserver->UpdateCachingStatus( *iUri, iOngoingCacheUpdate );
        iOngoingCacheUpdate = ECachingComplete;
	    }
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
                                          TInt aFieldType,
                                          CPsData *aPhoneData)
{
	const MVPbkFieldType* myContactDataField = 
		iContactManager->FieldTypes().Find( aFieldType );

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
			for ( TInt i = 0 ; i< iFieldsToCache.Count(); i++ )
			{
				if (aFieldType == iFieldsToCache[i])
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
                        CleanupStack::PopAndDestroy(newData);
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
// Fetches all the initial contact links from vpbk 
// --------------------------------------------------------------------------- 
void CPcsContactStore::FetchAllInitialContactLinksL()
{
    PRINT2 ( _L("CPcsContactStore::FetchAllInitialContactLinksL: URI=%S. Fetching %d contact links"),
             &*iUri, iInitialContactCount );

    __LATENCY_MARK ( _L("CPcsContactStore::FetchAllInitialContactLinksL ==== fetch contact links start") );
    
    for(TInt cnt = 0; cnt < iInitialContactCount; cnt++)
    {
        // Get the contact link
        MVPbkContactLink* tempLink = iContactViewBase->CreateLinkLC(cnt);
        iInitialContactLinks.AppendL( tempLink );
        CleanupStack::Pop();
    }

    __LATENCY_MARKEND ( _L("CPcsContactStore::FetchAllInitialContactLinksL ==== fetch contact links end") );                          

    // No callback to wait for next state
    iObserver->UpdateCachingStatus(*iUri, ECachingInProgress);
    iNextState = EFetchContactBlock;
    IssueRequest( 0 );
}

// ---------------------------------------------------------------------------
// Fetches the data from the vpbk using the contact links 
// --------------------------------------------------------------------------- 
void CPcsContactStore::FetchContactsBlockL()
{
    iFetchBlockLowerNumber = iFetchBlockUpperNumber;
    iFetchBlockUpperNumber += KLinksToFetchInOneGo;
    if ( iFetchBlockUpperNumber > iInitialContactCount )
    {
        iFetchBlockUpperNumber = iInitialContactCount;
    }
    
    PRINT2 ( _L("CPcsContactStore::FetchContactsBlockL: Fetched %d contacts, fetching more until %d"),
             iFetchBlockLowerNumber, iFetchBlockUpperNumber );

    for(TInt cnt = iFetchBlockLowerNumber; cnt < iFetchBlockUpperNumber; cnt++)
    {
        // Retrieve the contact
        iContactManager->RetrieveContactL( *iInitialContactLinks[cnt], *this );
    }

    // Destroy the array of temporary contact links when all
    // contacts from the initial view are retrieved
    if (iFetchBlockUpperNumber == iInitialContactCount)
    {
        iInitialContactLinks.ResetAndDestroy();
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
    TRAPD(err, iInitialContactCount = aView.ContactCountL());
    
    if(err != KErrNone)
    {
    	PRINT( _L("CPcsContactStore::ContactViewReady: Unable to obtain Contacts count"));
        iObserver->UpdateCachingStatus(*iUri, ECachingCompleteWithErrors);
    	iNextState = EComplete;
   		IssueRequest( 0 );
   		return;
    }
    
    PRINT1 ( _L("CPcsContactStore::ContactViewReady: Total number of contacts for this view: %d"),
            iInitialContactCount );
    if(iInitialContactCount == 0)
    {
        // No Contacts to cache, hence update the status accordingly
    	iObserver->UpdateCachingStatus(*iUri, ECachingComplete);
    	iNextState = EComplete;
   		IssueRequest( 0 );
   		return;
    }
    iFetchBlockLowerNumber = 0;
    iFetchBlockUpperNumber = 0;
    iVPbkCallbackCount = 0;
    
    // Change the iNextState to fetch the contacts 
    if(iContactViewReady == EFalse)
    {
    	iObserver->UpdateCachingStatus(*iUri, ECachingInProgress);
    	iContactViewReady = ETrue;
    	iNextState = EFetchAllLinks;
   		IssueRequest( 0 );
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
    PRINT1 ( _L("CPcsContactStore::ContactViewUnavailable: URI=%S"),
             &*iUri );

    // Update the caching status to cachingComplete, the client of PCS--Cmail can
    // perform searching even when contactview of somestore is unavailable.
    // For Contacts and Group view we get one of this event brfore ContactViewReady
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
         TInt aError, 
         TBool /*aErrorNotified*/ )  
{
    PRINT2 ( _L("CPcsContactStore::ContactViewError: URI=%S, Error=%d"),
             &*iUri, aError );
}

// ---------------------------------------------------------------------------
// Creates the contact view to fetch data from this store
// --------------------------------------------------------------------------- 
void CPcsContactStore::CreateContactFetchViewL()
{
	PRINT ( _L("Enter CPcsContactStore::CreateContactFetchViewL") );
    PRINT1 ( _L("CPcsContactStore::CreateContactFetchViewL: URI=%S"),
             &*iUri );
	
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
	if ( iUri->CompareC(KVPbkDefaultGrpDbURI) == 0)
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
	else if ( iUri->CompareC(KVPbkDefaultCntDbURI) == 0)
    {       
        // For phone contacts DB we use the shared "AllContacts" -view so that 
        // we don't need to generate a new view. This way we save some RAM
        // from contacts server and some CPU time because the view is already
        // generated and sorted. Difference is noticiable with larger amount 
        // of contacts e.g. 7000. 
	
        viewDef->SetUriL( iUri->Des() );
        viewDef->SetType( EVPbkContactsView );

        if( !iSortOrderMan )
            {
            iSortOrderMan = CPbk2SortOrderManager::NewL( 
                iContactManager->FieldTypes(), &iContactManager->FsSession() );
            }

        iContactViewBase = iContactManager->CreateContactViewLC( 
            *this, *viewDef, iSortOrderMan->SortOrder() );
        CleanupStack::Pop(); // iContactViewBase       
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
		__LATENCY_MARK ( _L("CPcsContactStore::CreateContactFetchViewL ==== create view start") );
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
        case ECreateView:
            PRINT1 ( _L("CPcsContactStore::RunL(ECreateView): URI=%S. Create contact view to fetch data from store"),
                     &*iUri );
            CreateContactFetchViewL();
            break;

        case EFetchAllLinks:
            PRINT1 ( _L("CPcsContactStore::RunL(EFetchAllLinks): URI=%S. Get all contact links in initial view"),
                     &*iUri );
            FetchAllInitialContactLinksL();
            break;
   		
	   	case EFetchContactBlock:
            PRINT4 ( _L("CPcsContactStore::RunL(EFetchContactBlock): URI=%S. Issuing fetch request for next block of max %d contacts (%d/%d fetched)"),
                     &*iUri, KLinksToFetchInOneGo, iFetchBlockUpperNumber, iInitialContactCount);
            FetchContactsBlockL();
			break;
	   	
	   	case EComplete:
		    PRINT3 ( _L("CPcsContactStore::RunL(EComplete): URI=%S. Contacts Caching FINISHED, (%d/%d contacts fetched)"),
		             &*iUri, iFetchBlockUpperNumber, iInitialContactCount );
		    PRINT1_BOOT_PERFORMANCE ( _L("CPcsContactStore::RunL(EComplete): URI=%S. Contacts Caching FINISHED"),
		                              &*iUri );
		    break;
    }
}

// ---------------------------------------------------------------------------------
// Called in case of any errros 
// ---------------------------------------------------------------------------------
TInt CPcsContactStore::RunError(TInt /*aError*/) 
{
	PRINT ( _L("Enter CPcsContactStore::RunError") );

	PRINT1 ( _L("CPcsContactStore::RunError(): URI=%S. Completing caching with status ECachingCompleteWithErrors"),
	         &*iUri );
    iObserver->UpdateCachingStatus(*iUri, ECachingCompleteWithErrors);

    PRINT ( _L("End CPcsContactStore::RunError") );
	return KErrNone;
}

// ---------------------------------------------------------------------------------
// Read the fields to cache from the central repository
// ---------------------------------------------------------------------------------
void CPcsContactStore::ReadFieldsToCacheFromCenrepL()
{
    CRepository* repository = CRepository::NewL( KCRUidPSContacts );

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
void CPcsContactStore::IssueRequest( TInt aTimeDelay )
{
    if( IsActive() ) // cannot activate allready active object
        return;

    if ( 0 == aTimeDelay )
    {
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
    }
    else
    {
        PRINT1 ( _L("CPcsContactStore::IssueRequest: Applying delay of %d milliseconds"),
                 aTimeDelay );

        iTimer.After( iStatus, aTimeDelay);
    }

    SetActive();
}

// ---------------------------------------------------------------------------------
// Creates a sort order depending on the fields specified 
// in the cenrep when calling this function pass the 
// masterList (i.e list containing all the vpbk fields)
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

// ---------------------------------------------------------------------------------
// Checks MyCard extension of contact
// ---------------------------------------------------------------------------------
TBool CPcsContactStore::IsMyCard( const MVPbkBaseContact& aContact )
    {
    TBool isMyCard( EFalse);
    // this is temporary solution to hide own contact from phonebook contacts list,
    // TODO remove this code when we can hide own contact with contact model

    MVPbkBaseContact& contact = const_cast<MVPbkBaseContact&>( aContact );
    TAny* extension = contact.BaseContactExtension( KVPbkBaseContactExtension2Uid );

    if ( extension )
        {
        MVPbkBaseContact2* baseContactExtension = static_cast<MVPbkBaseContact2*>( extension );
        TInt error( KErrNone );
        isMyCard = baseContactExtension->IsOwnContact( error );
        }
    
    return isMyCard;
    }
// End of file
