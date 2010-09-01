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








// INCLUDES
#include "T_AddressAndNumberSelect.h"

#include <CPbkContactItem.h>
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldsInfo.h>


/*******************************************************************
*
*	CONSTRUCTOR
*
*******************************************************************/
T_AddressAndNumberSelect::T_AddressAndNumberSelect()
    {
    }

/*******************************************************************
*
*	DESTRUCTOR
*
*******************************************************************/
T_AddressAndNumberSelect::~T_AddressAndNumberSelect()
    {
    }


/*******************************************************************
*
*	FillContactNameFieldL()
*
*******************************************************************/
void T_AddressAndNumberSelect::FillContactNameFieldL( 
		CPbkContactItem& aItem,CPbkContactEngine* aContactEngine)
	{
	//Add second first name field
	TPbkContactItemField* field =  aItem.AddOrReturnUnusedFieldL(
			*aContactEngine->FieldsInfo().Find( EPbkFieldIdFirstName ) );
	field->TextStorage()->SetTextL(KDefaultFirstName);

	field =  aItem.AddOrReturnUnusedFieldL(
		*aContactEngine->FieldsInfo().Find( EPbkFieldIdLastName ) );
	field->TextStorage()->SetTextL(KDefaultLastName);
	}

/*******************************************************************
*
*	FillContactCompanyNameFieldL()
*
*******************************************************************/
void T_AddressAndNumberSelect::FillContactCompanyNameFieldL(
        CPbkContactItem& aItem,CPbkContactEngine* aContactEngine)
	
	{
	TPbkContactItemField* field =  aItem.AddOrReturnUnusedFieldL(
		*aContactEngine->FieldsInfo().Find( EPbkFieldIdCompanyName ) );
	field->TextStorage()->SetTextL(KDefaultCompanyName);
    }

/*******************************************************************
*
*	FillContactFieldsL()
*
*******************************************************************/
void T_AddressAndNumberSelect::FillContactFieldsL( CPbkContactItem& aItem )
    {
	for (TInt i = 0; i < aItem.CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = aItem.CardFields()[i];

        switch (field.PbkFieldId())
            {
	        case EPbkFieldIdLastName:
				{
	            field.TextStorage()->SetTextL(KDefaultLastName);
	            break;
				}
	        case EPbkFieldIdFirstName:
				{
	            field.TextStorage()->SetTextL(KDefaultFirstName);
	            break;
				}
	        case EPbkFieldIdCompanyName:
				{
	            field.TextStorage()->SetTextL(KDefaultCompanyName);
				break;
				}
			case EPbkFieldIdEmailAddress:
				{
				field.TextStorage()->SetTextL(KDefaultEmailAddress);
				break;
				}

			// Fill only one phone number field
			case EPbkFieldIdPhoneNumberGeneral:
				{
				field.TextStorage()->SetTextL(KFirstPhoneNumberGeneral);
				break;
				}

			case EPbkFieldIdPhoneNumberWork:
			case EPbkFieldIdPhoneNumberMobile:	
			case EPbkFieldIdFaxNumber:
			case EPbkFieldIdPagerNumber:
				{
				break;
				}

	        default:
				{
				switch (field.StorageType())
					{
					case KStorageTypeText:
						{
						field.TextStorage()->SetTextL(KFillText);
						break;
						}
					case KStorageTypeDateTime:
						{
						TTime now;
						now.UniversalTime();
						field.DateTimeStorage()->SetTime(now);
						break;
						}

					default:
						{
						Panic(KPanicUnsupportedFieldType);
					    break;
						}
					} // switch(field.StorageType())
				
				}                 
			break;
			} 
        }
    }

/*******************************************************************
*
*	EmailAddressesSaved()
*
*******************************************************************/
TBool T_AddressAndNumberSelect::EmailAddressesSaved(CPbkContactItem& aItem)
	{
	TInt count=0;
	TInt search=0;
	for (TInt index=0; index < KEmailAddresses ;index++)
		{
		search+=index;
		TPbkContactItemField* field = aItem.FindField(
						EPbkFieldIdEmailAddress, search);

		switch(index)
			{
			case 0:	
				{
				if( field->TextStorage()->Text() != KDefaultEmailAddress )
					{
					return EFalse;
					}
				else
					{
					count++;
					}
				break;
				}
			case 1:
				{
				if (field->TextStorage()->Text() != KSecondEmailAddress)
					{
					return EFalse;
					}
				else
					{
					count++;
					}
				break;
				}
			default:
				{
				return EFalse;
				}
			}
		}

	// Return false if not all email addresses saved correctly.
	if ( count != KEmailAddresses )
		{
		return EFalse;
		}

	// All data saved.
	return ETrue;
	}

