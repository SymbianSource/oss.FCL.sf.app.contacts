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
* Description:  ECom Predictive search interface definition
*
*/


// INCLUDE FILES
#include <ecom/ECom.h>
#include <mdatastoreobserver.h>
#include <mstorelistobserver.h>

// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CPsDataPlugin::NewL
// Two phase construction
// ----------------------------------------------------------------------------
inline CPsDataPlugin* CPsDataPlugin::NewL( TUid aImpUid, MDataStoreObserver* aObserverForDataStore,
																					MStoreListObserver* aStoreListObserver)
{
  CPsDataPlugin* self = CPsDataPlugin::NewLC( aImpUid, aObserverForDataStore, aStoreListObserver );	
	CleanupStack::Pop();
	return self;
}

// ----------------------------------------------------------------------------
// CPsDataPlugin::NewLC
// Two phase construction
// ----------------------------------------------------------------------------
inline CPsDataPlugin* CPsDataPlugin::NewLC( TUid aImpUid, MDataStoreObserver* aObserverForDataStore,
																						MStoreListObserver* aStoreListObserver)
{
       
    // Construct the strucutre with the observers
    TPsDataPluginParams params(aObserverForDataStore, aStoreListObserver);
    
	  TAny* implementation = REComSession::CreateImplementationL ( aImpUid,
	    _FOFF ( CPsDataPlugin, iDtor_ID_Key ), &params );
	  CPsDataPlugin* self = REINTERPRET_CAST( CPsDataPlugin*, implementation );
	  self->iPluginId = aImpUid;
	  CleanupStack::PushL( self );
	
	  return self;
}

// ----------------------------------------------------------------------------
// CPsDataPlugin::ConstructL
// Destructor
// ----------------------------------------------------------------------------
inline CPsDataPlugin::~CPsDataPlugin()
{
    REComSession::DestroyedImplementation( iDtor_ID_Key );
}

// ----------------------------------------------------------------------------
// CPsDataPlugin::PluginId
// Returns the plugin id
// ----------------------------------------------------------------------------
inline TUid CPsDataPlugin::PluginId() const
{  
    return iPluginId;
}
 
