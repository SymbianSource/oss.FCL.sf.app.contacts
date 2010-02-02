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
* Description:        ?Description
*
*/









// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "MTPbkContactItem.h"
#include <CPbkContactItem.h>
#include <CPbkContactEngine.h>
#include <CPbkFieldInfo.h>
#include <CPbkFieldsInfo.h>


// the string which will be set to textfields in FillContactFieldsL
_LIT(KFillText, "TESTDATA");
// some defined phonenumbers
_LIT(KNumber1,"5005001");
_LIT(KNumber2,"6005001");
_LIT(KNumber3,"7105001");
// panic text
_LIT(KPanicText,"TE_CPbkContactItem");

// size of CPbkContactItem class
const TInt KClassSize = 40;
// lenght of phone number
const TInt KPhoneNumberLenght = 10;

//maximum lenghts of fields
const TInt KNameFieldMaxLenght = 50;
const TInt KEmailFieldMaxLenght = 150;
const TInt KAddressFieldMaxLenght = 250;
const TInt KAddressSubfieldFieldMaxLenght = 50;
const TInt KPostalCodeFieldMaxLenght = 20;
const TInt KURLFieldMaxLenght = 1000;
const TInt KDTMFFieldMaxLenght = 60;
const TInt KNoteFieldMaxLenght = 1000;
const TInt KFileNameFieldMaxLenght = 256;
const TInt KVOIPFieldMaxLenght = 100;
const TInt KIdSuffixMaxLenght = 10;

enum TPanicCodes
    {
    KPanicUnsupportedFieldType = 1
    };


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES  
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMTPbkContactItem::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkContactItem::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkContactItem::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkContactItem::RunMethodL( 
    CStifItemParser& aItem ) 
    {

     TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "TestsizeofCPbkContactItemclass", CMTPbkContactItem::TestSize ),
        ENTRY( "CreateanddeleteCPbkContactItem", CMTPbkContactItem::RunNewLTest ),
        ENTRY( "Createemptycontact", CMTPbkContactItem::RunCreateEmptyContact ),
        ENTRY( "Defaultphonenumber", CMTPbkContactItem::RunDefaultTestL ),
        ENTRY( "DefaultMMS", CMTPbkContactItem::RunDefaultTest1L ),
        ENTRY( "DefaultSMS", CMTPbkContactItem::RunDefaultTest2L ),
        ENTRY( "DefaultEmail", CMTPbkContactItem::RunDefaultTest3L ),
        ENTRY( "Defaultvideonumber", CMTPbkContactItem::RunDefaultTest4L ),
        ENTRY( "DefaultPoc", CMTPbkContactItem::RunDefaultTest5L ),
        ENTRY( "DefaultVOIP", CMTPbkContactItem::RunDefaultTest6L ),
        ENTRY( "DefaultEmailOverSms", CMTPbkContactItem::RunDefaultTest7L ),
        ENTRY( "AllDefault", CMTPbkContactItem::RunAllDefaultL ),
        ENTRY( "Removeallfromend", CMTPbkContactItem::RunRemoveAllFromEnd ),
        ENTRY( "Removeallfromstart", CMTPbkContactItem::RunRemoveAllFromStart),
        ENTRY( "Addfields", CMTPbkContactItem::RunAddFieldsL ),
        ENTRY( "Updatefieldset", CMTPbkContactItem::RunUpdateFieldSetL ),
        ENTRY( "Getcontacttitle", CMTPbkContactItem::RunGetContactTitleL ),
        ENTRY( "Getcontacttitleornull", CMTPbkContactItem::RunGetContactTitleOrNullL ),
        ENTRY( "Getcontacticonindex", CMTPbkContactItem::RunGetContactIconIndexL ),
        ENTRY( "FindFieldplain", CMTPbkContactItem::RunFindFieldPlainL ),
        ENTRY( "FindFieldindexed", CMTPbkContactItem::RunFindFieldIndexedL ),
        ENTRY( "FindFieldwithinfo", CMTPbkContactItem::RunFindFieldWithInfoL ),
        ENTRY( "FindFieldwithindexedinfo", CMTPbkContactItem::RunFindFieldIndexedInfoL ),
        ENTRY( "FindFieldindex", CMTPbkContactItem::RunFindFieldIndex ),
        ENTRY( "FindContactItemFieldIndex", CMTPbkContactItem::RunFindContactItemFieldIndex ),
        ENTRY( "RunFindNextFieldWithPhoneNumberExact", CMTPbkContactItem::RunFindNextFieldWithPhoneNumberExact ),
        ENTRY( "RunFindNextFieldWithPhoneNumberDigits", CMTPbkContactItem::RunFindNextFieldWithPhoneNumberDigits ),
        ENTRY( "RunFindNextFieldWithPhoneNumberNot", CMTPbkContactItem::RunFindNextFieldWithPhoneNumberNot ),
        ENTRY( "FindNextFieldWithTextBasic", CMTPbkContactItem::RunFindNextFieldWithTextBasic ),
        ENTRY( "FindNextFieldWithTextNot", CMTPbkContactItem::RunFindNextFieldWithTextNot ),
        ENTRY( "FindSameField", CMTPbkContactItem::RunFindSameFieldL ),
        ENTRY( "CanAcceptDataOfType", CMTPbkContactItem::RunCanAcceptDataOfTypeL ),
        ENTRY( "AddOrReturnUnusedFieldL", CMTPbkContactItem::RunAddOrReturnUnUsedFieldL ),
        ENTRY( "Gettype", CMTPbkContactItem::RunGetTypeL ),
        ENTRY( "Getid", CMTPbkContactItem::RunGetIdL ),
        ENTRY( "Getcontactitem", CMTPbkContactItem::RunGetContactItemL ),
        ENTRY( "Getgroupsjoined", CMTPbkContactItem::RunGroupsJoinedL ),
        ENTRY( "Addallfields", CMTPbkContactItem::RunAddAllFieldsL ),
        ENTRY( "PbkFieldAt", CMTPbkContactItem::RunPbkFieldAtL ),
        ENTRY( "PbkFieldCount", CMTPbkContactItem::RunPbkFieldCountL ),
        ENTRY( "Testcontactitemequality", CMTPbkContactItem::RunEqualsTestL ),

        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//---------------------------------------------------------------
//----------------- Helper classes ---------------------------
//---------------------------------------------------------------

class MDefaultTestApi
    {
    public:
        /**
         * Sets aItem's default to aField. If aField is NULL removes the default.
         */
        virtual void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField) = 0;

        /**
         * Removes default setting from aItem.
         */
        virtual void RemoveDefault(CPbkContactItem& aItem) = 0;

        /**
         * Returns aItem's default field, NULL if not set.
         */
        virtual TPbkContactItemField* DefaultField(const CPbkContactItem& aItem) = 0;

        /**
         * Returns ETrue if aItem's default can be set to aField.
         */ 
        virtual TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField) = 0;
    };
    
/**
 * Default setting API class for phone number default.
 */
class TPhoneNumberDefaultTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TPhoneNumberDefaultTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultPhoneNumberFieldL(aField);
    }

void TPhoneNumberDefaultTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultPhoneNumberField();
    }

TPbkContactItemField* TPhoneNumberDefaultTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultPhoneNumberField();
    }

TBool TPhoneNumberDefaultTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().IsPhoneNumberField();
    }

/**
 * Default setting API class for MMS default.
 */
class TMmsDefaultTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TMmsDefaultTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultMmsFieldL(aField);
    }

void TMmsDefaultTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultMmsField();
    }

TPbkContactItemField* TMmsDefaultTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultMmsField();
    }

TBool TMmsDefaultTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().IsMmsField();
    }

/**
 * Default setting API class for SMS default.
 */
class TSmsDefaultTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TSmsDefaultTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultSmsFieldL(aField);
    }

void TSmsDefaultTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultSmsField();
    }

TPbkContactItemField* TSmsDefaultTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultSmsField();
    }

TBool TSmsDefaultTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().IsPhoneNumberField();
    }

/**
 * Default setting API class for email default.
 */
class TDefaultEmailTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TDefaultEmailTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultEmailFieldL(aField);
    }

void TDefaultEmailTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultEmailField();
    }

TPbkContactItemField* TDefaultEmailTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultEmailField();
    }

TBool TDefaultEmailTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().FieldId() == EPbkFieldIdEmailAddress;
    }
    
/**
 * Default setting API class for video number default.
 */   
class TDefaultVideoNumberTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TDefaultVideoNumberTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultVideoNumberFieldL(aField);
    }

void TDefaultVideoNumberTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultVideoNumberField();
    }

TPbkContactItemField* TDefaultVideoNumberTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultVideoNumberField();
    }

TBool TDefaultVideoNumberTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().IsPhoneNumberField();
    }
    
/**
 * Default setting API class for Poc default.
 */   
class TDefaultPocTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TDefaultPocTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultPocFieldL(aField);
    }

void TDefaultPocTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultPocField();
    }

TPbkContactItemField* TDefaultPocTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultPocField();
    }

TBool TDefaultPocTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().IsPocField();
    }    
    
/**
 * Default setting API class for VOIP default.
 */   
class TDefaultVoipTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TDefaultVoipTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultVoipFieldL(aField);
    }

void TDefaultVoipTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultVoipField();
    }

TPbkContactItemField* TDefaultVoipTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultVoipField();
    }

TBool TDefaultVoipTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().IsVoipField();
    }
    
/**
 * Default setting API class for EmailOverSms default.
 */   
class TDefaultEmailOverSmsTestApi : public MDefaultTestApi
    {
    void SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField);
    void RemoveDefault(CPbkContactItem& aItem);
    TPbkContactItemField* DefaultField(const CPbkContactItem& aItem);
    TBool IsSupportedDefault(const CPbkContactItem& aItem, const TPbkContactItemField& aField);
    };

void TDefaultEmailOverSmsTestApi::SetDefaultL(CPbkContactItem& aItem, TPbkContactItemField* aField)
    {
    aItem.SetDefaultEmailOverSmsFieldL(aField);
    }

void TDefaultEmailOverSmsTestApi::RemoveDefault(CPbkContactItem& aItem)
    {
    aItem.RemoveDefaultEmailOverSmsField();
    }

TPbkContactItemField* TDefaultEmailOverSmsTestApi::DefaultField(const CPbkContactItem& aItem)
    {
    return aItem.DefaultEmailOverSmsField();
    }

TBool TDefaultEmailOverSmsTestApi::IsSupportedDefault(const CPbkContactItem& /*aItem*/, const TPbkContactItemField& aField)
    {
    return aField.FieldInfo().IsEmailOverSmsField();
    }

/**
 * Fills contact item fields with test data.
 */
void CMTPbkContactItem::FillContactFieldsL(
        CPbkContactItem& aItem, 
        const TInt aBasePhoneNumber,
        const TBool aConstantNumber)
    {
    iFilledFields = 0;

    for (TInt i = 0; i < aItem.CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = aItem.CardFields()[i];
        
        if (field.FieldInfo().IsPhoneNumberField())
            {
            // Set a different number for each phonenumber;
            // just simply set the loop counter
            TInt number = aBasePhoneNumber;
            if (!aConstantNumber)
                {
                number = number+ i;
                }

            TBuf<KPhoneNumberLenght> phoneNumber;
            phoneNumber.Num(number);
            field.TextStorage()->SetTextL(phoneNumber);

            if ( field.FieldInfo().FieldId() == EPbkFieldIdPhoneNumberMobile )
                {
                iPhoneNumberHome  = phoneNumber;
                iPhoneNumberHomeIndex = i;
                break;
                }
            }
        else
            {
            switch (field.StorageType())
                {
                case KStorageTypeText:
                    {
                    field.TextStorage()->SetTextL(KFillText);
                    iFilledFields++;
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
                    User::Panic(KPanicText, KPanicUnsupportedFieldType);
                    }
                }
            }
        }
    }



//  METHODS

//---------------------------------------------------------------
//----------------- Private methods ---------------------------
//---------------------------------------------------------------

/**
 * Returns ETrue if aItem has no defaults set.
 */
TBool CMTPbkContactItem::NoDefaultsSet(const CPbkContactItem& aItem)
    {
    return !aItem.DefaultPhoneNumberField() 
        && !aItem.DefaultSmsField() && !aItem.DefaultMmsField() 
        && !aItem.DefaultEmailField() && !aItem.DefaultPocField() 
        && !aItem.DefaultVoipField() && !aItem.DefaultEmailOverSmsField() 
        && !aItem.DefaultVideoNumberField()
            ;
    }
    
/**
 * Get a field for defined fieldId.
 */
TPbkContactItemField* CMTPbkContactItem::FindFieldForModificationL(
	    CPbkContactItem& aItem,
	    const TPbkFieldId aFieldId,
        const TPbkFieldLocation& aType)
    {
    
    CPbkFieldInfo* field = iContactEngine->FieldsInfo().Find(aFieldId,aType);
    
    if ( field )
        {
        return aItem.AddOrReturnUnusedFieldL(*field);
        }
    else
        {
        return NULL;    
        }
    
    }

/**
* Get maximun lenght of a field for defined fieldId.
*/    
TInt CMTPbkContactItem::FindFieldMaximumLength(TPbkFieldId aFieldId)
    {
	CPbkFieldInfo *fi = iContactEngine->FieldsInfo().Find(aFieldId);
	if(fi)
        {
        TInt maxlen = fi->MaxLength();
        
		return maxlen;
        }
	else
        {
		return 0;
        }
    }
    
/**
* Add fields with values. Test field lenghts.
*/
TBool CMTPbkContactItem::AddFieldWithValueL(
	    CPbkContactItem& aDestItem,
	    TPbkFieldId aFieldId,
	    const TDesC &aValue,
        const TPbkFieldLocation& aType)
    {
           
	TPbkContactItemField* f;
	f = FindFieldForModificationL(aDestItem, aFieldId, aType);
	if(f)
	    {
		// Copy the property value
		HBufC *buf = HBufC::NewL(aValue.Length());
		CleanupStack::PushL(buf);
		TPtr buffer = buf->Des();
		buffer = aValue;

		
		TInt maxlen = FindFieldMaximumLength(f->FieldInfo().FieldId());
	
	    //field lenght should not be narrowed
		switch ( f->FieldInfo().FieldId() )
		    {
		    case EPbkFieldIdPhoneNumberStandard:    
            case EPbkFieldIdPhoneNumberHome:        
            case EPbkFieldIdPhoneNumberWork: 
            case EPbkFieldIdPhoneNumberMobile:
            case EPbkFieldIdPhoneNumberVideo:       
            case EPbkFieldIdFaxNumber:              
            case EPbkFieldIdPagerNumber:
		        {
		        //Phonebook uses value from central repository so no need to test
		        break;  
		        }
		    case EPbkFieldIdLastName:
		    case EPbkFieldIdFirstName:
		    case EPbkFieldIdSecondName:
		    case EPbkFieldIdJobTitle:
		    case EPbkFieldIdCompanyName:
		        {
		        ASSERT( maxlen >= KNameFieldMaxLenght );
		        break;
		        }
		    case EPbkFieldIdEmailAddress:
		        {
		        ASSERT( maxlen >= KEmailFieldMaxLenght );
		        break;
		        }
		    case EPbkFieldIdPostalAddress:
		        {
		        ASSERT( maxlen >= KAddressFieldMaxLenght );
		        break;
		        }
		    case EPbkFieldIdPOBox:
		    case EPbkFieldIdPostalCode:
		        {
		        ASSERT( maxlen >= KPostalCodeFieldMaxLenght );
		        break;
		        }
		    case EPbkFieldIdExtendedAddress:
		    case EPbkFieldIdStreetAddress:
		    case EPbkFieldIdCity:
		    case EPbkFieldIdState:
		    case EPbkFieldIdCountry:
		        {
		        ASSERT( maxlen >= KAddressSubfieldFieldMaxLenght );
		        break;
		        }
		    case EPbkFieldIdURL:
		        {		            
		        ASSERT( maxlen >= KURLFieldMaxLenght );
		        break;
		        }
		   case EPbkFieldIdDTMFString:
		        {		            
		        ASSERT( maxlen >= KDTMFFieldMaxLenght );
		        break;
		        }
		   case EPbkFieldIdNote:
		        {		            
		        ASSERT( maxlen >= KNoteFieldMaxLenght );
		        break;
		        }
		   case EPbkFieldIdPicture:
		   case EPbkFieldIdThumbnailImage:
		   case EPbkFieldIdPersonalRingingToneIndication:
		   case EPbkFieldIdLocationIdIndication:
		        {		            
		        ASSERT( maxlen >= KFileNameFieldMaxLenght );
		        break;
		        }
		   case EPbkFieldIdVOIP:
		   case EPbkFieldIdPushToTalk:
		   case EPbkFieldIdSIPID:
		        case EPbkFieldIdShareView:
		        {		            
		        //VOIP, Push-to-Talk and SIP fields lenght are equals
		        ASSERT( maxlen >= KVOIPFieldMaxLenght );
		        break;
		        }
		   case EPbkFieldIdPrefix:
		   case EPbkFieldIdSuffix:
		        {		            
		        ASSERT( maxlen >= KIdSuffixMaxLenght );
		        break;
		        }
		   case EPbkFieldIdDate:
		        {		            
		        ASSERT( maxlen == 0 );
		        break;
		        }
		   default:
		        {
		        //FAIL_TEST( "Unknown field type" ); 
		        ASSERT(0);
		        }
	    }
		
		// Clip the property value if it does not fit the field
		if(maxlen)
            {
			if(maxlen < aValue.Length())
				buffer.SetLength(maxlen);
            }

        if (f->ItemField().StorageType() == KStorageTypeText)
            {
		    CContactTextField* tf = f->ItemField().TextStorage();
	        tf->SetTextL(buffer);
		    }
        CleanupStack::PopAndDestroy(buf); 
		return ETrue;
    	}
	return EFalse;
    } 
 
