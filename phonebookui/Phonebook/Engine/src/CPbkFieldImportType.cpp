/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Phonebook field import type.
*
*/


// INCLUDE FILES
#include "CPbkFieldImportType.h"
#include <barsread.h>
#include <cntdef.h>
#include "CPbkUidMap.h"
#include "CPbkVcardProperty.h"
#include "TPbkContactItemFieldType.h"
#include "TPbkVcardFieldType.h"
#include "TPbkMatchPriorityLevel.h"


// MODULE DATA STRUCTURES

/**
 * An entry in CPbkFieldImportType::iImportProperties array.
 */
struct CPbkFieldImportType::TImportProperty
    {
    MPbkVcardProperty* iProperty;
    TPbkContactItemFieldType iFieldType;
    TPbkVcardFieldType iVcardType;
    };


/// Unnamed namespace for local definitions
namespace {

// LOCAL FUNCTIONS

void UpdateUidMapL
        (CPbkUidMap& aUidMap, const MPbkVcardParameters& aParameters)
    {
    const TInt count = aParameters.ParameterCount();
    for (TInt i=0; i < count; ++i)
        {
        aUidMap.InsertL(aParameters.ParameterAt(i));
        }
    }

inline void UpdateUidMapL
        (CPbkUidMap& aUidMap, const MPbkVcardProperty& aProperty)
    {
    aUidMap.InsertL(aProperty.PropertyName());
    UpdateUidMapL(aUidMap,aProperty.PropertyParameters());
    }
}


// ==================== MEMBER FUNCTIONS ====================

inline CPbkFieldImportType::CPbkFieldImportType():
    iImportProperties(1) // 1 granularity used instead of default 8
    {
    // CBase::operator(TLeave) resets members
    }

inline void CPbkFieldImportType::ConstructL(TResourceReader& aResReader)
    {
    TInt count = aResReader.ReadInt16();
	while (count-- > 0)
        {
        TImportProperty importProp;
        importProp.iProperty = CPbkVcardProperty::NewLC(aResReader);
        User::LeaveIfError(iImportProperties.Append(importProp));
        CleanupStack::Pop();  // importProp.iProperty
        }
    iExcludeParameters.ConstructL(aResReader);
    }

CPbkFieldImportType* CPbkFieldImportType::NewL(TResourceReader& aResReader)
    {
    CPbkFieldImportType* self = new(ELeave) CPbkFieldImportType;
    CleanupStack::PushL(self);
    self->ConstructL(aResReader);
    CleanupStack::Pop(self);
    return self;
    }

CPbkFieldImportType::~CPbkFieldImportType()
    {
    for (TInt i=iImportProperties.Count()-1; i >= 0; --i)
        {
        delete iImportProperties[i].iProperty;
        }
    iImportProperties.Close();
    }

inline TBool CPbkFieldImportType::ContainsExcludeTypes
        (const TPbkFieldTypeSignature& aSignature) const
    {
    return !(Intersection(aSignature,iExcludeSignature).IsEmpty());
    }

TBool CPbkFieldImportType::Match
        (const TPbkContactItemFieldType& aFieldType) const
    {
    if (ContainsExcludeTypes(aFieldType.Signature()))
        {
        return EFalse;
        }

    if (iContactItemExportType.Match(aFieldType))
        {
        return ETrue;
        }

    const TInt count = iImportProperties.Count();
    for (TInt i=0; i < count; ++i)
        {
        if (iImportProperties[i].iFieldType.Match(aFieldType))
            {
            return ETrue;
            }
        }
    return EFalse;
    }

TInt CPbkFieldImportType::Match
        (const TPbkContactItemFieldType& aFieldType,
        const TPbkMatchPriorityLevel& aMatchPriority) const
    {
    if (ContainsExcludeTypes(aFieldType.Signature()))
        {
        return EFalse;
        }

    const TInt matchPriority = aMatchPriority.CurrentLevel();
    if (matchPriority==0 && iContactItemExportType.Match(aFieldType))
        {
        return ETrue;
        }

    if (matchPriority < iImportProperties.Count())
        {
        return iImportProperties[matchPriority].iFieldType.Match(aFieldType);
        }
    else
        {
        return EFalse;
        }
    }

TInt CPbkFieldImportType::Match
        (const TPbkVcardFieldType& aVcardType,
        const TPbkMatchPriorityLevel& aMatchPriority) const
    {
    if (ContainsExcludeTypes(aVcardType.Signature()))
        {
        return EFalse;
        }

    const TInt matchPriority = aMatchPriority.CurrentLevel();
    if (matchPriority < iImportProperties.Count())
        {
        return iImportProperties[matchPriority].iVcardType.Match(aVcardType);
        }
    else
        {
        return EFalse;
        }
    }

TInt CPbkFieldImportType::ImportPropertyCount() const
    {
    return iImportProperties.Count();
    }

void CPbkFieldImportType::UpdateTypeUidMapL(CPbkUidMap& aTypeUidMap) const
    {
    const TInt count = iImportProperties.Count();
    for (TInt i=0; i < count; ++i)
        {
        UpdateUidMapL(aTypeUidMap, *iImportProperties[i].iProperty);
        }
    UpdateUidMapL(aTypeUidMap, iExcludeParameters);
    }

void CPbkFieldImportType::CalculateSignatures
        (const CPbkFieldInfo& aFieldInfo, const CPbkUidMap& aUidMap)
    {
    iContactItemExportType =
        TPbkContactItemFieldType(aFieldInfo, aUidMap);
    TPbkFieldTypeSignature newExcludeSignature;
    newExcludeSignature.AddParameters(iExcludeParameters, aUidMap);
    iExcludeSignature = newExcludeSignature;
    const TInt count = iImportProperties.Count();
    for (TInt i=0; i < count; ++i)
        {
        TImportProperty& importProp = iImportProperties[i];
        // Field import type
        importProp.iFieldType = TPbkContactItemFieldType(
                aFieldInfo, *importProp.iProperty, aUidMap);
        // vCard import type
        importProp.iVcardType = 
            TPbkVcardFieldType(*importProp.iProperty, aUidMap);
        }
    }


// End of File
