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
* Description:  This is the PS server client side interface implementation.
*
*/


// INCLUDE FILES
#include "RPsSession.h"
#include "CPsData.h"
#include "CPcsDebug.h"

// ----------------------------------------------------------------------------
// StartServer
// Starts the server. Used only when the server is implemented as a transient.
// ----------------------------------------------------------------------------
TInt StartServer()
{
    PRINT ( _L("Enter RPsSession->StartServer") );

	RProcess server;
    server.Create ( KPcsServerExeName, KNullDesC );

    TRequestStatus status;
    server.Rendezvous(status);

    if ( status != KRequestPending )
    {
		server.Kill(0);
		server.Close();
		return KErrGeneral;
    }
    else
    {
		server.Resume();
    }

    User::WaitForRequest(status);

    if ( status != KErrNone )
    {
    	server.Close();
		return status.Int();
    }

    PRINT ( _L("End RPsSession->StartServer") );

    return KErrNone;
}
// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// RPsSession::RPsSession
// Constructor
// ----------------------------------------------------------------------------
RPsSession::RPsSession() : RSessionBase(),
                           iSearchQueryBufferPtr (0, 0),
                           iResultsBufferPtr (0, 0)
{
    PRINT ( _L("Enter RPsSession::RPsSession") );
    PRINT ( _L("End RPsSession::RPsSession") );
}


// ----------------------------------------------------------------------------
// RPsSession::Connects to the search server
// Returns the version number
// ----------------------------------------------------------------------------
TInt RPsSession::Connect()
{
    PRINT ( _L("Enter RPsSession::Connect") );

    TInt err = CreateSession( KPcsServerName, Version() );
    if ( err != KErrNone )
        {
        PRINT ( _L("Predictive Search server not running. Trying to start") );
        RMutex mutex;
        TInt errMutex = mutex.CreateGlobal( KPcsServerName );
        if ( errMutex == KErrAlreadyExists )
            {
            mutex.OpenGlobal( KPcsServerName );
            }
        mutex.Wait();
        err = CreateSession( KPcsServerName, Version() );
        if ( err != KErrNone )
            {
            err = StartServer();
            PRINT1( _L("Predictive Search Engine, StartServer(), Err = %d"), err );
            if ( err == KErrNone )
                {
                err = CreateSession( KPcsServerName, Version() );
                }
            }
        mutex.Signal();
        mutex.Close();
        }

    PRINT ( _L("End RPsSession::Connect") );

    return err;
}

// ----------------------------------------------------------------------------
// RPsSession::Version
// Returns the version number
// ----------------------------------------------------------------------------
TVersion RPsSession::Version() const
{
    return ( TVersion ( KPcsServerMajorVersionNumber,
		                KPcsServerMinorVersionNumber,
		             	KPcsServerBuildVersionNumber ) );
}

// ----------------------------------------------------------------------------
// RPsSession::SearchL
// Initiate a search request.
// ----------------------------------------------------------------------------
void RPsSession::SetSearchSettingsL(const TDes8& aSettings)
{
    PRINT ( _L("Enter RPsSession::SetSearchSettingsL") );

    TIpcArgs args(&aSettings);

    // Send the search settings to the server. sync call
    TRequestStatus status;
	SendReceive(ESearchSettings,
	            args,
	            status);
    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::SetSearchSettingsL") );
}

// ----------------------------------------------------------------------------
// RPsSession::SearchL
// Initiate a search request.
// ----------------------------------------------------------------------------
void RPsSession::SearchL(const TDes8& aSearchQuery,
                         TPtr8 aResultsBuffer,
                         TRequestStatus& aStatus)
{
    PRINT ( _L("Enter RPsSession::SearchL") );

    // Hold the pointer to buffers till the async request is complete
    iSearchQueryBufferPtr.Set(aSearchQuery);
    iResultsBufferPtr.Set(aResultsBuffer);

    TIpcArgs args(&iSearchQueryBufferPtr, &iResultsBufferPtr);

    aStatus = KRequestPending;

    // Initiate the search request
	SendReceive(ESearch,
	            args,
	            aStatus );

    PRINT ( _L("End RPsSession::SearchL") );
}

// ----------------------------------------------------------------------------
// RPsSession::CancelSearch
// Send a request to cancel search. Synchronous.
// ----------------------------------------------------------------------------
void RPsSession::CancelSearch()
{
    PRINT ( _L("Enter RPsSession::CancelSearch") );

    TRequestStatus status;

	SendReceive(ECancelSearch,
	            TIpcArgs(),
	            status );

    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::CancelSearch") );
}

// ----------------------------------------------------------------------------
// RPsSession::SearchL
// Initiate a search request.
// ----------------------------------------------------------------------------
void RPsSession::SearchL(const TDes8& aSearchQuery,
                         const TDes8& aSearchData,
                         TPtr8 aResultsBuffer)
{
    PRINT ( _L("Enter RPsSession::SearchL") );

    TIpcArgs args(&aSearchQuery, &aSearchData, &aResultsBuffer);

    TRequestStatus status;

    // Initiate the search request
	SendReceive(ESearchInput,
	            args,
	            status );

    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::SearchL") );
}

