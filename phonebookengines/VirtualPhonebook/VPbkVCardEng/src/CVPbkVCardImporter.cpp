/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Importing the CParserProperty field from CParserVCard to 
*                contact field.
*
*/

#include "CVPbkVCardImporter.h"
#include "CVPbkVCardPropertyParser.h"
#include "CVPbkVCardAttributeHandler.h"
#include "CVPbkVCardFieldTypeProperty.h"
#include "CVPbkVCardContactFieldIterator.h"
#include "CVPbkVCardData.h"
#include "TVPbkVCardDataConverter.h"
#include "CVPbkVCardContactFieldData.h"

#include <MVPbkStoreContact.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactCopyPolicy.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactStoreProperties.h>
#include <MVPbkContactAttribute.h>
#include <MVPbkContactOperationBase.h>
#include <CVPbkContactManager.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <vpbkeng.rsg>

// System includes
#include <vcard.h>
#include <bldvariant.hrh>
#include <s32buf.h>

#include <VPbkStoreUriLiterals.h>
_LIT8( KPropXSelf, "X-SELF" );
_LIT8( KPropXCategories, "X-CATEGORIES");
_LIT(KColon, ":");

CVPbkVCardImporter* CVPbkVCardImporter::NewL(
            RPointerArray<MVPbkStoreContact>& aImportedContacts,
            RReadStream& aSourceStream,
            MVPbkContactStore& aTargetStore, 
            CVPbkVCardData& aData )  
    {
    CVPbkVCardImporter* self = new (ELeave) CVPbkVCardImporter(
            aImportedContacts, aTargetStore, aData);
    CleanupStack::PushL( self );
    self->ConstructL(aSourceStream);
    CleanupStack::Pop( self );
    return self;
    }
    
CVPbkVCardImporter::CVPbkVCardImporter(
        RPointerArray<MVPbkStoreContact>& aImportedContacts,
        MVPbkContactStore& aTargetStore, 
        CVPbkVCardData& aData ) :
            CActive( EPriorityStandard ), 
            iImportedContacts( aImportedContacts ),
            iTargetStore( aTargetStore ),
            iData( aData )
    {
    CActiveScheduler::Add( this );
    }

CVPbkVCardImporter::~CVPbkVCardImporter()
    {
    Cancel();
   	delete iAddCntContactFieldOperation;
    delete iCntContact;
    if (iCntModelStore)
        {
        iCntModelStore->Close(*this);
        }
    iSourceStream.Close();
    delete iStreamBuffer;
    delete iAddFieldOperation;
    delete iAttributeHandler;
    delete iContactFieldIterator;
    delete iCurContact;
    delete iPropertyParser;
    delete iParser;
    delete iGroupCardValue;
    
    iErasableFields.Close();
    }
    
void CVPbkVCardImporter::ConstructL(RReadStream& aSourceStream)
    {
    
    // Get the source stream size
    MStreamBuf* buf = aSourceStream.Source();    
    TInt streamSize = buf->SizeL();

    // Read the source contents
    iStreamBuffer = HBufC8::NewL(streamSize);  
    TPtr8 ptr(iStreamBuffer->Des());
    aSourceStream.ReadL(ptr, streamSize);    
    
    // Create a new local stream
    iSourceStream.Open(*iStreamBuffer);                                 
    
    iParser = CParserVCard::NewL();
    

    iAttributeHandler = CVPbkVCardAttributeHandler::NewL();    
    iPropertyParser = 
        CVPbkVCardPropertyParser::NewL( iData, *iAttributeHandler );
    iCopyPolicy = &iData.CopyPolicyL( iTargetStore.StoreProperties().Uri() );
    if ( iTargetStore.StoreProperties().Uri().UriDes().CompareC(
                KVPbkSimGlobalAdnURI()) == 0 )
    	{
    	iCntCopyPolicy = &iData.CopyPolicyL( iData.GetCntModelStoreL().StoreProperties().Uri() );
    	}
    iOwnCard = EFalse;
    // Construct an empty string to ensure that iGroupCardValue always exists
    // See also GetGroupcardvalue()
    iGroupCardValue = HBufC::NewL(0);
    }
    
