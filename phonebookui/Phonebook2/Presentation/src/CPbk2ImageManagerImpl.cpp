/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides methods for CPbkImageManager implementation classes.
*
*/


// INCLUDE FILES
#include "CPbk2ImageManagerImpl.h"

// From Phonebook2
#include "CPbk2EmbeddedImage.h"

// From Virtual Phonebook
#include <MVPbkBaseContact.h>
#include <MVPbkStoreContact.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkFieldType.h>
#include <CVPbkContactManager.h>

// Unnamed namespace for local definitions
namespace
	{
	/**
	 * Finds field index from contact.
	 *
	 * @param aContactManager   Reference to contact manager.
	 * @param aContact          Reference to contact,
	 * @param aFieldType        Reference to field type.
	 * @return Pointer to found field. NULL if field cannot be found.
	 */
	template <class VPbkContactField, class VPbkContact>
    	VPbkContactField* FindFieldFromContactL(
    	CVPbkContactManager& aContactManager,
    	VPbkContact& aContact, 
    	const MVPbkFieldType& aFieldType)
    		{
    		const MVPbkFieldTypeList& supportedFieldTypes = 
				aContactManager.FieldTypes();			
			VPbkContactField* field = NULL;
			
		    for ( TInt n = 0; n < aContact.Fields().FieldCount(); ++n )
		        {
		        field = &aContact.Fields().FieldAt( n );

		        const TInt maxMatchPriority = 
		            supportedFieldTypes.MaxMatchPriority();
		        for (TInt matchPriority = 0; 
		             matchPriority <= maxMatchPriority; 
		             ++matchPriority)
		            {
		            const MVPbkFieldType* sourceFieldType = 
		                field->MatchFieldType( matchPriority );
		            if ( sourceFieldType && 
		                 aFieldType.IsSame( *sourceFieldType ) )
		                {	                
		                return field;
		                }
		            }
		        }
		    		 
			return NULL;
    		}
    		
	/**
	 * Finds field index from contact.
	 *
	 * @param aContactManager   Reference to contact manager.
	 * @param aContact          Reference to contact,
	 * @param aFieldType        Reference to field type.
	 * @return Found field index. KErrNotFound if field cannot be found.
	 */
   	TInt FindFieldIndex(
		CVPbkContactManager& aContactManager,
		const MVPbkBaseContact& aContact,
		const MVPbkFieldType& aFieldType)
		{
		const MVPbkBaseContactFieldCollection& fields = aContact.Fields();	
		const TInt fieldCount = fields.FieldCount();
	    
	    const TInt maxMatchPriority = 
	    	aContactManager.FieldTypes().MaxMatchPriority();
	    for (TInt matchPriority = 0; 
	    	matchPriority <= maxMatchPriority; 
	    	++matchPriority)
	        {            
	        for (TInt i = 0; i < fieldCount; ++i)
	            {
	            const MVPbkFieldType* fieldType = 
	                    fields.FieldAt(i).MatchFieldType(matchPriority);
	            if (fieldType && fieldType->IsSame(aFieldType))
	                {
	                return i;
	                }
	            }
	        }    
	    return KErrNotFound;
		}		
	} // unnamed namespace
// ================= MEMBER FUNCTIONS =======================

// --------------------------------------------------------------------------
// Pbk2ImageOperationFactory::CreateReaderLC
// --------------------------------------------------------------------------
//
MPbk2ImageReader* Pbk2ImageOperationFactory::CreateReaderLC
        (CVPbkContactManager& aContactManager,
        const MVPbkBaseContact& aContact,
        MPbk2ImageGetObserver& aObserver,
        const MVPbkFieldType& aFieldType)        
    {
    const MVPbkBaseContactField* field = 
    	FindFieldFromContactL<const MVPbkBaseContactField, const MVPbkBaseContact>(
    		aContactManager, aContact, aFieldType);
    
    MPbk2ImageReader* reader = NULL;
    if (field && SupportedStorageType(field))
        {
        reader = CPbk2EmbeddedImageReader::NewLC(*field,aObserver);
        }
    return reader;
    }

// --------------------------------------------------------------------------
// Pbk2ImageOperationFactory::CreateWriterLC
// --------------------------------------------------------------------------
//
MPbk2ImageWriter* Pbk2ImageOperationFactory::CreateWriterLC
        (CVPbkContactManager& aContactManager,
        MVPbkStoreContact& aContact, 
        MPbk2ImageSetObserver& aObserver,
        const MVPbkFieldType& aFieldType)
    {
    MPbk2ImageWriter* writer = NULL;

	// Find existing field from contact
    MVPbkStoreContactField* field = NULL;
    
    field =	FindFieldFromContactL<MVPbkStoreContactField, MVPbkStoreContact>(
    		aContactManager, aContact, aFieldType);
    	
    if (field && !SupportedStorageType(field))
        {
        // Existing field's storage type is not supported -> remove field
        TInt index(FindFieldIndex(
        	aContactManager, 
        	static_cast<const MVPbkBaseContact&>(aContact), 
        	aFieldType));
        User::LeaveIfError(index);
        aContact.RemoveField(index);
        field = NULL;
        }

    if (!field)
        {
        // No thumbnail field -> create one
        field = aContact.CreateFieldLC(aFieldType);
        TInt fieldIndex = aContact.AddFieldL(field); // takes ownership
        CleanupStack::Pop(); // field
		field = &aContact.Fields().FieldAt(fieldIndex);			
        }
	
	if (SupportedStorageType(field))
		{
		writer = 
		    CPbk2EmbeddedImageWriter::NewLC(
		        aContactManager, 
		        *field, 
		        aObserver);
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
		
    return writer;
    }

// --------------------------------------------------------------------------
// Pbk2ImageOperationFactory::SupportedStorageType
// --------------------------------------------------------------------------
//
TBool Pbk2ImageOperationFactory::SupportedStorageType
        (const MVPbkBaseContactField* aField)
    {
    TVPbkFieldStorageType storageType = aField->FieldData().DataType();
    return EVPbkFieldStorageTypeBinary==storageType ? ETrue: EFalse;
    }

//  End of File
