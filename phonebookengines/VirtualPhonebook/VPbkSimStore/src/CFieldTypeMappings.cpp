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
* Description:  A class to map Virtual Phonebook and SIM
*                phonebook field types.
*
*/


// INCLUDE FILES
#include "CFieldTypeMappings.h"

// Virtual Phonebook
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkFieldType.h>
#include <VPbkUtil.h>

// System includes
#include <barsread.h>


namespace VPbkSimStore {

// ============================= LOCAL FUNCTIONS ============================

/**
 * Versit data mapper.
 */
NONSHARABLE_CLASS( TVersitMapData )
    {
    public: // Construction
        /**
         * Constructs this object from a resource.
         *
         * @param aResReader    Resource reader.
         */
        TVersitMapData(
                TResourceReader& aResReader );

    public: // Data
        /// Own: The SIM field type id
        TVPbkSimCntFieldType iSimFieldId;
        /// Own: Virtual Phonebook Versit property
        TVPbkFieldVersitProperty iVersitProperty;

    };


/**
 * Non-Versit data mapper.
 */
NONSHARABLE_CLASS( TNonVersitMapData )
    {
    public: // Construction
        /**
         * Constructs this object from a resource.
         *
         * @param aResReader    Resource reader.
         */
        TNonVersitMapData(
                TResourceReader& aResReader );

    public: // Data
        /// Own: The SIM field type id
        TVPbkSimCntFieldType iSimFieldId;
        /// Own: Virtual Phonebook Non-Versit type
        TVPbkNonVersitFieldType iNonVersitType;

    };

// --------------------------------------------------------------------------
// TVersitMapData::TVersitMapData
// --------------------------------------------------------------------------
//
TVersitMapData::TVersitMapData
        ( TResourceReader& aResReader ):
        iSimFieldId( static_cast<TVPbkSimCntFieldType>
            ( aResReader.ReadInt32() ) ),
        iVersitProperty( aResReader )
    {
    }

// --------------------------------------------------------------------------
// TNonVersitMapData::TNonVersitMapData
// --------------------------------------------------------------------------
//
TNonVersitMapData::TNonVersitMapData
        ( TResourceReader& aResReader ):
        iSimFieldId( static_cast<TVPbkSimCntFieldType>
            ( aResReader.ReadInt32() ) ),
        iNonVersitType
            ( static_cast<TVPbkNonVersitFieldType>( aResReader.ReadUint8() ) )
    {
    }

// --------------------------------------------------------------------------
// InitFromResourceL
// A function to read versit mapping data to array.
// Initializes a mapping data array from resources.
// --------------------------------------------------------------------------
//
void InitFromResourceL( RArray<TVersitMapData>& aMapDataArray, 
        TResourceReader& aResReader, VPbkEngUtils::CTextStore& aTextStore )
    {
    TInt count = aResReader.ReadInt16();
    while ( count-- > 0 )
        {
        TVersitMapData mapData( aResReader );
        
        // TVPbkFieldVersitProperty has only a reference to the extension
        // name descriptor. The name must be saved permanently to the
        // CTextStore and then set the reference to the iVersitProperty.
        if (mapData.iVersitProperty.ExtensionName().Length() > 0)
            {
            // Store the extension name to the text store and set the property
            // pointer to the stored version
            mapData.iVersitProperty.SetExtensionName( 
                aTextStore.AddL( mapData.iVersitProperty.ExtensionName() ) );
            }
        
        User::LeaveIfError( 
            aMapDataArray.Append( mapData ) );
        }
    }

// --------------------------------------------------------------------------
// InitFromResourceL
// A function to read non versit mapping data to array.
// Initializes a mapping data array from resources.
// --------------------------------------------------------------------------
//
void InitFromResourceL( RArray<TNonVersitMapData>& aMapDataArray, 
        TResourceReader& aResReader )
    {
    TInt count = aResReader.ReadInt16();
    while ( count-- > 0 )
        {
        User::LeaveIfError( aMapDataArray.Append( 
            TNonVersitMapData( aResReader ) ) );
        }
    }

// ============================ MEMBER FUNCTIONS ============================

// --------------------------------------------------------------------------
// CFieldTypeMappings::CFieldTypeMappings
// C++ default constructor can NOT contain any code, that
// might leave.
// --------------------------------------------------------------------------
//
CFieldTypeMappings::CFieldTypeMappings( 
        const MVPbkFieldTypeList& aMasterFieldTypeList,
        VPbkEngUtils::CTextStore& aTextStore )
        :   iMasterFieldTypeList( aMasterFieldTypeList ),
            iTextStore( aTextStore )
    {
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::~CFieldTypeMappings
// --------------------------------------------------------------------------
//
CFieldTypeMappings::~CFieldTypeMappings()
    {
    iVersitMapData.Close();
    iNonVersitMapData.Close();
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::NewL
// Two-phased constructor.
// --------------------------------------------------------------------------
//
CFieldTypeMappings* CFieldTypeMappings::NewL( 
    const MVPbkFieldTypeList& aMasterFieldTypeList,
    VPbkEngUtils::CTextStore& aTextStore )
    {
    CFieldTypeMappings* self = 
        new( ELeave ) CFieldTypeMappings( aMasterFieldTypeList, aTextStore );
    return self;
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::InitVersitMappingsL
// --------------------------------------------------------------------------
//
void CFieldTypeMappings::InitVersitMappingsL( TResourceReader& aResReader )
    {
    InitFromResourceL( iVersitMapData, aResReader, iTextStore );
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::InitNonVersitMappingsL
// --------------------------------------------------------------------------
//
void CFieldTypeMappings::InitNonVersitMappingsL( TResourceReader& aResReader )
    {
    InitFromResourceL( iNonVersitMapData, aResReader );
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::Match
// --------------------------------------------------------------------------
//
TVPbkSimCntFieldType CFieldTypeMappings::Match( 
    const MVPbkFieldType& aVPbkFieldType ) const
    {
    TArray<TVPbkFieldVersitProperty> properties = 
        aVPbkFieldType.VersitProperties();
    
    TInt mapCount = iVersitMapData.Count();
    TInt propCount = properties.Count();
    TInt i = 0;
    for ( ;i < mapCount; ++i )
        {
        for ( TInt j = 0; j < propCount; ++j )
            {
            if ( properties[j].Matches( iVersitMapData[i].iVersitProperty ) )
                {
                return iVersitMapData[i].iSimFieldId;
                }
            }
        }
    
    TVPbkNonVersitFieldType nonVersitType = aVPbkFieldType.NonVersitType();
    mapCount = iNonVersitMapData.Count();
    for ( i = 0; i < mapCount; ++i )
        {
        if ( iNonVersitMapData[i].iNonVersitType == nonVersitType )
            {
            return iNonVersitMapData[i].iSimFieldId;
            }
        }
    return EVPbkSimUnknownType;
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::Match
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CFieldTypeMappings::Match( 
        const MVPbkFieldTypeList& aFieldTypeList, 
        TVPbkSimCntFieldType aSimFieldType ) const
    {
    TInt typeListCount = aFieldTypeList.FieldTypeCount();
    TInt maxPriority = aFieldTypeList.MaxMatchPriority();
    TInt mapCount = iVersitMapData.Count();
    TInt i = 0;
    // First loop tries to find the mapping object for the given sim type
    for ( ; i < mapCount; ++i )
        {
        if ( iVersitMapData[i].iSimFieldId == aSimFieldType )
            {
            // The mapping was found -> now the corresponding vpbk type
            // is tried to find from the given list
            for ( TInt j = 0; j < typeListCount; ++j )
                {
                for ( TInt k = 0; k <= maxPriority; ++k )
                    {
                    const MVPbkFieldType& vpbkType = 
                        aFieldTypeList.FieldTypeAt( j );
                    if ( vpbkType.Matches( iVersitMapData[i].iVersitProperty,
                         k ) )
                        {
                        return &vpbkType;
                        }
                    }
                }
            }
        }

    // There was no mapping object in the versit list ->
    // Find from non-versit type list
    mapCount = iNonVersitMapData.Count();
    for ( i = 0; i < mapCount; ++i )
        {
        if ( iNonVersitMapData[i].iSimFieldId == aSimFieldType )
            {
            for ( TInt j = 0; j < typeListCount; ++j )
                {
                const MVPbkFieldType& vpbkType = 
                        aFieldTypeList.FieldTypeAt( j );
                if ( vpbkType.NonVersitType() == 
                     iNonVersitMapData[i].iNonVersitType )
                    {
                    return &vpbkType;
                    }
                }
            }
        }
    return NULL;
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::Match
// --------------------------------------------------------------------------
//
const MVPbkFieldType* CFieldTypeMappings::Match( 
        TVPbkSimCntFieldType aSimFieldType ) const
    {
    return Match( iMasterFieldTypeList, aSimFieldType );
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::Match
// --------------------------------------------------------------------------
//
TVPbkSimCntFieldType CFieldTypeMappings::Match(
        const TVPbkFieldVersitProperty& aVersitProperty ) const
    {
    TVPbkSimCntFieldType ret = EVPbkSimUnknownType;

    TInt mapCount = iVersitMapData.Count();
    for ( TInt i=0; i < mapCount; ++i )
        {
        if ( aVersitProperty.Matches( iVersitMapData[i].iVersitProperty ) )
            {
            ret = iVersitMapData[i].iSimFieldId;
            break;
            }
        }

    return ret;
    }

// --------------------------------------------------------------------------
// CFieldTypeMappings::Match
// --------------------------------------------------------------------------
//
TVPbkSimCntFieldType CFieldTypeMappings::Match( 
        TVPbkNonVersitFieldType aNonVersitType ) const
    {
    TVPbkSimCntFieldType ret = EVPbkSimUnknownType;

    TInt mapCount = iNonVersitMapData.Count();
    for ( TInt i = 0; i < mapCount; ++i )
        {
        if ( iNonVersitMapData[i].iNonVersitType == aNonVersitType )
            {
            ret = iNonVersitMapData[i].iSimFieldId;
            break;
            }
        }

    return ret;
    }

} // namespace VPbkSimStore

//  End of File
