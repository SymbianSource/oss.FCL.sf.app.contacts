/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact find policy.
*
*/


#include "CPbk2ContactFindPolicy.h"

// Phonebook2 includes
#include <CPbk2SortOrderManager.h>
#include <MPbk2ContactNameFormatter.h>
#include <MPbk2ContactNameFormatter2.h>
#include <Pbk2ContactNameFormatterFactory.h>

// Virtual phonebook includes
#include <MVPbkBaseContact.h>
#include <CVPbkFieldTypeRefsList.h>
#include <CVPbkContactFieldIterator.h>
#include <MVPbkContactFieldTextData.h>
#include <VPbkContactStoreUris.h>
#include <VPbkEng.rsg>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkBaseContactFieldCollection.h>

// System includes
#include <FindUtil.h>
#include <featmgr.h>

// Constants
const TInt KCountOfWords = 1;
const TInt KMaxLengthOfTwoNameFields = 100;
const TInt KDefaultTitleFormatting =
        MPbk2ContactNameFormatter::EPreserveLeadingSpaces;

#ifdef _DEBUG
enum TPanicCode
    {
    EPanicNullArray = 1
    };

void Panic( TPanicCode aReason )
    {
    _LIT( KPanicText, "CPbk2ContactFindPolicy" );
    User::Panic( KPanicText, aReason );
    }
#endif // _DEBUG

// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::CPbk2ContactFindPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactFindPolicy::CPbk2ContactFindPolicy()
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::~CPbk2ContactFindPolicy
// --------------------------------------------------------------------------
//
CPbk2ContactFindPolicy::~CPbk2ContactFindPolicy()
    {
    delete iFindUtil;
    delete iSortOrderManager;
    delete iNameFormatter;
    delete iFieldTypeRefsList;
    FeatureManager::UnInitializeLib();
    }

// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactFindPolicy* CPbk2ContactFindPolicy::NewL( TParam* aParam )
    {
    CPbk2ContactFindPolicy* self = new ( ELeave ) CPbk2ContactFindPolicy();
    CleanupStack::PushL( self );
    self->ConstructL( aParam );
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::ConstructL
// --------------------------------------------------------------------------
//
inline void CPbk2ContactFindPolicy::ConstructL( TParam* aParam )
    {
    iFindUtil = CFindUtil::NewL();
    
    if ( aParam )
        {
        // Create sort order manager
        iSortOrderManager = CPbk2SortOrderManager::NewL(
            aParam->iFieldTypeList, &aParam->iRFs );
        
        // Create contact name formatter
        iNameFormatter = Pbk2ContactNameFormatterFactory::CreateL(
            aParam->iFieldTypeList, *iSortOrderManager, &aParam->iRFs );
            
        //Create field type list
        iFieldTypeRefsList = CVPbkFieldTypeRefsList::NewL();
        }
    FeatureManager::InitializeLibL();
    }

// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::Match
// --------------------------------------------------------------------------
//
TBool CPbk2ContactFindPolicy::Match
        ( const TDesC& aText, const TDesC& aSearchString )
    {
    return iFindUtil->Interface()->Match( aText, aSearchString );
    }

// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::MatchContactNameL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactFindPolicy::MatchContactNameL( 
    const MDesCArray& aFindWords,
    const MVPbkBaseContact& aContact )
    {
    // This class constructed without aParam, so iNameFormatter
    // doesn't exist. Let's leave.
    if ( !iNameFormatter )
        {
        User::Leave( KErrNotSupported );
        }
    
    TBool match( ETrue );
    TInt findWordsCount = aFindWords.MdcaCount();
    const TInt KGranularity = 4; 
    CDesCArrayFlat* allNamesArray = new ( ELeave ) CDesCArrayFlat
        ( KGranularity );
    CleanupStack::PushL( allNamesArray );

    HBufC* title = NULL;
    if( FeatureManager::FeatureSupported(KFeatureIdFfContactsCompanyNames) )
        {
        MPbk2ContactNameFormatter2* nameformatterExtension =
            reinterpret_cast<MPbk2ContactNameFormatter2*>(iNameFormatter->
                    ContactNameFormatterExtension( MPbk2ContactNameFormatterExtension2Uid ) );
        title = nameformatterExtension->GetContactTitleWithCompanyNameL( aContact.Fields(),
                KDefaultTitleFormatting );
        } 
    else
        {
        title = iNameFormatter->GetContactTitleL( aContact.Fields(), 
                KDefaultTitleFormatting );
        }

    CleanupStack::PushL( title );
        
    if ( title->Length() )
    	{
    	SplitContactFieldTextIntoArrayL( allNamesArray, *title );
    	}
    else 
    	{
    	//If FDN contact, no name, use phonenumber as title if exists
    	GetTitleFromFDNNumberL( aContact, *allNamesArray );
    	}   

    CleanupStack::PopAndDestroy( title );

    // Match every search word in contact's fields. 
    if ( findWordsCount <= allNamesArray->MdcaCount() )
        {
        for ( TInt i = 0; i < findWordsCount && match; ++i )
            {
            for ( TInt j = 0; j < allNamesArray->MdcaCount(); ++j )   
                {
                // Match refine
                if ( !MatchRefineL( allNamesArray->MdcaPoint( j ), 
                        aFindWords.MdcaPoint( i ) ) )
                    {
                    match = EFalse;
                    }
                else
                    {
                    // Match is found, delete it from array to prevent 
                    // matching with the same word again
                    match = ETrue;
                    allNamesArray->Delete( j );
                    break;
                    }  
                }            
            }   
        }
    else
        {
        // More search words than name fields, can't match
        match = EFalse;
        }
    
        
    // Special case when searching names in China variant:
    // Last and first names are merged and search must find
    // names without typing space between them.
    if ( findWordsCount == KCountOfWords && !match )
        {
        FeatureManager::InitializeLibL();
        TBool chinese = FeatureManager::FeatureSupported( KFeatureIdChinese );
        FeatureManager::UnInitializeLib();
        
        TBool korean = (User::Language() == ELangKorean);

        if ( chinese || korean )
            {
            match = MatchChineseContactNameL( aFindWords, aContact );
            }
        }
    
    CleanupStack::PopAndDestroy( allNamesArray );
    return match;
    }


// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::MatchChineseContactNameL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactFindPolicy::MatchChineseContactNameL( 
    const MDesCArray& aFindWords,
    const MVPbkBaseContact& aContact )
    {
    HBufC* nameFields = HBufC::NewLC( KMaxLengthOfTwoNameFields );
    TPtr nameFieldsPtr = nameFields->Des();
    TBool match;
    
    // Create iterator
    MVPbkBaseContactFieldIterator* iterator = iNameFormatter->
    ActualTitleFieldsLC( *iFieldTypeRefsList, aContact.Fields() );
    
    // Loop iterator
    while ( iterator->HasNext() )
        {
        const MVPbkBaseContactField* field = iterator->Next();

        // Check field's type
        if ( field->FieldData().DataType() == EVPbkFieldStorageTypeText )
            {
            const MVPbkContactFieldTextData& data =
                MVPbkContactFieldTextData::Cast( field->FieldData() );
            // Catenate contact's name fields
            nameFieldsPtr.Append( data.Text() );
            }
        }
    // Match refine
    if ( !MatchRefineL( nameFieldsPtr, aFindWords.MdcaPoint( 0 ) ) )
        {
        match = EFalse;
        }
    else
        {
        match = ETrue;
        }
    CleanupStack::PopAndDestroy(); // iterator
    CleanupStack::PopAndDestroy( nameFields );
    
    return match;
    }


// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::MatchRefineL
// --------------------------------------------------------------------------
//
TBool CPbk2ContactFindPolicy::MatchRefineL
        ( const TDesC& aText, const TDesC &aSearchString )
    {
    TBool ret = iFindUtil->Interface()->MatchRefineL( aText, aSearchString );

    return ret;
    }

// --------------------------------------------------------------------------
// CPbk2ContactFindPolicy::IsWordValidForMatching
// --------------------------------------------------------------------------
//
TBool CPbk2ContactFindPolicy::IsWordValidForMatching
        ( const TDesC& aWord )
    {
    return iFindUtil->Interface()->IsWordValidForMatching(aWord);
    }

//----------------------------------------------------------------------------
// CPbk2ContactFindPolicy::SplitContactFieldTextIntoArrayL
//----------------------------------------------------------------------------
//
void CPbk2ContactFindPolicy::SplitContactFieldTextIntoArrayL(
		CDesCArray* aArray,
        const TDesC& aText )
    {
    __ASSERT_DEBUG( aArray, Panic( EPanicNullArray ) );

    TBool firstWord = ETrue; // To avoid ignoring space from first word in contact field
    const TInt textLength = aText.Length();
    for ( TInt beg = 0; beg < textLength; ++beg )
        {
        // Skip separators before next word
        if ( !iNameFormatter->IsFindSeparatorChar( aText[beg] ) )
            {
            // Scan the end of the word
            TInt end = beg;
            for (; end < textLength &&
                   !iNameFormatter->IsFindSeparatorChar( aText[end] );
                ++end )
                {
                }
            const TInt len = end - beg;
            // Append found word to the array
            if (firstWord)
	            {
	            aArray->AppendL( aText.Mid( 0,end ) );
	            firstWord = EFalse;
	            }
            else
            	{
            	aArray->AppendL( aText.Mid( beg,len ) );
            	}
            // Scan for next word
            beg = end;
            }
        }

    if ( aArray->MdcaCount() == 0 && textLength > 0 )
        {
        // aText is all word separator characters
        // -> make a "word" out of those
        aArray->AppendL( aText );
        }
    }

//----------------------------------------------------------------------------
// CPbk2ContactFindPolicy::GetTitleFromFDNNumber
//----------------------------------------------------------------------------
//
void CPbk2ContactFindPolicy::GetTitleFromFDNNumberL( const MVPbkBaseContact& aContact, CDesC16Array& aAllNamesArray )
	{    
    //Check if it's a SIM FDN contact
    if( aContact.MatchContactStore( VPbkContactStoreUris::SimGlobalFdnUri() ) )
        {
        const MVPbkBaseContactFieldCollection& fieldCollection = aContact.Fields(); 
        
        for(TInt i = 0; i < fieldCollection.FieldCount(); i++ )
            {
            const MVPbkBaseContactField& contactField = fieldCollection.FieldAt( i );
            const MVPbkFieldType* fieldType = contactField.BestMatchingFieldType();
            
            //Check if mobile number is used in the view
            if( fieldType->FieldTypeResId() == R_VPBK_FIELD_TYPE_MOBILEPHONEGEN )
                {
                const MVPbkContactFieldTextData* textData =
                                               &MVPbkContactFieldTextData::Cast
                                                   ( contactField.FieldData() );
                aAllNamesArray.AppendL( textData->Text() );
                break;
                }
            }
        }    
	}
    
// End of File
