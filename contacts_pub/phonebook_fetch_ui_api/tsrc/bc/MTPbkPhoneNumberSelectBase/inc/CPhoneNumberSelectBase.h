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








#ifndef __CPHONENUMBERSELECTBASE_H__
#define __CPHONENUMBERSELECTBASE_H__

//  INCLUDES
#include    "CPbkPhoneNumberSelectBase.h"

// CLASS DECLARATION
class CPbkPhoneNumberSelectBase;
class CMTPbkPhoneNumberSelectBase;

/**
 * Phonebook call number selection dialog. This dialog implements the call
 * number selection logic of Phonebook.
 */
class CPhoneNumberSelectBase : public CPbkPhoneNumberSelectBase
    {
    public:  // Interface
        /**
         * Constructor.
         */
        CPhoneNumberSelectBase( CMTPbkPhoneNumberSelectBase& aSelectBase );

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
         * Runs the phone number selection query (if necessary).
         *
         * @param aParams   parameters and return values object of the query.
         * @return  true if query was accepted, false if canceled.
         * @see TParams
         */
        TBool ExecuteLD(TParams& aParams);

        /**
         * Destructor.
         */
        ~CPhoneNumberSelectBase();
        
    private:  // from CPbkAddressSelect

        IMPORT_C const TDesC& QueryTitleL();
        IMPORT_C TInt QuerySoftkeysResource() const;
        IMPORT_C TKeyResponse PbkControlKeyEventL
            (const TKeyEvent& aKeyEvent,TEventCode aType);

    private: // data members
        /// Own: Title for email address selection list
        HBufC*                          iQueryTitle;
        CPbkPhoneNumberSelectBase*      iPhoneNumberSelectBase;    
        CMTPbkPhoneNumberSelectBase*   iSelectBase;
    };

#endif // __CPHONENUMBERSELECTBASE_H__

// End of File
