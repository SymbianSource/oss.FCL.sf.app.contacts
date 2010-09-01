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
* Description:  The virtual phonebook store field data implementation
*
*/


// INCLUDES
#include "TContactFieldData.h"

#include <CVPbkSimCntField.h>

namespace VPbkSimStore {

TContactFieldData::TContactFieldData()
:   iSimField( NULL )
    {
    // Constructor implemented to force class vftable generation to this module
    }

TContactFieldData::~TContactFieldData()
    {
    // Destructor implemented to force class vftable generation to this module
    }

void TContactFieldData::SetField( CVPbkSimCntField& aSimField )
    {
    iSimField = &aSimField;
    }

TBool TContactFieldData::IsEmpty() const
    {
    return iSimField->Data().Length() == 0;
    }

TPtrC TContactFieldData::Text() const
    {
    return iSimField->Data();
    }

void TContactFieldData::SetTextL( const TDesC& aText )
    {
    iSimField->SetDataL( aText );
    }

void TContactFieldData::CopyL( const MVPbkContactFieldData& aFieldData )
    {
    if ( aFieldData.DataType() == DataType() )
        {
        SetTextL( MVPbkContactFieldTextData::Cast( aFieldData ).Text() );
        }
    }

TInt TContactFieldData::MaxLength() const
    {
    return iSimField->MaxDataLength();
    }
}  // namespace VPbkSimStore

