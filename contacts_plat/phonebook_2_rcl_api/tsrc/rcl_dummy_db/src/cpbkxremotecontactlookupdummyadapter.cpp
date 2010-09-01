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
#include <ecom/implementationproxy.h>
#include <cntdb.h>
#include <cntfldst.h>

#include "cpbkxremotecontactlookupdummyadapter.h"
#include "pbkxremotecontactlookupdummyadapterimpldefines.hrh"
#include <mpbkxremotecontactlookupprotocolenv.h>   
#include "cpbkxremotecontactlookupdummyadaptersession.h"
#include "pbkxdummyadapterdebug.h"

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 
        KPbkxRemoteContactLookupDummyAdapterImplUid,
        CPbkxRemoteContactLookupDummyAdapter::NewL )
    };

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    return ImplementationTable;
    }

CPbkxRemoteContactLookupDummyAdapter* CPbkxRemoteContactLookupDummyAdapter::NewL(
    TAny* aEnvironment
    )
    {
    MPbkxRemoteContactLookupProtocolEnv* env = 
        reinterpret_cast<MPbkxRemoteContactLookupProtocolEnv*>( aEnvironment );
    CPbkxRemoteContactLookupDummyAdapter* adapter = 
        new ( ELeave ) CPbkxRemoteContactLookupDummyAdapter( env );
    CleanupStack::PushL( adapter );
    adapter->ConstructL();
    CleanupStack::Pop( adapter );
    return adapter;
    }

CPbkxRemoteContactLookupDummyAdapter::CPbkxRemoteContactLookupDummyAdapter(
    MPbkxRemoteContactLookupProtocolEnv* aEnvironment ) : iEnvironment( aEnvironment )
    {
    }

void CPbkxRemoteContactLookupDummyAdapter::ConstructL()
    {
    }

CPbkxRemoteContactLookupDummyAdapter::~CPbkxRemoteContactLookupDummyAdapter()
    {
    }

CPbkxRemoteContactLookupProtocolAccount* CPbkxRemoteContactLookupDummyAdapter::NewProtocolAccountL(
    const TPbkxRemoteContactLookupProtocolAccountId& aAccountId )
    {
    if ( aAccountId.iProtocolUid == TUid::Uid( KPbkxRemoteContactLookupDummyAdapterImplUid )
        && aAccountId.iAccountId == KPbkxRemoteContactLookupDummyAdapterDummyUid )
        {
        return iEnvironment->NewProtocolAccountL( aAccountId, _L("Dummy") );
        }
    else
        {
        return NULL;
        }
    }

void CPbkxRemoteContactLookupDummyAdapter::GetAllProtocolAccountsL(
    RPointerArray<CPbkxRemoteContactLookupProtocolAccount>& aAccounts )
    {
    TPbkxRemoteContactLookupProtocolAccountId id( 
        TUid::Uid( KPbkxRemoteContactLookupDummyAdapterImplUid ),
        KPbkxRemoteContactLookupDummyAdapterDummyUid );
    aAccounts.AppendL( NewProtocolAccountL( id ) );
    }

CPbkxRemoteContactLookupProtocolSession* CPbkxRemoteContactLookupDummyAdapter::NewSessionL()
    {
    CPbkxRemoteContactLookupDummyAdapterSession* session = 
        CPbkxRemoteContactLookupDummyAdapterSession::NewL(
        iEnvironment );
    return session;
    }

//////////////////////////////////////////////////////////////////////////////
// CPbkxRemoteContactLookupDummyAdapterDatabase
//////////////////////////////////////////////////////////////////////////////

const TInt KTickCount = 50;

CPbkxRemoteContactLookupDummyAdapterDatabase* CPbkxRemoteContactLookupDummyAdapterDatabase::NewL(
    const TDesC& aPath,
    MPbkxRemoteContactLookupProtocolEnv* aEnvironment )
    {
    CPbkxRemoteContactLookupDummyAdapterDatabase* database = 
        new ( ELeave ) CPbkxRemoteContactLookupDummyAdapterDatabase( aEnvironment );
    CleanupStack::PushL( database );
    database->ConstructL( aPath );
    CleanupStack::Pop( database );
    return database;
    }

CPbkxRemoteContactLookupDummyAdapterDatabase::~CPbkxRemoteContactLookupDummyAdapterDatabase()
    {
    delete iContactDatabase;
    delete iIdArray;
    iContactCards.ResetAndDestroy();
    iOldContactCards.ResetAndDestroy();
    }

CPbkxRemoteContactLookupDummyAdapterDatabase::CPbkxRemoteContactLookupDummyAdapterDatabase(
    MPbkxRemoteContactLookupProtocolEnv* aEnvironment ) :
    CBase(), iEnvironment( aEnvironment )
    {
    }

void CPbkxRemoteContactLookupDummyAdapterDatabase::ConstructL( const TDesC& aPath )
    {
    CreateDatabaseL( aPath );
    }

