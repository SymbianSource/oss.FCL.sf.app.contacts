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
*    CPbkFieldInfo export type <-> MPbkVcardProperty adapter.
*
*/


// INCLUDE FILES
#include "TBCardPropertyAdapter.h"
#include "CBCardParserProperty.h"
#include "CBCardFieldUidMapping.h"


// ==================== MEMBER FUNCTIONS ====================

TBCardPropertyAdapter::TBCardPropertyAdapter
        (const CParserProperty& aProperty,
        TInt aFieldIndex,
        const CBCardFieldUidMapping& aFieldUidMapping) :
    iProperty(CBCardParserProperty::Cast(aProperty)),
    iFieldIndex(aFieldIndex),
    iFieldUidMapping(aFieldUidMapping)
    {
    }

// Explicit destructor prevents compiler generating multiple destructors in
// client object files.
TBCardPropertyAdapter::~TBCardPropertyAdapter()
    {
    }

TUid TBCardPropertyAdapter::PropertyName() const
    {
    const TInt mappingIndex = iFieldUidMapping.Find(
        CBCardFieldUidMapping::TVcardField(iProperty.Name(), iFieldIndex));
    if (mappingIndex >= 0)
        {
        return iFieldUidMapping[mappingIndex].iUid;
        }
    return TUid::Null();
    }

const MPbkVcardParameters& TBCardPropertyAdapter::PropertyParameters() const
    {
    return *this;
    }

TInt TBCardPropertyAdapter::ParameterCount() const
    {
    const CArrayPtr<CParserParam>* paramArray = iProperty.ArrayOfParams();
    if (paramArray)
        {
        return paramArray->Count();
        }
    return 0;
    }

TUid TBCardPropertyAdapter::ParameterAt(TInt aIndex) const
    {
    const CParserParam* param = iProperty.ArrayOfParams()->At(aIndex);
    if (param)
        {
        TInt mappingIndex;
        _LIT8(KNameType,"TYPE");
        if (param->Name() == KNameType)
            {
            // Parameter may be of format "TYPE=FAX" -> we must 
            // get the value of the parameter to get the 
            // actual parameter name.
            mappingIndex = iFieldUidMapping.Find(param->Value());
            }
        else
            {
            mappingIndex = iFieldUidMapping.Find(param->Name());
            }

        if (mappingIndex >= 0)
            {
            return iFieldUidMapping[mappingIndex].iUid;
            }
        }
    return TUid::Null();
    }


// End of File
