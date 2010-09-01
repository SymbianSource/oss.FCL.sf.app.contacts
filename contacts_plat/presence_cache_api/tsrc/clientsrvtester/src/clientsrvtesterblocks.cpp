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
* Description:  ?Description
*
*/



// INCLUDE FILES
// #include <hal.h>
#include <e32svr.h>
#include <StifParser.h>
#include <StifItemParser.h>
#include <Stiftestinterface.h>
#include "ClientSrvtester.h"

#include <presencecachewriter2.h>
#include <presencecachereader2.h>
#include <mpresencebuddyinfo2.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ClientSrvTester::Delete
//  
// -----------------------------------------------------------------------------
//
void CClientSrvTester::DeleteMe() 
    {
    delete iWriter;
    iWriter = NULL; 
    delete iReader;
    iReader = NULL;
    iBuddyArray.ResetAndDestroy();
    iBuddyArray.Close();
    delete iExpiredBuddyId;
    iExpiredBuddyId = NULL;
    }

// -----------------------------------------------------------------------------
// ClientSrvTester::RunMethodL
// 
// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::RunMethodL( CStifItemParser& aItem ) 
    {
    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "SaveBuddyL", CClientSrvTester::SaveBuddyL ),
        ENTRY( "FetchBuddyL", CClientSrvTester::FetchBuddyL ),
        ENTRY( "FetchAllBuddiesL", CClientSrvTester::FetchAllBuddiesL ),        
        ENTRY( "SubscribeBuddyL", CClientSrvTester::SubscribeBuddyL ),
        ENTRY( "UnsubscribeBuddyL", CClientSrvTester::UnsubscribeBuddyL ),         
        ENTRY( "SetNextDataL", CClientSrvTester::SetNextDataL ),
        ENTRY( "CheckCountersL", CClientSrvTester::CheckCountersL),
        ENTRY( "DeleteServiceL", CClientSrvTester::DeleteServiceL),
        ENTRY( "DeleteBuddyL", CClientSrvTester::DeleteBuddyL),
        ENTRY( "WriteMultiplePresenceL", CClientSrvTester::WriteMultiplePresenceL),         
        ENTRY( "VerifyThatBuddyNotFoundL", CClientSrvTester::VerifyThatBuddyNotFoundL )
        };

    const TInt count = sizeof( KFunctions ) / sizeof( TStifFunctionInfo );
    return RunInternalL( KFunctions, count, aItem );
    }

