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
*
*/








#ifndef __T_ADDRESSANDNUMBERSELECT_H__
#define __T_ADDRESSANDNUMBERSELECT_H__

// INCLUDES
#include <f32file.h>

// FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactEngine;

// CLASS-SPECIFIC PANIC CODES
enum TPanicCodes
    {
	KPanicUnsupportedFieldType=0,
	KPanicMultipleEmailAddressSavingFailed,
	KPanicUnsupportedFunctionCall,
	EPanicInvalidIndex
    };

// CONSTANTS
_LIT(KTestDBName,"c:CPbkAddressAndNumberSelect.cdb");
_LIT(KFillText, "TESTDATA");

_LIT(KDefaultFirstName, "James");
_LIT(KDefaultLastName, "Brown");
_LIT(KDefaultCompanyName, "Acme");
_LIT(KDefaultEmailAddress, "firstemail@mail.com"); // length=19
_LIT(KSecondEmailAddress, "secondemail@mail.com"); // length=20

_LIT(KFirstPhoneNumberGeneral, "1000001");
_LIT(KSecondPhoneNumberGeneral, "2000001");

_LIT(KFirstPhoneNumberWork, "1000002");

const TInt KEmailAddresses = 2;  
const TInt KPhoneNumbers = 2;
const TInt KMmsAddresses = 4;

const TInt KBufferSize = 25;

/**
 * Static class for testing PbkView APIs
 */
class T_AddressAndNumberSelect 
    {
    public:

		T_AddressAndNumberSelect();
    	~T_AddressAndNumberSelect();

        /**
         * Tests that email addresses are saved correctly to contact item.
         * @param aItem A reference to contact item.
         * @return ETrue if data saved correctly.
         */
		TBool EmailAddressesSaved(CPbkContactItem& aItem);

        /**
         * Fills contact item first and last name fields with test data.
         */
		void FillContactNameFieldL(CPbkContactItem& aItem, CPbkContactEngine* aContactEngine);

        /**
         * Fills company name field with test data.
         */
		void FillContactCompanyNameFieldL(CPbkContactItem& aItem, CPbkContactEngine* aContactEngine);

        /**
         * Fills contact item fields with test data.
         */
		void FillContactFieldsL(CPbkContactItem& aItem);

        /**
         * Fills contact item fields with multiple email addresses and
         * with other default test data.
         */
		void FillContactWithMultipleFieldsL (CPbkContactItem& aItem, CPbkContactEngine* aContactEngine,
						TBool aEmailAddresses, TBool aMmsNames,
						TBool aSmsNames, TBool aPhoneNumbers);

        /**
         * Tests that email address is in first field
         */
		TBool IsFirstEmailAddressL(const CPbkContactItem& aItem) const;

        /**
         * Tests that email address is in second field
         */
		TBool IsSecondEmailAddressL(const CPbkContactItem& aItem) const;

		/**
		* For email address field validity checking.
		* @param aIndex Index number to email address field.
		* @return ETrue if correct email address in index. 
		*/ 
		TBool IsCorrectEmailAddressL( const TInt& aIndex, const CPbkContactItem& aItem) const;
	
        /**
         * Returns number of email addresses
         */
		TInt  EmailAddresses() const;

        /**
         * Returns number of mms addresses
         */
		TInt  MmsAddresses() const;

        /**
         * Returns number of phone numbers
         */
		TInt  PhoneNumbers() const;

        /**
         * Returns mms field id
         */
		TInt MmsAddressFieldId(const TInt& aIndex) const;

		TBool IsCorrectMmsAddressL(TInt& aIndex, const CPbkContactItem& aItem) const;
		TBool IsCorrectPhoneNumberL(const TInt& aPhoneNumberType, TInt& aIndex, const CPbkContactItem& aItem) const;
	
	private:

        /**
         * Fills contact item second email field with default test data.
         */
		void AddSecondEmailFieldL(CPbkContactItem& aItem, CPbkContactEngine* aContactEngine );

        /**
         * Fills contact item second phone number field with default test data.
         */
		void AddSecondPhoneNumberFieldL(CPbkContactItem& aItem, CPbkContactEngine* aContactEngine );

        /**
         * Panic
         */
		void Panic(TInt aReason);
    };

#endif // __T_ADDRESSANDNUMBERSELECT_H__

// End of File