void CVPbkVCardImporter::StartL()
    {    
    iOwnCard = EFalse;
    TRAPD( err, iParser->InternalizeL( iSourceStream ) );
        
    if ( err != KErrEof ) // allow partial vcard data
    {
        User::LeaveIfError( err );
    }    
    
    if ( !iParser->ArrayOfProperties( EFalse ) )
    {
        User::Leave( KErrNotFound );
    }
    
    // This function just starts the process asynchronously.
   (KErrEof == err) ?  StartAsync( EStop ) : StartAsync( EParseNext );
    }
    
void CVPbkVCardImporter::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        switch( iState )
            {
            case EParseNext:
                {
                if ( !iCurContact )
                    {
                    // If contact haven't not been created, create new one.
                    iCurContact = iTargetStore.CreateNewContactLC();
                    CleanupStack::Pop(); // iCurContact
                    if ( iCntCopyPolicy )
                        {                    
                        iCntModelStore = &iData.GetCntModelStoreL();
                        iCntModelStore->OpenL(*this);
                        break;
                        }
                    }
                ParseNextL();
                break;
                }
            case ESaveField:
                {
                SaveFieldL();
                break;
                }
            case ESaveContact:
                {
                SaveContactL();
                break;
                }
            case EStop:
                {
                iObserver->ContactsImportingCompleted();                    
                break;
                }
            default:;
            }
        }
    }
    
TInt CVPbkVCardImporter::RunError( TInt aError )
    {
    HandleError( aError );
    return KErrNone;
    }
    
void CVPbkVCardImporter::DoCancel()
    {
    }

void CVPbkVCardImporter::SetObserver( 
        MVPbkImportOperationObserver& aObserver )
    {
    iObserver = &aObserver;
    }

CParserProperty* CVPbkVCardImporter::NextProperty()
    {
    // Get next property from parser array
    CParserProperty* property = NULL;
    TInt elementCount = iParser->ArrayOfProperties( EFalse )->Count();
    if ( iArrayElementIndex < elementCount ) 
        {        
        property = 
            iParser->ArrayOfProperties( EFalse )->At( iArrayElementIndex );
        ++iArrayElementIndex;
        }
    return property;
    }
    
void CVPbkVCardImporter::ParseNextL()
    {
    delete iContactFieldIterator;
    iContactFieldIterator = NULL;

    CParserProperty* property = NextProperty();
    if ( property )
        {
        if(property->Name().Compare(KPropXSelf())==0)
        {
        	iOwnCard = ETrue;
        }

        if( property->Name().Compare(KPropXCategories())==0)
            {
            iGroupCard = ETrue;
            CParserPropertyValue* value = property->Value();
            // Get the HBufC property
            CParserPropertyValueHBufC* hbufVal = 
            static_cast<CParserPropertyValueHBufC*>(value);
            TPtrC normalVal = hbufVal->Value();
            // Create a modifiable descriptor
            if( iGroupCardValue )
                {
                delete iGroupCardValue;
                iGroupCardValue = NULL;
                }
            iGroupCardValue = normalVal.AllocL();
            }
        // property is owned by iParser->ArrayOfProperties(), we are not
        // responsible of deleting it.
        iContactFieldIterator = iPropertyParser->ParseNextL( property );  
        iState = ESaveField;      
        }
    else
        {
        // No properties anymore, all fields are added. Save contact.
        iState = ESaveContact;
        }
   StartAsync( iState );
   }

TInt CVPbkVCardImporter::FindContactFieldL( 
        MVPbkStoreContact* aContact, 
        TInt aFieldTypeResId )
    {
    if ( aContact )
        {
        const MVPbkStoreContactFieldCollection& fields = aContact->Fields();
        const TInt fieldCount = fields.FieldCount();
    
        const TInt maxMatchPriority = 0;
            iData.GetContactManager().FieldTypes().MaxMatchPriority();
        
        for ( TInt matchPriority = 0;
                matchPriority <= maxMatchPriority; ++matchPriority )
            {
            for ( TInt i = 0; i < fieldCount; ++i )
                {
                const MVPbkFieldType* fieldType =
                        fields.FieldAt(i).MatchFieldType(matchPriority);
                if ( fieldType->FieldTypeResId() == aFieldTypeResId )
                    {
                    return i;
                    }
                }
            }
        }
    
    return KErrNotFound;
    }
        
