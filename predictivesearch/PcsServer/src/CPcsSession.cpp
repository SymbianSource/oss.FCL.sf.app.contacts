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
* Description:  PCS Session class
*
*/

// INCLUDE FILES
#include <e32cmn.h>
#include <s32mem.h>
#include <e32debug.h>
#include <utf.h>

#include "CPcsDefs.h"
#include "CPcsServer.h"
#include "CPcsSession.h"
#include "CPsClientData.h"
#include "CPsQuery.h"
#include "CPcsPluginInterface.h"
#include "CPcsDebug.h"

// ============================== MEMBER FUNCTIONS ============================

// ----------------------------------------------------------------------------
// CPcsSession::NewL
// Two Phase Construction
// ----------------------------------------------------------------------------
CPcsSession* CPcsSession::NewL(CPcsServer* aServer)
{
    PRINT ( _L("Enter CPcsSession::NewL") );

    CPcsSession* self= new (ELeave) CPcsSession(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self

    PRINT ( _L("End CPcsSession::NewL") );

    return self;
}

// ----------------------------------------------------------------------------
// CPcsSession::CPcsSession
// Construtor
// ----------------------------------------------------------------------------
CPcsSession::CPcsSession( CPcsServer* aServer ) :iServer(aServer)
{
	PRINT ( _L("Enter CPcsSession::CPcsSession") );
	PRINT ( _L("End CPcsSession::CPcsSession") );
}

// ----------------------------------------------------------------------------
// CPcsSession::ConstructL
// Second phase constructor
// ----------------------------------------------------------------------------
void CPcsSession::ConstructL()
{
	PRINT ( _L("Enter CPcsSession::ConstructL") );

	iBufferOverFlow = EFalse;

	iSettings = CPsSettings::NewL();

	PRINT ( _L("End CPcsSession::ConstructL") );
}

// ----------------------------------------------------------------------------
// CPcsSession::CPcsSession
// Destructor
// ----------------------------------------------------------------------------
CPcsSession::~CPcsSession()
{
	PRINT ( _L("Enter CPcsSession::~CPcsSession") );

	if ( iSettings )
	{
		delete iSettings;
		iSettings = NULL;
	}
	
	delete iDes;

	PRINT ( _L("End CPcsSession::~CPcsSession") );
}

// ----------------------------------------------------------------------------
// CPcsSession::ServiceL
//
// ----------------------------------------------------------------------------
void CPcsSession::ServiceL(const RMessage2& aMessage)
{
	PRINT ( _L("Enter CPcsSession::ServiceL") );

	TInt errStatus = KErrNone;

	// Do the service
	TRAP ( errStatus, DoServiceL(aMessage) );

    // Check the error status returned
    if ( errStatus != KErrNone )
    {
    	aMessage.Complete(errStatus);
    }

    PRINT ( _L("End CPcsSession::ServiceL") );
}

// ----------------------------------------------------------------------------
// CPcsSession::DoServiceL
//
// ----------------------------------------------------------------------------
void CPcsSession::DoServiceL(const RMessage2& aMessage)
{
	PRINT ( _L("Enter CPcsSession::DoServiceL") );

    switch ( aMessage.Function() )
    {
    	case ESearchSettings:
    	    PRINT ( _L("Received function ESearchSettings") );
    	    SetSearchSettingsL(aMessage);
    	    break;

    	case ESearch:
    	    PRINT ( _L("Received function ESearch") );
     	    GetAsyncPcsResultsL(aMessage);
    	    break;

    	case ECancelSearch:
    	    PRINT ( _L("Received function ECancelSearch") );
    	    iBufferOverFlow = EFalse;
            delete iDes;
            iDes = NULL;
    	    aMessage.Complete(ECancelComplete);
    	    break;

    	case ESearchInput:
	    	PRINT ( _L("Received function ESearchInput") );
     	    SearchInputL(aMessage);
    	    break;
    	    
        case ESearchMatchString:
            PRINT ( _L("Received function ESearchMatchString") );
            SearchMatchStringL(aMessage);
            break;    	    

    	case ELangSupport:
			PRINT ( _L("Received function ELangSupport") );
			IsLanguageSupportedL(aMessage);
			break;

    	case EGetDataOrder:
			PRINT ( _L("Received function EGetDataOrder") );
			GetDataOrderL(aMessage);
			break;

    	case EGetSortOrder:
			PRINT ( _L("Received function EGetSortOrder") );
			GetSortOrderL(aMessage);
			break;

        case ESetSortOrder:
			PRINT ( _L("Received function ESetSortOrder") );
			ChangeSortOrderL(aMessage);
			break;

	    case EShutdown:
		    PRINT ( _L("Received function EShutdown") );
			ShutdownServerL(aMessage);
			break;
    }

	PRINT ( _L("End CPcsSession::DoServiceL") );

	return;
}
// ----------------------------------------------------------------------------
// CPcsSession::ServiceError
//
// ----------------------------------------------------------------------------
void CPcsSession::ServiceError(const RMessage2& aMessage, TInt aError)
{
	PRINT ( _L("Enter CPcsSession::ServiceError") );

	aMessage.Complete(aError);

	PRINT ( _L("End CPcsSession::ServiceError") );
}


// ----------------------------------------------------------------------------
// CPcsSession::SetSearchSettings
//
// ----------------------------------------------------------------------------
void CPcsSession::SetSearchSettingsL(const RMessage2& aMessage)
{
	PRINT ( _L("Enter CPcsSession::SetSearchSettingsL") );

	// Read search settings from the message
	HBufC8* searchSettings = HBufC8::NewLC(KBufferMaxLen);

	TPtr8 searchSettingsPtr(searchSettings->Des());
	aMessage.ReadL(0, searchSettingsPtr);

	// Stream over the buffer
	RDesReadStream searchSettingsStream(searchSettingsPtr);
	searchSettingsStream.PushL();

    if ( iSettings )
    {
    	delete iSettings;
    	iSettings = NULL;
    }

    iSettings = CPsSettings::NewL();

	iSettings->InternalizeL(searchSettingsStream);

	// searchSettingsStream, searchSettings
	CleanupStack::PopAndDestroy(2, searchSettings);

	aMessage.Complete(KErrNone);

	PRINT ( _L("End CPcsSession::SetSearchSettingsL") );
}

// ----------------------------------------------------------------------------
// CPcsSession::GetAsyncPcsResultsL
//
// ----------------------------------------------------------------------------
void CPcsSession::GetAsyncPcsResultsL(const RMessage2& aMessage)
{
    __LATENCY_MARK ( _L("CPcsSession::GetAsyncPcsResultsL") );
    PRINT ( _L("Enter CPcsSession::GetAsyncPcsResultsL") );

    if ( iBufferOverFlow == EFalse )
    {
        // -------------------------------------------------------------

	    // Read search query from the message
	    HBufC8* searchQuery = HBufC8::NewLC(KBufferMaxLen);

	    TPtr8 searchQueryPtr(searchQuery->Des());
        aMessage.ReadL(0, searchQueryPtr);

	    // Stream over the buffer
	    RDesReadStream searchQueryStream(searchQueryPtr);
	    searchQueryStream.PushL();

	    // Query object
	    CPsQuery* psQuery = CPsQuery::NewL();

	    psQuery->InternalizeL(searchQueryStream);

	    // searchQueryStream, searchQuery
	    CleanupStack::PopAndDestroy(2, searchQuery);

	    // -------------------------------------------------------------

		// Dynamic data buffer
	    CBufFlat* buf = CBufFlat::NewL(KBufferMaxLen);
	    CleanupStack::PushL(buf);

	    // Stream over the buffer
	    RBufWriteStream stream(*buf);
	    stream.PushL();

        RPointerArray<CPsClientData>  searchResults;
	    RPointerArray<CPsPattern>     searchSeqs;


		iServer->PluginInterface()->PerformSearchL(*iSettings,
		                                           *psQuery,
		                                           searchResults,
		                                           searchSeqs);

        // Write the number of contacts
	    stream.WriteInt32L(searchResults.Count());

	    // Write the contacts
	    for ( int i = 0; i < searchResults.Count(); i++ )
	    {
	        searchResults[i]->ExternalizeL(stream);
	    }

	    // Write the number of char seqs
	    stream.WriteInt32L(searchSeqs.Count());

	    // Write the seqs
	    for ( int j = 0; j < searchSeqs.Count(); j++ )
	    {
	        searchSeqs[j]->ExternalizeL(stream);
	    }

	    // Cleanup
	    searchResults.ResetAndDestroy();
	    searchSeqs.ResetAndDestroy();
	    delete psQuery;

        // Results are already packed in the stream
        stream.CommitL();

        // Create a heap descriptor from the buffer
	    iDes = HBufC8::NewL(buf->Size());
	    TPtr8 ptr(iDes->Des());
	    buf->Read(0, ptr, buf->Size());

	    CleanupStack::PopAndDestroy(2, buf); // buf, stream
    }

    TInt rcevdBufferSize = aMessage.GetDesMaxLength(1);
    User::LeaveIfNull( iDes );
    TInt reqdBufferSize  = iDes->Size();

    PRINT1 ( _L("Received buffer size = %d"), rcevdBufferSize );
    PRINT1 ( _L("Required buffer size = %d"), reqdBufferSize );

    // If the received buffer size from Client API is less than
    // the required buffer size write the required buffer size
    // and return.
    if ( rcevdBufferSize < reqdBufferSize )
    {
        PRINT ( _L("In-adequate buffer received") );
        PRINT ( _L("Packing the required buffer size in response") );

    	TPckgC<TInt> bufferSizePackage(reqdBufferSize);
    	aMessage.WriteL(1, bufferSizePackage);
    	aMessage.Complete(KErrOverflow);

    	iBufferOverFlow = ETrue;
    }
    else
    {
        PRINT ( _L("Adequate buffer received") );
        PRINT ( _L("Packing the results in response") );

		aMessage.Write(1, *iDes);
		aMessage.Complete(ESearchComplete);

		iBufferOverFlow = EFalse;
    	delete iDes;
        iDes = NULL;
    }


    PRINT ( _L("End CPcsSession::GetAsyncPcsResultsL") );

    __LATENCY_MARKEND ( _L("CPcsSession::GetAsyncPcsResultsL") );

}

// ----------------------------------------------------------------------------
// CPcsSession::SearchInputL
//
// ----------------------------------------------------------------------------
void CPcsSession::SearchInputL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CPcsSession::SearchInputL") );
    __LATENCY_MARK ( _L("CPcsSession::SearchInputL") );

    // -------------------------------------------------------------

    // Read search query from the message
    HBufC8* searchQuery = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 searchQueryPtr(searchQuery->Des());
    aMessage.ReadL(0, searchQueryPtr);

    // Stream over the buffer
    RDesReadStream searchQueryStream(searchQueryPtr);
    searchQueryStream.PushL();

    // Query object
    CPsQuery* psQuery = CPsQuery::NewL();
    psQuery->InternalizeL(searchQueryStream);

    // searchQueryStream, searchQuery
    CleanupStack::PopAndDestroy(2, searchQuery);

    // -------------------------------------------------------------

    // Read search data from the message
    HBufC8* searchData = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 searchDataPtr(searchData->Des());
    aMessage.ReadL(1, searchDataPtr);

    // Stream over the buffer
    RDesReadStream searchDataStream(searchDataPtr);
    searchDataStream.PushL();

    // Data to be searched
    // Read data size
	TUint16 szData = searchDataStream.ReadUint16L();
    HBufC* data =  HBufC::NewL(searchDataStream, szData);

    // searchQueryStream, searchQuery
    CleanupStack::PopAndDestroy(2, searchData);

    // -------------------------------------------------------------

	// Dynamic data buffer
    CBufFlat* buf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buf);

    // Stream over the buffer
    RBufWriteStream stream(*buf);
    stream.PushL();

    // To hold the matches
    RPointerArray<TDesC> searchSeqs;

    // To hold matched location
    RArray<TPsMatchLocation> sequenceLoc;


	iServer->PluginInterface()->SearchInputL(*psQuery,
	                                         *data,
	                                         searchSeqs,
	                                         sequenceLoc);


    // Delete the search query and search data
    delete psQuery;
    delete data;

    // --------- write match sequence ---------------------------
    // Write the number of match seqs
    stream.WriteUint16L(searchSeqs.Count());

    // Write the matches
    for ( int j = 0; j < searchSeqs.Count(); j++ )
    {
        TInt length = searchSeqs[j]->Length();

        // Write the char sequence length
        stream.WriteUint16L(length);

        // Write the char sequence
    	stream << *searchSeqs[j];
    }

    // --------- write match sequence location ------------------
    // write number of sequnece
    stream.WriteUint16L(sequenceLoc.Count());

    // write each structure element
    for(TInt k = 0; k < sequenceLoc.Count(); k++)
    {
    	TPsMatchLocation temp = sequenceLoc[k];

    	// write the starting index
    	stream.WriteUint16L(temp.index);

    	// write the length of match
    	stream.WriteUint16L(temp.length);

    	//write directionality of string
    	stream.WriteUint16L(temp.direction);

    }

    // Reset search seqs
    searchSeqs.ResetAndDestroy();
    sequenceLoc.Reset();
    sequenceLoc.Close();

    stream.CommitL();

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewLC(buf->Size());
    TPtr8 ptr(des->Des());
    buf->Read(0, ptr, buf->Size());

	aMessage.Write(2, *des);
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(3, buf); // buf, stream, des
	
    PRINT ( _L("End CPcsSession::SearchInputL") );
    __LATENCY_MARKEND ( _L("CPcsSession::SearchInputL") );

}

