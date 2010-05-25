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
* Description:  This is the client side interface declaration for the 
*                predictive search server.
*
*/


#ifndef __CPSREQUESTHANDLER_H__
#define __CPSREQUESTHANDLER_H__

// INCLUDE FILES

// SYSTEM INCLUDES
#include <e32base.h>
#include <e32svr.h>
#include <s32mem.h>
#include <e32property.h>
#include <MVPbkContactLink.h>
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactIdConverter.h>
#include <VPbkContactStoreUris.h>
#include <badesca.h>

// USER INCLUDES
#include <CPcsDefs.h>
#include <CPsClientData.h>
#include <CPsQuery.h>
#include <CPsSettings.h>
#include <CPsPattern.h>
#include <RPsSession.h>

// FORWARD DECLARATIONS
class MPsResultsObserver;
class CPsPropertyHandler;
class CPsUpdateHandler;


// CLASS DECLARATION
/**
* CPSRequestHandler
* An instance of the request handler object for the Predicitve Search application
*
* @lib PsServerClientAPI.lib
* @since S60 v3.2
*/
class CPSRequestHandler : public CActive
{

	public: // Constructors and destructors

		/**
		* NewL.
		* Two-phased constructor.
		* Creates a CPSRequestHandler object using two phase construction,
		* and return a pointer to the created object.
		*
		* @return A pointer to the created instance of CPSRequestHandler.
		*/
		IMPORT_C static CPSRequestHandler* NewL();

		/**
		* NewLC.
		* Two-phased constructor.
		* Creates a CPSRequestHandler object using two phase construction,
		* and return a pointer to the created object.
		*	
		* @return A pointer to the created instance of CPSRequestHandler.
		*/
		IMPORT_C static CPSRequestHandler* NewLC();

		/**
		* ~CPSRequestHandler.
		* Destructor.
		* Destroys the object and release all memory objects.
		*/
		virtual ~CPSRequestHandler();

	public: // New functions

		/**
		* Version.
		* Returns the predictive search server version.
		*
		* @return The current version of predictive search server.
		*/
		IMPORT_C TVersion Version() const;		
		
		/**
		* AddObserver.
		*
		* @param aObserver The object to be used to handle updates 
		* from the server.
		*/
		IMPORT_C void AddObserverL ( MPsResultsObserver* aObserver );
		
		/**
		* RemoveObserverL.
		*
		* @param aObserver The object used to handle updates from the server.
		* @return KErrNone if removed.
		*         KErrNotFound if observer not added already.
		*/
		IMPORT_C TInt RemoveObserver ( MPsResultsObserver* aObserver );

		/**
		* SetSearchSettingsL.
		* Set the databases and fields to be included in search.
		* Databases ids are mapped to virtual phone book URLs.
		* Fields to be searched are mapped to virutal phonebook identifiers.
		*
		* @param aSettings Holds the search settings.
		*/
		IMPORT_C void SetSearchSettingsL(const CPsSettings& aSettings);

		/**
		* GetAllContentsL. 
		* Sends a request to the predictive search server to get all cached contents.
		* This is a asynchronous request.
		*/
		IMPORT_C void GetAllContentsL();

		/**
		* SearchL. 
		* Sends a request to the predictive search server.
		* This is a asynchronous request.
		* 
		* @param aSearchQuery The input search query.(Length of aSearchQuery <= KPsQueryMaxLen)
		* @param aStatus      Holds the completion status of search request.
		*/
		IMPORT_C void SearchL(const CPsQuery& aSearchQuery);
		
		/**
		* SearchL. 
		* Sends a request to the predictive search server.
		* This is a asynchronous request. It considers bookmarked contacts 
		* while sending the results back to the client
		* 
		* @param aSearchQuery 		The input search query.(Length of aSearchQuery <= KPsQueryMaxLen)
		* @param aMarkedContacts    Array of marked contacts
		* @param aContactManager	Contact manager instance
		*/
		IMPORT_C void SearchL(const CPsQuery& aSearchQuery, 
							  RPointerArray<CPsClientData>& aMarkedContacts,
							  CVPbkContactManager* aContactManager);
		
