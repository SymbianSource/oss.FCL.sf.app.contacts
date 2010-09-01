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
*       Provides Phonebook2 image manager methods.
*
*/



// INCLUDE FILES
#include "CPbk2ImageManager.h"

// From Phonebook2
#include "MPbk2ImageOperationObservers.h"
#include "CPbk2ImageManagerImpl.h"

// From Virtual Phonebook
#include "CVPbkContactManager.h"
#include "MVPbkFieldType.h"
#include <MVPbkStoreContact.h>
#include <MVPbkContactFieldData.h>

// From System

// Unnamed namespace for local definitions
namespace
	{	
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
// CPbk2ImageManager::CPbk2ImageManager
// --------------------------------------------------------------------------
//
inline CPbk2ImageManager::CPbk2ImageManager
        (CVPbkContactManager& aContactManager)
        : iContactManager(aContactManager)
    {
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::~CPbk2ImageManager
// --------------------------------------------------------------------------
//
CPbk2ImageManager::~CPbk2ImageManager()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2ImageManager* CPbk2ImageManager::NewL(
        CVPbkContactManager& aContactManager)
    {
    CPbk2ImageManager* self = 
        new(ELeave) CPbk2ImageManager(aContactManager);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self); 
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ImageManager::ConstructL()
    {
    }
    
// --------------------------------------------------------------------------
// CPbk2ImageManager::GetImageAsyncL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2ImageOperation* CPbk2ImageManager::GetImageAsyncL
	(TPbk2ImageManagerParams* aParams,
	 MVPbkBaseContact& aContact,
	 const MVPbkFieldType& aFieldType,
	 MPbk2ImageGetObserver& aObserver)
    {
    MPbk2ImageReader* reader = Pbk2ImageOperationFactory::CreateReaderLC
        (iContactManager, aContact, aObserver, aFieldType);
    if (reader)
        {
        reader->StartReadL(aParams);
        CleanupStack::Pop();  // reader
        }
    return reader;
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::SetImageAsyncL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2ImageOperation* CPbk2ImageManager::SetImageAsyncL(
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
    	MPbk2ImageSetObserver& aObserver,
    	const CFbsBitmap& aBitmap)
    {
    MPbk2ImageWriter* writer = Pbk2ImageOperationFactory::CreateWriterLC
        (iContactManager, aContact, aObserver, aFieldType);
    if ( writer )
        {
        writer->StartWriteL(aBitmap);
        CleanupStack::Pop();  // writer
        }    
    return writer;
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::SetImageAsyncL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2ImageOperation* CPbk2ImageManager::SetImageAsyncL(
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
    	MPbk2ImageSetObserver& aObserver,
    	const TDesC8& aImageBuffer)
    {
    MPbk2ImageWriter* writer = Pbk2ImageOperationFactory::CreateWriterLC
        (iContactManager, aContact, aObserver, aFieldType);
    writer->StartWriteL(aImageBuffer);
    CleanupStack::Pop();  // writer
    return writer;
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::SetImageAsyncL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2ImageOperation* CPbk2ImageManager::SetImageAsyncL(
        MVPbkStoreContact& aContact,
        const MVPbkFieldType& aFieldType,
    	MPbk2ImageSetObserver& aObserver,
    	const TDesC& aFileName)
    {
    MPbk2ImageWriter* writer = Pbk2ImageOperationFactory::CreateWriterLC
        (iContactManager, aContact, aObserver, aFieldType);
    writer->StartWriteL(aFileName);
    CleanupStack::Pop();  // writer
    return writer;
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::HasImage
// --------------------------------------------------------------------------
//
EXPORT_C TBool CPbk2ImageManager::HasImage
        (const MVPbkBaseContact& aContact, 
   		const MVPbkFieldType& aFieldType) const
    {
    TInt fieldId = FindFieldIndex(iContactManager, aContact, aFieldType);
    if ( fieldId != KErrNotFound )
        {
        const MVPbkBaseContactField& field = aContact.Fields().FieldAt(fieldId);
        if ( !field.FieldData().IsEmpty() )
            {
            return ETrue;
            }
        }
	return EFalse;
    }

// --------------------------------------------------------------------------
// CPbk2ImageManager::RemoveImage
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2ImageManager::RemoveImage(
	MVPbkStoreContact& aContact,
    const MVPbkFieldType& aFieldType)
    {    
    TInt index(FindFieldIndex(iContactManager, aContact, aFieldType));
    if (index != KErrNotFound)
    	{
	    aContact.RemoveField(index);    	
    	}
    }

//  End of File
