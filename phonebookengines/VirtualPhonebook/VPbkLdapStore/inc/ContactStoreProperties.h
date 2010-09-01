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
* Description:  CContactStoreProperties - LDAP Store properties.
*
*/


#ifndef __CONTACTSTOREPROPERTIES_H__
#define __CONTACTSTOREPROPERTIES_H__

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <mvpbkcontactstoreproperties.h> 

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

/**
*  CContactStoreProperties - ldap store properties.
*  
*/
class CContactStoreProperties : public CBase,
                                public MVPbkContactStoreProperties
    {
    public:     // Types
        // Flags for boolean properties
        enum TPropertyFlag
            {
            EPropertyReadOnly               = 0x00000001,
            EPropertyPersistent             = 0x00000002,
            EPropertyLocal                  = 0x00000004,
            EPropertyRemovable              = 0x00000008,
            EPropertyFieldLabelSupported    = 0x00000010,
            EPropertyDefaultsSupported      = 0x00000020,
            EPropertyVoiceTagsSupported     = 0x00000040,
            EPropertySpeedDialsSupported    = 0x00000080,
            EPropertyContactGroupsSupported = 0x00000100
            };

    public:     // CContactStoreProperties public constructors and destructor
        /**
        @function   NewL
        @discussion Create CContactStoreProperties object
        @return     Pointer to instance of CContactStoreProperties
        */
        static CContactStoreProperties* NewL();

        /**
        @function   NewLC
        @discussion Create CContactStoreProperties object
        @return     Pointer to instance of CContactStoreProperties
        */
        static CContactStoreProperties* NewLC();

        /**
        @function   ~CContactStoreProperties
        @discussion CContactStoreProperties destructor
        */
        ~CContactStoreProperties();

    public:     // CContactStoreProperties public methods

        /**
        @function   SetName
        @discussion Sets the name of the store.
        @param      aName Name of the store
        */
        void SetName(const TVPbkContactStoreUriPtr& aName);

        /**
        @function   SetName
        @discussion Sets supported fields of contact store.
        @param      aSupportedFields Supported field types.
        */
        void SetSupportedFields(const MVPbkFieldTypeList& aSupportedFields);

        /**
        @function   SetBooleanProperty
        @discussion Sets the value of a boolean property.
        @param      aFlag  Flag to set.
        @param      aValue Boolean value to set.
        */
        void SetBooleanProperty(TPropertyFlag aFlag,TBool aValue);

    public:     // Methods from MVPbkContactStoreProperties

        /**
        @function   Name
        @discussion Returns EContactStoreUriStoreLocation component of the URI.
        @return     EContactStoreUriStoreLocation component of the URI
        */
        TVPbkContactStoreUriPtr Name() const;

        /**
        @function   Uri
        @discussion Returns the complete URI of the store.
        @return    Complete URI of the store.
        */
        TVPbkContactStoreUriPtr Uri() const;

        /**
        @function   ReadOnly
        @discussion Returns true if the store is read-only.
        @return     ETrue if the store is read-only.
        */
        TBool ReadOnly() const;

        /**
        @function   Persistent
        @discussion Returns true if the store is persistent.
        @return     ETrue if the store is persistent.
        */
        TBool Persistent() const;

        /**
        @function   Local
        @discussion Returns true if the store is local.
        @return     ETrue if the store is local.
        */
        TBool Local() const;

        /**
        @function   Removable
        @discussion Returns true if the store data is stored on removable media.
        @return     ETrue if the store data is stored on removable media.
        */
        TBool Removable() const;

        /**
        @function   SupportsFieldLabels
        @discussion Returns true if the store supports field labels.
        @return     ETrue if the store supports field labels.
        */
        TBool SupportsFieldLabels() const;

        /**
        @function   SupportsDefaults
        @discussion Returns true if the store supports defaults.
        @return     ETrue if the store supports defaults.
        */
        TBool SupportsDefaults() const;

        /**
        @function   SupportsVoiceTags
        @discussion Returns true if the store supports voice tags.
        @return     ETrue if the store supports voice tags.
        */
        TBool SupportsVoiceTags() const;

        /**
        @function   SupportsSpeedDials
        @discussion Returns true if the store supports speed dials.
        @return     ETrue if the store supports speed dials.
        */
        TBool SupportsSpeedDials() const;

        /**
        @function   SupportsContactGroups
        @discussion Returns true if the store supports contact groups.
        @return     ETrue if the store supports contact groups.
        */
        TBool SupportsContactGroups() const;

        /**
        @function   SupportedFields
        @discussion Returns list of supported fields.
        @return     Supported fields
        */
        const MVPbkFieldTypeList& SupportedFields() const;
        
        /**
        * See MVPbkContactStoreProperties
        */
        TBool SupportsService(TUid aServiceUid) const;


    private:    // CContactStoreProperties private constructors
        /**
        @function   CContactStoreProperties
        @discussion CContactStoreProperties default contructor
        */
        CContactStoreProperties();

        /**
        @function   ConstructL
        @discussion Perform the second stage construction of CContactStoreProperties 
        */
        void ConstructL();

    private:    // CContactStoreProperties private member variables

        /// Store name, the store name descriptor is owned by the plugin
        TVPbkContactStoreUriPtr iUriName;

        /// Boolean flags
        TUint32 iFlags;

        /// List of supported fields, reference
        const MVPbkFieldTypeList* iSupportedFields;

    };
} // End of namespace LDAPStore
#endif // __CONTACTSTOREPROPERTIES_H__
