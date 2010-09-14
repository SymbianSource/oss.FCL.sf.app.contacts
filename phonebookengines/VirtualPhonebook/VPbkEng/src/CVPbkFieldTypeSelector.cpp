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
* Description:  Field Type selector
*
*/


#include <CVPbkFieldTypeSelector.h>

// Virtual Phonebook
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkBaseContactField.h>
#include <MVPbkFieldType.h>
#include <VPbkUtil.h>
#include <VPbkFieldType.hrh>
#include <VPbkError.h>
#include <MVPbkContactFieldData.h>
#include <MVPbkContactFieldUriData.h>
#include <vpbkeng.rsg>
#include <VPbkFieldTypeSelectorFactory.h>
#include <MVPbkStoreContactFieldCollection.h>

// System includes
#include <barsread.h>
#include <s32strm.h>
#include <s32mem.h>
#include <spsettings.h>
#include <spentry.h>
#include <spproperty.h>

/// Unnamed namespace for local definitions
namespace {

/// Field type selector static data
namespace KFieldTypeSelectorStatic
    {
    const TUint8 KVersionNumber = 1;
    }

/// Buffer sizes
namespace KFieldTypeSelectorExternalizeSizes
    {
    const TInt KVersionNumberSize = 1;
    const TInt KTypeCountSize = 2;
    const TInt KExtensionLengthSize = 2;
    const TInt KNonVersitTypeSize = 4;
    }

/**
 * Compares entry service name to scheme.
 * 
 * @param aEntry entry to be analyzed
 * @param aScheme to be compared with
 */
TBool CompareService(CSPEntry& aEntry, const TDesC& aScheme)
    {
    const TDesC& name = aEntry.GetServiceName();
    if (name.CompareF(aScheme) == KErrNone)
        {
        return ETrue;
        }
    return EFalse;
    }

/**
 * Compares entry based no contact action type.
 * 
 * @param aContactActionType contact action type
 * @param aEntry service entry
 * @param aScheme scheme
 */
TBool CompareContactActionType(
        TInt aContactActionType, CSPEntry& aEntry, const TDesC& aScheme)
    {
    TBool result = EFalse;
    const CSPProperty* prop = NULL;
    if (aContactActionType == 
            VPbkFieldTypeSelectorFactory::EInstantMessagingSelector)
        {
        if (aEntry.GetProperty(prop, ESubPropertyIMLaunchUid) == KErrNone)
            {
            result = CompareService(aEntry, aScheme);
            }
        }
    
    if (aContactActionType == 
            VPbkFieldTypeSelectorFactory::EVOIPCallSelector)
        {
        if (aEntry.GetProperty(prop, EPropertyServiceAttributeMask) == KErrNone)
            {
            TInt value = 0;
            prop->GetValue(value);
            if (value & ESupportsInternetCall)
                {
                result = CompareService(aEntry, aScheme);
                }
            }
        }
    return result;
    }


} /// namespace

struct CVPbkFieldTypeSelector::TSelectionProperty
    {
    /**
     * Resets selection properties.
     */
    void Reset();

    /// Own: selector version
    TUint8 iSelectorVersion;
    /// Own: Versit properties
    RArray<TVPbkFieldVersitProperty> iVersitProperties;
    /// Own: Exclude parameters for versit properties
    TVPbkFieldTypeParameters iExcludedTypeParameters;
    /// Own: Non-Versit types
    RArray<TVPbkNonVersitFieldType> iNonVersitTypes;
    /// Own: Versit property extension names store
    VPbkEngUtils::CTextStore* iTextStore;
    };


// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::CVPbkFieldTypeSelector
// --------------------------------------------------------------------------
//
void CVPbkFieldTypeSelector::TSelectionProperty::Reset()
    {
    iVersitProperties.Reset();
    iExcludedTypeParameters.Reset();
    iNonVersitTypes.Reset();
    delete iTextStore;
    iTextStore = NULL;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::CVPbkFieldTypeSelector
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector::CVPbkFieldTypeSelector
        ( const MVPbkFieldTypeList& aFieldTypeList ) :
            iFieldTypeList( aFieldTypeList )
    {
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::~CVPbkFieldTypeSelector
// --------------------------------------------------------------------------
//
CVPbkFieldTypeSelector::~CVPbkFieldTypeSelector()
    {
    SelectionPropertyReset();
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeSelector* CVPbkFieldTypeSelector::NewL
        ( TResourceReader& aReader, const MVPbkFieldTypeList& aFieldTypeList )
    {
    CVPbkFieldTypeSelector* self =
        new ( ELeave ) CVPbkFieldTypeSelector( aFieldTypeList );
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeSelector* CVPbkFieldTypeSelector::NewL
        ( const MVPbkFieldTypeList& aFieldTypeList )
    {
    CVPbkFieldTypeSelector* self =
        new ( ELeave ) CVPbkFieldTypeSelector( aFieldTypeList );
    CleanupStack::PushL( self );
    self->CommonConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CVPbkFieldTypeSelector* CVPbkFieldTypeSelector::NewL
        ( const CVPbkFieldTypeSelector& aSelector )
    {
    CVPbkFieldTypeSelector* self =
        new ( ELeave ) CVPbkFieldTypeSelector( aSelector.iFieldTypeList );
    CleanupStack::PushL( self );
    self->ConstructL( aSelector );
    CleanupStack::Pop( self );
    return self;

    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::CommonConstructL
// --------------------------------------------------------------------------
//
void CVPbkFieldTypeSelector::CommonConstructL()
    {
    }
    
// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::ConstructL
// --------------------------------------------------------------------------
//
inline void CVPbkFieldTypeSelector::ConstructL( TResourceReader& aReader )
    {
    CommonConstructL();
    DoAppendL( aReader );
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::ConstructL
// --------------------------------------------------------------------------
//
inline void CVPbkFieldTypeSelector::ConstructL
        ( const CVPbkFieldTypeSelector& aSelector )
    {
    CommonConstructL();
    iContactActionType = aSelector.iContactActionType;

    TInt i = 0;
    
    TInt count = aSelector.iSelectionProperties.Count();
    while (--count >= 0)
        {
        TSelectionProperty fromProperty = aSelector.iSelectionProperties[count];

        // create property to copy to
        TSelectionProperty selectionProperty;
        selectionProperty.iTextStore = new (ELeave) VPbkEngUtils::CTextStore;
        CleanupStack::PushL(selectionProperty.iTextStore);
        
        selectionProperty.iSelectorVersion = fromProperty.iSelectorVersion;
        
        const TInt versitPropertyCount = fromProperty.iVersitProperties.Count();
        for ( i = 0; i < versitPropertyCount; ++i )
            {
            User::LeaveIfError( selectionProperty.iVersitProperties.Append
                ( fromProperty.iVersitProperties[i] ) );
            TVPbkFieldVersitProperty& property =
                selectionProperty.iVersitProperties[
                    selectionProperty.iVersitProperties.Count()-1];
            if ( property.ExtensionName().Length() > 0 )
                {
                property.SetExtensionName
                    ( selectionProperty.iTextStore->AddL( property.ExtensionName() ) );
                }
            }
        
        // copy exclude params
        TUint32* slots = fromProperty.iExcludedTypeParameters.FieldTypeParameters();
        TUint32* targetSlots = selectionProperty.iExcludedTypeParameters.FieldTypeParameters();
        for (TInt i = 0; 
             i < fromProperty.iExcludedTypeParameters.Size();
             ++i)
            {
            targetSlots[i] = slots[i];
            }
    
        const TInt nonVersitTypeCount = fromProperty.iNonVersitTypes.Count();
        for ( TInt i = 0; i < nonVersitTypeCount; ++i )
            {
            User::LeaveIfError( selectionProperty.iNonVersitTypes.Append
                ( fromProperty.iNonVersitTypes[i] ) );
            }

        iSelectionProperties.AppendL(selectionProperty);
        CleanupStack::Pop(selectionProperty.iTextStore);
        }
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::ExternalizeLC
// Field type selector pack format 1:
//
// stream := StreamVersion SelectorVersion VersitPropertyCount
//           NonVersitTypeCount VersitProperty* NonVersitType*
// StreamVersion := TUint8
// SelectorVersion := TUint8
// VersitPropertyCount := TUint16
// NonVersitTypeCount := TUint16
// VersitProperty := PropertyData ExtensionNameLength ExtensionName
// PropertyData := sizeof(TVPbkFieldVersitProperty)
// ExtensionNameLength := TUint16
// ExtensionName := char*
// NonVersitType := TInt32
// --------------------------------------------------------------------------
//
EXPORT_C HBufC8* CVPbkFieldTypeSelector::ExternalizeLC() const
    {
    const TInt bufferSize = CalculateBufferSize();
    HBufC8* buffer = HBufC8::NewLC( bufferSize );
    TPtr8 bufPtr( buffer->Des() );
    RDesWriteStream writeStream( bufPtr );
    writeStream.PushL();
    DoFillBufferL( writeStream );
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    return buffer;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::InternalizeL
// --------------------------------------------------------------------------
//
EXPORT_C void CVPbkFieldTypeSelector::InternalizeL
        ( const TDesC8& aBuffer )
    {
    SelectionPropertyReset();

    RDesReadStream readStream( aBuffer );
    readStream.PushL();

    // Pack format version number
    const TInt vsn = readStream.ReadUint8L();
    __ASSERT_ALWAYS( vsn == KFieldTypeSelectorStatic::KVersionNumber,
            VPbkError::Panic( VPbkError::EInvalidTypeParameter ) );

    const TInt selectionPropertyCount = readStream.ReadUint16L();

    for (TInt i = 0; i < selectionPropertyCount; ++i)
        {
        TSelectionProperty selectionProperty;
        selectionProperty.iTextStore = new (ELeave) VPbkEngUtils::CTextStore;
        CleanupStack::PushL(selectionProperty.iTextStore);
    
        // VPBK_FIELD_TYPE_SELECTOR version number.
        selectionProperty.iSelectorVersion = readStream.ReadUint8L();
        
        // Read count of types
        TInt versitCount = readStream.ReadUint16L();
    
        TInt excludeParamsSize = 0;
        if (selectionProperty.iSelectorVersion >= EVPbkFieldTypeSelectorV2)
            {
            excludeParamsSize = readStream.ReadUint16L();
            }
        TInt nonVersitCount = readStream.ReadUint16L();
    
        // Read types
        for ( TInt i = 0; i < versitCount; ++i )
            {
            // Read basic Versit property
            TVPbkFieldVersitProperty property;
            TPckg<TVPbkFieldVersitProperty> versitPckg( property );
            readStream.ReadL( versitPckg, versitPckg.Size() ); 
            
            // Read ext name
            TInt extNameLength = readStream.ReadUint16L();
            if ( extNameLength > 0 )
                {
                HBufC8* extName = HBufC8::NewLC( extNameLength );
                TPtr8 extNamePtr( extName->Des() );
                readStream.ReadL( extNamePtr, extNameLength );
                if ( extName->Length() > 0 )
                    {
                    property.SetExtensionName( selectionProperty.iTextStore->AddL( *extName ) );
                    }
                CleanupStack::PopAndDestroy( extName ); // extName
                }
    
            selectionProperty.iVersitProperties.AppendL( property );
            }
    
        TUint32* slots = selectionProperty.iExcludedTypeParameters.FieldTypeParameters();
        for (TInt i = 0; i < excludeParamsSize; ++i)
            {
            slots[i] = readStream.ReadUint32L();
            }
    
        for ( TInt i = 0; i < nonVersitCount; ++i )
            {
            TVPbkNonVersitFieldType type = 
                static_cast<TVPbkNonVersitFieldType>( readStream.ReadInt32L() );
            selectionProperty.iNonVersitTypes.AppendL( type );
            }
    
        iSelectionProperties.AppendL(selectionProperty);
        CleanupStack::Pop(selectionProperty.iTextStore);
        }
    
    iContactActionType = readStream.ReadInt32L();
    CleanupStack::PopAndDestroy( &readStream );
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::MatchingVersitProperty
// --------------------------------------------------------------------------
//
EXPORT_C const TVPbkFieldVersitProperty*
    CVPbkFieldTypeSelector::MatchingVersitProperty(
        const MVPbkFieldType& aFieldType ) const
    {
    const TVPbkFieldVersitProperty* ret = NULL;

    TInt count = iSelectionProperties.Count();
    while (--count >= 0 && ret == NULL)
        {
        TSelectionProperty selectionProperty = iSelectionProperties[count];
        const TInt versitPropertyCount = selectionProperty.iVersitProperties.Count();
        for (TInt i = 0; i < versitPropertyCount && !ret; ++i )
            {
            const TInt maxMatchPriority =
                aFieldType.VersitProperties().Count() - 1;
            for ( TInt matchPriority = 0;
                    matchPriority <= maxMatchPriority; ++matchPriority )
                {
                if ( selectionProperty.iVersitProperties[i].Matches(
                        aFieldType.VersitProperties()[matchPriority] ) )
                    {
                    ret = &selectionProperty.iVersitProperties[i];
                    break;
                    }
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::MatchingNonVersitType
// --------------------------------------------------------------------------
//
EXPORT_C TVPbkNonVersitFieldType CVPbkFieldTypeSelector::MatchingNonVersitType(
        const MVPbkFieldType& aFieldType ) const
    {
    TVPbkNonVersitFieldType ret = EVPbkNonVersitTypeNone;

    TInt count = iSelectionProperties.Count();
    while (--count >= 0 && ret == EVPbkNonVersitTypeNone)
        {
        TSelectionProperty selectionProperty = iSelectionProperties[count];    
        const TInt nonVersitTypeCount = selectionProperty.iNonVersitTypes.Count();
        for (TInt i = 0; i < nonVersitTypeCount; ++i )
            {
            if ( selectionProperty.iNonVersitTypes[i] == aFieldType.NonVersitType() )
                {
                ret = selectionProperty.iNonVersitTypes[i];
                break;
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::FieldTypeList
// --------------------------------------------------------------------------
//
const MVPbkFieldTypeList&
        CVPbkFieldTypeSelector::FieldTypeList() const
    {
    return iFieldTypeList;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::AppendL
// --------------------------------------------------------------------------
//
void CVPbkFieldTypeSelector::AppendL( TResourceReader& aReader )
    {
    DoAppendL( aReader );
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::SetContactActionType
// --------------------------------------------------------------------------
//
void CVPbkFieldTypeSelector::SetContactActionType(TInt aContactActionType)
    {
    iContactActionType = aContactActionType;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::IsFieldIncluded
// --------------------------------------------------------------------------
//
TBool CVPbkFieldTypeSelector::IsFieldIncluded
        ( const MVPbkBaseContactField& aField ) const
    {
    const TInt maxMatchPriority = iFieldTypeList.MaxMatchPriority();
    for ( TInt matchPriority = 0;
            matchPriority <= maxMatchPriority; ++matchPriority )
        {
        const MVPbkFieldType* ft = aField.MatchFieldType( matchPriority );
        if ( ft && IsFieldTypeIncluded( *ft ) && IsFieldDataIncluded(aField))
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::AddressFieldsIncluded
// --------------------------------------------------------------------------
//
EXPORT_C TInt CVPbkFieldTypeSelector::AddressFieldsIncluded
        ( const MVPbkStoreContactFieldCollection& aFields ) const
    {
    TBool addressGeneric = EFalse;
    TBool addressHome = EFalse;
    TBool addressWork = EFalse;
    for ( TInt i = 0; i < aFields.FieldCount(); ++i )
    	{
    	const MVPbkStoreContactField& field = aFields.FieldAt( i );
        const TInt maxMatchPriority = iFieldTypeList.MaxMatchPriority();
        for ( TInt matchPriority = 0;
                matchPriority <= maxMatchPriority; ++matchPriority )
            {
            const MVPbkFieldType* ft = field.MatchFieldType( matchPriority );
            if ( ft && IsFieldTypeIncluded( *ft )
                    && IsFieldDataIncluded( field ) )
                {
                TArray<TVPbkFieldVersitProperty> props =
                    ft->VersitProperties();
                for ( TInt ii = 0; ii < props.Count(); ii++ )
                	{
                	if ( props[ii].Parameters().Contains( EVPbkVersitParamHOME ) )
                		{
                		addressHome = ETrue;
                		}
                	else if ( props[ii].Parameters().Contains( EVPbkVersitParamWORK ) )
                	    {
                	    addressWork = ETrue;
                	    }
                	else
                	    {
                	    addressGeneric = ETrue;
                	    }
                	}
                }
            }
    	}
    return addressGeneric + addressHome + addressWork;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::IsFieldTypeIncluded
// --------------------------------------------------------------------------
//
TBool CVPbkFieldTypeSelector::IsFieldTypeIncluded
        ( const MVPbkFieldType& aFieldType ) const
    {
    TInt i;
    TBool ret = EFalse;

    TInt count = iSelectionProperties.Count();
    while (--count >= 0 && ret == EFalse)
        {
        TSelectionProperty selectionProperty = iSelectionProperties[count];    
    
        // First match Versit properties
        const TInt versitPropertyCount = selectionProperty.iVersitProperties.Count();
        for ( i = 0; i < versitPropertyCount; ++i )
            {
            const TInt maxMatchPriority =
                aFieldType.VersitProperties().Count() - 1;
            for ( TInt matchPriority = 0;
                    matchPriority <= maxMatchPriority; ++matchPriority )
                {
                TVPbkFieldVersitProperty prop = aFieldType.VersitProperties()[matchPriority];            
                if ( selectionProperty.iVersitProperties[i].Matches(prop) &&
                     prop.Parameters().ContainsNone(selectionProperty.iExcludedTypeParameters) )
                    {
                    ret = ETrue;
                    break;
                    }
                }
            if ( ret )
                {
                break;
                }
            }

        if ( !ret )
            {
            // Then match non-Versit properties
            const TInt nonVersitTypeCount = selectionProperty.iNonVersitTypes.Count();
            for ( i = 0; i < nonVersitTypeCount; ++i )
                {
                if ( selectionProperty.iNonVersitTypes[i] == aFieldType.NonVersitType() )
                    {
                    ret = ETrue;
                    break;
                    }
                }
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::DoAppendL
// --------------------------------------------------------------------------
//
void CVPbkFieldTypeSelector::DoAppendL( TResourceReader& aReader )
    {
    TSelectionProperty selectionProperty;
    selectionProperty.iTextStore = new (ELeave) VPbkEngUtils::CTextStore;
    CleanupStack::PushL(selectionProperty.iTextStore);

    // VPBK_FIELD_TYPE_SELECTOR version number.
    selectionProperty.iSelectorVersion = aReader.ReadUint8();

    TInt i = 0;
    const TInt versitPropertyCount = aReader.ReadInt16();
    for ( i = 0; i < versitPropertyCount; ++i )
        {
        TVPbkFieldVersitProperty versitProperty( aReader );
        User::LeaveIfError( selectionProperty.iVersitProperties.Append( versitProperty ) );
        TVPbkFieldVersitProperty& property =
            selectionProperty.iVersitProperties[selectionProperty.iVersitProperties.Count()-1];
        if ( property.ExtensionName().Length() > 0 )
            {
            property.SetExtensionName
                ( selectionProperty.iTextStore->AddL( property.ExtensionName() ) );
            }
        }

    const TInt nonVersitTypeCount = aReader.ReadInt16();
    for ( i = 0; i < nonVersitTypeCount; ++i )
        {
        TVPbkNonVersitFieldType nonVersitType =
            static_cast<TVPbkNonVersitFieldType>( aReader.ReadUint8() );
        User::LeaveIfError( selectionProperty.iNonVersitTypes.Append( nonVersitType ) );
        }

    if (selectionProperty.iSelectorVersion >= EVPbkFieldTypeSelectorV2)
        {
        selectionProperty.iExcludedTypeParameters.AddFromResource(aReader);
        }

    iSelectionProperties.AppendL(selectionProperty);
    CleanupStack::Pop(); // selectionProperty.iTextStore
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::CalculateBufferSize
// --------------------------------------------------------------------------
//
TInt CVPbkFieldTypeSelector::CalculateBufferSize() const
    {
    // Add packet version size
    TInt bufferSize = KFieldTypeSelectorExternalizeSizes::KVersionNumberSize;
    
    TInt count = iSelectionProperties.Count();
    bufferSize += sizeof(TUint16);
    while (--count >= 0)
        {
        TSelectionProperty selectionProperty = iSelectionProperties[count];    
        
        // Add selector version size
        bufferSize += KFieldTypeSelectorExternalizeSizes::KVersionNumberSize;
        
        // Then, allocate size for Versit and non-Versit type counts
        bufferSize += KFieldTypeSelectorExternalizeSizes::KTypeCountSize;
        
        if (selectionProperty.iSelectorVersion >= EVPbkFieldTypeSelectorV2)
            {
            // add exclude params size
            bufferSize += KFieldTypeSelectorExternalizeSizes::KTypeCountSize;
            }
        bufferSize += KFieldTypeSelectorExternalizeSizes::KTypeCountSize;
        
        const TInt propertyCount = selectionProperty.iVersitProperties.Count();
        for (TInt i = 0; i < propertyCount; ++i)
            {
            // Add size of property data
            TVPbkFieldVersitProperty tempProp;
            TPckg<TVPbkFieldVersitProperty> tempPckg( tempProp );
            bufferSize += tempPckg.Size();
            // Add space for extension length
            bufferSize += KFieldTypeSelectorExternalizeSizes::KExtensionLengthSize;
            if ( selectionProperty.iVersitProperties[i].ExtensionName().Length() > 0 )
                {
                // Add space for extension name
                bufferSize += selectionProperty.iVersitProperties[i].ExtensionName().Size();
                }
            }

        if (selectionProperty.iSelectorVersion >= EVPbkFieldTypeSelectorV2)
            {
            for (TInt i = 0; i < selectionProperty.iExcludedTypeParameters.Size(); ++i)
                {
                bufferSize += sizeof(TUint32);
                }
            }
    
        // Add non-versit property sizes
        for (TInt i = 0; i < selectionProperty.iNonVersitTypes.Count(); ++i)
            {
            bufferSize += KFieldTypeSelectorExternalizeSizes::KNonVersitTypeSize;
            }
        }
    
    // add contact action type size
    bufferSize += sizeof(TInt32);
    
    return bufferSize;
    }

// --------------------------------------------------------------------------
// CVPbkFieldTypeSelector::DoFillBufferL
// --------------------------------------------------------------------------
//
void CVPbkFieldTypeSelector::DoFillBufferL( RWriteStream& aWriteStream ) const
    {
    // Write package version number
    aWriteStream.WriteUint8L( KFieldTypeSelectorStatic::KVersionNumber );

    TInt count = iSelectionProperties.Count();
    aWriteStream.WriteUint16L(count);
    while (--count >= 0)
        {
        TSelectionProperty selectionProperty = iSelectionProperties[count];    

        // Write selector version number
        aWriteStream.WriteUint8L( selectionProperty.iSelectorVersion );
    
        // Write type counts to the stream
        aWriteStream.WriteUint16L( selectionProperty.iVersitProperties.Count() );
        
        if (selectionProperty.iSelectorVersion >= EVPbkFieldTypeSelectorV2)
            {
            aWriteStream.WriteUint16L( selectionProperty.iExcludedTypeParameters.Size() );
            }

        aWriteStream.WriteUint16L( selectionProperty.iNonVersitTypes.Count() );
    
        // Write types to the stream
        for ( TInt i = 0; i < selectionProperty.iVersitProperties.Count(); ++i )
            {
            // Write basic Versit property
            TVPbkFieldVersitProperty property = selectionProperty.iVersitProperties[i];
            TPckg<TVPbkFieldVersitProperty> versitPckg( property );
            aWriteStream.WriteL( versitPckg );
            // Then write property ext name separately
            TInt extNameLength = property.ExtensionName().Length();
            aWriteStream.WriteUint16L( extNameLength );
            if ( extNameLength > 0 )
                {
                aWriteStream.WriteL( property.ExtensionName() );
                }
            }

        if (selectionProperty.iSelectorVersion >= EVPbkFieldTypeSelectorV2)
            {
            const TUint32* slots = selectionProperty
                .iExcludedTypeParameters.FieldTypeParameters();
            for (TInt i = 0; i < selectionProperty.iExcludedTypeParameters.Size(); ++i)
                {
                aWriteStream.WriteUint32L(slots[i]);
                }
            }

        for ( TInt j = 0; j < selectionProperty.iNonVersitTypes.Count(); ++j )
            {
            // Enumeration is treated as TInt
            TInt nonVersitType = 
                static_cast<TVPbkNonVersitFieldType>( selectionProperty.iNonVersitTypes[j] );
            aWriteStream.WriteInt32L( nonVersitType );
            }
        }
    
    aWriteStream.WriteInt32L(iContactActionType);
    }

void CVPbkFieldTypeSelector::SelectionPropertyReset()
    {
    TInt count = iSelectionProperties.Count();
    while (--count >= 0)
        {
        iSelectionProperties[count].Reset();
        }
    iSelectionProperties.Reset();
    }

TBool CVPbkFieldTypeSelector::IsFieldDataIncluded(
        const MVPbkBaseContactField& aField) const
    {
    TBool result = EFalse;    
    // assume that field is not included if leaves
    TRAP_IGNORE(result = DoIsFieldDataIncludedL(aField));
    return result;
    }

TBool CVPbkFieldTypeSelector::DoIsFieldDataIncludedL(
        const MVPbkBaseContactField& aField) const
    {
    const MVPbkContactFieldData& data = aField.FieldData();
    // by default data is included
    TBool result = ETrue;

    const MVPbkFieldType* type = aField.BestMatchingFieldType();
    if (!data.IsEmpty() && type && type->FieldTypeResId() == R_VPBK_FIELD_TYPE_IMPP)
        {
        result = EFalse;
        const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast(data); 
        TPtrC scheme = uri.Scheme();

        CSPSettings* settings = CSPSettings::NewLC();
        RIdArray idArray;
        CleanupClosePushL(idArray);
        
        TInt error = settings->FindServiceIdsL(idArray);
        if (error == KErrNone)
            {
            for (TInt i = 0; !result && i < idArray.Count();++i)
                {
                CSPEntry* entry = CSPEntry::NewLC();
                settings->FindEntryL(idArray[i], *entry);
                
                result = CompareContactActionType(
                    iContactActionType, *entry, scheme);

                CleanupStack::PopAndDestroy();
                }
            }
        CleanupStack::PopAndDestroy(2); // idArray, settings
        }
    return result;
    }

// End of File