void CVPbkVCardImporter::SaveFieldL()
    {
    if ( iContactFieldIterator->HasNext() )
        {
        // Get next field data
        CVPbkVCardContactFieldData* data = iContactFieldIterator->NextLC();
        // Save data asynchronously, if data is not saved e.g. empty field
        // this is treated as synchronous function.
        TBool saved = EFalse;
        // SaveDataL may leave when constructing add field operation
        // (e.g. for valid but unsupported in the current build field type)
        // leave should be trapped and saving continue
        TRAPD(err, saved = SaveDataL( *data ));
        CleanupStack::PopAndDestroy( data );
        if ( !saved )
            {
            // If data was not saved try to move next field.
            if ( err == KErrNone )
            	{
            	StartAsync( ESaveField );
            	}
            else
            	{
            	HandleFailedFieldAdditionL( err );
            	}
            }
        }
    else
        {
        StartAsync( EParseNext );
        }
    }
    
TBool CVPbkVCardImporter::SaveDataL( CVPbkVCardContactFieldData& aData )
    {
    // Data value type should be asked from aData
    TBool isSaved ( ETrue );
    TVPbkVCardDataConverter converter;
    if ( aData.Uid() == TUid::Uid( KVersitPropertyHBufCUid ) ||
         aData.Uid() == TUid::Uid( KVersitPropertyCDesCArrayUid ) )
        {
        const TDesC& value = converter.GetDesCData( aData );
        // If it is an IMPP field
        if ( aData.FieldType().FieldTypeResId() == R_VPBK_FIELD_TYPE_IMPP )
            {
            TInt pos = value.Find(KColon);
            if( 0 == pos || KErrNotFound == pos )
                {
                // If the service name or the colon doesn't exist, don't save the data.
                // e.g.:
                // (1) If the value is YAHOO:peterpan@yahoo.com, YAHOO is the service name
                // (2) If the value is :peterpan@yahoo.com, the service name is NULL
                // (3) If the value is peterpan@yahoo.com, neither the service name and the colon exists
                isSaved = EFalse;
                }
            }
        if ( isSaved )
            {
            SaveL( aData, value );
            }
        }
    else if ( aData.Uid() == TUid::Uid( KVersitPropertyBinaryUid ) )
        {
        const TDesC8& value = converter.GetBinaryData( aData );
        if ( value.Length() > 0 )
            {
            SaveL( aData, value );
            }
        else
            {
            isSaved = EFalse;
            }
        }
    else if ( aData.Uid() == TUid::Uid( KVersitPropertyDateUid ) )
        {
        const TVersitDateTime* value = converter.GetDateTimeData( aData );
        if ( value )
            {
            SaveL( aData, *value );    
            }
        else
            {
            isSaved = EFalse;
            }
        }
    else
        {
        // In case the UID is something else. Should not happen ever, because
        // uids are defined by us.        
        isSaved = EFalse;
        }

    return isSaved;        
    }
    
void CVPbkVCardImporter::SaveL( 
    CVPbkVCardContactFieldData& aData, const TVersitDateTime& aValue )
    {
    iAddFieldOperation = iCopyPolicy->AddFieldToContactL(
                                        *iCurContact, 
                                        aData.FieldType(), 
                                        aValue.iDateTime, 
                                        NULL, 
                                        aData.Attribute(), 
                                        *this );
    }

