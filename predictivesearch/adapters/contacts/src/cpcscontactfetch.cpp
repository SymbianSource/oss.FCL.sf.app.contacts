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
* Description:  Active object to handle all data stores from vpbk
*                Handles add/ modify/ delete of contacts.
*
*/


// SYSTEM INCLUDES
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
#include <vpbkeng.rsg>
#include <VPbkContactStoreUris.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkContactViewBase.h>
#include <VPbkContactView.hrh>
#include <CVPbkContactIdConverter.h>
#include <centralrepository.h>
#include <MVPbkContactGroup.h>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactLinkArray.h>

// USER INCLUDES
#include "cpcscontactfetch.h"
#include "CPsData.h"
#include "CPcsDebug.h" 
#include "CPcsDefs.h"

// ============================== MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------------
// 1st phase constructor
// ---------------------------------------------------------------------------------
CPcsContactFetch* CPcsContactFetch::NewL()
{
	PRINT ( _L("Enter CPcsContactFetch::NewL") );

	CPcsContactFetch* self = new ( ELeave ) CPcsContactFetch( );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	
	PRINT ( _L("End CPcsContactFetch::NewL") );
	return self;
}

// ---------------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------------
CPcsContactFetch::CPcsContactFetch() : 
    CActive( CActive::EPriorityStandard ),
    iAtLeastOneStoreReady(EFalse),
    iRequestForData(EFalse),
    iSubStoreCreated(EFalse)
{
    PRINT ( _L("Enter CPcsContactFetch::CPcsContactFetch") );
	CActiveScheduler::Add( this );
	PRINT ( _L("End CPcsContactFetch::CPcsContactFetch") );
}


// ---------------------------------------------------------------------------------
// CContactsSearcher::ConstructL() 
// 2nd phase constructor
// ---------------------------------------------------------------------------------
void CPcsContactFetch::ConstructL()
{
    PRINT ( _L("Enter CPcsContactFetch::ConstructL") );
    
    //Read the URIs from the central repository
    ReadUrisFromCenrepL();
    
	// Initialize virtual phonebook with uris read from central repository.
	CVPbkContactStoreUriArray* uriArray = CVPbkContactStoreUriArray::NewLC();
	for(TInt i = 0; i< iUriFromCenrep.Count();i++)
	{
		uriArray->AppendL( TVPbkContactStoreUriPtr(*iUriFromCenrep[i]) );
	}

	// Create the instance of the contact manager
	iContactManager = CVPbkContactManager::NewL(*uriArray);
	CleanupStack::PopAndDestroy(uriArray); 
	 
	
	iWait = new( ELeave )CActiveSchedulerWait();
	 
	// Open the data stores associated with this contact manager 
	iContactManager->ContactStoresL().OpenAllL( *this );
	PRINT ( _L("End CPcsContactFetch::ConstructL") );
    
}

// ---------------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------------
CPcsContactFetch::~CPcsContactFetch() 
{
	PRINT ( _L("Enter CPcsContactFetch::~CPcsContactFetch") );
	
	//Delete the data stores
	iAllDataStores.ResetAndDestroy();
	
	//Delete the contact manager
	if ( iContactManager )
	{
	    TRAP_IGNORE(iContactManager->ContactStoresL().CloseAll( *this ));
		delete iContactManager;
		iContactManager = NULL;	
	}
	
	if ( iWait )
	{
    	if( iWait->IsStarted() ) 
    		iWait->AsyncStop();
		delete iWait;
		iWait = NULL;
    }
    	
    iUriFromCenrep.ResetAndDestroy();
    
    if(IsActive())
	{
		Deque();
	}

	PRINT ( _L("End CPcsContactFetch::~CPcsContactFetch") );
}

// ----------------------------------------------------------------------------
// CPcsContactFetch::SetObserver
// 
// ----------------------------------------------------------------------------
void CPcsContactFetch::SetObserver(MDataStoreObserver& aObserver)
{
	iObserver = &aObserver;
}