/*******************************************************************
*
*	FillContactWithMultipleFieldsL()
*
*******************************************************************/
void T_AddressAndNumberSelect::FillContactWithMultipleFieldsL(
        CPbkContactItem& aItem,
		CPbkContactEngine* aContactEngine,
		TBool aEmailAddresses,
		TBool aMmsNames,
		TBool aSmsNames,
		TBool aPhoneNumbers )
    {
    	
	// Only one parameter can have ETrue value.
	if ( ( TInt(aEmailAddresses)+
		   TInt(aMmsNames)+
		   TInt(aSmsNames)+
		   TInt(aPhoneNumbers) ) != 1 )
		{
		Panic( KPanicUnsupportedFunctionCall );
		}

	FillContactFieldsL( aItem );

	if( aEmailAddresses )
		{
		AddSecondEmailFieldL(aItem,aContactEngine);
		} 
	
	else if( aMmsNames )
		{
		AddSecondEmailFieldL(aItem,aContactEngine);
		AddSecondPhoneNumberFieldL(aItem,aContactEngine);
		}

	else if( aPhoneNumbers || aSmsNames )
		{
		AddSecondPhoneNumberFieldL(aItem,aContactEngine);
		}
	else
		{
		Panic(KPanicUnsupportedFunctionCall);
		}

    }

/*******************************************************************
*
*	IsFirstEmailAddressL()
*
*******************************************************************/
TBool T_AddressAndNumberSelect::IsFirstEmailAddressL(const CPbkContactItem& aItem) const
	{
	TPbkContactItemField* field = NULL;
	TBuf<KBufferSize> text;


	field = aItem.FindField(EPbkFieldIdEmailAddress);

	if (field)
		{
		field->GetTextL(text);
		}
	return ( text.Compare(KDefaultEmailAddress)==0 );
	}

/*******************************************************************
*
*	IsSecondEmailAddressL()
*
*******************************************************************/
TBool T_AddressAndNumberSelect::IsSecondEmailAddressL(const CPbkContactItem& aItem) const
	{
	TPbkContactItemField* field = NULL;
	TInt searchindex = 0;
	TBuf<KBufferSize> text;

	field = aItem.FindField(EPbkFieldIdEmailAddress, searchindex);
	if (searchindex <0)
		{
		return EFalse;
		}
	// Second email is asked, so do find it starting after
	// the first found email index 
	++searchindex;
	field = aItem.FindField(EPbkFieldIdEmailAddress, searchindex);
	if (field)
		{
		field->GetTextL(text);
		}
	return ( text.Compare(KSecondEmailAddress)==0 );
	}

/*******************************************************************
*
*	IsCorrectEmailAddressL()
*
*******************************************************************/
TBool T_AddressAndNumberSelect::IsCorrectEmailAddressL(
		const TInt& aIndex, const CPbkContactItem& aItem) const
	{
	TPbkContactItemField* field = NULL;
	TInt searchindex = 0;
	field = aItem.FindField(EPbkFieldIdEmailAddress, searchindex);
	// If the second email is asked, do find it starting after
	// the first found email index 
	if (aIndex > 0)
		{
		++searchindex;
		field = aItem.FindField(EPbkFieldIdEmailAddress, searchindex);
		}

	if (field)
		{
		TBuf<KBufferSize> text;
		field->GetTextL(text);
		switch( aIndex )
			{
			case 0:
				{
				return ( text.Compare(KDefaultEmailAddress)==0 );
				}
			case 1:
				{
				return ( text.Compare(KSecondEmailAddress)==0 );
				}
			default:
				{
				return EFalse;
				}
			}
		}

	return EFalse;
	}
	
/*******************************************************************
*
*	EmailAddresses()
*
*******************************************************************/
TInt T_AddressAndNumberSelect::EmailAddresses() const
	{
	return KEmailAddresses;
	}

