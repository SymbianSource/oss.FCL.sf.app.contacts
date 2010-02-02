/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
*
*/



// [INCLUDE FILES] - do not remove
#include "MT_VPbkEng_SelectorFactory.h"
#include "stifunitmacros.h"
#include "constants.h"

#include <VPbkFieldTypeSelectorFactory.h>
#include <rlocalizedresourcefile.h>
#include <vpbkdatacaging.hrh>
#include <VPbkEng.rsg>
#include <CVPbkFieldTypeSelector.h>
#include <CVPbkContactTestUtils.h>
#include <CVPbkContactManager.h>
#include <MVPbkContactStore.h>
#include <MVPbkStoreContact.h>
#include <MVPbkStoreContactField.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkFieldType.h>
#include <MVPbkContactFieldUriData.h>
#include <MVPbkStoreContactFieldCollection.h>
#include <VPbkContactViewFilterBuilder.h>

#include <spentry.h>
#include <spproperty.h>
#include <spsettings.h>
#include <e32property.h>

// System includes
#include <f32file.h>
#include <barsread.h>

#include <VPbkEng.rsg>

namespace {

const TUint KSPSupportCallOutKey = 0x2000B160;


void CreateTextFieldL(
                MVPbkStoreContact& aNewContact, 
                const MVPbkFieldType* aFieldType, 
                const TDesC& aText)
    {
    __ASSERT_ALWAYS(aFieldType, User::Leave(KErrNotFound));
    MVPbkStoreContactField* newField = aNewContact.CreateFieldLC(*aFieldType);
    MVPbkContactFieldTextData& textData = MVPbkContactFieldTextData::Cast(newField->FieldData());       
    textData.SetTextL(aText);
    aNewContact.AddFieldL(newField);
    CleanupStack::Pop(); // newField
    }

void CreateUriFieldL(
                MVPbkStoreContact& aNewContact, 
                const MVPbkFieldType* aFieldType, 
                const TDesC& aScheme,
                const TDesC& aText)
    {
    __ASSERT_ALWAYS(aFieldType, User::Leave(KErrNotFound));
    MVPbkStoreContactField* newField = aNewContact.CreateFieldLC(*aFieldType);
    MVPbkContactFieldUriData& uriData = MVPbkContactFieldUriData::Cast(
            newField->FieldData());
    uriData.SetUriL(aScheme, aText);
    aNewContact.AddFieldL(newField);
    CleanupStack::Pop(); // newField
    }

TInt Find(const TInt type, const TArray<TInt> aArray)
    {
    for (TInt i = 0; i < aArray.Count(); ++i)
        {
        if (type == aArray[i])
            {
            return i;
            }
        }
    return KErrNotFound;
    }

void FillL(RArray<TInt>& aArray, const TInt* aTable, const TInt aSize)
    {
    for (TInt i = 0; i < aSize; ++i)
        {
        if (aArray.Find(aTable[i]) == KErrNotFound)
            {
            // allows to add only once 
            aArray.AppendL(aTable[i]);
            }
        }
    }

void MergeL(RArray<TInt>& aArray, const MVPbkStoreContact& aContact)
    {
    const MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    RArray<TInt> result;
    CleanupClosePushL(result);
    for (TInt i = 0; i < fields.FieldCount(); ++i)
        {
        const MVPbkStoreContactField& field = fields.FieldAt(i);
        TInt resourceId = field.BestMatchingFieldType()->FieldTypeResId();

        if (Find(resourceId, aArray.Array()) >= 0)
            {
            if (result.Find(resourceId) == KErrNotFound)
                {
                result.AppendL(resourceId);
                }
            }
        }

    aArray.Reset();
    for (TInt i = 0; i < result.Count(); ++i)
        {
        aArray.AppendL(result[i]);
        }
    CleanupStack::PopAndDestroy();
    }

void CreateServiceL(const TDesC& aName, CSPSettings& aSettings)
    {
    CSPEntry* entry = CSPEntry::NewLC();
    entry->SetServiceName(aName);
    
    CSPProperty* prop = CSPProperty::NewLC();
    User::LeaveIfError( prop->SetName(ESubPropertyIMLaunchUid) );
    User::LeaveIfError( prop->SetValue(1) );
    User::LeaveIfError( entry->AddPropertyL(*prop) );
    CleanupStack::PopAndDestroy();
    
    CSPProperty* mask = CSPProperty::NewLC();
    User::LeaveIfError( mask->SetName( EPropertyServiceAttributeMask ) );
    TInt maskValue = ESupportsInternetCall | ESupportsMSISDNAddressing | 
                     EIsVisibleInCallMenu;;
    User::LeaveIfError( mask->SetValue( maskValue ) );
    User::LeaveIfError( entry->AddPropertyL( *mask ) );
    CleanupStack::PopAndDestroy( mask );

    User::LeaveIfError( aSettings.AddEntryL(*entry) );
    
    CleanupStack::PopAndDestroy();
    }



}