/**
 * Invariant test.
 */
TBool CMTPbkContactItem::Invariant(CPbkContactEngine& aEngine, const CPbkContactItem& aItem)
    {
    const CPbkFieldArray& fields = aItem.CardFields();

    // Check field counts
    if (aItem.ContactItem().CardFields().Count() < fields.Count())
        {
        return EFalse;
        }
        
    TInt phoneDefaultCount = 0;
    TInt smsDefaultCount = 0;
    TInt emailDefaultCount = 0;
    TInt voiceTagFieldCount = 0;

    // Scan all fields
    TInt i;
    for (i = 0; i < fields.Count(); ++i)
        {
        const TPbkContactItemField& pbkField = fields[i];

        // Check field info for all fields
        const CPbkFieldInfo* fieldInfo = aEngine.FieldsInfo().Find(pbkField.ItemField());
        if (!fieldInfo || !fieldInfo->IsSame(pbkField.FieldInfo()))
            return EFalse;

        // Search field from iItem's fieldset

        TInt j;
        for (j = 0; j < aItem.ContactItem().CardFields().Count(); ++j)
            {
            CContactItemField& field = aItem.ContactItem().CardFields()[j];
            // Check if field found
            if(&pbkField.ItemField() == &field)
                {
                break;
                }
            }
        // field must be found
        if (j == aItem.ContactItem().CardFields().Count())
            return EFalse;

        // Update default field counts
        TInt prefCount = 0;
        TInt smsCount = 0;
        TInt voiceTagCount = 0;
        const CContentType& contentType = pbkField.ItemField().ContentType();
        for (TInt t = 0; t < contentType.FieldTypeCount(); ++t)
            {
            if (contentType.FieldType(t) == KUidContactFieldVCardMapPREF)
                ++prefCount;
            else if (contentType.FieldType(t) == KUidContactFieldSms) 
                ++smsCount;
            else if (contentType.FieldType(t) == KUidContactsVoiceDialField) 
                ++voiceTagCount;
            }
        if (prefCount > 2) return EFalse;
        if (smsCount > 1) return EFalse;
        if (prefCount == 2 && smsCount < 1) return EFalse;
        if (voiceTagCount > 1) return EFalse;

        if (pbkField.FieldInfo().IsPhoneNumberField() && prefCount >= 1)
            {
            if (smsCount == 1)
                ++smsDefaultCount;
            if ((prefCount==1 && smsCount==0) || (prefCount==2 && smsCount==1))
                ++phoneDefaultCount;
            }
        if (pbkField.FieldInfo().FieldId() == EPbkFieldIdEmailAddress && prefCount==1)
            ++emailDefaultCount;
        if (voiceTagCount > 0)
            ++voiceTagFieldCount;
        }

    // Check default counts: only one default per category allowed
    if (phoneDefaultCount > 1) return EFalse;
    if (smsDefaultCount > 1) return EFalse;
    if (emailDefaultCount > 1) return EFalse;
    if (voiceTagFieldCount > 1) return EFalse;

    // Test field sorting
    for (i = 0; i < fields.Count()-1; ++i)
        {
        if (fields[i].Compare(fields[i+1]) > 0)
            return EFalse;
        }

    return ETrue;
    }

/**
 * Generic Default setting test method.
 */
TBool CMTPbkContactItem::DoDefaultTestL()
    {
    TBool result = ETrue;

    for (TInt i=0; i < iContactItem->CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = iContactItem->CardFields()[i];

        TRAPD(err, iTestApi->SetDefaultL(*iContactItem, &field));
        // Invariant should always be ok
        if (!Invariant(*iContactEngine, *iContactItem))
            {
            result = EFalse;
            break;
            }

        if (err == KErrNotSupported)
            {
            if (iTestApi->IsSupportedDefault(*iContactItem, field))
                {
                //"Setting function triggered a KErrNotSupported leave although the field type is supported."
                result = EFalse;
                break;
                }
            }
        else if (err == KErrNone)
            {
            if (!iTestApi->IsSupportedDefault(*iContactItem, field))
                {
                //"Able to set the default although it is not supported."
                result = EFalse;
                break;
                }

            // Test that default is set correctly
            if (iTestApi->DefaultField(*iContactItem) != &field)
                {
                //"Default set to wrong field"
                result = EFalse;
                break;
                }
            }
        else
            {
            // If the operation leaves, default should not be set to field
            if (iTestApi->DefaultField(*iContactItem) == &field)
                {
                //"Default was set although the setting function leaved."
                result = EFalse;
                break;
                }

            // Rethrow the error for further handling
            User::Leave(err);
            }
            
        //Should not to leave if field is NULL.
        TRAPD(error, iTestApi->SetDefaultL(*iContactItem, NULL)); 
        if (error != KErrNone)
            {
            result = EFalse;
            }
        
        }
    // Reset the default
    iTestApi->RemoveDefault(*iContactItem);
    
    if (iTestApi->DefaultField(*iContactItem))
        {
        //"Default is set after resetting it."
        result = EFalse;
        }
    return result;
    }

/**
 * Test setting all defaults to same field.
 */
