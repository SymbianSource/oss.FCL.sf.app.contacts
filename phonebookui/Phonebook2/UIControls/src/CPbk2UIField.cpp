/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact editor dialog UI field.
*
*/

#include <e32base.h>
#include "CPbk2UIField.h"

// System includes

#include <barsread.h>


// --------------------------------------------------------------------------
// CPbk2UIField::CPbk2UIField
// --------------------------------------------------------------------------
//    
CPbk2UIField::CPbk2UIField()
    {
    }

// --------------------------------------------------------------------------
// CPbk2UIField::~CPbk2UIField
// --------------------------------------------------------------------------
//    
CPbk2UIField::~CPbk2UIField()
    {
    delete iDefaultLabel;
    }

// --------------------------------------------------------------------------
// CPbk2UIField::ConstructL
// --------------------------------------------------------------------------
//    
inline void CPbk2UIField::ConstructL( TResourceReader& aReader )
    {
    aReader.ReadInt8(); // read version number
    iMultiplicity = aReader.ReadInt8();
    iMaxLength = aReader.ReadInt16();
    iEditMode = aReader.ReadInt8();
    iOrder = aReader.ReadInt8();
    iDefaultCase = aReader.ReadInt8();
    iIconId = TPbk2IconId( aReader );
    iCtrlType = aReader.ReadInt8();
    iFlags = aReader.ReadUint32();    
    iDefaultLabel = aReader.ReadHBufCL();
    }

// --------------------------------------------------------------------------
// CPbk2UIField::NewLC
// --------------------------------------------------------------------------
//    
CPbk2UIField* CPbk2UIField::NewL( TResourceReader& aReader )
    {
    CPbk2UIField* self = new( ELeave ) CPbk2UIField;
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2UIField::Multiplicity
// --------------------------------------------------------------------------
//    
TPbk2FieldMultiplicity CPbk2UIField::Multiplicity() const
    {
    return static_cast<TPbk2FieldMultiplicity>(iMultiplicity);
    }

// --------------------------------------------------------------------------
// CPbk2UIField::MaxLength
// --------------------------------------------------------------------------
//
TInt CPbk2UIField::MaxLength() const
    {
    return iMaxLength;
    }

// --------------------------------------------------------------------------
// CPbk2UIField::EditMode
// --------------------------------------------------------------------------
//
TPbk2FieldEditMode CPbk2UIField::EditMode() const
    {
    return static_cast<TPbk2FieldEditMode>(iEditMode);
    }

// --------------------------------------------------------------------------
// CPbk2UIField::DefaultCase
// --------------------------------------------------------------------------
//
TPbk2FieldDefaultCase CPbk2UIField::DefaultCase() const
    {
    return static_cast<TPbk2FieldDefaultCase>(iDefaultCase);
    }

// --------------------------------------------------------------------------
// CPbk2UIField::IconId
// --------------------------------------------------------------------------
//
const TPbk2IconId& CPbk2UIField::IconId() const
    {
    return iIconId;
    }

// --------------------------------------------------------------------------
// CPbk2UIField::CtrlType
// --------------------------------------------------------------------------
//
TPbk2FieldCtrlTypeExt CPbk2UIField::CtrlType() const
    {
    return static_cast<TPbk2FieldCtrlTypeExt>(iCtrlType);
    }

// --------------------------------------------------------------------------
// CPbk2UIField::Order
// --------------------------------------------------------------------------
//
TPbk2FieldOrder CPbk2UIField::Order() const
    {
    return static_cast<TPbk2FieldOrder>(iOrder);
    }

// --------------------------------------------------------------------------
// CPbk2UIField::Flags
// --------------------------------------------------------------------------
//
TUint CPbk2UIField::Flags() const
    {
    return iFlags;
    }

// --------------------------------------------------------------------------
// CPbk2UIField::DefaultLabel
// --------------------------------------------------------------------------
//
const TDesC& CPbk2UIField::DefaultLabel() const
    {
    return iDefaultLabel ? *iDefaultLabel : KNullDesC();
    }

// End of file