// ============================ MEMBER FUNCTIONS ===============================

// executed once for whole test set
void CMT_VPbkEng_SelectorFactory::BaseSetupL()
    {
    CActiveScheduler* activeScheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(activeScheduler);
    iTestUtils = CVPbkContactTestUtils::NewL();
    iAssertUtils = new CAssertUtils(*iLog);
    
    CVPbkContactManager* cmanager = iTestUtils->ContactManager();

    iFieldTypes = &cmanager->FieldTypes();

    // reset sp settings table before starting to test
    iSpSettings = CSPSettings::NewL();
    
    RIdArray idArray;
    CleanupClosePushL(idArray);
    User::LeaveIfError( iSpSettings->FindServiceIdsL(idArray) );
    
    for (TInt i = 0; i < idArray.Count(); ++i)
        {
        TServiceId id = idArray[i];
        iSpSettings->DeleteEntryL(id);
        }
    
    CleanupStack::PopAndDestroy(); // idArray
    
    MVPbkContactStore* store = iTestUtils->S60ContactModelStore();
    iContact = store->CreateNewContactLC();
    CleanupStack::Pop(); // iContact

    const MVPbkFieldType* field = iFieldTypes->Find(R_VPBK_FIELD_TYPE_FIRSTNAME);
    CreateTextFieldL(*iContact, field, _L("Jambo"));
    
    field = iFieldTypes->Find(R_VPBK_FIELD_TYPE_LANDPHONEHOME);
    CreateTextFieldL(*iContact, field, _L("09123456"));
    
    field = iFieldTypes->Find(R_VPBK_FIELD_TYPE_MOBILEPHONEHOME);
    CreateTextFieldL(*iContact, field, _L("040123456"));
    
    field = iFieldTypes->Find(R_VPBK_FIELD_TYPE_IMPP);
    CreateUriFieldL(*iContact, field, _L("gtalk"), _L("jambo@gtalk.com"));
    
    field = iFieldTypes->Find(R_VPBK_FIELD_TYPE_IMPP);
    CreateUriFieldL(*iContact, field, _L("msn"), _L("jambo@msn.com"));
    
    field = iFieldTypes->Find(R_VPBK_FIELD_TYPE_EMAILHOME);
    CreateTextFieldL(*iContact, field, _L("myemail"));

    iTestUtils->CommitL(*iContact);
    }

// executed once for whole test set
void CMT_VPbkEng_SelectorFactory::BaseTeardown()
    {
    iTestUtils->DeleteAllContactsInCntDbL();
    delete iTestUtils;
    delete iAssertUtils;
    delete iSpSettings;
    delete iContact;
    
    CActiveScheduler::Install(NULL);
    }

void CMT_VPbkEng_SelectorFactory::Setup()
    {
    STIF_LOG("Setup");
    }

void CMT_VPbkEng_SelectorFactory::SetupVoipL()
    {
    CSPEntry* entry = CSPEntry::NewLC();
    entry->SetServiceName(_L("PlainVoip"));
    
    CSPProperty* prop = CSPProperty::NewLC();
    prop->SetName(ESubPropertyVoIPSettingsId);
    prop->SetValue(1);
    entry->AddPropertyL(*prop);

    // set voip service mask
    CSPProperty* mask = CSPProperty::NewLC();
    User::LeaveIfError( mask->SetName( EPropertyServiceAttributeMask ) );
    TInt val = ESupportsInternetCall | EIsVisibleInCallMenu;
    TInt maskValue = val;
    User::LeaveIfError( mask->SetValue( maskValue ) );
    User::LeaveIfError( entry->AddPropertyL( *mask ) );
    CleanupStack::PopAndDestroy( mask );    
    
    User::LeaveIfError( iSpSettings->AddEntryL(*entry) );
    
    CleanupStack::PopAndDestroy(2);
    }