TBool CMTPbkContactItem::DoAllDefaultTestL()
    {
    // Loop throgh all fields
    TBool result = ETrue;
    CPbkFieldArray& fields = iContactItem->CardFields();
    for (TInt i=0; i < fields.Count(); ++i)
        {
        TPbkContactItemField& field = fields[i];

        // Set all phone number defaults to each phone number field
        if (field.FieldInfo().IsPhoneNumberField())
            {
            iContactItem->SetDefaultPhoneNumberFieldL(&field);
            iContactItem->SetDefaultSmsFieldL(&field);
            iContactItem->SetDefaultVideoNumberFieldL(&field);
            iContactItem->SetVoiceTagFieldL(&field);
            iContactItem->SetDefaultPocFieldL(&field);
            iContactItem->SetDefaultVoipFieldL(&field);
            //pager and fax numbers are not supported to MMS and Email over SMS
            if (field.FieldInfo().FieldId() != EPbkFieldIdFaxNumber &&
                    field.FieldInfo().FieldId() != EPbkFieldIdPagerNumber)
                {
                iContactItem->SetDefaultMmsFieldL(&field);
                iContactItem->SetDefaultEmailOverSmsFieldL(&field);
                }
            // Verify Setting
            if (!Invariant(*iContactEngine, *iContactItem))
                {
                //"Invariant failed after setting all phone number defaults"
                result = EFalse;
                break;
                }
            
            // Check defaults fields            
            TPbkContactItemField* pbkContactItemField= iContactItem->DefaultPhoneNumberField();
            if (!pbkContactItemField || !field.IsSame(*pbkContactItemField))
                {
                result = EFalse;
                break;
                }            
            
            pbkContactItemField = iContactItem->DefaultSmsField();
            if (!pbkContactItemField || !field.IsSame(*pbkContactItemField))
                {
                result = EFalse;
                break;
                }
            
            pbkContactItemField = iContactItem->DefaultVideoNumberField();
            if (!pbkContactItemField || !field.IsSame(*pbkContactItemField))
                {
                result = EFalse;
                break;
                }
            
            // Voicetag field is not set if SIND is enabled
            pbkContactItemField = iContactItem->VoiceTagField();
            if (pbkContactItemField && !field.IsSame(*pbkContactItemField))
                {
                result = EFalse;
                break;
                }
            pbkContactItemField = iContactItem->DefaultPocField();
            if (!pbkContactItemField || !field.IsSame(*pbkContactItemField))
                {
                result = EFalse;
                break;
                }
            pbkContactItemField = iContactItem->DefaultVoipField();
            if (!pbkContactItemField || !field.IsSame(*pbkContactItemField))
                {
                result = EFalse;
                break;
                }
            
            //pager and fax numbers are not supported to MMS and Email over SMS
            if (field.FieldInfo().FieldId() != EPbkFieldIdFaxNumber &&
                    field.FieldInfo().FieldId() != EPbkFieldIdPagerNumber)
                {
                TPbkContactItemField* mmsContactItemField = iContactItem->DefaultMmsField();
                if (!mmsContactItemField ||!field.IsSame(*mmsContactItemField))
                    {
                    //"Failed to set all phone number defaults to the same field"
                    result = EFalse;
                    break;
                    }
                TPbkContactItemField* emailContactItemField = iContactItem->DefaultEmailOverSmsField();
                if ( !emailContactItemField ||!field.IsSame(*emailContactItemField))
                    {
                    //"Failed to set all phone number defaults to the same field"
                    result = EFalse;
                    break;                    
                    }
                }
            }

        // Set the email default to all email fields
        if (field.FieldInfo().FieldId() == EPbkFieldIdEmailAddress)
            {
            iContactItem->SetDefaultEmailFieldL(&field);
            // Verify
            if (!Invariant(*iContactEngine, *iContactItem))
                {
                //"Invariant failed after setting email default"
                result = EFalse;
                break;
                }

            TPbkContactItemField* emailContactItemField = iContactItem->DefaultEmailField();                
            if (!emailContactItemField ||!field.IsSame(*emailContactItemField))
                {
                //"Failed to set default email field"
                result = EFalse;
                break;
                }
            }

        // Remove (and verify removal of) all defaults
        iContactItem->RemoveDefaultPhoneNumberField();
        if (iContactItem->DefaultPhoneNumberField() || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Failed to remove default phone number field"
            result = EFalse;
            break;
            }
        iContactItem->RemoveDefaultSmsField();
        if (iContactItem->DefaultSmsField() || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Failed to remove default SMS field"
            result = EFalse;
            break;
            }
        iContactItem->RemoveDefaultVideoNumberField();
        if (iContactItem->DefaultVideoNumberField() || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Failed to remove default video number field"
            result = EFalse;
            break;
            }
        iContactItem->RemoveVoiceTagField();
        if (iContactItem->VoiceTagField() || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Failed to remove voice tag field"
            result = EFalse;
            break;
            }
        iContactItem->RemoveDefaultEmailField();
        if (iContactItem->DefaultEmailField() || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Failed to remove default email field"
            result = EFalse;
            break;
            }
        iContactItem->RemoveDefaultPocField();
        if (iContactItem->DefaultPocField() || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Failed to remove default Poc field"
            result = EFalse;
            break;
            }
        iContactItem->RemoveDefaultVoipField();
        if (iContactItem->DefaultVoipField() || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Failed to remove default VOIP field"
            result = EFalse;
            break;
            }
        //pager and fax numbers are not supported to MMS and Email over SMS
        if (field.FieldInfo().FieldId() != EPbkFieldIdFaxNumber &&
            field.FieldInfo().FieldId() != EPbkFieldIdPagerNumber)
            {
            iContactItem->RemoveDefaultMmsField();
            if (iContactItem->DefaultMmsField() || !Invariant(*iContactEngine, *iContactItem))
                {
                //"Failed to remove default MMS field"
                result = EFalse;
                break;
                }
            iContactItem->RemoveDefaultEmailOverSmsField();
            if (iContactItem->DefaultEmailOverSmsField() || !Invariant(*iContactEngine, *iContactItem))
                {
                //"Failed to remove default EmailOverSms field"
                result = EFalse;
                break;
                }
            }
        }
        
    return result;
    }
    
/**
 * Test removing all fields from a contact (from the end).
 */
TBool CMTPbkContactItem::DoRemoveAllFromEndTest()
    {
    TBool result = ETrue;    
    const TInt count = iContactItem->CardFields().Count();
    for (TInt i = count-1; i >= 0; --i)
        {
        iContactItem->RemoveField(i);
        if (iContactItem->CardFields().Count() != i || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Removing field failed"
            result = EFalse;
            break;
            }
        }
    if (iContactItem->CardFields().Count() != 0 || !Invariant(*iContactEngine, *iContactItem))
        {
        //"Field count != 0 after removing all fields"
        result = EFalse;
        }

    return result;    
    }

/**
 * Test removing all fileds from a contact (from the start).
 */
TBool CMTPbkContactItem::DoRemoveAllFromStartTest()
    {
    TBool result = ETrue;    
    const TInt count = iContactItem->CardFields().Count();
    for (TInt i = 1; i <= count; ++i)
        {
        iContactItem->RemoveField(0);
        if (iContactItem->CardFields().Count() != count-i || !Invariant(*iContactEngine, *iContactItem))
            {
            //"Removing field failed"
            result = EFalse;
            break;
            }
        }
    if (iContactItem->CardFields().Count() != 0 || !Invariant(*iContactEngine, *iContactItem))
        {
        //"Field count != 0 after removing all fields"));
        result = EFalse;
        }

    return result;    
    }
    
/**
 * Test adding fields to a contact.
 */
TBool CMTPbkContactItem::DoAddFieldTestL()
    {
    TBool result = ETrue;

    // Loop through all field types
    const CPbkFieldsInfo& fieldTypes = iContactEngine->FieldsInfo();
    for (TInt i = 0; i < fieldTypes.Count(); ++i)
        {
        CPbkFieldInfo& fieldType = *fieldTypes[i];
        if (fieldType.Multiplicity() == EPbkFieldMultiplicityMany ||
            iContactItem->FindField(fieldType))
            {
            const TInt countBefore = iContactItem->CardFields().Count();
            TPbkContactItemField* newField = NULL;
            TRAPD(err, newField = &iContactItem->AddFieldL(fieldType));
            if (err == KErrNone)
                {
                if (!newField)
                    {
                    //"New field is NULL"
                    result = EFalse;
                    break;
                    }
                if (iContactItem->CardFields().Count() != countBefore+1)
                    {
                    //"Incorrect field count after add"
                    result = EFalse;
                    break;
                    }
                if (iContactItem->FindFieldIndex(*newField) < 0)
                    {
                    //"New field not found in contact"
                    result = EFalse;
                    break;
                    }
                if (!newField->FieldInfo().IsSame(fieldType))
                    {
                    //"New field has wrong FieldInfo"
                    result = EFalse;
                    break;
                    }
                if (!Invariant(*iContactEngine, *iContactItem))
                    {
                    //"Invariant failed after AddItemL"
                    result = EFalse;
                    break;
                    }
                }
            else 
                {
                // AddFieldL leaved, contact shouldn't have changed
                if (iContactItem->CardFields().Count() != countBefore)
                    {
                    //"ERROR: AddFieldL leaved but field count changed"
                    result = EFalse;
                    }
                // Rethrow the error
                User::Leave(err);
                }
            }
        }
    return result;
    }
    
/**
* Test updating field set
*/
TBool CMTPbkContactItem::DoUpdateFieldSetTestL()
    {
    TBool result= ETrue; 
       
    TRAPD(err, iContactItem->UpdateFieldSetL( iContactEngine->FieldsInfo() ));
    if (err != KErrNone)
        {
        result = EFalse;
        }
       
    return result;
    }

//---------------------------------------------------------------
//----------------- TEST CASE METHODS ---------------------------
//---------------------------------------------------------------
/**
* Set nothing
*/
void CMTPbkContactItem::Setup()
    {    
    
    }
/**
 * Set the NewL test case
 */ 
void CMTPbkContactItem::SetNewLTestL()
    {
    CContactCard* card = CContactCard::NewLC();

    // NewL implementation calls NewLC -> it's enough to test NewL.
    iContactItem = CPbkContactItem::NewL(card, iContactEngine->FieldsInfo(),
        iContactEngine->ContactNameFormat());
    CleanupStack::Pop(card);    
    }    

/**
 * Teardown nothing
 */ 
void CMTPbkContactItem::Teardown()
    {
    }

/**
 * Teardown the NewL test case
 */ 
void CMTPbkContactItem::TeardownContactItem()
    {
    delete iContactItem;
    iContactItem = NULL;
    }

/**
 * create an empty contact.
 */ 
