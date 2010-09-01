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
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <flogger.h>
#include <barsread.h>
#include <VPbkEng.rsg>
#include <MVPbkFieldType.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h> 
#include <MVPbkContactViewBase.h>
#include <TVPbkFieldTypeMapping.h>
#include <CVPbkDefaultAttribute.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldBinaryData.h>

// service table
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>
#include <spdefinitions.h>

#include "contactdata2.h"
#include "creatorserver2.h"
#include "creatordefines2.h"
#include "cmstestercontactcreator2.h"


#include <MVPbkContactFieldUriData.h>

// ----------------------------------------------------------
// CCreatorServer2::CCreatorServer2
// 
// ----------------------------------------------------------
//
CCreatorServer2::CCreatorServer2()
    {
    }

// ----------------------------------------------------------
// CCreatorServer2::New
// 
// ----------------------------------------------------------
//
CCreatorServer2* CCreatorServer2::NewLC()
    {
    CCreatorServer2* self = new CCreatorServer2();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------
// CCreatorServer2::New
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ConstructL()
    {
    User::LeaveIfError( iFileSession.Connect() );
    iContactCreator = CCmsTesterContactCreator2::NewL( iFileSession );
    
    iSettings = CSPSettings::NewL();    
    }

// ----------------------------------------------------------
// CCreatorServer2::CCreatorServer2
// 
// ----------------------------------------------------------
//
CCreatorServer2::~CCreatorServer2()
    {
    delete iSettings;
        
    if( iContactView ) 
        iContactView->RemoveObserver( *this );
    delete iContact;
    delete iContactView;
    delete iContactCreator;
    iFileSession.Close();
    }

// ----------------------------------------------------
// CCreatorServer2::FileSession
// 
// ----------------------------------------------------
//
RFs& CCreatorServer2::FileSession()
    {
    return iFileSession;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::CreateContactsL
// 
// ----------------------------------------------------------
//
void CCreatorServer2::CreateContactsL()
    {
    iContactCreator->InitStoresL( *this );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ContactViewReady
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ContactViewReady( MVPbkContactViewBase& aView )
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer2::ContactViewReady() - Contact count: %d" ), aView.ContactCountL() );
	#endif
	
	// remove old services
    // Fetch service IDs
    RArray<TServiceId> ids;
    CleanupClosePushL( ids );
    TInt err = iSettings->FindServiceIdsL( ids );
    
    TInt srvCount = ids.Count();
    for ( TInt i=0; i<srvCount; i++ )
    	{
        TRAP_IGNORE( iSettings->DeleteEntryL(ids[i]) ); 	
    	}
    CleanupStack::PopAndDestroy(); // ids

    TInt attribute = 0;
    TInt pluginid = 1;
    TUint launchId = 1;   
    TInt spLaunchId = (TInt)launchId;    
	
	TRAP_IGNORE( AddServiceEntryL( _L("gtalk"), _L("Google"), pluginid, attribute, spLaunchId ));
	attribute = 1;
	TRAP_IGNORE( AddServiceEntryL( _L("voip"), _L("1"), pluginid, attribute, spLaunchId ));
	TRAP_IGNORE( AddServiceEntryL( _L("msn"), _L("10"), pluginid, attribute, spLaunchId )) ;		
	
    iContactView = &aView;
    if( iContactView->ContactCountL() <= 0  )
    	{
#ifdef _MASS_TEST    	    
        DoCreateContact50L(); 
#else        
        DoCreateContactL();
#endif
    	}
    else CActiveScheduler::Stop();
    }
    

// -----------------------------------------------------------------------------
//
void CCreatorServer2::AddServiceEntryL(const TDesC& aServiceName, const TDesC& aBrandingId,  TInt& aPluginUid,  
	TInt& aServiceAttribute, TInt& aLaunchId )
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer2::AddEntryL" ));
	#endif

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


    if(aServiceAttribute!=0)
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
         
    // iServiceId = entry->GetServiceId();


    CleanupStack::PopAndDestroy( entry ); // >>> entry   
    }    

// ----------------------------------------------------------
// CCmsTesterContactCreator::DoCreateContactL
// 
// ----------------------------------------------------------
//
void CCreatorServer2::DoCreateContactL()
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer2::DoCreateContactL()" ) );
	#endif
    MVPbkContactStore& store = iContactCreator->ContactStore();
    CVPbkContactManager& manager = iContactCreator->ContactManager();
    iContact = store.CreateNewContactLC();
    CleanupStack::Pop();  //iContact

    const MVPbkFieldType* fieldType1 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_FIRSTNAME );
    MVPbkStoreContactField* field1 = iContact->CreateFieldLC( *fieldType1 );
    MVPbkContactFieldTextData::Cast( field1->FieldData() ).SetTextL( TPtrC( KFirstNameArray[iContactCount] ) );
    iContact->AddFieldL( field1 );
    CleanupStack::Pop();  //field1
            
    const MVPbkFieldType* fieldType2 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME  );
    MVPbkStoreContactField* field2 = iContact->CreateFieldLC( *fieldType2 );
    MVPbkContactFieldTextData::Cast( field2->FieldData() ).SetTextL( TPtrC( KLastNameArray[iContactCount] ) );
    iContact->AddFieldL( field2 );
    CleanupStack::Pop();  //field2
    
    /*
    EVPbkDefaultTypeUndefined,
    EVPbkDefaultTypePhoneNumber, 
    EVPbkDefaultTypeVideoNumber, 
    EVPbkDefaultTypeSms,
    EVPbkDefaultTypeMms, 
    EVPbkDefaultTypeEmail, 
    EVPbkDefaultTypeEmailOverSms,
    EVPbkDefaultTypeVoIP,
    EVPbkDefaultTypePOC,                
    */
    
    const MVPbkFieldType* landPhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LANDPHONEGEN );
    MVPbkStoreContactField* landPhoneField = iContact->CreateFieldLC( *landPhoneType );
    SetDefaultAttributeL( landPhoneField, EVPbkDefaultTypePhoneNumber );
    MVPbkContactFieldTextData::Cast( landPhoneField->FieldData() ).SetTextL( TPtrC( KLandNumberArray[iContactCount] ) );
    iContact->AddFieldL( landPhoneField );
    CleanupStack::Pop();  //landPhoneField
       
       /*
    for( TInt j = 0;j < KMobilePhoneCount;j++ )
        {
        const MVPbkFieldType* mobilePhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN );
        MVPbkStoreContactField* mobilePhoneField = iContact->CreateFieldLC( *mobilePhoneType );
        MVPbkContactFieldTextData::Cast( mobilePhoneField->FieldData() ).SetTextL( TPtrC( KMobileNumberArray[iMobileNbrIndex] ) );
        switch( j )
            {
            case 0:
                SetDefaultAttributeL( mobilePhoneField, EVPbkDefaultTypeSms );
                break;
            case 1:
                SetDefaultAttributeL( mobilePhoneField, EVPbkDefaultTypeMms );
                break;
            case 2:
                SetDefaultAttributeL( mobilePhoneField, EVPbkDefaultTypeEmailOverSms );
                break;
            }
        iContact->AddFieldL( mobilePhoneField );
        CleanupStack::Pop();  //mobilePhoneField
        iMobileNbrIndex++;
        }
        */
    
    const MVPbkFieldType* videoType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_VIDEONUMBERGEN );
    MVPbkStoreContactField* videoField = iContact->CreateFieldLC( *videoType );
    SetDefaultAttributeL( videoField, EVPbkDefaultTypeVideoNumber );
    MVPbkContactFieldTextData::Cast( videoField->FieldData() ).SetTextL( TPtrC( KVideoNumberArray[iContactCount] ) );
    iContact->AddFieldL( videoField );
    CleanupStack::Pop();  //videoField

    const MVPbkFieldType* emailGenType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILGEN );
    MVPbkStoreContactField* emailGenField = iContact->CreateFieldLC( *emailGenType );
    SetDefaultAttributeL( emailGenField, EVPbkDefaultTypeEmail );
    MVPbkContactFieldTextData::Cast( emailGenField->FieldData() ).SetTextL( TPtrC( KEmailAddressArray[iContactCount] ) );
    iContact->AddFieldL( emailGenField );
    CleanupStack::Pop();  //emailGenField