// ----------------------------------------------------------------------------
// RPsSession::SearchMatchStringL
// Initiate a search request.
// ----------------------------------------------------------------------------
void RPsSession::SearchMatchStringL(const TDes8& aSearchQuery,
                         const TDes8& aSearchData,
                         TDes& aResultsBuffer)
{
    PRINT ( _L("Enter RPsSession::SearchMatchStringL") );

    TIpcArgs args(&aSearchQuery, &aSearchData, &aResultsBuffer);

    TRequestStatus status;

    // Initiate the search request
    SendReceive(ESearchMatchString,
                args,
                status );

    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::SearchMatchStringL") );
}

// ----------------------------------------------------------------------------
// RPsSession::SendNewBufferL
// Send a new buffer to recover the search results.
// ----------------------------------------------------------------------------
void RPsSession::SendNewBufferL(TPtr8 aResultsBuffer,
                                TRequestStatus& aStatus)
{
    PRINT ( _L("Enter RPsSession::SendNewBufferL") );

	iResultsBufferPtr.Set(aResultsBuffer);

	aStatus = KRequestPending;

    // Search command is reused here. Hence there is no need to fill
    // the search query.
    TIpcArgs args(TIpcArgs::ENothing,
                  &iResultsBufferPtr);

	SendReceive(ESearch,
	            args,
	            aStatus);

    PRINT ( _L("Enter RPsSession::SendNewBufferL") );
}

// -----------------------------------------------------------------------------
// RPsSession::IsLanguageSupportedL()
// Checks if the language variant is supported by
// the predictive search engine.
// Synchronous
// -----------------------------------------------------------------------------
void RPsSession::IsLanguageSupportedL(const TDes8& aLanguage,
                                      TPtr8 aResultsBuffer)
{
	PRINT ( _L("Enter RPsSession::IsLanguageSupportedL") );

	TIpcArgs args(&aLanguage, &aResultsBuffer);
	TRequestStatus status;

	// initiate the request
	SendReceive(ELangSupport,
				args,
				status);

	User::WaitForRequest(status);

	PRINT ( _L("End RPsSession::IsLanguageSupportedL") );
}

// ----------------------------------------------------------------------------
// RPsSession::GetDataOrderL
// Send a request to the PS server to recover data fields supported by a store.
// ----------------------------------------------------------------------------
void RPsSession::GetDataOrderL(const TDes8& aURI,
                               TPtr8 aResultsBuffer)
{
    PRINT ( _L("Enter RPsSession::GetDataOrderL") );

    TIpcArgs args(&aURI, &aResultsBuffer);

    TRequestStatus status;

    // Initiate the request
	SendReceive(EGetDataOrder,
	            args,
	            status );

    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::GetDataOrderL") );
}

// ----------------------------------------------------------------------------
// RPsSession::GetSortOrderL
// Send a request to the PS server to recover sort order supported by a store.
// ----------------------------------------------------------------------------
void RPsSession::GetSortOrderL(const TDes8& aURI,
                               TPtr8 aResultsBuffer)
{
    PRINT ( _L("Enter RPsSession::GetSortOrderL") );

    TIpcArgs args(&aURI, &aResultsBuffer);

    TRequestStatus status;

    // Initiate the request
	SendReceive(EGetSortOrder,
	            args,
	            status );

    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::GetSortOrderL") );
}

// ----------------------------------------------------------------------------
// RPsSession::ChangeSortOrderL
// Send a request to the PS server to set sort order for a store.
// ----------------------------------------------------------------------------
void RPsSession::ChangeSortOrderL(const TDes8& aInput)
{
    PRINT ( _L("Enter RPsSession::ChangeSortOrderL") );

    TIpcArgs args(&aInput);

    TRequestStatus status;

    // Initiate the request
	SendReceive(ESetSortOrder,
	            args,
	            status );

    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::ChangeSortOrderL") );
}

// ----------------------------------------------------------------------------
// RPsSession::GetAdaptiveGridL
// Initiate the Adaptive Grid request.
// ----------------------------------------------------------------------------
void RPsSession::GetAdaptiveGridL( const TDesC8& aURIs,
                                   const TBool aCompanyName,
                                   TDes& aResultsBuffer )
{
    PRINT ( _L("Enter RPsSession::GetAdaptiveGrid") );

    TIpcArgs args( &aURIs, aCompanyName, &aResultsBuffer );

    TRequestStatus status;

    // Initiate the Adaptive Grid request
    SendReceive( EGetAdaptiveGrid,
                 args,
                 status );

    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::GetAdaptiveGrid") );
}

// ----------------------------------------------------------------------------
// RPsSession::ShutdownServerL
// Shutsdown the PS Server. Synchronous.
// ----------------------------------------------------------------------------
void RPsSession::ShutdownServerL()
{
    PRINT ( _L("Enter RPsSession::ShutdownServerL") );

    TIpcArgs args;

    TRequestStatus status;
    SendReceive(EShutdown, args, status );
    User::WaitForRequest(status);

    PRINT ( _L("End RPsSession::ShutdownServerL") );
}

// End of File
