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
* Description:  Phonebook 2 field property group.
*
*/


#include "CPbk2FieldPropertyGroup.h"

// System includes
#include <barsread.h>

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::CPbk2FieldPropertyGroup
// --------------------------------------------------------------------------
//
CPbk2FieldPropertyGroup::CPbk2FieldPropertyGroup()
    {
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::~CPbk2FieldPropertyGroup
// --------------------------------------------------------------------------
//
CPbk2FieldPropertyGroup::~CPbk2FieldPropertyGroup()
    {
    iProperties.Close();
    delete iLabel;
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2FieldPropertyGroup::ConstructL( TResourceReader& aReader )
    {
    aReader.ReadInt8(); // read version number
    iGroupId = static_cast<TPbk2FieldGroupId>( aReader.ReadInt8() );
    iLabel = aReader.ReadHBufC16L();
    iIconId = TPbk2IconId( aReader );
    iAddItemOrdering = aReader.ReadInt8();
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::NewLC
// --------------------------------------------------------------------------
//
CPbk2FieldPropertyGroup* CPbk2FieldPropertyGroup::NewLC
        ( TResourceReader& aReader )
    {
    CPbk2FieldPropertyGroup* self = new ( ELeave ) CPbk2FieldPropertyGroup;
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::Count
// --------------------------------------------------------------------------
//
EXPORT_C TInt CPbk2FieldPropertyGroup::Count() const
    {
    return iProperties.Count();
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::At
// --------------------------------------------------------------------------
//
EXPORT_C const MPbk2FieldProperty& CPbk2FieldPropertyGroup::At
        ( TInt aIndex ) const
    {
    return *iProperties[aIndex];
    }

// --------------------------------------------------------------------------
// CPbk2FieldPropertyGroup::AddFieldPropertyL
// --------------------------------------------------------------------------
//  
void CPbk2FieldPropertyGroup::AddFieldPropertyL
        ( const MPbk2FieldProperty& aProperty )
    {
    iProperties.AppendL( &aProperty );
    }

// End of File  