void CMT_VPbkEng_SelectorFactory::SetupVoipCallOutL()
    {
    CSPEntry* entry = CSPEntry::NewLC();
    entry->SetServiceName(_L("Voip call out"));
    
    CSPProperty* prop = CSPProperty::NewLC();
    prop->SetName(ESubPropertyVoIPSettingsId);
    prop->SetValue(1);
    entry->AddPropertyL(*prop);
    
    CSPProperty* mask = CSPProperty::NewLC();
    User::LeaveIfError( mask->SetName( EPropertyServiceAttributeMask ) );
    TInt val = ESupportsInternetCall | ESupportsMSISDNAddressing | 
        EIsVisibleInCallMenu;
    TInt maskValue = val;
    User::LeaveIfError( mask->SetValue( maskValue ) );
    User::LeaveIfError( entry->AddPropertyL( *mask ) );
    CleanupStack::PopAndDestroy( mask );    

    User::LeaveIfError( iSpSettings->AddEntryL(*entry) );
    
    CleanupStack::PopAndDestroy(2);

    }

void CMT_VPbkEng_SelectorFactory::SetupGtalkSupportsVoipL()
    {
    CreateServiceL(_L("gtalk"), *iSpSettings);
    }
    
void CMT_VPbkEng_SelectorFactory::Teardown()
    {
    User::LeaveIfError( RProperty::Set( KUidSystemCategory, KSPSupportCallOutKey, 0 ) );
    
    RIdArray idArray;
    CleanupClosePushL(idArray);
    User::LeaveIfError( iSpSettings->FindServiceIdsL(idArray) );
    
    for (TInt i = 0; i < idArray.Count(); ++i)
        {
        TServiceId id = idArray[i];
        iSpSettings->DeleteEntryL(id);
        }
    
    CleanupStack::PopAndDestroy(); // idArray

    STIF_LOG("Teardown");
    }

//TESTCASE("OK test", "CNone", Setup, TestPassed, Teardown)
void CMT_VPbkEng_SelectorFactory::TestPassed()
    {
    STIF_LOG("The Ultimate Answer %d", 42);        

    TInt* p = NULL;
    TInt i = 5;
    STIF_ASSERT_NULL( p );
    STIF_ASSERT( !p );
    STIF_ASSERT_NOT_NULL( &i );
    
    TInt* p1 = &i;
    p1++;
    STIF_ASSERT_SAME( &i, &i );
    STIF_ASSERT_NOT_SAME( &i, p1 );
    }

//TESTCASE("FieldSelector Valid", "VPbkFieldTypeSelectorFactory", Setup, TestFieldSelectorFactoryValid, Teardown)
void CMT_VPbkEng_SelectorFactory::TestFieldSelectorFactoryValid()
    {
    CVPbkFieldTypeSelector* selectorPtr = 
            VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EMobileNumberSelector, *iFieldTypes);
    
    STIF_ASSERT_NOT_NULL( selectorPtr );
    
    delete selectorPtr;
    }
    
//TESTCASE("FieldSelector Invalid", "VPbkFieldTypeSelectorFactory", Setup, TestFieldSelectorFactoryInvalid, Teardown)
void CMT_VPbkEng_SelectorFactory::TestFieldSelectorFactoryInvalid()
    {
    CVPbkFieldTypeSelector* selectorPtr = NULL;
    TInt notExistingPredefinedSelector(-1);
        
    STIF_ASSERT_LEAVES( selectorPtr = 
               VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL(
                       (VPbkFieldTypeSelectorFactory::TVPbkFieldTypeSelector)notExistingPredefinedSelector,
                       *iFieldTypes) );
    
    delete selectorPtr;
    }

//TESTCASE("CommSelector Valid", "VPbkFieldTypeSelectorFactory", Setup, TestCommSelectorFactoryValid, Teardown)
void CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryValid()
    {
    CVPbkFieldTypeList* aFieldTypes = CreateFieldTypeListL();
        
    CVPbkFieldTypeSelector* selectorPtr = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVoiceCallSelector, *iFieldTypes);
    
    STIF_ASSERT_NOT_NULL( selectorPtr );
    
    delete selectorPtr;
    delete aFieldTypes;
    }
  