/*
    const MVPbkFieldType* emailHomeType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILHOME );
    MVPbkStoreContactField* emailHomeField = iContact->CreateFieldLC( *emailHomeType );
    MVPbkContactFieldTextData::Cast( emailHomeField->FieldData() ).SetTextL( TPtrC( KEmailAddressArray[iContactCount] ) );
    iContact->AddFieldL( emailHomeField );
    CleanupStack::Pop();  //emailHomeField

    const MVPbkFieldType* emailWorkType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILWORK );
    MVPbkStoreContactField* emailWorkField = iContact->CreateFieldLC( *emailWorkType );
    MVPbkContactFieldTextData::Cast( emailWorkField->FieldData() ).SetTextL( TPtrC( KEmailAddressArray[iContactCount] ) );
    iContact->AddFieldL( emailWorkField );
    CleanupStack::Pop();  //emailWorkField
  */  
    const MVPbkFieldType* voipGenType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_VOIPGEN );
    MVPbkStoreContactField* voipGenField = iContact->CreateFieldLC( *voipGenType );
    MVPbkContactFieldTextData::Cast( voipGenField->FieldData() ).SetTextL( TPtrC( KVoIPAddressArray[iContactCount] ) );
    iContact->AddFieldL( voipGenField );
    CleanupStack::Pop();  //voipGenField

