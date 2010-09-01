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


// INCLUDES
#include <bautils.h>
#include <barsread.h>

#include <VPbkEng.rsg>

#include <CVPbkSortOrder.h>
#include <MVPbkContactLink.h>
#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactViewBase.h>
#include <CVPbkContactLinkArray.h>          
#include <MVPbkContactLinkArray.h>
#include <MVPbkContactStoreList.h>
#include <CVPbkContactIdConverter.h>
#include <TVPbkContactStoreUriPtr.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactViewDefinition.h>
#include <MVPbkBaseContactFieldCollection.h>
#include <CVPbkContactStoreUriArray.h>

#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>

#include "contactpresencephonebook.h"



// ----------------------------------------------------------
// CContactPresencePhonebook::CContactPresencePhonebook
// 
// ----------------------------------------------------------
//
CContactPresencePhonebook::CContactPresencePhonebook() : iStoreReady( EFalse ),
                                             iSelectorState( ESelectorIdle )
    {
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::NewL
// 
// ----------------------------------------------------------
//    
CContactPresencePhonebook* CContactPresencePhonebook::NewL()
    {
    CContactPresencePhonebook* self = new ( ELeave ) CContactPresencePhonebook();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::ConstructL
// 
// ----------------------------------------------------------
//        
void CContactPresencePhonebook::ConstructL()
    {
    CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC();
    uriList->AppendL( TVPbkContactStoreUriPtr( KCmsCntDbUri ) );
    iContactManager = CVPbkContactManager::NewL( *uriList, &iFileSession );
    CleanupStack::PopAndDestroy();  //uriList
    iStoreList = &iContactManager->ContactStoresL();
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::~CContactPresencePhonebook
// 
// ----------------------------------------------------------
//
CContactPresencePhonebook::~CContactPresencePhonebook()
    {
    delete iContact;
    delete iLinkArray;
    delete iContactManager;
    }

// ----------------------------------------------------------
//
CVPbkContactManager* CContactPresencePhonebook::ContactManager()
    {
    return iContactManager;
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::InitStoresL
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::InitStoresL( TRequestStatus& aStatus )
    {
    if( !iStoreReady )
        {
        iClientStatus = &aStatus;
        aStatus = KRequestPending;
        iStoreList->OpenAllL( *this );
        iSelectorState = ESelectorInit;
        }
    else
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrNone );
        }
    }


// ----------------------------------------------------------
//
void CContactPresencePhonebook::CreateContactL( TRequestStatus& aStatus )
    {
    if( iStoreReady )
        {
        iClientStatus = &aStatus;
        aStatus = KRequestPending;
        iSelectorState = ESelectorCreateContact;
        DoCreateContactL();
        }
    else
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrCancel );
        }
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::FetchLinkL
// 
// ----------------------------------------------------------
//

HBufC8* CContactPresencePhonebook::FetchLinkLC( TInt32 aContactId )
    {
    CVPbkContactIdConverter* converter = CVPbkContactIdConverter::NewL( *iContactStore );
	CleanupStack::PushL( converter );
    iContactLink = converter->IdentifierToLinkLC( aContactId );
    CleanupStack::Pop();  //iContactLink
    CleanupStack::PopAndDestroy(); //converter
    return iContactLink->PackLC();
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::FetchContactL
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::FetchContactL( TInt32 aContactId, TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;    
    aStatus = KRequestPending;
    CVPbkContactIdConverter* converter = CVPbkContactIdConverter::NewL( *iContactStore );
	CleanupStack::PushL( converter );
    iContactLink = converter->IdentifierToLinkLC( aContactId );
    CleanupStack::Pop();  //iContactLink
    CleanupStack::PopAndDestroy(); //converter
    if( iContactLink )
        {
        iOperation = iContactManager->RetrieveContactL( *iContactLink, *this );
        iSelectorState = ESelectorFetchContact;
        }
    else 
        {
        User::RequestComplete( iClientStatus, KErrNotFound );
        }
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::FetchContactL
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::FetchContactL( const TDesC8& aPackedLink, TRequestStatus& aStatus  )
    {
    ResetData();
    iClientStatus = &aStatus;    
    aStatus = KRequestPending;
    CVPbkContactLinkArray* temp = CVPbkContactLinkArray::NewLC( aPackedLink, *iStoreList );
    iLinkArray = temp;
    // iLinkArray = CVPbkContactLinkArray::NewLC( aPackedLink, *iStoreList );
    iOperation = iContactManager->RetrieveContactL( iLinkArray->At( 0 ), *this );
    iSelectorState = ESelectorFetchContact;
    CleanupStack::Pop();  //iLinkArray
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::FetchContactIDL
// 
// ----------------------------------------------------------
//
TInt32 CContactPresencePhonebook::FetchContactIDL()
    {
    TInt32 contactId = KErrNotFound;
	CVPbkContactIdConverter* converter = CVPbkContactIdConverter::NewL( *iContactStore );
	CleanupStack::PushL( converter );
	contactId = converter->LinkToIdentifier( *iContact->CreateLinkLC() );	
	CleanupStack::PopAndDestroy( 2 ); //CreateLinkLC, converter
    return contactId;
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::FetchContactL
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::FetchContactL( const TDesC& aMsisdn, TRequestStatus& aStatus )
    {
    iClientStatus = &aStatus;    
    aStatus = KRequestPending;
    iOperation = iContactManager->MatchPhoneNumberL( aMsisdn, aMsisdn.Length(), *this );
    iSelectorState = ESelectorFetchContact;
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::Cancel
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::Cancel()
    {
    if( iOperation )
        {
        delete iOperation;
        iOperation = NULL;
        iSelectorState = ESelectorIdle;
        User::RequestComplete( iClientStatus, KErrCancel );
        }
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::OpenComplete
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::OpenComplete()
    {
    if( iClientStatus )
    	{
    	User::RequestComplete( iClientStatus, KErrNone );
    	iSelectorState = ESelectorIdle;
    	iStoreReady = ETrue;
    	}
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::FindCompleteL
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::FindCompleteL( MVPbkContactLinkArray* aResults )
    {
    ResetData();
    iLinkArray = aResults;
    TInt count( aResults->Count() );
    if( count > 0 )
        {
        iOperation = iContactManager->RetrieveContactL( iLinkArray->At( 0 ), *this );
        }
    else
        {
        ResetData();
        User::RequestComplete( iClientStatus, KErrNotFound );
        }
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::FindFailed
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::FindFailed( TInt aError )
    {
    ResetData();
    User::RequestComplete( iClientStatus, aError );
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::StoreReady
// 
// ----------------------------------------------------------
//      
void CContactPresencePhonebook::StoreReady( MVPbkContactStore& aContactStore )
    {
    iContactStore = &aContactStore;    
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::StoreUnavailable
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::StoreUnavailable( MVPbkContactStore& /*aContactStore*/, TInt /*aReason*/ )
    { 
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::HandleStoreEventL
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::HandleStoreEventL( MVPbkContactStore& /*aContactStore*/,
                                             TVPbkContactStoreEvent /*aStoreEvent*/ )
    { 
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::ResetData
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::ResetData()
    {
    delete iLinkArray;
    iLinkArray = NULL;
    delete iOperation;
    iOperation = NULL;
    delete iContactLink;
    iContactLink = NULL;
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::ResetData
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::ResetData( MVPbkContactOperationBase& aOperation )
    {
    delete iLinkArray;
    iLinkArray = NULL;
    if( iOperation == &aOperation )
        {
        delete iOperation;
        iOperation = NULL;
        delete iContactLink;
        iContactLink = NULL;
        }
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::VPbkSingleContactOperationComplete
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::VPbkSingleContactOperationComplete( MVPbkContactOperationBase& aOperation,
                                                                    MVPbkStoreContact* aContact )
    {
    ResetData( aOperation );
    TInt result = reinterpret_cast<TInt>( aContact );
    User::RequestComplete( iClientStatus, result );
    iSelectorState = ESelectorIdle;
    iContact = aContact;
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::~CContactPresencePhonebook
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::VPbkSingleContactOperationFailed( MVPbkContactOperationBase& /*aOperation*/, TInt aError )
    {
    ResetData();
    iSelectorState = ESelectorIdle;
    User::RequestComplete( iClientStatus, aError );
    }

// ----------------------------------------------------------
// CContactPresencePhonebook::DoCreateContactL
// 
// ----------------------------------------------------------
//
void CContactPresencePhonebook::DoCreateContactL()
    {
    
    iContactCount = 1;
    
    _LIT(KMyFirstName, "First" );
    _LIT(KMyGiven, "Given" );
    _LIT(KMyLandPhone, "+35812345678");
    _LIT(KMyMobile, "+35850321654");
    // _LIT(KMyVoipBegin,"sip:tester");
    // _LIT(KMyVoipEnd,"@ece.com");
    _LIT(KMyMsnBegin, "msn:tester");
    _LIT(KMyMsnEnd,"@msn.com"); 
    _LIT(KMyGoogleBegin, "google:tester");
    _LIT(KMyGoogleEnd,"@ece.com");      
    
    TBuf<100> workBuffer;
    TBuf<3>numBuf;
    numBuf.Num( iContactCount );    
            
    MVPbkContactStore& store = *iContactStore;
    CVPbkContactManager& manager = *iContactManager;
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
//
void CContactPresencePhonebook::ContactOperationCompleted( TContactOpResult /*aResult*/ )
    {          
    iContactCount++;
    if( iContactCount < KNumberOfContacts )
        {
        delete iContact;
        iContact = NULL;       
        TRAP_IGNORE( DoCreateContactL() );
        }
    else
        {             
        User::RequestComplete( iClientStatus, KErrNone );            
        }
    }

// ----------------------------------------------------------
//
void CContactPresencePhonebook::ContactOperationFailed( TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/ )
    {         
    User::RequestComplete( iClientStatus, aErrorCode );     
    }


// End of File
