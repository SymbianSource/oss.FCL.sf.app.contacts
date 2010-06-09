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
* Description:
*
*/


#ifndef PSTEST_QUERY_HANDLER_H
#define PSTEST_QUERY_HANDLER_H


//SYSTEM INCLUDES
#include <CPsRequestHandler.h>
#include <MPsResultsObserver.h>

//User INCLUDES
#include "pstestdefs.h"

// FORWARD DECLARATION
class CPsSettings;

// CLASS DECLARATION
/**
 *  @lib
 *  @since S60 v3.2
 */
class CPsTestQueryHandler: public CBase,
                           public MPsResultsObserver
{

    public: // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CPsTestQueryHandler* NewL();

        /**
        * Destructor.
        */
        virtual ~CPsTestQueryHandler();

    public: // New functions

        /**
        * Sets the test case currently being executed
        *
        * @param aCurrentTestCase Current test case.
        */
        void SetCurrentTestCase(TTestCase aCurrentTestCase);

        /**
        * Sets the error code for test case
        *
        * @param aCurrentErrorCode Error code for the test case.
        */
        void SetCurrentErrorCode(TInt aCurrentErrorCode);

        /**
        * Returns the error code for test case
        *
        * @return Error code for the test case.
        */
        TInt CurrentErrorCode();

        void GetAllContentsL();

        void StartSearchL(const CPsQuery& aSearchQuery);

        void TestSearchL(const TDesC& aBuf, TKeyboardModes aMode);

        void InputSearchL(const CPsQuery& aSearchQuery, const TDesC& aInput, RPointerArray<TDesC>& aResultSet);

        void SetSearchSettingsL(RPointerArray<TDesC>& aSearchUri, RArray<TInt>& aDisplayFields, TInt aMaxResults);

        void ShutDownL();

        void Cancel();

        TVersion Version();

        TBool IsLanguageSupportedL(TLanguage aLang);

    public: // Function from base class MPsResultsObserver

        /**
        * HandlePsResultsUpdateL.
        * Handle updates from the server.
        *
        * @param searchResults Search results returned from the server.
        * @param searchSeqs    List of matching character sequences.
        */
        void HandlePsResultsUpdate(RPointerArray<CPsClientData>& searchResults, RPointerArray<TDesC>& searchSeqs);

        /**
        * HandlePsError.
        * Handle errors from the server.
        *
        * @param aErrorCode Search errors returned from the server.
        */
        void HandlePsError(TInt aErrorCode);

        /**
        * CachingStatus
        * Gets called when caching is completed.
        * Observers need to implement this function accordingly
        * to take action after cache completion
        * @param aStatus - caching status, 20 - Caching completes succesfully, 30 - Caching completed with errors
        * @param aError - Any error that occurred while caching. KErrNone if no error else the error code
        */
        void CachingStatus(TCachingStatus& aStatus, TInt& aError);

    public:

        /**
        * Sets the list of contacts created for test case
        *
        * @param aContactsList List of contacts (each contact as CPsClientData).
        */
        void SetContactsDetails(RPointerArray<CPsClientData>& aContactsList);

        TInt IndexForGroupDataL();

    private:

        /**
        * C++ default constructor.
        */
        CPsTestQueryHandler();

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

    private:

        void FormTestSearchQueryL(const TDesC& aBuf, TKeyboardModes aMode);

        void VerifyResult(RPointerArray<CPsClientData>& aSearchResults, TBool aResultType);

        TBool VerifySortResult(RPointerArray<CPsClientData>& aSearchResults);

        void SwitchEPsData(RPointerArray<CPsClientData>& searchResults);

        TInt CompareByData ( const CPsClientData& aObject1, const CPsClientData& aObject2 );

    private: // data

        TTestCase iCurrentTestCase;

        TInt iCurrentErrorCode;

        CPSRequestHandler* iAsyncHandler;

        CPsSettings* iSettings;

        CPsQuery *iPsQuery;

        RPointerArray<CPsClientData> iContactsList;
};

#endif //PSTEST_QUERY_HANDLER_H
