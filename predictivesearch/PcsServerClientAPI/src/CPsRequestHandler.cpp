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
* Description:  This is the client side interface for the predictive search
*                server.
*
*/

// SYSTEM INCLUDES
#include <MPsResultsObserver.h>
#include <CPsRequestHandler.h>
#include <RPsSession.h>

// USER INCLUDE
#include "CPsPropertyHandler.h"
#include "CPsUpdateHandler.h"
#include "CPcsDebug.h"
#include "CPsPattern.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CPSRequestHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CPSRequestHandler* CPSRequestHandler::NewL()
    {
    PRINT ( _L("Enter CPSRequestHandler::NewL") );

    CPSRequestHandler* self = NewLC();
    CleanupStack::Pop(self);

    PRINT ( _L("End CPSRequestHandler::NewL") );

    return self;
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
EXPORT_C CPSRequestHandler* CPSRequestHandler::NewLC()
    {
    PRINT ( _L("Enter CPSRequestHandler::NewLC") );

    CPSRequestHandler* self = new (ELeave) CPSRequestHandler();
    CleanupStack::PushL(self);
    self->ConstructL();

    PRINT ( _L("End CPSRequestHandler::NewLC") );

    return self;
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
void CPSRequestHandler::ConstructL()
    {
    PRINT ( _L("Enter CPSRequestHandler::ConstructL") );

    User::LeaveIfError(iSession.Connect());

    // Initiate the property handler
    iPropertyHandler = CPsPropertyHandler::NewL(this);

    // Initiate handlers for cache updating
    iContactAddedHandler = CPsUpdateHandler::NewL( *this, EPsKeyContactAddedCounter );
    iContactModifiedHandler = CPsUpdateHandler::NewL( *this, EPsKeyContactModifiedCounter );
    iContactRemovedHandler = CPsUpdateHandler::NewL( *this, EPsKeyContactRemovedCounter );
    
    // Initialize the contact id converter
    iConverter = NULL;

    PRINT ( _L("End CPSRequestHandler::ConstructL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::CCSAsyncRequestHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CPSRequestHandler::CPSRequestHandler() :
    CActive(EPriorityStandard)
    {
    PRINT ( _L("Enter CPSRequestHandler::CPSRequestHandler") );

    CActiveScheduler::Add(this);

    PRINT ( _L("End CPSRequestHandler::CPSRequestHandler") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::~CPSRequestHandler()
// Destructor.
// -----------------------------------------------------------------------------
CPSRequestHandler::~CPSRequestHandler()
    {
    PRINT ( _L("Enter CPSRequestHandler::~CPSRequestHandler") );

    Cancel(); // Causes call to DoCancel()

    // Close the session
    iSession.Close();

    delete iSearchQueryBuffer;
    delete iPendingSearchQueryBuffer;
    delete iSearchDataBuffer;
    delete iSearchResultsBuffer;
    delete iPropertyHandler;
    delete iContactAddedHandler;
    delete iContactModifiedHandler;
    delete iContactRemovedHandler;
    delete iConverter;

    iObservers.Reset();
    iObservers.Close();

    PRINT ( _L("End CPSRequestHandler::~CPSRequestHandler") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::AddObserverL()
// 
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::AddObserverL(MPsResultsObserver* aObserver)
    {
    iObservers.InsertInAddressOrderL(aObserver);
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::RemoveObserver()
// 
// -----------------------------------------------------------------------------
EXPORT_C TInt CPSRequestHandler::RemoveObserver(MPsResultsObserver* aObserver)
    {
    TInt index = iObservers.FindInAddressOrder(aObserver);
    if (index != KErrNotFound)
        {
        iObservers.Remove(index);
        return KErrNone;
        }
    else
        {
        return KErrNotFound;
        }
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::SetSearchSettingsL()
// Send the search settings to the predictive search engine
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::SetSearchSettingsL(const CPsSettings& aSettings)
    {
    PRINT ( _L("Enter CPSRequestHandler::SetSearchSettingsL") );

    // Tmp buffer        
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buf);

    // Stream over the temp buffer
    RBufWriteStream stream(*buf);
    stream.PushL();

    // Write the search query to the stream
    aSettings.ExternalizeL(stream);
    stream.CommitL();

    // Create a HBufC8 for IPC
    HBufC8* settingsBuffer = HBufC8::NewLC(buf->Size());
    TPtr8 ptr(settingsBuffer->Des());
    buf->Read(0, ptr, buf->Size());

    // Send the settings to the server
    iSession.SetSearchSettingsL(settingsBuffer->Des());

    CleanupStack::PopAndDestroy(3, buf); // buf, stream, settingsBuffer	                 

    PRINT ( _L("End CPSRequestHandler::SetSearchSettingsL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::SearchL()
// Sends a request to the predictive search engine to perform a search.
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::SearchL(const CPsQuery& aSearchQuery)
    {
    // __LATENCY_MARK ( _L("CPSRequestHandler::SearchL  <--- INITIATE SEARCH") );
    PRINT ( _L("Enter CPSRequestHandler::SearchL") );

    // ----------------------- Search Query ----------------------
    iSearchRequestCancelled = EFalse;
    
    // Tmp buffer        
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buf);

    // Stream over the temp buffer
    RBufWriteStream stream(*buf);
    stream.PushL();

    // Write the search query to the stream
    aSearchQuery.ExternalizeL(stream);
    stream.CommitL();

    // If there is no submitted request, then submits the new coming search request     
    if (!IsActive())
        {
        // Create a HBufC8 for IPC
        if (iSearchQueryBuffer)
            {
            delete iSearchQueryBuffer;
            iSearchQueryBuffer = NULL;
            }
        iSearchQueryBuffer = HBufC8::NewLC(buf->Size());
        CleanupStack::Pop(); // iSearchQueryBuffer
        TPtr8 ptr(iSearchQueryBuffer->Des());
        buf->Read(0, ptr, buf->Size());

        // Clear pending search query buffer
        if (iPendingSearchQueryBuffer)
            {
            delete iPendingSearchQueryBuffer;
            iPendingSearchQueryBuffer = NULL;
            }

        // ------------------------- Results Buffer ---------------------

        // Create a buffer to store the search results.    
        if (iSearchResultsBuffer)
            {
            delete iSearchResultsBuffer;
            iSearchResultsBuffer = NULL;
            }
        iSearchResultsBuffer = HBufC8::NewL(KSearchResultsBufferLen);

        // --------------------------------------------------------------      

        // Initiate the search
        iSession.SearchL(iSearchQueryBuffer->Des(), iSearchResultsBuffer->Des(), iStatus);
        SetActive();
        }
    else // else holds it in iPendingSearchQueryBuffer( which will be submitted in HandleSearchResults() )
        {
        CancelSearch();
        iPendingSearchQueryBuffer = HBufC8::NewLC(buf->Size());
        CleanupStack::Pop();
        TPtr8 ptr(iPendingSearchQueryBuffer->Des());
        buf->Read(0, ptr, buf->Size());
        }

    CleanupStack::PopAndDestroy(2, buf); // buf, stream   

    PRINT ( _L("End CPSRequestHandler::SearchL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::LookupL()
// Sends a request to the predictive search engine to perform a search.
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::LookupL(const CPsQuery& aSearchQuery, 
                                         const TDesC& aSearchData,
                                         CDesCArray& aMatchSet, 
                                         RArray<TPsMatchLocation>& aMatchLocation)
    {
    PRINT ( _L("Enter CPSRequestHandler::LookupL") );

    // ----------------------- Search Query ----------------------

    // Tmp buffer        
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buf);

    // Stream over the temp buffer
    RBufWriteStream stream(*buf);
    stream.PushL();

    // Write the search query to the stream
    aSearchQuery.ExternalizeL(stream);
    stream.CommitL();

    // Create a HBufC8 for IPC
    HBufC8* searchQueryBuffer = HBufC8::NewL(buf->Size());
    TPtr8 ptr(searchQueryBuffer->Des());
    buf->Read(0, ptr, buf->Size());
    
    CleanupStack::PopAndDestroy(2, buf); // buf, stream
    CleanupStack::PushL(searchQueryBuffer);

    // ----------------------- Search Data ----------------------

    // Tmp buffer        
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);

    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();

    // Write the search data to the stream
    dataStream.WriteUint16L(aSearchData.Size());
    dataStream << aSearchData;

    // Create a HBufC8 for IPC
    HBufC8* searchDataBuffer = HBufC8::NewL(dataBuf->Size());
    TPtr8 dataPtr(searchDataBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());

    CleanupStack::PopAndDestroy(2, dataBuf); // dataBuf, dataStream
    CleanupStack::PushL(searchDataBuffer);
    
    // ------------------------- Results Buffer ---------------------

    // Create a buffer to store the search results.    
    HBufC8* searchResultsBuffer = HBufC8::NewLC(KBufferMaxLen);

    // --------------------------------------------------------------      

    // Initiate the search
    iSession.SearchL(searchQueryBuffer->Des(), searchDataBuffer->Des(),
                     searchResultsBuffer->Des());

    // Parse the results
    RDesReadStream resultStream(searchResultsBuffer->Des());
    resultStream.PushL();

    // Number of results
    TInt seqCount = resultStream.ReadUint16L();

    PRINT1 ( _L("CPSRequestHandler::LookupL: Number of search seqs received = %d"), seqCount );

    // Internalize each char seq
    for (int i = 0; i < seqCount; i++)
        {
        // Size of sequence
        TInt szSeq = resultStream.ReadUint16L();

        // Character sequence
        HBufC* seq = HBufC::NewLC(resultStream, szSeq);

        aMatchSet.AppendL(seq->Des());

        CleanupStack::PopAndDestroy();
        }

    // --------- match sequence location ------------------    
    // number of results
    TInt count = resultStream.ReadUint16L();

    // read each data struct
    for (TInt k = 0; k < count; k++)
        {
        TPsMatchLocation matchLoc;

        matchLoc.index = resultStream.ReadUint16L();
        matchLoc.length = resultStream.ReadUint16L();
        matchLoc.direction  = (TBidiText::TDirectionality) (resultStream.ReadUint16L());

        aMatchLocation.Append(matchLoc);
        }

    //cleanup
    CleanupStack::PopAndDestroy(4, searchQueryBuffer); 
    // resultStream, searchResultsBuffer, searchDataBuffer, searchQueryBuffer
	
    PRINT1        ( _L("CPSRequestHandler::LookupL: Search Data: %S"), &aSearchData );
    PRINTMATCHSET ( _L("CPSRequestHandler::LookupL: "), aMatchSet );
    PRINTMATCHLOC ( _L("CPSRequestHandler::LookupL: "), aMatchLocation );
    PRINT ( _L("End CPSRequestHandler::LookupL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::LookupMatchL()
// Sends a request to the predictive search engine to perform a search.
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::LookupMatchL( const CPsQuery& aSearchQuery, 
                                               const TDesC& aSearchData,
                                               TDes& aMatch )
    {
    PRINT ( _L("Enter CPSRequestHandler::LookupMatchL") );

    // ----------------------- Search Query ----------------------

    // Tmp buffer        
    CBufFlat *buf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buf);

    // Stream over the temp buffer
    RBufWriteStream stream(*buf);
    stream.PushL();

    // Write the search query to the stream
    aSearchQuery.ExternalizeL(stream);
    stream.CommitL();

    // Create a HBufC8 for IPC
    HBufC8* searchQueryBuffer = HBufC8::NewL(buf->Size());
    TPtr8 ptr(searchQueryBuffer->Des());
    buf->Read(0, ptr, buf->Size());
    
    CleanupStack::PopAndDestroy(2, buf); // buf, stream
    CleanupStack::PushL(searchQueryBuffer);

    // ----------------------- Search Data ----------------------

    // Tmp buffer        
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);

    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();

    // Write the search data to the stream
    dataStream.WriteUint16L(aSearchData.Size());
    dataStream << aSearchData;

    // Create a HBufC8 for IPC
    HBufC8* searchDataBuffer = HBufC8::NewL(dataBuf->Size());
    TPtr8 dataPtr(searchDataBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());

    CleanupStack::PopAndDestroy(2, dataBuf); // dataBuf, dataStream
    CleanupStack::PushL(searchDataBuffer);      

    // Initiate the search
    iSession.SearchMatchStringL( 
            searchQueryBuffer->Des(), searchDataBuffer->Des(), aMatch );

    //cleanup
    CleanupStack::PopAndDestroy(searchDataBuffer);
    CleanupStack::PopAndDestroy(searchQueryBuffer); 
    
    PRINT1 ( _L("CPSRequestHandler::LookupMatchL: Search Data: %S"), &aSearchData );
    PRINT1 ( _L("CPSRequestHandler::LookupMatchL: Result Match: %S"), &aMatch );
    PRINT  ( _L("End CPSRequestHandler::LookupMatchL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::SearchL()
// Sends a request to the predictive search engine to perform a search.
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::SearchL(const CPsQuery& aSearchQuery,
                                         RPointerArray<CPsClientData>& aMarkedContacts,
                                         CVPbkContactManager* aContactManager)
    {
    PRINT ( _L("Enter CPSRequestHandler::SearchL") );

    // Fire the normal search with the input search query
    SearchL(aSearchQuery);

    // Clear the bookmarked contacts if any
    iMarkedContacts.Reset();

    // Copy the contact manager
    iBookMarkContactManager = aContactManager;

    // Copy the bookmarked contacts
    for (TInt i = 0; i < aMarkedContacts.Count(); i++)
        {
        iMarkedContacts.Append(aMarkedContacts[i]);
        }

    PRINT ( _L("End CPSRequestHandler::SearchL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::CancelSearch()
// Cancels ongoing search.
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::CancelSearch()
    {
    if( IsActive() )
        {
        iSearchRequestCancelled = ETrue;
        }
    
    if( iPendingSearchQueryBuffer )
        {
        delete iPendingSearchQueryBuffer;
        iPendingSearchQueryBuffer = NULL;
        }
    
    iSession.CancelSearch();
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::HandleSearchResultsL()
// Search is complete. Parse and send the results to the client.
// -----------------------------------------------------------------------------
void CPSRequestHandler::HandleSearchResultsL()
    {
    PRINT ( _L("Enter CPSRequestHandler::HandleSearchResultsL") );

    // No pending search query, process the search result
    RPointerArray<CPsClientData> searchResults;
    RPointerArray<CPsPattern> searchSeqs;

    if( !iSearchResultsBuffer )
        {
        return;
        }
    RDesReadStream stream(iSearchResultsBuffer->Des());
    stream.PushL();

    // Number of results
    TInt count = stream.ReadInt32L();

    PRINT1 ( _L("CPSRequestHandler::HandleSearchResultsL: Number of search results received = %d"), count );

    // Internalize each data element
    for (TInt i = 0; i < count; i++)
        {
        CPsClientData* data = CPsClientData::NewL();
        CleanupStack::PushL(data);

        data->InternalizeL(stream);

        CleanupStack::Pop(data);
        searchResults.Append(data);
        }

    // Check if book marked contacts are there
    TInt nBookmark = 0;
    if (iMarkedContacts.Count() != 0)
        {
        nBookmark = AddMarkedContactsL(searchResults);
        }
    
#ifdef _DEBUG
    for ( TInt i = 0; i < searchResults.Count(); i++ )
        {
        for ( TInt j = 0; j < searchResults[i]->DataElementCount(); j++ )
            {
            if ( searchResults[i]->Data(j) )
                {
                PRINT3 ( _L("CPSRequestHandler::HandleSearchResultsL: Results[%d,%d] = %S"),
                         i, j, &*(searchResults[i]->Data(j)) );
                }       
            }
        }    
#endif // _DEBUG

    // Read number of character sequences
    TInt seqCount = stream.ReadInt32L();

    PRINT1 ( _L("CPSRequestHandler::HandleSearchResultsL: Number of match sequences received = %d"), seqCount );

    // Internalize each char seq
    for (TInt i = 0; i < seqCount; i++)
        {
        CPsPattern* pattern = CPsPattern::NewL();
        CleanupStack::PushL(pattern);

        pattern->InternalizeL(stream);

        CleanupStack::Pop();
        if (pattern->FirstIndex() >= 0) // -1 means that is a Alphabetical sort
            {
            pattern->SetFirstIndex(pattern->FirstIndex() + nBookmark);
            }
        searchSeqs.Append(pattern);
        }

    CleanupStack::PopAndDestroy( &stream );

    // Pass the results to the observer
    for (TInt i = 0; i < iObservers.Count(); i++)
        {
        iObservers[i]->HandlePsResultsUpdate(searchResults, searchSeqs);
        }

    // Clear all the internal buffers
    searchResults.ResetAndDestroy();
    searchSeqs.ResetAndDestroy();
    delete iSearchQueryBuffer;
    iSearchQueryBuffer = NULL;
    delete iSearchResultsBuffer;
    iSearchResultsBuffer = NULL;

    PRINT ( _L("End CPSRequestHandler::HandleSearchResultsL") );

    // __LATENCY_MARKEND ( _L("CPSRequestHandler::HandleSearchResultsL  <--- SEARCH COMPLETE") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::RunSearchFromBufferL()
// Runs search if search query buffer ( iPendingSearchQueryBuffer ) is not empty
// -----------------------------------------------------------------------------
void CPSRequestHandler::RunSearchFromBufferL()
    {
    PRINT ( _L("Enter CPSRequestHandler::RunSearchFromBufferL") );

    if( iPendingSearchQueryBuffer )
        {
        if (iSearchQueryBuffer)
            {
            delete iSearchQueryBuffer;
            iSearchQueryBuffer = NULL;
            }
        iSearchQueryBuffer = HBufC8::NewLC(iPendingSearchQueryBuffer->Size());
        CleanupStack::Pop();
        iSearchQueryBuffer->Des().Copy(iPendingSearchQueryBuffer->Des());

        delete iPendingSearchQueryBuffer;
        iPendingSearchQueryBuffer = NULL;

        // ------------------------- Results Buffer ---------------------

        // Create a buffer to store the search results.    
        if (iSearchResultsBuffer)
            {
            delete iSearchResultsBuffer;
            iSearchResultsBuffer = NULL;
            }
        iSearchResultsBuffer = HBufC8::NewL(KSearchResultsBufferLen);

        // --------------------------------------------------------------      

        // Initiate the search
        iSession.SearchL(iSearchQueryBuffer->Des(), iSearchResultsBuffer->Des(), iStatus);

        SetActive();
        }
   
    
    PRINT ( _L("End CPSRequestHandler::RunSearchFromBufferL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::AddBookMarkedContacts()
// Adds the necessary bookmark contacts to the search result set
// -----------------------------------------------------------------------------
TInt CPSRequestHandler::AddMarkedContactsL(RPointerArray<CPsClientData>& searchResults)
    {
    __LATENCY_MARK ( _L(" CPSRequestHandler::AddMarkedContactsL") );
    PRINT ( _L("Enter CPSRequestHandler::AddMarkedContactsL") );

    RPointerArray<CPsClientData> filteredBookMarkList; // Local array to store the filtered bookmark contacts
    TInt nBookmark = 0;

    for (TInt i = 0; i < iMarkedContacts.Count(); i++)
        {
        MVPbkContactLink* bookMarkLink = NULL;

        // See if this contact is already in the result set
        TInt id = iMarkedContacts[i]->Id();

        TInt j = 0;
        for (; j < searchResults.Count(); j++)
            {
            if (id > 0)
                {
                if (id == searchResults[j]->Id())
                    {
                    searchResults[j]->SetMark(); // Set the bookmark
                    break; // Found, go to the next bookmark contact
                    }
                else
                    {
                    continue;
                    }
                }
            else
                {
                if (searchResults[j]->Id() > 0)
                    {
                    continue;
                    }
                else
                    {
                    if (!bookMarkLink)
                        {
                        bookMarkLink = ConvertToVpbkLinkLC(*(iMarkedContacts[i]),
                                                           *iBookMarkContactManager);
                        CleanupStack::Pop(); //  bookMarkLink  
                        }

                    MVPbkContactLink* searchContactLink = ConvertToVpbkLinkLC(*(searchResults[j]),
                                                                              *iBookMarkContactManager);
                    CleanupStack::Pop(); // searchContactLink

                    if (bookMarkLink->IsSame(*searchContactLink))
                        {
                        searchResults[j]->SetMark(); // Set the bookmark
                        break; // Found, go to the next bookmark contact
                        }
                    else
                        continue;
                    }
                }
            }
        if (j == searchResults.Count()) // Not found in the result set, Add it
            {
            filteredBookMarkList.Append(iMarkedContacts[i]);
            }
        }

    // Create the new CPsClientData objects for each filtered results 
    // and prepend them to the actual result set
    // in reverse order so that they will be in sorted order among themselves

    nBookmark = filteredBookMarkList.Count();
    for (TInt i = filteredBookMarkList.Count() - 1; i >= 0; i--)
        {
        CPsClientData* temp = CPsClientData::NewL();
        temp->SetId(filteredBookMarkList[i]->Id());
        temp->SetUriL(filteredBookMarkList[i]->Uri()->Des());

        // Add the data fields
        for (TInt j = 0; j < filteredBookMarkList[i]->DataElementCount(); j++)
            {
            temp->SetDataL(j, filteredBookMarkList[i]->Data(j)->Des());
            }

        temp->SetDataExtensionL(filteredBookMarkList[i]->DataExtension());
        temp->SetMark(); // Set the bookmark

        searchResults.Insert(temp, 0);
        }

    // Free the bookmark arrays
    iMarkedContacts.Reset();
    filteredBookMarkList.Reset();

    PRINT ( _L("END CPSRequestHandler::AddMarkedContactsL") );
    __LATENCY_MARKEND ( _L(" CPSRequestHandler::AddMarkedContactsL") );
    return nBookmark;
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::HandleBufferOverFlowL()
// Handle the buffer overflow error
// -----------------------------------------------------------------------------
void CPSRequestHandler::HandleBufferOverFlowL()
    {
    PRINT ( _L("Enter CPSRequestHandler::HandleBufferOverFlowL") );

    // New buffer size is now stored in results buffer
    RDesReadStream stream(iSearchResultsBuffer->Des());
    stream.PushL();

    // Read the buffer size and create a new buffer
    TInt bufferSize = stream.ReadInt32L();

    CleanupStack::PopAndDestroy(); // stream

    // Delete and recreate the results buffer
    if (iSearchResultsBuffer)
        {
        delete iSearchResultsBuffer;
        iSearchResultsBuffer = NULL;
        }

    // Buffer created for the new size
    iSearchResultsBuffer = HBufC8::NewL(bufferSize);

    // Recover the search results 
    iSession.SendNewBufferL(iSearchResultsBuffer->Des(), iStatus);

    SetActive();

    PRINT ( _L("End CPSRequestHandler::HandleBufferOverFlowL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::RunL()
// Invoked to handle responses from the server.
// -----------------------------------------------------------------------------
void CPSRequestHandler::RunL()
    {
    switch (iStatus.Int())
        {
        case ESearchComplete:
            // The server has completed the request, signalled the client
            // thread and the clients active scheduler runs the active object.
            // Now send the search results over the observer interface.
            // If request was canceled we will do search from buffer.
            if( iSearchRequestCancelled )
                {
                RunSearchFromBufferL();
                }
            else
                {
                HandleSearchResultsL();
                }
            break;
        
        case KErrOverflow:
            // Internal buffer used for IPC is in-adequate as the match results
            // are more. Send a new buffer to recover the results.
            // If request was canceled we will do search from buffer.
            if( iSearchRequestCancelled )
                {
                RunSearchFromBufferL();
                }
            else
                {
                HandleBufferOverFlowL();
                }
            break;
        
        case ECancelComplete:
        case KErrCancel:
            // The search request was canceled
            if( iPendingSearchQueryBuffer )
                {
                RunSearchFromBufferL();
                }
            break;

        default:
            // Errors
            HandleErrorL(iStatus.Int());
        }
    iSearchRequestCancelled = EFalse;
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::HandleErrorL()
// Send the error code to the client.
// -----------------------------------------------------------------------------
void CPSRequestHandler::HandleErrorL(TInt aErrorCode)
    {
    for (int i = 0; i < iObservers.Count(); i++)
        {
        iObservers[i]->HandlePsError(aErrorCode);
        }
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
void CPSRequestHandler::DoCancel()
    {
    iSession.CancelSearch();
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::Version()
// Recovers the predictive search server version.
// -----------------------------------------------------------------------------
EXPORT_C TVersion CPSRequestHandler::Version() const
    {
    return (iSession.Version());
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::ShutdownServerL()
// Shuts down the predictive search engine.
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::ShutdownServerL()
    {
    return (iSession.ShutdownServerL());
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::IsLanguageSupportedL()
// Checks if the language variant is supported by 
// the predictive search engine.
// -----------------------------------------------------------------------------
EXPORT_C TBool CPSRequestHandler::IsLanguageSupportedL(const TLanguage aLanguage)
    {
    PRINT ( _L("Enter CPSRequestHandler::IsLanguageSupportedL") );

    // ----------------------- language id----------------------

    // Tmp buffer        
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);

    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();

    // Write the language id to the stream
    dataStream.WriteUint32L(aLanguage);

    // Create a HBufC8 for IPC
    if (iSearchDataBuffer)
        {
        delete iSearchDataBuffer;
        iSearchDataBuffer = NULL;
        }

    iSearchDataBuffer = HBufC8::NewLC(dataBuf->Size());
    CleanupStack::Pop(); // iSearchDataBuffer
    TPtr8 dataPtr(iSearchDataBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());

    CleanupStack::PopAndDestroy(2, dataBuf); // dataBuf, dataStream

    // ------------------------- Results Buffer ---------------------

    // Create a buffer to store the search results.    
    if (iSearchResultsBuffer)
        {
        delete iSearchResultsBuffer;
        iSearchResultsBuffer = NULL;
        }
    iSearchResultsBuffer = HBufC8::NewL(KBufferMaxLen);

    // -------------------------------------------------------------- 
    // Send the request
    iSession.IsLanguageSupportedL(iSearchDataBuffer->Des(), iSearchResultsBuffer->Des());

    // parse the result
    RDesReadStream resultStream(iSearchResultsBuffer->Des());
    resultStream.PushL();

    // result ETrue or EFalse
    TBool flag = resultStream.ReadUint8L();

    CleanupStack::PopAndDestroy(); // resultStream

    PRINT ( _L("End CPSRequestHandler::IsLanguageSupportedL") );
    return flag;
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::GetCachingStatusL()
// Gets the status of the caching synchronously
// -----------------------------------------------------------------------------
EXPORT_C TInt CPSRequestHandler::GetCachingStatusL(TCachingStatus& aStatus)
    {
    PRINT ( _L("Enter CPSRequestHandler::GetCachingStatusL") );

    // Get the status from property handler
    // No need to send the request to engine
    TInt cacheError = iPropertyHandler->GetCachingStatusL(aStatus);

    PRINT ( _L("End CPSRequestHandler::GetCachingStatusL") );
    return cacheError;
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::ConvertToVpbkLinkLC()
// Get the contact link associated with the search result
// -----------------------------------------------------------------------------
EXPORT_C MVPbkContactLink* CPSRequestHandler::ConvertToVpbkLinkLC(const CPsClientData& aPsData, 
                                                                  CVPbkContactManager& aContactManager)
    {
    PRINT ( _L("Enter CPSRequestHandler::ConvertToVpbkLinkLC") );

    MVPbkContactLink* contactLink = NULL;

    // Get the originating uri for this result    
    HBufC* URI = aPsData.Uri();

    // Leave if not in contacts domain
    if (URI->CompareC(KVPbkDefaultCntDbURI) != 0 && URI->CompareC(KVPbkSimGlobalAdnURI) != 0 \
            && URI->CompareC(KVPbkSimGlobalFdnURI) != 0 && URI->CompareC(KVPbkSimGlobalSdnURI) != 0
            && URI->CompareC(KVPbkDefaultGrpDbURI) != 0)
        {
        User::Leave(KErrNotSupported);
        }

    // Create converter instance if not already available	
    if (iConverter == NULL)
        {
        // Get the store corresponding to the URI
        MVPbkContactStoreList& storeList = aContactManager.ContactStoresL();
        TVPbkContactStoreUriPtr uriPtr(KVPbkDefaultCntDbURI);
        MVPbkContactStore* store = storeList.Find(uriPtr);

        // Create converter 			
        if (store)
            {
            iConverter = CVPbkContactIdConverter::NewL(*store);
            }
        }

    if (aPsData.Id() < 0) // ID is negative for contacts in SIM domain
        {
        // Contact link is already packed for SIM contacts in CPsClientData
        HBufC8* linkBuf = (HBufC8*) aPsData.DataExtension();

        // ---------------- Convert HBufC8* to contact link ---------------------------	   	   				

        if (linkBuf->Length())
            {
            // Get the store corresponding to the URI
            MVPbkContactStoreList& storeList = aContactManager.ContactStoresL();

            // Link array
            CVPbkContactLinkArray* array = CVPbkContactLinkArray::NewLC(*(linkBuf), storeList);

            TInt count = array->Count();

            if (count == 1) // should contain only single contact link
                {
                const MVPbkContactLink& link = array->At(0);
                contactLink = link.CloneLC();
                CleanupStack::Pop(); // contactLink
                }

            CleanupStack::PopAndDestroy(); // array	
            }

        // ---------------- End convert HBufC8* to contact link -----------------------
        }
    else
        {
        // cntdb domain
        if (iConverter)
            {
            contactLink = iConverter->IdentifierToLinkLC(aPsData.Id());
            CleanupStack::Pop();
            }
        }

    // Add to cleanup stack
    CleanupDeletePushL(contactLink);

    PRINT ( _L("End CPSRequestHandler::ConvertToVpbkLinkLC") );

    return contactLink;

    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::GetAllContentsL()
// Sends a request to the predictive search server to get all cached contents.
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::GetAllContentsL()
    {
    PRINT ( _L("Enter CPSRequestHandler::GetAllContentsL") );

    // Create a empty CPsQuery
    CPsQuery* query = CPsQuery::NewL();

    // Perform a search
    SearchL(*query);

    // Cleanup
    delete query;

    PRINT ( _L("Enter CPSRequestHandler::GetAllContentsL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::GetDataOrderL()
// 
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::GetDataOrderL(const TDesC& aURI,
        RArray<TInt>& aDataOrder)
    {
    PRINT ( _L("Enter CPSRequestHandler::GetDataOrderL") );

    // ----------------------- URI ----------------------

    // Tmp buffer        
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);

    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();

    // Write the URI details in the stream
    TInt length = aURI.Length();
    dataStream.WriteUint16L(length);
    dataStream << aURI;

    // Create a HBufC8 for IPC
    if (iSearchDataBuffer)
        {
        delete iSearchDataBuffer;
        iSearchDataBuffer = NULL;
        }

    iSearchDataBuffer = HBufC8::NewL(dataBuf->Size());
    TPtr8 dataPtr(iSearchDataBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());

    // ------------------------- Results Buffer ---------------------

    // Create a buffer to store the search results.    
    if (iSearchResultsBuffer)
        {
        delete iSearchResultsBuffer;
        iSearchResultsBuffer = NULL;
        }
    iSearchResultsBuffer = HBufC8::NewL(KBufferMaxLen);

    // --------------------------------------------------------------      

    // Send the request
    iSession.GetDataOrderL(iSearchDataBuffer->Des(), iSearchResultsBuffer->Des());

    // Parse the results
    RDesReadStream resultStream(iSearchResultsBuffer->Des());
    resultStream.PushL();

    // Number of fields
    TInt fieldCount = resultStream.ReadUint16L();

    // Fields
    for (TInt i = 0; i < fieldCount; i++)
        {
        TInt fieldId = resultStream.ReadUint16L();
        aDataOrder.Append(fieldId);
        }

    // Cleanup
    delete iSearchResultsBuffer;
    delete iSearchDataBuffer;
    CleanupStack::PopAndDestroy(3, dataBuf); // resultStream, dataStream, dataBuf
    iSearchResultsBuffer = NULL;
    iSearchDataBuffer = NULL;

    PRINT ( _L("End CPSRequestHandler::GetDataOrderL") );

    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::GetSortOrderL()
// 
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::GetSortOrderL(const TDesC& aURI, RArray<TInt>& aDataOrder)
    {
    PRINT ( _L("Enter CPSRequestHandler::GetSortOrderL") );

    // ----------------------- URI ----------------------

    // Tmp buffer        
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);

    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();

    // Write the URI details in the stream
    TInt length = aURI.Length();
    dataStream.WriteUint16L(length);
    dataStream << aURI;

    // Create a HBufC8 for IPC
    if (iSearchDataBuffer)
        {
        delete iSearchDataBuffer;
        iSearchDataBuffer = NULL;
        }

    iSearchDataBuffer = HBufC8::NewL(dataBuf->Size());
    TPtr8 dataPtr(iSearchDataBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());

    // ------------------------- Results Buffer ---------------------

    // Create a buffer to store the search results.    
    if (iSearchResultsBuffer)
        {
        delete iSearchResultsBuffer;
        iSearchResultsBuffer = NULL;
        }
    iSearchResultsBuffer = HBufC8::NewL(KBufferMaxLen);

    // --------------------------------------------------------------      

    // Send the request
    iSession.GetSortOrderL(iSearchDataBuffer->Des(), iSearchResultsBuffer->Des());

    // Parse the results
    RDesReadStream resultStream(iSearchResultsBuffer->Des());
    resultStream.PushL();

    // Number of fields
    TInt fieldCount = resultStream.ReadUint16L();

    // Fields
    for (int i = 0; i < fieldCount; i++)
        {
        TInt fieldId = resultStream.ReadUint16L();
        aDataOrder.Append(fieldId);
        }

    // Cleanup
    delete iSearchResultsBuffer;
    delete iSearchDataBuffer;
    CleanupStack::PopAndDestroy(3, dataBuf); // resultStream, dataStream, dataBuf
    iSearchResultsBuffer = NULL;
    iSearchDataBuffer = NULL;

    PRINT ( _L("End CPSRequestHandler::GetSortOrderL") );

    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::ChangeSortOrderL()
// 
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::ChangeSortOrderL(const TDesC& aURI, RArray<TInt>& aSortOrder)
    {
    PRINT ( _L("Enter CPSRequestHandler::ChangeSortOrderL") );

    // ----------------------- URI and Sort Order --------------------

    // Tmp buffer        
    CBufFlat* dataBuf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(dataBuf);

    // Stream over the temp buffer
    RBufWriteStream dataStream(*dataBuf);
    dataStream.PushL();

    // Write the URI details in the stream
    TInt length = aURI.Length();
    dataStream.WriteUint16L(length);
    dataStream << aURI;

    // Write the sort order
    dataStream.WriteUint16L(aSortOrder.Count());
    for (int i = 0; i < aSortOrder.Count(); i++)
        {
        dataStream.WriteUint16L(aSortOrder[i]);
        }

    // Create a HBufC8 for IPC
    if (iSearchDataBuffer)
        {
        delete iSearchDataBuffer;
        iSearchDataBuffer = NULL;
        }

    iSearchDataBuffer = HBufC8::NewL(dataBuf->Size());
    TPtr8 dataPtr(iSearchDataBuffer->Des());
    dataBuf->Read(0, dataPtr, dataBuf->Size());

    // Send the request
    iSession.ChangeSortOrderL(iSearchDataBuffer->Des());

    // Cleanup
    delete iSearchDataBuffer;
    CleanupStack::PopAndDestroy(2, dataBuf); // dataStream, dataBuf
    iSearchDataBuffer = NULL;

    PRINT ( _L("End CPSRequestHandler::ChangeSortOrderL") );
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::NotifyCachingStatus()
// 
// -----------------------------------------------------------------------------
void CPSRequestHandler::NotifyCachingStatus( TCachingStatus aStatus, TInt aError )
    {
    for ( TInt i = 0; i < iObservers.Count(); i++ )
        {
        iObservers[i]->CachingStatus(aStatus, aError);
        }
    }

// -----------------------------------------------------------------------------
// CPSRequestHandler::GetAdaptiveGridCharactersL()
// 
// -----------------------------------------------------------------------------
EXPORT_C void CPSRequestHandler::GetAdaptiveGridCharactersL( const MDesCArray& aDataStores,
                                                             const TDesC& aSearchText,
                                                             const TBool aCompanyName,
                                                             TDes& aAdaptiveGrid )
    {
    /*
     * aSearchText is unused in the current implementation.
     * If/when this API method will support grids at bigger level it will be used.
     * Grids at bigger level can be kept in a multilevel Adaptive Grid cache, or could
     * be calculated inside PCS Engine. There are two implementation choices.
     */
    
    PRINT ( _L("Enter CPSRequestHandler::GetAdaptiveGridCharactersL") );

    if ( aSearchText.Length() > KPsAdaptiveGridSupportedMaxLen )
        {
        PRINT1 ( _L("CPSRequestHandler::GetAdaptiveGridCharactersL: Too many chars in search text, Max supported is %d"),
		         KPsAdaptiveGridSupportedMaxLen );
        }
    else
	    {    
        // -------------------- Data Stores --------------------
    
        // Temp buffer
        CBufFlat* dataBuffer = CBufFlat::NewL( KBufferMaxLen );
        CleanupStack::PushL( dataBuffer );
    
        // Stream over the temp buffer
        RBufWriteStream dataStream( *dataBuffer );
        dataStream.PushL();
    
        // Write the URI count in the stream
        TInt dataStoresCount = aDataStores.MdcaCount();
        dataStream.WriteUint16L( dataStoresCount );
    
        for ( TUint i=0; i < dataStoresCount; i++ )
            {
            // Write the URI details in the stream
            TPtrC16 uri = aDataStores.MdcaPoint(i);
            dataStream.WriteUint16L( uri.Length() );
            dataStream << uri;
            }

        // Create a HBufC8 for IPC
        HBufC8* storesDataBuffer = HBufC8::NewL( dataBuffer->Size() );
        TPtr8 storesDataBufferPtr( storesDataBuffer->Des() );
        dataBuffer->Read( 0, storesDataBufferPtr, dataBuffer->Size() );

        CleanupStack::PopAndDestroy( 2, dataBuffer ); // dataBuffer, dataStream
        CleanupStack::PushL( storesDataBuffer );

        // -----------------------------------------------------
    
        iSession.GetAdaptiveGridL( storesDataBuffer->Des(), aCompanyName, aAdaptiveGrid );
    
        // Cleanup
        CleanupStack::PopAndDestroy(storesDataBuffer);
        }
	
#ifdef _DEBUG
    for ( TUint i=0; i < aDataStores.MdcaCount(); i++ )
        {
        TPtrC16 ptr = aDataStores.MdcaPoint(i);
        PRINT2 ( _L("CPSRequestHandler::GetAdaptiveGridCharactersL: Data Store [%d]: %S"),
                 i, &ptr );
        }
    PRINT1 ( _L("CPSRequestHandler::GetAdaptiveGridCharactersL: Search Text: %S"), &aSearchText );
    PRINT1 ( _L("CPSRequestHandler::GetAdaptiveGridCharactersL: Company Name: %d"), aCompanyName );
    PRINT1 ( _L("CPSRequestHandler::GetAdaptiveGridCharactersL: Grid: %S"), &aAdaptiveGrid );
#endif // _DEBUG

    PRINT ( _L("End CPSRequestHandler::GetAdaptiveGridCharactersL") );
    }

// End of File
