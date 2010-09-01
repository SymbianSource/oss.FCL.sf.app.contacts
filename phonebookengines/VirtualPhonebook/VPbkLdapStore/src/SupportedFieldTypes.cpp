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
* Description:  CSupportedFieldTypes implementation
*
*/


// INCLUDE FILES
#include "supportedfieldtypes.h"
#include "fieldtypemappings.h"
#include "ldapfieldtype.h"      // LDAP Field type

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::CSupportedFieldTypes
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSupportedFieldTypes::CSupportedFieldTypes()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::ConstructL
// CSupportedFieldTypes constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CSupportedFieldTypes::ConstructL(
    RArray<TLDAPFieldType>&   aFields,
    const CFieldTypeMappings& aFieldTypeMappings
)
    {
    // Store field type count
    TInt count = aFields.Count();
    // Find match
    for (TInt loop = 0;loop < count;loop++)
        {
        // Contact model type id
        TInt type = aFields[loop].iCntFieldId;
        if (type)
            {
            // Find field
            const MVPbkFieldType* fp = aFieldTypeMappings.Match(type);
            if (fp)
                {
                // Add field
                iSupportedTypes.AppendL(fp);
                }
            }
        } // End of loop over store fields
    }
// -----------------------------------------------------------------------------
// CSupportedFieldTypes::NewLC
// CSupportedFieldTypes two-phased co§nstructor.
// -----------------------------------------------------------------------------
//
CSupportedFieldTypes* CSupportedFieldTypes::NewLC(
    RArray<TLDAPFieldType>&   aFields,
    const CFieldTypeMappings& aFieldTypeMappings
)
    {
    CSupportedFieldTypes* self = new (ELeave)CSupportedFieldTypes();
    CleanupStack::PushL(self);
    self->ConstructL(aFields,aFieldTypeMappings);
    return self;
    }
// -----------------------------------------------------------------------------
// CSupportedFieldTypes::NewL
// CSupportedFieldTypes two-phased constructor.
// -----------------------------------------------------------------------------
//
CSupportedFieldTypes* CSupportedFieldTypes::NewL(
    RArray<TLDAPFieldType>&   aFields,
    const CFieldTypeMappings& aFieldTypeMappings
)
    {
    CSupportedFieldTypes* self = CSupportedFieldTypes::NewLC(aFields,
                                                             aFieldTypeMappings);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::~CSupportedFieldTypes
// CSupportedFieldTypes Destructor
// -----------------------------------------------------------------------------
//
CSupportedFieldTypes::~CSupportedFieldTypes()
    {
    iSupportedTypes.Close();
    }

// -----------------------------------------------------------------------------
//                  MVPbkFieldTypeList implementation
// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FieldTypeCount
// -----------------------------------------------------------------------------
//
TInt CSupportedFieldTypes::FieldTypeCount() const
    {
    return iSupportedTypes.Count();
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FieldTypeAt
// -----------------------------------------------------------------------------
//
const MVPbkFieldType& CSupportedFieldTypes::FieldTypeAt(TInt aIndex) const
    {
    return *iSupportedTypes[aIndex];
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::ContainsSame
// -----------------------------------------------------------------------------
//
TBool CSupportedFieldTypes::ContainsSame(const MVPbkFieldType& aFieldType) const
    {
    TInt count = iSupportedTypes.Count();
    for (TInt loop = 0;loop < count;loop++)
        {
        if (iSupportedTypes[loop] == &aFieldType)
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::MaxMatchPriority
// -----------------------------------------------------------------------------
//
TInt CSupportedFieldTypes::MaxMatchPriority() const
    {
    // Just one, matches or not
    return 1;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FindMatch
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* CSupportedFieldTypes::FindMatch( 
    const TVPbkFieldVersitProperty& aMatchProperty,
    TInt aMatchPriority) const
    {
    TInt count = iSupportedTypes.Count();
    for (TInt loop = 0;loop < count;loop++)
        {
        if (iSupportedTypes[loop]->Matches(aMatchProperty,aMatchPriority))
            {
            return iSupportedTypes[loop];
            }
        }
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSupportedFieldTypes::FindMatch
// -----------------------------------------------------------------------------
//
const MVPbkFieldType* CSupportedFieldTypes::FindMatch( 
    TVPbkNonVersitFieldType aNonVersitType) const
    {
    TInt count = iSupportedTypes.Count();
    for (TInt loop = 0;loop < count;loop++)
        {
        if (iSupportedTypes[loop]->NonVersitType() == aNonVersitType)
            {
            return iSupportedTypes[loop];
            }
        }
    return NULL;
    }

}  // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
