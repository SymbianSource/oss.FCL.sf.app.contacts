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
* Description:  
*
*/


// INCLUDE FILES
#include "CPsDataPluginInterface.h"
#include "CPsDataPlugin.h"
#include "CPcsDebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPsDataPluginInterface* CPsDataPluginInterface::NewL(MDataStoreObserver* aObserverForDataStore,
													MStoreListObserver* aStoreListObserver)
{
    PRINT ( _L("Enter CPsDataPluginInterface::NewL") );
    
    CPsDataPluginInterface* self = CPsDataPluginInterface::NewLC(aObserverForDataStore, aStoreListObserver);
    CleanupStack::Pop( self );
    
    PRINT ( _L("End CPsDataPluginInterface::NewL") );
    
    return self;
}

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::NewLC
// Two Phase Construction
// ---------------------------------------------------------------------------- 
CPsDataPluginInterface* CPsDataPluginInterface::NewLC(MDataStoreObserver* aObserverForDataStore,
													MStoreListObserver* aStoreListObserver)
{
    PRINT ( _L("Enter CPsDataPluginInterface::NewLC") );
    
    CPsDataPluginInterface* self = new( ELeave ) CPsDataPluginInterface();
    CleanupStack::PushL( self );
    self->ConstructL(aObserverForDataStore, aStoreListObserver);
    
    PRINT ( _L("End CPsDataPluginInterface::NewLC") );
    
    return self;
}

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::CPsDataPluginInterface
// Constructor
// ----------------------------------------------------------------------------    
CPsDataPluginInterface::CPsDataPluginInterface()
{
    PRINT ( _L("Enter CPsDataPluginInterface::CPsDataPluginInterface") );
    PRINT ( _L("End CPsDataPluginInterface::CPsDataPluginInterface") );
}

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------------------------- 
void CPsDataPluginInterface::ConstructL(MDataStoreObserver* aObserverForDataStore,
										MStoreListObserver* aStoreListObserver)
{
    PRINT ( _L("Enter CPsDataPluginInterface::ConstructL") );
    iObserverForDataStore = aObserverForDataStore;
    iStoreListObserver = aStoreListObserver;
    PRINT ( _L("End CPsDataPluginInterface::ConstructL") );
}

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::~CPsDataPluginInterface
// Constructor
// ---------------------------------------------------------------------------- 
CPsDataPluginInterface::~CPsDataPluginInterface( ) 
{
    PRINT ( _L("Enter CPsDataPluginInterface::~CPsDataPluginInterface") );
    
    UnloadPlugIns();
    iPsDataPluginInstances.ResetAndDestroy();
    
    PRINT ( _L("End CPsDataPluginInterface::~CPsDataPluginInterface") );
}
        
// ----------------------------------------------------------------------------
// CPsDataPluginInterface::InstantiateAllPlugInsL
// Instantiates all plugins
// ----------------------------------------------------------------------------     
void CPsDataPluginInterface::InstantiateAllPlugInsL( )
{
    PRINT ( _L("Enter CPsDataPluginInterface::InstantiateAllPlugInsL") );

    RImplInfoPtrArray infoArray;
    
    // Get list of all implementations
    ListAllImplementationsL( infoArray );
    
    // Instantiate plugins for all impUIds by calling 
    // InstantiatePlugInFromImpUidL
    for ( TInt i=0; i<infoArray.Count(); i++ )
    {
        // Get imp info
        CImplementationInformation& info( *infoArray[i] );
        
        // Get imp UID
        TUid impUid ( info.ImplementationUid() );
        
        PRINT ( _L("------------- PS Data Store Details -----------"));
        PRINT1 ( _L("Name: %S"), &(info.DisplayName()) );
        PRINT1 ( _L("UID: %X"), impUid.iUid );
        PRINT ( _L("-----------------------------------------------"));
        
        CPsDataPlugin* plugin = NULL;
        
        //instantiate plugin for impUid
        plugin = InstantiatePlugInFromImpUidL( impUid, iObserverForDataStore, iStoreListObserver ) ;
        if ( plugin )
        {
            
            iPsDataPluginInstances.AppendL( plugin );
                   
        }    	        	     
    }
    
    infoArray.ResetAndDestroy();
    
    PRINT ( _L("End CPsDataPluginInterface::InstantiateAllPlugInsL") );
}

