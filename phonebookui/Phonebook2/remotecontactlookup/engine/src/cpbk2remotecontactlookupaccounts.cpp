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
* Description:  Implementation of the class Pbk2RemoteContactLookupFactory.
*
*/


#include <cpbk2remotecontactlookupaccounts.h>

#include "pbkxrclutils.h"
#include "cpbkxrclprotocolenvimpl.h"
#include "cpbkxremotecontactlookupprotocoladapter.h"

void CleanupResetAndDestroyInfoArray(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RImplInfoPtrArray*>(aObj)->ResetAndDestroy();
        }
    }

void CleanupResetAndDestroyAccArray(TAny* aObj)
    {
    if (aObj)
        {
        static_cast<RPointerArray<CPbkxRemoteContactLookupProtocolAccount>*>(aObj)->ResetAndDestroy();
        }
    }


// ======== MEMBER FUNCTIONS ========


EXPORT_C CPbk2RemoteContactLookupAccounts* CPbk2RemoteContactLookupAccounts::NewL()
    {
    CPbk2RemoteContactLookupAccounts* self = CPbk2RemoteContactLookupAccounts::NewLC();
    CleanupStack::Pop( self );
    return self;
    }



EXPORT_C CPbk2RemoteContactLookupAccounts* CPbk2RemoteContactLookupAccounts::NewLC()
    {
    CPbk2RemoteContactLookupAccounts* self = new (ELeave) CPbk2RemoteContactLookupAccounts();
    CleanupStack::PushL( self );
    return self;
    }



CPbk2RemoteContactLookupAccounts::~CPbk2RemoteContactLookupAccounts()
    {
    iAdapters.ResetAndDestroy();
    delete iProtocolEnv;
    }



EXPORT_C TPbkxRemoteContactLookupProtocolAccountId CPbk2RemoteContactLookupAccounts::DefaultAccountIdL()
    {
    return PbkxRclUtils::DefaultProtocolAccountIdL();
    }



EXPORT_C CPbkxRemoteContactLookupProtocolAccount* CPbk2RemoteContactLookupAccounts::GetAccountL(
    const TPbkxRemoteContactLookupProtocolAccountId& aProtocolAccountId )
    {
    CPbkxRclProtocolEnvImpl* protocolEnv = CPbkxRclProtocolEnvImpl::NewLC();

    CPbkxRemoteContactLookupProtocolAccount* account = NULL;

    // Need load the plugin    
    CPbkxRemoteContactLookupProtocolAdapter* adapter = NULL;
    TRAP_IGNORE( adapter = CPbkxRemoteContactLookupProtocolAdapter::NewL( 
            aProtocolAccountId.iProtocolUid, 
            *protocolEnv ) );
    if ( adapter )
        {
        CleanupStack::PushL( adapter );

        // Delegate to the plugin
        account = adapter->NewProtocolAccountL( aProtocolAccountId );
        CleanupStack::PopAndDestroy( adapter );
        }
    CleanupStack::PopAndDestroy(); // protocolEnv
    return account;
    }



EXPORT_C void CPbk2RemoteContactLookupAccounts::LoadProtocolPluginsL()
    {
    iAdapters.ResetAndDestroy();
    if ( !iProtocolEnv )
        {
        iProtocolEnv = CPbkxRclProtocolEnvImpl::NewL();
        }
    
    RImplInfoPtrArray implArray;
    TCleanupItem cleanupItem(CleanupResetAndDestroyInfoArray, &implArray);
    CleanupStack::PushL( cleanupItem );
    REComSession::ListImplementationsL( TUid::Uid(KPbkxRemoteContactLookupAdapterIfUid), implArray );

    //TODO Need to also keep track of the plugin protocol UIDs. ???
    // Also impl. UIDS to avoid unnecessary loading?
    // The plugin interface doesn't have a method for asking the protocol UID!!

    // Now create all the protocol adapters.
    // The actual ECom loading is implemented in CPbkxRemoteContactLookupProtocolAdapter.
    for ( TInt i = 0; i < implArray.Count(); i++ )
        {
        CPbkxRemoteContactLookupProtocolAdapter* adapter = NULL;
        adapter = CPbkxRemoteContactLookupProtocolAdapter::NewL(
            implArray[i]->ImplementationUid(), *iProtocolEnv );
        CleanupStack::PushL( adapter );
        User::LeaveIfError( iAdapters.Append( adapter ) );
        CleanupStack::Pop( adapter );
        }
    CleanupStack::PopAndDestroy(); // cleanupItem 
    }



