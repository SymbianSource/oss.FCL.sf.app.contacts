/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <StifItemParser.h>
#include <Stiftestinterface.h>

#include <presencecachewriter2.h>
#include <presencecachereader2.h>
#include <mpresencebuddyinfo2.h>

#include <contactpresencefactory.h>
#include <mcontactpresence.h>

#include "contactpresenceapitester.h"
#include "contactpresenceapihandler.h"
#include "contactpresencephonebook.h"
#include "contactpresencepbhandler.h"

 
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::Delete
//  
// -----------------------------------------------------------------------------
//
void CContactPresenceApiTester::Delete() 
    {
    if( iContactPresence )
        {
        iContactPresence->Close();
        iContactPresence = NULL;
        }
    if( iLinkArray )
        {
        iLinkArray->Delete( 0, iLinkArray->MdcaCount() );
        delete iLinkArray;
        iLinkArray = NULL;
        }
    if( iContactPresenceHandler )
        {
        delete iContactPresenceHandler;
        iContactPresenceHandler = NULL;
        }
    if( iPhonebook )
        {
        delete iPhonebook;
        iPhonebook = NULL;
        }
    if( iPhonebookHandler )
        {
        delete iPhonebookHandler;
        iPhonebookHandler = NULL;
        }
    
    delete iWriter; 
    iWriter = NULL;     
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::RunMethodL
// 
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::RunMethodL( CStifItemParser& aItem ) 
    {
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CreateTestServicesL", CContactPresenceApiTester::CreateTestServicesL ),                 
        ENTRY( "CreateTestContactsL", CContactPresenceApiTester::CreateTestContactsL ),                        
        ENTRY( "OpenStoresL", CContactPresenceApiTester::OpenStoresL ),
        ENTRY( "GetIconInfoL", CContactPresenceApiTester::GetIconInfoL ),
        ENTRY( "FetchContactL", CContactPresenceApiTester::FetchContactL ),
        ENTRY( "GetBrandedIconL", CContactPresenceApiTester::GetBrandedIconL ),
        ENTRY( "CreateContactPresenceL", CContactPresenceApiTester::CreateContactPresenceL ),
        ENTRY( "DoWaitSec", CContactPresenceApiTester::DoWaitSec ),
        ENTRY( "FeedPresenceL", CContactPresenceApiTester::FeedPresenceL ),
        ENTRY( "CancelAll", CContactPresenceApiTester::CancelAll )
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::CreateContactPresenceL
// 
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::CreateContactPresenceL( CStifItemParser& /*aItem*/ )
    {
    MContactPresence* contactpresence = TContactPresenceFactory::NewContactPresenceL( *iContactPresenceHandler );
    contactpresence->Close();
    contactpresence = NULL;
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::CreateTestServicesL( CStifItemParser& /*aItem*/ )
    {   

    TInt attribute = 1;
    TInt pluginid = 1;
    TUint launchId = 1;   
    TInt spLaunchId = (TInt)launchId;    
    
    AddServiceEntryL( _L("MSN"), _L("1"), pluginid, attribute, spLaunchId );   
    return KErrNone;    
    } 

// -----------------------------------------------------------------------------
//
void CContactPresenceApiTester::AddServiceEntryL(const TDesC& aServiceName, const TDesC& aBrandingId,  TInt& aPluginUid,  
    TInt& aServiceAttribute, TInt& aLaunchId )
    {
    iLog->Log( _L( "CCreatorServer2::AddServiceEntryL"));         

    CSPEntry* entry = CSPEntry::NewLC();
    TInt err = entry->SetServiceName( aServiceName );
    
    CSPProperty* brandId = CSPProperty::NewLC();  // << brandId
    err = brandId->SetName( EPropertyBrandId );
    brandId->SetValue( aBrandingId );
    err = entry->AddPropertyL( *brandId );
    CleanupStack::PopAndDestroy( brandId ); 

    CSPProperty* pluginId = CSPProperty::NewLC();  // << pluginId
    err = pluginId->SetName( EPropertyPCSPluginId );
    pluginId->SetValue( aPluginUid );
    err = entry->AddPropertyL( *pluginId );
    CleanupStack::PopAndDestroy( pluginId ); 

    if (aServiceAttribute!=0)
        {
        CSPProperty* serviceatt = CSPProperty::NewLC();  // << brandId
        err = serviceatt->SetName( EPropertyServiceAttributeMask );
        serviceatt->SetValue( aServiceAttribute );
        err = entry->AddPropertyL( *serviceatt );        
        CleanupStack::PopAndDestroy( serviceatt ); 
        }
    
    CSPProperty* launchId = CSPProperty::NewLC();  // << launchId
    err = launchId->SetName( ESubPropertyIMLaunchUid );
    launchId->SetValue( aLaunchId);
    err = entry->AddPropertyL( *launchId );
    CleanupStack::PopAndDestroy( launchId );     
            
    err = iSettings->AddEntryL( *entry );
         
    CleanupStack::PopAndDestroy( entry ); // >>> entry   
    }   
    
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::CreateTestContactsL( CStifItemParser& /*aItem*/ )
    {   
    iLog->Log( _L(" OpenStoresL calls iPhonebook->CreateContactL") );      
    iPhonebook->CreateContactL( iPhonebookHandler->iStatus );
    iLog->Log( _L(" FetchContacL activates iPhonebookHandler") );     
    iPhonebookHandler->Activate( EPbHandlerCreateContact );
    return KErrNone;    
    } 

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::OpenStoresL
// 
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::OpenStoresL( CStifItemParser& /*aItem*/ )
    {
    iLog->Log( _L(" OpenStoresL calls iPhonebook->InitStoresL") );      
    iPhonebook->InitStoresL( iPhonebookHandler->iStatus );
    iLog->Log( _L(" FetchContacL activates iPhonebookHandler") );     
    iPhonebookHandler->Activate( EPbHandlerOpenStores );
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::FetchContactL
// 
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::FetchContactL( CStifItemParser& aItem )
    {
    iLog->Log( _L(" FetchContacL begins") );    
    TPtrC numDesc;
    aItem.GetString( _L( "Contact:" ), numDesc );
    iLog->Log( _L(" FetchContacL calls iPhoneBook") );      
    iPhonebook->FetchContactL( numDesc, iPhonebookHandler->iStatus );
    iLog->Log( _L(" FetchContacL activates iPhonebookHandler") );     
    iPhonebookHandler->Activate( EPbHandlerFetchContact );
    iLog->Log( _L(" FetchContacL ends") );      
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::GetIconInfoL
// 
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::GetIconInfoL( CStifItemParser& aItem )
    {    
    iExpectedResult = KErrNone; 
    
    TBool isGet(EFalse);
    
    TPtrC getOnce;
    aItem.GetString( _L( "GetOnce:" ), getOnce );
    TInt noSubscrbe = TesterAtoi( getOnce ) ;    
    if ( noSubscrbe >= 0 )
        {
        isGet = ETrue;
        }      
            
    TPtrC cancelP;    
    aItem.GetString( _L( "Cancel:" ), cancelP ); 
    TInt cancelI = TesterAtoi( cancelP );
    
    TPtrC mgrP;    
    aItem.GetString( _L( "MgrGiven:" ), mgrP ); 
    TInt managerGiven = TesterAtoi( mgrP );   
                               
    if ( !iContactPresence ) 
        {
        if ( managerGiven > 0 )
            {
            iContactPresence = TContactPresenceFactory::NewContactPresenceL( *iManager, *iContactPresenceHandler );              
            }
        else
            {
            iContactPresence = TContactPresenceFactory::NewContactPresenceL( *iContactPresenceHandler );              
            }      
        }
    
    if ( isGet )
        {
        TInt opId = iContactPresence->GetPresenceInfoL( iLinkArray->At(0) );         
        }
    else
        {
        iContactPresence->SubscribePresenceInfoL( *iLinkArray );        
        }    
    
    if ( cancelI == 1 )
        {
        iContactPresence->CancelSubscribePresenceInfo( *iLinkArray );
        // Callback observer will not be called, so complete the test here.
        RequestComplete( KErrNone );       
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::GetBrandedIconL
// 
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::GetBrandedIconL( CStifItemParser& aItem )
    {
    _LIT8( KECEDefaultBrandId_test, "test");
    _LIT8( KElementIdExists, "person:notavailable:image");
    
    iExpectedResult = KErrNone;      
            
    TPtrC cancelP;    
    aItem.GetString( _L( "Cancel:" ), cancelP ); 
    TInt cancelI = TesterAtoi( cancelP );    
        
    if ( !iContactPresence ) 
        {
        iContactPresence = TContactPresenceFactory::NewContactPresenceL( *iContactPresenceHandler );        
        }    
    iContactPresence->SetPresenceIconSize( TSize(15,15) );
    

    TInt opId = iContactPresence->GetPresenceIconFileL( KECEDefaultBrandId_test, KElementIdExists );
    if ( cancelI > 0 )
        {
        iContactPresence->CancelOperation( opId );
        // Callback observer will not be called, so complete the test here.
        RequestComplete( KErrNone );          
        }
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CContactPresenceApiTester::DoWaitSec
//
// SECONDS
// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::DoWaitSec( CStifItemParser& aItem )
    {
    
    iExpectedResult = KErrNone;    

    //Get time SECONDS
    TInt seconds;
    if( aItem.GetNextInt(seconds) != KErrNone )
    	{
        return KErrArgument;
    	}

    DoDoWaitSec( seconds );
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::FeedPresenceL( CStifItemParser& aItem )
    {
    TPtrC text;
    TPtrC identity;
    TPtrC serviceId;
    TInt availability = 0;
    aItem.GetString( _L( "Text:" ), text );
    aItem.GetString( _L( "Identity:" ), identity );
    aItem.GetString( _L( "ServiceId:" ), serviceId );
    aItem.GetInt( _L( "Availability:" ), availability );

    MPresenceBuddyInfo2::TAvailabilityValues value( ( MPresenceBuddyInfo2::TAvailabilityValues )availability );
    
    if ( !iWriter )
        { 
        iWriter = MPresenceCacheWriter2::CreateWriterL();       
        }
        
    MPresenceBuddyInfo2* buddy = MPresenceBuddyInfo2::NewLC();
    buddy->SetIdentityL( identity );
    buddy->SetAvailabilityL( value, text);

    iWriter->WritePresenceL( buddy ); 
    CleanupStack::PopAndDestroy( ); // buddy
         
    return KErrNone;     
    }

// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::CleanFeedPresenceL( CStifItemParser& aItem )
    {
    TPtrC serviceId;
    aItem.GetString( _L( "ServiceId:" ), serviceId );
    
    iWriter->DeleteService( serviceId );
    delete iWriter;
    iWriter = NULL;    
    return KErrNone;  
    }

// -----------------------------------------------------------------------------
//
TInt CContactPresenceApiTester::CancelAll( CStifItemParser& /*aItem*/ )
    {
    if ( iContactPresence )
        {
        iContactPresence->CancelAll();      
        } 
    return KErrNone;      
    }
        

//  End of File