// ----------------------------------------------------------------------------
// CPcsContactFetch::GetSupportedDataStoresL
// 
// ----------------------------------------------------------------------------
void CPcsContactFetch::GetSupportedDataStoresL( RPointerArray<TDesC> &aDataStoresURIs )
{
	PRINT ( _L("Enter CPcsContactFetch::GetSupportedDataStoresL") );
	for(TInt i =0 ; i < iUriFromCenrep.Count();i++)
	{		
		aDataStoresURIs.Append(iUriFromCenrep[i]);		
	}
	PRINT ( _L("End CPcsContactFetch::GetSupportedDataStoresL") );

}

// ----------------------------------------------------------------------------
// CPcsContactFetch::RequestForDataL
// 
// ----------------------------------------------------------------------------
void  CPcsContactFetch::RequestForDataL(TDesC& aDataStoreURI)
{
	PRINT ( _L("Enter CPcsContactFetch::RequestForDataL") );

	// Create the substore for this uri
	CreateSubStoresL(aDataStoreURI);
	
	PRINT ( _L("End CPcsContactFetch::RequestForDataL") );

}

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreListObserver.
// ---------------------------------------------------------------------------
void CPcsContactFetch::OpenComplete()
{
	PRINT ( _L("Enter CPcsContactFetch::OpenComplete") );
	
    TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	SetActive();
	
	PRINT ( _L("End CPcsContactFetch::OpenComplete") );
}
    
// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// ---------------------------------------------------------------------------
void CPcsContactFetch::StoreReady(MVPbkContactStore& /*aStore*/)
{
	PRINT ( _L("Enter CPcsContactFetch::StoreReady") );
	iAtLeastOneStoreReady = ETrue;
	if( iWait->IsStarted() )
		iWait->AsyncStop();
	
	PRINT ( _L("End CPcsContactFetch::StoreReady") );
}

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// ---------------------------------------------------------------------------
void CPcsContactFetch::StoreUnavailable(
        MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/)
{
    PRINT ( _L("Enter CPcsContactFetch::StoreUnavailable") );
    //Do nothing
    PRINT ( _L("End CPcsContactFetch::StoreUnavailable") );
}

// ---------------------------------------------------------------------------
// From class MVPbkContactStoreObserver.
// ---------------------------------------------------------------------------
void CPcsContactFetch::HandleStoreEventL(MVPbkContactStore& aContactStore, 
                TVPbkContactStoreEvent aStoreEvent)
{

    for ( TInt i=0; i<iAllDataStores.Count(); i++)
    {
        // Check if the store event is related to groups, 
        // if so, then call HandleStoreEventL() for the Groups data store
        if((aStoreEvent.iEventType == TVPbkContactStoreEvent::EGroupAdded)||
           (aStoreEvent.iEventType == TVPbkContactStoreEvent::EGroupDeleted)||
           (aStoreEvent.iEventType == TVPbkContactStoreEvent::EGroupChanged) )
        {
            if(iAllDataStores[i]->GetStoreUri().Compare(KVPbkDefaultGrpDbURI) == 0 )
            {
            	iAllDataStores[i]->HandleStoreEventL(aContactStore, aStoreEvent);
				break;
            }
        }
        else if(iAllDataStores[i]->GetStoreUri().Compare(aContactStore.StoreProperties().Uri().UriDes()) == 0 )
        {
        	iAllDataStores[i]->HandleStoreEventL(aContactStore, aStoreEvent);
			break;
        }
    }
	
}

// ---------------------------------------------------------------------------------
// Implements cancellation of an outstanding request.
// ---------------------------------------------------------------------------------
void CPcsContactFetch::DoCancel() 
{
    // Do nothing
}


// ---------------------------------------------------------------------------------
// The function is called by the active scheduler 
// ---------------------------------------------------------------------------------
void CPcsContactFetch::RunL() 
{
	
	if (!iAtLeastOneStoreReady) 
	{
		if( iWait->IsStarted() ) 
			iWait->AsyncStop();
	}
}

// ---------------------------------------------------------------------------------
// Called in case of any errros 
// ---------------------------------------------------------------------------------
TInt CPcsContactFetch::RunError(TInt aError) 
{
	return aError;
}

