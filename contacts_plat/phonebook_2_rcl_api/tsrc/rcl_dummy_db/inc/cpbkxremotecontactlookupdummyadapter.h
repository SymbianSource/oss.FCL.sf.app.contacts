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
#ifndef CPBKXREMOTECONTACTLOOKUPDUMMYADAPTER_H
#define CPBKXREMOTECONTACTLOOKUPDUMMYADAPTER_H

#include "cpbkxremotecontactlookupprotocoladapter.h"

class CPbkxRemoteContactLookupDummyAdapterDatabase;
class MPbkxRemoteContactLookupProtocolEnv;

/**
* Dummy adapter for testing purposes.
*/
class CPbkxRemoteContactLookupDummyAdapter : public CPbkxRemoteContactLookupProtocolAdapter
    {
public: // functions from base class

    static CPbkxRemoteContactLookupDummyAdapter* NewL( TAny* aEnvironment );

    virtual ~CPbkxRemoteContactLookupDummyAdapter();

public: // from CPbkxRemoteContactLookupProtocolAdapter

    /**
    * Creates new protocol account with given UID.
    *
    * @param aAccountId Account id.
    * @return Create account.
    */
    virtual CPbkxRemoteContactLookupProtocolAccount* NewProtocolAccountL( 
        const TPbkxRemoteContactLookupProtocolAccountId& aAccountId );
    
    /**
     * Gives an array containing all the available protocol accounts. Owernship of the
     * accounts is transfered to the caller.
     *
     * @param aAccounts an array containing all the available protocol 
     *                  accounts.
     */
    virtual void GetAllProtocolAccountsL( 
        RPointerArray<CPbkxRemoteContactLookupProtocolAccount>& aAccounts );

    /**
     * Create protocol session.
     *
     * @return protocol session
     */
    virtual CPbkxRemoteContactLookupProtocolSession* NewSessionL();


private: // constructors

    CPbkxRemoteContactLookupDummyAdapter(
        MPbkxRemoteContactLookupProtocolEnv* aEnvironment );

    void ConstructL();
    
private: // methods used internally

    void LaunchPeriodicL();

    static TInt PeriodicCallback( TAny* aObject );

    void SendResponseL();

    void CreateDatabaseL();

private: // data

    // Environment.
    MPbkxRemoteContactLookupProtocolEnv* iEnvironment;

    };

class CPbkxRemoteContactLookupDummyAdapterDatabase : public CBase
    {
public: // constructor and destructor

    static CPbkxRemoteContactLookupDummyAdapterDatabase* NewL(
        const TDesC& aPath,
        MPbkxRemoteContactLookupProtocolEnv* aEnvironment );

    virtual ~CPbkxRemoteContactLookupDummyAdapterDatabase();

public: // new methods

    TBool LooseSearchL(
        const TDesC& aQueryText,
        TInt aMaxMatches,
        RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aFoundContacts );

    void RetrieveDetailsL( RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aContacts );

    void CancelSearch();

private: // constructors

    CPbkxRemoteContactLookupDummyAdapterDatabase( 
        MPbkxRemoteContactLookupProtocolEnv* aEnvironment );

    void ConstructL( const TDesC& aPath );

private: // methods used internally

    void CreateDatabaseL( const TDesC& aPath );

    CContactCard* CreateContactCardL( const CContactCard* aCard );

    void AddDetailsL( const CContactCard* aOriginal, CContactCard* aCard );
    
private: // data

    // Contact data base
    CContactDatabase* iContactDatabase;

    // Contact id array.
    CContactIdArray* iIdArray;

    // Array for keeping the contact cards that matched the search criteria.
    RPointerArray<CContactCard> iContactCards;

    // Old contact cards for keeping them in case new search was canceled.
    RPointerArray<CContactCard> iOldContactCards;

    // Machine id used to get contact uids.
    TInt64 iMachineId;

    // Environment for creating objects.
    MPbkxRemoteContactLookupProtocolEnv* iEnvironment;

    // Counter to indicate where search is going
    TInt iTick;
    };

#endif
