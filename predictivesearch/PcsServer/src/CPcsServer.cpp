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
* Description:  PCS Server main class. Co-ordinates server startup,  
*                shutdown and receives client requests.
*
*/

// INCLUDE FILES
#include <e32base.h>
#include <featmgr.h>

#include "CPcsDebug.h"
#include "CPcsDefs.h"
#include "CPcsServer.h"
#include "CPcsSession.h"
#include "CPcsPluginInterface.h"

//Costant Declaration
const TInt KAlgorithmNameMaxLen = 50;
// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsServer::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsServer* CPcsServer::NewL()
{
    PRINT ( _L("Enter CPcsServer::NewL") );
    
    CPcsServer* self = new (ELeave) CPcsServer;
    CleanupStack::PushL(self);
    self->ConstructL();
    self->StartL(KPcsServerName);
    CleanupStack::Pop(); 
    
    PRINT ( _L("End CPcsServer::NewL") );
    
    return self;
}

// ----------------------------------------------------------------------------
// CPcsServer::CPcsServer
// Constructor
// ----------------------------------------------------------------------------
CPcsServer::CPcsServer() : CServer2(EPriorityStandard)
{
	PRINT ( _L("Enter CPcsServer::CPcsServer") );
	PRINT ( _L("End CPcsServer::CPcsServer") );
}

// ----------------------------------------------------------------------------
// CPcsServer::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CPcsServer::ConstructL()
{  
	PRINT ( _L("Enter CPcsServer::ConstructL") );
	
	iPcs = CPcsPluginInterface::NewL();
    
	// Check if the phone is chinese feature id installed
	iIsChineseVariant = IsChineseFeatureInitilizedL();
    // Create the plugin for the required algorithm
    // Matches the ECOM Plugin's display name definition in rss file
    // Check for the chinese feature id flag.
	if(iIsChineseVariant) 
	    {
	    // Chinese variant phones. Use Algorithm 2
	    PRINT ( _L("Enter CPcsServer::ConstructL() - Chinese Variant Phone Algorithm Instantiated") );
	    TBuf<KAlgorithmNameMaxLen> algorithmName(KPcsAlgorithm_Chinese);
	    iPcs->InstantiateAlgorithmL(algorithmName);
	    
	    
	    }
	else 
	    {
	    // NON-Chinese variant phones. Use Algorithm 1
	    PRINT ( _L("Enter CPcsServer::ConstructL() - NON-Chinese Variant Phone Algorithm Instantiated") );
	    TBuf<KAlgorithmNameMaxLen> algorithmName(KPcsAlgorithm_NonChinese);
	   // TBuf<KAlgorithmNameMaxLen> algorithmName(KPcsAlgorithm_Chinese);
	    iPcs->InstantiateAlgorithmL(algorithmName);
	    
	    }
  
	PRINT ( _L("End CPcsServer::ConstructL") );
}

// ----------------------------------------------------------------------------
// CPcsServer::~CPcsServer
// Destructor
// ----------------------------------------------------------------------------
CPcsServer::~CPcsServer()
{
	PRINT ( _L("Enter CPcsServer::~CPcsServer") );
	delete iPcs;
	PRINT ( _L("End CPcsServer::~CPcsServer") );
}

// ----------------------------------------------------------------------------
// CPcsServer::NewSessionL
// Constructor
// ----------------------------------------------------------------------------
CSession2* CPcsServer::NewSessionL(const TVersion& aVersion,
	                               const RMessage2& /*aMessage*/ ) const
{
	PRINT ( _L("Enter CPcsServer::NewSessionL") );
	
    TVersion serverVersion( 1, 0, 0);
    if ( !User::QueryVersionSupported(serverVersion,aVersion) )
		User::Leave(KErrNotSupported);

    CPcsSession* session = CPcsSession::NewL(const_cast<CPcsServer*> (this));
    
    PRINT ( _L("End CPcsServer::NewSessionL") );
    
    return session;
}

// ----------------------------------------------------------------------------
// CPcsServer::PluginInterface
// Get the plugin interface
// ----------------------------------------------------------------------------
CPcsPluginInterface* CPcsServer::PluginInterface()
{
	return iPcs;
};

// ----------------------------------------------------------------------------
// CPcsServer::IsChineseFeatureInitilizedL()
// Returns ETrue if Chinese variant feature is initilized
// ----------------------------------------------------------------------------
TBool CPcsServer::IsChineseFeatureInitilizedL()
{
     FeatureManager::InitializeLibL();
    
    TBool chineseFeatureInitialized = FeatureManager::FeatureSupported(KFeatureIdChinese) &&
        ( FeatureManager::FeatureSupported(KFeatureIdChinesePrcFonts) || FeatureManager::FeatureSupported(KFeatureIdChineseTaiwanHkFonts)  );
      
    FeatureManager::UnInitializeLib();
    
    return chineseFeatureInitialized;
}

// ----------------------------------------------------------------------------
// CPcsServer::IsChineseVariant() const
// Returns ETrue if there is Chinese variant feature in phone
// ---------------------------------------------------------------------------- 
TBool CPcsServer::IsChineseVariant() const
    {
    return iIsChineseVariant;
    }
