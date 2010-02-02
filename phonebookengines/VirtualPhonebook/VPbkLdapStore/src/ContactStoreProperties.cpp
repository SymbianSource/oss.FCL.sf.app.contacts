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
* Description:  CContactStoreProperties implementation
*
*/


// INCLUDE FILES
#include "contactstoreproperties.h"

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// -----------------------------------------------------------------------------
// CContactStoreProperties::CContactStoreProperties
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CContactStoreProperties::CContactStoreProperties()
    {
    // No implementation required
    }
// -----------------------------------------------------------------------------
// CContactStoreProperties::ConstructL
// CContactStoreProperties constructor for performing 2nd stage construction
// -----------------------------------------------------------------------------
//
void CContactStoreProperties::ConstructL()
    {
    // No implementation at the moment
    }
// -----------------------------------------------------------------------------
// CContactStoreProperties::NewLC
// CContactStoreProperties two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreProperties* CContactStoreProperties::NewLC()
    {
    CContactStoreProperties* self = new (ELeave)CContactStoreProperties();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
// -----------------------------------------------------------------------------
// CContactStoreProperties::NewL
// CContactStoreProperties two-phased constructor.
// -----------------------------------------------------------------------------
//
CContactStoreProperties* CContactStoreProperties::NewL()
    {
    CContactStoreProperties* self = CContactStoreProperties::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::~CContactStoreProperties
// CContactStoreProperties Destructor
// -----------------------------------------------------------------------------
//
CContactStoreProperties::~CContactStoreProperties()
    {
    }

// -----------------------------------------------------------------------------
//                  CContactStoreProperties public methods
// -----------------------------------------------------------------------------
// CContactStoreProperties::SetName
// -----------------------------------------------------------------------------
//
void CContactStoreProperties::SetName(const TVPbkContactStoreUriPtr& aName)
    {
    iUriName.Set(aName);
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SetSupportedFields
// -----------------------------------------------------------------------------
//
void CContactStoreProperties::SetSupportedFields
(
    const MVPbkFieldTypeList& aSupportedFields
)
    {
    iSupportedFields = &aSupportedFields;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SetBooleanProperty
// -----------------------------------------------------------------------------
//
void CContactStoreProperties::SetBooleanProperty
(
    TPropertyFlag aFlag,
    TBool        aValue
)
    {
    if (aValue)
        {
        iFlags |= static_cast<TUint32>(aFlag);
        }
    else
        {
        iFlags &= ~static_cast<TUint32>(aFlag);
        }
    }

// -----------------------------------------------------------------------------
//                  MVPbkContactStoreProperties implementation
// -----------------------------------------------------------------------------
// CContactStoreProperties::Name
// -----------------------------------------------------------------------------
//
TVPbkContactStoreUriPtr CContactStoreProperties::Name() const
    {
    // Parse actual name of the contact store
    return iUriName.Component(TVPbkContactStoreUriPtr::EContactStoreUriStoreLocation);
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::Uri
// -----------------------------------------------------------------------------
//
TVPbkContactStoreUriPtr CContactStoreProperties::Uri() const
    {
    return iUriName;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::ReadOnly
// Returns true if the store is read-only.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::ReadOnly() const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::Persistent
// Returns true if the store is persistent.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::Persistent() const
    {
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::Local
// Returns true if the store is local.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::Local() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::Removable
// Returns true if the store data is stored on removable media.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::Removable() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SupportsFieldLabels
// Returns true if the store supports field labels.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::SupportsFieldLabels() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SupportsDefaults
// Returns true if the store supports defaults.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::SupportsDefaults() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SupportsVoiceTags
// Returns true if the store supports voice tags.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::SupportsVoiceTags() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SupportsSpeedDials
// Returns true if the store supports speed dials.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::SupportsSpeedDials() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SupportsContactGroups
// Returns true if the store supports contact groups.
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::SupportsContactGroups() const
    {
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CContactStoreProperties::SupportedFields
// -----------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CContactStoreProperties::SupportedFields() const
    {
    return *iSupportedFields;
    }


// -----------------------------------------------------------------------------
// CContactStoreProperties::SupportsService
// -----------------------------------------------------------------------------
//
TBool CContactStoreProperties::SupportsService(TUid aServiceUid) const
    {
    // TODO
    return EFalse;
    }


} // End of namespace LDAPStore
// -----------------------------------------------------------------------------
//  End of File
// -----------------------------------------------------------------------------
