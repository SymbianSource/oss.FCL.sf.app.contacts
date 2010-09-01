/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook2 contact editor dialog.
*
*/


// INCLUDE FILES
#include "CPbk2AdaptiveSearchGridFiller.h"
#include "MVPbkViewContact.h"
#include "MVPbkContactViewBase.h"
#include "MPbk2ContactNameFormatter.h"
#include "MPbk2FilteredViewStack.h"

#include <MVPbkBaseContactFieldCollection.h>
#include <MVPbkContactFieldTextData.h>
#include <MVPbkBaseContactField.h>


#include <MPbk2ContactViewSupplier.h>
#include <MPbk2ApplicationServices.h>
#include <MPbk2AppUi.h>
#include <CPbk2StoreConfiguration.h>
#include <MPbk2ContactNameFormatter2.h>
#include <FindUtil.h>
#include <badesca.h>
#include <featmgr.h>

#include <CPsRequestHandler.h>

const TInt KMaxAdaptiveGridCacheCount = 10;
const TInt KAdaptiveSearchKeyMapGranularity = 100;
const TInt KAdaptiveSearchRefineStep = 25;
const TInt KContactFormattingFlags = MPbk2ContactNameFormatter::EPreserveLeadingSpaces |
            MPbk2ContactNameFormatter::EReplaceNonGraphicChars |
            MPbk2ContactNameFormatter::EDisableCompanyNameSeparator;

namespace {
enum TNameOrder
    {
    ETopContactOrderNumber = 0,     //TC control data, not shown
    ENameFirstPart,                 //Contact name data
    ENameSecondPart,                //Contact name data
    ENameCompanyPart                //to support Company name
    };
} // namespace

NONSHARABLE_CLASS(CPbk2AdaptiveGrid) : public CBase
	{
	HBufC* iFindText;
	HBufC* iKeyMap;

	public:

		CPbk2AdaptiveGrid()
			{
			}

		~CPbk2AdaptiveGrid()
			{
			delete iFindText;
			delete iKeyMap;
			}

		void SetKeyMapL( const TDesC& aFindText, const TDesC& aKeyMap )
			{
			delete iFindText;
			delete iKeyMap;

			iFindText = iKeyMap = NULL;

			iFindText = aFindText.AllocL();
			iKeyMap = aKeyMap.AllocL();
			}

		const TDesC& GetFindText() const
			{
			return *iFindText;
			}

		const TDesC& GetKeyMap() const
			{
			return *iKeyMap;
			}
	};

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::CPbk2ContactEditorDlg
// --------------------------------------------------------------------------
//
CPbk2AdaptiveSearchGridFiller::CPbk2AdaptiveSearchGridFiller( CAknSearchField& aField, MPbk2ContactNameFormatter& aNameFormatter )
	: CActive( CActive::EPriorityIdle ), iSearchField( aField ), iNameFormatter( aNameFormatter ),
	iInvalidateAdaptiveSearchGrid( EFalse ),iSetFocusToSearchGrid( ETrue )
    {
	CActiveScheduler::Add( this );
    }

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::~CPbk2ContactEditorDlg
// --------------------------------------------------------------------------
//
CPbk2AdaptiveSearchGridFiller::~CPbk2AdaptiveSearchGridFiller()
    {
    Cancel();
    if ( iPsHandler )
        {
        iPsHandler->RemoveObserver( this );
        delete iPsHandler;
        }
	delete iKeyMap;
	delete iCurrentGrid;
	iAdaptiveGridCache.ResetAndDestroy();
	delete iSearchString;
	delete iFindUtil;
	iDigraphContactsTitleArray.ResetAndDestroy();
    }


// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::NewL
// --------------------------------------------------------------------------
//
CPbk2AdaptiveSearchGridFiller* CPbk2AdaptiveSearchGridFiller::NewL(  CAknSearchField& aField, MPbk2ContactNameFormatter& aNameFormatter )
    {
    CPbk2AdaptiveSearchGridFiller* self =
        new(ELeave) CPbk2AdaptiveSearchGridFiller( aField, aNameFormatter );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }


// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2AdaptiveSearchGridFiller::ConstructL()
    {
	iKeyMap = HBufC::NewL( KAdaptiveSearchKeyMapGranularity );
	iFindUtil = CFindUtil::NewL();
    // UI Language
	TLanguage uiLanguage = User::Language();
	if ( uiLanguage != ELangJapanese && uiLanguage != ELangPrcChinese && 
	        uiLanguage != ELangHongKongChinese && uiLanguage != ELangTaiwanChinese &&
	        uiLanguage != ELangKorean )
	    {  
        iPsHandler = CPSRequestHandler::NewL();
        iPsHandler->AddObserverL( this );
	    }
    }

