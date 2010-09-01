/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Virtual Phonebook field type mapper.
*
*/


// INCLUDES
#include "CFieldTypeMap.h"
#include <TVPbkFieldVersitProperty.h>
#include <MVPbkFieldType.h>
#include <VPbkUtil.h>

#include <barsread.h>
#include <cntdef.h>
#include <cntfield.h>

namespace VPbkCntModel {

/**
 * Contact Model field type and vCard mapping to
 * Virtual Phonebook versit field type mapping data.
 */
NONSHARABLE_CLASS( CFieldTypeMap::TVersitNameMapData )
    {
    public:
        /// Contact Model type id
        TInt iCntModelId;
        /// contact model vCard mapping
        TInt iCntModelVCardId;
        /// Virtual Phonebook Versit property
        TVPbkFieldVersitProperty iVersitProperty;

        /**
         * Constructs this object from a resource.
         */
        TVersitNameMapData(TResourceReader& aResReader);

        /**
         * Collects type data from this mapping object to Virtual Phonebook
         * Versit field type.
         */
        void CollectTypeData
            (TVPbkFieldVersitProperty& aVersitType) const;
    };

/**
 * Contact Model vCard parameter to Virtual Phonebook versit field type mapping data.
 */
NONSHARABLE_CLASS( CFieldTypeMap::TVersitParameterMapData )
    {
    public:
        /// Contact Model VCard mapping id
        TInt iCntModelId;
        /// Virtual Phonebook Versit parameters
        TVPbkFieldTypeParameters iVersitParameters;

        /**
         * Constructs this object from a resource.
         */
        TVersitParameterMapData(TResourceReader& aResReader);

        /**
         * Collects type data from this mapping object to Virtual Phonebook
         * Versit field type.
         */
        void CollectTypeData
            (TVPbkFieldVersitProperty& aVersitType) const;
    };


/**
 * Contact Model field type -> Virtual Phonebook non-versit field type mapping data.
 */
NONSHARABLE_CLASS( CFieldTypeMap::TNonVersitMapData )
    {
    public:
        /// Contact Model type id
        TInt iCntModelId;
        /// Virtual Phonebook Non-Versit type
        TVPbkNonVersitFieldType iNonVersitType;

        /**
         * Constructs this object from a resource.
         */
        TNonVersitMapData(TResourceReader& aResReader);
    };


// TVersitNameMapData implementation
CFieldTypeMap::TVersitNameMapData::TVersitNameMapData(
        TResourceReader& aResReader) :
    iCntModelId(aResReader.ReadInt32()),
    iCntModelVCardId(aResReader.ReadInt32()),
    iVersitProperty(aResReader)
    {
    }

void CFieldTypeMap::TVersitNameMapData::CollectTypeData
        (TVPbkFieldVersitProperty& aVersitType) const
    {
    if (aVersitType.Name() == EVPbkVersitNameNone)
        {
        aVersitType.SetName(iVersitProperty.Name());
        }
    if (aVersitType.SubField() == EVPbkVersitSubFieldNone)
        {
        aVersitType.SetSubField(iVersitProperty.SubField());
        }
    aVersitType.Parameters().AddAll(iVersitProperty.Parameters());
    if  (aVersitType.Name() == EVPbkVersitNameX &&
            aVersitType.ExtensionName().Length() == 0)
        {
        aVersitType.SetExtensionName(iVersitProperty.ExtensionName());
        }
    }

// TVersitParameterMapData implementation
CFieldTypeMap::TVersitParameterMapData::TVersitParameterMapData(
        TResourceReader& aResReader) :
    iCntModelId(aResReader.ReadInt32()),
    iVersitParameters(aResReader)
    {
    }

void CFieldTypeMap::TVersitParameterMapData::CollectTypeData
        (TVPbkFieldVersitProperty& aVersitType) const
    {
    aVersitType.Parameters().AddAll(iVersitParameters);
    }

// CFieldTypeMap::TNonVersitMapData implementation
CFieldTypeMap::TNonVersitMapData::TNonVersitMapData
        (TResourceReader& aResReader) :
    iCntModelId(aResReader.ReadInt32()),
    iNonVersitType(static_cast<TVPbkNonVersitFieldType>(aResReader.ReadUint8()))
    {
    }


// Local helper functions

/**
 * Initializes a versit name mapping data array from resources.
 */
inline void InitVersitNamePropertiesFromResourceL
        (RArray<CFieldTypeMap::TVersitNameMapData>& aVersitNameMapDataArray,
        TResourceReader& aResReader,
        VPbkEngUtils::CTextStore& aTextStore)
    {
    TInt count = aResReader.ReadInt16();
    while (count-- > 0)
        {
        User::LeaveIfError(aVersitNameMapDataArray.Append(
                CFieldTypeMap::TVersitNameMapData(aResReader)));

        // set the versit extension name to the text store
        TVPbkFieldVersitProperty& property =
                aVersitNameMapDataArray[aVersitNameMapDataArray.Count()-1].iVersitProperty;
        if (property.ExtensionName().Length() > 0)
            {
            // Store the extension name to the text store and set the property
            // pointer to the stored version
            property.SetExtensionName(aTextStore.AddL(property.ExtensionName()));
            }
        }
    }

/**
 * Initializes a versit parameter mapping data array from resources.
 */
inline void InitVersitParameterPropertiesFromResourceL(
        RArray<CFieldTypeMap::TVersitParameterMapData>& aVersitParameterMapDataArray,
        TResourceReader& aResReader)
    {
    TInt count = aResReader.ReadInt16();
    while (count-- > 0)
        {
        User::LeaveIfError(aVersitParameterMapDataArray.Append(
                CFieldTypeMap::TVersitParameterMapData(aResReader)) );
        }
    }

/**
 * Initializes a non versit mapping data array from resources.
 */
inline void InitNonVersitPropertiesFromResourceL(
        RArray<CFieldTypeMap::TNonVersitMapData>& aNonVersitMapDataArray,
        TResourceReader& aResReader)
    {
    TInt count = aResReader.ReadInt16();
    while (count-- > 0)
        {
        User::LeaveIfError(
            aNonVersitMapDataArray.Append(
                CFieldTypeMap::TNonVersitMapData( aResReader ) ) );
        }
    }

/**
 * Searches the mapping data array by Contact Model id.
 *
 * @param aMapData      array of mapping data to search.
 * @param aCntModelId   Contact Model id to search for.
 * @return matching mapping data or NULL if no match found.
 */
template<typename TFieldTypeMapData>
const TFieldTypeMapData* FindByCntModelId(
        const RArray<TFieldTypeMapData>& aMapData,
        TInt aCntModelId)
    {
    const TInt count = aMapData.Count();
    for (TInt i = 0; i < count; ++i)
        {
        const TFieldTypeMapData& mapData = aMapData[i];
        if (mapData.iCntModelId == aCntModelId)
            {
            return &mapData;
            }
        }
    return NULL;
    }

/**
 * Searches the versit name mapping array with the contact model id
 * and the vcard mapping.
 *
 * @param aMapData      array of mapping data to search.
 * @param aCntModelId   Contact Model id to search for.
 * @param aVCardMappingId   Contact Model versit parameter id to search for.
 * @return matching mapping data or NULL if no match found.
 */
template<typename TFieldTypeMapData>
const TFieldTypeMapData* FindByCntModelIdAndVCardId(
        const RArray<TFieldTypeMapData>& aMapData,
        TInt aCntModelId,
        TInt aVCardMappingId)
    {
    const TInt count = aMapData.Count();
    for(TInt i = 0; i < count; ++i)
        {
        const TFieldTypeMapData& mapDataElement = aMapData[i];
        if (mapDataElement.iCntModelId == aCntModelId &&
            mapDataElement.iCntModelVCardId == aVCardMappingId)
            {
            return &mapDataElement;
            }
        }
    return NULL;
    }

/**
 * Helper wrapper for CFieldTypeMap::TVersitMapData::CollectTypeData
 */
template<typename TVersitMapData>
inline void CollectVersitTypeData
        (const TVersitMapData* aMapData,
        TVPbkFieldVersitProperty& aVersitType)
    {
    if (aMapData)
        {
        aMapData->CollectTypeData(aVersitType);
        }
    }

////////////////////////////////////////////////////////////////////////////
// CFieldTypeMap implementation

CFieldTypeMap::CFieldTypeMap(const MVPbkFieldTypeList& aMasterFieldTypeList,
        VPbkEngUtils::CTextStore& aTextStore) :
    iMasterFieldTypeList(aMasterFieldTypeList),
    iTextStore(aTextStore)
    {
    }

CFieldTypeMap* CFieldTypeMap::NewL(const MVPbkFieldTypeList& aMasterFieldTypeList,
        VPbkEngUtils::CTextStore& aTextStore)
    {
    return new(ELeave) CFieldTypeMap(aMasterFieldTypeList, aTextStore);
    }

void CFieldTypeMap::InitVersitNameMappingsL(TResourceReader& aResReader)
    {
    // fills iVersitMapData with the aResReader data
    InitVersitNamePropertiesFromResourceL(iVersitNameMapData, aResReader, iTextStore);
    }

void CFieldTypeMap::InitVersitParameterMappingsL(TResourceReader& aResReader)
    {
    // fills iVersitMapData with the aResReader data
    InitVersitParameterPropertiesFromResourceL(iVersitParameterMapData, aResReader);
    }

void CFieldTypeMap::InitNonVersitMappingsL(TResourceReader& aResReader)
    {
    // fills iNonVersitMapData with the aResReader data
    InitNonVersitPropertiesFromResourceL(iNonVersitMapData, aResReader);
    }

CFieldTypeMap::~CFieldTypeMap()
    {
    iNonVersitMapData.Close();
    iVersitParameterMapData.Close();
    iVersitNameMapData.Close();
    }

const MVPbkFieldType* CFieldTypeMap::GenericFieldType
        (const CContactItemField& aField,
        TInt aMatchPriority) const
    {
    const MVPbkFieldType* result = NULL;

    // maps the contact model aField to the VPbk field type
    const TVPbkFieldVersitProperty versitType(VersitType(aField.ContentType()));
    if (versitType.Name() != EVPbkVersitNameNone)
        {
        result = iMasterFieldTypeList.FindMatch(versitType, aMatchPriority);
        }
    if (!result)
        {
        const TVPbkNonVersitFieldType nonVersitType(NonVersitType(aField.ContentType()));
        if (nonVersitType != EVPbkNonVersitTypeNone)
            {
            result = iMasterFieldTypeList.FindMatch(nonVersitType);
            }
        }
    return result;
    }

const MVPbkFieldType* CFieldTypeMap::FindByCntModelFieldType
        (TFieldType aFieldType,
        TInt aMatchPriority) const
    {
    const MVPbkFieldType* result = NULL;

    // maps the contacts model field type uid to a VPbk field type
    TVPbkFieldVersitProperty versitType;
    CollectVersitTypeData(FindByCntModelId(iVersitNameMapData, aFieldType.iUid),
            versitType);

    result = iMasterFieldTypeList.FindMatch(versitType, aMatchPriority);

    if (!result)
        {
        const CFieldTypeMap::TNonVersitMapData* mapData = FindByCntModelId(
                iNonVersitMapData, aFieldType.iUid);
        if (mapData)
            {
            const TVPbkNonVersitFieldType nonVersitType(mapData->iNonVersitType);
            if (nonVersitType != EVPbkNonVersitTypeNone)
                {
                result = iMasterFieldTypeList.FindMatch(nonVersitType);
                }
            }
        }

    return result;
    }

TVPbkFieldVersitProperty CFieldTypeMap::VersitType
        (const CContentType& aContentType) const
    {
    TVPbkFieldVersitProperty result;
    const TInt typeCount = aContentType.FieldTypeCount();

    // first get the cntmodel field type id and the fields vCard mapping
    // map this to a certain VERSIT_FIELDNAME_MAPPING entry
    // then map the rest of the vCard mappings
    // this should then produce the correct TVPbkFieldVersitProperty object
    if (typeCount > 0)
        {
        // find and collect versit data with the contact model type and the vcard mapping
        CollectVersitTypeData(
            FindByCntModelIdAndVCardId(iVersitNameMapData, aContentType.FieldType(0).iUid,
                aContentType.Mapping().iUid),
            result);
        }

    // add extra vcard mappings in the field
    for(TInt i = 1; i < typeCount; ++i)
        {
        CollectVersitTypeData(
            FindByCntModelId(iVersitParameterMapData, aContentType.FieldType(i).iUid),
            result);
        }

    return result;
    }

TVPbkNonVersitFieldType CFieldTypeMap::NonVersitType
        (const CContentType& aContentType) const
    {
    TVPbkNonVersitFieldType result = EVPbkNonVersitTypeNone;
    // get the contact model field type uid and match it to a non versit property
    if (aContentType.FieldTypeCount() > 0)
        {
        // Match only with main field type
        const CFieldTypeMap::TNonVersitMapData* mapData = FindByCntModelId
            (iNonVersitMapData, aContentType.FieldType(0).iUid);
        if (mapData)
            {
            result = mapData->iNonVersitType;
            }
        }
    return result;
    }

TInt CFieldTypeMap::MaxMatchPriority() const
    {
    return iMasterFieldTypeList.MaxMatchPriority();
    }

}  // namespace VPbkCntModel

