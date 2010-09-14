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
* Description: 
*       
*
*/


// INCLUDE FILES
#include <cntdb.h>
#include <e32std.h>
#include <spentry.h>
#include <flogger.h>
#include <barsread.h>
#include <vpbkeng.rsg>
#include <spsettings.h>
#include <spproperty.h>
#include <spdefinitions.h>
#include <MVPbkFieldType.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h> 
#include <MVPbkContactViewBase.h>
#include <VPbkContactStoreUris.h>
#include <TVPbkFieldTypeMapping.h>
#include <CVPbkDefaultAttribute.h>
#include <MVPbkContactOperationBase.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <MVPbkContactFieldBinaryData.h>
#include "contactdata.h"
#include "creatorserver.h"
#include "creatordefines.h"
#include "cmstestercontactcreator.h"
#include <MVPbkContactFieldUriData.h>

// ----------------------------------------------------------
// CCreatorServer::CCreatorServer
// 
// ----------------------------------------------------------
//
CCreatorServer::CCreatorServer()
    {
    iContactCountXsp = 0;
    iContactCountDefault = 0;
    }

// ----------------------------------------------------------
// CCreatorServer::New
// 
// ----------------------------------------------------------
//
CCreatorServer* CCreatorServer::NewL( const TDesC& aStoreUri )
    {
    CCreatorServer* self = new CCreatorServer();
    CleanupStack::PushL( self );
    self->ConstructL( aStoreUri );
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CCreatorServer::New
// 
// ----------------------------------------------------------
//
void CCreatorServer::ConstructL( const TDesC& aStoreUri )
    {
    User::LeaveIfError( iFileSession.Connect() );
    iContactCreator = CCmsTesterContactCreator::NewL( iFileSession, aStoreUri );
    using namespace VPbkContactStoreUris;
    iDefaultStore = aStoreUri.Compare( DefaultCntDbUri() ) == 0 ? ETrue : EFalse;
    }

// ----------------------------------------------------------
// CCreatorServer::CCreatorServer
// 
// ----------------------------------------------------------
//
CCreatorServer::~CCreatorServer()
    {
    if( iContactView ) 
        iContactView->RemoveObserver( *this );
    delete iContact;
    delete iContactView;
    delete iContactCreator;
    iFileSession.Close();
    }

// ----------------------------------------------------
// CCreatorServer::FileSession
// 
// ----------------------------------------------------
//
RFs& CCreatorServer::FileSession()
    {
    return iFileSession;
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::CreateContactsL
// 
// ----------------------------------------------------------
//
void CCreatorServer::CreateContactsL()
    {
    iContactCreator->InitStoresL( *this );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ContactViewReady
// 
// ----------------------------------------------------------
//
void CCreatorServer::ContactViewReady( MVPbkContactViewBase& aView )
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer::ContactViewReady() - Contact count: %d" ), aView.ContactCountL() );
	#endif

	iContactView = &aView;
    if( iContactView->ContactCountL() <= 0  )
    	{
    	iDefaultStore ? CreateDefaultContactL() : CreateXSPContactL();
    	}
    else CActiveScheduler::Stop();
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::CreateXSPContactL
// 
// ----------------------------------------------------------
//
void CCreatorServer::CreateXSPContactL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CCreatorServer::CreateXSPContactL()" ) );
#endif

    MVPbkContactStore& store = iContactCreator->ContactStore();
    CVPbkContactManager& manager = iContactCreator->ContactManager();
    iContact = store.CreateNewContactLC();
    CleanupStack::Pop();  //iContact

    const MVPbkFieldType* fieldType1 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_FIRSTNAME );
    MVPbkStoreContactField* field1 = iContact->CreateFieldLC( *fieldType1 );
    MVPbkContactFieldTextData::Cast( field1->FieldData() ).SetTextL( TPtrC( KFirstNameArrayXSP[iContactCountXsp] ) );
    iContact->AddFieldL( field1 );
    CleanupStack::Pop();  //field1
            
    const MVPbkFieldType* fieldType2 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME  );
    MVPbkStoreContactField* field2 = iContact->CreateFieldLC( *fieldType2 );
    MVPbkContactFieldTextData::Cast( field2->FieldData() ).SetTextL( TPtrC( KLastNameArrayXSP[iContactCountXsp] ) );
    iContact->AddFieldL( field2 );
    CleanupStack::Pop();  //field2
   
    
    const MVPbkFieldType* landPhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LANDPHONEGEN );
    MVPbkStoreContactField* landPhoneField = iContact->CreateFieldLC( *landPhoneType );
    MVPbkContactFieldTextData::Cast( landPhoneField->FieldData() ).SetTextL( TPtrC( KLandNumberArrayXSP[iContactCountXsp] ) );
    iContact->AddFieldL( landPhoneField );
    CleanupStack::Pop();  //landPhoneField
       
    for( TInt j = 0; j < KMobilePhoneCountXSP; j++ )
        {
        const MVPbkFieldType* mobilePhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN );
        MVPbkStoreContactField* mobilePhoneField = iContact->CreateFieldLC( *mobilePhoneType );
        MVPbkContactFieldTextData::Cast( mobilePhoneField->FieldData() ).SetTextL( TPtrC( KMobileNumberArrayXSP[iContactCountXsp*KMobilePhoneCountXSP + j] ) );
        iContact->AddFieldL( mobilePhoneField );
        CleanupStack::Pop();  //mobilePhoneField
        }
    
    const MVPbkFieldType* emailGenType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILGEN );
    MVPbkStoreContactField* emailGenField = iContact->CreateFieldLC( *emailGenType );
    SetDefaultAttributeL( emailGenField, EVPbkDefaultTypeEmail );
    MVPbkContactFieldTextData::Cast( emailGenField->FieldData() ).SetTextL( TPtrC( KEmailAddressArrayXSP[iContactCountXsp] ) );
    iContact->AddFieldL( emailGenField );
    CleanupStack::Pop();  //emailGenField
    
    const MVPbkFieldType* imppType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    MVPbkStoreContactField* imppField = iContact->CreateFieldLC( *imppType );        
    MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( imppField->FieldData() );
    uri.SetUriL( TPtrC( KImppAddressArrayXSP[iContactCountXsp] ));
    iContact->AddFieldL( imppField );
    CleanupStack::Pop();  // imppField
        
    iContact->CommitL( *this );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::CreateDefaultContactL