void CMTPbkContactItem::SetEmptyContactL()
    {
    iContactItem = iContactEngine->CreateEmptyContactL();
    }
    
/**
 * create a filled contact.
 */ 
void CMTPbkContactItem::SetFilledContactL()
    {
    SetEmptyContactL();
    FillContactFieldsL(*iContactItem,0);
    }

/**
 * create contact with multiple workphonenumbers.
 */ 
void CMTPbkContactItem::SetMultiWorkPhoneContactL()
    {
    SetFilledContactL();
    
    // add work phone numbers
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberWork, KNumber1, EPbkFieldLocationWork);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberWork, KNumber2, EPbkFieldLocationWork);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberWork, KNumber3, EPbkFieldLocationWork);
    }
    
/**
 * create contact with multiple phonenumbers of different fields.
 * Is NOT based on filled contact.
 */ 
void CMTPbkContactItem::SetMultiPhoneContactL()
    {
    SetEmptyContactL();
    
    // add work phone numbers
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberWork, KNumber1, EPbkFieldLocationWork);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberWork, KNumber2, EPbkFieldLocationWork);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberWork, KNumber3, EPbkFieldLocationWork);
    
    // add home phone numbers
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberStandard, KNumber1, EPbkFieldLocationNone);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberStandard, KNumber2, EPbkFieldLocationNone);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberStandard, KNumber3, EPbkFieldLocationNone);

    // add mobile phone numbers
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberMobile, KNumber1);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberMobile, KNumber2);
	AddFieldWithValueL(*iContactItem, EPbkFieldIdPhoneNumberMobile, KNumber3);    
    }

/**
* Test size of CPbkContactItem class
*/
TInt CMTPbkContactItem::TestSize()
    {
    	Setup();
    ASSERT( sizeof(CPbkContactItem) >= KClassSize );
    Teardown();
    return KErrNone;
    }



/**
 * Run the CreateEmptyContact test case
 */ 
TInt CMTPbkContactItem::RunCreateEmptyContact()
    {
    SetEmptyContactL();
    ASSERT(Invariant(*iContactEngine, *iContactItem));
    ASSERT(NoDefaultsSet(*iContactItem));
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Set the DefaultPhone test case
 */ 
void CMTPbkContactItem::SetDefaultPhoneL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TPhoneNumberDefaultTestApi;
    }
    
/**
 * Set the DefaultMms test case
 */ 
void CMTPbkContactItem::SetDefaultMmsL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TMmsDefaultTestApi;
    }

/**
 * Set the DefaultSms test case
 */ 
void CMTPbkContactItem::SetDefaultSmsL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TSmsDefaultTestApi;
    }
    
/**
 * Set the DefaultEmail test case
 */ 
void CMTPbkContactItem::SetDefaultEmailL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TDefaultEmailTestApi;
    }
    
/**
 * Set the VideoNumberPhone test case
 */ 
void CMTPbkContactItem::SetDefaultVideoNumberL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TDefaultVideoNumberTestApi;
    }

/**
 * Set the DefaultPoc test case
 */ 
void CMTPbkContactItem::SetDefaultPocL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TDefaultPocTestApi;
    }
    
/**
 * Set the DefaultVoip test case
 */ 
void CMTPbkContactItem::SetDefaultVoipL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TDefaultVoipTestApi;
    }
    
/**
 * Set the DefaultEmailOverSms test case
 */ 
void CMTPbkContactItem::SetDefaultEmailOverSmsL()
    {
    SetEmptyContactL();
    iTestApi = new (ELeave) TDefaultEmailOverSmsTestApi;
    }

/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTestL()
    {
    SetDefaultPhoneL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }

/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTest1L()
    {
    SetDefaultMmsL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }

/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTest2L()
    {
    SetDefaultSmsL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }


/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTest3L()
    {
    SetDefaultEmailL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }


/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTest4L()
    {
    SetDefaultVideoNumberL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }


/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTest5L()
    {
    SetDefaultPocL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }


/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTest6L()
    {
    SetDefaultVoipL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }


/**
 * Run a 'default' test case
 */ 
TInt CMTPbkContactItem::RunDefaultTest7L()
    {
    SetDefaultEmailOverSmsL();
    ASSERT(DoDefaultTestL());
    TeardownDefaultTest();
    return KErrNone;
    }


/**
 * Teardown a 'default' test case
 */ 
void CMTPbkContactItem::TeardownDefaultTest()
    {
    TeardownContactItem();
    
    delete iTestApi;
    iTestApi =NULL;
    }
    
/**
 * Run AllDefault test case
 */ 
TInt CMTPbkContactItem::RunAllDefaultL()
    {
    SetEmptyContactL();
    ASSERT(DoAllDefaultTestL());
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run RemoveAllFromEnd test case
 */ 
TInt CMTPbkContactItem::RunRemoveAllFromEnd()
    {
    SetEmptyContactL();
    ASSERT(DoRemoveAllFromEndTest());
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run RemoveAllFromStart test case
 */ 
TInt CMTPbkContactItem::RunRemoveAllFromStart()
    {
    SetEmptyContactL();
    ASSERT(DoRemoveAllFromStartTest());
    TeardownContactItem();
    return KErrNone;
    }
    
/**
 * Run AddFields test case
 */ 
TInt CMTPbkContactItem::RunAddFieldsL()
    {
    SetEmptyContactL();
    ASSERT(DoAddFieldTestL());
    TeardownContactItem();
    return KErrNone;
    }
    
/**
 * Run UpdateFieldSetL test case
 */ 
TInt CMTPbkContactItem::RunUpdateFieldSetL()
    {
    SetFilledContactL();
    ASSERT(DoUpdateFieldSetTestL());
    TeardownContactItem();
    return KErrNone;
    } 
 
/**
 * Run 'Get contact title' testcase
 */ 
TInt CMTPbkContactItem::RunGetContactTitleL()
    {
    SetFilledContactL();
    _LIT(KSpace," ");
    TChar nameSeparator = iContactEngine->NameSeparator();
      
    HBufC* titleHeap = HBufC::NewLC(KFillText().Length()*2 + KSpace().Length()); 
    TPtr title = titleHeap->Des();

    title.Append( KFillText );
    if ( nameSeparator != 0 )
        {
        title.Append( nameSeparator );
        }
    title.Append( KSpace );
    title.Append( KFillText );
    
    HBufC* resultTitle = iContactItem->GetContactTitleL();
    CleanupStack::PushL(resultTitle);
    ASSERT(resultTitle->Compare(*titleHeap) == 0);
    CleanupStack::PopAndDestroy(resultTitle); 
    CleanupStack::PopAndDestroy(titleHeap);
    
    CPbkContactItem* emptyItem = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyItem);
   
    HBufC* resultTitle2 = emptyItem->GetContactTitleL();
    CleanupStack::PushL(resultTitle2);

    // Title must be qtn_phob_unnamed
    ASSERT(resultTitle2->Compare( iContactEngine->UnnamedTitle() ) == 0);
    
    CleanupStack::PopAndDestroy(resultTitle2);
    CleanupStack::PopAndDestroy(emptyItem);
    TeardownContactItem();    
    return KErrNone;
    }
    
/**
 * Run 'Get contact title or null' testcase
 */ 
TInt CMTPbkContactItem::RunGetContactTitleOrNullL()
    {
    SetFilledContactL();
    _LIT(KSpace," ");
    TChar nameSeparator = iContactEngine->NameSeparator();
    
    HBufC* titleHeap = HBufC::NewLC(KFillText().Length()*2 + KSpace().Length()); 
    TPtr title = titleHeap->Des();

    title.Append(KFillText);
    if ( nameSeparator != 0 )
        {
        title.Append( nameSeparator );
        }
    title.Append(KSpace);
    title.Append(KFillText);
    
    HBufC* resultTitle = iContactItem->GetContactTitleOrNullL();
    CleanupStack::PushL(resultTitle);

    ASSERT(resultTitle->Compare(*titleHeap) == 0);
    
    CleanupStack::PopAndDestroy(resultTitle); 
    CleanupStack::PopAndDestroy(titleHeap);
    
    CPbkContactItem* emptyItem = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyItem);

    // Title must be null
    ASSERT(!emptyItem->GetContactTitleOrNullL());
    CleanupStack::PopAndDestroy(emptyItem);
    TeardownContactItem();
    return KErrNone;
    }
    
/**
 * Run 'Get contact icon index' testcase
 */ 
