/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*       Phonebook call number selection dialog. This dialog implements the call
*       number selection logic of Phonebook.
*
*/


#ifndef __CPbkSendKeyAddressSelect_H__
#define __CPbkSendKeyAddressSelect_H__

// INCLUDES
#include  "CPbkAddressSelect.h"

// FORWARD DECLARATIONS
class CPbkContactEngine;

// CLASS DECLARATION

/**
 * Phonebook call number selection dialog. This dialog implements the call
 * number selection logic of Phonebook.
 */
class CPbkSendKeyAddressSelect : public CPbkAddressSelect
    {
    public:  // Interface
        /**
         * Constructor.
         * @param aContactEngine Phonebook contact engine.
         */
        IMPORT_C CPbkSendKeyAddressSelect(
            CPbkContactEngine& aContactEngine);

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
                 * @param aContactItem  contact where to select a phone number.
                 * @param aDefaultField default field to use
                 */
                IMPORT_C TParams(const CPbkContactItem& aContact,
                    const TPbkContactItemField* aDefaultField);

            private: // data
				/// Own: spare data
                TInt32 iSpare1;
            };

        /**
         * Runs the phone number selection query (if necessary).
         *
         * @param aParams   parameters and return values object of the query.
         * @return  true if query was accepted, false if canceled.
         * @see TParams
         */
        IMPORT_C TBool ExecuteLD(TParams& aParams);

        /**
         * Destructor.
         */
        IMPORT_C ~CPbkSendKeyAddressSelect();

    private:  // from CPbkAddressSelect
        IMPORT_C const TDesC& QueryTitleL();
        IMPORT_C TInt QuerySoftkeysResource() const;
        IMPORT_C TKeyResponse PbkControlKeyEventL
            (const TKeyEvent& aKeyEvent,TEventCode aType);
        IMPORT_C TBool AddressField(const TPbkContactItemField& aField) const;
        IMPORT_C void NoAddressesL();

    private: // data members
        /// Own: Title for phonenumber selection list
        HBufC* iQueryTitle;
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        // Spare data
        TInt32 iSpare1;
    }; 

#endif // __CPbkSendKeyAddressSelect_H__
            
// End of File
