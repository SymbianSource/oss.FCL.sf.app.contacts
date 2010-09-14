/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*     Field property for a Phonebook 2 field type.
*
*/


// INCLUDE FILES
#include "CPbk2StorePropertyArray.h"

// From Phonebook2
#include "Pbk2PresentationUtils.h"
#include "CPbk2StoreProperty.h"
#include <RPbk2LocalizedResourceFile.h>
#include <pbk2presentation.rsg>
#include <Pbk2DataCaging.hrh>

// From Virtual Phonebook

// From System
#include <barsread.h>
#include <coemain.h>

// ========================= MEMBER FUNCTIONS ===============================

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::CPbk2StorePropertyArray
// --------------------------------------------------------------------------
//
CPbk2StorePropertyArray::CPbk2StorePropertyArray()
    {
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::~CPbk2StorePropertyArray
// --------------------------------------------------------------------------
//
CPbk2StorePropertyArray::~CPbk2StorePropertyArray()
    {
    iStoreProperties.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StorePropertyArray* CPbk2StorePropertyArray::NewL(
        TResourceReader& aReader)
    {
    CPbk2StorePropertyArray* self = new(ELeave) CPbk2StorePropertyArray;
    CleanupStack::PushL(self);
    self->ConstructFromResourceL(aReader);
    CleanupStack::Pop(self);
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StorePropertyArray* CPbk2StorePropertyArray::NewL()
    {
    CPbk2StorePropertyArray* self = new(ELeave) CPbk2StorePropertyArray;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
            
// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StorePropertyArray::ConstructL()
    {
    RPbk2LocalizedResourceFile resFile(*CCoeEnv::Static());
    resFile.OpenLC(KPbk2RomFileDrive, 
                   KDC_RESOURCE_FILES_DIR, 
                   Pbk2PresentationUtils::PresentationResourceFile());
    
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC
        (reader, R_PHONEBOOK2_STORE_PROPERTIES);
    ConstructFromResourceL(reader);
        
    CleanupStack::PopAndDestroy(2); // reader, resFile
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::ConstructFromResourceL
// --------------------------------------------------------------------------
//
void CPbk2StorePropertyArray::ConstructFromResourceL(TResourceReader& aReader)
    {
    TInt storePropertyCount = aReader.ReadInt16();
    while (storePropertyCount--)
        {
        iStoreProperties.AppendL(CPbk2StoreProperty::NewLC(aReader));
        CleanupStack::Pop(); // CPbk2StoreProperty::NewLC(aReader)
        }
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::Count
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2StorePropertyArray::Count() const
    {
    return iStoreProperties.Count();
    }
    
// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::At
// --------------------------------------------------------------------------
//
EXPORT_C const CPbk2StoreProperty& CPbk2StorePropertyArray::At(TInt aIndex) const
    {
    return *iStoreProperties[aIndex];   
    }
        
// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::FindProperty
// --------------------------------------------------------------------------
//
EXPORT_C const CPbk2StoreProperty* CPbk2StorePropertyArray::FindProperty(
        const TVPbkContactStoreUriPtr& aUriPtr) const
    {
    const CPbk2StoreProperty* result = NULL;
    
    const TInt count = iStoreProperties.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iStoreProperties[i]->StoreUri().UriDes().Compare(aUriPtr.UriDes()) == 0)
            {
            result = iStoreProperties[i];
            break;
            }
        }
    
    return result;
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::AppendFromResourceL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StorePropertyArray::AppendFromResourceL(
        TResourceReader& aReader)
    {
    CPbk2StorePropertyArray* newProps = CPbk2StorePropertyArray::NewL(aReader);
    CleanupStack::PushL(newProps);
    AppendFromArrayL(*newProps);
    CleanupStack::PopAndDestroy(newProps);
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::AppendFromArrayL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StorePropertyArray::AppendFromArrayL(
        CPbk2StorePropertyArray& aPropertyArray)
    {
    const TInt count = aPropertyArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2StoreProperty* prop = CPbk2StoreProperty::NewLC(aPropertyArray.At(i));
        AppendL(prop);
        CleanupStack::Pop(prop);  
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::AppendL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StorePropertyArray::AppendL(CPbk2StoreProperty* aProperty)
    {
    RemoveDuplicates(*aProperty);
    iStoreProperties.AppendL(aProperty);
    }

// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::DeleteProperty
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StorePropertyArray::DeleteProperty(
        const TVPbkContactStoreUriPtr& aUriPtr)
    {
    TInt index = FindPropertyIndex(aUriPtr);
    if (index != KErrNotFound) 
        {
        delete iStoreProperties[index];
        iStoreProperties.Remove(index);
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::RemoveDuplicates
// --------------------------------------------------------------------------
//
void CPbk2StorePropertyArray::RemoveDuplicates(CPbk2StoreProperty& aProperty)
    {
    TVPbkContactStoreUriPtr ptr(aProperty.StoreUri());
    if (FindProperty(ptr))
        {
        DeleteProperty(ptr);
        }
    }
    
// --------------------------------------------------------------------------
// CPbk2StorePropertyArray::FindPropertyIndex
// --------------------------------------------------------------------------
//
TInt CPbk2StorePropertyArray::FindPropertyIndex(
    const TVPbkContactStoreUriPtr& aUriPtr)
    {
    TInt result = KErrNotFound;
    const TInt count = iStoreProperties.Count();
    for (TInt i = 0; (i < count) && (result == KErrNotFound); ++i)
        {
        if (iStoreProperties[i]->StoreUri().UriDes().Compare(aUriPtr.UriDes()) == 0)
            {
            result = i;
            }
        }
    
    return result;
    }

// End of File