//TESTCASE("CommSelector uni editor", "VPbkFieldTypeSelectorFactory", Setup, TestCommSelectorFactoryUniEditor, Teardown)
void CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryUniEditor()
    {
    CVPbkFieldTypeList* aFieldTypes = CreateFieldTypeListL();
        
    CVPbkFieldTypeSelector* selectorPtr = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EUniEditorSelector, *iFieldTypes);
    
    STIF_ASSERT_NOT_NULL( selectorPtr );
    
    delete selectorPtr;
    delete aFieldTypes;
    }


//TESTCASE("CommSelector Invalid", "VPbkFieldTypeSelectorFactory", Setup, TestCommSelectorFactoryInvalid, Teardown)
void CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryInvalid()
    {
    TInt notExistingPredefinedSelector(-1);
        
    CVPbkFieldTypeSelector* selector = VPbkFieldTypeSelectorFactory::
            BuildContactActionTypeSelectorL(
                    (VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector)
                    notExistingPredefinedSelector,
                    *iFieldTypes);
    CleanupStack::PushL(selector);
    
    RArray<TInt> array;
    CleanupClosePushL(array);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);    
    
    CleanupStack::PopAndDestroy(2); // array, selector
    }


//TESTCASE("TestCommSelectorFactoryNoList", "VPbkFieldTypeSelectorFactory", Setup, TestCommSelectorFactoryNoList, Teardown)
void CMT_VPbkEng_SelectorFactory::TestCommSelectorFactoryNoList()
    {
    CVPbkFieldTypeSelector* selectorPtr = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EUniEditorSelector,
                    *iFieldTypes);
    
    STIF_ASSERT_NOT_NULL( selectorPtr );
    
    delete selectorPtr;
    }

//TESTCASE("TestUniEditorSelectionL", "VPbkFieldTypeSelectorFactory", Setup, TestUniEditorSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestUniEditorSelectionL()
    {
    CVPbkFieldTypeSelector* selectorPtr = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EUniEditorSelector,
                    *iFieldTypes);
    
    CleanupStack::PushL(selectorPtr);
    
    iAssertUtils->AssertTypesL(
            *selectorPtr, KUniEditorFields, KUniEditorFieldsSize, *iFieldTypes, *iResult);
    
    CleanupStack::PopAndDestroy();
    }

//TESTCASE("TestVoiceCallSelectionL", "VPbkFieldTypeSelectorFactory", Setup, TestVoiceCallSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestVoiceCallSelectionL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVoiceCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);
    
    TFixedArray<TInt,KEVoiceCallSelectorFieldsSize> array(
            KEVoiceCallSelectorFields, KEVoiceCallSelectorFieldsSize);

    AssertTypesL(
            *selector, 
            array.Array(), 
            *iFieldTypes);

    // test externalization and internalization
    HBufC8* buf = selector->ExternalizeLC();
    CleanupStack::Pop();
    CleanupStack::PopAndDestroy();
    CleanupStack::PushL(buf);

    selector = CVPbkFieldTypeSelector::NewL(*iFieldTypes);
    CleanupStack::PushL(selector);
    selector->InternalizeL(*buf);
    
    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);
    
    // test selector creation from an other selector
    CVPbkFieldTypeSelector* copyselector = CVPbkFieldTypeSelector::NewL(*selector);
    CleanupStack::PopAndDestroy(2, buf);
    CleanupStack::PushL(copyselector);
    AssertTypesL(
            *copyselector,
            array.Array(),
            *iFieldTypes);
    CleanupStack::PopAndDestroy(copyselector);
    }

//TESTCASE("TestVoipNotSupportedSelectionL", "VPbkFieldTypeSelectorFactory", Setup, TestVoipNotSupportedSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestVoipNotSupportedSelectionL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    RArray<TInt> array;
    CleanupClosePushL(array);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy(2); // array, selector
    }

//TESTCASE("Test No Voip available", "VPbkFieldTypeSelectorFactory", Setup, TestVoipNotAvailableSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestVoipNotAvailableSelectionL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);
    
    RArray<TInt> array;
    CleanupClosePushL(array);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy(2);
    }