TInt CMTPbkContactItem::RunGetContactIconIndexL()
    {
 	SetFilledContactL();
    //test no icon
    TPbkIconId iconIdx = iContactItem->ContactIconIdL();    
    ASSERT(iconIdx == EPbkNullIconId);    
    
    //set default phone number
    CPbkFieldArray& fields = iContactItem->CardFields();
    for (TInt i=0; i < fields.Count(); ++i)
        {
        TPbkContactItemField& field = fields[i];

        if (field.FieldInfo().IsPhoneNumberField())
            {
            iContactItem->SetDefaultPhoneNumberFieldL(&field);
            break;
            }
        }
        
    //test icon appears   
    iconIdx = iContactItem->ContactIconIdL();   
    ASSERT(iconIdx != EPbkNullIconId); 
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'Find field plain' testcase
 */ 
TInt CMTPbkContactItem::RunFindFieldPlainL()
    {
    SetFilledContactL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);
    
    const TInt baseNumber = 123321; 
    FillContactFieldsL(*item, baseNumber);
    
    TPbkContactItemField* field1 = item->FindField(EPbkFieldIdPhoneNumberMobile);    
    ASSERT(field1 && field1->FieldInfo().FieldId()==EPbkFieldIdPhoneNumberMobile);
    
    ASSERT(field1->TextStorage()->Text() == iPhoneNumberHome);

    // The basic filled item does not contain Thummbnail field
    TPbkContactItemField* field2 = iContactItem->FindField(EPbkFieldIdThumbnailImage);    
    ASSERT(!field2);
    
    CleanupStack::PopAndDestroy(item);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindField indexed' testcase
 */ 
TInt CMTPbkContactItem::RunFindFieldIndexedL()
    {
    SetMultiWorkPhoneContactL();
    TInt index = 0;
    TInt count = 0;
    do
        {
        TPbkContactItemField* field = iContactItem->FindField(
            EPbkFieldIdPhoneNumberWork, index);
        if (index > 0)
            {
            index++;
            count++;
            switch (count)
                {
                case 1:
                    {
                    ASSERT(field->TextStorage()->Text() == KNumber1);
                    break;
                    }
                case 2:
                    {
                    ASSERT(field->TextStorage()->Text() == KNumber2);
                    break;
                    }
                case 3:
                    {
                    ASSERT(field->TextStorage()->Text() == KNumber3);
                    break;
                    }
                default:
                    {
                    // do nothing
                    break;
                    }
                }
            }
        else
            {
            ASSERT(!field);
            }

        } while (index > 0); 
        
    // count should be 3; one default from the basic 
    // filled contact and three separately added fields
    ASSERT(count == 3);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindField using info' testcase
 */ 
TInt CMTPbkContactItem::RunFindFieldWithInfoL()
    {    
    SetFilledContactL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);
    
    const TInt baseNumber = 123321; 
    FillContactFieldsL(*item, baseNumber);

    CPbkFieldInfo* homeField = iContactEngine->FieldsInfo().Find(EPbkFieldIdPhoneNumberMobile);
    TPbkContactItemField* field1 = item->FindField(*homeField);    
    
    ASSERT(field1 && field1->FieldInfo().FieldId()==EPbkFieldIdPhoneNumberMobile);
    ASSERT(field1->TextStorage()->Text() == iPhoneNumberHome);

    // The basic filled item does not contain Thummbnail field
	CPbkFieldInfo* thumbField = iContactEngine->FieldsInfo().Find(EPbkFieldIdThumbnailImage);
    TPbkContactItemField* field2 = iContactItem->FindField(*thumbField);    
    ASSERT(!field2);
    
    CleanupStack::PopAndDestroy(item);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindField with indexed info' testcase
 */ 
TInt CMTPbkContactItem::RunFindFieldIndexedInfoL()
    {
    SetMultiWorkPhoneContactL();
    CPbkFieldInfo* workField = iContactEngine->FieldsInfo().Find(EPbkFieldIdPhoneNumberWork);

    TInt index = 0;
    TInt count = 0;
    do
        {
        TPbkContactItemField* field = iContactItem->FindField(*workField, index);
        if (index > 0)
            {
            index++;
            count++;
            switch (count)
                {
                case 1:
                    {
                    ASSERT(field->TextStorage()->Text() == KNumber1);
                    break;
                    }
                case 2:
                    {
                    ASSERT(field->TextStorage()->Text() == KNumber2);
                    break;
                    }
                case 3:
                    {
                    ASSERT(field->TextStorage()->Text() == KNumber3);
                    break;
                    }
                default:
                    {
                    // do nothing
                    break;
                    }
                }
            }
        else
            {
            ASSERT(!field);
            }

        } while (index > 0); 
    
    // count should be 3; one default from the basic 
    // filled contact and three separately added fields
    ASSERT(count == 3);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindField index' testcase
 */ 
TInt CMTPbkContactItem::RunFindFieldIndex()
    {
    SetFilledContactL();
    TPbkContactItemField* homeField = iContactItem->FindField(EPbkFieldIdPhoneNumberMobile);    
    ASSERT(iContactItem->FindFieldIndex(*homeField) == iPhoneNumberHomeIndex);

    // try a new field - should no be found
    TPbkContactItemField notField;    
    ASSERT(iContactItem->FindFieldIndex(notField) == -1);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindContactItemFieldIndex' testcase
 */ 
TInt CMTPbkContactItem::RunFindContactItemFieldIndex()
    {
    SetFilledContactL();
    CContactItem& cItem = iContactItem->ContactItem(); 
    
    CContactItemField& cField = cItem.CardFields()[iPhoneNumberHomeIndex];
    
    // Again,the homephone index and value are based on the 
    // logic of FillContactFieldsL 
    ASSERT(cField.TextStorage()->Text() == iPhoneNumberHome);

    TInt cIndex = iContactItem->FindContactItemFieldIndex(cField);
    
    ASSERT(cIndex == iPhoneNumberHomeIndex);
    
    // try a new field - should no be found
    CContactItemField* notField = NULL;    
    cIndex = iContactItem->FindContactItemFieldIndex(*notField);
   
    ASSERT(cIndex == -1);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindNextFieldWithPhoneNumberExact' testcase.
 * Find exact matches.
 */ 
TInt CMTPbkContactItem::RunFindNextFieldWithPhoneNumberExact()
    {
    	SetMultiPhoneContactL();
    SetMultiPhoneContactL();
    TInt index = 0;
    TInt count = 0;
    const TInt noOfDigits = 0;
    TPbkContactItemField* field;
    
    do
        {
        field = iContactItem->FindNextFieldWithPhoneNumber(KNumber2, noOfDigits, index);
        if (index > 0)
            {
            index++;
            count++;
            ASSERT(field->TextStorage()->Text() == KNumber2);
            }
        } while (index > 0);
        
        // count = 3 derived from SetMultiPhoneContactL
        ASSERT(count == 3);
    TeardownContactItem();
    return KErrNone;
    
    }

/**
 * Run 'FindNextFieldWithPhoneNumberDigits' testcase.
 * Find matches with reduced digits matches.
 */ 
TInt CMTPbkContactItem::RunFindNextFieldWithPhoneNumberDigits()
    {
    	SetMultiPhoneContactL();
    TInt index = 0;
    TInt count = 0;
    const TInt noOfDigits = 6;
    TPbkContactItemField* field;
    
    do
        {
        field = iContactItem->FindNextFieldWithPhoneNumber(KNumber2, noOfDigits, index);
        if (index > 0)
            {
            index++;
            count++;
            TBool number1Found = (field->TextStorage()->Text() == KNumber1);
            TBool number2Found = (field->TextStorage()->Text() == KNumber2);

            ASSERT(number1Found || number2Found);
            
            }
        else
            {
            ASSERT(!field);
            }
        } while (index > 0);
        
        // count = 6 derived from SetMultiPhoneContactL
        ASSERT(count == 6);
    TeardownContactItem();
    return KErrNone;
    }


/**
 * Run 'FindNextFieldWithPhoneNumberNot' testcase.
 * Actually don't find matches.
 */ 
TInt CMTPbkContactItem::RunFindNextFieldWithPhoneNumberNot()
    {
    	SetMultiPhoneContactL();
    TPbkContactItemField* field;    
    _LIT(KNotExisting,"876876");
    
    // First exact match
    TInt index = 0;
    TInt noOfDigits = 0;
    field = iContactItem->FindNextFieldWithPhoneNumber(KNotExisting, noOfDigits, index);   
    ASSERT(!field);
    ASSERT(index == -1);
    
    // One digit match match
    index = 0;
    noOfDigits = 1;
    field = iContactItem->FindNextFieldWithPhoneNumber(KNotExisting, noOfDigits, index);   
    ASSERT(!field);
    ASSERT(index == -1);

    // Large number of digits
    index = 0;
    noOfDigits = 12;
    _LIT(KShortNumber,"5001"); // note: this end is common for all defined numbers
    field = iContactItem->FindNextFieldWithPhoneNumber(KShortNumber, noOfDigits, index);   
    ASSERT(!field);
    ASSERT(index == -1);


    // with empty string -> actually should find the Fax
    // field which is empty (created with iContactEngine->CreateEmptyContactL)
    index = 0;
    noOfDigits = 0;
    TBool result;
    do
        {
        field = iContactItem->FindNextFieldWithPhoneNumber(KNullDesC, noOfDigits, index);            
        if (index > 0)
            {
            index++;
            switch (field->PbkFieldId())
                {
                case EPbkFieldIdFaxNumber:
                case EPbkFieldIdPagerNumber:
                case EPbkFieldIdPhoneNumberVideo:
                    {
                    result = ETrue;
                    break;
                    }
                default:
                    {
                    result = EFalse;
                    }
                }        
            ASSERT(result);            
            }
        else
            {
            ASSERT(!field);
            }
        } while (index > 0);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindNextFieldWithTextBasic' testcase.
 * Based on standard filled item
 */ 
TInt CMTPbkContactItem::RunFindNextFieldWithTextBasic()
    {    
		SetFilledContactL();    	
    TInt index = 0;
    TInt count = 0;
    TPbkContactItemField* field;
    
    // This string is part of KFillText 
    _LIT(KFindText, "STDA");

    do
        {
        field = iContactItem->FindNextFieldWithText(KFindText, index);
        if (index >= 0)
            {
            index++;
            count++;
            ASSERT(field->TextStorage()->Text() == KFillText);
            }
        else
            {
            ASSERT(!field);
            }
        
        } while (index > 0);
        
        ASSERT(count == iFilledFields);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'FindNextFieldWithTextNot' testcase.
 * Actually don't find fields
 */ 
TInt CMTPbkContactItem::RunFindNextFieldWithTextNot()
    {    
    SetFilledContactL();
    TInt index = 0;
    TPbkContactItemField* field;

    _LIT(KFindText, "hilipatipippan");    
    field = iContactItem->FindNextFieldWithText(KFindText, index);
    ASSERT(!field);
    ASSERT(index == -1);

    // go through all fields
    _LIT(KEmpty, "");
    TInt count = 0;
    index = 0;
    do
        {
        field = iContactItem->FindNextFieldWithText(KEmpty, index);           
        if (index >= 0)
            {
            index++;
            count++;
            }
        else
            {
            ASSERT(!field);
            }
        } while (index > 0);
    
    // find all fields
    ASSERT(count == iContactItem->CardFields().Count());
    TeardownContactItem();
    return KErrNone;
    }
    
/**
 * Run 'FindSameField' testcase
 */ 
TInt CMTPbkContactItem::RunFindSameFieldL()
    {
    	SetFilledContactL();
    TPbkContactItemField* field = iContactItem->FindField(EPbkFieldIdPhoneNumberMobile);    
    ASSERT(iContactItem->FindSameField(*field) == field);
    
    // try a new field - should no be found
    TPbkContactItemField* newField = new (ELeave) TPbkContactItemField();
    CleanupStack::PushL(newField);
    
    ASSERT(!(iContactItem->FindSameField(*newField)));
    
    CleanupStack::PopAndDestroy(newField);
    TeardownContactItem();
    return KErrNone;
    }
    
/**
 * Run 'CanAcceptDataOfType' testcase.
 * Check if there is a free/addable field.
 */ 
TInt CMTPbkContactItem::RunCanAcceptDataOfTypeL()
    {
    	SetFilledContactL();
	CPbkFieldInfo* firstNameField = iContactEngine->FieldsInfo().Find(EPbkFieldIdFirstName);
    
    // firstname already exists - should fail
    ASSERT(!iContactItem->CanAcceptDataOfType(*firstNameField));

	CPbkFieldInfo* workPhoneField = iContactEngine->FieldsInfo().Find(EPbkFieldIdPhoneNumberWork);
    
    // workphone should be able to add
    ASSERT(iContactItem->CanAcceptDataOfType(*workPhoneField));

    CPbkContactItem* emptyItem = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyItem);

    // firstname not in an empty item
    ASSERT(emptyItem->CanAcceptDataOfType(*firstNameField));
    CleanupStack::PopAndDestroy(emptyItem);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'AddOrReturnUnUsedFieldL' testcase.
 * Check if there is a available field.
 */ 
TInt CMTPbkContactItem::RunAddOrReturnUnUsedFieldL()
    {
    	SetFilledContactL();
	CPbkFieldInfo* firstNameInfo = iContactEngine->FieldsInfo().Find(EPbkFieldIdFirstName);
    
    // firstname already exists - should fail
    ASSERT(!iContactItem->AddOrReturnUnusedFieldL(*firstNameInfo));

	CPbkFieldInfo* workPhoneInfo = iContactEngine->FieldsInfo().Find(EPbkFieldIdPhoneNumberWork);
    
    // workphone should be able to add
    ASSERT(iContactItem->AddOrReturnUnusedFieldL(*workPhoneInfo));

    CPbkContactItem* emptyItem = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyItem);

    // firstname not in an empty item
    ASSERT(emptyItem->AddOrReturnUnusedFieldL(*firstNameInfo));
    CleanupStack::PopAndDestroy(emptyItem);    
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'Get item type' testcase.
 */ 
TInt CMTPbkContactItem::RunGetTypeL()
    {
    	SetFilledContactL();
    TUid type = iContactItem->Type();
    ASSERT(type.iUid > 0);

    CPbkContactItem* emptyItem = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyItem);

    type = emptyItem->Type();
    ASSERT(type.iUid > 0);
    CleanupStack::PopAndDestroy(emptyItem);    
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'Get item id' testcase.
 */ 
TInt CMTPbkContactItem::RunGetIdL()
    {
    	SetFilledContactL();
    TContactItemId id = iContactItem->Id();
    ASSERT(id >= 0);

    CPbkContactItem* emptyItem = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyItem);

    id = emptyItem->Id();
    ASSERT(id >= 0);
    CleanupStack::PopAndDestroy(emptyItem); 
    TeardownContactItem();   
    return KErrNone;
    }

/**
 * Run 'Get contact item' testcase.
 */ 
TInt CMTPbkContactItem::RunGetContactItemL()
    {
    	SetFilledContactL();
    CContactItem& citem = iContactItem->ContactItem();
     
    // Just try some kind of editing
    TContactItemId cid = 666;
    citem.SetTemplateRefId(cid);

    ASSERT(citem.TemplateRefId() == cid);

    const CContactItem& citem2 = iContactItem->ContactItem();
    ASSERT(citem2.TemplateRefId() > 0);
        
    // Do the same things with an empty contact:
    CPbkContactItem* emptyItem = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(emptyItem);
    CContactItem& citem3 = emptyItem->ContactItem();

    // Just try some kind of editing
    citem3.SetTemplateRefId(cid);

    ASSERT(citem3.TemplateRefId() == cid);

    const CContactItem& citem4 = emptyItem->ContactItem();
    ASSERT(citem4.TemplateRefId() > 0);

    CleanupStack::PopAndDestroy(emptyItem);        
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'Get groups joined' testcase.
 */ 
TInt CMTPbkContactItem::RunGroupsJoinedL()
    {
    	SetFilledContactL();
    // first test a item which has no groups defined
    CContactIdArray* emptyArray = iContactItem->GroupsJoinedLC();
    
    ASSERT(emptyArray->Count() == 0);
    
    CleanupStack::PopAndDestroy(emptyArray);

    // Add contact group
    _LIT(KGroupName,"AnotherTestGroup");
    CContactGroup* cgroup = iContactEngine->CreateContactGroupL(KGroupName);
   
    CleanupStack::PushL(cgroup);
    
    // get the id of the group
    TInt groupId1 = cgroup->Id();
    CleanupStack::PopAndDestroy(cgroup);

    // Add another contact group
    _LIT(KGroupName2,"AnotherTestGroup2");
    CContactGroup* cgroup2 = iContactEngine->CreateContactGroupL(KGroupName2);
    CleanupStack::PushL(cgroup2);
    
    // get the id of the group
    TInt groupId2 = cgroup2->Id();
    CleanupStack::PopAndDestroy(cgroup2);
    
    // add item to DB and groups
    TContactItemId itemId = iContactEngine->AddNewContactL(*iContactItem);
    iContactEngine->AddContactToGroupL(itemId, groupId1);
    iContactEngine->AddContactToGroupL(itemId, groupId2);

    // read the grouped item from db
    CPbkContactItem* item = iContactEngine->ReadContactLC(itemId);

    // Get the array with groups
    CContactIdArray* anotherArray = item->GroupsJoinedLC();
    
    ASSERT(anotherArray->Count() == 2);
    ASSERT(anotherArray->Find(groupId1) != KErrNotFound);
    ASSERT(anotherArray->Find(groupId2) != KErrNotFound);

    CleanupStack::PopAndDestroy(anotherArray);
    CleanupStack::PopAndDestroy(item);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'Add all fields' testcase.
 */ 
TInt CMTPbkContactItem::RunAddAllFieldsL()
    {
    	SetEmptyContactL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);

	TInt addedFields = 0;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdLastName, KFillText));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdSecondName, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFirstName, KFillText));
	addedFields++;
	
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberStandard, KNumber1));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberStandard, KNumber2));
	addedFields++;
	
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberGeneral, KNumber1));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberGeneral, KNumber2));
	addedFields++;
    
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberHome, KNumber1, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberHome, KNumber2, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberHome, KNumber3, EPbkFieldLocationHome));
	addedFields++;
    
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber1, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber2, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber3, EPbkFieldLocationHome));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberWork, KNumber1, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberWork, KNumber2, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberWork, KNumber3, EPbkFieldLocationWork));
	addedFields++;
 
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber1));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber2));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber3));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber1, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber2, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber3, EPbkFieldLocationHome));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber1, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber2, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberMobile, KNumber3, EPbkFieldLocationWork));
	addedFields++;
	
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPhoneNumberVideo, KNumber1));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber1, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber2, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber3, EPbkFieldLocationHome));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber1, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber2, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber3, EPbkFieldLocationWork));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber1));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber2));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdFaxNumber, KNumber3));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPagerNumber, KNumber1));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPagerNumber, KNumber2));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPagerNumber, KNumber3));
	addedFields++;
    
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdEmailAddress, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdEmailAddress, KFillText, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdEmailAddress, KFillText, EPbkFieldLocationHome));
	addedFields++;
        
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPostalAddress, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPostalAddress, KFillText, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCompanyAddress, KFillText, EPbkFieldLocationWork));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPOBox, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdExtendedAddress, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdStreetAddress, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPostalCode, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCity, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdState, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCountry, KFillText));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPOBox, KFillText, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdExtendedAddress, KFillText, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdStreetAddress, KFillText, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPostalCode, KFillText, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCity, KFillText, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdState, KFillText, EPbkFieldLocationHome));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCountry, KFillText, EPbkFieldLocationHome));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPOBox, KFillText, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdExtendedAddress, KFillText, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdStreetAddress, KFillText, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPostalCode, KFillText, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCity, KFillText, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdState, KFillText, EPbkFieldLocationWork));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCountry, KFillText, EPbkFieldLocationWork));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdURL, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdJobTitle, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdCompanyName, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdDTMFString, KFillText));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdDate, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdNote, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPicture, KFillText));
	addedFields++;

    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdThumbnailImage, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPersonalRingingToneIndication, KFillText));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdLocationIdIndication, KFillText));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdVOIP, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPushToTalk, KFillText));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdSIPID, KFillText));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdShareView, KFillText));
	addedFields++;
    ASSERT(AddFieldWithValueL(*item, EPbkFieldIdPrefix, KFillText));
	addedFields++;
	ASSERT(AddFieldWithValueL(*item, EPbkFieldIdSuffix, KFillText));
	addedFields++;

    
    // Date differently
    TTime now;
    now.UniversalTime();
    TPbkContactItemField* field = FindFieldForModificationL(*item, EPbkFieldIdDate);
    field->DateTimeStorage()->SetTime(now);
	addedFields++;
  
    // do invariant check
    ASSERT(Invariant(*iContactEngine, *item));
    
    CleanupStack::PopAndDestroy(item);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run 'PbkFieldAt' testcase.
 */ 
