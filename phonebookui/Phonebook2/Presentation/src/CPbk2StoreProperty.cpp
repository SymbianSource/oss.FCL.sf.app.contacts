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
* Description:  Phonebook 2 store property.
*
*/


#include <CPbk2StoreProperty.h>

// Phonebook 2
#include "CPbk2LocalizedText.h"
#include <CPbk2StoreViewDefinition.h>
#include <CPbk2FieldPropertyArray.h>
#include <CPbk2StorePropertyContentStoreName.h>

// System includes
#include <barsread.h>
#include <coemain.h>


// --------------------------------------------------------------------------
// CPbk2StoreProperty::CPbk2StoreProperty
// --------------------------------------------------------------------------
//
CPbk2StoreProperty::CPbk2StoreProperty()
    {
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::~CPbk2StoreProperty
// --------------------------------------------------------------------------
//
CPbk2StoreProperty::~CPbk2StoreProperty()
    {
    delete iStoreUri;
    delete iStoreName;
    iStoreViews.ResetAndDestroy();
    iLocalizedTexts.ResetAndDestroy();
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreProperty* CPbk2StoreProperty::NewL()
    {
    return new ( ELeave ) CPbk2StoreProperty;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreProperty* CPbk2StoreProperty::NewLC
        ( TResourceReader& aReader )
    {
    CPbk2StoreProperty* self = new ( ELeave ) CPbk2StoreProperty;
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreProperty* CPbk2StoreProperty::NewLC
        ( const CPbk2StoreProperty& aProperty )
    {
    CPbk2StoreProperty* self = new ( ELeave ) CPbk2StoreProperty;
    CleanupStack::PushL( self );
    self->ConstructL( aProperty );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreProperty::ConstructL( TResourceReader& aReader )
    {
    aReader.ReadInt8(); // read version number
    iStoreUri = aReader.ReadHBufCL();
    iStoreName = aReader.ReadHBufCL();
    iFlags = aReader.ReadUint32();
    TInt viewCount = aReader.ReadInt16();
    while (viewCount-- > 0 )
        {
        iStoreViews.AppendL(CPbk2StoreViewDefinition::NewLC(aReader));
        CleanupStack::Pop(); // CPbk2StoreView::NewLC(aReader)
        }
    iMemoryInfoTypes = aReader.ReadUint32();
    iFieldPropertiesResId = aReader.ReadInt32();
    TInt textCount = aReader.ReadInt16();
    while ( textCount-- > 0 )
        {
        iLocalizedTexts.AppendL( CPbk2LocalizedText::NewL( aReader ) );
        }
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreProperty::ConstructL( const CPbk2StoreProperty& aProperty )
    {
    iStoreUri = aProperty.StoreUri().UriDes().AllocL();
    iStoreName = aProperty.StoreName().AllocL();

    iFlags = aProperty.Flags();
    TArray<const CPbk2StoreViewDefinition*> views = aProperty.Views();
    TInt viewCount = views.Count();
    for (TInt i = 0; i < viewCount; ++i)
        {
        iStoreViews.AppendL(CPbk2StoreViewDefinition::NewLC(*views[i]));
        CleanupStack::Pop(); // CPbk2StoreView::NewLC(aReader)
        }
    iMemoryInfoTypes = aProperty.MemoryInfoTypes();
    iFieldPropertiesResId = aProperty.FieldPropertiesResId();
    CreateLocalizedTextsL( aProperty );
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::StoreUri
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkContactStoreUriPtr CPbk2StoreProperty::StoreUri() const
    {
    return TVPbkContactStoreUriPtr(*iStoreUri);
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::StoreName
// --------------------------------------------------------------------------
//
EXPORT_C const TDesC& CPbk2StoreProperty::StoreName() const
    {
    return *iStoreName;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::MemoryInfoTypes
// --------------------------------------------------------------------------
//
EXPORT_C TUint32 CPbk2StoreProperty::MemoryInfoTypes() const
    {
    return iMemoryInfoTypes;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::Views
// --------------------------------------------------------------------------
//
EXPORT_C TArray<const CPbk2StoreViewDefinition*>
        CPbk2StoreProperty::Views() const
    {
    return iStoreViews.Array();
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::Flags
// --------------------------------------------------------------------------
//
EXPORT_C TUint32 CPbk2StoreProperty::Flags() const
    {
    return iFlags;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::FieldPropertiesResId
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2StoreProperty::FieldPropertiesResId() const
    {
    return iFieldPropertiesResId;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::SetStoreUriL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreProperty::SetStoreUriL(const TDesC& aStoreUri)
    {
    HBufC* temp = aStoreUri.AllocL();
    delete iStoreUri;
    iStoreUri = temp;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::SetStoreNameL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreProperty::SetStoreNameL(const TDesC& aStoreName)
    {
    HBufC* temp = aStoreName.AllocL();
    delete iStoreName;
    iStoreName = temp;    
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::AppendViewDefinitionL
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreProperty::AppendViewDefinitionL
        ( CPbk2StoreViewDefinition* aViewDefinition )
    {
    iStoreViews.AppendL( aViewDefinition );
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::SetMemoryInfoTypes
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreProperty::SetMemoryInfoTypes(TUint32 aInfoTypes)
    {
    iMemoryInfoTypes = aInfoTypes;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::SetFlags
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreProperty::SetFlags(TUint32 aFlags)
    {
    iFlags = aFlags;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::SetFieldPropertiesResId
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreProperty::SetFieldPropertiesResId
        ( TInt aFieldPropertiesResId )
    {
    iFieldPropertiesResId = aFieldPropertiesResId;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::FindView
// --------------------------------------------------------------------------
//
EXPORT_C const CPbk2StoreViewDefinition* CPbk2StoreProperty::FindView
        ( TPbk2ContactViewType aViewType ) const
    {
    const CPbk2StoreViewDefinition* result = NULL;

    const TInt count = iStoreViews.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iStoreViews[i]->ViewType() == aViewType)
            {
            result = iStoreViews[i];
            break;
            }
        }

    return result;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::CreateStoreSpecificFieldPropertiesL
// --------------------------------------------------------------------------
//
EXPORT_C MPbk2FieldPropertyArray*
    CPbk2StoreProperty::CreateStoreSpecificFieldPropertiesL
        ( const MVPbkFieldTypeList& aFieldTypeList ) const
    {
    CPbk2FieldPropertyArray* props = NULL;
    if ( iFieldPropertiesResId )
        {
        TResourceReader reader;
        CCoeEnv::Static()->CreateResourceReaderLC
            (reader, iFieldPropertiesResId);
        props = CPbk2FieldPropertyArray::NewL(aFieldTypeList, reader,
            CCoeEnv::Static()->FsSession() );
        CleanupStack::PopAndDestroy(); // reader
        }
    return props;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::RetrieveContentLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2Content* CPbk2StoreProperty::RetrieveContentLC
        ( TPbk2ContentId aContext ) const
    {
    CPbk2Content* content = NULL;

    if ( aContext == EPbk2MemInfoPhone || aContext == EPbk2CopyQueryItem || aContext == EPbk2MemInfoContacts )
        {        
        // Both EPbk2MemInfoPhone EPbk2CopyQueryItem and EPbk2MemInfoContacts use same kind of 
        // content
        content =  new ( ELeave ) CPbk2StorePropertyContentStoreName
            (  iLocalizedTexts.Array(), aContext );
        CleanupStack::PushL( content );
        }

    return content;
    }

// --------------------------------------------------------------------------
// CPbk2StoreProperty::CreateLocalizedTextsL
// --------------------------------------------------------------------------
//
void CPbk2StoreProperty::CreateLocalizedTextsL( 
        const CPbk2StoreProperty& aProperty )
    {        
    // Remember to make copies of all supported content types.
    // Check RetrieveContentLC() to see all supported types.
    
    // Copy EPbk2MemInfoPhone
    CPbk2Content* content = 
        aProperty.RetrieveContentLC( EPbk2MemInfoPhone );
    CPbk2StorePropertyContentStoreName* propContent = 
        dynamic_cast<CPbk2StorePropertyContentStoreName*>( content );
    if ( propContent )
        {
        TInt uid = propContent->Uid();
        HBufC* text = propContent->StoreName().AllocLC();
        iLocalizedTexts.AppendL( CPbk2LocalizedText::NewL( uid, text ) );
        CleanupStack::Pop( text ); 
        // CPbk2LocalizedText::NewL has taken the ownership of text
        }
    CleanupStack::PopAndDestroy( content ); 
    
    // Copy EPbk2CopyQueryItem
    content = 
        aProperty.RetrieveContentLC( EPbk2CopyQueryItem );
    propContent = 
        dynamic_cast<CPbk2StorePropertyContentStoreName*>( content );
    if ( propContent )
        {
        TInt uid = propContent->Uid();
        HBufC* text = propContent->StoreName().AllocLC();
        iLocalizedTexts.AppendL( CPbk2LocalizedText::NewL( uid, text ) );
        CleanupStack::Pop( text ); 
        // CPbk2LocalizedText::NewL has taken the ownership of text
        }
    CleanupStack::PopAndDestroy( content );
    	
   	// Copy EPbk2MemInfoContacts
    content = 
        aProperty.RetrieveContentLC( EPbk2MemInfoContacts );
    propContent = 
        dynamic_cast<CPbk2StorePropertyContentStoreName*>( content );
    if ( propContent )
        {
        TInt uid = propContent->Uid();
        HBufC* text = propContent->StoreName().AllocLC();
        iLocalizedTexts.AppendL( CPbk2LocalizedText::NewL( uid, text ) );
        CleanupStack::Pop( text ); 
        // CPbk2LocalizedText::NewL has taken the ownership of text
        }
    CleanupStack::PopAndDestroy( content );
        
    }
    
// End of File