// ----------------------------------------------------------------------------
// CPcsSession::SearchMatchStringL
//
// ----------------------------------------------------------------------------
void CPcsSession::SearchMatchStringL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CPcsSession::SearchMatchStringL") );
    __LATENCY_MARK ( _L("CPcsSession::SearchMatchStringL") );

    // -------------------------------------------------------------

    // Read search query from the message
    HBufC8* searchQuery = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 searchQueryPtr(searchQuery->Des());
    aMessage.ReadL(0, searchQueryPtr);

    // Stream over the buffer
    RDesReadStream searchQueryStream(searchQueryPtr);
    searchQueryStream.PushL();

    // Query object
    CPsQuery* psQuery = CPsQuery::NewL();
    psQuery->InternalizeL(searchQueryStream);

    // searchQueryStream, searchQuery
    CleanupStack::PopAndDestroy(2, searchQuery);

    // -------------------------------------------------------------

    // Read search data from the message
    HBufC8* searchData = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 searchDataPtr(searchData->Des());
    aMessage.ReadL(1, searchDataPtr);

    // Stream over the buffer
    RDesReadStream searchDataStream(searchDataPtr);
    searchDataStream.PushL();

    // Data to be searched
    // Read data size
    TUint16 szData = searchDataStream.ReadUint16L();
    HBufC* data =  HBufC::NewL(searchDataStream, szData);

    // searchQueryStream, searchQuery
    CleanupStack::PopAndDestroy(2, searchData);

    // -------------------------------------------------------------

    // Dynamic data buffer
    CBufFlat* buf = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buf);

    // Create a heap descriptor from the buffer
    HBufC* des = HBufC::NewLC(KBufferMaxLen);
    TPtr ptr(des->Des());
    
    iServer->PluginInterface()->SearchMatchStringL(*psQuery,
                                             *data,
                                             ptr);


    // Delete the search query and search data
    delete psQuery;
    delete data;

    aMessage.Write(2, *des);
    aMessage.Complete(KErrNone);
    CleanupStack::PopAndDestroy(2, buf); // buf, des
    
    PRINT ( _L("End CPcsSession::SearchMatchStringL") );
    __LATENCY_MARKEND ( _L("CPcsSession::SearchMatchStringL") );

}