//TESTCASE("Test Voip Enabled", "VPbkFieldTypeSelectorFactory", SetupVoipL, TestVoipCallSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestVoipCallSelectionL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    TFixedArray<TInt,KEVOIPCallSelectorFieldsSize> array(
            KEVOIPCallSelectorFields, KEVOIPCallSelectorFieldsSize);  

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy();
    }

//TESTCASE("Test Voip Call Out Enabled", "VPbkFieldTypeSelectorFactory", SetupVoipCallOutL, TestVoipCallOutSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestVoipCallOutSelectionL()
    {
    // test selector creation from resourceb
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    RArray<TInt> array;
    CleanupClosePushL(array);
    
    FillL(array, KEVOIPCallSelectorFields, KEVOIPCallSelectorFieldsSize);
    FillL(array, KEVOIPCallOutSelectorFields, KEVOIPCallOutSelectorFieldsSize);
    
    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy(2);
    }

//TESTCASE("Test xsp supports voip", "VPbkFieldTypeSelectorFactory", SetupGtalkSupportsVoipL, TestXspSupportsVoipSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestXspSupportsVoipSelectionL()
    {
    // test selector creation from resourceb
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    RArray<TInt> array;
    CleanupClosePushL(array);

    FillL(array, KEVOIPCallSelectorFields, KEVOIPCallSelectorFieldsSize);
    FillL(array, KEVOIPCallOutSelectorFields, KEVOIPCallOutSelectorFieldsSize);
    FillL(array, KEVOIPXspSelectorFields, KEVOIPXspSelectorFieldsSize);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy(2);
    }

//TESTCASE("TestVideoSelectorL", "VPbkFieldTypeSelectorFactory", Setup, TestVideoSelectorL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestVideoSelectorL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVideoCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    TFixedArray<TInt,KVideoCallSelectorsSize> array(
            KVideoCallSelectors, KVideoCallSelectorsSize);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy();
    }


//TESTCASE("TestURLSelectorL", "VPbkFieldTypeSelectorFactory", Setup, TestURLSelectorL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestURLSelectorL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EURLSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    TFixedArray<TInt,KURLSelectorsSize> array(
            KURLSelectors, KURLSelectorsSize);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy();
    }

//TESTCASE("TestEmailSelectorL", "VPbkFieldTypeSelectorFactory", Setup, TestEmailSelectorL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestEmailSelectorL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EEmailEditorSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    TFixedArray<TInt,KEmailEditorSelectorsSize> array(
            KEmailEditorSelectors, KEmailEditorSelectorsSize);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy();
    }

//TESTCASE("TestIMSelectorL", "VPbkFieldTypeSelectorFactory", Setup, TestIMSelectorL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestIMSelectorL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = 
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EInstantMessagingSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    TFixedArray<TInt,KInstantMessagingSelectorsSize> array(
            KInstantMessagingSelectors, KInstantMessagingSelectorsSize);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy();
    }

//TESTCASE("Test gtalk contact selection", "VPbkFieldTypeSelectorFactory", SetupGtalkSupportsVoipL, TestGtalkContactSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestGtalkContactSelectionL()
    {
    RArray<TInt> array;
    CleanupClosePushL(array);
    FillL(array, KEVOIPCallSelectorFields, KEVOIPCallSelectorFieldsSize);
    FillL(array, KEVOIPCallOutSelectorFields, KEVOIPCallOutSelectorFieldsSize);
    FillL(array, KEVOIPXspSelectorFields, KEVOIPXspSelectorFieldsSize);

    // assert only with fields that are found from contact
    // this is a sub set of array content
    MergeL(array, *iContact);

    // test selector creation from resource
    CVPbkFieldTypeSelector* selector =
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    CDesCArray* providers = new (ELeave) CDesC16ArrayFlat(3);
    CleanupStack::PushL(providers);
    providers->AppendL(_L("gtalk"));

    AssertFieldsL(
            *selector,
            *iContact,
            array.Array(),
            providers);

    CleanupStack::PopAndDestroy(3);
    }