/*
    const MVPbkFieldType* voipHomeType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_VOIPHOME );
    MVPbkStoreContactField* voipHomeField = iContact->CreateFieldLC( *voipHomeType );
    MVPbkContactFieldTextData::Cast( voipHomeField->FieldData() ).SetTextL( TPtrC( KVoIPAddressArray[iContactCount] ) );
    SetDefaultAttributeL( voipHomeField, EVPbkDefaultTypeVoIP );
    iContact->AddFieldL( voipHomeField );
    CleanupStack::Pop();  //voipHomeField

    const MVPbkFieldType* voipWorkType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_VOIPWORK );
    MVPbkStoreContactField* voipWorkField = iContact->CreateFieldLC( *voipWorkType );
    MVPbkContactFieldTextData::Cast( voipWorkField->FieldData() ).SetTextL( TPtrC( KVoIPAddressArray[iContactCount] ) );
    iContact->AddFieldL( voipWorkField );
    CleanupStack::Pop();  //voipWorkField
*/      
    
    const MVPbkFieldType* sipType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_SIP );
    MVPbkStoreContactField* sipField = iContact->CreateFieldLC( *sipType );
    MVPbkContactFieldTextData::Cast( sipField->FieldData() ).SetTextL( TPtrC( KSipAddressArray[iContactCount] )  );
    iContact->AddFieldL( sipField );
    CleanupStack::Pop();  //sipField
    
    /*
    TPtrC bitMap = TPtrC( KThumbnailImageArray[iContactCount]);
    if (bitMap.Length() > 0 )
    {
        const MVPbkFieldType* thumbnailType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_THUMBNAILPIC );
        MVPbkStoreContactField* thumbnailField = iContact->CreateFieldLC( *thumbnailType );
        MVPbkContactFieldBinaryData::Cast( thumbnailField->FieldData() ).SetBinaryDataL( BinaryDataLC( TPtrC( KThumbnailImageArray[iContactCount] ) )->Des() );
        iContact->AddFieldL( thumbnailField );
        CleanupStack::PopAndDestroy();  //BinaryDataLC()
        CleanupStack::Pop();  //thumbnailField
    }
    */
        

	// Wanna test with non existig field as well, the first test person.
	const MVPbkFieldType* imppType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
	// msn
	MVPbkStoreContactField* imppField = iContact->CreateFieldLC( *imppType );        
	MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( imppField->FieldData() );        
	uri.SetUriL( TPtrC( KImppAddressArray[iContactCount] ));
	iContact->AddFieldL( imppField );
	CleanupStack::Pop();  // imppField
	// gtalk  
	MVPbkStoreContactField* imppField2 = iContact->CreateFieldLC( *imppType ); 
	MVPbkContactFieldUriData& uri2 = MVPbkContactFieldUriData::Cast( imppField2->FieldData() );   
	uri2.SetUriL( TPtrC( KGtalkAddressArray[iContactCount] ));
	iContact->AddFieldL( imppField2 );
	CleanupStack::Pop();  // imppField2        

    
    iContact->CommitL( *this );
    }
    
