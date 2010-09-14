/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Field group properties
*
*/


#include <CPbk2FieldGroupPropertyArray.h>

// Phonebook 2
#include "Pbk2PresentationUtils.h"
#include <MPbk2FieldPropertyArray.h>
#include <RPbk2LocalizedResourceFile.h>
#include <pbk2presentation.rsg>
#include <MPbk2FieldProperty.h>
#include <CPbk2FieldPropertyGroup.h>
#include <Pbk2DataCaging.hrh>

// Virtual Phonebook
#include <MVPbkFieldType.h>
#include <TVPbkFieldVersitProperty.h>
#include <CVPbkFieldTypeSelector.h>

// System includes
#include <barsc.h>
#include <barsread.h>
#include <coemain.h>

// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::CPbk2FieldGroupPropertyArra
// ---------------------------------------------------------------------------
//
CPbk2FieldGroupPropertyArray::CPbk2FieldGroupPropertyArray()
    {
    }

// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::ConstructL
// ---------------------------------------------------------------------------
//
void CPbk2FieldGroupPropertyArray::ConstructL( 
        const MPbk2FieldPropertyArray& aFieldProperties )
    {
    RPbk2LocalizedResourceFile resFile( *CCoeEnv::Static() );
    resFile.OpenLC(KPbk2RomFileDrive, 
                   KDC_RESOURCE_FILES_DIR,
                   Pbk2PresentationUtils::PresentationResourceFile());
    
    // Read field group properties
    TResourceReader reader;
    CCoeEnv::Static()->CreateResourceReaderLC( reader, 
        R_PHONEBOOK2_FIELD_GROUP_ARRAY );
    ReadFieldGroupsL( reader );
    CleanupStack::PopAndDestroy(2); // reader, resFile
    
    PopulateGroupsL( aFieldProperties );
    }


// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2FieldGroupPropertyArray* CPbk2FieldGroupPropertyArray::NewL(
        const MPbk2FieldPropertyArray& aFieldProperties )
    {
    CPbk2FieldGroupPropertyArray* self = 
        CPbk2FieldGroupPropertyArray::NewLC( aFieldProperties );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CPbk2FieldGroupPropertyArray* CPbk2FieldGroupPropertyArray::NewLC(
        const MPbk2FieldPropertyArray& aFieldProperties )
    {
    CPbk2FieldGroupPropertyArray* self = new( ELeave ) CPbk2FieldGroupPropertyArray;
    CleanupStack::PushL( self );
    self->ConstructL( aFieldProperties );
    return self;
    }

CPbk2FieldGroupPropertyArray::~CPbk2FieldGroupPropertyArray()
    {
    iFieldGroups.ResetAndDestroy();
    }

// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::GroupCount
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2FieldGroupPropertyArray::GroupCount() const
    {
    return iFieldGroups.Count();
    }

// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::GroupAt
// ---------------------------------------------------------------------------
//    
EXPORT_C const CPbk2FieldPropertyGroup& CPbk2FieldGroupPropertyArray::GroupAt(
        TInt aIndex ) const
    {
    return *iFieldGroups[aIndex];
    }

// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::FindGroupForProperty
// ---------------------------------------------------------------------------
//
EXPORT_C const CPbk2FieldPropertyGroup* 
        CPbk2FieldGroupPropertyArray::FindGroupForProperty(
            const MPbk2FieldProperty& aProperty) const
    {
    const TInt groupCount = iFieldGroups.Count();
    for (TInt i = 0; i < groupCount; ++i)
        {
        const CPbk2FieldPropertyGroup& group = *iFieldGroups[i];
        TInt propCount = group.Count();
        for (TInt j = 0; j < propCount; ++j)
            {
            if (aProperty.FieldType().IsSame(group.At(j).FieldType()))
                {
                return &group;
                }
            }
        }
    return NULL;
    }

// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::ReadFieldGroupsL
// ---------------------------------------------------------------------------
//
void CPbk2FieldGroupPropertyArray::ReadFieldGroupsL
        (TResourceReader& aReader)
    {
    const TInt count = aReader.ReadInt16();
    for (TInt i = 0; i < count; ++i)
        {
        CPbk2FieldPropertyGroup* group =
            CPbk2FieldPropertyGroup::NewLC(aReader);
        iFieldGroups.AppendL(group);
        CleanupStack::Pop(group);
        }
    }

// ---------------------------------------------------------------------------
// CPbk2FieldGroupPropertyArray::PopulateGroupsL
// ---------------------------------------------------------------------------
//
void CPbk2FieldGroupPropertyArray::PopulateGroupsL(
        const MPbk2FieldPropertyArray& aFieldProperties )
    {
    const TInt propCount = aFieldProperties.Count();
    const TInt groupCount = iFieldGroups.Count();
    for (TInt i = 0; i < propCount; ++i)
        {
        TInt groupId = aFieldProperties.At( i ).GroupId();
        for (TInt j = 0; j < groupCount; ++j)
            {
            if (iFieldGroups[j]->GroupId() == groupId)
                {
                iFieldGroups[j]->AddFieldPropertyL( 
                    aFieldProperties.At( i ) );
                break;
                }
            }
        }
    
    // Remove empty groups
    for (TInt j = groupCount - 1; j >= 0; --j)
        {
        if (iFieldGroups[j]->Count() == 0)
            {
            delete iFieldGroups[j];
            iFieldGroups.Remove(j);
            }
        }
    }

// End of File
