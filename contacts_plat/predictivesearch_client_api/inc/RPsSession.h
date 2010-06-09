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
*                predictive contact search server.
*
*/

#ifndef R_PS_SESSION_H
#define R_PS_SESSION_H

// INCLUDE FILES

// SYSTEM INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <e32cmn.h>
#include <s32mem.h>
#include <e32debug.h>
#include <utf.h>
#include <s32mem.h>
#include <CPcsDefs.h>

// CLASS DECLARATION
/**
* Client side interface for predictive search server.
*/
class RPsSession : public RSessionBase
{

    public:

		/**
		* RPcsSession.
		* Constructor
		*/
		RPsSession();

		/**
		* Connect.
		* Connects to the predictive search server
		*/
		TInt Connect();

		/**
		* Version.
		* Returns the version number of predictive search server
		*
		* @return Version information
		*/
		TVersion Version() const;

	    /**
        * SetSearchSettingsL.
        * Send the search settings (databases/ display fields) to the server.
        * @param aSettings      The search settings.
        */
	    void SetSearchSettingsL(const TDes8& aSettings);

        /**
        * SearchL.
        * Initiates a predictive search request to the server.
        * @param aSearchQuery   The search query.
        * @param aResultsBuffer Pointer to the initial results buffer.
	    * @param aStatus        Holds the completion status of request.
        */
	    void SearchL(const TDes8& aSearchQuery,
	                 TPtr8 aResultsBuffer,
					 TRequestStatus& aStatus);

	    /**
	    * CancelSearchL.
	    * Cancels a pending search request.
	    * @param aStatus        Holds the completion status of request.
	    */
	    void CancelSearch();

	    /**
        * SearchL.
        * Initiates a predictive search request to the server.
        * @param aSearchQuery   The search query.
        * @param aSearchData    The search data.
        * @param aResultsBuffer Pointer to the initial results buffer.
        */
	    void SearchL(const TDes8& aSearchQuery,
	                 const TDes8& aSearchData,
	                 TPtr8 aResultsBuffer);

        /**
        * SearchMatchStringL.
        * Initiates a predictive search request to the server with result as string.
        * @param aSearchQuery   The search query.
        * @param aSearchData    The search data.
        * @param aResultsBuffer Pointer to the initial results buffer.
        */
       void SearchMatchStringL(const TDes8& aSearchQuery,
                               const TDes8& aSearchData,
                               TDes& aResultsBuffer);

	    /**
	    * SendNewBufferL.
	    * Send a new buffer to server if a overflow occurs.
	    *
	    * @param aResultsBuffer Pointer to the new results buffer.
	    * @param aStatus        Holds the completion status of request.
	    */
	    void SendNewBufferL(TPtr8 aResultsBuffer,
	                        TRequestStatus& aStatus);

        /**
        * IsLanguageSupportedL
        * Sends a request to PS server to decide if the language
        * variant is supported or not
        *
        * @param aLanguage      The language id info.
        * @param aResultsBuffer Pointer to the initial results buffer.
        */
        void IsLanguageSupportedL(const TDes8& aLanguage,
                                  TPtr8 aResultsBuffer);
        /**
        * GetDataOrderL
        * Sends a request to PS server to get the data fields supported
        * for a data store.
        *
        * @param aURI           Data store identifier.
        * @param aResultsBuffer Pointer to the results buffer.
        */
        void GetDataOrderL(const TDes8& aURI,
                           TPtr8 aResultsBuffer);

        /**
        * GetSortOrderL
        * Sends a request to PS server to get the sort order of fields
        * supported for a data store.
        *
        * @param aURI           Data store identifier.
        * @param aResultsBuffer Pointer to the results buffer.
        */
        void GetSortOrderL(const TDes8& aURI,
                           TPtr8 aResultsBuffer);

        /**
        * ChangeSortOrderL
        * Sends a request to PS server to set the sort order of fields
        * for a data store.
        *
        * @param aInput         Data store + Sort Order values.
        */
        void ChangeSortOrderL(const TDes8& aInput);

        /**
        * GetAdaptiveGridL
        * This is a support method for Adaptive Search functionality.
        * As Predictive Search Engine is well suited for keeping an maintaining data
        * structures related to contacts and updated according to contacts
        * additions/modifications/deletions, like it is done already in Predictive Search
        * Engine.
        *
        * @param aURIs         The data store URIs.
        * @param aCompanyName  The info if Company Name is required.
        * @param aAdaptiveGrid The returned Adaptive Grid.
        */
        void GetAdaptiveGridL( const TDesC8& aURIs,
                               const TBool aCompanyName,
                               TDes& aResultsBuffer );

		/**
		* Shuts down the predictive search server
		*/
		void ShutdownServerL();

	private:

	    /**
	    * iSearchQueryBufferPtr. Pointer to the buffer holding search query.
	    */
		TPtrC8 iSearchQueryBufferPtr;

		/**
	    * iResultsBufferPtr. Pointer to the buffer holding search results
	    */
		TPtr8 iResultsBufferPtr;

};

#endif // R_PS_SESSION_H

// End of file