// -----------------------------------------------------------------------------
// CPsDataPluginInterface::UnloadPlugIns
// Unloads plugins
// -----------------------------------------------------------------------------
void CPsDataPluginInterface::UnloadPlugIns()
{
    PRINT ( _L("Enter CPsDataPluginInterface::UnloadPlugIns") );
    
    REComSession::FinalClose();
    
    PRINT ( _L("End CPsDataPluginInterface::UnloadPlugIns") );
}

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::InstantiatePlugInFromImpUidL
// Instantiates plugin
// ---------------------------------------------------------------------------- 
CPsDataPlugin* CPsDataPluginInterface::InstantiatePlugInFromImpUidL( const TUid& aImpUid, 
																	 MDataStoreObserver* aObserverForDataStore,
																	 MStoreListObserver* aStoreListObserver )
{
    PRINT ( _L("Enter CPsDataPluginInterface::InstantiatePlugInFromImpUidL") );
    
    CPsDataPlugin* plugin = CPsDataPlugin::NewL(aImpUid, aObserverForDataStore, aStoreListObserver);
    
    PRINT ( _L("End CPsDataPluginInterface::InstantiatePlugInFromImpUidL") );
    
    return plugin;
}
    

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::RequestForDataL
// Requests for data from the data store
// ---------------------------------------------------------------------------- 
void CPsDataPluginInterface::RequestForDataL(TDesC& aDataStore)
{

    PRINT ( _L("Enter CPsDataPluginInterface::RequestForDataL") );
   
    for ( TInt idx = 0; idx < iPsDataPluginInstances.Count(); idx++ )
    {
        if ( iPsDataPluginInstances[idx]->IsDataStoresSupportedL(aDataStore) )
        {
            iPsDataPluginInstances[idx]->RequestForDataL(aDataStore);
            break; 
        }
    }
    
    PRINT ( _L("End CPsDataPluginInterface::RequestForDataL") );

}

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::GetAllSupportedDataStoresL
// Requests all the supported data stores from data adapters
// ---------------------------------------------------------------------------- 
void CPsDataPluginInterface::GetAllSupportedDataStoresL(RPointerArray<TDesC>& aDataStores)
{

    PRINT ( _L("Enter CPsDataPluginInterface::GetAllSupportedDataStoresL") );
   
    for ( TInt idx = 0; idx < iPsDataPluginInstances.Count(); idx++ )
    {
        iPsDataPluginInstances[idx]->GetSupportedDataStoresL(aDataStores);
    }
    
    PRINT ( _L("End CPsDataPluginInterface::GetAllSupportedDataStoresL") );

}

// ----------------------------------------------------------------------------
// CPsDataPluginInterface::GetAllSupportedDataFieldsL
// Requests all the supported data fields from from data adapters
// ---------------------------------------------------------------------------- 
void  CPsDataPluginInterface::GetSupportedDataFieldsL(TDesC& aUri, RArray<TInt>& aDataFields)
{

    PRINT ( _L("Enter CPsDataPluginInterface::GetAllSupportedDataStoresL") );
   
    for ( TInt idx = 0; idx < iPsDataPluginInstances.Count(); idx++ )
    {
       RPointerArray<TDesC> dataStores;
       CleanupClosePushL( dataStores );
       iPsDataPluginInstances[idx]->GetSupportedDataStoresL(dataStores);
       
       for ( TInt i(0); i<dataStores.Count(); i++)
       {
           if ( dataStores[i]->Compare(aUri) == 0 )
           {
               iPsDataPluginInstances[idx]->GetSupportedDataFieldsL(aDataFields);
               break;
           }
       }
       CleanupStack::PopAndDestroy( &dataStores ); // Close
    }
    
    PRINT ( _L("End CPsDataPluginInterface::GetAllSupportedDataStoresL") );

}
// End of file
