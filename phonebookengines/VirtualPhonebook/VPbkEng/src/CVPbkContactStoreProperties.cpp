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
* Description:  A store property implementation class for the stores
*
*/



// INCLUDE FILES
#include "CVPbkContactStoreProperties.h"

#include <MVPbkContactStoreProperties2.h>

// LOCAL
/// Unnamed namespace for local definitions
namespace
    {
    #ifdef _DEBUG
    enum TPanicCode
        {
        EPanicSupportedFieldsPreCond = 1
        };

    void Panic( TPanicCode aPanic )
        {
        _LIT( KPanicTxt, "CVPbkContactStoreProperties" );
        User::Panic( KPanicTxt(), aPanic );
        }
    #endif // _DEBUG
    } // unnamed namespace


/*
* Small helper class which implements MVPbkContactStoreProperties2
*
*/
class TVPbkContactStoreProperties2 : public MVPbkContactStoreProperties2
    {
    public:

        TVPbkContactStoreProperties2( TUint32& aFlags )
        : iFlags( aFlags )
            {
            }

    public://from MVPbkContactStoreProperties2

        TBool SupportsOwnContact() const
            {
            return (iFlags & CVPbkContactStoreProperties::EPropertyOwnContactSupported) != 0;
            }

    private:

        ///Ref: Boolean flags
        TUint32& iFlags;

    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::CVPbkContactStoreProperties
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVPbkContactStoreProperties::CVPbkContactStoreProperties()
    {
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CVPbkContactStoreProperties* CVPbkContactStoreProperties::NewL()
    {
    CVPbkContactStoreProperties* self = new( ELeave )CVPbkContactStoreProperties;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();//self
    return self;
    }

// Destructor
CVPbkContactStoreProperties::~CVPbkContactStoreProperties()
    {
    iServiceTable.Close();

    delete iExtension2;
    }


// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::ConstructL
// -----------------------------------------------------------------------------
//
void CVPbkContactStoreProperties::ConstructL()
    {
    iExtension2 = new( ELeave ) TVPbkContactStoreProperties2( iFlags );
    }


// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SetName
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactStoreProperties::SetName(
        const TVPbkContactStoreUriPtr& aName)
    {
    iUriName.Set(aName);
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SetSupportedFields
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactStoreProperties::SetSupportedFields(
        const MVPbkFieldTypeList& aSupportedFields)
    {
    iSupportedFields = &aSupportedFields;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SetBooleanProperty
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactStoreProperties::SetBooleanProperty(
        TPropertyFlag aFlag,
        TBool aValue)
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
// CVPbkContactStoreProperties::SetServiceProperty
// -----------------------------------------------------------------------------
//
EXPORT_C void CVPbkContactStoreProperties::SetServicePropertyL(
        TUid aServiceUid,
        TBool aValue)
    {
    TBool append = ETrue;
    // look if the service is already in the table,
    for (TInt i = 0; i < iServiceTable.Count(); ++i)
        {
        TServiceElement element = iServiceTable[i];
        if (element.iServiceUid == aServiceUid)
            {
            // service uid exists in service table, replace value in table
            iServiceTable[i].iValue = aValue;
            append = EFalse;
            break;
            }
        }
    // service UID not in service table, append it to the array
    if (append)
        {
        TServiceElement element;
        element.iServiceUid = aServiceUid;
        element.iValue = aValue;
        iServiceTable.AppendL(element);
        }
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::Name
// -----------------------------------------------------------------------------
//
TVPbkContactStoreUriPtr CVPbkContactStoreProperties::Name() const
    {
    // parse actual name of the contact store
    return iUriName.Component(TVPbkContactStoreUriPtr::EContactStoreUriStoreLocation);
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::Uri
// -----------------------------------------------------------------------------
//
TVPbkContactStoreUriPtr CVPbkContactStoreProperties::Uri() const
    {
    return iUriName;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::ReadOnly
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::ReadOnly() const
    {
    return (iFlags & EPropertyReadOnly) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::Persistent
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::Persistent() const
    {
    return (iFlags & EPropertyPersistent) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::Local
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::Local() const
    {
    return (iFlags & EPropertyLocal) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::Removable
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::Removable() const
    {
    return (iFlags & EPropertyRemovable) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SupportsFieldLabels
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::SupportsFieldLabels() const
    {
    return (iFlags & EPropertyFieldLabelSupported) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SupportsDefaults
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::SupportsDefaults() const
    {
    return (iFlags & EPropertyDefaultsSupported) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SupportsVoiceTags
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::SupportsVoiceTags() const
    {
    return (iFlags & EPropertyVoiceTagsSupported) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SupportsSpeedDials
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::SupportsSpeedDials() const
    {
    return (iFlags & EPropertySpeedDialsSupported) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SupportsContactGroups
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::SupportsContactGroups() const
    {
    return (iFlags & EPropertyContactGroupsSupported) != 0;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SupportedFields
// -----------------------------------------------------------------------------
//
const MVPbkFieldTypeList& CVPbkContactStoreProperties::SupportedFields() const
    {
    // Make sure that iSupportedFields has been assigned
    __ASSERT_DEBUG( iSupportedFields, Panic( EPanicSupportedFieldsPreCond ) );

    return *iSupportedFields;
    }

// -----------------------------------------------------------------------------
// CVPbkContactStoreProperties::SupportsService
// -----------------------------------------------------------------------------
//
TBool CVPbkContactStoreProperties::SupportsService(TUid aServiceUid) const
    {
    // this function is able to map a service UID to a certain service
    // and return whether the store supports it.
    // this allows clients to query support for services, such as voice tag,
    // without the need to make changes to existing client interfaces
    TBool ret = EFalse;
    for (TInt i = 0; i < iServiceTable.Count(); ++i)
        {
        TServiceElement element = iServiceTable[i];
        if (element.iServiceUid == aServiceUid)
            {
            // service uid exists in service table,
            // return service table value
            ret = element.iValue;
            break;
            }
        }
    return ret;
    }


TAny* CVPbkContactStoreProperties::ContactStorePropertiesExtension( TUid aExtensionUid )
    {
    if( aExtensionUid == KMVPbkContactStorePropertiesExtension2Uid )
        {
        return static_cast<MVPbkContactStoreProperties2*>( iExtension2 );
        }

    return NULL;
    }

//  End of File
