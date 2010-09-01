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
* Description:  A class that reads an array of PSU2_COPY_TO_SIM_FIELDTYPE_INFO
*                items
*
*/



// INCLUDE FILES
#include "CPsu2CopyToSimFieldInfoArray.h"

#include <RPbk2LocalizedResourceFile.h>
#include <Pbk2DataCaging.hrh>
#include <Pbk2USimUIRes.rsg>
#include <Pbk2CommonUi.rsg>

#include <TVPbkFieldTypeMapping.h>
#include <CVPbkFieldTypeSelector.h>
#include <MVPbkFieldType.h>

#include <coemain.h>
#include <barsread.h>
#include <gsmerror.h>
#include <featmgr.h>
#include <VPbkEng.rsg>

namespace
{

_LIT(KPsu2USimExtensionResFile, "Pbk2USimUIRes.rsc");

// ============================= LOCAL FUNCTIONS ===============================

#ifdef _DEBUG
enum TPanicCode
    {
    EPostCond_ConstructL,
    EPostCond_CreateMappingsL
    };

void Panic(TInt aReason)
    {
    _LIT(KPanicText, "SimFieldInfoArray");
    User::Panic(KPanicText, aReason);
    }
#endif // _DEBUG
}

/**
 * A class that can read PSU2_COPY_TO_SIM_FIELDTYPE_MAPPING resource
 * structure from Pbk2USimUi.rh
 */