EXPORT_C void CPbk2RemoteContactLookupAccounts::GetAllAccountsL(
    RPointerArray<CPbkxRemoteContactLookupProtocolAccount>& aAccounts )
    {
    if ( iAdapters.Count() == 0 )
        {
        // No plugins were loaded.
        LoadProtocolPluginsL();
        }
    
    // Loop through all the plugins and collect all the accounts
    const TInt protocolAdapterCount = iAdapters.Count();
    for ( TInt n = 0; n < protocolAdapterCount; n++ )
        {
        CPbkxRemoteContactLookupProtocolAdapter& adapter = *iAdapters[n];
        // Using a temp array, just to avoid an adapter resetting the main array
        RPointerArray<CPbkxRemoteContactLookupProtocolAccount> protocolAccountsTmp;
        TCleanupItem cleanupItemAcc(CleanupResetAndDestroyAccArray, &protocolAccountsTmp);
        CleanupStack::PushL(cleanupItemAcc);
        adapter.GetAllProtocolAccountsL( protocolAccountsTmp );
        
        // Move accounts from temp array to main array
        
        // First expand the main array so that we can move ownership in one go.
        // TODO OOPS, METHOD NOT AVAILABLE iAdapters.SetReserveL( iAdapters.Count() + protocolAccountsTmp.Count() );

        
        // Don't need the cleanup item anymore since iAdapters will take ownership,
        // and the iAdapters has already been expanded.
        CleanupStack::Pop(); // cleanupItemAcc
        for ( TInt x = 0; x < protocolAccountsTmp.Count(); x++ )
            {
            // Ownership is passed
            User::LeaveIfError( aAccounts.Append( protocolAccountsTmp[ x ] ) );
            }
        // just let protocolAccountsTmp pass out of scope. It doesn't own anything now.
        }
    }



EXPORT_C void CPbk2RemoteContactLookupAccounts::GetAccountsL(
    RPointerArray<CPbkxRemoteContactLookupProtocolAccount>& aAccounts,
    const TUid& aProtocol )
    {
    //TODO Should optimise this.

    RPointerArray<CPbkxRemoteContactLookupProtocolAccount> protocolAccounts;
    TCleanupItem cleanupItemAcc(CleanupResetAndDestroyAccArray, &protocolAccounts);
    CleanupStack::PushL(cleanupItemAcc);

    GetAllAccountsL( protocolAccounts );

    // Collect the accounts which have correct protocol
    for ( TInt j = protocolAccounts.Count() - 1; j >= 0; j-- )
        {
        CPbkxRemoteContactLookupProtocolAccount* account = protocolAccounts[j]; 
        if( account->Id().iProtocolUid == aProtocol )
            {
            // Transfer ownership
            User::LeaveIfError( aAccounts.Append( account ) );
            }
        else
            {
            // Wrong account. Don't need it.
            delete account;
            }
        protocolAccounts.Remove( j );
        }
    CleanupStack::Pop(); // cleanupItemAcc
    }

CPbk2RemoteContactLookupAccounts::CPbk2RemoteContactLookupAccounts()
    {
    }

EXPORT_C void CPbk2RemoteContactLookupAccounts::SetDefaultAccountIdL(
        const TPbkxRemoteContactLookupProtocolAccountId& aAccountId )
    {
    PbkxRclUtils::StoreDefaultProtocolAccountIdL( aAccountId );    
    }

// end of file
