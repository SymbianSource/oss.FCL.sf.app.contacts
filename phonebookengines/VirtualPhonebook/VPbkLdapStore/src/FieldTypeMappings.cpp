/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CFieldTypeMappings implementation
*
*/


// INCLUDE FILES
#include "fieldtypemappings.h"
#include <barsc.h>
#include <barsread.h>
#include <tvpbkfieldversitproperty.h>
#include <mvpbkfieldtype.h>

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {
// -----------------------------------------------------------------------------
//                      TVersitMapData class
// -----------------------------------------------------------------------------
class CFieldTypeMappings::TVersitMapData
    {
    public:
        /// Contact model id
        TInt iCntModelId;

        /// Virtual Phonebook Versit property
        TVPbkFieldVersitProperty iVersitProperty;

        /**
        @function   TVersitMapData
        @discussion TVersitMapData contructor
        @param      aResReader Resource reader
        */
        TVersitMapData(TResourceReader& aResReader);
    };
// -----------------------------------------------------------------------------
// CFieldTypeMappings::TVersitMapData::TVersitMapData
// TVersitMapData C++ default constructor
// -----------------------------------------------------------------------------
CFieldTypeMappings::TVersitMapData::TVersitMapData(TResourceReader& aReader)
:   iCntModelId(aReader.ReadInt32()), iVersitProperty(aReader)
    {
    }

// -----------------------------------------------------------------------------
//                      TNonVersitMapData class
// -----------------------------------------------------------------------------
class CFieldTypeMappings::TNonVersitMapData
    {
    public:  
        /// Contact model id
        TInt iCntModelId;

        /// Virtual Phonebook Non-Versit type
        TVPbkNonVersitFieldType iNonVersitType;

        /**
        @function   TNonVersitMapData
        @discussion TNonVersitMapData contructor
        @param      aResReader Resource reader
        */
        TNonVersitMapData(TResourceReader& aResReader);
    };
// -----------------------------------------------------------------------------
// CFieldTypeMappings::TNonVersitMapData::TNonVersitMapData
// TNonVersitMapData C++ default constructor
// -----------------------------------------------------------------------------
CFieldTypeMappings::TNonVersitMapData::TNonVersitMapData(TResourceReader& aReader)
:   iCntModelId(aReader.ReadInt32()),
    iNonVersitType(static_cast<TVPbkNonVersitFieldType>(aReader.ReadUint8()))
    {
    }

// -----------------------------------------------------------------------------
//                      CFieldTypeMappings class
// -----------------------------------------------------------------------------
// CFieldTypeMappings::CFieldTypeMappings
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFieldTypeMappings::CFieldTypeMappings(
const MVPbkFieldTypeList& aMasterFieldTypeList)
:    iMasterFieldTypeList(aMasterFieldTypeList)
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CFieldTypeMappings::ConstructL
// CFieldTypeMappings constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CFieldTypeMappings::ConstructL()
    {
    // No implementation at the moment
    }
// -----------------------------------------------------------------------------
// CFieldTypeMappings::NewLC
// CFieldTypeMappings two-phased constructor.
// -----------------------------------------------------------------------------
//
CFieldTypeMappings* CFieldTypeMappings::NewLC
(
    const MVPbkFieldTypeList& aMasterFieldTypeList
)
    {
    CFieldTypeMappings* self = new (ELeave)CFieldTypeMappings(aMasterFieldTypeList);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
// -----------------------------------------------------------------------------
// CFieldTypeMappings::NewL
// CFieldTypeMappings two-phased constructor.
// -----------------------------------------------------------------------------
//
CFieldTypeMappings* CFieldTypeMappings::NewL
(
    const MVPbkFieldTypeList& aMasterFieldTypeList
)
    {
    CFieldTypeMappings* self = CFieldTypeMappings::NewLC(aMasterFieldTypeList);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CFieldTypeMappings::~CFieldTypeMappings
// CFieldTypeMappings Destructor
// -----------------------------------------------------------------------------
//
CFieldTypeMappings::~CFieldTypeMappings()
    {
    iVersitMapData.Close();
    iNonVersitMapData.Close();
    }

// -----------------------------------------------------------------------------
// CFieldTypeMappings::InitVersitMappingsL
// -----------------------------------------------------------------------------
//
void CFieldTypeMappings::InitVersitMappingsL(TResourceReader& aReader)
    {
    TInt count = aReader.ReadInt16();
    while (count-- > 0)
        User::LeaveIfError(iVersitMapData.Append(TVersitMapData(aReader)));
    }

// -----------------------------------------------------------------------------
// CFieldTypeMappings::InitNonVersitMappingsL
// -----------------------------------------------------------------------------
//
void CFieldTypeMappings::InitNonVersitMappingsL(TResourceReader& aReader)
    {
    TInt count = aReader.ReadInt16();
    while (count-- > 0)
        User::LeaveIfError(iNonVersitMapData.Append(TNonVersitMapData(aReader)));
    }

// -----------------------------------------------------------------------------
//                  Like MVPbkFieldTypeList implementation
// -----------------------------------------------------------------------------
// CFieldTypeMappings::FieldTypeCount
// -----------------------------------------------------------------------------
//
TInt CFieldTypeMappings::FieldTypeCount() const
    {
    return iVersitMapData.Count();
    }

// -----------------------------------------------------------------------------
// CFieldTypeMappings::Match
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* CFieldTypeMappings::Match(TInt aFieldType) const
    {
    // Master list count
    TInt master_count = iMasterFieldTypeList.FieldTypeCount();

    // Resource data count
    TInt count = iVersitMapData.Count();
    TInt loop = 0;

    // Find the mapping object for the given type
    for (loop = 0;loop < count;loop++)
        {
        // Contact model type id
        if (iVersitMapData[loop].iCntModelId == aFieldType)
            {
            TInt max_priority = iMasterFieldTypeList.MaxMatchPriority();
            // Find type from master list
            for (TInt next = 0;next < master_count;next++)
                {
                for (TInt priority = 0;priority <= max_priority;priority++ )
                    {
                    // Field at
                    const MVPbkFieldType& fp = iMasterFieldTypeList.FieldTypeAt(next);
                    if (fp.Matches(iVersitMapData[loop].iVersitProperty,priority))
                        return &fp;
                    }
                }
            }
        }

    // Find from non-versit type list
    count = iNonVersitMapData.Count();
    for (loop = 0;loop < count;loop++)
        {
        // Contact model type id
        if (iNonVersitMapData[loop].iCntModelId == aFieldType)
            {
            // Find type from master list
            for (TInt next = 0;next < master_count;next++)
                {
                // Field at
                const MVPbkFieldType& fp = iMasterFieldTypeList.FieldTypeAt(next);
                if (fp.NonVersitType() == iNonVersitMapData[loop].iNonVersitType)
                    return &fp;
                }
            }
        }
    return NULL;
    }
} // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
