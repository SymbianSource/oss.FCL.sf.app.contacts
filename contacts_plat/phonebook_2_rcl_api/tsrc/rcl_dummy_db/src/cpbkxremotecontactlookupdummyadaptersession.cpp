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
#include "cpbkxremotecontactlookupdummyadaptersession.h"

#include "cpbkxremotecontactlookupdummyadapter.h"



#include <mpbkxremotecontactlookupprotocolenv.h>	





_LIT( KDatabasePath, "c:DummyContacts.cdb" );



const TInt KDelay = 1000000;

const TInt KSearchDelay = 1;



CPbkxRemoteContactLookupDummyAdapterSession* CPbkxRemoteContactLookupDummyAdapterSession::NewL(

    MPbkxRemoteContactLookupProtocolEnv* aEnvironment )

    {

    CPbkxRemoteContactLookupDummyAdapterSession* session = 

        new ( ELeave ) CPbkxRemoteContactLookupDummyAdapterSession( aEnvironment );

    CleanupStack::PushL( session );

    session->ConstructL();

    CleanupStack::Pop( session );

    return session;

    }



CPbkxRemoteContactLookupDummyAdapterSession::CPbkxRemoteContactLookupDummyAdapterSession(

    MPbkxRemoteContactLookupProtocolEnv* aEnvironment ) : 

    iAccountId( TUid::Uid( 0 ), 0 ), iEnvironment( aEnvironment )

    {

    }



CPbkxRemoteContactLookupDummyAdapterSession::~CPbkxRemoteContactLookupDummyAdapterSession()

    {

    delete iDatabase;



    if ( iPeriodic != NULL )

        {

        iPeriodic->Cancel();

        }

    delete iPeriodic;



    if ( iSearchPeriodic != NULL )

        {

        iSearchPeriodic->Cancel();

        }

    delete iSearchPeriodic;



    iQueryText.Close();

    iResultArray.Reset();

    }



void CPbkxRemoteContactLookupDummyAdapterSession::ConstructL()

    {

    }



void CPbkxRemoteContactLookupDummyAdapterSession::InitializeL(

    MPbkxRemoteContactLookupProtocolSessionObserver& aObserver,

    const TPbkxRemoteContactLookupProtocolAccountId& aId )

    {

    iObserver = &aObserver;

    iAccountId = aId;

    }



void CPbkxRemoteContactLookupDummyAdapterSession::LooseSearchL(

    const TDesC& aQueryText,

    TInt aMaxMatches )

    {

    if ( iObserver != NULL )

        {

        iResultArray.Reset();



        iMaxMatches = aMaxMatches;

        iQueryText.Close();

        iQueryText.CreateL( aQueryText );



        iState = ESearch;

        LaunchPeriodicL();

        }

    }



void CPbkxRemoteContactLookupDummyAdapterSession::RetrieveContactFieldsL(

    RPointerArray<CPbkxRemoteContactLookupProtocolResult>& aResults )

    {

    iDetailsArray = &aResults;

    iState = ERetrieve;

    LaunchPeriodicL();

    }



void CPbkxRemoteContactLookupDummyAdapterSession::Cancel()

    {

    switch ( iState )

        {

        case ESearch:

            if ( iSearchPeriodic != NULL )

                {

                // delete search results, since they are not yet delivered

                iResultArray.ResetAndDestroy();

                iDatabase->CancelSearch();

                iSearchPeriodic->Cancel();

                }

            break;

        case ERetrieve:

            if ( iPeriodic != NULL )

                {

                iPeriodic->Cancel();

                }

            break;

        }

    iState = ENone;

    }



void CPbkxRemoteContactLookupDummyAdapterSession::LaunchPeriodicL(

    TBool aSearchPeriodic )

    {

    if ( !aSearchPeriodic )

        {

        if ( iPeriodic == NULL )

            {

            iPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );

            }

    

        iPeriodic->Cancel();

        

        iPeriodic->Start( 

            KDelay, 

            KDelay, 

            TCallBack( CPbkxRemoteContactLookupDummyAdapterSession::PeriodicCallback, this ) );

        }

    else

        {

        if ( iSearchPeriodic == NULL )

            {

            iSearchPeriodic = CPeriodic::NewL( CActive::EPriorityStandard );

            }

        iSearchPeriodic->Cancel();



        iSearchPeriodic->Start(

            KSearchDelay,

            KSearchDelay,

            TCallBack( CPbkxRemoteContactLookupDummyAdapterSession::SearchPeriodicCallback, this ) );

        }

    }



TInt CPbkxRemoteContactLookupDummyAdapterSession::PeriodicCallback( TAny* aObject )

    {

    CPbkxRemoteContactLookupDummyAdapterSession* session = 

        reinterpret_cast<CPbkxRemoteContactLookupDummyAdapterSession*>( aObject );

    TRAPD( err, session->DoOperationsL() );

    if ( err != KErrNone )

        {

        session->ReportError( err );

        }

    return err;

    }



TInt CPbkxRemoteContactLookupDummyAdapterSession::SearchPeriodicCallback( TAny* aObject )

    {

    CPbkxRemoteContactLookupDummyAdapterSession* session =

        reinterpret_cast<CPbkxRemoteContactLookupDummyAdapterSession*>( aObject );

    TRAPD( err, session->DoSearchL() );

    if ( err != KErrNone )

        {

        session->ReportError( err );

        }

    return err;

    }



void CPbkxRemoteContactLookupDummyAdapterSession::DoOperationsL()

    {

    if ( iPeriodic != NULL )

        {

        iPeriodic->Cancel();

        }



    if ( iObserver != NULL )

        {



        if ( iDatabase == NULL )

            {

            CreateDatabaseL();

            }



        switch ( iState )

            {

            case ESearch:

                LaunchPeriodicL( ETrue );

                //iDatabase->LooseSearchL( iQueryText, iMaxMatches, iResultArray );

                //iObserver->LooseSearchCompleted( KErrNone, iResultArray );

                break;

            case ERetrieve:

                iDatabase->RetrieveDetailsL( *iDetailsArray );

                iObserver->ContactFieldsRetrievalCompleted( KErrNone );

                iState = ENone;

                break;

            default:

                break;

            }

        }

    }



void CPbkxRemoteContactLookupDummyAdapterSession::DoSearchL()

    {

    if ( iDatabase->LooseSearchL( iQueryText, iMaxMatches, iResultArray ) )

        {

        // search ready

        iSearchPeriodic->Cancel();

        iObserver->LooseSearchCompleted( KErrNone, iResultArray );

        iState = ENone;

        }

    else

        {

        LaunchPeriodicL( ETrue );

        }

    }



void CPbkxRemoteContactLookupDummyAdapterSession::CreateDatabaseL()

    {

    iDatabase = CPbkxRemoteContactLookupDummyAdapterDatabase::NewL( 

        KDatabasePath,

        iEnvironment );

    }



void CPbkxRemoteContactLookupDummyAdapterSession::ReportError( TInt aError )

    {

    if ( iObserver != NULL )

        {

        switch ( iState )

            {

            case ESearch:

                if ( iSearchPeriodic != NULL )

                    {

                    iSearchPeriodic->Cancel();

                    }

                iObserver->LooseSearchCompleted( aError, iResultArray );

                break;

            case ERetrieve:

                iObserver->ContactFieldsRetrievalCompleted( aError );

                break;

            default:

                break;

            }

        }

    }

