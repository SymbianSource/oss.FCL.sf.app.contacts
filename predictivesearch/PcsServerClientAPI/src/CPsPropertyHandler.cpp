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
* Description:  This is the client side internal file to handle
*                property used in Publish and Subscribe framework.
*
*/

//SYSTEM INCLUDES
#include <CPsRequestHandler.h>
#include <MPsResultsObserver.h>

// USER INCLUDES
#include "CPsPropertyHandler.h"
#include "CPcsDebug.h"

// UID used for Publish and Subscribe mechanism
// This should be same as the one defined in CPcsAlgorithm.cpp
// Server UID3 has to be used for this framework
const TUid KCStatus = {0x2000B5B6};

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CPsPropertyHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
 CPsPropertyHandler* CPsPropertyHandler::NewL( CPSRequestHandler* aRequestHandler )
{
    PRINT ( _L("Enter CPsPropertyHandler::NewL") );
    
    CPsPropertyHandler* self = new ( ELeave ) CPsPropertyHandler(aRequestHandler);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    PRINT ( _L("End CPsPropertyHandler::NewL") );
    
    return( self ) ;
}

// -----------------------------------------------------------------------------
// CPsPropertyHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CPsPropertyHandler::ConstructL(  )
{
    PRINT ( _L("Enter CPsPropertyHandler::ConstructL") );
    
    TInt err = iCacheStatusProperty.Attach(KCStatus, 0 );
    User::LeaveIfError(err);   
    
    // Attach the cache error property
    err = iCacheErrorProperty.Attach(KCStatus, 1);
    User::LeaveIfError(err); 
    
    iCacheStatusProperty.Subscribe(iStatus);
    
    SetActive();
    
    PRINT ( _L("End CPsPropertyHandler::ConstructL") );
}

// -----------------------------------------------------------------------------
// CPsPropertyHandler::CPsPropertyHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CPsPropertyHandler::CPsPropertyHandler( CPSRequestHandler* aRequestHandler )
: CActive( EPriorityStandard ), 
  iRequestHandler(aRequestHandler)
{
    PRINT ( _L("Enter CPsPropertyHandler::CPsPropertyHandler") );
    
    CActiveScheduler::Add( this );
    
    PRINT ( _L("End CPsPropertyHandler::CPsPropertyHandler") );
}

// -----------------------------------------------------------------------------
// CPsPropertyHandler::~CPsPropertyHandler()
// Destructor.
// -----------------------------------------------------------------------------
CPsPropertyHandler::~CPsPropertyHandler()
{
    PRINT ( _L("Enter CPsPropertyHandler::~CPsPropertyHandler") );

    Cancel(); // Causes call to DoCancel()
    
    iCacheStatusProperty.Close();

}

// -----------------------------------------------------------------------------
// CPsPropertyHandler::RunL()
// Invoked to handle responses from the server.
// -----------------------------------------------------------------------------
void CPsPropertyHandler::RunL()
{
	iCacheStatusProperty.Subscribe(iStatus);
	SetActive();
	
	//Get the value
	TCachingStatus status; 
	TInt statusValue;
	iCacheStatusProperty.Get(statusValue);
	status = (TCachingStatus)statusValue;
	
	TInt cacheError;
	iCacheErrorProperty.Get(cacheError);
	
	if( (status == ECachingComplete) || (status == ECachingCompleteWithErrors))
	{
		for(TInt i = 0; i < iRequestHandler->iObservers.Count(); i++)
			iRequestHandler->iObservers[i]->CachingStatus(status, cacheError);
	}
}

// -----------------------------------------------------------------------------
// CPsPropertyHandler::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
void CPsPropertyHandler::DoCancel()
{
	iCacheStatusProperty.Cancel();
}

// -----------------------------------------------------------------------------
// CPsPropertyHandler::GetCachingStatusL()
// Returns the caching status fr synchronous request
// -----------------------------------------------------------------------------
TInt CPsPropertyHandler::GetCachingStatusL(TCachingStatus& aStatus)
{
	TInt status;
	//Get the status from the property
	iCacheStatusProperty.Get(status);
	
	aStatus = (TCachingStatus)status;
	
	TInt cacheError;
	iCacheErrorProperty.Get(cacheError);
	
	return cacheError;
}

//END OF FILE