//TESTCASE("Test two services contact selection", "VPbkFieldTypeSelectorFactory", Setup, TestTwoServicesContactSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestTwoServicesContactSelectionL()
    {
    CreateServiceL(_L("gtalk"), *iSpSettings);
    CreateServiceL(_L("msn"), *iSpSettings);

    RArray<TInt> array;
    CleanupClosePushL(array);
    FillL(array, KEVOIPCallSelectorFields, KEVOIPCallSelectorFieldsSize);
    FillL(array, KEVOIPCallOutSelectorFields, KEVOIPCallOutSelectorFieldsSize);
    FillL(array, KEVOIPXspSelectorFields, KEVOIPXspSelectorFieldsSize);
    
    // assert only with fields that are found from contact
    // this is a sub set of array content
    MergeL(array, *iContact);

    // test selector creation from resource
    CVPbkFieldTypeSelector* selector =
            VPbkFieldTypeSelectorFactory::BuildContactActionTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EVOIPCallSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);

    CDesCArray* providers = new (ELeave) CDesC16ArrayFlat(3);
    CleanupStack::PushL(providers);
    providers->AppendL(_L("gtalk"));
    providers->AppendL(_L("msn"));

    AssertFieldsL(
            *selector,
            *iContact,
            array.Array(),
            providers);

    CleanupStack::PopAndDestroy(3);
    }

//TESTCASE("TestVersion1SelectorL", "VPbkFieldTypeSelectorFactory", Setup, TestVersion1SelectorL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestVersion1SelectorL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector =
            VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::EFirstNameSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);
    
    const TInt fields[] = {R_VPBK_FIELD_TYPE_FIRSTNAME};

    TFixedArray<TInt,KInstantMessagingSelectorsSize> array(fields, 1);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy();
    }