        /**
        * CancelSearch.
        * Cancels ongoing search.
        */
		IMPORT_C void CancelSearch();
		
		/**
		* LookupL. 
		* Sends a request to the predictive search server.
		* Does a predictive search in aSearchData for aSearchQuery and return
		* the matches.
		* This is a synchronous request.
		* 
		* @param aSearchQuery The input search query.(Length of aSearchQuery <= KPsQueryMaxLen)
		* @param aSearchData  The input data to be searched.
		* @param aMatchSet    The list of matches.
		* @param aMatchLocation The list matched index and length of match
		*/
		IMPORT_C void LookupL(const CPsQuery& aSearchQuery,
                              const TDesC& aSearchData,
                              CDesCArray& aMatchSet,
                              RArray<TPsMatchLocation>& aMatchLocation);

        /**
        * LookupMatchL. 
        * Sends a request to the predictive search server.
        * Does a predictive search in aSearchData for aSearchQuery and return
        * the match string in aMatch.
        * If there is no full match aMatch will be empty (Length()==0)
        * This is a synchronous request.
        * 
        * @param aSearchQuery The input search query.(Length of aSearchQuery <= KPsQueryMaxLen)
        * @param aSearchData  The input data to be searched.
        * @param aMatch       The matched result
        */
        IMPORT_C void LookupMatchL(const CPsQuery& aSearchQuery,
                                   const TDesC& aSearchData,
                                   TDes& aMatch);
		
		/**
		* IsLanguageSupportedL.
		* Checks if the language variant is supported by 
		* the predictive search engine.
		* This is a synchronous request.
		*
		* @param aLanguage The language which has to be checked
		* @return ETrue if the language specified is supported by PS engine
		*         EFalse otherwise
		* 
		*/ 
		IMPORT_C TBool IsLanguageSupportedL(const TLanguage aLanguage);

		/**
		* GetCachingStatusL
		* Gets the status of the caching synchronously
		*
		* @param aResultsBuffer Pointer to the result buffer.
		*/   
		IMPORT_C TInt GetCachingStatusL(TCachingStatus& aStatus);

		/**
		* ConvertToVpbkLinkLC.
		* Extract the VPBK contact link associated with the search result.
		* This is a synchronous request.
		*
		* @param aSearchResult PS result for which contact link is required.
		* @param aContactManager An instance of contact manager used for link extraction.
		* @return The contact link.
		*/
		IMPORT_C MVPbkContactLink* ConvertToVpbkLinkLC( const CPsClientData& aSearchResult,
		                                                CVPbkContactManager& aContactManager );
		    
        /**
        * GetDataOrderL.
        * This function returns the list of supported data fields (mapped to VPbk identifiers)
        * supported by a datastore.
        * This is a synchronous request.
        * 
        * @param aDataStore The data store URI.
        * @param aDataOrder This array has the field identifiers returned from the server.
        */
        IMPORT_C void GetDataOrderL(const TDesC& aDataStore, RArray<TInt>& aDataOrder);
        
        /**
        * GetSortOrderL.
        * This function returns the sort order of data fields (mapped to VPbk identifiers)
        * set on a datastore.
        * This is a synchronous request.
        * 
        * @param aDataStore The data store URI.
        * @param aSortOrder This array has the field identifiers returned from the server.
        */
        IMPORT_C void GetSortOrderL(const TDesC& aDataStore, RArray<TInt>& aSortOrder);
        
        /**
        * ChangeSortOrderL.
        * This function sets the sort order of data fields (mapped to VPbk identifiers)
        * on a datastore.
        * This is a synchronous request.
        * 
        * @param aDataStore The data store URI.
        * @param aSortOrder This array has the field identifiers to be set for this store.
        */
        IMPORT_C void ChangeSortOrderL(const TDesC& aDataStore, RArray<TInt>& aSortOrder);
                
