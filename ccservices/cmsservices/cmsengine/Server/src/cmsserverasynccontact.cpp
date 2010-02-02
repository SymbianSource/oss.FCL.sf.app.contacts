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


// INCLUDES
#include <spsettings.h>
#include <spproperty.h>
#include <MVPbkFieldType.h>
#include <MVPbkStoreContact.h>
#include <CVPbkContactManager.h>
#include <presencecachereader2.h>
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>
#include <Pbk2ContactNameFormatterFactory.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkContactFieldDateTimeData.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldBinaryData.h>
#include <CVPbkDefaultAttribute.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreProperties.h>
#include "bpas.h"
#include "cmsserver.h"
#include "cmsdefines.h"
#include "cmsserverutils.h"
#include "cmsservercontact.h"
#include "cmsphonebookproxy.h"
#include "cmsserverasynccontact.h"
#include "cmsserverxspcontacthandler.h"
#include "cmsserverdefines.h"
#include "cmsdebug.h"

// ----------------------------------------------------
// CCmsServerAsyncContact::CCmsServerAsyncContact
// 
// ----------------------------------------------------
//
CCmsServerAsyncContact::CCmsServerAsyncContact( CCmsServerContact& aSyncContact,
                                                CCmsServer& aCmsServer ) :
                                  CActive( EPriorityStandard ),
                                  iSyncContact( aSyncContact ),
                                  iCmsServer( aCmsServer )
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::NewL
// 
// ----------------------------------------------------
//
CCmsServerAsyncContact* CCmsServerAsyncContact::NewL( CCmsServerContact& aSyncContact,
                                                      CCmsServer& aCmsServer )
    {
    CCmsServerAsyncContact* self = new( ELeave ) CCmsServerAsyncContact(
        aSyncContact, aCmsServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::~CCmsServerAsyncContact
// 
// ----------------------------------------------------
//
CCmsServerAsyncContact::~CCmsServerAsyncContact()
    {
    Cancel();
    delete iSortOrderManager;
    delete iNameFormatter;  
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::ConstructL
// 
// ----------------------------------------------------
//
void CCmsServerAsyncContact::ConstructL()
    {
    PRINT( _L("Start CCmsServerAsyncContact::ConstructL()" ) );
    
    iContactManager = iCmsServer.PhonebookProxyHandle().ContactManager();
    
    //for full name

    iSortOrderManager = CPbk2SortOrderManager::NewL( iContactManager->FieldTypes() );            
    iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL
            ( iContactManager->FieldTypes(), *iSortOrderManager, &iCmsServer.FileSession() );
    
    PRINT( _L("End CCmsServerAsyncContact::ConstructL()" ) );
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::ContactDataTextLC
// 
// ----------------------------------------------------
//        
CDesCArrayFlat* CCmsServerAsyncContact::ContactDataTextLC( CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::ContactDataTextL( group )" ) );
    
    CDesCArrayFlat* fieldArray = NULL;
    switch( aFieldGroup )
        {
        case CCmsContactFieldItem::ECmsGroupVoIP:
            {
            const TInt count = sizeof( KCmsVoIPGroup ) / sizeof( KCmsVoIPGroup[0] );
            fieldArray = FieldsFromArrayLC( count, KCmsVoIPGroup );
            break;
            }
        case CCmsContactFieldItem::ECmsGroupVoice:
            {
            const TInt count = sizeof( KCmsVoiceCallGroup ) / sizeof( KCmsVoiceCallGroup[0] );
            fieldArray = FieldsFromArrayLC( count, KCmsVoiceCallGroup );
            break;
            }
        case CCmsContactFieldItem::ECmsGroupEmail:
            {
            const TInt count = sizeof( KCmsEmailAddressGroup ) / sizeof( KCmsEmailAddressGroup[0] );
            fieldArray = FieldsFromArrayLC( count, KCmsEmailAddressGroup );
            break;
            }
        default:
            {
            User::Leave( KErrArgument );
            break;
            }
        }
    PRINT( _L(" End CCmsServerAsyncContact::ContactDataTextL( group )" ) );
        
    return fieldArray;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::ContactDataUriLC
// 
// ----------------------------------------------------
//        
CDesCArrayFlat* CCmsServerAsyncContact::ContactDataUriLC( TInt aResourceId )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::ContactDataUriLC()" ) );
    
    MCmsContactInterface& syncContact = iSyncContact;
    CDesCArrayFlat* fieldArray = new ( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( fieldArray );
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( aResourceId );
    CVPbkBaseContactFieldTypeIterator* fieldIterator = CVPbkBaseContactFieldTypeIterator::NewLC( *type, syncContact.Contact().Fields() );
    while( fieldIterator->HasNext() )
        {
        const MVPbkBaseContactField* field = fieldIterator->Next();
        const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
        fieldArray->AppendL( uri.Uri() );
        }
    CleanupStack::PopAndDestroy();  //fieldIterator
    PRINT( _L(" End CCmsServerAsyncContact::ContactDataUriLC()" ) );
    return fieldArray;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::FieldsFromArray
// 
// ----------------------------------------------------
//        
CDesCArrayFlat* CCmsServerAsyncContact::FieldsFromArrayLC( TInt aCount, const TInt aResourceArray[] )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::FieldsFromArrayLC()" ) );
  
    CDesCArrayFlat* fieldArray = new ( ELeave ) CDesCArrayFlat( aCount );
    CleanupStack::PushL( fieldArray );
    for( TInt i = 0;i < aCount;i++ )
        {
        PhonebookDataL( aResourceArray[i], *fieldArray );
        }
    return fieldArray;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::PhonebookDataL
// 
// ----------------------------------------------------
//        
void CCmsServerAsyncContact::PhonebookDataL( TInt aResourceId, CDesCArrayFlat& aFieldArray )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::PhonebookDataL()" ) );
    
    MCmsContactInterface& syncContact = iSyncContact;
    CVPbkBaseContactFieldTypeIterator* fieldIterator = NULL;
    CCmsServerXSPContactHandler* xspHandler = iSyncContact.XSPHandler();
    const TInt xspCount( xspHandler ? xspHandler->ContactCount() : 0 );
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( aResourceId );
    fieldIterator = CVPbkBaseContactFieldTypeIterator::NewLC( *type, syncContact.Contact().Fields() );
    GetOneFieldL( aFieldArray, *fieldIterator );
    CleanupStack::PopAndDestroy();  //fieldIterator
    for( TInt i = 0; i < xspCount; i++ )
        {
        fieldIterator = CVPbkBaseContactFieldTypeIterator::NewLC( *type, xspHandler->Contact( i )->Fields() );
        GetOneFieldL( aFieldArray, *fieldIterator );
        CleanupStack::PopAndDestroy();  //fieldIterator
        }
    PRINT( _L("End CCmsServerAsyncContact::PhonebookDataL()" ) );
        
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::GetOneFieldL
// 
// ----------------------------------------------------
//        
void CCmsServerAsyncContact::GetOneFieldL( CDesCArrayFlat& aFieldArray, CVPbkBaseContactFieldTypeIterator& aFieldIterator )
    {
    PRINT( _L("Start CCmsServerAsyncContact::GetOneFieldL()" ) );
    
    while( aFieldIterator.HasNext() )
        {
        HBufC* data = ParseContactFieldL( *aFieldIterator.Next() );  
        if( data )
            {
            TInt index( 0 );
            CleanupStack::PushL( data );
            TPtrC descriptor( data->Des() );
            if( aFieldArray.Find( descriptor, index ) != 0 )
                {
                aFieldArray.AppendL( descriptor );
                }
            CleanupStack::PopAndDestroy();  //data
            }
        }
    PRINT( _L("End CCmsServerAsyncContact::GetOneFieldL()" ) );   
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::ContactDataBinaryL
// 
// ----------------------------------------------------
//        
HBufC8* CCmsServerAsyncContact::ContactDataBinaryL( TInt aResourceId )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::ContactDataBinaryL()" ) );
    
    HBufC8* binData = NULL;
    MCmsContactInterface& syncContact = iSyncContact;
    const MVPbkStoreContact& contact = syncContact.Contact();
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( aResourceId );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( *type, contact.Fields() );
    if( itr->HasNext() )
        {
        PRINT( _L( "CCmsServerAsyncContact::ContactDataBinaryL():Phonebook thumbnail available" ) );
        const MVPbkBaseContactField* field = itr->Next();
        const MVPbkContactFieldBinaryData& data = MVPbkContactFieldBinaryData::Cast( field->FieldData() );
        binData = data.BinaryData().AllocL();
        }
    else
        {   
        PRINT( _L( "CCmsServerAsyncContact::ContactDataBinaryL(): Try to fetch avatar" ) );
        
        binData = FetchAvatarL();
        }
    CleanupStack::PopAndDestroy( itr );
    PRINT( _L(" End CCmsServerAsyncContact::ContactDataBinaryL()" ) );
    return binData;
    }

// ----------------------------------------------------------
// CCmsServerAsyncContact::FetchAvatarL
// 
// ----------------------------------------------------------
//
HBufC8* CCmsServerAsyncContact::FetchAvatarL()
    {
    PRINT( _L(" Start CCmsServerAsyncContact::FetchAvatarL()" ) );
    
    TBool found( EFalse );
    HBufC8* avatarBuf = NULL;
    MPresenceCacheReader2* reader = MPresenceCacheReader2::CreateReaderL();
    CleanupDeletePushL( reader );
    CDesCArrayFlat* uriArray( ContactDataUriLC( R_VPBK_FIELD_TYPE_IMPP ) );
    const TInt count( uriArray->MdcaCount() );
    for( TInt i = 0; !found && i < count; i++ )
        {
        MPresenceBuddyInfo2* info = reader->PresenceInfoLC( uriArray->MdcaPoint( i ) );
        if( info )
            {
            TPtrC8 avatarDesc( info->Avatar() );
            PRINT1( _L("CCmsServerAsyncContact::FetchAvatarL(): Avatar data length: %d" ), avatarDesc.Length() );
            avatarBuf = avatarDesc.AllocL();
            CleanupStack::PopAndDestroy();  //info
            found = ETrue;
            }
        }
    CleanupStack::PopAndDestroy( 2 );  //uriArray, reader
    PRINT( _L(" End  CCmsServerAsyncContact::FetchAvatarL()" ) );
    return avatarBuf;
    }

// ----------------------------------------------------------
// CCmsServerAsyncContact::FetchContactdata
// 
// ----------------------------------------------------------
//
void CCmsServerAsyncContact::FetchContactData( CCmsContactFieldItem::TCmsContactField aField )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::FetchContactData()" ) );
    
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, ( TInt )aField );
    iAsyncOperation = SelectOperationType( aField ); 
    SetActive();
    PRINT( _L(" End CCmsServerAsyncContact::FetchContactData()" ) );
    }

// ----------------------------------------------------------
// CCmsServerAsyncContact::FetchEnabledFields
// 
// ----------------------------------------------------------
//
void CCmsServerAsyncContact::FetchEnabledFields()
    {
    PRINT( _L(" Start CCmsServerAsyncContact::FetchEnabledFields()" ) );
    
    iStatus = KRequestPending;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    iAsyncOperation = EFetchEnabledFields;
    SetActive();
    PRINT( _L(" End CCmsServerAsyncContact::FetchEnabledFields()" ) );
    }

// ----------------------------------------------------------
// CCmsServerAsyncContact::SelectOperationType
// 
// ----------------------------------------------------------
//
CCmsServerAsyncContact::TCmsAsyncOperation CCmsServerAsyncContact::SelectOperationType( TInt aCmsField )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::SelectOperationType()" ) );
    
    CCmsServerAsyncContact::TCmsAsyncOperation operation = CCmsServerAsyncContact::ENone;
    switch( aCmsField )
        {
        case CCmsContactFieldItem::ECmsPresenceData:
            {
            operation = CCmsServerAsyncContact::EFetchPresenceData;
            break;
            }
        case CCmsContactFieldItem::ECmsThumbnailPic:
            {
            operation = CCmsServerAsyncContact::EFetchPhonebookBinData;
            break;
            }
        case CCmsContactFieldItem::ECmsImpp:
            {
            operation = CCmsServerAsyncContact::EFetchPhonebookUriData;
        	break;
            }
        case CCmsContactFieldItem::ECmsAnniversary:
            {
            operation = CCmsServerAsyncContact::EFetchPhonebookDateTimeData;
        	break;
            }
        default:
            {
            operation = CCmsServerAsyncContact::EFetchPhonebookTextData;
            break;
            }
        }
    PRINT( _L(" End CCmsServerAsyncContact::SelectOperationType()" ) );    
    return operation;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::DoGetEnabledFieldsLC
// 
// ----------------------------------------------------
//
HBufC* CCmsServerAsyncContact::GetEnabledFieldsLC()
    {
    TInt bufferIndex = 0;
    CBufFlat* dataBuffer = CBufFlat::NewL( 50 );
    CleanupStack::PushL( dataBuffer );
    MCmsContactInterface& syncContact = iSyncContact;
    CCmsServerXSPContactHandler* xspHandler = iSyncContact.XSPHandler();
    const TInt xspCount( xspHandler ? xspHandler->ContactCount() : 0 );
    DoGetEnabledFieldsL( bufferIndex, dataBuffer, syncContact.Contact() );
    for( TInt i = 0; i < xspCount; i++ )
        {
        DoGetEnabledFieldsL( bufferIndex, dataBuffer, *xspHandler->Contact( i ) );
        }
    //Fullname is always available. Pbhk default name used atleast. 
    TBuf8<32> fullnameBuffer( _L8( "" ) );
    fullnameBuffer.AppendNum( CCmsContactFieldItem::ECmsFullName );
    fullnameBuffer.Append( KDelimiter );
    dataBuffer->InsertL( 0, fullnameBuffer );

    //Make thumbnail to be always "available". There may also be
    //an avatar available in the presence cache, so we need to 
    //make the client application ask this field explicitly
    TBuf8<32> thumbnailBuffer( _L8( "" ) );
    thumbnailBuffer.AppendNum( CCmsContactFieldItem::ECmsThumbnailPic );
    thumbnailBuffer.Append( KDelimiter );
    dataBuffer->InsertL( 0, thumbnailBuffer );
        
    TInt dataLength( dataBuffer->Size() );
    HBufC8* data8 = HBufC8::NewLC( dataLength );
    TPtr8 desc( data8->Des() );
    dataBuffer->Read( 0, desc, dataLength );
    dataBuffer->Reset();
    PRINT1( _L("CCmsServerAsyncContact::GetEnabledFieldsLC():Enabled fields: %S" ), &desc );
    
    HBufC* data16 = CmsServerUtils::ConvertDescLC( desc );
    CleanupStack::Pop();  //data16
    CleanupStack::PopAndDestroy( 2 );  //data8, dataBuffer
    CleanupStack::PushL( data16 );
    return data16;
    }
        
// ----------------------------------------------------
// CCmsServerAsyncContact::DoGetEnabledFieldsLC
// 
// ----------------------------------------------------
//
void CCmsServerAsyncContact::DoGetEnabledFieldsL( TInt& aBufferIndex, CBufFlat* aDataBuffer,
                                                  const MVPbkStoreContact& aContact )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::DoGetEnabledFieldsLC()" ) );
    
    TBuf8<32> itemBuffer( _L8( "" ) );
    const MVPbkBaseContactFieldCollection& fieldCollection( aContact.Fields() );    
    TInt fieldCount = fieldCollection.FieldCount();
    for( TInt i = 0; i < fieldCount; i++ )
        {
        const MVPbkBaseContactField& fieldBase( fieldCollection.FieldAt( i ) );
        const MVPbkContactFieldData& field( fieldBase.FieldData() );
        const TInt KMatchPriority = 0;
        const MVPbkFieldType* type = fieldBase.MatchFieldType( KMatchPriority );
        if( type && !field.IsEmpty() )
            {
            itemBuffer.Zero();
            CCmsContactFieldItem::TCmsContactField cmsField( CmsServerUtils::MatchFieldType( type->FieldTypeResId() ) );
            itemBuffer.AppendNum( cmsField );
            if( !HasElement( aDataBuffer->Ptr( 0 ), itemBuffer ) )
                {
                itemBuffer.Append( KDelimiter );
                aDataBuffer->InsertL( aBufferIndex, itemBuffer );
                aBufferIndex = aBufferIndex + itemBuffer.Length();
                }
            }
        }
    PRINT( _L(" End CCmsServerAsyncContact::DoGetEnabledFieldsLC()" ) );
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::HasElement
// 
// ----------------------------------------------------
// 
TBool CCmsServerAsyncContact::HasElement( const TDesC8& aBuffer, const TDesC8& aItem )
    {
    TBool match = EFalse;
    TLex8 items( aBuffer );
    TPtrC8 token( _L8( " " ) );
    while( aBuffer.Length() > 0 && !match && 0 < token.Length() )
        {
        token.Set( items.NextToken() );
        if( token.Compare( aItem ) == 0 )
            {
            match = ETrue;
            }
        items.SkipSpace();
        }
    return match;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::DoGetDataLC
// 
// ----------------------------------------------------
//
HBufC* CCmsServerAsyncContact::DoGetDataLC( TInt aResourceId )
    {    
    //If full name is requested
    //Full name is created by using PbhkNameFormatter.
    HBufC* returnData = NULL;
    if( CCmsContactFieldItem::ECmsFullName == aResourceId )
        {  
        returnData = DoGetFullNameDataLC();         
        }
    else    
        {
        returnData = DoGetPhonebookDataLC( aResourceId );
        }
    return returnData;                           
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::FindImAvailability
// 
// ----------------------------------------------------
//
TInt CCmsServerAsyncContact::FindImAvailabilityL()
    {
    TInt result( 0 );
    MCmsContactInterface& syncContact = iSyncContact;
    const MVPbkStoreContact& contact = syncContact.Contact();
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_IMPP );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( *type, contact.Fields() );
    while( itr->HasNext() && result == 0 )
        {
        const MVPbkBaseContactField* field = itr->Next();
        const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( field->FieldData() );
        TPtrC scheme( uri.Scheme() ); 
        if( CmsServerUtils::IsFeatureSupportedL( ESubPropertyIMLaunchUid, scheme ) )//  IsImCapableL( scheme ) )
            {
            result = 1;
            }
        }
    CleanupStack::PopAndDestroy();  //itr
    return result; 
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::DefaultAttributesL
// 
// ----------------------------------------------------
//
TInt CCmsServerAsyncContact::DefaultAttributesL( const MVPbkBaseContactField& aContactField )
    {
    const MVPbkStoreContactField& field = static_cast<const MVPbkStoreContactField&>(aContactField);
    TInt defaultAttributes = 0; 
    TInt type = EVPbkDefaultTypeUndefined;

    while( EVPbkLastDefaultType != type )
        {                                                 
        CVPbkDefaultAttribute* attr = CVPbkDefaultAttribute::NewL( (TVPbkDefaultType)type );
        CleanupStack::PushL( attr );

        if(iContactManager->ContactAttributeManagerL().HasFieldAttributeL( *attr, field ))
            {
            defaultAttributes |= MatchTypesToInternalEnum((TVPbkDefaultType)type);
            }
        
        CleanupStack::PopAndDestroy( attr );        
        type++;            
        }

    return defaultAttributes;
    }

// ----------------------------------------------------------
// CCmsServerAsyncContact::MatchTypesToInternalEnum
//
// ----------------------------------------------------------
//
CCmsContactFieldItem::TCmsDefaultAttributeTypes
    CCmsServerAsyncContact::MatchTypesToInternalEnum( TVPbkDefaultType aDefaultType )
    {
    CCmsContactFieldItem::TCmsDefaultAttributeTypes defaultAttributeType =
        CCmsContactFieldItem::ECmsDefaultTypeUndefined;
    switch( aDefaultType )
        {
        case EVPbkDefaultTypePhoneNumber:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypePhoneNumber;
            break;
            }
        case EVPbkDefaultTypeVideoNumber:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeVideoNumber;
            break;
            }
        case EVPbkDefaultTypeSms:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeSms;
            break;
            }
        case EVPbkDefaultTypeMms:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeMms;
            break;
            }
        case EVPbkDefaultTypeEmail:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeEmail;
            break;
            }
        case EVPbkDefaultTypeEmailOverSms:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeEmailOverSms;
            break; 
            }
        case EVPbkDefaultTypeVoIP:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeVoIP;
            break;
            }
        case EVPbkDefaultTypePOC:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypePOC;
            break;
            }
        case EVPbkDefaultTypeChat:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeImpp;
            break;
            }
        case EVPbkDefaultTypeOpenLink:
            {
            defaultAttributeType = CCmsContactFieldItem::ECmsDefaultTypeUrl;
            break;
            }
        default:
            {
            break;
            }
        }
    return defaultAttributeType;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::IsVoIPCapableL
