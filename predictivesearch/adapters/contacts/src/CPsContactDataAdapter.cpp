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
* Description:  Predictive Contact Search Algorithm 1 main class
*
*/


// USER INCLUDES
#include "CPsContactDataAdapter.h"
#include "CPcsDebug.h"
#include "cpcscontactfetch.h"
#include "CPsData.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPsContactDataAdapter::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPsContactDataAdapter* CPsContactDataAdapter::NewL(TAny* aPsDataPluginParameters)
{
    PRINT ( _L("Enter CPsContactDataAdapter::NewL") );
    
    // Get the PsData plugin parametrs
    TPsDataPluginParams* params = reinterpret_cast<TPsDataPluginParams*>( aPsDataPluginParameters );
	CPsContactDataAdapter* self = new ( ELeave ) CPsContactDataAdapter();
	CleanupStack::PushL( self );
	self->ConstructL(params->iDataStoreObserver, params->iStoreListObserver);
	CleanupStack::Pop( self );

    PRINT ( _L("End CPsContactDataAdapter::NewL") );
    
	return self;
}

// ----------------------------------------------------------------------------
// CPsContactDataAdapter::CPsContactDataAdapter
// Two Phase Construction
// ----------------------------------------------------------------------------
CPsContactDataAdapter::CPsContactDataAdapter()
{		
    PRINT ( _L("Enter CPsContactDataAdapter::CPsContactDataAdapter") );
    PRINT ( _L("End CPsContactDataAdapter::CPsContactDataAdapter") );
}

// ----------------------------------------------------------------------------
// CPsContactDataAdapter::ConstructL
// Two Phase Construction
// ----------------------------------------------------------------------------
void CPsContactDataAdapter::ConstructL(MDataStoreObserver* aObserverForDataStore,
									   MStoreListObserver* aStoreListObserver)
{
    PRINT ( _L("Enter CPsContactDataAdapter::ConstructL") );   
    
    iContactFetcher = CPcsContactFetch::NewL(); 
     
    iObserverForDataStore = aObserverForDataStore;
    iStoreListObserver = aStoreListObserver;
    
    iContactFetcher->SetObserver(*iObserverForDataStore);	
                
    PRINT ( _L("End CPsContactDataAdapter::ConstructL") );
} 
	
	
// ----------------------------------------------------------------------------
// CPsContactDataAdapter::~CPsContactDataAdapter
// Destructor
// ----------------------------------------------------------------------------
CPsContactDataAdapter::~CPsContactDataAdapter()
{
    PRINT ( _L("Enter CPsContactDataAdapter::~CPsContactDataAdapter") );
    
    delete iContactFetcher;
    
    PRINT ( _L("End CPsContactDataAdapter::~CPsContactDataAdapter") );
}
                                 
// ----------------------------------------------------------------------------
// CPsContactDataAdapter::RequestForDataL
// 
// ----------------------------------------------------------------------------
void  CPsContactDataAdapter::RequestForDataL( TDesC& aDataStoreURI )
{
	PRINT ( _L("Enter CPsContactDataAdapter::RequestForDataL") );

    iContactFetcher->RequestForDataL(aDataStoreURI);  	 	

	PRINT ( _L("End CPsContactDataAdapter::RequestForDataL") );		     
     
}

// ----------------------------------------------------------------------------
// CPsContactDataAdapter::GetSupportedDataStoresL
// 
// ----------------------------------------------------------------------------

void CPsContactDataAdapter::GetSupportedDataStoresL( RPointerArray<TDesC> &aDataStoresURIs )
{
	PRINT ( _L("Enter CPsContactDataAdapter::GetSupportedDataStoresL") );
	
	iContactFetcher->GetSupportedDataStoresL(aDataStoresURIs);

	PRINT ( _L("End CPsContactDataAdapter::GetSupportedDataStoresL") );
}


// ----------------------------------------------------------------------------
// CPsContactDataAdapter::IsDataStoresSupportedL
// 
// ----------------------------------------------------------------------------
TBool CPsContactDataAdapter::IsDataStoresSupportedL( TDesC& aDataStoreURI ) 
{
	return iContactFetcher->IsDataStoresSupportedL(aDataStoreURI);

}
// ----------------------------------------------------------------------------
// CPsContactDataAdapter::GetSupportedDataFieldsL
// 
// ----------------------------------------------------------------------------
void CPsContactDataAdapter::GetSupportedDataFieldsL(RArray<TInt> &aDataFields )
{
	PRINT ( _L("Enter CPsContactDataAdapter::GetSupportedDataFieldsL") );
	
	iContactFetcher->GetSupportedDataFieldsL(aDataFields );
	
	PRINT ( _L("End CPsContactDataAdapter::GetSupportedDataFieldsL") );
}

// End of file

