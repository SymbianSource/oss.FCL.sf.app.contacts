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
* Description:  
*
*/


// INCLUDES
#include <cntdef.h>
#include <cntfield.h>
#include "TContactFieldData.h"
#include "TContactFieldDataImpl.h"
#include "VPbkCntModelUid.h"

namespace VPbkCntModel {

TContactFieldData::TContactFieldData()
    {
    // Constructor implemented to force class vftable generation to this module
    }

TContactFieldData::~TContactFieldData()
    {
    // Destructor implemented to force class vftable generation to this module
    }

void TContactFieldData::SetField(CContactItemField& aCntModelField)
    {
    // Instantiate a layout-compatible mapping class to this object's location
    // using placement new.
    switch( aCntModelField.StorageType() )
        {
        case KStorageTypeText:
            {
            TFieldType fType = {KUidContactFieldIMPPValue};
            if (aCntModelField.ContentType().ContainsFieldType(fType))
                {
                ::new(this) TContactFieldUriData(aCntModelField);
                }
            else
                {
                ::new(this) TContactFieldTextData(aCntModelField);
                }
            break;
            }
        case KStorageTypeStore:
            {
            ::new(this) TContactFieldBinaryData(aCntModelField);
            break;
            }
        case KStorageTypeDateTime:
            {
            ::new(this) TContactFieldDateTimeData(aCntModelField);
            break;
            }
        default:
            {
            ResetField();
            }
        }
    }

void TContactFieldData::ResetField()
    {
    // Reset vfptr
    ::new(this) TContactFieldData;
    }

TVPbkFieldStorageType TContactFieldData::DataType() const
    {
    return EVPbkFieldStorageTypeNull;
    }

TBool TContactFieldData::IsEmpty() const
    {
    return ETrue;
    }

void TContactFieldData::CopyL(const MVPbkContactFieldData& /*aFieldData*/)
    {
    }

}  // namespace VPbkCntModel