TBool CPbkxRemoteContactLookupDummyAdapterDatabase::LooseSearchL(
    const TDesC& aQueryText,
    TInt aMaxMatches,
    RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aFoundContacts )
    {

    if ( iTick == 0 )
        {
        iOldContactCards.ResetAndDestroy();
        for ( TInt i = 0; i < iContactCards.Count(); i++ )
            {
            TRAP_IGNORE( iOldContactCards.AppendL( iContactCards[i] ) );
            }
        iContactCards.Reset();
        }
    
    TInt count = 0;

    // search only for a certain amount of items in one call
    for ( TInt i = iTick; i < iIdArray->Count() && count < KTickCount ; i++ )
        {
        
        CContactItem* item = iContactDatabase->ReadContactL( (*iIdArray)[i] );
        CleanupStack::PushL( item );
        CContactCard* card = dynamic_cast<CContactCard*>( item );

        // search match from name and emails
        CContactItemFieldSet& fields = card->CardFields();
       
        // get name
        TPtrC firstName;
        TPtrC lastName;

        _LIT( KNameFormat, "%S %S" );

        TInt index = fields.Find( KUidContactFieldGivenName );

        if ( index != KErrNotFound )
            {
            firstName.Set( fields[index].TextStorage()->Text() );
            }

        index = fields.Find( KUidContactFieldFamilyName );

        if ( index != KErrNotFound )
            {
            lastName.Set( fields[index].TextStorage()->Text() );
            }

        RBuf name;
        name.CreateL( firstName.Length() + lastName.Length() + 1 );
        CleanupClosePushL( name );

        name.Format( KNameFormat, &firstName, &lastName );

        if ( name.FindC( aQueryText ) != KErrNotFound )
            {
            // match found
            CleanupStack::PopAndDestroy( &name );
            CContactCard* resultCard = CreateContactCardL( card );
            CPbkxRemoteContactLookupProtocolResult* result =
                iEnvironment->NewProtocolResultL( EFalse, *resultCard, KNullDesC );
            aFoundContacts.AppendL( result );
            iContactCards.AppendL( card );
            CleanupStack::Pop( item );
            count++;
            iTick++;
            continue;
            }

        CleanupStack::PopAndDestroy( &name );

        // if match is not found, search in email fields
        TBool found = EFalse;
        index = fields.FindNext( KUidContactFieldEMail );
        while ( index != KErrNotFound )
            {
            TPtrC email = fields[index].TextStorage()->Text();
            if ( email.FindC( aQueryText ) != KErrNotFound )
                {
                CContactCard* resultCard = CreateContactCardL( card );
                CPbkxRemoteContactLookupProtocolResult* result =
                    iEnvironment->NewProtocolResultL( EFalse, *resultCard, KNullDesC );
                aFoundContacts.AppendL( result );
                iContactCards.AppendL( card );
                CleanupStack::Pop( item );
                found = ETrue;
                break;
                }
            index = fields.FindNext( KUidContactFieldEMail, index + 1 );
            }

        if ( !found )
            {
            CleanupStack::PopAndDestroy( item );
            }

        count++;
        iTick++;
        }

    if ( iTick == iIdArray->Count() )
        {
        iTick = 0;
        return ETrue;
        }
    else
        {
        return EFalse;
        }
       
    }

void CPbkxRemoteContactLookupDummyAdapterDatabase::RetrieveDetailsL(
    RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aContacts )
    {
    for ( TInt i = 0; i < aContacts.Count(); i++ )
        {
        CPbkxRemoteContactLookupProtocolResult* result = aContacts[i];
        if ( !result->IsComplete() )
            {
            CContactCard* card = dynamic_cast<CContactCard*>( &( result->ContactItem() ) );
            CContactCard* orig = NULL;
            for ( TInt j = 0; j < iContactCards.Count(); j++ )
                {
                CContactCard* origCard = iContactCards[j];
                if ( origCard->UidStringL( iMachineId ) == 
                     card->UidStringL( iMachineId ) )
                    {
                    orig = origCard;
                    break;
                    }
                }
            if ( orig != NULL )
                {
                AddDetailsL( orig, card );
                result->Complete();
                }
            }
        }
    }

void CPbkxRemoteContactLookupDummyAdapterDatabase::CancelSearch()
    {
    if ( iTick != 0 )
        {
        iTick = 0;
        iContactCards.ResetAndDestroy();
        for ( TInt i = 0; i < iOldContactCards.Count(); i++ )
            {
            TRAP_IGNORE( iContactCards.AppendL( iOldContactCards[i] ) );
            }
        iOldContactCards.Reset();
        }
    }

void CPbkxRemoteContactLookupDummyAdapterDatabase::CreateDatabaseL( const TDesC& aPath )
    {
    iContactDatabase = CContactDatabase::OpenL( aPath );
   
    iMachineId = iContactDatabase->MachineId();

    iContactDatabase->SetDbViewContactType( KUidContactCard );

    iIdArray = CContactIdArray::NewL( iContactDatabase->SortedItemsL() );

    }

CContactCard* CPbkxRemoteContactLookupDummyAdapterDatabase::CreateContactCardL( 
    const CContactCard* aCard )
    {
    TPtrC uid = aCard->UidStringL( iMachineId );

    CContactCard* result = CContactCard::NewL();
    CleanupStack::PushL( result );

    result->SetUidStringL( uid );

    CContactItemFieldSet& fields = aCard->CardFields();
    // add first three fields
    for ( TInt i = 0; i < 3; i++ )
        {
        CContactItemField& field = fields[i];
        CContactItemField* newField = CContactItemField::NewL( field );
        CleanupStack::PushL( newField );
        result->AddFieldL( *newField );
        CleanupStack::Pop( newField );
        }
    CleanupStack::Pop( result );
    return result;
    }

void CPbkxRemoteContactLookupDummyAdapterDatabase::AddDetailsL(
    const CContactCard* aOriginal,
    CContactCard* aCard )
    {
    // contact card has already first three fields, add the rest also
    CContactItemFieldSet& fields = aOriginal->CardFields();
    for ( TInt i = 3; i < fields.Count(); i++ )
        {
        CContactItemField& field = fields[i];
        CContactItemField* newField = CContactItemField::NewL( field );
        CleanupStack::PushL( newField );
        aCard->AddFieldL( *newField );
        CleanupStack::Pop( newField );
        }
    }
