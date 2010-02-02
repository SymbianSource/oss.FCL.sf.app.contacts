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
#include "MTPbkFields.h"
#include <PbkFields.hrh>  // Values to be tested

//  INTERNAL INCLUDES
#include "PbkFields30.hrh"  // Reference values

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
// CMTPbkFields::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CMTPbkFields::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CMTPbkFields::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CMTPbkFields::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "Size_of_Class", CMTPbkFields::CheckSizeOfs_PbkFieldIdArrayL ),
        ENTRY( "Constant_values", CMTPbkFields::CheckConstantValuesL ),
        ENTRY( "Enum_values", CMTPbkFields::CheckEnumValuesL ),
        ENTRY( "Flag_bit_masks", CMTPbkFields::CheckFlagBitMasksL ),
        ENTRY( "Methods", CMTPbkFields::CheckMethodsL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

CPbkFieldIdArray* CMTPbkFields::CreateAndPopulateFieldIdArrayL()
    {
    CPbkFieldIdArray* fields = new (ELeave) CPbkFieldIdArray; 
    CleanupStack::PushL(fields);
    ASSERT(fields);

    // Add desired fields to the field id array
    fields->AppendL(EPbkFieldIdLastName);
    fields->AppendL(EPbkFieldIdFirstName);
    fields->AppendL(EPbkFieldIdPhoneNumberHome);
    fields->AppendL(EPbkFieldIdPhoneNumberWork);
    fields->AppendL(EPbkFieldIdPhoneNumberMobile);

    CleanupStack::Pop(fields);
    return (fields);
    }



void CMTPbkFields::SetupL(  )
    {
    iFields = CreateAndPopulateFieldIdArrayL();
    }

void CMTPbkFields::Teardown(  )
    {
    delete iFields;
    iFields = NULL;
    }

//----------------------------------------------------------------------------
TInt CMTPbkFields::CheckSizeOfs_PbkFieldIdArrayL()
    {
    SetupL();
    static const TInt sizeof_class_CPbkFieldIdArray_30 = 24;
//    RDebug::Printf("Classes: sizeof(class CPbkFieldIdArray)=%d\n", sizeof(class CPbkFieldIdArray));
    ASSERT( sizeof(class CPbkFieldIdArray) >= sizeof_class_CPbkFieldIdArray_30 );


    ASSERT( sizeof(TPbkFieldId) == sizeof(TPbkFieldId_30));

    ASSERT( sizeof(TPbkFieldGroupId) == sizeof(TPbkFieldGroupId_30));

    ASSERT( sizeof(TPbkOrderingGroup) == sizeof(TPbkOrderingGroup_30));

    ASSERT( sizeof(TPbkOrderingItem) == sizeof(TPbkOrderingItem_30));

    ASSERT( sizeof(TPbkAdditionalOrderingItem) == sizeof(TPbkAdditionalOrderingItem_30));

    ASSERT( sizeof(TPbkAddItemOrdering) == sizeof(TPbkAddItemOrdering_30));
	Teardown();
	return KErrNone;

    }


//----------------------------------------------------------------------------
TInt CMTPbkFields::CheckConstantValuesL()
    {
    SetupL();
// Unique Phonebook field type ids
    ASSERT(EPbkFieldIdNone                 == EPbkFieldIdNone_30);
    ASSERT(EPbkFieldIdLastName             == EPbkFieldIdLastName_30);
    ASSERT(EPbkFieldIdFirstName            == EPbkFieldIdFirstName_30);
    ASSERT(EPbkFieldIdPhoneNumberGeneral   == EPbkFieldIdPhoneNumberGeneral_30);
    ASSERT(EPbkFieldIdPhoneNumberStandard  == EPbkFieldIdPhoneNumberStandard_30);
    ASSERT(EPbkFieldIdPhoneNumberHome      == EPbkFieldIdPhoneNumberHome_30);
    ASSERT(EPbkFieldIdPhoneNumberWork      == EPbkFieldIdPhoneNumberWork_30);
    ASSERT(EPbkFieldIdPhoneNumberMobile    == EPbkFieldIdPhoneNumberMobile_30);
    ASSERT(EPbkFieldIdFaxNumber            == EPbkFieldIdFaxNumber_30);
    ASSERT(EPbkFieldIdPagerNumber          == EPbkFieldIdPagerNumber_30);
    ASSERT(EPbkFieldIdEmailAddress         == EPbkFieldIdEmailAddress_30);
    ASSERT(EPbkFieldIdPostalAddress        == EPbkFieldIdPostalAddress_30);
    ASSERT(EPbkFieldIdURL                  == EPbkFieldIdURL_30);
    ASSERT(EPbkFieldIdJobTitle             == EPbkFieldIdJobTitle_30);
    ASSERT(EPbkFieldIdCompanyName          == EPbkFieldIdCompanyName_30);
    ASSERT(EPbkFieldIdCompanyAddress       == EPbkFieldIdCompanyAddress_30);
    ASSERT(EPbkFieldIdDTMFString           == EPbkFieldIdDTMFString_30);
    ASSERT(EPbkFieldIdDate                 == EPbkFieldIdDate_30);
    ASSERT(EPbkFieldIdNote                 == EPbkFieldIdNote_30);
    ASSERT(EPbkFieldIdPicture              == EPbkFieldIdPicture_30);
    ASSERT(EPbkFieldIdThumbnailImage       == EPbkFieldIdThumbnailImage_30);
    ASSERT(EPbkFieldIdVoiceTagIndication   == EPbkFieldIdVoiceTagIndication_30);
    ASSERT(EPbkFieldIdSpeedDialIndication  == EPbkFieldIdSpeedDialIndication_30);
    ASSERT(EPbkFieldIdPersonalRingingToneIndication
                                                 == EPbkFieldIdPersonalRingingToneIndication_30);
    ASSERT(EPbkFieldIdPOBox                == EPbkFieldIdPOBox_30);
    ASSERT(EPbkFieldIdExtendedAddress      == EPbkFieldIdExtendedAddress_30);
    ASSERT(EPbkFieldIdStreetAddress        == EPbkFieldIdStreetAddress_30);
    ASSERT(EPbkFieldIdPostalCode           == EPbkFieldIdPostalCode_30);
    ASSERT(EPbkFieldIdCity                 == EPbkFieldIdCity_30);
    ASSERT(EPbkFieldIdState                == EPbkFieldIdState_30);
    ASSERT(EPbkFieldIdCountry              == EPbkFieldIdCountry_30);
    ASSERT(EPbkFieldIdWVID                 == EPbkFieldIdWVID_30);
    ASSERT(EPbkFieldIdSecondName           == EPbkFieldIdSecondName_30);
    ASSERT(EPbkFieldIdPhoneNumberVideo     == EPbkFieldIdPhoneNumberVideo_30);
    ASSERT(EPbkFieldIdLastNameReading      == EPbkFieldIdLastNameReading_30);
    ASSERT(EPbkFieldIdFirstNameReading     == EPbkFieldIdFirstNameReading_30);
    ASSERT(EPbkFieldIdLocationIdIndication == EPbkFieldIdLocationIdIndication_30);
    ASSERT(EPbkFieldIdVOIP                 == EPbkFieldIdVOIP_30);
    ASSERT(EPbkFieldIdPushToTalk           == EPbkFieldIdPushToTalk_30);
    ASSERT(EPbkFieldIdShareView            == EPbkFieldIdShareView_30);
    ASSERT(EPbkFieldIdSIPID                == EPbkFieldIdSIPID_30);
    ASSERT(EPbkFieldIdCodTextID            == EPbkFieldIdCodTextID_30);
    ASSERT(EPbkFieldIdCodImageID           == EPbkFieldIdCodImageID_30);
    ASSERT(EPbkFieldIdPrefix               == EPbkFieldIdPrefix_30);
    ASSERT(EPbkFieldIdSuffix               == EPbkFieldIdSuffix_30);

    ASSERT(KPbkFieldIdMask                 == KPbkFieldIdMask_30);


// Unique Phonebook field type group ids
    ASSERT(EPbkFieldGroupIdNone            == EPbkFieldGroupIdNone_30);
    ASSERT(EPbkFieldGroupIdPostalAddress   == EPbkFieldGroupIdPostalAddress_30);
    ASSERT(EPbkFieldGroupIdHomeAddress     == EPbkFieldGroupIdHomeAddress_30);
    ASSERT(EPbkFieldGroupIdCompanyAddress  == EPbkFieldGroupIdCompanyAddress_30);
    ASSERT(EPbkFieldGroupIdFirstName       == EPbkFieldGroupIdFirstName_30);
    ASSERT(EPbkFieldGroupIdLastName        == EPbkFieldGroupIdLastName_30);


// Definitions for minimum and maximum date values
    ASSERT(KPbkDateMinDay                  == KPbkDateMinDay_30);
    ASSERT(KPbkDateMinMonth                == KPbkDateMinMonth_30);
    ASSERT(KPbkDateMinYear                 == KPbkDateMinYear_30);
    ASSERT(KPbkDateMaxDay                  == KPbkDateMaxDay_30);
    ASSERT(KPbkDateMaxMonth                == KPbkDateMaxMonth_30);
    ASSERT(KPbkDateMaxYear                 == KPbkDateMaxYear_30);


    ASSERT(KPbkFieldFlagNull               == KPbkFieldFlagNull_30);
    ASSERT(KPbkFieldFlagDefTemplate        == KPbkFieldFlagDefTemplate_30);
    ASSERT(KPbkFieldFlagUserCanAddField    == KPbkFieldFlagUserCanAddField_30);
    ASSERT(KPbkFieldFlagDisableEdit        == KPbkFieldFlagDisableEdit_30);
	Teardown();
	return KErrNone;

    }

//----------------------------------------------------------------------------
TInt CMTPbkFields::CheckEnumValuesL()
    {
    SetupL();
// enum TPbkFieldMultiplicity
    ASSERT(EPbkFieldMultiplicityOne        == EPbkFieldMultiplicityOne_30);
    ASSERT(EPbkFieldMultiplicityMany       == EPbkFieldMultiplicityMany_30);

// enum TPbkFieldEditMode
    ASSERT(EPbkFieldEditModeAlpha          == EPbkFieldEditModeAlpha_30);
    ASSERT(EPbkFieldEditModeNumeric        == EPbkFieldEditModeNumeric_30);
    ASSERT(EPbkFieldEditModeDate           == EPbkFieldEditModeDate_30);
    ASSERT(EPbkFieldEditModeSelector       == EPbkFieldEditModeSelector_30);
    ASSERT(EPbkFieldEditModeTBD            == EPbkFieldEditModeTBD_30);
    ASSERT(EPbkFieldEditModeLatinOnly      == EPbkFieldEditModeLatinOnly_30);

// enum TPbkFieldDefaultCase
    ASSERT(EPbkFieldDefaultCaseNone        == EPbkFieldDefaultCaseNone_30);
    ASSERT(EPbkFieldDefaultCaseLower       == EPbkFieldDefaultCaseLower_30);
    ASSERT(EPbkFieldDefaultCaseText        == EPbkFieldDefaultCaseText_30);

// enum TPbkFieldCtrlType
    ASSERT(EPbkFieldCtrlTypeNone           == EPbkFieldCtrlTypeNone_30);
    ASSERT(EPbkFieldCtrlTypeTextEditor     == EPbkFieldCtrlTypeTextEditor_30);
    ASSERT(EPbkFieldCtrlTypeDateEditor     == EPbkFieldCtrlTypeDateEditor_30);
    ASSERT(EPbkFieldCtrlTypeNumberEditor   == EPbkFieldCtrlTypeNumberEditor_30);

// enum TPbkFieldLocation
    ASSERT(EPbkFieldLocationNone           == EPbkFieldLocationNone_30);
    ASSERT(EPbkFieldLocationHome           == EPbkFieldLocationHome_30);
    ASSERT(EPbkFieldLocationWork           == EPbkFieldLocationWork_30);

// enum TPbkVersitStorageType
    ASSERT(EPbkVersitPropertyNULL          == EPbkVersitPropertyNULL_30);
    ASSERT(EPbkVersitPropertyHBufC         == EPbkVersitPropertyHBufC_30);
    ASSERT(EPbkVersitPropertyBinary        == EPbkVersitPropertyBinary_30);
    ASSERT(EPbkVersitPropertyCDesCArray    == EPbkVersitPropertyCDesCArray_30);
    ASSERT(EPbkVersitPropertyMultiDateTime == EPbkVersitPropertyMultiDateTime_30);
    ASSERT(EPbkVersitPropertyDateTime      == EPbkVersitPropertyDateTime_30);
    ASSERT(EPbkVersitPropertyInt           == EPbkVersitPropertyInt_30);
	Teardown();
	return KErrNone;
    }

//----------------------------------------------------------------------------
TInt CMTPbkFields::CheckFlagBitMasksL()
    {
	SetupL();
    ASSERT(KPbkFieldFlagNull               == KPbkFieldFlagNull_30);
    ASSERT(KPbkFieldFlagDefTemplate        == KPbkFieldFlagDefTemplate_30);
    ASSERT(KPbkFieldFlagUserCanAddField    == KPbkFieldFlagUserCanAddField_30);
    ASSERT(KPbkFieldFlagDisableEdit        == KPbkFieldFlagDisableEdit_30);
	Teardown();
	return KErrNone;

    }

//----------------------------------------------------------------------------
TInt CMTPbkFields::CheckMethodsL()
    {
    SetupL();
// Existing fields
    TInt originalInsertOrder = 0;
    ASSERT(iFields->Find(EPbkFieldIdLastName) == originalInsertOrder);
    originalInsertOrder++;
    ASSERT(iFields->Find(EPbkFieldIdFirstName) == originalInsertOrder);
    originalInsertOrder++;
    ASSERT(iFields->Find(EPbkFieldIdPhoneNumberHome) == originalInsertOrder);
    originalInsertOrder++;
    ASSERT(iFields->Find(EPbkFieldIdPhoneNumberWork) == originalInsertOrder);
    originalInsertOrder++;
    ASSERT(iFields->Find(EPbkFieldIdPhoneNumberMobile) == originalInsertOrder);

// Some non-existing fields
    ASSERT(iFields->Find(EPbkFieldIdFaxNumber) == KErrNotFound);
    ASSERT(iFields->Find(EPbkFieldIdPagerNumber) == KErrNotFound);
    ASSERT(iFields->Find(EPbkFieldIdEmailAddress) == KErrNotFound);
    ASSERT(iFields->Find(EPbkFieldIdPostalAddress) == KErrNotFound);
    ASSERT(iFields->Find(EPbkFieldIdURL) == KErrNotFound);

// Existing fields
    ASSERT(iFields->Contains(EPbkFieldIdLastName));
    ASSERT(iFields->Contains(EPbkFieldIdFirstName));
    ASSERT(iFields->Contains(EPbkFieldIdPhoneNumberHome));
    ASSERT(iFields->Contains(EPbkFieldIdPhoneNumberWork));
    ASSERT(iFields->Contains(EPbkFieldIdPhoneNumberMobile));

// Some non-existing fields
    ASSERT(!iFields->Contains(EPbkFieldIdFaxNumber));
    ASSERT(!iFields->Contains(EPbkFieldIdPagerNumber));
    ASSERT(!iFields->Contains(EPbkFieldIdEmailAddress));
    ASSERT(!iFields->Contains(EPbkFieldIdPostalAddress));
    ASSERT(!iFields->Contains(EPbkFieldIdURL));
	Teardown();
	return KErrNone;
    }
//----------------------------------------------------------------------------

//  [End of File] - Do not remove
