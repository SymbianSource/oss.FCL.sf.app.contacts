/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*       Provides Image conflict resolution dialog for Contact Merge command.
*
*/

#include <aknlists.h>
#include <AknIconArray.h>
#include <eikapp.h>
#include <gulicon.h>
#include <StringLoader.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <Pbk2UIControls.rsg>
#include <MPbk2MergeConflict.h>
#include "CPbk2MergePhotoConflictDlg.h"

/// Unnamed namespace for local definitions
namespace {

_LIT( KPictureRowFormat, "%d\t   %S\t\t" );

}

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::CPbk2MergePhotoConflictDlg
// -----------------------------------------------------------------------------
//
CPbk2MergePhotoConflictDlg::CPbk2MergePhotoConflictDlg( CFbsBitmap* aFirstImage,
                                                        CFbsBitmap* aSecondImage, 
                                                        TInt* aResult ):
        iFirstImage( aFirstImage ), iSecondImage( aSecondImage ), iSelectedItem( aResult )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::~CPbk2MergePhotoConflictDlg
// -----------------------------------------------------------------------------
//
CPbk2MergePhotoConflictDlg::~CPbk2MergePhotoConflictDlg()
    {
    delete iNaviDecorator;
    delete iSelectedString;
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2MergePhotoConflictDlg* CPbk2MergePhotoConflictDlg::NewL( CFbsBitmap* aFirstImage,
                                                                       CFbsBitmap* aSecondImage, 
                                                                       TInt* aResult )
    {
    CPbk2MergePhotoConflictDlg* self = 
        new (ELeave) CPbk2MergePhotoConflictDlg( aFirstImage, aSecondImage, aResult );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::ConstructL()
    {
    SetNaviPaneL();
    SetTitlePaneL( ETrue );
    *iSelectedItem = EPbk2ConflictedFirst;
    iSelectedString = StringLoader::LoadL( R_QTN_PHOB_TITLE_SELECTED );
    CAknDialog::ConstructL( R_AVKON_MENUPANE_EMPTY );
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::PreLayoutDynInitL()
    {
    CAknDialog::PreLayoutDynInitL();
    
    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    CEikCba* eikCba = static_cast<CEikCba*>( cba.ButtonGroup() );
    if( eikCba )
        {
        eikCba->EnableItemSpecificSoftkey( EFalse );
        }
    
    iListBox = static_cast<CEikFormattedCellListBox*>( Control( 1 ) );
    
    SetIconsL();
    SetItemsL();
    
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
            CEikScrollBarFrame::EOff,
            CEikScrollBarFrame::EAuto );
  
    iListBox->HandleItemAdditionL();
    iListBox->SetListBoxObserver( this ); // MEikListBoxObserver
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CPbk2MergePhotoConflictDlg::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyDone || aButtonId == EAknSoftkeyCancel )
        {
        TRAP_IGNORE( SetTitlePaneL( EFalse ) );
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CPbk2MergePhotoConflictDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result = EKeyWasNotConsumed;
    
    if ( aKeyEvent.iCode == EKeyEscape )
       {
       result = CEikDialog::OfferKeyEventL( aKeyEvent, aType );
       return result;
       }
    
    result = iListBox->OfferKeyEventL( aKeyEvent, aType );
    
    if ( aType == EEventKeyDown && aKeyEvent.iScanCode == EStdKeyDevice3 )
        {
        SwitchRadioButtonL();
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {     
    if( AknLayoutUtils::PenEnabled() )  
        {
        if ( aEventType == EEventItemSingleClicked )
            {
            SwitchRadioButtonL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::SetIconsL
// Sets icons for the listbox.
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::SetIconsL()
    {
    if ( iFirstImage == NULL || iSecondImage == NULL )
        {
        User::Leave( KErrArgument );
        }
    
    CArrayPtr<CGulIcon>* iconList = new (ELeave) CAknIconArray( 2 );
    CleanupStack::PushL( iconList );

    CGulIcon* first = CGulIcon::NewL( iFirstImage );
    first->SetBitmapsOwnedExternally( ETrue );
    CleanupStack::PushL( first );
    iconList->AppendL( first );
    CleanupStack::Pop( first );
    
    CGulIcon* second = CGulIcon::NewL( iSecondImage );
    second->SetBitmapsOwnedExternally( ETrue );
    CleanupStack::PushL( second );
    iconList->AppendL( second );
    CleanupStack::Pop( second );
    
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconList );
    CleanupStack::Pop( iconList );
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::SetItemsL
// Sets items to the listbox.
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::SetItemsL()
    {    
    CDesCArray* itemArray = new ( ELeave ) CDesCArrayFlat( 2 );
    
    RBuf listItem;
    listItem.Create( KPictureRowFormat().Length() + iSelectedString->Length() );
    listItem.Format( KPictureRowFormat, EPbk2ConflictedFirst, iSelectedString );
    itemArray->AppendL( listItem );
    listItem.Close();
    
    listItem.Create( KPictureRowFormat().Length() + iSelectedString->Length() );
    listItem.Format( KPictureRowFormat, EPbk2ConflictedSecond, &KNullDesC );
    itemArray->AppendL( listItem );
    listItem.Close();

    iListBox->Model()->SetItemTextArray( itemArray );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    iListBox->HandleItemAdditionL();
    }

// -----------------------------------------------------------------------------
// CPbk2MergePhotoConflictDlg::SwitchRadioButtonL
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::SwitchRadioButtonL()
    {
    TInt itemIndex = iListBox->CurrentItemIndex();
    if ( *iSelectedItem != itemIndex)
        {
        CDesCArray* itemArray = static_cast<CDesC16ArrayFlat*>( iListBox->Model()->ItemTextArray() );
        
        // deselect item
        RBuf listItem;
        listItem.Create( KPictureRowFormat().Length() );
        listItem.Format( KPictureRowFormat, *iSelectedItem , &KNullDesC );
        itemArray->Delete( *iSelectedItem );
        itemArray->InsertL( *iSelectedItem, listItem );
        listItem.Close();
        
        // select item
        listItem.Create( KPictureRowFormat().Length() + iSelectedString->Length() );
        listItem.Format( KPictureRowFormat, itemIndex, iSelectedString );
        itemArray->Delete( itemIndex );
        itemArray->InsertL( itemIndex, listItem );
        listItem.Close();
        
        *iSelectedItem = itemIndex;
        iListBox->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::SetNaviPaneL
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::SetNaviPaneL()
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities( TUid::Uid( EEikStatusPaneUidNavi ) ).IsPresent() )
        {
        CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*>
            ( statusPane->ControlL ( TUid::Uid( EEikStatusPaneUidNavi ) ) );
            
        HBufC* naviLabel = StringLoader::LoadLC( R_QTN_PHOB_TITLE_SELECT_IMAGE );

        iNaviDecorator = naviPane->CreateNavigationLabelL( *naviLabel );
        CleanupStack::PopAndDestroy( naviLabel );   
        naviPane->PushL( *iNaviDecorator );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::SetTitlePaneL
// -----------------------------------------------------------------------------
//
void CPbk2MergePhotoConflictDlg::SetTitlePaneL( TBool aCustom )
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities( TUid::Uid( EEikStatusPaneUidTitle ) ).IsPresent() )
        {
        CAknTitlePane* titlePane = static_cast<CAknTitlePane*>
            ( statusPane->ControlL ( TUid::Uid( EEikStatusPaneUidTitle ) ) );
        
        if ( aCustom )
            {
            HBufC* title = StringLoader::LoadLC( R_QTN_PHOB_TITLE_CONFLICT_RESOLUTION );
            titlePane->SetTextL( *title );
            CleanupStack::PopAndDestroy( title );  
            }
        else
            {
            titlePane->SetTextToDefaultL();
            }
        }
    }