void CVPbkVCardImporter::SaveL( 
    CVPbkVCardContactFieldData& aData, const TDesC& aValue )
    {
    if( iCntContact )
    	{
	    iAddCntContactFieldOperation = iCntCopyPolicy->AddFieldToContactL(
	                                        *iCntContact, 
	                                        aData.FieldType(), 
	                                        aValue, 
	                                        NULL, 
	                                        aData.Attribute(), 
	                                        *this );
    	}
    
    // If it is 'FN' vcard field, 
    // it can be overwriten by lastname subfield of 'N' vcard field
    TInt fieldTypeResId = iContactFieldIterator->FindVCardFieldL( 
            EVPbkVersitNameFN,
            EVPbkVersitSubFieldNone );
    if ( fieldTypeResId != KErrNotFound )
        {
        if ( FindContactFieldL( iCurContact, fieldTypeResId ) == 
                KErrNotFound )
            {
            iErasableFields.Append( fieldTypeResId );
            }
        }
    else
        {
        fieldTypeResId = aData.FieldType().FieldTypeResId();
        TInt id = iErasableFields.Find( fieldTypeResId );
        if ( id != KErrNotFound )
            {
            TInt field = FindContactFieldL( iCurContact, fieldTypeResId );
            if ( field != KErrNotFound )
                {
                iCurContact->RemoveField( field );
                }
            iErasableFields.Remove( id );
            }
        }

    iAddFieldOperation = iCopyPolicy->AddFieldToContactL(
                                        *iCurContact, 
                                        aData.FieldType(), 
                                        aValue, 
                                        NULL, 
                                        aData.Attribute(), 
                                        *this );
    }

void CVPbkVCardImporter::SaveL( 
    CVPbkVCardContactFieldData& aData, const TDesC8& aValue )
    {
    iAddFieldOperation = iCopyPolicy->AddFieldToContactL(
                                        *iCurContact, 
                                        aData.FieldType(), 
                                        aValue, 
                                        NULL, 
                                        aData.Attribute(), 
                                        *this );
    }
    
void CVPbkVCardImporter::FieldAddedToContact(
        MVPbkContactOperationBase& aOperation)
    {
    if (&aOperation == iAddFieldOperation)
        {
        delete iAddFieldOperation;
        iAddFieldOperation = NULL;
        }    
    if (&aOperation == iAddCntContactFieldOperation)
        {
        delete iAddCntContactFieldOperation;
        iAddCntContactFieldOperation = NULL;
        }
    // When both operations have completed
    if ( !iAddFieldOperation && !iAddCntContactFieldOperation )
        {
        StartAsync( ESaveField );
        }
    }
    
void CVPbkVCardImporter::FieldAddingFailed(
        MVPbkContactOperationBase& aOperation, 
        TInt aError)
    {
    if (&aOperation == iAddFieldOperation)
        {
        delete iAddFieldOperation;
        iAddFieldOperation = NULL;
        TRAPD( error, HandleFailedFieldAdditionL( aError ) );
        HandleError( error );
        }    
    if (&aOperation == iAddCntContactFieldOperation)
        {
        delete iAddCntContactFieldOperation;
        iAddCntContactFieldOperation = NULL;
        }
    }
    
void CVPbkVCardImporter::FieldsCopiedToContact(
        MVPbkContactOperationBase& aOperation)
    {
    if (&aOperation == iAddFieldOperation)
        {
	    delete iAddFieldOperation;
	    iAddFieldOperation = NULL;
	    
	    TRAPD( res, ContinueCopyingToNewContactL() );
	    HandleError( res );
        }
    if (&aOperation == iAddCntContactFieldOperation)
        {
        delete iAddCntContactFieldOperation;
        iAddCntContactFieldOperation = NULL;
        }
    }
    
void CVPbkVCardImporter::FieldsCopyFailed(
        MVPbkContactOperationBase& aOperation, 
        TInt aError)
    {
    if (&aOperation == iAddFieldOperation)
        {
	    delete iAddFieldOperation;
	    iAddFieldOperation = NULL;
	
	    HandleError( aError );
        }
    if (&aOperation == iAddCntContactFieldOperation)
        {
        delete iAddCntContactFieldOperation;
        iAddCntContactFieldOperation = NULL;
        }
    }
    
void CVPbkVCardImporter::SaveContactL()
    {
    if( iCntContact && iImportedContacts.Count() == 0 )
    	{
    	iAddCntContactFieldOperation = iCopyPolicy->CopyTitleFieldsL(
                *iCntContact,
                *iCurContact,
                *this );  
    	}
    
    // We have looped through property array with all match priorities
    // ==> all fields have been added
    iImportedContacts.AppendL( iCurContact );
    iCurContact = NULL;
    
    // Remove empty fields and empty contacts
    RemoveEmptyFieldsAndContacts();
        iObserver->ContactsImported();
    }
    
