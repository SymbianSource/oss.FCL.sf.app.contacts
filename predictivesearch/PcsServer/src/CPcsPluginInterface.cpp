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
* Description:  PCS Server Plugin Interface
*
*/


// INCLUDE FILES
#include "CPcsPluginInterface.h"
#include "CPcsPlugin.h"
#include "CPcsDebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsPluginInterface::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsPluginInterface* CPcsPluginInterface::NewL()
{
    PRINT ( _L("Enter CPcsPluginInterface::NewL") );
    
    CPcsPluginInterface* self = CPcsPluginInterface::NewLC();
    CleanupStack::Pop( self );
    
    PRINT ( _L("End CPcsPluginInterface::NewL") );
    
    return self;
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::NewLC
// Two Phase Construction
// ---------------------------------------------------------------------------- 
CPcsPluginInterface* CPcsPluginInterface::NewLC()
{
    PRINT ( _L("Enter CPcsPluginInterface::NewLC") );
    
    CPcsPluginInterface* self = new( ELeave ) CPcsPluginInterface();
    CleanupStack::PushL( self );
    self->ConstructL();
    
    PRINT ( _L("End CPcsPluginInterface::NewLC") );
    
    return self;
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::CPcsPluginInterface
// Constructor
// ----------------------------------------------------------------------------    
CPcsPluginInterface::CPcsPluginInterface()
{
    PRINT ( _L("Enter CPcsPluginInterface::CPcsPluginInterface") );
    PRINT ( _L("End CPcsPluginInterface::CPcsPluginInterface") );
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::ConstructL
// 2nd phase constructor
// ---------------------------------------------------------------------------- 
void CPcsPluginInterface::ConstructL()
{
    PRINT ( _L("Enter CPcsPluginInterface::ConstructL") );
    PRINT ( _L("End CPcsPluginInterface::ConstructL") );
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::~CPcsPluginInterface
// Constructor
// ---------------------------------------------------------------------------- 
CPcsPluginInterface::~CPcsPluginInterface( ) 
{
    PRINT ( _L("Enter CPcsPluginInterface::~CPcsPluginInterface") );
    
    UnloadPlugIns();
    iPcsPluginInstances.ResetAndDestroy();
    
    PRINT ( _L("End CPcsPluginInterface::~CPcsPluginInterface") );
}
        
// ----------------------------------------------------------------------------
// CPcsPluginInterface::InstantiateAllPlugInsL
// Instantiates all plugins
// ----------------------------------------------------------------------------     
void CPcsPluginInterface::InstantiateAlgorithmL( TDes& aAlgorithm )
{
    PRINT ( _L("Enter CPcsPluginInterface::InstantiateAlgorithmL") );

    RImplInfoPtrArray infoArray;
    
    // Get list of all implementations
    ListAllImplementationsL( infoArray );
    
    // Instantiate plugins for all impUIds by calling 
    // InstantiatePlugInFromImpUidL
    for ( TInt i=0; i<infoArray.Count(); i++ )
    {
        // Get imp info
        CImplementationInformation& info( *infoArray[i] );
        
        if ( info.DisplayName() == aAlgorithm )
        {
            // Get imp UID
	        TUid impUid ( info.ImplementationUid() );
	        
	        PRINT ( _L("------------- PCS Algorithm Details -----------"));
	        PRINT1 ( _L("Name: %S"), &(info.DisplayName()) );
	        PRINT1_BOOT_PERFORMANCE ( _L("Name: %S"), &(info.DisplayName()) );
	        PRINT1 ( _L("UID: %X"), impUid.iUid );
	        iAlgorithmInUse.iUid = impUid.iUid;
	        PRINT ( _L("-----------------------------------------------"));
	        
	        CPcsPlugin* searchPlugin = NULL;
	        
	        //instantiate plugin for impUid
	        searchPlugin = InstantiatePlugInFromImpUidL( impUid );
	        if ( searchPlugin )
	        {
	            iPcsPluginInstances.AppendL( searchPlugin );
	        }    
	        
	        break;
        }
    }
    
    infoArray.ResetAndDestroy();
    
    PRINT ( _L("End CPcsPluginInterface::InstantiateAlgorithmL") );
}

// -----------------------------------------------------------------------------
// CPcsPluginInterface::UnloadPlugIns
// Unloads plugins
// -----------------------------------------------------------------------------
void CPcsPluginInterface::UnloadPlugIns()
{
    PRINT ( _L("Enter CPcsPluginInterface::UnloadPlugIns") );
    
    REComSession::FinalClose();
    
    PRINT ( _L("End CPcsPluginInterface::UnloadPlugIns") );
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::InstantiatePlugInFromImpUidL
// Instantiates plugin
// ---------------------------------------------------------------------------- 
CPcsPlugin* CPcsPluginInterface::InstantiatePlugInFromImpUidL( const TUid& aImpUid )
{
    PRINT ( _L("Enter CPcsPluginInterface::InstantiatePlugInFromImpUidL") );
    
    CPcsPlugin* search = CPcsPlugin::NewL(aImpUid);
    
    PRINT ( _L("End CPcsPluginInterface::InstantiatePlugInFromImpUidL") );
    
    return search;
}
    
    
// ----------------------------------------------------------------------------
// CPcsPluginInterface::PerformSearchL
// Search
// ----------------------------------------------------------------------------      
void CPcsPluginInterface::PerformSearchL(CPsSettings& aSettings,
                                         CPsQuery& aCondition,
                                         RPointerArray<CPsClientData>& aPsData,
                                         RPointerArray<CPsPattern>& aPattern)                                         
{  
    PRINT ( _L("Enter CPcsPluginInterface::PerformSearchL") );

    for ( TInt idx = 0; idx < iPcsPluginInstances.Count(); idx++ )
    {
       iPcsPluginInstances[idx]->PerformSearchL( aSettings,
                                                 aCondition,
                                                 aPsData,
                                                 aPattern );
    }
    
    PRINT ( _L("End CPcsPluginInterface::PerformSearchL") );
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::SearchInputL
// Search
// ----------------------------------------------------------------------------      
void CPcsPluginInterface::SearchInputL(CPsQuery& aSearchQuery,
                                       TDesC& aSearchData,
                                       RPointerArray<TDesC>& aMatchSet,
                            		   RArray<TPsMatchLocation>& aMatchLocation)
{  
    PRINT ( _L("Enter CPcsPluginInterface::SearchInputL") );

    for ( TInt idx = 0; idx < iPcsPluginInstances.Count(); idx++ )
    {
       iPcsPluginInstances[idx]->SearchInputL( aSearchQuery,
                                               aSearchData,                                                  
                                               aMatchSet,
                                               aMatchLocation );
    }
    
    PRINT ( _L("End CPcsPluginInterface::SearchInputL") );
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::SearchMatchStringL
// Search
// ----------------------------------------------------------------------------      
void CPcsPluginInterface::SearchMatchStringL(CPsQuery& aSearchQuery,
                   TDesC& aSearchData,
                   TDes& aMatch )
{  
    PRINT ( _L("Enter CPcsPluginInterface::SearchMatchStringL") );

    for ( TInt idx = 0; idx < iPcsPluginInstances.Count(); idx++ )
    {
       iPcsPluginInstances[idx]->SearchMatchStringL( aSearchQuery,
                                                     aSearchData,
                                                     aMatch );
    }
    
    PRINT ( _L("End CPcsPluginInterface::SearchMatchStringL") );
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::IsLanguageSupported
// 
// ----------------------------------------------------------------------------      
TBool CPcsPluginInterface::IsLanguageSupportedL(const TUint32 aLanguage)
{
	return(iPcsPluginInstances[0]->IsLanguageSupportedL(aLanguage));
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::GetDataOrderL
// 
// ----------------------------------------------------------------------------      
void CPcsPluginInterface::GetDataOrderL(TDesC& aURI, RArray<TInt>& aDataOrder)
{
	return(iPcsPluginInstances[0]->GetDataOrderL(aURI, aDataOrder));
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::GetSortOrderL
// 
// ----------------------------------------------------------------------------      
void CPcsPluginInterface::GetSortOrderL(TDesC& aURI, RArray<TInt>& aSortOrder)
{
	return(iPcsPluginInstances[0]->GetSortOrderL(aURI, aSortOrder));
}

// ----------------------------------------------------------------------------
// CPcsPluginInterface::ChangeSortOrderL
// 
// ----------------------------------------------------------------------------      
void CPcsPluginInterface::ChangeSortOrderL(TDesC& aURI, RArray<TInt>& aSortOrder)
{
	return(iPcsPluginInstances[0]->ChangeSortOrderL(aURI, aSortOrder));
}
// End of file