TInt CMTPbkContactItem::RunPbkFieldAtL()
    {
    	SetEmptyContactL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);
    
    TInt homeIdx = KErrNotFound;
    TInt lastNameIdx = KErrNotFound;
    TInt emailIdx = KErrNotFound;

    for (TInt i = 0; i < item->CardFields().Count(); ++i)
        {
        TPbkContactItemField& field = item->CardFields()[i];
        
        if (field.FieldInfo().IsPhoneNumberField())
            {            
            if (field.FieldInfo().FieldId() == EPbkFieldIdPhoneNumberMobile)
                {
                homeIdx = i;
                }
            TInt number = i;
            TBuf<KPhoneNumberLenght> phoneNumber;
            phoneNumber.Num(number);
            field.TextStorage()->SetTextL(phoneNumber);
            }
        else
            {
            switch (field.StorageType())
                {

                case KStorageTypeText:
                    {
                    field.TextStorage()->SetTextL(KFillText);
                    
                    if (field.FieldInfo().FieldId() == EPbkFieldIdLastName)
                        {
                        lastNameIdx = i;
                        }
                    
                    if (field.FieldInfo().FieldId() == EPbkFieldIdEmailAddress)
                        {
                        emailIdx = i;
                        }
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
                    User::Panic(KPanicText, KPanicUnsupportedFieldType);
                    break;
                    }
                }
            }
        }

    ASSERT(item->PbkFieldAt(homeIdx).PbkFieldId() == EPbkFieldIdPhoneNumberMobile);
    ASSERT(item->PbkFieldAt(lastNameIdx).PbkFieldId() == EPbkFieldIdLastName);
    ASSERT(item->PbkFieldAt(emailIdx).PbkFieldId() == EPbkFieldIdEmailAddress);

    //test const function
    const MPbkFieldData& homeFieldData = item->PbkFieldAt(homeIdx);
    ASSERT(homeFieldData.PbkFieldId() == EPbkFieldIdPhoneNumberMobile);
    const MPbkFieldData& lastNameFieldData  = item->PbkFieldAt(lastNameIdx);
    ASSERT(lastNameFieldData.PbkFieldId() == EPbkFieldIdLastName);
    const MPbkFieldData& emailFieldData  = item->PbkFieldAt(emailIdx);
    ASSERT(emailFieldData.PbkFieldId() == EPbkFieldIdEmailAddress);
    
    CleanupStack::PopAndDestroy(item);
    TeardownContactItem();
    return KErrNone;
    }
    