// ----------------------------------------------------------------------------
// CPcsSession::IsLanguageSupportedL
//
// ----------------------------------------------------------------------------
void CPcsSession::IsLanguageSupportedL(const RMessage2& aMessage)
{
	PRINT ( _L("Enter CPcsSession::IsLanguageSupportedL") );

	// --------------------------------------------------------------

    // Read TLanguage from the message
    HBufC8* buffer = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 bufferPtr(buffer->Des());
    aMessage.ReadL(0, bufferPtr);

    // Stream over the buffer
    RDesReadStream stream(bufferPtr);
    stream.PushL();

    // Read language ID
   	TUint32 languageId = stream.ReadUint32L();

    // stream, buffer
    CleanupStack::PopAndDestroy(2, buffer);

    // --------------------------------------------------------------

    // Get is language supported from the algorithm
    TBool flag = iServer->PluginInterface()->IsLanguageSupportedL(languageId);

    // --------------------------------------------------------------
    // Dynamic data buffer
    CBufFlat* buffer1 = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buffer1);

    // Stream over the buffer
    RBufWriteStream stream1(*buffer1);
    stream1.PushL();

    // Pack the result back in response
	stream1.WriteUint8L(flag);

    stream1.CommitL();

    // --------------------------------------------------------------

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewLC(buffer1->Size());
    TPtr8 ptr(des->Des());
    buffer1->Read(0, ptr, buffer1->Size());

	aMessage.Write(1, *des);
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(3, buffer1); // buffer1, stream1, des

	PRINT ( _L("End CPcsSession::IsLanguageSupportedL") );
}