// -----------------------------------------------------------------------------
// ClientSrvTester::SaveBuddyL
// 
// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::SaveBuddyL( CStifItemParser& aItem )
    {
    TPtrC buddyId;
    aItem.GetString( _L( "Buddy:" ), buddyId );
    TPtrC AText;
    aItem.GetString( _L( "Atext:" ), AText );
    TPtrC AValue;
    aItem.GetString( _L( "Avalue:" ), AValue );
    TPtrC avatar;
    aItem.GetString( _L( "avatar:" ), avatar ); 
    TPtrC statusText;
    aItem.GetString( _L( "Statustext:" ), statusText );  
    TPtrC newKey;
    aItem.GetString( _L( "Newkey:" ), newKey ); 
    TPtrC newValue;
    aItem.GetString( _L( "Newvalue:" ), newValue ); 
    TBuf8<100> my8byteBuffer;
    
    TPtrC nbr_keys;
    aItem.GetString( _L( "Nbr_keys:" ), nbr_keys ); 
    TInt keyNumber = TesterAtoi( nbr_keys );  
    
    TPtrC multiple;
    aItem.GetString( _L( "Multiple:" ), multiple ); 
    TInt saveMultiple = TesterAtoi( multiple ); 
    
    TPtrC manyTimes;
    aItem.GetString( _L( "ManyTimes:" ), manyTimes ); 
    iWriteManyTimes = TesterAtoi( manyTimes );     

    TPtrC expiryPtr;
    aItem.GetString( _L( "Expiry:" ), expiryPtr ); 
    TInt64 expiry( 0 );
    if( expiryPtr.Length() > 0 )
        {
        TLex lex( expiryPtr );
        User::LeaveIfError( lex.Val( expiry ) );    
        }
    
    // TInt avValue = TesterAtoi( AValue );
    MPresenceBuddyInfo2::TAvailabilityValues intAval = (MPresenceBuddyInfo2::TAvailabilityValues)TesterAtoi( AValue );      
    
    if ( !iWriter )
        {
        iWriter = MPresenceCacheWriter2::CreateWriterL();
        }
        
    MPresenceBuddyInfo2* buddy = MPresenceBuddyInfo2::NewLC();
    
    // Another way to initiate buddy info
    // -------------------------------------
    MPresenceBuddyInfo2* buddy2 = iWriter->NewBuddyPresenceInfoLC();
    CleanupStack::PopAndDestroy( ); // buddy2  
    buddy2 = NULL;    
    // -------------------------------------
    buddy->SetIdentityL( buddyId );
    buddy->SetAvailabilityL( intAval, AText);

    if ( statusText.Length() )
        {
        buddy->SetStatusMessageL( statusText);
        }
    if ( newKey.Length() || newValue.Length())
        {
        my8byteBuffer.Copy( newValue );         
        buddy->SetAnyFieldL( newKey, my8byteBuffer );
        } 
    if( 0 != expiry )
        {
        TPckg<TInt64> expiryPck( expiry );
        buddy->SetAnyFieldL( NPresenceCacheFieldName::KExpiry, expiryPck );
        }
    
    if ( saveMultiple == 1 )
        {
        iBuddyArray.AppendL( buddy);
        CleanupStack::Pop( ); // buddy           
        }
    else
        {
        iWriter->WritePresenceL( buddy ); 
        
        if ( iWriteManyTimes > 0 )
            {
            for (TInt i=1; i < iWriteManyTimes; i++ )
                {
                // write another time immediately in a row
                buddy->SetAvailabilityL( intAval, _L("ANOTHER EXTRA WRITE"));
                iWriter->WritePresenceL( buddy );                 
                }            
            
            }
        else
            {
            // May be -1
            iWriteManyTimes = 0;
            }
        CleanupStack::PopAndDestroy( ); // buddy             
        }  
   
          
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// ClientSrvTester::FetchBuddyL
// 
// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::FetchBuddyL( CStifItemParser& aItem )
    {
    TPtrC buddyId;
    aItem.GetString( _L( "Buddy:" ), buddyId );
    TPtrC AText;
    aItem.GetString( _L( "Atext:" ), AText );
    TPtrC AValue;
    aItem.GetString( _L( "Avalue:" ), AValue );
    TPtrC avatar;
    aItem.GetString( _L( "avatar:" ), avatar ); 
    TPtrC statusText;
    aItem.GetString( _L( "Statustext:" ), statusText );  
    TPtrC newKey;
    aItem.GetString( _L( "Newkey:" ), newKey ); 
    TPtrC newValue;
    aItem.GetString( _L( "Newvalue:" ), newValue ); 
    TBuf8<100> my8byteBuffer;
    
    TPtrC nbr_keys;
    aItem.GetString( _L( "Nbr_keys:" ), nbr_keys ); 
    TInt keyNumber = TesterAtoi( nbr_keys );      

    TPtrC expiryPtr;
    aItem.GetString( _L( "Expiry:" ), expiryPtr ); 
    TInt64 expiry( 0 );
    if( expiryPtr.Length() > 0 )
        {
        TLex lex( expiryPtr );
        User::LeaveIfError( lex.Val( expiry ) );    
        }
    
    // TInt avValue = TesterAtoi( AValue );
    MPresenceBuddyInfo2::TAvailabilityValues intAval = (MPresenceBuddyInfo2::TAvailabilityValues)TesterAtoi( AValue );       
    
    if ( !iReader )
        {
        iReader = MPresenceCacheReader2::CreateReaderL();
        }
    
    MPresenceBuddyInfo2* buddy = iReader->PresenceInfoLC(buddyId);   
    
    
    if ( buddy->BuddyId().Compare(buddyId) )
        {
        return KErrGeneral;
        }
    if ( buddy->Availability() != intAval )
        {
        return KErrGeneral;
        }    
    if ( buddy->AvailabilityText().Compare(AText) )
        {
        return KErrGeneral;
        }
             
    if ( statusText.Length())
        {
        if (buddy->StatusMessage().CompareF( statusText ))
            {
            return KErrGeneral;            
            }            
        }  
    if ( newKey.Length() || newValue.Length())
        {
        my8byteBuffer.Copy( newValue ); 
        if ( buddy->GetAnyField( newKey ).CompareF( my8byteBuffer ))
            {
            return KErrGeneral;            
            }            
        }
    
    if( expiryPtr.Length() > 0 )
        {
        TInt64 expiryBuddy;
        TPckg<TInt64> expiryPck( expiryBuddy );
        expiryPck.Copy( buddy->GetAnyField( 
                         NPresenceCacheFieldName::KExpiry ) );
        TL(expiry == expiryBuddy);
        }
    
    // Test non-existing field too
    my8byteBuffer = _L8("");    
    my8byteBuffer = buddy->GetAnyField( _L("x-no-key") );
    if ( my8byteBuffer.Length())
        {
        return KErrGeneral;            
          
        }    
           
    CDesCArrayFlat* fieldArray = new ( ELeave ) CDesCArrayFlat( 5 );    
    buddy->GetFieldKeysL( *fieldArray );    
    
    if ( fieldArray->Count() != keyNumber )
        {
        return KErrGeneral;            
        }
    
    fieldArray->Reset();
    delete fieldArray; fieldArray = NULL;
    
    CleanupStack::PopAndDestroy(); // budd
         
    return KErrNone;
    }


// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::FetchAllBuddiesL( CStifItemParser& aItem )
    {
    TPtrC service;
    aItem.GetString( _L( "Service:" ), service );
    
    TPtrC nbr_buddies;
    aItem.GetString( _L( "Nbr_buddies:" ), nbr_buddies ); 
    iBuddiesNumber = TesterAtoi( nbr_buddies );
    
    TPtrC doCancel;
    aItem.GetString( _L( "Cancel:" ), doCancel ); 
    TInt makeCancel = TesterAtoi( doCancel );    
   
    if ( !iReader )
        {
        iReader = MPresenceCacheReader2::CreateReaderL();
        }
    
    TInt ret = iReader->AllBuddiesPresenceInService( service, this );
    
    if (makeCancel == 1)
        {
        delete iReader;
        iReader = NULL;
        }
   
    return ret;
    }


// -----------------------------------------------------------------------------
// ClientSrvTester::SetNextDataL
// 
// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::SetNextDataL( CStifItemParser& aItem )
    {
    TPtrC buddyId;
    aItem.GetString( _L( "Buddy:" ), buddyId );
    TPtrC AText;
    aItem.GetString( _L( "Atext:" ), AText );
    TPtrC AValue;
    aItem.GetString( _L( "Avalue:" ), AValue );
    TPtrC avatar;
    aItem.GetString( _L( "avatar:" ), avatar ); 
    TPtrC statusText;
    aItem.GetString( _L( "Statustext:" ), statusText );  
    TPtrC newKey;
    aItem.GetString( _L( "Newkey:" ), newKey ); 
    TPtrC newValue;
    aItem.GetString( _L( "Newvalue:" ), newValue ); 
    
    TPtrC nbr_keys;
    aItem.GetString( _L( "Nbr_keys:" ), nbr_keys ); 
    TInt keyNumber = TesterAtoi( nbr_keys ); 
    
    TPtrC setCheckState;
    aItem.GetString( _L( "Check:" ), setCheckState ); 
    iCheckState = TesterAtoi( setCheckState );    
    
    TBuf8<100> my8byteBuffer;      
    if ( newKey.Length() || newValue.Length())
        {
        my8byteBuffer.Copy( newValue ); 
        }
    
    delete iBuddyId;
    iBuddyId = NULL;    
    iBuddyId = buddyId.AllocL();
      
    delete iAText;
    iAText = NULL;    
    iAText = AText.AllocL();
    
    TInt aValNumber = TesterAtoi( AValue );      
    iAvailability = (MPresenceBuddyInfo2::TAvailabilityValues)aValNumber;     
    
    delete iStatusText;
    iStatusText = NULL;    
    iStatusText = statusText.AllocL();
           
    delete iNewKey;
    iNewKey = NULL;    
    iNewKey = newKey.AllocL();
    
    delete iNewValue;
    iNewValue = NULL;    
    iNewValue = my8byteBuffer.AllocL();    
    
    iKeyNumber = keyNumber;  
    
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// ClientSrvTester::SubscribeBuddyL
// 
// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::SubscribeBuddyL( CStifItemParser& aItem )
    {
    TPtrC buddyId;
    aItem.GetString( _L( "Buddy:" ), buddyId );
    
    TPtrC deleteWriter;
    aItem.GetString( _L( "DeleteWriter:" ), deleteWriter ); 
     
    TPtrC expiry;
    aItem.GetString( _L( "Expiry:" ), expiry ); 
    
    if ( !iReader )
        {
        iReader = MPresenceCacheReader2::CreateReaderL();
        }
           
    TInt error = iReader->SetObserverForSubscribedNotifications( this );
    if ( error )
        {
        return KErrGeneral;
        }
    
    error = iReader->SubscribePresenceBuddyChangeL( buddyId );  
    if ( error )
        {
        return KErrGeneral;
        }  
    
    if ( deleteWriter.Length() )
        {
        delete iReader;
        iReader = NULL;
        }
    
    if( expiry.Length() )
        {
        delete iExpiredBuddyId;
        iExpiredBuddyId = NULL;
        iExpiredBuddyId = buddyId.AllocL();
        }
    
    return KErrNone;
    }

// -------------------------------------------------------------------------
//
TInt CClientSrvTester::UnsubscribeBuddyL( CStifItemParser& aItem )
    {
    TInt error(KErrNone);    
    TPtrC buddyId;    
    aItem.GetString( _L( "Buddy:" ), buddyId );
     
    if ( !iReader )
        {
        iReader = MPresenceCacheReader2::CreateReaderL();
        error = iReader->SetObserverForSubscribedNotifications( this );
        if ( error )
            {
            return KErrGeneral;
            }        
        }
               
    iReader->UnSubscribePresenceBuddyChangeL( buddyId );      
                
    return KErrNone;
    }

// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::DeleteBuddyL( CStifItemParser& aItem )
    {
    TPtrC buddyId;
    aItem.GetString( _L( "Buddy:" ), buddyId );
     
    if ( !iWriter )
        {
        iWriter = MPresenceCacheWriter2::CreateWriterL();
        }
    TInt error = iWriter->DeletePresenceL( buddyId );                      
    return error;
    }

// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::WriteMultiplePresenceL( CStifItemParser& /*aItem*/ )
    {
     
    if ( !iWriter )
        {
        iWriter = MPresenceCacheWriter2::CreateWriterL();
        }
    TInt error = iWriter->WriteMultiplePresenceL( iBuddyArray, this );                     
    return error;
    }

// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::CheckCountersL( CStifItemParser& aItem )
    {
    // Services: x Buddies: 0 BuddiesInService: 0 Service: sip
    TPtrC services;
    aItem.GetString( _L( "Services:" ), services );
    TInt nbrServices = TesterAtoi( services ); 
    
    TPtrC service;
    aItem.GetString( _L( "Service:" ), service );
    
    TPtrC buddies;
    aItem.GetString( _L( "Buddies:" ), buddies );
    TInt nbrBuddies = TesterAtoi( buddies );
    
    TPtrC buddiesInService;
    aItem.GetString( _L( "BuddiesInService:" ), buddiesInService );
    TInt nbrBuddiesInService = TesterAtoi( buddiesInService );    
    
    TInt ret = KErrNone; 
    TInt val = 0;    
    
    if ( !iReader )
        {
        iReader = MPresenceCacheReader2::CreateReaderL();
        }
    
    if ( nbrServices >= 0 )
        {
        val = iReader->ServicesCount();
        if ( val != nbrServices )
            {
            ret--;             
            }
        }
    
    if ( nbrBuddies >= 0 )
        {
        val = iReader->BuddyCountInAllServices();
        if ( val != nbrBuddies )
            {
            ret--;             
            }
        } 
    
    if ( service.Length() )
        {
        val = iReader->BuddyCountInService( service );
        if ( !nbrBuddiesInService && val == KErrNotFound )
            {
            val = 0;
            }
        if ( val != nbrBuddiesInService )
            {
            ret--;             
            }
        }    
    
    return ret;    
    }

// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::DeleteServiceL( CStifItemParser& aItem )
    {
    TPtrC service;
    aItem.GetString( _L( "Service:" ), service );
    
    TInt ret = KErrNone;   
    
    if ( !iWriter )
        {
        iWriter = MPresenceCacheWriter2::CreateWriterL();
        }

    ret = iWriter->DeleteService( service );
    
    TInt retShould = 0;
    if ( !service.Compare( _L("nonexists")))
        {
        retShould = -1;
        }
        
    return (ret == retShould) ? KErrNone : ret ;    
    }

// -----------------------------------------------------------------------------
// ClientSrvTester::HandlePresenceReadL
// 
// -----------------------------------------------------------------------------
//

void CClientSrvTester::HandlePresenceReadL(TInt aErrorCode,
    RPointerArray<MPresenceBuddyInfo2>& aPresenceBuddyInfoList)
    {
    TInt size = aPresenceBuddyInfoList.Count();
    
    if ( size != iBuddiesNumber )
        {
        aErrorCode = KErrCorrupt;
        }
    if ( iBuddiesNumber==0 && aErrorCode==KErrNotFound )
        {
        aErrorCode = KErrNone;
        }
    
    TBuf16<50> testBuffer;
    MPresenceBuddyInfo2* buddy = NULL;    
    // Take ownership
    for ( TInt i=0; i<size; i++ )
        {  
        buddy = aPresenceBuddyInfoList[0];
        testBuffer = buddy->BuddyId();
        // Breakpoint: check
        testBuffer = buddy->AvailabilityText();
        delete aPresenceBuddyInfoList[0];
        aPresenceBuddyInfoList.Remove(0);        
        }   
    RequestComplete(  aErrorCode );    
    }

// -----------------------------------------------------------------------------
//
TInt CClientSrvTester::VerifyThatBuddyNotFoundL( CStifItemParser& aItem )
    {
    TPtrC buddyId;
    aItem.GetString( _L( "Buddy:" ), buddyId );

    if ( !iReader )
        {
        iReader = MPresenceCacheReader2::CreateReaderL();
        }
    
    MPresenceBuddyInfo2* buddy = iReader->PresenceInfoLC(buddyId);   
    if( buddy )
        {
        CleanupStack::PopAndDestroy(); // buddy
        }
    
    TL( !buddy )
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// ClientSrvTester::HandlePresenceNotificationL
// 
// -----------------------------------------------------------------------------
//
void CClientSrvTester::HandlePresenceNotificationL(TInt aErrorCode,
        MPresenceBuddyInfo2* aPresenceBuddyInfo)
    {
    TInt ret = KErrNone;    
    if ( !aPresenceBuddyInfo )
        {
        ret = aErrorCode;
        RequestComplete( ret );      
        return;
        }
    if( iExpiredBuddyId &&
            iExpiredBuddyId->CompareF( aPresenceBuddyInfo->BuddyId() ) == 0 )
        {
        RequestComplete( ret );
        return;
        }
    
    TBuf<100> testBuffer;
    

    if ( aPresenceBuddyInfo->Availability() != iAvailability )
        {
        ret--;                    
        }                

    testBuffer = aPresenceBuddyInfo->BuddyId();
    if ( aPresenceBuddyInfo->BuddyId().Compare( iBuddyId->Des() ) )
        {
        ret--;                    
        }
    
    if ( aPresenceBuddyInfo->AvailabilityText().Compare( iAText ? iAText->Des() : TPtrC() ) )
        {
        ret--;                    
        } 
    
    if ( aPresenceBuddyInfo->StatusMessage().Compare( iStatusText ? iStatusText->Des() : TPtrC() ) )
        {
        ret--;                    
        }
        
    if ( iNewKey )
        {
        if ( aPresenceBuddyInfo->GetAnyField( iNewKey->Des() ).CompareF( iNewValue ? iNewValue->Des() : TPtrC8() ))
            {
            ret--;
            }            
        } 
    
    CDesCArrayFlat* fieldArray = new ( ELeave ) CDesCArrayFlat( 5 );    
    aPresenceBuddyInfo->GetFieldKeysL( *fieldArray );    
    
    if ( fieldArray->Count() != iKeyNumber )
        {
        ret--;          
        }
    
    fieldArray->Reset();
    delete fieldArray; fieldArray = NULL;
    
    delete aPresenceBuddyInfo;
    
    if ( !iCheckState )
        {
        ret = KErrNone;
        }
       
    /*
    if ( !iWriteManyTimes )
        {
        RequestComplete( ret );         
        }
    else
        {
        iWriteManyTimes--;
        }
        */
    RequestComplete( ret );      
  
    }


void CClientSrvTester::HandlePresenceWriteL(TInt aErrorCode)
    {
    RequestComplete( aErrorCode );      
    }



/*
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksMem, this );
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksHandles, this );
TestModuleIf().SetBehavior( CTestModuleIf::ETestLeaksRequests, this );
TestModuleIf().SetBehavior( CTestModuleIf::EOOMDisableLeakChecks, this );
*/


//  End of File