/*******************************************************************
*
*	IsCorrectPhoneNumberL()
*
*******************************************************************/
TBool T_AddressAndNumberSelect::IsCorrectPhoneNumberL(
		const TInt& aPhoneNumberType,
		TInt& aIndex, const CPbkContactItem& aItem) const
	{
	if ( aPhoneNumberType == EPbkFieldIdPhoneNumberGeneral )
		{
		TPbkContactItemField* field = NULL;
		TInt searchindex = 0;
		field = aItem.FindField(aPhoneNumberType, searchindex);
		// If the second phonenumber is asked, do find it starting after
		// the first found phonenumber index 
		if (aIndex > 0)
			{
			++searchindex;
			field = aItem.FindField(aPhoneNumberType, searchindex);
			}
		if (field)
			{
			TBuf<KBufferSize> text;
			field->GetTextL(text);
			switch( aIndex )
				{
				case 0:
					{
					return ( text.Compare(KFirstPhoneNumberGeneral)==0 );
					}
				case 1:
					{
					return ( text.Compare(KSecondPhoneNumberGeneral)==0 );
					}
				default:
					{
					return EFalse;
					}
				}
			}
			
		}
		return EFalse;
	}


/*******************************************************************
*
*	IsCorrectPhoneNumberL()
*
*******************************************************************/
TInt T_AddressAndNumberSelect::PhoneNumbers() const
	{
	return KPhoneNumbers;
	}


/*******************************************************************
*
*	IsCorrectMmsAddressL()
*
*******************************************************************/
TBool T_AddressAndNumberSelect::IsCorrectMmsAddressL(TInt& aIndex, const CPbkContactItem& aItem ) const
	{
	if ( aIndex < KPhoneNumbers && aIndex >= 0)
		{
		TInt type = EPbkFieldIdPhoneNumberGeneral;
		return IsCorrectPhoneNumberL( type , aIndex, aItem );
		}
	else if ( aIndex >= 0 && aIndex < ( KPhoneNumbers + KEmailAddresses ) )
		{ 
		return IsCorrectEmailAddressL( (aIndex - KPhoneNumbers), aItem );
		}
	else
		{
		return EFalse;
		}
	}

/*******************************************************************
*
*	MmsAddresses()
*
*******************************************************************/
TInt T_AddressAndNumberSelect::MmsAddresses() const
	{
	return KMmsAddresses;
	}

/*******************************************************************
*
*	MmsAddressFieldId()
*
*******************************************************************/
TInt T_AddressAndNumberSelect::MmsAddressFieldId(const TInt& aIndex) const
	{
	if (aIndex < 0 || aIndex > KMmsAddresses )
		{
		return -1;
		}
	// Decide focused field type.
	return (aIndex < KPhoneNumbers ? 
		EPbkFieldIdPhoneNumberGeneral : 
		EPbkFieldIdEmailAddress);
	}

/*******************************************************************
*
*	AddSecondEmailFieldL()
*
*******************************************************************/
void T_AddressAndNumberSelect::AddSecondEmailFieldL(
									CPbkContactItem& aItem, 
									CPbkContactEngine* aContactEngine )
	{
	//Add second email address field
	TPbkContactItemField* field =  aItem.AddOrReturnUnusedFieldL(
		*aContactEngine->FieldsInfo().Find( EPbkFieldIdEmailAddress ) );
	field->TextStorage()->SetTextL(KSecondEmailAddress);

	if ( !EmailAddressesSaved(aItem) )
		{
		Panic ( (KPanicMultipleEmailAddressSavingFailed+2) );
		};
	}

/*******************************************************************
*
*	AddSecondPhoneNumberFieldL()
*
*******************************************************************/
void T_AddressAndNumberSelect::AddSecondPhoneNumberFieldL(
									CPbkContactItem& aItem, 
									CPbkContactEngine* aContactEngine )
	{
	TPbkContactItemField* field =  aItem.AddOrReturnUnusedFieldL(
		*aContactEngine->FieldsInfo().Find( EPbkFieldIdPhoneNumberGeneral ) );
	field->TextStorage()->SetTextL(KSecondPhoneNumberGeneral);
	}	

/*******************************************************************
*
*	Panic()
*
*******************************************************************/
void T_AddressAndNumberSelect::Panic(TInt aReason)
	{
	_LIT(KClassName, "T_AddressAndNumberSelect");
	User::Panic(KClassName, aReason);
	}

// End of File