// ----------------------------------------------------------------------------
// CPcsSession::GetDataOrderL
//
// ----------------------------------------------------------------------------
void CPcsSession::GetDataOrderL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CPcsSession::GetDataOrderL") );

    // --------------------------------------------------------------

    // Read URI from the message
    HBufC8* buffer = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 bufferPtr(buffer->Des());
    aMessage.ReadL(0, bufferPtr);

    // Stream over the buffer
    RDesReadStream stream(bufferPtr);
    stream.PushL();

	// Size of URI
	TInt uriSize = stream.ReadUint16L();

    // URI
    HBufC* uri = HBufC::NewLC(stream, uriSize);

    

    // --------------------------------------------------------------

    // Get the data order
    RArray<TInt> dataOrder;
    iServer->PluginInterface()->GetDataOrderL(*uri, dataOrder);
    
    // uri, stream, buffer
    CleanupStack::PopAndDestroy(3, buffer);
    
    CleanupClosePushL(dataOrder);
    
    // --------------------------------------------------------------

    // Dynamic data buffer
    CBufFlat* buffer1 = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buffer1);

    // Stream over the buffer
    RBufWriteStream stream1(*buffer1);
    stream1.PushL();

    // Pack the data order fields
	TInt fieldCount = dataOrder.Count();
	stream1.WriteUint16L(fieldCount);

	// Pack the fields
	for ( int i = 0; i < fieldCount; i++ )
	{
		stream1.WriteUint16L(dataOrder[i]);
	}

    stream1.CommitL();

    // --------------------------------------------------------------

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewLC(buffer1->Size());
    TPtr8 ptr(des->Des());
    buffer1->Read(0, ptr, buffer1->Size());

	aMessage.Write(1, *des);
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(4, &dataOrder); // des, stream1, buffer1, dataOrder

    PRINT ( _L("End CPcsSession::GetDataOrderL") );
}

