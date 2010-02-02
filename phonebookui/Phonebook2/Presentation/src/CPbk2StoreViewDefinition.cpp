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
* Description:  Phonebook 2 store view definition.
*
*/


#include "CPbk2StoreViewDefinition.h"

// Virtual Phonebook
#include <CVPbkContactViewDefinition.h>

// System includes
#include <barsread.h>


// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::CPbk2StoreViewDefinition
// --------------------------------------------------------------------------
//
CPbk2StoreViewDefinition::CPbk2StoreViewDefinition()
    {
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::~CPbk2StoreViewDefinition
// --------------------------------------------------------------------------
//
CPbk2StoreViewDefinition::~CPbk2StoreViewDefinition()
    {
    delete iViewDefinition;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::NewL
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreViewDefinition* CPbk2StoreViewDefinition::NewL()
    {
    return new ( ELeave ) CPbk2StoreViewDefinition;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreViewDefinition* CPbk2StoreViewDefinition::NewLC
        ( TResourceReader& aReader )
    {
    CPbk2StoreViewDefinition* self = new ( ELeave ) CPbk2StoreViewDefinition;
    CleanupStack::PushL( self );
    self->ConstructL( aReader );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::NewLC
// --------------------------------------------------------------------------
//
EXPORT_C CPbk2StoreViewDefinition* CPbk2StoreViewDefinition::NewLC
        ( const CPbk2StoreViewDefinition& aViewDef )
    {
    CPbk2StoreViewDefinition* self = new ( ELeave ) CPbk2StoreViewDefinition;
    CleanupStack::PushL( self );
    self->ConstructL( aViewDef );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreViewDefinition::ConstructL( TResourceReader& aReader )
    {
    aReader.ReadInt8(); // read version number
    iViewType = TPbk2ContactViewType( aReader.ReadInt8() );
    iViewLocation = TPbk2ContactViewLocation( aReader.ReadInt8() );
    iViewDefinition = CVPbkContactViewDefinition::NewL( aReader );
    iViewItemIconId = TPbk2IconId( aReader );
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2StoreViewDefinition::ConstructL
        ( const CPbk2StoreViewDefinition& aViewDef )
    {
    iViewType = aViewDef.ViewType();
    iViewLocation = aViewDef.ViewLocation();
    iViewDefinition =
        CVPbkContactViewDefinition::NewL( aViewDef.ViewDefinition() );
    iViewItemIconId = aViewDef.ViewItemIconId();  
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::ViewType
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2ContactViewType CPbk2StoreViewDefinition::ViewType() const
    {
    return iViewType;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::ViewLocation
// --------------------------------------------------------------------------
//
EXPORT_C TPbk2ContactViewLocation
        CPbk2StoreViewDefinition::ViewLocation() const
    {
    return iViewLocation;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::ViewDefinition
// --------------------------------------------------------------------------
//
EXPORT_C const CVPbkContactViewDefinition&
        CPbk2StoreViewDefinition::ViewDefinition() const
    {
    return *iViewDefinition;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::ViewItemIconId
// --------------------------------------------------------------------------
//
EXPORT_C const TPbk2IconId& CPbk2StoreViewDefinition::ViewItemIconId() const
    {
    return iViewItemIconId;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::SetViewType
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreViewDefinition::SetViewType
        ( TPbk2ContactViewType aType )
    {
    iViewType = aType;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::SetViewLocation
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreViewDefinition::SetViewLocation
        ( TPbk2ContactViewLocation aLocation )
    {
    iViewLocation = aLocation;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::SetViewDefinition
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreViewDefinition::SetViewDefinition
        ( CVPbkContactViewDefinition* aViewDefinition )
    {
    delete iViewDefinition;
    iViewDefinition = aViewDefinition;
    }

// --------------------------------------------------------------------------
// CPbk2StoreViewDefinition::SetViewItemIcon
// --------------------------------------------------------------------------
//
EXPORT_C void CPbk2StoreViewDefinition::SetViewItemIcon
        ( const TPbk2IconId& aIconId )
    {
    iViewItemIconId = aIconId;
    }

// End of File