// ----------------------------------------------------------
// CCmsTesterContactCreator::DoCreateContact50L
// 
// ----------------------------------------------------------
//
void CCreatorServer2::DoCreateContact50L()
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer2::DoCreateContact50L()" ) );
	#endif
	
	_LIT(KMyFirstName, "First" );
	_LIT(KMyGiven, "Given" );
	_LIT(KMyLandPhone, "+3583123456");
	_LIT(KMyMobile, "+35850321654");
	_LIT(KMyVoipBegin,"sip:tester");
	_LIT(KMyVoipEnd,"@ece.com");
	_LIT(KMyMsnBegin, "msn:tester");
	_LIT(KMyMsnEnd,"@msn.com");	
	_LIT(KMyGoogleBegin, "gtalk:tester");
	_LIT(KMyGoogleEnd,"@ece.com");		
	
	TBuf<100> workBuffer;
    TBuf<3>numBuf;
    numBuf.Num( iContactCount );	
			
    MVPbkContactStore& store = iContactCreator->ContactStore();
    CVPbkContactManager& manager = iContactCreator->ContactManager();
    iContact = store.CreateNewContactLC();
    CleanupStack::Pop();  //iContact

    workBuffer = KMyFirstName;    
    workBuffer.Append(numBuf);     
    const MVPbkFieldType* fieldType1 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_FIRSTNAME );
    MVPbkStoreContactField* field1 = iContact->CreateFieldLC( *fieldType1 );
    MVPbkContactFieldTextData::Cast( field1->FieldData() ).SetTextL( workBuffer );
    iContact->AddFieldL( field1 );
    CleanupStack::Pop();  //field1
            
    workBuffer = KMyGiven;    
    workBuffer.Append(numBuf);                
    const MVPbkFieldType* fieldType2 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME  );
    MVPbkStoreContactField* field2 = iContact->CreateFieldLC( *fieldType2 );
    MVPbkContactFieldTextData::Cast( field2->FieldData() ).SetTextL( workBuffer );
    iContact->AddFieldL( field2 );
    CleanupStack::Pop();  //field2
    
    workBuffer = KMyLandPhone;    
    workBuffer.Append(numBuf);      
    const MVPbkFieldType* landPhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LANDPHONEGEN );
    MVPbkStoreContactField* landPhoneField = iContact->CreateFieldLC( *landPhoneType );
    SetDefaultAttributeL( landPhoneField, EVPbkDefaultTypePhoneNumber );
    MVPbkContactFieldTextData::Cast( landPhoneField->FieldData() ).SetTextL( workBuffer );
    iContact->AddFieldL( landPhoneField );
    CleanupStack::Pop();  //landPhoneField
       
    workBuffer = KMyMobile;    
    workBuffer.Append(numBuf);        
    const MVPbkFieldType* mobilePhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN );
    MVPbkStoreContactField* mobilePhoneField = iContact->CreateFieldLC( *mobilePhoneType );
    MVPbkContactFieldTextData::Cast( mobilePhoneField->FieldData() ).SetTextL( workBuffer );
    iContact->AddFieldL( mobilePhoneField );
    CleanupStack::Pop();  //mobilePhoneField

    
    workBuffer = KMyFirstName; 
    workBuffer.Append(numBuf);     
    workBuffer.Append(_L("."));
    workBuffer.Append(KMyGiven);
    workBuffer.Append(numBuf);        
    workBuffer.Append(KMyVoipEnd);   
    const MVPbkFieldType* emailGenType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILGEN );
    MVPbkStoreContactField* emailGenField = iContact->CreateFieldLC( *emailGenType );
    SetDefaultAttributeL( emailGenField, EVPbkDefaultTypeEmail );
    MVPbkContactFieldTextData::Cast( emailGenField->FieldData() ).SetTextL( workBuffer );
    iContact->AddFieldL( emailGenField );
    CleanupStack::Pop();  //emailGenField

    workBuffer = KMyVoipBegin;    
    workBuffer.Append(numBuf);
    workBuffer.Append(KMyVoipEnd);        
    const MVPbkFieldType* voipGenType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_VOIPGEN );
    MVPbkStoreContactField* voipGenField = iContact->CreateFieldLC( *voipGenType );
    MVPbkContactFieldTextData::Cast( voipGenField->FieldData() ).SetTextL( workBuffer );
    iContact->AddFieldL( voipGenField );
    CleanupStack::Pop();  //voipGenField
        
    
    workBuffer = KMyMsnBegin;    
    workBuffer.Append(numBuf); 
    workBuffer.Append(KMyMsnEnd);             
    // Wanna test with non existig field as well, the first test person.
    const MVPbkFieldType* imppType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    // msn
    MVPbkStoreContactField* imppField = iContact->CreateFieldLC( *imppType );        
    MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( imppField->FieldData() );        
    uri.SetUriL( workBuffer);
    iContact->AddFieldL( imppField );
    CleanupStack::Pop();  // imppField
    // gtalk 
    workBuffer = KMyGoogleBegin;    
    workBuffer.Append(numBuf); 
    workBuffer.Append(KMyGoogleEnd);      
    MVPbkStoreContactField* imppField2 = iContact->CreateFieldLC( *imppType ); 
    MVPbkContactFieldUriData& uri2 = MVPbkContactFieldUriData::Cast( imppField2->FieldData() );   
    uri2.SetUriL( workBuffer);
    iContact->AddFieldL( imppField2 );
    CleanupStack::Pop();  // imppField2        
    
    iContact->CommitL( *this );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::BinaryDataL
