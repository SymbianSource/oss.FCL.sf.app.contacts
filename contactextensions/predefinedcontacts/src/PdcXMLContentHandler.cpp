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
* Description:    Implementation of the Xml:MContentHandler interface
*
*/

// System includes
#include <MVPbkContactStore.h>              // MVPbkContactStore
#include <MVPbkStoreContact.h>              // MVPbkStoreContact
#include <MVPbkFieldType.h>                 // MVPbkFieldType
#include <MVPbkContactStoreProperties.h>    // MVPbkContactStoreProperties
#include <MVPbkContactFieldData.h>          // MVPbkContactFieldData
#include <MVPbkContactFieldTextData.h>      // MVPbkContactFieldTextData
#include <MVPbkContactFieldBinaryData.h>    // MVPbkContactFieldBinaryData
#include <MVPbkContactFieldDateTimeData.h>  // MVPbkContactFieldDateTimeData
#include <cntdef.h>                         // TContactItemId
#include <CVPbkContactLinkArray.h>          // CVPbkContactLinkArray
#include <CPbkFieldInfo.h> 
#include <utf.h>
#include <cntfield.h> // CPbkContactItem
#include <CVPbkContactIdConverter.h>
#include <VPbkContactStoreUris.h>
#include <MVPbkContactStoreList.h>
#include <CPbkContactEngine.h>      // Phonebook Engine
#include <CPbkContactItem.h>        // Phonebook Contact
#include <e32cmn.h> 

// User includes
#include "pdcstringconstants.h"     // PdcStringTable
#include "PdcXMLContentHandler.h"   // CPdcXmlContentHandler
#include "pdcxmlmapping.h"          // KFieldResIdLookupTable
#include "PhCltTypes.h"             //For TPhCltTelephoneNumber

// Constants
/**
 * The default size of the content buffer
 */
const TInt KDefaultContentBufferSize = 128;

/**
 * Index of the start contact tag in pcstringtable
 */
const TInt KContactStartTagIndex = 0;
const TInt KSpeeddialNoTagIndex = PdcStringTable::ESpeeddialNo;
const TInt KSpeeddialAssignedTagIndex = PdcStringTable::ESpeeddialNoAssign;
const TInt KSpeeddialIndexInit = 0;

// To distinguish the invalid assigned field index -1, 
// set the initial value of speeddialAssign tag with -2
const TInt KSpeeddialAssignedFieldIndexInit = -2;

using namespace Xml;