        /**
		* ShutdownServerL
		* Shuts down the predictive search engine.
		*
		* CAUTION: Shutdown of predictive search server shouldn't be done for each
		* session. It should be done only when the search engine needs to be 
		* completely shutdown.
		*/
		IMPORT_C void ShutdownServerL();

	protected: // Functions from base classes

		/**
		* From CActive, RunL.
		* Callback function.
		* Invoked to handle responses from the server.
		*/
		void RunL();

		/**
		* From CActive, DoCancel.
		* Cancels any outstanding operation.
		*/
		void DoCancel();

	public:
	
		/**
		* Notify observers about the cahcing status
		*/
		void NotifyCachingStatus( TCachingStatus aStatus, TInt aError );
    	
	private: // Constructors and destructors

		/**
		* CPSRequestHandler.
		* Performs the first phase of two phase construction.
		*/
		CPSRequestHandler();

		/**
		* ConstructL.
		* Performs the second phase construction of a
		* CPSRequestHandler object.
		*/
		void ConstructL();

	private: // Internal data handlers

		/**
		* ParseResultsL.
		* Parses the results buffer and converts them to data elements.
		*/
		void HandleSearchResultsL();
		    
		/**
		* HandleBufferOverFlowL.
		* Handles internal buffer overflow event.
		*/
		void HandleBufferOverFlowL();

		/**
		* HandleErrorL.
		* Handles error events.
		*/
		void HandleErrorL(TInt aErrorCode);
		
		/**
		* AddMarkedContacts
		* Filters the bookmark results and adds them to final search result set
		*/
		TInt AddMarkedContactsL(RPointerArray<CPsClientData>& searchResults);

		/**
        * RunSearchFromBufferL
        * Runs search if search query buffer ( iPendingSearchQueryBuffer ) is not empty
        */
        void RunSearchFromBufferL();
        
	private: // Data

		/**
		* iSession, the predictive search server session.
		*/
		RPsSession iSession;

		/**
		* iObservers, observers which handles updates from the server.
		*/
		RPointerArray<MPsResultsObserver> iObservers;

		/**
		* iSearchQueryBuffer, the buffer that holds the search query.
		*/
		HBufC8* iSearchQueryBuffer;

		/**
         * iPendingSearchQueryBuffer, the buffer that holds the pending search query.
        */
        HBufC8* iPendingSearchQueryBuffer;

		/**
		* iSearchDataBuffer, the buffer that holds the search data.
		*/
		HBufC8* iSearchDataBuffer;

		/**
		* iSearchResultsBuffer, the buffer that holds the search results.
		*/
		HBufC8* iSearchResultsBuffer;
		
		/**
		* iConverter, instance of contact id converter
		*/
		CVPbkContactIdConverter* iConverter;
		
        /**
        * iPropertyHandler, the handler to property which is asynchronously monitored
        */
        CPsPropertyHandler* iPropertyHandler;
        
        /**
        * iContactAddedHandler, handler for reacting to contact being added to cache
        */
        CPsUpdateHandler* iContactAddedHandler;

        /**
        * iContactRemovedHandler, handler for reacting to contact being removed from cache
        */
        CPsUpdateHandler* iContactRemovedHandler;
        
        /**
        * iContactModifiedHandler, handler for reacting to contact being modified in cache
        */
        CPsUpdateHandler* iContactModifiedHandler;
        
        /**
        * Not Owned
        * iBookMarkContactManager, contact manager reference received from the client
        * To be used only for handling marked contacts.
        */
        CVPbkContactManager* iBookMarkContactManager;
        
        /**
        * iMarkedContacts, stores the bookmarked contacts if any, sent by the client
        */
        RPointerArray<CPsClientData> iMarkedContacts;
        
        /**
        * ETrue, if search request was cancelled
        */
        TBool iSearchRequestCancelled;
		
};


#endif //__CPSREQUESTHANDLER_H__

// End of File