void CPbk2AdaptiveSearchGridFiller::StartFillingL( const MVPbkContactViewBase& aView,
        const TDesC& aSearchString, TBool aClearCache )
	{
	
    if( aClearCache )
        {
        ClearCache();
        }
    
	if ( IsActive() && iView == &aView && iViewItemCount == aView.ContactCountL() 
	        && iSearchString && !iSearchString->Compare( aSearchString ) )
	    {
	    return;
	    }
	else
	    {
	    StopFilling();
	    }

    CPbk2AdaptiveGrid* keyMap = KeyMapFromCache( aSearchString );
    
    if( keyMap )
        {
        iSearchField.SetAdaptiveGridChars( keyMap->GetKeyMap() );
        return;
        }
    
	iViewItemCount = aView.ContactCountL();
	delete iSearchString;
	iSearchString = NULL;

	iSearchString = aSearchString.AllocL();

	// If there is no search word, the user is not searching any contacts
	// so we should reset the array to prepare for the next searching.
    if ( iSearchString->Length() == 0 )
    	{
    	iDigraphContactsTitleArray.ResetAndDestroy();
    	}
	iView = &aView;

	iKeyMap->Des().Zero();

	iCounter = 0;

	if ( iSearchString->Length() <= KPsAdaptiveGridSupportedMaxLen && GridFromPsEngineL( aView ) )
	    {
	    return;
	    }
	
	SetActive();
	TRequestStatus* status = &iStatus;
	User::RequestComplete( status, KErrNone );
	}

void CPbk2AdaptiveSearchGridFiller::StopFilling()
	{
	Cancel();
	iView = NULL;
	}

void CPbk2AdaptiveSearchGridFiller::RunL()
	{
	if( !iView )
		{
		return;
		}

	TInt stopCount = iCounter + KAdaptiveSearchRefineStep;
	const TInt itemCount = iView->ContactCountL();
    
	if( stopCount > itemCount )
		{
		stopCount = itemCount;
		}

	TInt maxSpacesNumber = 0;

    CDesC16Array* searchWordsArray = SplitSearchFieldTextIntoArrayLC( *iSearchString );
    TInt searchStringSpacesNumber = searchWordsArray->MdcaCount() - 1;
    
	for( ; iCounter < stopCount; iCounter++ )
		{
		const MVPbkViewContact& contact = iView->ContactAtL( iCounter );
		const TInt titleLength = iNameFormatter.MaxTitleLength( contact.Fields(), KContactFormattingFlags );
		HBufC* title = NULL;
		
		if( FeatureManager::FeatureSupported( KFeatureIdFfContactsCompanyNames ) )
            {
            MPbk2ContactNameFormatter2* nameformatterExtension =
                    reinterpret_cast<MPbk2ContactNameFormatter2*>( iNameFormatter.
                    ContactNameFormatterExtension( MPbk2ContactNameFormatterExtension2Uid ) );
            if ( nameformatterExtension && titleLength )
                {
                title = nameformatterExtension->GetContactTitleWithCompanyNameL( contact.Fields(),
                    KContactFormattingFlags );
                
                }
            }
        else if ( titleLength )
            {
            title = iNameFormatter.GetContactTitleL( contact.Fields(), KContactFormattingFlags );
            
            // In FDN, the number will be displayed in the list if the contact is no name.
            // If it is, set the search string as NULL.
            if ( IsActualTitleEmpty( contact ) )    
                {
                delete title;
                title = NULL;
            	}
            }
		
		if ( !title )
		    {
		    title = HBufC::NewL( titleLength );
		    }

        CleanupStack::PushL( title );
		BuildGridL( *title, searchWordsArray, iKeyMap );
		
		// check number of spaces in the contact title
		TInt numberOfSpaces = NumberOfSpacesInString( *title );
		if ( numberOfSpaces > maxSpacesNumber )
		    {
		    maxSpacesNumber = numberOfSpaces;
		    }
		// Check if the contact's title include drgraphs,
		// if it is, add it to array to save.
		if ( IsDigraphContactsTitleL( *title ) )
			{			
			iDigraphContactsTitleArray.AppendL( title );
			CleanupStack::Pop(); //title
			}
		else
			{
			CleanupStack::PopAndDestroy( title );
			}
		}
	// If there are titles in array, we should add them to build grids again,
	// because the contacts include drgraphs will be filtered 
	// when the application builds grids again.
    if ( iDigraphContactsTitleArray.Count()!= 0 )
    	{
    	for( TInt i(0); i < iDigraphContactsTitleArray.Count() ; i++ )
    		{
    		TPtr ptrContactsTitle = iDigraphContactsTitleArray[i]->Des();
        	BuildGridL( ptrContactsTitle, searchWordsArray, iKeyMap );
    		}
		}

    CleanupStack::PopAndDestroy( searchWordsArray ); //searchWordsArray

	if( stopCount == itemCount )
		{
		SetAdaptiveGridCharsL( maxSpacesNumber, searchStringSpacesNumber );
		AddKeyMapToCacheL( *iSearchString, *iKeyMap );
		}
	else
		{
		//else continue
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete( status, KErrNone );
		}
	}

