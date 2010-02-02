/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
#ifndef CPBKXREMOTECONTACTLOOKUPDUMMYADAPTERSESSION_H
#define CPBKXREMOTECONTACTLOOKUPDUMMYADAPTERSESSION_H

#include "cpbkxremotecontactlookupprotocolsession.h"

class MPbkxRemoteContactLookupProtocolEnv;
class CPbkxRemoteContactLookupDummyAdapterDatabase;

class CPbkxRemoteContactLookupDummyAdapterSession : 
    public CPbkxRemoteContactLookupProtocolSession
    {
public: // constructor and destructor

    static CPbkxRemoteContactLookupDummyAdapterSession* NewL(
        MPbkxRemoteContactLookupProtocolEnv* aEnvironment );

    virtual ~CPbkxRemoteContactLookupDummyAdapterSession();

public: // methods from CPbkxRemoteContactLookupProtocolSession

    /**
     * Initialize session search session.
     *
     * @param aObserver observer for the session
     * @param aAccountId remote account that is searched
     */
    virtual void InitializeL( 
        MPbkxRemoteContactLookupProtocolSessionObserver& aObserver,
        const TPbkxRemoteContactLookupProtocolAccountId& aAccountId );
        

    /**
     * Start loose search with given query.     
     *
     * @param aQueryText query text
     * @param aMaxMatches maximum number of matches
     */
    virtual void LooseSearchL( const TDesC& aQueryText, TInt aMaxMatches );
     
    /**
     * Retrieves rest of the contact fields. Contact items in the results
     * array are reused and new fields are just added to them.
     *
     * @param aResults set of results for which the 
     */
    virtual void RetrieveContactFieldsL( 
        RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults );
        
    /**
     * Cancels either the search or detail retrieval process.
     */
    virtual void Cancel();

private: // methods used internally

    void LaunchPeriodicL( TBool aSearchPeriodic = EFalse );

    static TInt PeriodicCallback( TAny* aObject );

    static TInt SearchPeriodicCallback( TAny* aObject );

    void DoOperationsL();

    void DoSearchL();

    void CreateDatabaseL();

    void ReportError( TInt aError );

private: // constructors

    CPbkxRemoteContactLookupDummyAdapterSession(
        MPbkxRemoteContactLookupProtocolEnv* aEnvironment );

    void ConstructL();

private: // data
    
    enum TState
        {
        ENone,
        ESearch,
        ERetrieve
        };

    TState iState;

    TPbkxRemoteContactLookupProtocolAccountId iAccountId;

    CPbkxRemoteContactLookupDummyAdapterDatabase* iDatabase;

    MPbkxRemoteContactLookupProtocolSessionObserver* iObserver;

    CPeriodic* iPeriodic;

    CPeriodic* iSearchPeriodic;
    
    TBool iSearchInProgress;

    RPointerArray<CPbkxRemoteContactLookupProtocolResult> iResultArray;

    RPointerArray<CPbkxRemoteContactLookupProtocolResult>* iDetailsArray;

    RBuf iQueryText;

    TInt iMaxMatches;

    MPbkxRemoteContactLookupProtocolEnv* iEnvironment;

    };

#endif