void CVPbkVCardImporter::ContinueCopyingToNewContactL()
    {
    // Save same field again to the new contact
    CVPbkVCardContactFieldData* data = iContactFieldIterator->CurrentLC();
    TBool saved = SaveDataL( *data );
    CleanupStack::PopAndDestroy( data ); 
    if ( !saved )
        {
        // If data was not saved try to move next field.
        StartAsync( ESaveField );
        }
    }    
    
void CVPbkVCardImporter::RemoveEmptyFieldsAndContacts()
    {
    TInt importedContactsCount = iImportedContacts.Count();
    for ( TInt i = importedContactsCount - 1; i >= 0; --i )
        {
        MVPbkStoreContactFieldCollection& fields = 
            iImportedContacts[i]->Fields();
        TInt fieldCount = fields.FieldCount();
        while ( fieldCount-- > 0 )
            {
            MVPbkStoreContactField& field = fields.FieldAt( fieldCount );    
            if ( field.FieldData().IsEmpty() )
                {
                iImportedContacts[i]->RemoveField( fieldCount );
                }
            }
            
        if ( fields.FieldCount() == 0 )
            {
            delete iImportedContacts[i];
            iImportedContacts.Remove( i );
            }
        }
    } 
    
void CVPbkVCardImporter::CopyContactTitlesL()
    {
    // If vcard was splitted to multiple contacts then copy
    // title fields from the first imported contact to the other contacts.
    iAddFieldOperation = iCopyPolicy->CopyTitleFieldsL(
            *iImportedContacts[0],
            *iCurContact,
            *this );  
    }       
    
void CVPbkVCardImporter::HandleFailedFieldAdditionL( TInt aError )
    {
    if ( aError == KErrNotSupported )
        {
        // Target contact didn't support field or didn't have field type
        // conversion for the field. Continue with next field.
        StartAsync( ESaveField );
        }
    else if ( aError == KErrOverflow )
        {
        if( iCntContact && iImportedContacts.Count() == 0 )
        	{
        	iAddCntContactFieldOperation = iCopyPolicy->CopyTitleFieldsL(
                    *iCntContact,
                    *iCurContact,
                    *this );  
        	}
        
        // Target contact is full of that field -> Create a new contact
        // and import field to that one.
        iImportedContacts.AppendL( iCurContact );
        iCurContact = NULL;
        iCurContact = iTargetStore.CreateNewContactLC();
        CleanupStack::Pop();
        // Copy title fields to new contact first
        CopyContactTitlesL();
        }
    else
        {
        // Some other error -> importing failed
        iObserver->ContactImportingFailed( aError );
        }
    } 
    
void CVPbkVCardImporter::HandleError( TInt aError )
    {
    if ( aError != KErrNone )
        {
        iObserver->ContactImportingFailed( aError );
        }
    }    
    
void CVPbkVCardImporter::StartAsync( TState aNextState )
    {
    iState = aNextState;
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }
    TBool CVPbkVCardImporter::IsOwncard()
    {
    	return iOwnCard;
    }

// ----------------------------------------------------------------------------
// CVPbkVCardImporter::IsGroupcard
// ----------------------------------------------------------------------------
TBool CVPbkVCardImporter::IsGroupcard()
    {
    return iGroupCard;
    }
// ----------------------------------------------------------------------------
// CVPbkVCardImporter::GetGroupcardvalue
// ----------------------------------------------------------------------------

TPtr  CVPbkVCardImporter::GetGroupcardvalue()
    { 
    return ( iGroupCardValue->Des());
    }
       
void CVPbkVCardImporter::StoreReady(MVPbkContactStore& aContactStore) 
    {
    if (iCntContact)
        {
        delete iCntContact;
        iCntContact = NULL;
        }
    
    iCntContact = aContactStore.CreateNewContactLC();
    CleanupStack::Pop(); // iCntContact
    
    ParseNextL();
    }

void CVPbkVCardImporter::StoreUnavailable(
		MVPbkContactStore& /*aContactStore*/, 
		TInt /*aReason*/) 
	{
	}

void CVPbkVCardImporter::HandleStoreEventL(
		MVPbkContactStore& /*aContactStore*/,
		TVPbkContactStoreEvent /*aStoreEvent*/)
	{
	}
            
// End of file
