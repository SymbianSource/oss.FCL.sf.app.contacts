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
#include "MT_VPbkEng_StrCntAnalyzer.h"
#include "stifunitmacros.h"
#include <CVPbkContactManager.h>
#include <CVPbkContactStoreUriArray.h>
#include <tvpbkcontactstoreuriptr.h>
#include <MVPbkContactStore.h>
#include <MVPbkContactStoreList.h>
#include <MVPbkStoreContact.h>
#include <MVPbkFieldType.h>
//#include <StrCntAnalyzer.rsg>
#include <vpbkeng.rsg>
#include <CVPbkContactStoreProperties.h>

#include "TTestContactField.h"

// ============================ MEMBER FUNCTIONS ===============================

_LIT( KDbUri, "cntdb://c:contacts.cdb" );


void CMT_VPbkEng_StrCntAnalyzer::Setup()
    {
    STIF_LOG("Setup");
    
    // install active scheduler ( important :) )
    if ( !CActiveScheduler::Current() )
         {
         CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
         CActiveScheduler::Install(scheduler);
         }
    
    // create contact store manager
    CVPbkContactStoreUriArray* uriList = CVPbkContactStoreUriArray::NewLC(); 
    uriList->AppendL( TVPbkContactStoreUriPtr( KDbUri ) );
    iContactManager = CVPbkContactManager::NewL( *uriList );
    CleanupStack::PopAndDestroy();  //uriList
    
    // create empty contact
    MVPbkContactStoreList& cntStoreList = iContactManager->ContactStoresL();
    MVPbkContactStore& cntStore = cntStoreList.At(0);
    iContact = cntStore.CreateNewContactLC();
    
    // Setup store properties
    MVPbkContactStoreProperties* store = const_cast<MVPbkContactStoreProperties*>(
            &(iContact->ParentStore().StoreProperties()));
    CVPbkContactStoreProperties* storeProperties = static_cast<CVPbkContactStoreProperties*>( store );
    storeProperties->SetSupportedFields(iContactManager->FieldTypes());
            
    CleanupStack::Pop(); //iContact

    }
    
void CMT_VPbkEng_StrCntAnalyzer::Teardown()
    {
    // todo: delete crashs
    // delete iContact;
    
    delete iContactManager;
    
    CActiveScheduler* scheduler = CActiveScheduler::Current();
    CActiveScheduler::Install(NULL);
    delete scheduler;
    
    STIF_LOG("Teardown");
    }

//TESTCASE("HasFieldL", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestHasField, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestHasField()
    {      
    // empty contact -> KErrNotFound is expected
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    STIF_ASSERT_EQUALS( KErrNotFound, analyzer.HasFieldL( VPbkFieldTypeSelectorFactory::EPhoneNumberSelector, 3, iContact ) ); 
    }

//TESTCASE("HasFieldL CommAction", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestHasFieldCA, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestHasFieldCA()
    {      
    // empty contact -> KErrNotFound is expected
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    STIF_ASSERT_EQUALS( KErrNotFound, analyzer.HasFieldL( VPbkFieldTypeSelectorFactory::EVoiceCallSelector, NULL ) ); 
    }

//TESTCASE("IsFieldTypeSupportedL", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeSupported, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeSupported()
    { 
    // ETrue is expected
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    STIF_ASSERT_TRUE( analyzer.IsFieldTypeSupportedL( VPbkFieldTypeSelectorFactory::EPhoneNumberSelector ) ); 
    }

//TESTCASE("IsFieldTypeSupportedL CommAction", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeSupportedCA, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeSupportedCA()
    { 
    // ETrue is expected
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    STIF_ASSERT_TRUE( analyzer.IsFieldTypeSupportedL( VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) ); 
    }

//TESTCASE("IsFieldTypeIncludedL true", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeIncludedTL, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeIncludedTL()
    {  
    // ETrue expected -> R_VPBK_FIELD_TYPE_LASTNAME == ELastNameSelector
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    const MVPbkFieldType* ftype = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME );
    STIF_ASSERT_TRUE(analyzer.IsFieldTypeIncludedL( *ftype ,VPbkFieldTypeSelectorFactory::ELastNameSelector ) ); 
    }

//TESTCASE("IsFieldTypeIncludedL false", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeIncludedFL, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeIncludedFL()
    {  
    // EFalse expected -> R_VPBK_FIELD_TYPE_LASTNAME != EPhoneNumberSelector
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    const MVPbkFieldType* ftype = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME );
    STIF_ASSERT_FALSE(analyzer.IsFieldTypeIncludedL( *ftype ,VPbkFieldTypeSelectorFactory::EPhoneNumberSelector ) ); 
    }

//TESTCASE("IsFieldTypeIncludedL false", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeIncludedFLCA, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeIncludedFLCA()
    {  
    // EFalse expected -> R_VPBK_FIELD_TYPE_LASTNAME != EPhoneNumberSelector
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    const MVPbkFieldType* ftype = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME );
    STIF_ASSERT_FALSE(analyzer.IsFieldTypeIncludedL( *ftype ,VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) ); 
    }

//TESTCASE("IsFieldTypeIncludedL2 true", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeIncludedTL2, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeIncludedTL2()
    {   
    // ETrue expected -> R_VPBK_FIELD_TYPE_LASTNAME == ELastNameSelector
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    const MVPbkFieldType* ftype = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME ); 
    TTestContactField baseCntField;
    baseCntField.SetFieldType(*ftype);
    STIF_ASSERT_TRUE(analyzer.IsFieldTypeIncludedL( baseCntField ,VPbkFieldTypeSelectorFactory::ELastNameSelector ) ); 
    }

//TESTCASE("IsFieldTypeIncludedL2 flase", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeIncludedFL2, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeIncludedFL2()
    {   
    // EFalse expected -> R_VPBK_FIELD_TYPE_LASTNAME != EPhoneNumberSelector
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    const MVPbkFieldType* ftype = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME ); 
    TTestContactField baseCntField;
    baseCntField.SetFieldType(*ftype);
    STIF_ASSERT_FALSE(analyzer.IsFieldTypeIncludedL( baseCntField ,VPbkFieldTypeSelectorFactory::EPhoneNumberSelector ) ); 
    }

//TESTCASE("IsFieldTypeIncludedL2 flase", "CMT_VPbkEng_StrCntAnalyzer", Setup, TestIsFieldTypeIncludedFL2CA, Teardown)
void CMT_VPbkEng_StrCntAnalyzer::TestIsFieldTypeIncludedFL2CA()
    {   
    // EFalse expected -> R_VPBK_FIELD_TYPE_LASTNAME != EPhoneNumberSelector
    
    TVPbkStoreContactAnalyzer analyzer( *iContactManager, iContact );
    const MVPbkFieldType* ftype = iContactManager->FieldTypes().Find( R_VPBK_FIELD_TYPE_LASTNAME ); 
    TTestContactField baseCntField;
    baseCntField.SetFieldType(*ftype);
    STIF_ASSERT_FALSE(analyzer.IsFieldTypeIncludedL( baseCntField ,VPbkFieldTypeSelectorFactory::EVoiceCallSelector ) ); 
    }

//  [End of File] - do not remove