// ---------------------------------------------------------------------------
// Reads the Uris from the cenrep
// --------------------------------------------------------------------------- 

void CPcsContactFetch::ReadUrisFromCenrepL()
{
    
    CRepository* repository = CRepository::NewL( KCRUidPSContacts );
    CleanupStack::PushL( repository );
    
    // Read the cenrep for URIs	
    TBuf<KCRMaxLen> uriStr;
    for (TInt i(KUriCenRepStartKey); i < KUriCenRepStartKey + KCenrepUriSupportedCount; i++ )
    {
        TInt err = repository->Get( i, uriStr );
        if (KErrNone != err)
        {
            break;
        }
        
        if (uriStr != KNullDesC)
        {
            HBufC* uri = uriStr.AllocLC();
            iUriFromCenrep.AppendL(uri);
            CleanupStack::Pop(uri);
        }
    
    }
    
    CleanupStack::PopAndDestroy( repository );
}


// ---------------------------------------------------------------------------
// Creates the contact view 
// --------------------------------------------------------------------------- 
void CPcsContactFetch::CreateSubStoresL(TDesC& aDataStoreURI)
{
	PRINT ( _L("Enter CPcsContactFetch::CreateSubStoresL") );
	
	if (!iAtLeastOneStoreReady) 
	{
		if( ! (iWait->IsStarted()) ) 
	    {
	    	iWait->Start();
	    }
	}
	
	// Check if data store is already present
	for ( TInt i = 0; i < iAllDataStores.Count(); i++ )
	{
	    CPcsContactStore* contactStore = iAllDataStores[i];
		if ( contactStore->GetStoreUri().Compare(aDataStoreURI) == 0 )
		{
		    // Remove the store
		    delete contactStore;
			iAllDataStores.Remove(i);
			break;
		}
	}
	
	MVPbkContactStoreList& stores = iContactManager->ContactStoresL();
    for ( TInt i(0); i < stores.Count(); ++i )
	{       
        if(stores.At(i).StoreProperties().Uri().UriDes().Compare(aDataStoreURI) == 0)
        {
        	CPcsContactStore* contactStore = CPcsContactStore::NewL(*iContactManager,
		                                                        *iObserver,
		                                                        stores.At(i).StoreProperties().Uri().UriDes());
			iAllDataStores.Append(contactStore);
			break;
        }
	}
	PRINT ( _L("End CPcsContactFetch::CreateSubStoresL") );
	
}

// ----------------------------------------------------------------------------
// CPcsContactFetch::GetSupportedDataFieldsL
// 
// ----------------------------------------------------------------------------
void CPcsContactFetch::GetSupportedDataFieldsL(RArray<TInt> &aDataFields )
{
	PRINT ( _L("Enter CPcsContactFetch::GetSupportedDataFieldsL") );
	CRepository *repository = CRepository::NewL( KCRUidPSContacts );

    // Read the data fields from cenrep
    for (TInt i(KCenrepFieldsStartKey); i < KCenrepFieldsStartKey + KCenrepNumberOfFieldsCount; i++ )
    {
    	TInt fieldToCache(-1);
    	
	    TInt err = repository->Get(i, fieldToCache );
	    if ( KErrNone != err )
	    {
		    break;
	    }
	    if ( fieldToCache != 0 )
	    {
		   aDataFields.Append(fieldToCache);
	    }
	
    }
    
    delete repository; 
   	PRINT ( _L("End CPcsContactFetch::GetSupportedDataFieldsL") );
}

// ----------------------------------------------------------------------------
// CPcsContactFetch::IsDataStoresSupportedL
// 
// ----------------------------------------------------------------------------
TBool CPcsContactFetch::IsDataStoresSupportedL( TDesC& aDataStoreURI ) 
{
	
	for ( TInt i = 0 ; i < iUriFromCenrep.Count() ; i++ )
	{
		if ( iUriFromCenrep[i]->Compare(aDataStoreURI) == 0 )
		    return ETrue;
	}
	
	return EFalse;
}

// End of file