void CPbk2AdaptiveSearchGridFiller::DoCancel()
	{
	iView = NULL;
	}

TInt CPbk2AdaptiveSearchGridFiller::RunError( TInt /*aError*/ )
	{
	//ignore errors, nothing critical has happened, lets forget it
	return KErrNone;
	}

CPbk2AdaptiveGrid* CPbk2AdaptiveSearchGridFiller::KeyMapFromCache( const TDesC& aFindText )
	{
	const TInt count = iAdaptiveGridCache.Count();

	for( TInt i( 0 ); i < count; i++ )
		{
		if( iAdaptiveGridCache[i] != NULL
		    && !aFindText.Compare( iAdaptiveGridCache[i]->GetFindText() ) )
			{
			return iAdaptiveGridCache[i];
			}
		}

	return NULL;
	}

void CPbk2AdaptiveSearchGridFiller::AddKeyMapToCacheL( const TDesC& aFindText, const TDesC& aKeyMap )
	{
    CPbk2AdaptiveGrid* keyMap = new( ELeave )CPbk2AdaptiveGrid;
    CleanupStack::PushL( keyMap );
    keyMap->SetKeyMapL( aFindText, aKeyMap );

    // Keep always in the cache at position 0 the grid cache element for empty find box,
    // which is the one that requires more time to be built
    if ( aFindText.Length() == 0 )
        {
        if ( iAdaptiveGridCache.Count() > 0 )
            {
            delete iAdaptiveGridCache[0];
            iAdaptiveGridCache.Remove( 0 );
            }

        iAdaptiveGridCache.InsertL( keyMap, 0 );
        }
    else
        {
        if ( iAdaptiveGridCache.Count() == 0 )
        {
        iAdaptiveGridCache.InsertL( NULL, 0 );
        }

        iAdaptiveGridCache.InsertL( keyMap, 1 );
    
        // Delete oldest cache element
        if( iAdaptiveGridCache.Count() > KMaxAdaptiveGridCacheCount )
            {
            delete iAdaptiveGridCache[KMaxAdaptiveGridCacheCount];
            iAdaptiveGridCache.Remove( KMaxAdaptiveGridCacheCount );
            }
        }

    CleanupStack::Pop(); //keyMap
	}

void CPbk2AdaptiveSearchGridFiller::ClearCache()
	{
	iAdaptiveGridCache.ResetAndDestroy();
	if ( iCurrentGrid )
	    {
        delete iCurrentGrid;
        iCurrentGrid = NULL;
	    }
	}

void CPbk2AdaptiveSearchGridFiller::InvalidateAdaptiveSearchGrid()
	{
	iInvalidateAdaptiveSearchGrid = ETrue;
	}

void CPbk2AdaptiveSearchGridFiller::SetFocusToAdaptiveSearchGrid()
    {
    iSetFocusToSearchGrid = ETrue;
    }

