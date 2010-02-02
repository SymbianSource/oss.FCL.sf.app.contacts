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

#include <FindUtil.h>
#include <badesca.h>

const TInt KMaxAdaptiveGridCacheCount = 10;
const TInt KAdaptiveSearchKeyMapGranularity = 100;
const TInt KAdaptiveSearchRefineStep = 200;
const TInt KContactFormattingFlags = MPbk2ContactNameFormatter::EPreserveLeadingSpaces | MPbk2ContactNameFormatter::EReplaceNonGraphicChars;


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

		const TDesC& FindText() const
			{
			return *iFindText;
			}

		const TDesC& KeyMap() const
			{
			return *iKeyMap;
			}
	};

// --------------------------------------------------------------------------
// CPbk2ContactEditorDlg::CPbk2ContactEditorDlg
// --------------------------------------------------------------------------
//
CPbk2AdaptiveSearchGridFiller::CPbk2AdaptiveSearchGridFiller( CAknSearchField& aField, MPbk2ContactNameFormatter& aNameFormatter )
	: CActive( CActive::EPriorityStandard ), iSearchField( aField ), iNameFormatter( aNameFormatter ),
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
    }

void CPbk2AdaptiveSearchGridFiller::StartFilling( const MVPbkContactViewBase& aView, const TDesC& aSearchString )
	{
	CPbk2AdaptiveGrid* keyMap = KeyMapFromCache( aSearchString );

	if( keyMap )
		{
		iSearchField.SetAdaptiveGridChars( keyMap->KeyMap() );
		return;
		}

	delete iSearchString;
	iSearchString = NULL;

	iSearchString = aSearchString.AllocL();

	// If there is no search word, the user is not searching any contacts
	// so we should reset the array to prepare for the next searching.
    if ( iSearchString->Length()== 0 )
    	{
    	iDigraphContactsTitleArray.ResetAndDestroy();
    	}
	iView = &aView;

	iKeyMap->Des().Zero();


	iCounter = 0;

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
	TInt maxSpacesNumber = 0;

	if( stopCount > itemCount )
		{
		stopCount = itemCount;
		}

	for( ; iCounter < stopCount; iCounter++ )
		{
		const MVPbkViewContact& contact = iView->ContactAtL( iCounter );
		const TInt titleLength = iNameFormatter.MaxTitleLength( contact.Fields(), KContactFormattingFlags );
		HBufC* title = HBufC::NewLC( titleLength );
		TPtr ptrTitle = title->Des();
		iNameFormatter.GetContactTitle( contact.Fields(), ptrTitle, KContactFormattingFlags );
		BuildGridL( ptrTitle, *iSearchString, iKeyMap );
		
		// check number of spaces in the contact title
		TInt numberOfSpaces = NumberOfSpacesInString( ptrTitle );
		if ( numberOfSpaces > maxSpacesNumber )
		    {
		    maxSpacesNumber = numberOfSpaces;
		    }
		// Check if the contact's title include drgraphs,
		// if it is, add it to array to save.
		if ( IsDigraphContactsTitleL( ptrTitle ) )
			{			
			iDigraphContactsTitleArray.AppendL( title );
			CleanupStack::Pop(); //title
			}
		else
			{
			CleanupStack::PopAndDestroy(); //title
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
        	BuildGridL( ptrContactsTitle, *iSearchString, iKeyMap );
    		}
		}


	if( stopCount == itemCount )
		{
		SetAdaptiveGridCharsL( maxSpacesNumber );
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
		if( !aFindText.Compare( iAdaptiveGridCache[i]->FindText() ) )
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
	iAdaptiveGridCache.InsertL( keyMap, 0 );
	CleanupStack::Pop();//keyMap

	if( iAdaptiveGridCache.Count() > KMaxAdaptiveGridCacheCount )
		{
		delete iAdaptiveGridCache[0];
		iAdaptiveGridCache.Remove( 0 );
		}
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

void CPbk2AdaptiveSearchGridFiller::SetAdaptiveGridCharsL(  const TInt aMaxSpacesNumber )
	{
	TPtr ptr = iKeyMap->Des();

	//To do upper case for all characters
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

    // Add space character only if user typed already some characters
    // in the find pane and more spaces can be found in contacts than
    // in the current search string.
    TInt searchTextLength = iSearchField.TextLength();
    HBufC* searchText = HBufC::NewL( searchTextLength );
	TPtr ptrSearchText = searchText->Des();
	iSearchField.GetSearchText( ptrSearchText );
    if ( searchTextLength > 0 
        && NumberOfSpacesInString( ptrSearchText ) < aMaxSpacesNumber  )
        {
        ptr.Append( TChar( ' ' ) );
        }
    delete searchText;
    searchText = NULL;
    
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

	iInvalidateAdaptiveSearchGrid = EFalse;

	delete iCurrentGrid;
	iCurrentGrid = NULL;
	iCurrentGrid = iKeyMap->Des().AllocL();

	iSearchField.SetAdaptiveGridChars( *iKeyMap );
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
    const TInt KGranularity = 2;
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( KGranularity );
    CleanupStack::PushL( array );
    const TInt textLength = aText.Length();
    for ( TInt beg = 0; beg < textLength; ++beg )
        {
        // Skip separators before next word
        if ( !iNameFormatter.IsFindSeparatorChar( aText[beg] ) )
            {
            // Scan the end of the word
            TInt end = beg;
            for (; end < textLength &&
                   !iNameFormatter.IsFindSeparatorChar( aText[end] );
                ++end )
                {
                }
            const TInt len = end - beg;
            // Append found word to the array
          	array->AppendL( aText.Mid( beg,len ) );
            // Scan for next word
            beg = end;
            }
        }

    if ( array->MdcaCount() == 0 && textLength > 0 )
        {
        // aText is all word separator characters
        // -> make a "word" out of those
        array->AppendL( aText );
        }
    return array;
    }



void CPbk2AdaptiveSearchGridFiller::BuildGridL( const TDesC& aContactTitle, const TDesC& aSearchString, HBufC*& aKeyMap )
	{
	CDesC16Array* contactTitles = SplitContactFieldTextIntoArrayLC( aContactTitle );
	CDesC16Array* searchWords = SplitContactFieldTextIntoArrayLC( aSearchString );

	//in searchWords list, the last term is the only one which generates new keymap characters
	//other ones are used only for matching

	if( searchWords->MdcaCount() == 0 )
		{
		searchWords->AppendL( KNullDesC );
		}

	if( aSearchString.Length() > 0 )
		{
		if( aSearchString[ aSearchString.Length() - 1 ] == TChar( ' ' ) )
			{
			//because we now start new search term, we must add KNullDesC so we
			//can find the matching new words
			searchWords->AppendL( KNullDesC );
			}
		}


	const TInt searchWordCount = searchWords->MdcaCount();

	TBool contactMatch( searchWordCount == 1 );

	for( TInt i( 0 ); i < searchWordCount; i++ )
		{
		TInt contactTitleCount = contactTitles->MdcaCount();

		TBool contactTitleMatch( EFalse );

		for( TInt j( 0 ); j < contactTitleCount; j++ )
			{
			TPtrC searchWord = searchWords->MdcaPoint( i );
			TPtrC contactTitle = contactTitles->MdcaPoint( j );

			const TBool lastSearchWord = ( i == searchWordCount - 1 );

			TBool match( EFalse );

			if( lastSearchWord )
				{
				if( !contactMatch )
					{
					//none of the previous words didin't match, so why this is not filtered?
					//marked contact!?
					}
				else
					{
					match = iFindUtil->Interface()->MatchAdaptiveRefineL( contactTitle, searchWord, aKeyMap );
					}
				}
			else
				{
				match = iFindUtil->Interface()->MatchRefineL( contactTitle, searchWord );
				}

			if( match )
				{

				if( !contactTitleMatch )
					{
					contactTitles->Delete( j );
					//allow one search word to take away only one contactTitle
					contactTitleMatch = ETrue;
					//for loop must go from first contact title to last
					//to be consistent with match functionality of VPbk.
					j--; contactTitleCount--;
					}
				contactMatch = ETrue;
				}
			}

		}

	CleanupStack::PopAndDestroy( 2 );//contactTitle, searchWords
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
// End of File
