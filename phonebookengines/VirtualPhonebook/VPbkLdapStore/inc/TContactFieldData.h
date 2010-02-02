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
* Description:  TContactFieldData - Contact field data class implementation
*
*/


#ifndef __TCONTACTFIELDDATA_H__
#define __TCONTACTFIELDDATA_H__

// INCLUDES
#include <mvpbkcontactfieldtextdata.h>

// FORWARD DECLARATIONS
class CLDAPContactField;

// -----------------------------------------------------------------------------
// LDAP Store namespace
// -----------------------------------------------------------------------------
namespace LDAPStore {

// CLASS DECLARATION

/**
*  TContactFieldData - contact field data class
*  
*/
class TContactFieldData : public MVPbkContactFieldTextData
    {
    public:     // TStoreContactField public constructor and destructor

        /**
        @function   TContactFieldData
        @discussion TContactFieldData default contructor
        */
        TContactFieldData();
        /**
        @function  ~TContactFieldData
        @discussion TContactFieldData destructor
        */
        ~TContactFieldData();

    public:  // New functions
        
        /**
        @function   SetField
        @discussion Sets this field.
        @param      aField Field.
        */
        void SetField(CLDAPContactField& aField);

    public:     // Methods from MVPbkContactFieldTextData

        /**
        @function   IsEmpty
        @discussion Returns true if this field is empty.
        @param      True, field data is empty or false when not.
        */
        TBool IsEmpty() const;

        /**
        @function   CopyL
        @discussion Copies passed data.
        @param      aFieldData Field data to copy.
        */
        void CopyL(const MVPbkContactFieldData& aFieldData);

        /**
        @function   Text
        @discussion Returns this field data text.
        @return     Field data text.
        */
        TPtrC Text() const;

        /**
        @function   SetTextL
        @discussion Sets this field data text.
        @return     aText Field data text.
        */
        void SetTextL(const TDesC& aText);

        /**
        @function   MaxLength
        @discussion Returns this field's maximum length.
        @return     Maximum length.
        */
        TInt MaxLength() const;
        
    private:    // TContactFieldData private member variables

        /// Contact field reference
        CLDAPContactField* iField;
    };

} // End of namespace LDAPStore

#endif // __TCONTACTFIELDDATA_H__