class CPsu2CopyToSimFieldtypeMapping : public CBase
    {
    public:
    
        /**
         * @param aReader a resource reader to 
         *  PSU2_COPY_TO_SIM_FIELDTYPE_MAPPING.
         * @param aMasterFieldTypeList all field types from CVPbkContactManager
         */
        static CPsu2CopyToSimFieldtypeMapping* NewLC( 
            TResourceReader& aReader,
            const MVPbkFieldTypeList& aMasterFieldTypeList );
        
        // Destructor
        ~CPsu2CopyToSimFieldtypeMapping();
        
        /**
         * The selector for the source field types.
         *
         * @return The selector for the source field types.
         */
        MVPbkFieldTypeSelector& SourceTypeSelector() const;
        
        /**
         * Returns the resource id (VPbkEng.rsg) of the target field type.
         *
         * @return the resource id (VPbkEng.rsg) of the target field type.
         */
        TInt TargetFieldtypeResId() const;
    
        /**
         * Returns flags defined Pbk2UsimUi.hrh
         *
         * @return flags defined Pbk2UsimUi.hrh
         */
        TUint8 Flags() const;
        
        /**
         * @return TPsu2ErrorCode that causes this sim field type 
         *  to be blocked.
         */
        TUint8 BlockingError() const;
    
    private:
        void ConstructL( TResourceReader& aReader,
            const MVPbkFieldTypeList& aMasterFieldTypeList );
            
    private: // Data
        /// Own: source field type selector
        CVPbkFieldTypeSelector* iSourceTypeSelector;
        /// Own: target field type resource id (VPbkEng.rsg)
        TInt iTargetFieldTypeResId;
        /// Flags from Pbk2UsimUi.hrh
        TUint8 iFlags;
        /// See TPsu2ErrorCode
        TUint8 iBlockingError;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TPsu2CopyToSimFieldInfo::TPsu2CopyToSimFieldInfo
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TPsu2CopyToSimFieldInfo::TPsu2CopyToSimFieldInfo( 
        const MVPbkFieldType& aSourceType,
        const MVPbkFieldType& aSimType, 
        TUint8 aFlags, 
        TUint8 aBlockingError )
        :   iSourceType( aSourceType ),
            iSimType( aSimType ),
            iFlags( aFlags ),
            iBlockingError( aBlockingError )
    {
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldtypeMapping::NewLC
// -----------------------------------------------------------------------------
//
CPsu2CopyToSimFieldtypeMapping* CPsu2CopyToSimFieldtypeMapping::NewLC( 
        TResourceReader& aReader,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    CPsu2CopyToSimFieldtypeMapping* self = 
        new( ELeave ) CPsu2CopyToSimFieldtypeMapping;
    CleanupStack::PushL( self );
    self->ConstructL( aReader, aMasterFieldTypeList );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldtypeMapping::ConstructL
// -----------------------------------------------------------------------------
//
void CPsu2CopyToSimFieldtypeMapping::ConstructL( TResourceReader& aReader,
        const MVPbkFieldTypeList& aMasterFieldTypeList )
    {
    iSourceTypeSelector = CVPbkFieldTypeSelector::NewL( aReader, 
        aMasterFieldTypeList );
    iTargetFieldTypeResId = aReader.ReadInt32();
    iFlags = aReader.ReadUint8();
    iBlockingError = aReader.ReadUint8();
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldtypeMapping::~CPsu2CopyToSimFieldtypeMapping
// -----------------------------------------------------------------------------
//
CPsu2CopyToSimFieldtypeMapping::~CPsu2CopyToSimFieldtypeMapping()
    {
    delete iSourceTypeSelector;
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldtypeMapping::SourceTypeSelector
// -----------------------------------------------------------------------------
//
MVPbkFieldTypeSelector& CPsu2CopyToSimFieldtypeMapping::SourceTypeSelector() const
    {
    return *iSourceTypeSelector;
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldtypeMapping::TargetFieldtypeResId
// -----------------------------------------------------------------------------
//
TInt CPsu2CopyToSimFieldtypeMapping::TargetFieldtypeResId() const
    {
    return iTargetFieldTypeResId;
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldtypeMapping::Flags
// -----------------------------------------------------------------------------
//
TUint8 CPsu2CopyToSimFieldtypeMapping::Flags() const
    {
    return iFlags;
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldtypeMapping::BlockingError
// -----------------------------------------------------------------------------
//
TUint8 CPsu2CopyToSimFieldtypeMapping::BlockingError() const
    {
    return iBlockingError;
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::CPsu2CopyToSimFieldInfoArray
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsu2CopyToSimFieldInfoArray::CPsu2CopyToSimFieldInfoArray(
    const MVPbkFieldTypeList& aMasterFieldTypeList)
    :   iMasterFieldTypeList(aMasterFieldTypeList)
    {
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsu2CopyToSimFieldInfoArray::ConstructL( RFs& aFs )
    {
    // Open USIM extension resource file for reading field mappings.
    // This class can be used from SIM copy policy so it must not
    // depend on CCoeEnv in anyway.
    RPbk2LocalizedResourceFile resFile( &aFs );
    resFile.OpenLC(KPbk2RomFileDrive,
        KDC_RESOURCE_FILES_DIR, KPsu2USimExtensionResFile);
    FeatureManager::InitializeLibL();
    TResourceReader reader;
    if( !FeatureManager::FeatureSupported( 
                    KFeatureIdFfTdClmcontactreplicationfromphonebooktousimcard ) )
        {
        reader.SetBuffer( 
               resFile.AllocReadLC( R_PSU2_COPY_TO_SIM_FIELDTYPE_MAPPINGS ) );
        }
    else
        {
        reader.SetBuffer( 
               resFile.AllocReadLC( R_PSU2_COPY_TO_USIM_FIELDTYPE_MAPPINGS ) );
        }
    
    // Go through all mappings in resource and create a mapping array
    const TInt count = reader.ReadInt16();
    for ( TInt i = 0; i < count; ++i )
        {
        CPsu2CopyToSimFieldtypeMapping* cmapping = 
            CPsu2CopyToSimFieldtypeMapping::NewLC( reader, 
                iMasterFieldTypeList );
        CreateMappingsL( *cmapping );
        CleanupStack::PopAndDestroy( cmapping );
        }
    CleanupStack::PopAndDestroy(); // R_PSU2_COPY_TO_SIM_FIELDTYPE_MAPPINGS
    
    // Valid characters for number fields.
    // Note: StringLoader can not be used in this class because it depends
    // on CCoeEnv. This class can be run in non-UI context because
    // CPsu2ContactCopyPolicy uses this class.
    reader.SetBuffer( 
        resFile.AllocReadLC( R_PSU2_STANDARD_NUMBER_CHARACTER_MAP ) );
    iNumberKeyMap = reader.ReadHBufCL();
    // R_PSU2_STANDARD_NUMBER_CHARACTER_MAP, resFile
    CleanupStack::PopAndDestroy(2); 
    
    // Get SIM name field type. This must be the same type as the type
    // defined in VPbkSimStore
    iSimNameType = iMasterFieldTypeList.Find( R_VPBK_FIELD_TYPE_LASTNAME );
    iSimReadingType = 
        iMasterFieldTypeList.Find( R_VPBK_FIELD_TYPE_LASTNAMEREADING );
    
    __ASSERT_DEBUG( iSimNameType && iSimReadingType, 
        Panic( EPostCond_ConstructL ) );
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsu2CopyToSimFieldInfoArray* CPsu2CopyToSimFieldInfoArray::NewL(
        const MVPbkFieldTypeList& aMasterFieldTypeList, RFs& aFs )
    {
    CPsu2CopyToSimFieldInfoArray* self =
        new( ELeave ) CPsu2CopyToSimFieldInfoArray(aMasterFieldTypeList);
    CleanupStack::PushL( self );
    self->ConstructL( aFs );
    CleanupStack::Pop();
    return self;
    }


// Destructor
CPsu2CopyToSimFieldInfoArray::~CPsu2CopyToSimFieldInfoArray()
    {
    iInfoArray.Close();
    iMatchedTypes.Close();
    delete iNumberKeyMap;
    FeatureManager::UnInitializeLib();
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::FindInfoForSourceType
// -----------------------------------------------------------------------------
//
const TPsu2CopyToSimFieldInfo* 
        CPsu2CopyToSimFieldInfoArray::FindInfoForSourceType(
            const MVPbkFieldType& aSourceType )
    {
    const TPsu2CopyToSimFieldInfo* result = NULL;
    const TInt count = iInfoArray.Count();
    for (TInt i = 0; i < count && !result; ++i)
        {
        if (iInfoArray[i].SourceType().IsSame( aSourceType ) )
            {
            result = &iInfoArray[i];
            }
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::RemoveUnSupportedTargetTypeFieldInfo
// -----------------------------------------------------------------------------
//
void CPsu2CopyToSimFieldInfoArray::RemoveUnSupportedFieldInfo( const MVPbkFieldTypeList& aSupportedTypes )
    {
    // Check all the field info in the iInfoArray, if the target of the fieldinfo 
    // is not contained in the supportedTypes, Remove it from iInfoArray.
    TInt i = iInfoArray.Count() - 1;
    while( i >= 0 )
        {
        if( !aSupportedTypes.ContainsSame( iInfoArray[i].SimType()))
            {
            iInfoArray.Remove( i );
            }
        i --;
        }
    }
    
// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::SimNameType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType& CPsu2CopyToSimFieldInfoArray::SimNameType() const
    {
    return *iSimNameType;
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::LastNameReadingType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType& CPsu2CopyToSimFieldInfoArray::LastNameReadingType() const
    {
    return *iSimReadingType;
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::ConvertToSimType
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* CPsu2CopyToSimFieldInfoArray::ConvertToSimType(
    const MVPbkFieldType& aSourceType)
    {
    const TInt count = iInfoArray.Count();
    for (TInt i = 0; i < count; ++i)
        {
        if (iInfoArray[i].SourceType().IsSame(aSourceType))
            {
            return &iInfoArray[i].SimType();
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::TruncationAllowed
// -----------------------------------------------------------------------------
//
TBool CPsu2CopyToSimFieldInfoArray::TruncationAllowed(
        const MVPbkFieldType& aSimType )
    {
    const TInt count = iInfoArray.Count();
    for ( TInt i = 0; i < count; ++i )
        {
        if ( iInfoArray[i].SimType().IsSame( aSimType ) )
            {
            return iInfoArray[i].TruncationAllowed();
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::NumberKeyMap
// -----------------------------------------------------------------------------
//
const TDesC& CPsu2CopyToSimFieldInfoArray::NumberKeyMap() const
    {
    return *iNumberKeyMap;
    }

// -----------------------------------------------------------------------------
// CPsu2CopyToSimFieldInfoArray::CreateMappingsL
// -----------------------------------------------------------------------------
//
void CPsu2CopyToSimFieldInfoArray::CreateMappingsL( 
        CPsu2CopyToSimFieldtypeMapping& aCMapping )
    {
    const TInt typeCount = iMasterFieldTypeList.FieldTypeCount();
    // First find the target type
    const MVPbkFieldType* targetType = iMasterFieldTypeList.Find( 
        aCMapping.TargetFieldtypeResId() );
    
    // Then find all source types and create mappings
    MVPbkFieldTypeSelector& selector = aCMapping.SourceTypeSelector();
    for ( TInt i = 0; i < typeCount; ++i )
        {
        if ( selector.IsFieldTypeIncluded( 
                iMasterFieldTypeList.FieldTypeAt( i ) ) )
            {
            TPsu2CopyToSimFieldInfo info( 
                iMasterFieldTypeList.FieldTypeAt( i ),
                *targetType, aCMapping.Flags(), aCMapping.BlockingError() );
            iInfoArray.AppendL( info );
            }
        }
    
    // Target type must be found from master field type list.
    // If it doesn't, then either the field type list is not the
    // master field types list or the field type resource id is corrupted.
    __ASSERT_DEBUG( targetType, Panic( EPostCond_CreateMappingsL) );
    }
//  End of File