// ----------------------------------------------------------------------------
// CPcsSession::GetSortOrderL
//
// ----------------------------------------------------------------------------
void CPcsSession::GetSortOrderL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CPcsSession::GetSortOrderL") );

    // --------------------------------------------------------------

    // Read URI from the message
    HBufC8* buffer = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 bufferPtr(buffer->Des());
    aMessage.ReadL(0, bufferPtr);

    // Stream over the buffer
    RDesReadStream stream(bufferPtr);
    stream.PushL();

	// Size of URI
	TInt uriSize = stream.ReadUint16L();

    // URI
    HBufC* uri = HBufC::NewLC(stream, uriSize);

    // --------------------------------------------------------------

    // Set the sort order
    RArray<TInt> sortOrder;
    iServer->PluginInterface()->GetSortOrderL(*uri, sortOrder);
    
    // uri, stream, buffer
    CleanupStack::PopAndDestroy(3, buffer);
    
    CleanupClosePushL( sortOrder );
    
    // --------------------------------------------------------------

    // Dynamic data buffer
    CBufFlat* buffer1 = CBufFlat::NewL(KBufferMaxLen);
    CleanupStack::PushL(buffer1);

    // Stream over the buffer
    RBufWriteStream stream1(*buffer1);
    stream1.PushL();

    // Pack the data order fields
	TInt fieldCount = sortOrder.Count();
	stream1.WriteUint16L(fieldCount);

	// Pack the fields
	for ( int i = 0; i < fieldCount; i++ )
	{
		stream1.WriteUint16L(sortOrder[i]);
	}

    stream1.CommitL();

    // --------------------------------------------------------------

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewLC(buffer1->Size());
    TPtr8 ptr(des->Des());
    buffer1->Read(0, ptr, buffer1->Size());

	aMessage.Write(1, *des);
	aMessage.Complete(KErrNone);
	CleanupStack::PopAndDestroy(4, &sortOrder); // des, buffer1, stream1, sortOrder

    PRINT ( _L("End CPcsSession::GetSortOrderL") );
}