/**
 * Run 'RunPbkFieldCountL' testcase.
 */ 
TInt CMTPbkContactItem::RunPbkFieldCountL()
    {
    	SetEmptyContactL();
    CPbkContactItem* item = iContactEngine->CreateEmptyContactL();
    CleanupStack::PushL(item);
    
    ASSERT(item->CardFields().Count() == item->PbkFieldCount() );
    
    CleanupStack::PopAndDestroy(item);
    TeardownContactItem();
    return KErrNone;
    }

/**
 * Run the NewL test case
 */ 
TInt CMTPbkContactItem::RunNewLTest()
    {
    SetNewLTestL();
    ASSERT((iContactItem->CardFields().Count() == 0));
    ASSERT(NoDefaultsSet(*iContactItem));
    ASSERT(Invariant(*iContactEngine, *iContactItem));
    TeardownContactItem();    
    return KErrNone;
    }
    

/**
 * Run the '==' test case
 */ 
TInt CMTPbkContactItem::RunEqualsTestL()
    {
    SetFilledContactL();
    TContactItemId id1 = iContactEngine->AddNewContactL( *iContactItem );
    CPbkContactItem* item1 = iContactEngine->OpenContactL( id1 );
    
    // fields are not same
    ASSERT( !( *item1 == *iContactItem ) );
    
    TContactItemId  id2 = iContactEngine->DuplicateContactL( id1 );
    CPbkContactItem* item2 = iContactEngine->OpenContactL( id2);
    
    CleanupStack::PushL( item1 );
    CleanupStack::PushL( item2 );
    
    //fields are same becouse items are duplicated
    ASSERT( *item1 == *item2 );
        
    iContactEngine->CloseContactL( id1 );
    iContactEngine->CloseContactL( id2 );
    CleanupStack::PopAndDestroy( item2 );
    CleanupStack::PopAndDestroy( item1 );
    TeardownContactItem();
    return KErrNone;
    }



//  [End of File] - Do not remove
