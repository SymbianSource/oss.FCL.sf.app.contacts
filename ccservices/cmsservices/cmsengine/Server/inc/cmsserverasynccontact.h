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


#ifndef __CMSSERVERASYNCCONTACT__
#define __CMSSERVERASYNCCONTACT__

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CPbk2SortOrderManager;
class MPbk2ContactNameFormatter;
class CVPbkBaseContactFieldTypeIterator;

// CONSTANTS
const TInt KDefaultBufferSize           = 12;

/**
 * CCmsServerAsyncContact handles contact asynchronous requests.
 */
NONSHARABLE_CLASS( CCmsServerAsyncContact ) : public CActive
    {
    public:  
        
        /**
        * Creates an instance of CCmsServerAsyncContact
        *
        * @param aSyncContact Reference to CCmsContact instance
        * @param aCmsServer Reference to CCmsServer instance
        * @return Instance of CCmsServerAsyncContact
        */
        static CCmsServerAsyncContact* NewL( CCmsServerContact& aSyncContact,
                                             CCmsServer& aCmsServer );

        /**
        * Fetches list of enabled fields, i.e. fields with some content
        */
        void FetchEnabledFields();
        
        /**
        * Fetches contact field
        *
        * @param aField Field to be fetched
        */
        void FetchContactData( CCmsContactFieldItem::TCmsContactField aField );
        
        /**
        * Retrieves content of all fields belonging to some communication method
        *
        * @param aFieldGroup Communication type
        * @return Array of fields. Ownershpi is transfered.
        */
        CDesCArrayFlat* ContactDataTextLC( CCmsContactFieldItem::TCmsContactFieldGroup aFieldGroup );
        
        /**
        * Reads contact URI fields
        *
        * @param aResourceId Resource identifying URI fields
        * @return Array with URI fields. Ownership is transfered.
        */
        CDesCArrayFlat* ContactDataUriLC( TInt aResourceId );
        
        /**
        * Read contact's fields to the array
        *
        * @param aResourceId Fields to be read
        * @param aFieldArray Result array
        */
        void PhonebookDataL( TInt aResourceId, CDesCArrayFlat& aFieldArray );
        
        /**
        * Reads all the data related to a contact: fields content, defaults,
        *   corresponding xSP contacts
        *
        * @param aResourceId Specifies type of data to read
        * @return Packed contact info. Ownership is transfered
        */
        HBufC* DoGetPhonebookDataLC( TInt aResourceId );
        
        /**
        * Checks if any IM service is supported
        *
        * @return 0 if not supported and 1 if supported
        */
        TInt FindImAvailabilityL();
        
        /**
        * Checks if Voip service is supported
        *
        * @param aUri Voip service provider
        * @return ETrue if this service is supported
        */
        TBool IsVoipCapableL( const TDesC& aUri );
        
        /**
        * Destructor
        */
        ~CCmsServerAsyncContact();
        
    private:
        
        // Types of fetch operation
        enum TCmsAsyncOperation
            {
            ENone = 0,
            EFetchEnabledFields,
            EFetchPhonebookBinData,
            EFetchPhonebookTextData,
            EFetchPresenceData,
            EFetchPhonebookUriData,
            EFetchPhonebookDateTimeData
            };

    private:
        
        /**
        * Default constructor
        * @param aSyncContact Reference to CCmsContact instance
        * @param aCmsServer Reference to CCmsServer instance
        */
        CCmsServerAsyncContact( CCmsServerContact& aSyncContact,
                                CCmsServer& aCmsServer );
        
        /**
        * Second-phase constructor
        */
        void ConstructL();
        
        /**
        * Reads contact's first binary field
        *
        * @param aResourcId Field type
        * @return Field content. Ownership is transfered.
        */
        HBufC8* ContactDataBinaryL( TInt aResourceId );
        
        /**
        * Gets the list of fields with some content
        *
        * @return Packed list of fields. Ownership is transfered.
        */
        HBufC* GetEnabledFieldsLC();
        
        /**
        * Gets the list of fields with some content
        *
        * @param aBufferIndex Buffer index to insert data
        * @param aDataBuffer Buffer to save the list of fields
        * @param aContact Store contact
        */
        void DoGetEnabledFieldsL( TInt& aBufferIndex, CBufFlat* aDataBuffer, 
                                  const MVPbkStoreContact& aContact );

        /**
        * Gets contact's full name or contact fields with default attributes.
        *
        * @param aResourceId: Specifies type of data to read. 
        * @return Buffer with contact's full name, or fields and default
        *        attrtibutes. Ownership is transfered.
        */
        HBufC* DoGetDataLC( TInt aResourceId );
        
        /**
        * Reads specified contact fields and default field
        *
        * @param aItemArray Array of contact fields to be filled
        * @param aDefaults Array of defaults to be filled
        * @param aResourceId Field type
        * @param aContact MVPbkStoreContact
        */
        void HandleOneContactL( CDesCArray& aItemArray, CDesCArray& aInfoArray, RArray<TInt>& aDefaults,
                                TInt aResourceId, const MVPbkStoreContact& aContact );
        
        /**
        * Packs information about contacts' default fields into the buffer
        *
        * @param aItemArray Array of contacts
        * @param aDefaults Array of default fields for the contacts
        * @return Packed info about defaults, ownership is transfered
        */
        HBufC* ConstructStreamBufferL( CDesCArray& aItemArray, CDesCArray& aInfoArray, RArray<TInt>& aDefaults );
        
        /**
        * Get contact's full name. 
        *
        * @return Contact's full name, ownership is transfered
        */
        HBufC* DoGetFullNameDataLC( );
        
        /**
        * Checks default attributes of a field.
        *
        * @param aContactField Contact field to seach default attributes for.
        * @return TInt TCmsDefaultAttributeTypes bitmask
        */
        TInt DefaultAttributesL( const MVPbkBaseContactField& aContactField );
        
        /**
        * Matches VPbk's enum to CMS internal enum.
        *
        * @param aDefaultType VPbk's default type.   
        * @return TCmsDefaultAttributeTypes CMS's default type.
        */
        CCmsContactFieldItem::TCmsDefaultAttributeTypes
            MatchTypesToInternalEnum(
                TVPbkDefaultType aDefaultType );
        
        /**
        * Reads all E-mail, VoIP and IMPP fields 
        *
        * @return Fields array, ownership is transfered
        */
        CDesCArrayFlat* ContactInfoArrayLC();
        
        /**
        * Reads field content to the buffer
        *
        * @param aField Contact field
        * @return Field content, ownership is transfered
        */
        HBufC* ParseContactFieldL( const MVPbkBaseContactField& aField );
        
        /**
        * Reads all contact fields specified in the resource
        *
        * @param aCount Number of field definitions in the resource array
        * @param aResourceArray Resource array
        * @return Array of fields content, ownership is transfered
        */
        CDesCArrayFlat* FieldsFromArrayLC( TInt aCount, const TInt aResourceArray[] );
        
        /**
        * Gets all fields content and adds them to the field array
        * 
        * @param aFieldArray Field array to keep all fields content
        * @param aFieldIterator Field iterator to go through all fields
        */
        void GetOneFieldL( CDesCArrayFlat& aFieldArray,
                           CVPbkBaseContactFieldTypeIterator& aFieldIterator );
        
        /**
        * Checks if one string is a subset of another string TODO: why so difficult?
        *
        * @param aBuffer Target string
        * @param aItem Search string
        * @return ETrue if found
        */
        TBool HasElement( const TDesC8& aBuffer, const TDesC8& aItem );
        
        /**
        * Copies field type and field content (separated by KDelimiter)
        * to the buffer.
        * @param aBuffer Buffer to copy data
        * @param aDescriptor Field content
        * @param aFieldType Field type
        */
        void AppendToBufferL( RBuf16& aBuffer, const TDesC& aDescriptor,
                              CCmsContactFieldItem::TCmsContactField aFieldType );

        /**
        * Copies fields from the source to the target string array
        *
        * @param aSource Source array
        * @param aTarget Target array        
        */
        void AppendFieldsL( const CDesCArrayFlat& aSource, CDesCArrayFlat& aTarget );
        
        /**
        * Maps Cms contact field to field type (binary, uri, text, etc.)
        *
        * @param aCmsField Cms contact field
        * @return Field type
        */
        TCmsAsyncOperation SelectOperationType( TInt aCmsField );
        
        /**
        * Fetches contact's avatar. First found is returned.
        *
        * @return Avatar (string or file). Ownership is transfered to client.
        */
        HBufC8* FetchAvatarL();

    private:  //From CActive

        void RunL();
        void DoCancel();
        TInt RunError( TInt aError ); 

    private:  //Data
        
        /// Server contact. Not owned.
        CCmsServerContact&                  iSyncContact;
        /// Type of fetch operation.
        TCmsAsyncOperation                  iAsyncOperation;
        /// Contact manager. Not owned.
        CVPbkContactManager*                iContactManager;
        /// Sort order manager. Owned.
        CPbk2SortOrderManager*              iSortOrderManager;
        /// Name formatter. Owned.
        MPbk2ContactNameFormatter*          iNameFormatter;
        /// Reference to CCmsServer. Not owned.
        CCmsServer&                         iCmsServer;

    };

#endif  //__CMSSERVERASYNCCONTACT__


// End of File
