/*
* Copyright (c) 2009-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 contact view double list box.
*
*/


// INCLUDE FILES
#include "cpbk2contactviewdoublelistbox.h"

#include <eikclbd.h>
#include <AknLayout2ScalableDef.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <aknlayoutscalable_apps.cdl.h>
#include "cpbk2doublelistboxmodelcmddecorator.h"

//Virtual phonebook
#include <MVPbkContactViewBase.h>
#include <MVPbkViewContact.h>
#include <MVPbkContactLink.h>

//Internal
#include "cpbk2contactviewdoublelistboxmodel.h"
#include "CPbk2IconArray.h"



// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::CPbk2ContactViewDoubleListBox
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBox::CPbk2ContactViewDoubleListBox(
    CCoeControl& aContainer,
    MPbk2ContactUiControlExtension* aUiExtension,
    CPbk2PredictiveSearchFilter& aSearchFilter,
    MVPbkContactViewBase& aView,
    CPbk2ThumbnailManager& aThumbManager ) :
    CPbk2ContactViewListBox( aContainer, aUiExtension, aSearchFilter ),
    iView( aView  ),
    iThumbManager( aThumbManager )
    {
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::~CPbk2ContactViewDoubleListBox
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBox::~CPbk2ContactViewDoubleListBox()
    {
    // reset thumbnails
    iThumbManager.Reset();
    
    // icon array is destroyed, remove it from the manager
    iThumbManager.RemoveIconArray();
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::NewL
// --------------------------------------------------------------------------
//
CPbk2ContactViewDoubleListBox* CPbk2ContactViewDoubleListBox::NewL(
    CCoeControl& aContainer,
    TResourceReader& aResourceReader,
    const CVPbkContactManager& aManager, 
    MVPbkContactViewBase& aView,
    MPbk2ContactNameFormatter& aNameFormatter,
    CPbk2StorePropertyArray& aStoreProperties,
    MPbk2ContactUiControlExtension* aUiExtension,
    CPbk2PredictiveSearchFilter& aSearchFilter,
    CPbk2ThumbnailManager& aThumbManager )
    {
    CPbk2ContactViewDoubleListBox* self =
        new ( ELeave ) CPbk2ContactViewDoubleListBox( aContainer,aUiExtension, aSearchFilter, aView, aThumbManager );
    CleanupStack::PushL( self );
    self->ConstructL( aContainer, aResourceReader, aManager,
        aView, aNameFormatter, aStoreProperties );
    
    
    
    CleanupStack::Pop( self );
    return self;
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::ConstructL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBox::ConstructL(
        CCoeControl& aContainer, 
        TResourceReader& aResourceReader,
        const CVPbkContactManager& aManager, 
        MVPbkContactViewBase& aView,
        MPbk2ContactNameFormatter& aNameFormatter,
        CPbk2StorePropertyArray& aStoreProperties )
    {
    CPbk2ContactViewListBox::ConstructL( aContainer, 
										aResourceReader, 
										aManager,
										aView, 
										aNameFormatter, 
										aStoreProperties );
    
    // set icon array for the manager
    iThumbManager.SetPbkIconArray( static_cast<CPbk2IconArray*>( ItemDrawer()->ColumnData()->IconArray() ) );
    iThumbManager.SetDefaultIconId( iResourceData.iDefaultIconId );
    }


// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::DoCreateModelL
// --------------------------------------------------------------------------
//
CPbk2ContactViewListBoxModel* CPbk2ContactViewDoubleListBox::DoCreateModelL( 
        		CPbk2ContactViewListBoxModel::TParams params )
	{
	return CPbk2ContactViewDoubleListBoxModel::NewL( params, iThumbManager );
	}


// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::DoCreateDecorator
// --------------------------------------------------------------------------
//
CPbk2ListboxModelCmdDecorator* CPbk2ContactViewDoubleListBox::DoCreateDecoratorL( 
		const CPbk2IconArray& aIconArray,
		TPbk2IconId aEmptyIconId,
		TPbk2IconId aDefaultIconId )
	{
	return CPbk2DoubleListboxModelCmdDecorator::NewL(
					aIconArray,
					aEmptyIconId,
					aDefaultIconId );
	}


// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::SizeChanged
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBox::SizeChanged()
    {
    CPbk2ContactViewListBox::SizeChanged();
    TRAP_IGNORE( SizeChangedL() );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::SizeChangedL
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBox::SizeChangedL()
    {
    
    // Modify list item format and layout. 
    // Text format: "0\tContactName\tStatusText\t1"
    // Icons: 
    //  0: Contact thumbnail
    //  1: Default listbox icon 1
    //
    // Layout: list_double_large_graphic_phob2_pane
    // ------------------------------------------------
    // |       |                                      |
    // |       | Contact Name                   [1]   |
    // |  [0]  |                                      |
    // |       | Status Text                          |
    // |       |                                      |
    // ------------------------------------------------
    //
    CColumnListBoxItemDrawer* itemDrawer = ItemDrawer();
    CColumnListBoxData* data = itemDrawer->ColumnData();

    // LIST PANE LAYOUT
    TAknWindowComponentLayout listPaneLayout( TAknWindowComponentLayout::Compose(
        AknLayoutScalable_Apps::phob2_listscroll_pane( 0 ),
        TAknWindowComponentLayout::Compose( 
            AknLayoutScalable_Apps::phob2_list_pane( 0 ), 
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane( 0 ) ) ) );    

    
    TAknLayoutRect listPaneLayoutRect;
    listPaneLayoutRect.LayoutRect( TRect( Size() ), listPaneLayout.LayoutLine() );
    TRect listPaneRowRect( listPaneLayoutRect.Rect() );
    
    SetHorizontalMargin( listPaneRowRect.iTl.iX );
    SetVerticalMargin( listPaneRowRect.iTl.iY );
    SetItemHeightL( listPaneRowRect.Height() );
    itemDrawer->SetItemCellSize( listPaneRowRect.Size() );
    
    // reset subcell definitions
    data->ResetSLSubCellArray();
    
    //THUMBNAIL SIZE
    TAknWindowComponentLayout thumbnailLayout( 
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane_g1( 0 ) );
    
    TAknLayoutRect thumbnailLayoutRect;
    thumbnailLayoutRect.LayoutRect( listPaneLayoutRect.Rect(), thumbnailLayout.LayoutLine() );
    TRect thumbnailRect( thumbnailLayoutRect.Rect() );
    
    // ADD ICON SIZE TO MANAGER
    if( &iThumbManager )
		{
		iThumbManager.SetThumbnailIconSize( thumbnailRect.Size() );
		}
    
    
    // 1. THUMBNAIL ICON
    data->SetGraphicSubCellL( 0, thumbnailLayout.LayoutLine() );	// cell index 0
    
    // 2. TEXT ROW 1 (Contact Name)
    TAknTextComponentLayout cntNameLayout( 
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane_t1( 0 ) );
    data->SetTextSubCellL( 1, cntNameLayout );// cell index 1

    // 3. TEXT ROW 2 (Status Text)
    TAknTextComponentLayout statusTextLayout( 
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane_t2( 0 ) );
    data->SetTextSubCellL( 2, statusTextLayout );	// cell index 2
    
    // 4. LISTBOX DEFAULT GFX 1
    TAknWindowLineLayout gfxLayout( 
            AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane_g2( 0 ) );
    data->SetGraphicSubCellL( 3, gfxLayout );	// cell index 3
    
    
    // Conditional subcells must be added in priority order!
    // Text row #1 variation for 1 post icon
    cntNameLayout = AknLayoutScalable_Apps::list_double_large_graphic_phob2_pane_t1( 1 );
    data->SetConditionalSubCellL( 3, cntNameLayout, 1 );	// cell index 3

    // Marking
    _LIT( KMarkReplacement, "-1" );
    itemDrawer->SetItemMarkPosition( 3 );				// cell index 3 
    itemDrawer->SetItemMarkReplacement( KMarkReplacement );
    }

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::ThumbnailLoadingComplete
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBox::ThumbnailLoadingComplete( TInt aError, TInt aIndex )
	{
	// there is a thumbnail
	if( aError == KErrNone )
		{
		View()->DrawItem( aIndex );
		}
	}

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::ThumbnailRemoved
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBox::ThumbnailRemoved( const MVPbkContactLink& /*aLink*/, TInt aIndex )
	{
	View()->DrawItem( aIndex );
	}

// --------------------------------------------------------------------------
// CPbk2ContactViewDoubleListBox::LoadingFinished
// --------------------------------------------------------------------------
//
void CPbk2ContactViewDoubleListBox::LoadingFinished()
	{
	DrawNow();
	}


// End of File