// 
// ----------------------------------------------------------
//
HBufC8* CCreatorServer2::BinaryDataLC( const TDesC& aFilePath )
    {
    RFile file;
    TInt size = 0;
    RFs fileSession;
    HBufC8* dataBuffer = NULL;
    User::LeaveIfError( fileSession.Connect() );
    CleanupClosePushL( fileSession );
    User::LeaveIfError( file.Open( fileSession, aFilePath, EFileRead ) );
    CleanupClosePushL( file );
    TInt error = file.Size( size );
    if( error == KErrNone )
    	{
    	dataBuffer = HBufC8::NewLC( size );
    	TPtr8 dataDesc( dataBuffer->Des() );
    	User::LeaveIfError( file.Read( dataDesc ) );
    	CleanupStack::Pop();  //dataBuffer
    	}
    CleanupStack::PopAndDestroy( 2 );  //file, fileSession
    if( dataBuffer )
    	{
    	CleanupStack::PushL( dataBuffer );
    	}
    return dataBuffer;
    }

// ----------------------------------------------------------
// CCreatorServer2::SetDefaultAttributesL
// 
// ----------------------------------------------------------
//
void CCreatorServer2::SetDefaultAttributeL( MVPbkStoreContactField* aField,  TInt aDefaultType ) 
    {
    CVPbkContactManager& manager = iContactCreator->ContactManager();
    CVPbkDefaultAttribute* attr = CVPbkDefaultAttribute::NewL( static_cast<TVPbkDefaultType>( aDefaultType ) );
    CleanupStack::PushL( attr );
    manager.ContactAttributeManagerL().SetFieldAttributeL( *aField, *attr, *this );
    CleanupStack::PopAndDestroy();  //attr
    CActiveScheduler::Start();
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ContactOperationCompleted
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ContactOperationCompleted( TContactOpResult /*aResult*/ )
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer2::ContactOperationCompleted()" ) );
	#endif
    iContactCount++;
    if( iContactCount < KNumberOfContacts )
    	{
        delete iContact;
    	iContact = NULL;
#ifdef _MASS_TEST    	    
        DoCreateContact50L(); 
#else        
        DoCreateContactL();
#endif
    	}
    else
    	{
        CActiveScheduler::Stop();
        #ifdef _DEBUG
	        WriteToLog( _L8( " Scheduler stopped" ) );
	    #endif
    	}
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::AttributeOperationComplete
// 
// ----------------------------------------------------------
//
void CCreatorServer2::AttributeOperationComplete( MVPbkContactOperationBase& aOperation )
    {
    delete &aOperation;
    CActiveScheduler::Stop();
    }

// ----------------------------------------------------------
// CCreatorServer2::AttributeOperationFailed
// 
// ----------------------------------------------------------
//
void CCreatorServer2::AttributeOperationFailed( MVPbkContactOperationBase& aOperation, TInt /*aError*/ )
    {
    delete &aOperation;
    CActiveScheduler::Stop();
    }

// ----------------------------------------------------------
// CCreatorServer2::ContactOperationFailed
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ContactOperationFailed( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer2::ContactOperationFailed() - Error: %d" ), aErrorCode );
	#endif
    CActiveScheduler::Stop();
    #ifdef _DEBUG
	    WriteToLog( _L8( " Scheduler stopped" ) );
	#endif
    }

// ----------------------------------------------------
// CCreatorServer2::WriteToLog
// 
// ----------------------------------------------------
//
void CCreatorServer2::WriteToLog( TRefByValue<const TDesC8> aFmt,... )
    {
    VA_LIST list;
    VA_START( list, aFmt );
    TBuf8<KLogBufferMaxSize> buf;
    buf.FormatList( aFmt, list );
    RFileLogger::Write( KCmsCreatorLogDir, KCmsCreatorLogFile, EFileLoggingModeAppend, buf );
    RDebug::RawPrint( buf );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ContactViewUnavailable
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ContactViewUnavailable( MVPbkContactViewBase& /*aView*/ )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::~CCmsTesterContactCreator
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ContactAddedToView( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/, 
                                         const MVPbkContactLink& /*aContactLink*/ )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::~CCmsTesterContactCreator
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ContactRemovedFromView( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/, 
                                             const MVPbkContactLink& /*aContactLink*/ )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ContactViewError
// 
// ----------------------------------------------------------
//
void CCreatorServer2::ContactViewError( MVPbkContactViewBase& /*aView*/, TInt /*aError*/, TBool /*aErrorNotified*/ )
    {
    }

// ----------------------------------------------------
// CCreatorServer2::StartThread
// 
// ----------------------------------------------------
//
TInt CCreatorServer2::StartThread()
    {
	User::LeaveIfError( User::RenameThread( KCreatorServerName ) );
	CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
	CleanupStack::PushL( scheduler );
    if( !CActiveScheduler::Current() )
        {
        CActiveScheduler::Install( scheduler );
        }
    CCreatorServer2* server = CCreatorServer2::NewLC();
    server->CreateContactsL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy( 2 );  //server, scheduler
    RProcess::Rendezvous( KErrNone );
	return KErrNone;
    }


    

