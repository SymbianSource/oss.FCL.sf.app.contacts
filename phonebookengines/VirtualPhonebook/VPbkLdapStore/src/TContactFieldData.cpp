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
* Description:  Contact field data class implementation
*
*/


// INCLUDES
#include "tcontactfielddata.h"
#include "ldapcontactfield.h"

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// TContactFieldData::TContactFieldData
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
TContactFieldData::TContactFieldData() : iField(NULL)
    {
    }

// -----------------------------------------------------------------------------
// TContactFieldData::~TContactFieldData
// TContactFieldData Destructor
// -----------------------------------------------------------------------------
//
TContactFieldData::~TContactFieldData()
    {
    }

// -----------------------------------------------------------------------------
//                          TContactFieldData public methods
// -----------------------------------------------------------------------------
// TContactFieldData::SetField
// Set field
// -----------------------------------------------------------------------------
//
void TContactFieldData::SetField(CLDAPContactField& aField)
    {
    iField = &aField;
    }

// -----------------------------------------------------------------------------
//                      MVPbkContactFieldData implementation
// -----------------------------------------------------------------------------
// TContactFieldData::IsEmpty
// Returns true if data is empty
// -----------------------------------------------------------------------------
//
TBool TContactFieldData::IsEmpty() const
    {
    return (iField) ? iField->Data().Length() == 0 : ETrue;
    }

// -----------------------------------------------------------------------------
// TContactFieldData::CopyL
// Copies data from given field data.
// -----------------------------------------------------------------------------
//
void TContactFieldData::CopyL(const MVPbkContactFieldData& aFieldData)
    {
    if (aFieldData.DataType() == DataType())
        {
        SetTextL(MVPbkContactFieldTextData::Cast(aFieldData).Text());
        }
    }

// -----------------------------------------------------------------------------
//                      MVPbkContactFieldTextData implementation
// -----------------------------------------------------------------------------
// TContactFieldData::Text
// Returns the string data.
// -----------------------------------------------------------------------------
//
TPtrC TContactFieldData::Text() const
    {
    return (iField) ? iField->Data() : TPtrC();
    }

// -----------------------------------------------------------------------------
// TContactFieldData::SetTextL
// Sets the string data.
// -----------------------------------------------------------------------------
//
void TContactFieldData::SetTextL(const TDesC& aText)
    {
    if (iField)
        {
        iField->SetDataL(aText);
        }
    }

// -----------------------------------------------------------------------------
// TContactFieldData::MaxLength
// Returns maximum length of the field.
// -----------------------------------------------------------------------------
// 
TInt TContactFieldData::MaxLength() const
    {
    return KVPbkUnlimitedFieldLength;
    }

} // End of namespace LDAPStore