void CPbk2AdaptiveSearchGridFiller::SetAdaptiveGridCharsL(
        const TInt aMaxSpacesNumber, const TInt aSearchStringSpacesNumber )
	{
	TPtr ptr = iKeyMap->Des();

	// Do upper case for all characters
	ptr.UpperCase();
	CDesCArray* array = new (ELeave) CDesCArrayFlat( KAdaptiveSearchKeyMapGranularity );
	CleanupStack::PushL( array );
	TInt length = ptr.Length();

	for( TInt ii = 0; ii < length; ii++ )
	    {
	    array->AppendL( ptr.Mid( ii, 1 ) );
	    }

	// Alphabetical sort
	array->Sort( ECmpCollated );
	ptr.Zero();

    // Add space character only if:
	// - user typed already some characters in the find pane,
	// - and more spaces can be found in contacts than in the current search string,
	// - and space is not the last character in the search string.
    if ( iSearchString->Length() > 0 
         && aMaxSpacesNumber > aSearchStringSpacesNumber
         && (*iSearchString)[iSearchString->Length() - 1] != TChar( ' ' ) )
        {
        ptr.Append( TChar( ' ' ) );
        }
     
	for( TInt ii = 0; ii < length; ii++ )
	    {
	    ptr.Append(array->MdcaPoint( ii ));
	    }
	CleanupStack::PopAndDestroy();//array

	if( iCurrentGrid )
		{
		if( !iCurrentGrid->Des().Compare( *iKeyMap ) )
			{
			//same grid again
			if( !iInvalidateAdaptiveSearchGrid )
				{
				//if grid hasn't been invalidated, we do not need to set it again
				return;
				}
			}
		}

	delete iCurrentGrid;
	iCurrentGrid = NULL;
	iCurrentGrid = iKeyMap->Des().AllocL();

	iSearchField.SetAdaptiveGridChars( *iKeyMap );
    
    iInvalidateAdaptiveSearchGrid = EFalse;
    
	if ( iSetFocusToSearchGrid )
	    {
        // set the focus to findbox
	    iSearchField.DrawDeferred();
	    iSetFocusToSearchGrid = EFalse;
	    }

	}


CDesC16Array* CPbk2AdaptiveSearchGridFiller::SplitContactFieldTextIntoArrayLC(
        const TDesC& aText )
    {
    // Attempt to minimize the allocations considering 3 words for the search fields:
    // FirstName, LastName, CompanyName.
    const TInt KGranularity = 2; // Attempt to minimize the allocations

    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( KGranularity );
    CleanupStack::PushL( array );
    const TInt textLength = aText.Length();
    for ( TInt beg = 0; beg < textLength; beg++ )
        {
        // Skip separators before next word
        if ( iNameFormatter.IsFindSeparatorChar( aText[beg] ) )
            {
            continue;
            }
        
        // Scan till the end of the word
        TInt end;
        for ( end = beg + 1;
              end < textLength && !iNameFormatter.IsFindSeparatorChar( aText[end] );
              ++end )
            {
            }

        // Append found word to the array
        array->AppendL( aText.Mid( beg, end - beg) );

        // Scan for next word
        beg = end;
        }

    return array;
    }

CDesC16Array* CPbk2AdaptiveSearchGridFiller::SplitSearchFieldTextIntoArrayLC(
        const TDesC& aText )
    {
    CDesC16Array* searchWordsArray = SplitContactFieldTextIntoArrayLC( aText );

    // In searchWordsArray, the last word is the only one which generates new keymap characters
    // for the grid; the other words are used only for matching the contact words.
    //
    // KNullDesC fake word as last word in search string will match all contact words so that all
    // initials of contact words will be put into the grid.
    // We do this in case the search string is empty or the last character is a space separator.
    
    if( searchWordsArray->MdcaCount() == 0 ||
        ( aText.Length() > 0 && aText[aText.Length() - 1] == TChar(' ') ) )
        {
        searchWordsArray->AppendL( KNullDesC );
        }

    return searchWordsArray;
    }

