/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CPbk2RemoteContactLookupAccounts
*
*/

#ifndef CPBK2REMOTECONTACTLOOKUPACCOUNTS_H
#define CPBK2REMOTECONTACTLOOKUPACCOUNTS_H

//  INCLUDES
#include <e32base.h>

#include <tpbkxremotecontactlookupprotocolaccountid.h>
#include <cpbkxremotecontactlookupprotocoladapter.h>
#include <cpbkxremotecontactlookupprotocolaccount.h>


// FORWARD DECLARATIONS
class CPbkxRemoteContactLookupProtocolAccount;
class CPbkxRclProtocolEnvImpl;

// CLASS DECLARATION

/**
 * A class for querying remote contact lookup accounts.
 */
class CPbk2RemoteContactLookupAccounts : public CBase
    {
    public:
        IMPORT_C static CPbk2RemoteContactLookupAccounts* NewL();
    
        IMPORT_C static CPbk2RemoteContactLookupAccounts* NewLC();
    
        ~CPbk2RemoteContactLookupAccounts();

    public: // Static methods

        /**
         * Gets the default account.
         *
         * The default account ID is stored in a CenRep.
         * It is managed by at least Phonebook2 application and also CMail.
         *
         * If the default has not been set, then the iProtocolUid of the returned
         * account ID will be NULL. Ie. you can check whether a default exists by
         * checking whether id.iProtocolUid is NULL.
         *
         * @return The default account ID.
         */
        IMPORT_C static TPbkxRemoteContactLookupProtocolAccountId DefaultAccountIdL();

		/**
         * Sets the default account.
         *
         * The default account ID is stored in a CenRep.
         * It is managed by at least Phonebook2 application and also CMail.
         *
         * 
         * @param aProtocolAccountId Protocol account ID.
         * @return None.
         */
        IMPORT_C static void SetDefaultAccountIdL(
                const TPbkxRemoteContactLookupProtocolAccountId& aAccountId );
                
        /**
         * Fetches an account from a protocol plugin. The plugin is loaded and then unloaded.
         * If the account is not found, then NULL is returned! It could mean
         * that either the protocol DLL does not exist, or the account managed
         * by that protocol DLL does not exist.
         *
         * @param aProtocolAccountId Protocol account ID.
         *
         * @return Object that represents one account, or NULL if account does
         * not exist. Ownership is given.
         * The returned object represents one account. It contains account ID + name.
         */
        IMPORT_C static CPbkxRemoteContactLookupProtocolAccount* GetAccountL(
            const TPbkxRemoteContactLookupProtocolAccountId& aProtocolAccountId );

    public: // non-static methods

        /**
         * Loads all the protocol plugins. The plugins remain loaded until this object is destroyed.
         */
        IMPORT_C void LoadProtocolPluginsL();

        /**
         * Gets all the protocol accounts. First loads all the plugins if they are not already loaded.
         * The plugins remain loaded until this object is destroyed.
         *
         * It is safe to destroy this CPbk2RemoteContactLookupAccounts and use
         * the account objects after that. TODO ?
         *
         * @param A reference to an array of accounts. The accounts are appended to this array.
         */
        IMPORT_C void GetAllAccountsL( RPointerArray<CPbkxRemoteContactLookupProtocolAccount>& aAccounts );

        /**
         * Gets protocol accounts for one protocol. Loads and unloads plugin(s).
         * If the protocol DLL does not exist, then no accounts are returned.
         *
         * It is safe to destroy this CPbk2RemoteContactLookupAccounts and use
         * the account objects after that. TODO ?
         *
         * @param A reference to an array of accounts. The accounts are appended to this array.
         */
        IMPORT_C void GetAccountsL(
            RPointerArray<CPbkxRemoteContactLookupProtocolAccount>& aAccounts,
            const TUid& aProtocol );


    private:
        CPbk2RemoteContactLookupAccounts();
    private:
        RPointerArray<CPbkxRemoteContactLookupProtocolAdapter> iAdapters; // owns
        CPbkxRclProtocolEnvImpl* iProtocolEnv;
    };

#endif // CPBK2REMOTECONTACTLOOKUPACCOUNTS_H

// End of File
