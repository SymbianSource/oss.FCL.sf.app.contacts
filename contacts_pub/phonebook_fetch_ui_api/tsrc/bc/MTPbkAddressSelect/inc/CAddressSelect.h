/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:       
*       Email address selection dialog. This dialog implements the email
*       address selection logic of Phonebook.
*
*/








#ifndef __CADDRESSSELECT_H__
#define __CADDRESSSELECT_H__

//  INCLUDES
#include    "CPbkAddressSelect.h"

// CLASS DECLARATION
class CPbkContactItem;
//class TParams;
class CContactItem;
class CPbkContactEngine;
class CMTPbkAddressSelect;
/**
 * Email address selection dialog. This dialog implements the email
 * address selection logic of Phonebook.
 */
class CAddressSelect : public CPbkAddressSelect
    {
    public:  // Interface
        /**
         * Constructor.
         */
        CAddressSelect();

        /**
         * Parameters for ExecuteLD(TParams&).
         *
         * @see CPbkAddressSelect::TBaseParams
         * @see ExecuteLD(TParams&)
         */
        class TParams : public CPbkAddressSelect::TBaseParams
            {
            public:
                /**
                 * Constructor.
                 * @param aContactItem  contact where to select an email
                 *                      address.
                 */
                TParams(const CPbkContactItem& aContact);

            private:  // Spare data
                TInt32 iSpare1;
            };

        /**
         * Runs the email address selection query (if necessary).
         *
         * @param aParams   parameters and return values object of the query.
         * @return  true if query was accepted, false if canceled.
         * @see TParams
         */
        TBool ExecuteLD(TParams& aParams, CMTPbkAddressSelect& aAddressSelect);

        /**
         * Destructor.
         */
        ~CAddressSelect();
        
    private:  // from CPbkAddressSelect

        IMPORT_C TBool AddressField(const TPbkContactItemField& aField) const;
        IMPORT_C void NoAddressesL();
        IMPORT_C const TDesC& QueryTitleL();
        IMPORT_C TInt QuerySoftkeysResource() const;

    private: // data members
        /// Own: Title for email address selection list
        HBufC*                  iQueryTitle;
        // Spare data
        TInt32                  iSpare1;
        const CPbkContactItem*  iContact;
        TParams*                iParams;
        TBool                   iIsSame;
        CMTPbkAddressSelect*   iAddressSelect;

    };

#endif // __CADDRESSSELECT_H__

// End of File