void CPbk2AdaptiveSearchGridFiller::BuildGridL( const TDesC& aContactString, const CDesC16Array* aSearchWords, HBufC*& aKeyMap )
	{
    CDesC16Array* contactWords = SplitContactFieldTextIntoArrayLC( aContactString );
	
    const TInt contactWordCount = contactWords->MdcaCount();
	const TInt searchWordCount = aSearchWords->MdcaCount();

    TPtrC searchWord;
    TPtrC contactWord;

    // Try to make as fast algorithm as possible if there is only one search word,
    // which is the most common case    
    if ( searchWordCount == 1 )
        {
        searchWord.Set( aSearchWords->MdcaPoint( 0 ) ); // Search word

        for( TInt j = 0; j < contactWordCount; j++ )
            {
            contactWord.Set( contactWords->MdcaPoint( j ) );
    
            iFindUtil->Interface()->MatchAdaptiveRefineL( contactWord, searchWord, aKeyMap );
            }
        }
    
    // The clients of this method will provide at least one search word, so 0 is unexpected
    else if ( searchWordCount > 1 )
        {
        RArray<TBool> contactWordsMatchedArray;
        contactWordsMatchedArray.ReserveL( contactWordCount );
        for ( TInt i = 0; i < contactWordCount; ++i )
            {
            contactWordsMatchedArray.AppendL( EFalse );
            }

        TBool matched = ETrue;

        // Scan search words except for the last one
        for ( TInt i = 0; matched && i < searchWordCount - 1; i++ )
            {
            searchWord.Set( aSearchWords->MdcaPoint( i ) );

            matched = EFalse; // Search word not matched yet
            
            // Check if the search word is matched in the contact
            for( TInt j = 0; !matched && j < contactWordCount; j++ )
                {
                contactWord.Set( contactWords->MdcaPoint( j ) );
    
                // Partially or fully matched
                if ( iFindUtil->Interface()->MatchRefineL( contactWord, searchWord ) )
                    {
                    // Allow one search word to match only one contact word.
                    // This could be done better if both search and grid creation would
                    // work in the same way for contacts matching...
                    // Example: Contact: "Dim Din Dit"
                    //          Search:  "DIN DI"
                    //          - DIN is matched fully
                    //          - DI is matched partially and assigned to "Dim"
                    //          - The grid will show "_T" instead of "_MT"
                    contactWordsMatchedArray[j] = ETrue;    
                    matched = ETrue;
                    }
                }
            }

        // If all search words before the last one matched (fully or partially),
        // add characters to the grid using last search word.
        if ( matched )
            {
            searchWord.Set( aSearchWords->MdcaPoint( searchWordCount - 1 ) ); // Last search word
    
            for( TInt j = 0; j < contactWordCount; j++ )
                {
                // skip Contact words matched by previous search words
                if (contactWordsMatchedArray[j])
                    {
                    continue;
                    }
    
                contactWord.Set( contactWords->MdcaPoint( j ) );
    
                iFindUtil->Interface()->MatchAdaptiveRefineL( contactWord, searchWord, aKeyMap );
                }
            }
    
        contactWordsMatchedArray.Close();
        }
        
	CleanupStack::PopAndDestroy( contactWords );
	}

TInt CPbk2AdaptiveSearchGridFiller::NumberOfSpacesInString(
    const TDesC& aSearchString )
    {
    TInt numberOfSpaces = 0;
	TInt searchResult = 0;
	TPtrC ptr = aSearchString;
	while ( searchResult != KErrNotFound )
	    {
		searchResult = ptr.Locate( TChar( ' ' ) );
		if ( searchResult != KErrNotFound )
		    {
		    numberOfSpaces++;
		    ptr.Set( ptr.Right( ptr.Length() - searchResult - 1 ) );
		    }
		}
    return numberOfSpaces;
    }

TBool CPbk2AdaptiveSearchGridFiller::IsDigraphContactsTitleL(const TDesC& aContactTitle)
	{
	TBool isDigraphic( EFalse );
	// Go through the contactTitles one-by-one and check if they
	// include digraphs
	const TInt KDigraphLength(2);
	if ( aContactTitle.Length()>= KDigraphLength )
		{
		TPtrC substring = aContactTitle.Left(1);
		if( !iFindUtil->Interface()->MatchRefineL( aContactTitle, substring ) )
			{
			// The substring did not match the characters of the contactTitles
			// For example with Croatian locale the contactTitles "nj" does
			// not include the substring "n" because "nj" is a digraph
			isDigraphic = ETrue;
			}
		}
	return isDigraphic;
	}

void CPbk2AdaptiveSearchGridFiller::HandlePsResultsUpdate(
        RPointerArray<CPsClientData>& /*searchResults*/,
        RPointerArray<CPsPattern>& /*searchSeqs*/ )
    {
    
    }

void CPbk2AdaptiveSearchGridFiller::HandlePsError( TInt /*aErrorCode*/ )
    {
    
    }

void CPbk2AdaptiveSearchGridFiller::CachingStatus( TCachingStatus& aStatus, TInt& /*aError*/ )
    {
    TRAP_IGNORE(
    MVPbkContactViewBase* allContactsView = Phonebook2::Pbk2AppUi()->ApplicationServices().ViewSupplier().AllContactsViewL();
    
    const MPbk2FilteredViewStack* filteredView = dynamic_cast<const MPbk2FilteredViewStack*> ( iView );
    
    if ( aStatus >= ECachingComplete && filteredView && filteredView->Level() == 0 && &filteredView->BaseView() == allContactsView )
        {
        HBufC* string = iSearchString->AllocL();
        CleanupStack::PushL( string );
        StartFillingL( *iView, *string, ETrue );
        CleanupStack::PopAndDestroy( string  );
        }
        );
    }