//TESTCASE("Test builder multiple selections", "VPbkFieldTypeSelectorFactory", Setup, TestMultipleSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestMultipleSelectionL()
    {
    const TInt KPhoneNumberFields[] = {
            R_VPBK_FIELD_TYPE_MOBILEPHONEGEN, 
            R_VPBK_FIELD_TYPE_MOBILEPHONEHOME,
            R_VPBK_FIELD_TYPE_MOBILEPHONEWORK,
            R_VPBK_FIELD_TYPE_LANDPHONEGEN,
            R_VPBK_FIELD_TYPE_LANDPHONEHOME,
            R_VPBK_FIELD_TYPE_LANDPHONEWORK,
            R_VPBK_FIELD_TYPE_FAXNUMBERWORK,
            R_VPBK_FIELD_TYPE_FAXNUMBERHOME,
            R_VPBK_FIELD_TYPE_FAXNUMBERGEN,
            R_VPBK_FIELD_TYPE_PAGERNUMBER,
            R_VPBK_FIELD_TYPE_CARPHONE,
            R_VPBK_FIELD_TYPE_VIDEONUMBERGEN,
            R_VPBK_FIELD_TYPE_VIDEONUMBERWORK,
            R_VPBK_FIELD_TYPE_VIDEONUMBERHOME,
            R_VPBK_FIELD_TYPE_ASSTPHONE
    };

    RArray<TInt> array;
    CleanupClosePushL(array);
    FillL(array, KPhoneNumberFields, sizeof(KPhoneNumberFields) / sizeof(TInt));
    FillL(array, KEmailEditorSelectors, KEmailEditorSelectorsSize);

    // test selector creation from resource
    CVPbkFieldTypeSelector* selector = CVPbkFieldTypeSelector::NewL(*iFieldTypes);  
    CleanupStack::PushL(selector);
    
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( *selector, 
                                                           EVPbkContactViewFilterPhoneNumber,
                                                           *iTestUtils->ContactManager() );
                
    VPbkContactViewFilterBuilder::BuildContactViewFilterL( *selector, 
                                                           EVPbkContactViewFilterEmail,
                                                           *iTestUtils->ContactManager() );

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    HBufC8* buf = selector->ExternalizeLC();
    CleanupStack::Pop();
    CleanupStack::PopAndDestroy(); // selector
    CleanupStack::PushL(buf);

    selector = CVPbkFieldTypeSelector::NewL(*iFieldTypes);
    CleanupStack::PushL(selector);
    selector->InternalizeL(*buf);

    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    // test selector creation from an other selector
    CVPbkFieldTypeSelector* copyselector = CVPbkFieldTypeSelector::NewL(*selector);
    CleanupStack::PopAndDestroy(2, buf);
    CleanupStack::PushL(copyselector);
    AssertTypesL(
            *copyselector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy(2); // copyselector, array
    }

//TESTCASE("TestNonVersitTypeSelectionL", "VPbkFieldTypeSelectorFactory", Setup, TestNonVersitTypeSelectionL, Teardown)
void CMT_VPbkEng_SelectorFactory::TestNonVersitTypeSelectionL()
    {
    // test selector creation from resource
    CVPbkFieldTypeSelector* selector =
            VPbkFieldTypeSelectorFactory::BuildFieldTypeSelectorL(
                    VPbkFieldTypeSelectorFactory::ECodTextSelector,
                    *iFieldTypes);

    CleanupStack::PushL(selector);
    
    const TInt fields[] = {R_VPBK_FIELD_TYPE_CALLEROBJTEXT};
    TFixedArray<TInt,KInstantMessagingSelectorsSize> array(fields, 1);
    AssertTypesL(
            *selector,
            array.Array(),
            *iFieldTypes);

    CleanupStack::PopAndDestroy();
    }

CVPbkFieldTypeList* CMT_VPbkEng_SelectorFactory::CreateFieldTypeListL()
    {
    // Create MVPbkFieldTypeList for an argument
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    CleanupClosePushL( fs );
    VPbkEngUtils::RLocalizedResourceFile resFile;
    resFile.OpenLC( fs, KVPbkRomFileDrive, KDC_RESOURCE_FILES_DIR, KVPbkDefResFileName );
    HBufC8* fieldTypesBuf = resFile.AllocReadLC( R_VPBK_GLOBAL_FIELD_TYPES );
    TResourceReader resReader;
    resReader.SetBuffer( fieldTypesBuf );
    CVPbkFieldTypeList* aFieldTypes = CVPbkFieldTypeList::NewL( resFile, resReader );
        
    CleanupStack::PopAndDestroy( 3 ); // fieldTypesBuf, resFile, fs
    
    return aFieldTypes;
    }

void CMT_VPbkEng_SelectorFactory::AssertTypesL(
        CVPbkFieldTypeSelector& aSelector,
        const TArray<TInt> aArray,
        const MVPbkFieldTypeList& aFieldTypes)
    {
    TInt count = aFieldTypes.FieldTypeCount();
    while (--count >= 0)
        {
        const MVPbkFieldType& type = aFieldTypes.FieldTypeAt(count);
        TBool included = aSelector.IsFieldTypeIncluded(type);
        TInt index = Find(type.FieldTypeResId(), aArray);
        if (index >= 0)
            {
            // assert that field is included
            STIF_LOG("resid: %d, included %d", type.FieldTypeResId(), included);
            STIF_ASSERT(included);
            }
        else
            {
            // assert that field is not included
            STIF_LOG("resid: %d, included %d", type.FieldTypeResId(), included);
            STIF_ASSERT(!included);
            }
        }
    }

void CMT_VPbkEng_SelectorFactory::AssertFieldsL(
        CVPbkFieldTypeSelector& aSelector,
        const MVPbkStoreContact& aContact,
        TArray<TInt> aAsserFields,
        CDesCArray* aServiceNames)
    {
    const MVPbkStoreContactFieldCollection& fields = aContact.Fields();
    for (TInt i = 0; i < fields.FieldCount(); ++i)
        {
        const MVPbkStoreContactField& field = fields.FieldAt(i);
        TInt resourceId = field.BestMatchingFieldType()->FieldTypeResId();
        TBool included = aSelector.IsFieldIncluded(field);
        TInt index = Find(resourceId, aAsserFields);
        STIF_LOG("resid: %d, included %d", resourceId, included);
        if (resourceId == R_VPBK_FIELD_TYPE_IMPP)
            {
            const MVPbkContactFieldData& data = field.FieldData();
            const MVPbkContactFieldUriData& uri = MVPbkContactFieldUriData::Cast(data);
            const TPtrC scheme = uri.Scheme();
            if (aServiceNames->Find(scheme, index) != KErrNone)
                {
                index = KErrNotFound;
                }
            }

        if (index >= 0)
            {
            // assert that field is included
            STIF_ASSERT(included);
            }
        else
            {
            // assert that field is not included
            STIF_ASSERT(!included);
            }
        }
    }

//  [End of File] - do not remove
