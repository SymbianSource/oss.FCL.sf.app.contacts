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
* Description:  A store property implementation class for the stores
*
*/



#ifndef CVPBKCONTACTSTOREPROPERTIES_H
#define CVPBKCONTACTSTOREPROPERTIES_H

//  INCLUDES
#include <e32base.h>
#include <MVPbkContactStoreProperties.h>


// FORWARD DECLARATION
class TVPbkContactStoreProperties2;

// CLASS DECLARATION

/**
 * A store property implementation class for the stores
 *
 * @lib VPbkEng.lib
 */
class CVPbkContactStoreProperties :
        public CBase,
        public MVPbkContactStoreProperties
    {
    public:  // Types
        /// Flags for boolean properties
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
            EPropertyContactGroupsSupported = 0x00000100,
            EPropertyOwnContactSupported	= 0x00000200
            };

    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         * @return a new instance of this class
         */
        IMPORT_C static CVPbkContactStoreProperties* NewL();

        /**
         * Destructor.
         */
        ~CVPbkContactStoreProperties();

    public: // New functions

        /**
         * Sets the name of the store. Only a reference to the name is stored.
         * The reference must be valid for the lifetime of this object.
         * @param aName the name of the store
         */
        IMPORT_C void SetName(const TVPbkContactStoreUriPtr& aName);

        /**
         * Sets supported fields of contact store.
         * @param aSupportedFields a list of field types that are supported
         *                         by the store.
         */
        IMPORT_C void SetSupportedFields(const MVPbkFieldTypeList& aSupportedFields);

        /**
         * Sets the value of a boolean property.
         * @param aFlag the flag to set
         * @param aValue the boolean value for the flag
         */
        IMPORT_C void SetBooleanProperty(TPropertyFlag aFlag, TBool aValue);

        /**
         * Sets the aServiceUid service UID to aValue.
         * @param aServiceUid   a service UID that is mapped to a service.
         * @param aValue    The property to set as the value for the service.
         */
        IMPORT_C void SetServicePropertyL(TUid aServiceUid, TBool aValue);

    public: // from MVPbkContactStoreProperties
        TVPbkContactStoreUriPtr Name() const;
        TVPbkContactStoreUriPtr Uri() const;
        TBool ReadOnly() const;
        TBool Persistent() const;
        TBool Local() const;
        TBool Removable() const;
        TBool SupportsFieldLabels() const;
        TBool SupportsDefaults() const;
        TBool SupportsVoiceTags() const;
        TBool SupportsSpeedDials() const;
        TBool SupportsContactGroups() const;
        const MVPbkFieldTypeList& SupportedFields() const;
        TBool SupportsService(TUid aServiceUid) const;


    private: // Construction
        CVPbkContactStoreProperties();

        void ConstructL();

    public: // from MVPbkContactStoreProperties

        TAny* ContactStorePropertiesExtension( TUid aExtensionUid );

    private:

        /**
         * Service table element.
         */
        struct TServiceElement
            {
            /// Own: UID associated with a service
            TUid iServiceUid;
            /// Own: value of the service
            TBool iValue;
            };

    private:    // Data
        /// Ref: Store name, the store name descriptor is owned by the plugin
        TVPbkContactStoreUriPtr iUriName;
        /// Own: Boolean flags
        TUint32 iFlags;
        /// Own: Number of supported digits in phone number matching
        TInt iMaxNumberMatchDigits;
        /// Ref: List of supported fields
        const MVPbkFieldTypeList* iSupportedFields;
        /// Own: Service property map
        RArray<TServiceElement> iServiceTable;
        /// Own: Extended properties
        TVPbkContactStoreProperties2* iExtension2;
    };

#endif      // CVPBKCONTACTSTOREPROPERTIES_H

// End of File