// The speeddial supported assigning fields
const TInt CPdcXmlContentHandler::iSpeeddailSupportedAssignedFieldsIndexSet[phoneNumberCount] = 
    {
    PdcStringTable::EGeneral, 
    PdcStringTable::EMobile,
    PdcStringTable::EVideoNo,
    PdcStringTable::EFax,
    -1    
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::NewL
// Symbian 1st phase constructor
// @return Self pointer to CPdcXmlContentHandler
// @param    aContactStore   contacts store
// @param    aLinkArray  links to  contacts added.
// ---------------------------------------------------------------------------
//
CPdcXmlContentHandler* CPdcXmlContentHandler::NewL( 
         MVPbkContactStore& aContactStore, CVPbkContactLinkArray& aLinkArray )
    {
    CPdcXmlContentHandler* self = new( ELeave )
        CPdcXmlContentHandler(aContactStore, aLinkArray );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::~CPdcXmlContentHandler
// Destructor.
// ---------------------------------------------------------------------------
//
CPdcXmlContentHandler::~CPdcXmlContentHandler()
    {
    iContentBuffer.Close();
    
    delete iContactItem;

    delete iWait;
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::CPdcXmlContentHandler
// C++ constructor
// @param    aContactStore   contacts store
// @param    aLinkArray  links to  contacts added.
// ---------------------------------------------------------------------------
//
CPdcXmlContentHandler::CPdcXmlContentHandler( MVPbkContactStore& aContactStore ,
                                          CVPbkContactLinkArray& aLinkArray)
    : iContactStore( aContactStore ), 
      iLinkArray( aLinkArray ),
      iSpeeddialIndex(KSpeeddialIndexInit),
      iSpeeddialAssignedFieldIndex(KSpeeddialAssignedFieldIndexInit)
    {

    }
    
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::ConstructL
// Second-phase constructor
// ---------------------------------------------------------------------------
//
void CPdcXmlContentHandler::ConstructL()
    {
    iContentBuffer.CreateL( KDefaultContentBufferSize );
    iWait = new ( ELeave ) CActiveSchedulerWait();
    }
    
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::SetStringPoolL
// Sets the string pool
// @param aStringPool   parser's stringPool
// ---------------------------------------------------------------------------
//
void CPdcXmlContentHandler::SetStringPoolL( RStringPool& aStringPool)
    {
    iStringPool = &aStringPool;
    iStringPool->OpenL( PdcStringTable::Table );
    }    
    
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::GetTagIndexL
// 
// @param aElement  
// ---------------------------------------------------------------------------
//  
TInt CPdcXmlContentHandler::GetTagIndexL( const RTagInfo& aElement ) const
    { 
    
    const TDesC8& localName = aElement.LocalName ().DesC();
    
	RStringF tagName = iStringPool->OpenFStringL ( localName );

	TInt index = tagName.Index ( PdcStringTable::Table );
    tagName.Close();
    
	return index;
    }
    
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::AddTextFieldL
// Adds a field to a contact, using the resource id of the field
// @param aFieldResId  resource id of field
// @param aBuffer   buffer containing data to be added
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::AddFieldL( TInt aFieldResId,
                                                       const TDesC8& aBuffer )
    {
    ASSERT( iContactItem );
    
    const MVPbkFieldTypeList& list = 
                iContactItem->ParentStore().StoreProperties().SupportedFields();
    
    const MVPbkFieldType* fieldType = list.Find( aFieldResId );
    
    MVPbkStoreContactField* contactField = 
                                    iContactItem->CreateFieldLC( *fieldType );
                                    
    MVPbkContactFieldData& fieldData = contactField->FieldData();
               
    TVPbkFieldStorageType storageType = fieldData.DataType();
    switch ( storageType )
        {
        case EVPbkFieldStorageTypeText:
            {
            SetTextFieldL( fieldData, aBuffer );
            }
            break;
        case EVPbkFieldStorageTypeBinary:
            {
            SetBinaryFieldL( fieldData, aBuffer );        
            }
            break;
        case EVPbkFieldStorageTypeDateTime:
            {
            SetDateFieldL( fieldData, aBuffer );
            }
            break;
        case EVPbkFieldStorageTypeNull:
        default:
            {
            // Un-handled storage type
            ASSERT( 0 );
            }   
        }               
                         
    iContactItem->AddFieldL( contactField );
    CleanupStack::Pop( contactField );
    }
 
 // ---------------------------------------------------------------------------
// CPdcXmlContentHandler::AddTextFieldL
// Adds a field to a contact, using the resource id of the field
// @param aFieldResId  resource id of field
// @param aBuffer   buffer containing data to be added
// ---------------------------------------------------------------------------
//      
void CPdcXmlContentHandler::SetTextFieldL( MVPbkContactFieldData& aFieldData,
                                                        const TDesC8& aBuffer )
    {
    MVPbkContactFieldTextData& textData = 
    MVPbkContactFieldTextData::Cast( aFieldData );
    
    // Expand the text from TDes8 to TDes16
    RBuf16 expandedBuffer;
    expandedBuffer.CreateL( aBuffer.Length() );
    expandedBuffer.CleanupClosePushL();
    
    //convert the UTF-8 to Unicode for displaying the Chinese correctly.
    //expandedBuffer.Copy( aBuffer );
    CnvUtfConverter::ConvertToUnicodeFromUtf8( expandedBuffer, aBuffer );

    // If there is a maximum length to the field, reduce
    // the text size if needed.
    TInt maxLength = textData.MaxLength();
    if ( maxLength != KVPbkUnlimitedFieldLength 
                                && expandedBuffer.Length() > maxLength )
        {
        expandedBuffer.SetLength( maxLength );
        }
    
    textData.SetTextL( expandedBuffer );
    CleanupStack::PopAndDestroy(); // expanded buffer
    }

 // ---------------------------------------------------------------------------
// CPdcXmlContentHandler::AddTextFieldL
// Adds a field to a contact, using the resource id of the field
// @param aFieldResId  resource id of field
// @param aBuffer   buffer containing data to be added
// ---------------------------------------------------------------------------
//      
void CPdcXmlContentHandler::SetBinaryFieldL( MVPbkContactFieldData& aFieldData,
                                                        const TDesC8& aBuffer )
    {
    MVPbkContactFieldBinaryData& binaryData = 
                MVPbkContactFieldBinaryData::Cast( aFieldData );
                
    // If there is a maximum length to the field, reduce
    // the  size if needed.
    TPtrC8 data( aBuffer );
    TInt maxLength = binaryData.MaxLength();
    if ( maxLength != KVPbkUnlimitedFieldLength && data.Length() > maxLength )
        {
        data.Set( data.Left( maxLength ) );
        }
    
    binaryData.SetBinaryDataL( data );            
    }
            
 // ---------------------------------------------------------------------------
// CPdcXmlContentHandler::AddTextFieldL
// Adds a field to a contact, using the resource id of the field
// @param aFieldResId  resource id of field
// @param aBuffer   buffer containing data to be added
// ---------------------------------------------------------------------------
//      
void CPdcXmlContentHandler::SetDateFieldL( MVPbkContactFieldData& aFieldData,
                                                        const TDesC8& aBuffer )
    {
    MVPbkContactFieldDateTimeData& timeData =
            MVPbkContactFieldDateTimeData::Cast( aFieldData );
    
    // Expand the text from TDes8 to TDes16
    RBuf16 expandedBuffer;
    expandedBuffer.CreateL( aBuffer.Length() );
    expandedBuffer.CleanupClosePushL();
    expandedBuffer.Copy( aBuffer );
    
    // Set the time
    TTime time;
    User::Leave( time.Set( expandedBuffer ) );
    timeData.SetDateTime( time );
    
    CleanupStack::PopAndDestroy(); // expanded buffer
    }  

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::OnStartDocumentL
// IGNORED
// This method is a callback to indicate the start of the document.
// @param				aDocParam Specifies the various parameters of the 
//                      document.
// @param				aDocParam.iCharacterSetName The character encoding of
//                      the document.
// @param				aErrorCode is the error code. This is always EErrNone 
//                      for the libxml2 XML parser. 
//					    
// ---------------------------------------------------------------------------
// 
void CPdcXmlContentHandler::OnStartDocumentL(
                const RDocumentParameters& /*aDocParam*/, TInt /*aErrorCode*/)
    {
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::OnEndDocumentL
// IGNORED
// This method is a callback to indicate the end of the document.
// @param				aErrorCode is the error code.This is always EErrNone 
//                      for the libxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnEndDocumentL(TInt /*aErrorCode*/)
    {
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::OnStartElementL
// This method is a callback to indicate an element has been parsed. If the 
// element is the start of a new contact, it creates a new contact item. 
// @param				aElement is a handle to the element's details.
// @param				aAttributes contains the attributes for the element.
// @param				aErrorCode is the error code.This is always EErrNone
//                      for thelibxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnStartElementL(const RTagInfo& aElement, 
                   const RAttributeArray& /*aAttributes*/, TInt /*aErrorCode*/)
    {
    TInt tagIndex = GetTagIndexL( aElement );
    
    // Only need to check for start of a new contact tag, as the processing of 
    // the data will take place in OnEndElementL(). 
    if ( tagIndex == KContactStartTagIndex )
        {
        // Found new contact tag, check not already processing a contact
        if ( iContactItem )
            {
            // If there is already a contact, the XML file
            // is corrupt, so leave
            User::Leave( KErrCorrupt );
            }
      
        // Create a new contact item
        iContactItem = iContactStore.CreateNewContactLC();
        CleanupStack::Pop(); // iContactItem
        }
    else
        {
        // Check it is a recongised tag
        iKnownTag = ETrue;
		if(tagIndex == -1)
          {
          iKnownTag = FALSE;
          }
        }  
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::OnEndElementL
// This method is a callback to indicate the end of the element has been 
// reached. If it the end of field tag, it adds the data for that field to
// the current contact item. If it the contact end tag, the contact is 
// committed. 
// @param				aElement is a handle to the element's details.
// @param				aErrorCode is the error code.This is always EErrNone 
//                      for the libxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnEndElementL(const RTagInfo& aElement,
                                                          TInt /*aErrorCode*/)
    {
   	// Get the index of the tag
    TInt tagIndex = GetTagIndexL( aElement );    

    // If the end tag marks the end of the contact, add the contact to the 
    // contact store.
    switch( tagIndex )
    	{
    	case KContactStartTagIndex:
    		{
            MVPbkStoreContactFieldCollection const& fieldCollection =
                iContactItem->Fields();
    		TInt fieldCount = fieldCollection.FieldCount();
    		if( fieldCount > 0 )
    		    {
    		    //add contact to cntdb and assign speeddial number
        	    TRAPD( err, AddContactL() );
        	    if ( err != KErrNone )
        	    	{
        	    	LOGTEXT2( _L("AddContactL() ERR:%d"), err ); 
        	    	User::LeaveIfError( err );
        	    	}
        	    LOGTEXT(_L("add contact successfully")); 
    		    }
    		//reset flags
            iSpeeddialIndex = KSpeeddialIndexInit;
            iSpeeddialAssignedFieldIndex = KSpeeddialAssignedFieldIndexInit; 
    		// Tidy up the current contact item.
    		delete iContactItem;
            iContactItem = NULL;
        	break;	
    		}
    	case KSpeeddialNoTagIndex:
    		{
    		if( iContentBuffer.Length() > 0 )
    		    {
	            //convert the speeddial value
                TLex8 lex(iContentBuffer);
                TInt value;
        	    if( KErrNone == lex.Val(value) )
            	    {
                    if( value >= KCntMinSpeedDialIndex  &&
                            value <= KCntMaxSpeedDialIndex ) 
                	    {
                	    iSpeeddialIndex = value; 
                	    }
            	    }
    		    }
            break;
    		}
    	case KSpeeddialAssignedTagIndex:
    		{
    		//get the speed dial assigned field index
        	if( iContentBuffer.Length() > 0 )
            	{
    			const TDesC8& localName = iContentBuffer;
		
				RStringF tagName = iStringPool->OpenFStringL ( localName );
                iSpeeddialAssignedFieldIndex =
                    tagName.Index ( PdcStringTable::Table);
    			tagName.Close();
            	}
            break;  
    		}
    	default:
    		{
    		if ( tagIndex != KErrNotFound )
            	{
            	// Lookup the tag in the field resource id versus xml tag mapping
            	TInt fieldResId = KFieldResIdLookupTable[tagIndex];
            	if ( fieldResId != KErrNotFound && iContentBuffer.Length() > 0 )
                	{
					AddFieldL( fieldResId, iContentBuffer );
                	}
            	}
    		}
    	}

    iKnownTag = EFalse;
    iContentBuffer.SetLength( 0 ); // reset the buffer
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::OnContentL
// This method stores the content of an element. When an OnEndElementL is 
// received this means there is no more data to be saved.
// @param		    aBytes is the raw content data for the element. 
//					The client is responsible for converting the data to the 
//					required character set if necessary.
//					In some instances the content may be binary and must not 
//                  be converted.
// @param			aErrorCode is the error code.This is always EErrNone for
//                  the libxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnContentL( const TDesC8& aBytes,
        TInt /*aErrorCode*/ )
    {
    // If the tag is known store the data for later use    
    if ( iKnownTag )
        {
        // Check that there is enough room in the buffer
        TInt maxLength = iContentBuffer.MaxLength();
        TInt newLength = iContentBuffer.Length() + aBytes.Length();
        if ( newLength > maxLength )
            {
            iContentBuffer.ReAllocL( newLength );
            }
        
        // Append the data to the content buffer    
        iContentBuffer.Append( aBytes );

        }   
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method is a notification of the beginning of the scope of a 
// prefix-URI Namespace mapping.This method is always called before the 
// corresponding OnStartElementL method.
// @param				aPrefix is the Namespace prefix being declared.
// @param				aUri is the Namespace URI the prefix is mapped to.
// @param				aErrorCode is the error code.This is always EErrNone
//                      for the libxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnStartPrefixMappingL(const RString& /*aPrefix*/,
                                const RString& /*aUri*/, TInt /*aErrorCode*/)
    {
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method is a notification of the end of the scope of a prefix-URI mapping.
// This method is called after the corresponding DoEndElementL method.
// @param				aPrefix is the Namespace prefix that was mapped.
// @param				aErrorCode is the error code.This is always EErrNone 
//                      for the libxml2 XML parser. 
// ---------------------------------------------------------------------------
// 
void CPdcXmlContentHandler::OnEndPrefixMappingL( const RString& /*aPrefix*/,
        TInt /*aErrorCode*/)
    { 
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method is a notification of ignorable whitespace in element content.
// @param		aBytes are the ignored bytes from the document being parsed.
// @param		aErrorCode is the error code.This is always EErrNone for the
//              libxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnIgnorableWhiteSpaceL(const TDesC8& /*aBytes*/, 
                                                          TInt /*aErrorCode*/)
    {  
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method is a notification of a skipped entity. If the parser encounters
// an external entity it does not need to expand it - it can return the entity 
// as aName for the client to deal with.
// @param				aName is the name of the skipped entity.
// @param				aErrorCode is the error code.This is always EErrNone
//                      for the libxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnSkippedEntityL( const RString& /*aName*/,
                                                         TInt /*aErrorCode*/)
    {
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method is a receive notification of a processing instruction.
// @param				aTarget is the processing instruction target.
// @param				aData is the processing instruction data. If empty 
//                      none was supplied.
// @param				aErrorCode is the error code.This is always EErrNone
//                      for the libxml2 XML parser. 
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnProcessingInstructionL(
        const TDesC8& /*aTarget*/,
        const TDesC8& /*aData*/,
        TInt /*aErrorCode*/ )
    {
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method indicates an error has occurred.
// @param				aError is the error code
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::OnError( TInt aErrorCode )
    {
    LOGTEXT2(_L("CPdcXmlContentHandler::OnError = %d"), aErrorCode);
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method obtains the interface matching the specified uid.
// @return				0 if no interface matching the uid is found.
// 					Otherwise, the this pointer cast to that interface.
// @param				aUid the uid identifying the required interface.
// ---------------------------------------------------------------------------
//   
TAny* CPdcXmlContentHandler::GetExtendedInterface(const TInt32 /*aUid*/)
    {
    return NULL;
    }
    
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// Called when a contact operation has succesfully completed.
//
// @param aResult   The result of the operation. The client takes
//                  the ownership of the iStoreContact immediately
//                  if set in aResult.
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::ContactOperationCompleted( 
        TContactOpResult /*aResult*/ )
    {
    // Store the fact that the operation has completed successfully
    iContactObserverError = KErrNone;

    // Stop the wait operation, program execution returns to OnEndElementL()
    iWait->AsyncStop();
    }

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// Called when a contact operation has failed.
//
// @param aOpCode           The operation that failed.
// @param aErrorCode        System error code of the failure.
//							KErrAccessDenied when EContactCommit 
//							means that contact hasn't been locked.
// @param aErrorNotified    ETrue if the implementation has already
//                          notified user about the error, 
//                          EFalse otherwise.
// ---------------------------------------------------------------------------
//   
void CPdcXmlContentHandler::ContactOperationFailed
    (TContactOp /*aOpCode*/, TInt aErrorCode, TBool /*aErrorNotified*/)
        {

        // Store the error code
        iContactObserverError = aErrorCode;

        // Stop the wait operation, program execution returns to OnEndElementL()
        iWait->AsyncStop();
        }
        
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// IGNORED
// This method assign speeddial number from predefinedcontacts .
// ---------------------------------------------------------------------------
// 
void CPdcXmlContentHandler::AddContactL()
	{
	
    LOGTEXT( _L("CPdcXmlContentHandler::AddContactL") );
    
	// A CActiveSchedulerWait is used to make the asyncronous commiting
    // of the contact item syncronous by waiting for the callback 
    // to either ContactOperationCompleted or ContactOperationFailed
	iContactItem->CommitL( *this );
	iWait->Start();

	// The contact has been added syncronously, so create the link to 
    // the contact and add to the link array.
	
	MVPbkContactLink* link = iContactItem->CreateLinkLC();	    
	iLinkArray.AppendL( link );
	CleanupStack::Pop( );
	
	    
	//if the speeddial is valid
    if( iSpeeddialIndex > 0 )
    	{    	
    	LOGTEXT(_L("Start assign speeddial."));    	
    		        
        TVPbkContactStoreUriPtr uri(
                VPbkContactStoreUris::DefaultCntDbUri() );
        MVPbkContactStore* defaultStore =
                iContactManager->ContactStoresL().Find( uri );
    	User::LeaveIfNull(defaultStore);
    	
    	LOGTEXT(_L("CVPbkContactIdConverter::NewL"));    
    	CVPbkContactIdConverter* idConverter;
    	idConverter = CVPbkContactIdConverter::NewL( *defaultStore ); 
    	CleanupStack::PushL(idConverter);
    	
	    LOGTEXT(_L("idConverter->LinkToIdentifier"));
    	TContactItemId contactIndex= idConverter->LinkToIdentifier( *link ); 
    	CleanupStack::PopAndDestroy(idConverter);
        
    	LOGTEXT(_L("CPbkContactEngine::NewL"));
    	CPbkContactEngine* pbkEngine;
    	pbkEngine = CPbkContactEngine::NewL(); 
		CleanupStack::PushL( pbkEngine );
		
		LOGTEXT( _L("pbkEngine->OpenContactL") );
        CPbkContactItem* item = pbkEngine->OpenContactL( contactIndex );	
    	CleanupStack::PushL( item );

    	TInt fieldIndex = GetSpeeddialAssignedFieldIndexL( item );
    	   	    	    
        if( 0 <= fieldIndex )
            {
            LOGTEXT( _L("SetFieldAsSpeedDialL") );
		    pbkEngine->SetFieldAsSpeedDialL( *item, fieldIndex, iSpeeddialIndex );
            }  

        CleanupStack::PopAndDestroy( item );
		LOGTEXT(_L("pbkEngine->CloseContactL"));	
        pbkEngine->CloseContactL( contactIndex );
        
        LOGTEXT( _L("PopAndDestroy(pbkEngine)") );
        CleanupStack::PopAndDestroy( pbkEngine );
    	}
 	}

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::GetSpeeddialAssignedFieldIndexL
// This method get the speeddial assigned field index
// @param aItem 
// ---------------------------------------------------------------------------
// 
TInt CPdcXmlContentHandler::GetSpeeddialAssignedFieldIndexL(
        CPbkContactItem* aItem )
	{
	LOGTEXT(_L("GetSpeeddialAssignedFieldIndexL"));      
    TInt fieldIndex = -1;

    // If the user don't assign the speed dial in the xml, 
    // use the default assigned method
    if( KSpeeddialAssignedFieldIndexInit == iSpeeddialAssignedFieldIndex )
        {
        iSpeeddialAssignedFieldIndex = GetDefaultSpeeddialAssignedFieldIndexL();
        }
   
    // if the user assign the speeddial to an xml field, then check the validity 
    // and get the contact item field index.
    if(0 <= iSpeeddialAssignedFieldIndex)    
	    {
	    TBool fieldIsValid = EFalse;
	    
        fieldIsValid = CheckValidityOfSpeeddialAssignedFieldL(
                iSpeeddialAssignedFieldIndex );
	    
        // If the assigned field is a valid field,
        // mapping the cntmodel field and get the index.
	    if( fieldIsValid )
	        {
			fieldIndex = ContactItemFieldIndexL( aItem );
            }     
        }               		

   	return fieldIndex;
	}	

//
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::GetDefaultSpeeddialAssignedFieldIndexL
// This method get the default speeddial assigned field index.
// @param 
// ---------------------------------------------------------------------------
// 	
TInt CPdcXmlContentHandler::GetDefaultSpeeddialAssignedFieldIndexL()
	{
	LOGTEXT(_L("GetDefaultSpeeddialAssignedFieldIndexL"));  
	TInt index = 0;
	TInt defaultFieldIndex = -1;

	defaultFieldIndex = iSpeeddailSupportedAssignedFieldsIndexSet[index];
   
    while(0 < defaultFieldIndex)
        {   
        MVPbkStoreContactFieldCollection const& fieldCollection =
                iContactItem->Fields();
        TInt fieldCount = fieldCollection.FieldCount();
        TInt searchIndex;
        
        for( searchIndex = 0; searchIndex < fieldCount; searchIndex++ )
            {
            MVPbkStoreContactField* storeContactField =
                    fieldCollection.FieldAtLC( searchIndex );
            const MVPbkFieldType* fieldType =
                    storeContactField->BestMatchingFieldType();
            TInt fieldResourceId = fieldType->FieldTypeResId();
            if( fieldResourceId == KFieldResIdLookupTable[defaultFieldIndex] )
            	{
                MVPbkContactFieldData& fieldData =
                        storeContactField->FieldData();
                
                if(!fieldData.IsEmpty())
                    {
                    CleanupStack::PopAndDestroy(); //pop iStoreContactField
                    return defaultFieldIndex;   
                    }
            	}
            CleanupStack::PopAndDestroy(); //pop iStoreContactField
            }

        ++index;
        defaultFieldIndex = iSpeeddailSupportedAssignedFieldsIndexSet[index];    
        }
    
    return defaultFieldIndex;
	}

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::CheckValidityOfSpeeddialAssignedFieldL
// This method check the validity of assigned field by index
// @param aFieldIndex 
// ---------------------------------------------------------------------------
// 
TBool CPdcXmlContentHandler::CheckValidityOfSpeeddialAssignedFieldL( 
        TInt aFieldIndex )
	{
	LOGTEXT(_L("CheckValidityOfSpeeddialAssignedFieldL")); 
	//check the speeddial assigned field is a valid field
	TBool validity = EFalse;
	TInt index = 0;
	
	TInt tempIndex = iSpeeddailSupportedAssignedFieldsIndexSet[index];
	    	
	while( tempIndex >= 0 )
	    {	
	    if( aFieldIndex == tempIndex )
	    	{		
            MVPbkStoreContactFieldCollection const& fieldCollection =
                    iContactItem->Fields();
            TInt fieldCount = fieldCollection.FieldCount();
            TInt searchIndex;
            for( searchIndex = 0; searchIndex < fieldCount; searchIndex++ )
            	{
                MVPbkStoreContactField* storeContactField =
                        fieldCollection.FieldAtLC( searchIndex );
                const MVPbkFieldType* fieldType =
                        storeContactField->BestMatchingFieldType();
            	TInt fieldResourceId = fieldType->FieldTypeResId();
            	
            	if( fieldResourceId == KFieldResIdLookupTable[aFieldIndex] )
            		{
                    MVPbkContactFieldData& fieldData =
                            storeContactField->FieldData();
                    // If the phonenumber field doesn't have data,
                    // set the index invalid.
                    if( !fieldData.IsEmpty() )
            	        {
            	        validity = ETrue;
            	        }
            	    CleanupStack::PopAndDestroy(); //pop iStoreContactField
            	    break;
            		}
            	CleanupStack::PopAndDestroy(); //pop iStoreContactField
            	}
	    	}
	    if( validity )
	    	{
	    	break;
	    	}
	    ++index;
	    tempIndex = iSpeeddailSupportedAssignedFieldsIndexSet[index];
	    }
	    
	return validity;
	}

// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::ContactItemFieldIndexL
// This method get the speeddial assigned field index
// @param aItem 
// ---------------------------------------------------------------------------
// 
TInt CPdcXmlContentHandler::ContactItemFieldIndexL(CPbkContactItem* aItem)
	{
    LOGTEXT(_L("ContactItemFieldIndex"));
    
    TInt fieldIndex  = -1;
    TBuf<20> speeddialAssignedField;  
         
    MVPbkStoreContactFieldCollection const& fieldCollection =
            iContactItem->Fields();
    TInt count = fieldCollection.FieldCount();
    TInt searchIndex;
    
    for( searchIndex = 0; searchIndex < count; searchIndex++)
    	{
        MVPbkStoreContactField* storeContactField =
                fieldCollection.FieldAtLC( searchIndex );
        const MVPbkFieldType* fieldType =
                storeContactField->BestMatchingFieldType();
        TInt fieldResourceId = fieldType->FieldTypeResId();
        
        if( fieldResourceId ==
                KFieldResIdLookupTable[iSpeeddialAssignedFieldIndex] )
    		{
    		speeddialAssignedField = storeContactField->FieldLabel();
    		CleanupStack::PopAndDestroy(); //pop iStoreContactField
    		break;
    		}
    	CleanupStack::PopAndDestroy(); //pop iStoreContactField
    	}

	CPbkFieldArray& fields = aItem->CardFields();
    const TInt fieldCount = fields.Count();   
            
    for ( TInt i = 0; i < fieldCount; ++i )
        {
        CContactItemField& itemfield = fields[i].ItemField();
        TBuf<20> buf = itemfield.Label();
            		
	    // find the user assigned field for speeddial
        if( 0 == speeddialAssignedField.Compare(buf) )
            {
            fieldIndex = i;
            break;
            }
        }
    return fieldIndex;
	}
	
// ---------------------------------------------------------------------------
// CPdcXmlContentHandler::
// This method get the pointer of contactmanager from XML importer
// @param aContactManager 
// ---------------------------------------------------------------------------
// 
void CPdcXmlContentHandler::SetContactManager(
        CVPbkContactManager* aContactManager )
    {
	iContactManager = aContactManager;
    }