// 
// ----------------------------------------------------
//
TBool CCmsServerAsyncContact::IsVoipCapableL( const TDesC& aScheme )
    {
    PRINT( _L( "Start CCmsServerContact::IsVoipCapableL()" ) );
    PRINT1( _L( "CCmsServerAsyncContact::IsVoipCapableL():Service provider: %S" ), &aScheme );
    RIdArray ids;
    CleanupClosePushL( ids );
    TBool retValue = EFalse;
    CSPSettings* settings = CSPSettings::NewL();
    CleanupStack::PushL( settings );
    CDesCArrayFlat* nameArray = new ( ELeave ) CDesCArrayFlat( 2 );
    CleanupStack::PushL( nameArray );    
    TInt error = settings->FindServiceIdsL( ids );  
    User::LeaveIfError( error );     
    error = settings->FindServiceNamesL( ids, *nameArray );  
    User::LeaveIfError( error );
    TBool ready = EFalse;   
    const TInt count = nameArray->MdcaCount();
    for ( TInt i = 0; !ready && i < count; i++ )
        {
        TPtrC desc = nameArray->MdcaPoint( i );
        if ( !desc.CompareF( aScheme ) )
            {
            CSPProperty* property = CSPProperty::NewLC();
            error = settings->FindPropertyL( ids[i], EPropertyServiceAttributeMask, *property );
            if ( KErrNone == error )
                {
                TInt value = 0; 
                property->GetValue( value );                    
                if ( value & ESupportsInternetCall )
                    {
                    retValue = ETrue;                
                    }
                }
            CleanupStack::PopAndDestroy();  //property
            break;
            }
        }
    CleanupStack::PopAndDestroy( 3 );  //nameArray, settings, ids
    PRINT( _L( "End CCmsServerContact::IsVoipCapableL()" ) );
    return retValue;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::DoGetPhonebookDataL
// 
// ----------------------------------------------------
//
HBufC* CCmsServerAsyncContact::DoGetPhonebookDataLC( TInt aResourceId )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::DoGetPhonebookDataLC()" ) );
    
    RArray<TInt> defaultAttributeArray;
    CleanupClosePushL( defaultAttributeArray );
    CDesCArrayFlat* itemArray = new ( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( itemArray );    
    CDesCArrayFlat* infoArray = new ( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( infoArray );    
    MCmsContactInterface& syncContact = iSyncContact;
    CCmsServerXSPContactHandler* xspHandler = iSyncContact.XSPHandler();
    const TInt xspCount( xspHandler ? xspHandler->ContactCount() : 0 );
    HandleOneContactL( *itemArray, *infoArray, defaultAttributeArray, aResourceId, syncContact.Contact() );   
    for( TInt i = 0; i < xspCount; i++ )
        {
        HandleOneContactL( *itemArray, *infoArray, defaultAttributeArray, aResourceId, *xspHandler->Contact( i ) );   
        }
    HBufC* retBuffer = ConstructStreamBufferL( *itemArray, *infoArray, defaultAttributeArray );   
        CleanupStack::PopAndDestroy( 3 ); 
    CleanupStack::PushL( retBuffer );
    PRINT( _L(" End CCmsServerAsyncContact::DoGetPhonebookDataLC()" ) );
    return retBuffer;  
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::HandleOneContactL
// 
// ----------------------------------------------------
//
void CCmsServerAsyncContact::HandleOneContactL( CDesCArray& aItemArray, CDesCArray& aInfoArray,  RArray<TInt>& aDefaults,
                                                TInt aResourceId, const MVPbkStoreContact& aContact )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::HandleOneContactL()" ) );
    
    const TDesC& storeName = aContact.ParentStore().StoreProperties().Name().UriDes();    
    
    TInt index( 0 );
    const MVPbkFieldType* type = iContactManager->FieldTypes().Find( aResourceId );
    CVPbkBaseContactFieldTypeIterator* itr = CVPbkBaseContactFieldTypeIterator::NewLC( *type, aContact.Fields() );
    while( itr->HasNext() )
        {
        const MVPbkBaseContactField* field = itr->Next();
        HBufC* item = ParseContactFieldL( *field );
        CleanupStack::PushL( item );
        TPtrC descriptor( item->Des() );
        if( aItemArray.Find( descriptor, index ) != 0 )
            {
            aItemArray.AppendL( descriptor );
            aInfoArray.AppendL( storeName );  
            //Reading default attributes.
            aDefaults.Append( DefaultAttributesL( *field ) );
            }
        CleanupStack::PopAndDestroy();   //item
        }
    CleanupStack::PopAndDestroy();  //itr
    PRINT( _L(" End CCmsServerAsyncContact::HandleOneContactL()" ) );
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::HandleOneContactL
// 
// ----------------------------------------------------
//
HBufC* CCmsServerAsyncContact::ConstructStreamBufferL( CDesCArray& aItemArray, CDesCArray& aInfoArray, RArray<TInt>& aDefaults )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::ConstructStreamBufferL()" ) );
    
    RBuf16 descBuffer;
        TBuf<15> lengthBuf;
    descBuffer.Create( 12 );
    descBuffer.CleanupClosePushL();
    const TInt count( aItemArray.MdcaCount() );
    for( TInt i = 0; i < count; i++ )
        {
        lengthBuf.Zero();
        TPtrC item( aItemArray.MdcaPoint( i ) );
        TPtrC info( aInfoArray.MdcaPoint( i ) );
        lengthBuf.AppendNum( item.Length() );
        lengthBuf.Append( KDelimiter );
        lengthBuf.AppendNum( info.Length() );
        lengthBuf.Append( KDelimiter );
        lengthBuf.AppendNum( aDefaults[i] );
        lengthBuf.Append( KDelimiter );
        PRINT3(_L( "Item [%d]: %S - Defaults: %W" ), i, &item, aDefaults[i] );
        User::LeaveIfError( descBuffer.ReAlloc( descBuffer.Length() + item.Length() + info.Length() + lengthBuf.Length() ) );
        descBuffer.Append( lengthBuf );
        descBuffer.Append( item );
        descBuffer.Append( info );
        }
    HBufC* retBuffer = descBuffer.AllocL();
    CleanupStack::PopAndDestroy();  //descBuffer
    PRINT( _L(" End CCmsServerAsyncContact::ConstructStreamBufferL()" ) );
    return retBuffer;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::ParseContactFieldL
// 
// ----------------------------------------------------
//
HBufC* CCmsServerAsyncContact::ParseContactFieldL( const MVPbkBaseContactField& aField )
    {
    HBufC* buffer = NULL;
    const TInt KMatchPriority = 0;
    const MVPbkFieldType* type = aField.MatchFieldType( KMatchPriority );
    if( EFetchPhonebookUriData == iAsyncOperation || R_VPBK_FIELD_TYPE_IMPP == type->FieldTypeResId() )
        {   
        const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast( aField.FieldData() );
        buffer = uri.Uri().AllocL(); 
        }
    else if( CCmsServerAsyncContact::EFetchPhonebookDateTimeData == iAsyncOperation )
        {
        TLocale locale;
        TBuf<256> dateBuffer;
        const MVPbkContactFieldDateTimeData& date = MVPbkContactFieldDateTimeData::Cast( aField.FieldData() );
        TTime time( date.DateTime() );
        time.FormatL( dateBuffer, KDateFormat, locale );
        buffer = HBufC::NewL( dateBuffer.Length() );
        buffer->Des().Copy( dateBuffer );
        }
    else
        {
        const MVPbkContactFieldTextData& text = MVPbkContactFieldTextData::Cast( aField.FieldData() );
        buffer = text.Text().AllocL();         
        }
    return buffer;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::DoGetFullNameDataLC
// 
// ----------------------------------------------------
//
HBufC* CCmsServerAsyncContact::DoGetFullNameDataLC()
    {
    RBuf16 descBuffer;
    TBuf<15> lengthBuf;
    _LIT( KDelimiter, " " );
    descBuffer.Create( 12 );
    descBuffer.CleanupClosePushL();
    MCmsContactInterface& syncContact = iSyncContact;    
    const MVPbkStoreContact& contact = syncContact.Contact();
    
    //Creating full name. 
    HBufC* fullname = iNameFormatter->GetContactTitleL
            ( contact.Fields(),
              MPbk2ContactNameFormatter::EPreserveLeadingSpaces |
              MPbk2ContactNameFormatter::EUseSeparator );
    CleanupStack::PushL( fullname );

    //Creating info string
    const TDesC& storeName = contact.ParentStore().StoreProperties().Name().UriDes();    
    
    //CREATING SENDING PROTOCOL
    //Create msg to client side
    lengthBuf.Zero();
    HBufC* item = fullname->Des().AllocL();
    CleanupStack::PopAndDestroy( fullname );   //fullname
    CleanupStack::PushL( item );    
    
    TPtr descriptor( item->Des() );
    lengthBuf.AppendNum( descriptor.Length() );
    lengthBuf.Append( KDelimiter );                     

    //Adding info length                            ?
    lengthBuf.AppendNum( storeName.Length() );
    lengthBuf.Append( KDelimiter );    
    
    //Adding attribute to protocol, No attributes for fullname
    lengthBuf.AppendNum( 0 );
    lengthBuf.Append( KDelimiter );
            
    User::LeaveIfError( descBuffer.ReAlloc( descBuffer.Length() + descriptor.Length() + storeName.Length()+ lengthBuf.Length() ) );
    descBuffer.Append( lengthBuf );
    descBuffer.Append( descriptor );
    descBuffer.Append( storeName );    
    CleanupStack::PopAndDestroy();   //item

    HBufC* retBuffer = descBuffer.AllocL();

    CleanupStack::PopAndDestroy( 1 );  //descBuffer
    CleanupStack::PushL( retBuffer );
        
    return retBuffer;
    }


// ----------------------------------------------------
// CCmsServerAsyncContact::AppendToBufferL
// 
// ----------------------------------------------------
// 
void CCmsServerAsyncContact::AppendToBufferL( RBuf16& aBuffer, const TDesC& aDescriptor,
                                              CCmsContactFieldItem::TCmsContactField aFieldType )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::AppendToBufferL()" ) );
    
    RBuf tempBuffer;
    TBuf<32> typeBuf;
    TBuf<32> lengthBuf;
    _LIT( KDelimiter, " " );
    tempBuffer.Create( KDefaultBufferSize );
    tempBuffer.CleanupClosePushL();
    if( 0 < aDescriptor.Length() )
        {
        typeBuf.Zero();
        typeBuf.AppendNum( aFieldType );
        typeBuf.Append( KDelimiter );
        tempBuffer.ReAllocL( typeBuf.Length() + aDescriptor.Length() );
        tempBuffer.Append( typeBuf );
        tempBuffer.Append( aDescriptor );
        lengthBuf.Zero();
        lengthBuf.AppendNum( tempBuffer.Length() );
        lengthBuf.Append( KDelimiter );
        aBuffer.ReAllocL( aBuffer.Length() + tempBuffer.Length() + lengthBuf.Length() );
        aBuffer.Append( lengthBuf );
        aBuffer.Append( tempBuffer );
        }
    CleanupStack::PopAndDestroy();  //tempBuffer
    PRINT( _L("End CCmsServerAsyncContact::AppendToBufferL()" ) );
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::ContactInfoArrayL
// 
// ----------------------------------------------------
// 
CDesCArrayFlat* CCmsServerAsyncContact::ContactInfoArrayLC()
    {
    PRINT( _L(" Start CCmsServerAsyncContact::ContactInfoArrayL()" ) );
    
    CDesCArrayFlat* idArray = new ( ELeave ) CDesCArrayFlat( 5 );
    CleanupStack::PushL( idArray );
    CDesCArrayFlat* email = ContactDataTextLC( CCmsContactFieldItem::ECmsGroupEmail );  
    AppendFieldsL( *email, *idArray );
    CleanupStack::PopAndDestroy();  //email
    CDesCArrayFlat* voip = ContactDataTextLC( CCmsContactFieldItem::ECmsGroupVoIP );
    AppendFieldsL( *voip, *idArray );
    CleanupStack::PopAndDestroy();  //voip
    PhonebookDataL( R_VPBK_FIELD_TYPE_IMPP, *idArray );
    PRINT( _L(" End CCmsServerAsyncContact::ContactInfoArrayL()" ) );
    return idArray;
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::AppendFieldsL
// 
// ----------------------------------------------------
//
void CCmsServerAsyncContact::AppendFieldsL( const CDesCArrayFlat& aSource, CDesCArrayFlat& aTarget )
    {
    PRINT( _L(" Start CCmsServerAsyncContact::AppendFieldsL()" ) );
    
    const TInt count = aSource.MdcaCount();
    for( TInt i = 0; i < count; i++ )
        {
        aTarget.AppendL( aSource.MdcaPoint( i ) );
        }
    PRINT( _L(" Start CCmsServerAsyncContact::AppendFieldsL()" ) );    
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::RunL
// 
// ----------------------------------------------------
//
void CCmsServerAsyncContact::RunL()
    {
    switch( iAsyncOperation )
        {
        case ENone:
            {
            break;
            }
        case EFetchEnabledFields:
            {
            MCmsContactInterface& syncContact = iSyncContact;
            HBufC* fields = GetEnabledFieldsLC();
            syncContact.HandleEnabledFields( fields ) ? 
   	            CleanupStack::PopAndDestroy() : CleanupStack::Pop();  //fields
            break;
            }
        case EFetchPhonebookTextData:
        case EFetchPhonebookUriData:
        case EFetchPhonebookDateTimeData:
            {
            MCmsContactInterface& syncContact = iSyncContact;
            CCmsContactFieldItem::TCmsContactField field =
                ( CCmsContactFieldItem::TCmsContactField )iStatus.Int();
            HBufC* data = DoGetDataLC( CmsServerUtils::MatchFieldType( field ) );
            syncContact.HandleField( data ) ? 
                CleanupStack::PopAndDestroy() : CleanupStack::Pop();  //data
            break;
            }
        case EFetchPhonebookBinData:
            {
            MCmsContactInterface& syncContact = iSyncContact;
            CCmsContactFieldItem::TCmsContactField field =
                ( CCmsContactFieldItem::TCmsContactField )iStatus.Int();
            HBufC8* data = ContactDataBinaryL( CmsServerUtils::MatchFieldType( field ) );
            CleanupStack::PushL( data );
            syncContact.HandleField( data ) ? 
                CleanupStack::PopAndDestroy() : CleanupStack::Pop();  //data
            break;
            }
        case EFetchPresenceData:
            {
            MBPASObserver* observer = &iSyncContact;
            CBPAS& presence( iSyncContact.BPASHandle() );
            presence.SetObserver( observer );
            CDesCArrayFlat* infoArray = ContactInfoArrayLC();
            presence.GiveInfosL( *infoArray );
            infoArray->Delete( 0, infoArray->MdcaCount() );
            CleanupStack::PopAndDestroy();  //infoArray
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::DoCancel
// 
// ----------------------------------------------------
//
void CCmsServerAsyncContact::DoCancel()
    {
    }

// ----------------------------------------------------
// CCmsServerAsyncContact::RunError
// 
// ----------------------------------------------------
//
TInt CCmsServerAsyncContact::RunError( TInt aError )
    {
    //Handle error 
    MCmsContactInterface& syncContact = iSyncContact;
    syncContact.HandleError( aError );
    return KErrNone;
    }



// End of File