// ----------------------------------------------------------------------------
// CPcsSession::ChangeSortOrderL
//
// ----------------------------------------------------------------------------
void CPcsSession::ChangeSortOrderL(const RMessage2& aMessage)
{
    PRINT ( _L("Enter CPcsSession::ChangeSortOrderL") );

    // --------------------------------------------------------------

    // Read URI from the message
    HBufC8* buffer = HBufC8::NewLC(KBufferMaxLen);

    TPtr8 bufferPtr(buffer->Des());
    aMessage.ReadL(0, bufferPtr);

    // Stream over the buffer
    RDesReadStream stream(bufferPtr);
    stream.PushL();

	// Size of URI
	TInt uriSize = stream.ReadUint16L();

    // URI
    HBufC* uri = HBufC::NewLC(stream, uriSize);
    CleanupStack::Pop(); // uri

    // Number of data fields
    TInt fieldCount = stream.ReadUint16L();
    RArray<TInt> sortOrder;

    for ( int i = 0; i < fieldCount; i++ )
    {
    	sortOrder.Append(stream.ReadUint16L());
    }

    // stream, buffer
    CleanupStack::PopAndDestroy(2, buffer);

    // --------------------------------------------------------------

    // Set the sort order
    iServer->PluginInterface()->ChangeSortOrderL(*uri, sortOrder);

    delete uri;
    sortOrder.Reset();

    // --------------------------------------------------------------

	aMessage.Complete(KErrNone);

    PRINT ( _L("End CPcsSession::ChangeSortOrderL") );
}

// ----------------------------------------------------------------------------
// CPcsSession::ShutdownServerL
//
// ----------------------------------------------------------------------------
void CPcsSession::ShutdownServerL(const RMessage2& aMessage)
{
	PRINT ( _L("Enter CPcsSession::ShutdownServerL") );

	aMessage.Complete(KErrNone);
	CActiveScheduler::Stop();

	PRINT ( _L("End CPcsSession::ShutdownServerL") );
}