TBool CPbk2AdaptiveSearchGridFiller::GridFromPsEngineL( const MVPbkContactViewBase& aView )
    {
    if ( iPsHandler == NULL )
        {
        return EFalse;
        }
    MPbk2ApplicationServices& appServices = Phonebook2::Pbk2AppUi()->ApplicationServices();
    MVPbkContactViewBase* allContactsView = appServices.ViewSupplier().AllContactsViewL();
    const MPbk2FilteredViewStack* filteredView = dynamic_cast<const MPbk2FilteredViewStack*> ( &aView );
        
    if ( filteredView && filteredView->Level() == 0 && &filteredView->BaseView() == allContactsView )
        {
        CPbk2StoreConfiguration& config = appServices.StoreConfiguration();
        CVPbkContactStoreUriArray* stores = NULL;
        stores = config.CurrentConfigurationL();
        if ( !stores || stores->Count() == 0 )
            {
            delete stores;
            return EFalse;
            }
        
        TInt count = stores->Count();
        CleanupStack::PushL(stores);
        
        CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( count );
        CleanupStack::PushL( array );
        
        for ( TInt i = 0; i < count; ++i)
            {
            TVPbkContactStoreUriPtr uriPtr = stores->operator[](i);
            array->AppendL( uriPtr.UriDes() );
            }

        TBool companyName = EFalse;
        TBuf<KPsAdaptiveGridStringMaxLen> gridChars;
        if( FeatureManager::FeatureSupported( KFeatureIdFfContactsCompanyNames ) )
            {
            companyName = ETrue;
            }
        iPsHandler->GetAdaptiveGridCharactersL( *array, *iSearchString, companyName, gridChars );
        
        CleanupStack::PopAndDestroy( array );
        CleanupStack::PopAndDestroy( stores );
        
        if ( !gridChars.Length() && iViewItemCount > 0 )
            {
            // grid should be created on standard way
            return EFalse;
            }
        if ( iKeyMap->Des().MaxLength() < gridChars.Length() )
            {
            iKeyMap = iKeyMap->ReAllocL( gridChars.Length() );
            }
        iKeyMap->Des().Copy( gridChars );
        
        delete iCurrentGrid;
        iCurrentGrid = NULL;
        iCurrentGrid = iKeyMap->Des().AllocL();

        iSearchField.SetAdaptiveGridChars( *iKeyMap );
        
        iInvalidateAdaptiveSearchGrid = EFalse;
        
        if ( iSetFocusToSearchGrid )
            {
            // set the focus to findbox
            iSearchField.DrawDeferred();
            iSetFocusToSearchGrid = EFalse;
            }
        AddKeyMapToCacheL( *iSearchString, *iKeyMap );
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

TBool CPbk2AdaptiveSearchGridFiller::IsActualTitleEmpty( const MVPbkViewContact& aContact )
    {
    TBool result = ETrue;
    const TInt fieldCount = aContact.Fields().FieldCount();
    if ( fieldCount > ENameCompanyPart )
        {
        const MVPbkBaseContactField& field = aContact.Fields().FieldAt( ENameCompanyPart );
        if ( iNameFormatter.IsTitleField( field ) )
            {
            return EFalse;
            }
        }
   
    if ( fieldCount > ENameFirstPart ) 
        {
        const MVPbkBaseContactField& field = aContact.Fields().FieldAt( ENameFirstPart );
        if ( iNameFormatter.IsTitleField( field ) )
            {
            const MVPbkContactFieldData& fieldData = field.FieldData();
            if ( fieldData.DataType() == EVPbkFieldStorageTypeText )
                {
                const TDesC& fieldText = MVPbkContactFieldTextData::Cast( fieldData ).Text();
                TInt length = fieldText.Length();
                    
                if ( length > 0 )
                    {
                    TInt firstNonSpaceChar = 0;
                    while ( firstNonSpaceChar < length 
                        && TChar( fieldText[firstNonSpaceChar] ).IsSpace() )
                        {
                        ++firstNonSpaceChar;
                        }
                    if ( firstNonSpaceChar != length )
                        {
                        result = EFalse;
                        }
                    }   
                }
            }
        }
    return result;
    }
// End of File