// 
// ----------------------------------------------------------
//
void CCreatorServer::CreateDefaultContactL()
    {
#ifdef _DEBUG
    WriteToLog( _L8( "CCreatorServer::CreateDefaultContactL()" ) );
#endif

    MVPbkContactStore& store = iContactCreator->ContactStore();
    CVPbkContactManager& manager = iContactCreator->ContactManager();
    iContact = store.CreateNewContactLC();
    CleanupStack::Pop();  //iContact

    const MVPbkFieldType* fieldType1 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_FIRSTNAME );
    MVPbkStoreContactField* field1 = iContact->CreateFieldLC( *fieldType1 );
    MVPbkContactFieldTextData::Cast( field1->FieldData() ).SetTextL( TPtrC( KFirstNameArray[iContactCountDefault] ) );
    iContact->AddFieldL( field1 );
    CleanupStack::Pop();  //field1
            
    const MVPbkFieldType* fieldType2 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME  );
    MVPbkStoreContactField* field2 = iContact->CreateFieldLC( *fieldType2 );
    MVPbkContactFieldTextData::Cast( field2->FieldData() ).SetTextL( TPtrC( KLastNameArray[iContactCountDefault] ) );
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
    MVPbkContactFieldTextData::Cast( landPhoneField->FieldData() ).SetTextL( TPtrC( KLandNumberArray[iContactCountDefault] ) );
    iContact->AddFieldL( landPhoneField );
    CleanupStack::Pop();  //landPhoneField
       
    for( TInt j = 0; j < KMobilePhoneCount; j++ )
        {
        const MVPbkFieldType* mobilePhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_MOBILEPHONEGEN );
        MVPbkStoreContactField* mobilePhoneField = iContact->CreateFieldLC( *mobilePhoneType );
        MVPbkContactFieldTextData::Cast( mobilePhoneField->FieldData() ).SetTextL( TPtrC( KMobileNumberArray[iContactCountDefault*KMobilePhoneCount + j] ) );
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
            case 3:
                // Need to do the following ?
                // SetDefaultAttributeL( mobilePhoneField, EVPbkDefaultType<xxx> );
                break;
            }
        iContact->AddFieldL( mobilePhoneField );
        CleanupStack::Pop();  //mobilePhoneField
        }
    
    const MVPbkFieldType* videoType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_VIDEONUMBERGEN );
    MVPbkStoreContactField* videoField = iContact->CreateFieldLC( *videoType );
    SetDefaultAttributeL( videoField, EVPbkDefaultTypeVideoNumber );
    MVPbkContactFieldTextData::Cast( videoField->FieldData() ).SetTextL( TPtrC( KVideoNumberArray[iContactCountDefault] ) );
    iContact->AddFieldL( videoField );
    CleanupStack::Pop();  //videoField

    const MVPbkFieldType* emailGenType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILGEN );
    MVPbkStoreContactField* emailGenField = iContact->CreateFieldLC( *emailGenType );
    SetDefaultAttributeL( emailGenField, EVPbkDefaultTypeEmail );
    MVPbkContactFieldTextData::Cast( emailGenField->FieldData() ).SetTextL( TPtrC( KEmailAddressArray[iContactCountDefault] ) );
    iContact->AddFieldL( emailGenField );
    CleanupStack::Pop();  //emailGenField

    const MVPbkFieldType* emailHomeType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILHOME );
    MVPbkStoreContactField* emailHomeField = iContact->CreateFieldLC( *emailHomeType );
    MVPbkContactFieldTextData::Cast( emailHomeField->FieldData() ).SetTextL( TPtrC( KEmailAddressArray[iContactCountDefault] ) );
    iContact->AddFieldL( emailHomeField );
    CleanupStack::Pop();  //emailHomeField

    const MVPbkFieldType* emailWorkType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_EMAILWORK );
    MVPbkStoreContactField* emailWorkField = iContact->CreateFieldLC( *emailWorkType );
    MVPbkContactFieldTextData::Cast( emailWorkField->FieldData() ).SetTextL( TPtrC( KEmailAddressArray[iContactCountDefault] ) );
    iContact->AddFieldL( emailWorkField );
    CleanupStack::Pop();  //emailWorkField
    
    const MVPbkFieldType* sipType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_SIP );
    MVPbkStoreContactField* sipField = iContact->CreateFieldLC( *sipType );
    MVPbkContactFieldTextData::Cast( sipField->FieldData() ).SetTextL( TPtrC( KSipAddressArray[iContactCountDefault] )  );
    iContact->AddFieldL( sipField );
    CleanupStack::Pop();  //sipField

    TPtrC bitMap = TPtrC( KThumbnailImageArray[iContactCountDefault]);
    if ( bitMap.Length() > 0 )
        {
        const MVPbkFieldType* thumbnailType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_THUMBNAILPIC );
        MVPbkStoreContactField* thumbnailField = iContact->CreateFieldLC( *thumbnailType );
        MVPbkContactFieldBinaryData::Cast( thumbnailField->FieldData() ).SetBinaryDataL( BinaryDataLC( TPtrC( KThumbnailImageArray[iContactCountDefault] ) )->Des() );
        iContact->AddFieldL( thumbnailField );
        CleanupStack::PopAndDestroy();  //BinaryDataLC()
        CleanupStack::Pop();  //thumbnailField
        }

    // Anniversary
    TTime time;
    time.HomeTime();
    const MVPbkFieldType* anniversaryType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_ANNIVERSARY );
    MVPbkStoreContactField* anniversaryField = iContact->CreateFieldLC( *anniversaryType );
    MVPbkContactFieldDateTimeData::Cast( anniversaryField->FieldData() ).SetDateTime( time );
    iContact->AddFieldL( anniversaryField );
    CleanupStack::Pop();  //anniversaryField

    const MVPbkFieldType* imppType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    MVPbkStoreContactField* imppField = iContact->CreateFieldLC( *imppType );        
    MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( imppField->FieldData() );
    uri.SetUriL( TPtrC( KImppAddressArray[iContactCountDefault] ));
    iContact->AddFieldL( imppField );
    CleanupStack::Pop();  // imppField

    iContact->CommitL( *this );
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::DoCreateImppContactL
// 
// ----------------------------------------------------------
//
void CCreatorServer::DoCreateImppContactL()
    {
    #ifdef _DEBUG
        WriteToLog( _L8( "CCreatorServer::DoCreateImppContactL()" ) );
    #endif

    MVPbkContactStore& store = iContactCreator->ContactStore();
    CVPbkContactManager& manager = iContactCreator->ContactManager();
    iContact = store.CreateNewContactLC();
    CleanupStack::Pop();  //iContact

    const MVPbkFieldType* fieldType1 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_FIRSTNAME );
    MVPbkStoreContactField* field1 = iContact->CreateFieldLC( *fieldType1 );
    MVPbkContactFieldTextData::Cast( field1->FieldData() ).SetTextL( TPtrC( KFirstNameArray[iContactCountDefault] ) );
    iContact->AddFieldL( field1 );
    CleanupStack::Pop();  //field1
            
    const MVPbkFieldType* fieldType2 = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME  );
    MVPbkStoreContactField* field2 = iContact->CreateFieldLC( *fieldType2 );
    MVPbkContactFieldTextData::Cast( field2->FieldData() ).SetTextL( TPtrC( KLastNameArray[iContactCountDefault] ) );
    iContact->AddFieldL( field2 );
    CleanupStack::Pop();  //field2
    
    
    const MVPbkFieldType* landPhoneType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_LANDPHONEGEN );
    MVPbkStoreContactField* landPhoneField = iContact->CreateFieldLC( *landPhoneType );
    SetDefaultAttributeL( landPhoneField, EVPbkDefaultTypePhoneNumber );
    MVPbkContactFieldTextData::Cast( landPhoneField->FieldData() ).SetTextL( TPtrC( KLandNumberArray[iContactCountDefault] ) );
    iContact->AddFieldL( landPhoneField );
    CleanupStack::Pop();  //landPhoneField
             
    /*
    if ( TPtrC(KVoIPAddressArray[iContactCountDefault]).Length() )
        {
        const MVPbkFieldType* voipWorkType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_VOIPWORK );
        MVPbkStoreContactField* voipWorkField = iContact->CreateFieldLC( *voipWorkType );
        MVPbkContactFieldTextData::Cast( voipWorkField->FieldData() ).SetTextL( TPtrC( KVoIPAddressArray[iContactCountDefault] ) );
        iContact->AddFieldL( voipWorkField );
        CleanupStack::Pop();  //voipWorkField        
        }
    */

    if ( TPtrC(KImppAddressArray[iContactCountDefault]).Length() )
        {
        // Wanna test with non existig field as well, the first test person.
        const MVPbkFieldType* imppType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
        MVPbkStoreContactField* imppField = iContact->CreateFieldLC( *imppType );        
        MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( imppField->FieldData() );
        uri.SetUriL( TPtrC( KImppAddressArray[iContactCountDefault] ));
        iContact->AddFieldL( imppField );
        CleanupStack::Pop();  // imppField           
        }
    
    if ( TPtrC(KGoogleAddressArray[iContactCountDefault]).Length() )
        {
        // Wanna test with non existig field as well, the first test person.
        const MVPbkFieldType* imppType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
        MVPbkStoreContactField* imppField = iContact->CreateFieldLC( *imppType );        
        MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( imppField->FieldData() );
        uri.SetUriL( TPtrC( KGoogleAddressArray[iContactCountDefault] ));
        iContact->AddFieldL( imppField );
        CleanupStack::Pop();  // imppField           
        }    
    
    if ( TPtrC(KOviAddressArray[iContactCountDefault]).Length() )
        {
        // Wanna test with non existig field as well, the first test person.
        const MVPbkFieldType* imppType = manager.FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
        MVPbkStoreContactField* imppField = iContact->CreateFieldLC( *imppType );        
        MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( imppField->FieldData() );
        uri.SetUriL( TPtrC( KOviAddressArray[iContactCountDefault] ));
        iContact->AddFieldL( imppField );
        CleanupStack::Pop();  // imppField           
        }    

    iContact->CommitL( *this );
    }


// ----------------------------------------------------------
// CCmsTesterContactCreator::BinaryDataL
// 
// ----------------------------------------------------------
//
HBufC8* CCreatorServer::BinaryDataLC( const TDesC& aFilePath )
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
// CCreatorServer::SetDefaultAttributesL
// 
// ----------------------------------------------------------
//
void CCreatorServer::SetDefaultAttributeL( MVPbkStoreContactField* aField,  TInt aDefaultType ) 
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
void CCreatorServer::ContactOperationCompleted( TContactOpResult aResult )
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer::ContactOperationCompleted()" ) );
	#endif
	TInt code( aResult.iOpCode );
    delete iContact;
    iContact = NULL;
    if( iDefaultStore )
        {
        iContactCountDefault++;
        if( iContactCountDefault < KNumberOfDefaultContacts )
            {
            (iContactCountDefault < KNumberOfDefaultImppContacts)
                ? CreateDefaultContactL()
                : DoCreateImppContactL();
            }
        else
            {
            CActiveScheduler::Stop();
            #ifdef _DEBUG
                WriteToLog( _L8( "Scheduler stopped" ) );
            #endif
            }
        }
    else
        {
        iContactCountXsp++;
        if ( iContactCountXsp < KNumberOfXSPContacts )
            {
            CreateXSPContactL();
            }
        else
            {
            CActiveScheduler::Stop();
            #ifdef _DEBUG
                WriteToLog( _L8( "Scheduler stopped" ) );
            #endif
            }
        }
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::AttributeOperationComplete
// 
// ----------------------------------------------------------
//
void CCreatorServer::AttributeOperationComplete( MVPbkContactOperationBase& aOperation )
    {
    delete &aOperation;
    CActiveScheduler::Stop();
    }

// ----------------------------------------------------------
// CCreatorServer::AttributeOperationFailed
// 
// ----------------------------------------------------------
//
void CCreatorServer::AttributeOperationFailed( MVPbkContactOperationBase& aOperation, TInt /*aError*/ )
    {
    delete &aOperation;
    CActiveScheduler::Stop();
    }

// ----------------------------------------------------------
// CCreatorServer::ContactOperationFailed
// 
// ----------------------------------------------------------
//
void CCreatorServer::ContactOperationFailed( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {
    #ifdef _DEBUG
	    WriteToLog( _L8( "CCreatorServer::ContactOperationFailed() - Error: %d" ), aErrorCode );
	#endif
    CActiveScheduler::Stop();
    #ifdef _DEBUG
	    WriteToLog( _L8( " Scheduler stopped" ) );
	#endif
    }

// ----------------------------------------------------
// CCreatorServer::WriteToLog
// 
// ----------------------------------------------------
//
void CCreatorServer::WriteToLog( TRefByValue<const TDesC8> aFmt,... )
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
void CCreatorServer::ContactViewUnavailable( MVPbkContactViewBase& /*aView*/ )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::~CCmsTesterContactCreator
// 
// ----------------------------------------------------------
//
void CCreatorServer::ContactAddedToView( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/, 
                                         const MVPbkContactLink& /*aContactLink*/ )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::~CCmsTesterContactCreator
// 
// ----------------------------------------------------------
//
void CCreatorServer::ContactRemovedFromView( MVPbkContactViewBase& /*aView*/, TInt /*aIndex*/, 
                                             const MVPbkContactLink& /*aContactLink*/ )
    {
    }

// ----------------------------------------------------------
// CCmsTesterContactCreator::ContactViewError
// 
// ----------------------------------------------------------
//
void CCreatorServer::ContactViewError( MVPbkContactViewBase& /*aView*/, TInt /*aError*/, TBool /*aErrorNotified*/ )
    {
    }

// ----------------------------------------------------
// CCreatorServer::StartThread
// 
// ----------------------------------------------------
//
TInt CCreatorServer::StartThread()
    {
	User::LeaveIfError( User::RenameThread( KCreatorServerName ) );
	CActiveScheduler* scheduler = new ( ELeave ) CActiveScheduler;
	CleanupStack::PushL( scheduler );
    if( !CActiveScheduler::Current() )
        {
        CActiveScheduler::Install( scheduler );
        }
    CCreatorServer* server = CCreatorServer::NewL( KCmsCreatorDbUri );
    CleanupStack::PushL( server );
    server->CreateContactsL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy();  //server
	server = NULL;
	TInt exists( CreateXSPStoreL() );
	//CContactDatabase::DeleteDatabaseL( KCmsXSPStore );
	if( KErrNone == exists )
	    {
	    TRAPD( error, server = CCreatorServer::NewL( KCmsContactStore ) );
	    CleanupStack::PushL( server );
	    TRAP( error, server->CreateContactsL() );
	    if( KErrNone == error )
	        {
	        CActiveScheduler::Start();
	        }
	    CleanupStack::PopAndDestroy();  //server
	    }
    CleanupStack::PopAndDestroy();  //scheduler
    RProcess::Rendezvous( KErrNone );
	return KErrNone;
    }

// ----------------------------------------------------
// CCreatorServer::CreateXSPStoreL
// 
// ----------------------------------------------------
//
TInt CCreatorServer::CreateXSPStoreL()
    {
    TInt err = KErrNone;
    CContactDatabase* database = NULL;
    TBool exists( CContactDatabase::ContactDatabaseExistsL( KCmsXSPStore ) );
    if( !exists )
        {
        TRAP( err, database = CContactDatabase::CreateL( KCmsXSPStore ) );
        delete database;
        database = NULL;
        }
    return exists ? KErrAlreadyExists : KErrNone;
    }




    

