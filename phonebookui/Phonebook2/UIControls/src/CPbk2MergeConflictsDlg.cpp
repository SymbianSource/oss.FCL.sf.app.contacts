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
* Description: 
*       Provides Conflict resolution dialog for Contact Merge command.
*
*/

#include <avkon.rsg>
#include <aknlists.h>
#include <eikclbd.h>
#include <AknIconArray.h>
#include <eikapp.h>
#include <gulicon.h>
#include <CPbk2IconFactory.h>
#include <Pbk2UID.h>
#include <Pbk2IconId.hrh>
#include <TPbk2IconId.h>
#include <StringLoader.h>
#include <aknnavide.h>
#include <akntitle.h>
#include <Pbk2UIControls.rsg>
#include <MPbk2MergeResolver.h>
#include <MPbk2MergeConflict.h>
#include "CPbk2MergeConflictsDlg.h"

/// Unnamed namespace for local definitions
namespace {

const TInt KSelectionCount = 2; // number of selections is fixed
const TInt KFirstRadioButt = 1;
const TInt KSecondRadioButt = 2;
_LIT( KSelectedRadioIconIndex, "0" );
_LIT( KDeselectedRadioIconIndex, "1" );
_LIT( KLabelRowFormat, "\t%S\t\t" );
_LIT( KRadioRowFormat, "%S\t   %S\t\t" );

} /// namespace

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::CPbk2MergeConflictsDlg
// -----------------------------------------------------------------------------
//
CPbk2MergeConflictsDlg::CPbk2MergeConflictsDlg( 
        MPbk2MergeResolver* aConflictResolver, TBool aPhotoConflictExpected ):
        iConflictResolver( aConflictResolver ), iPhotoConflictExpected( aPhotoConflictExpected )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::~CPbk2MergeConflictsDlg
// -----------------------------------------------------------------------------
//
CPbk2MergeConflictsDlg::~CPbk2MergeConflictsDlg()
    {
    iRadioControls.Close();
    delete iNaviDecorator;
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CPbk2MergeConflictsDlg* CPbk2MergeConflictsDlg::NewL( 
        MPbk2MergeResolver* aConflictResolver, TBool aPhotoConflictExpected )
    {
    CPbk2MergeConflictsDlg* self = new (ELeave) 
        CPbk2MergeConflictsDlg( aConflictResolver, aPhotoConflictExpected );
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self;
    return self;
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::ConstructL()
    {
    SetNaviPaneL();
    SetTitlePaneL( ETrue );
    CAknDialog::ConstructL( R_AVKON_MENUPANE_EMPTY );
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::PreLayoutDynInitL()
    {
    CAknDialog::PreLayoutDynInitL();

    CEikButtonGroupContainer& cba = ButtonGroupContainer();
    if ( iPhotoConflictExpected )
        {
        cba.SetCommandSetL( R_PBK2_SOFTKEYS_NEXT_CANCEL_SELECT );
        }
    else
        {
        cba.SetCommandSetL( R_PBK2_SOFTKEYS_DONE_CANCEL_SELECT );
        }
    
    CEikCba* eikCba = static_cast<CEikCba*>( cba.ButtonGroup() );
    if( eikCba )
        {
        eikCba->EnableItemSpecificSoftkey( EFalse );
        }
    
    iListBox = static_cast<CEikColumnListBox*>( Control( ESelectionListControl ) );
    
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
// CPbk2MergeConflictsDlg::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CPbk2MergeConflictsDlg::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyDone || aButtonId == EAknSoftkeyCancel )
        {
        ResolveConflicts();
        TRAP_IGNORE( SetTitlePaneL( EFalse ) );
        return ETrue;
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CPbk2MergeConflictsDlg::OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse result = EKeyWasNotConsumed;
    
    if ( aKeyEvent.iCode == EKeyEscape )
       {
       result = CEikDialog::OfferKeyEventL( aKeyEvent, aType );
       return result;
       }
    
    result = iListBox->OfferKeyEventL( aKeyEvent, aType );
    
    if ( aKeyEvent.iCode == EKeyDownArrow )
        {
        TInt rowType = 0; 
        TInt currentSelection = iListBox->CurrentItemIndex();
        rowType = RowType( currentSelection );
        TInt itemsCount = iListBox->Model()->ItemTextArray()->MdcaCount();
        
        if ( rowType == ELabelRow )
          {
          iListBox->SetCurrentItemIndexAndDraw( currentSelection + 1 );
          }
        }
    else if ( aKeyEvent.iCode == EKeyUpArrow )
        {
        TInt rowType = 0; 
        TInt currentSelection = iListBox->CurrentItemIndex();
        rowType = RowType( currentSelection );
        TInt itemsCount = iListBox->Model()->ItemTextArray()->MdcaCount();
        
        if ( rowType == ELabelRow )
          {
          if ( currentSelection - 1 >= 0 )
              {
              iListBox->SetCurrentItemIndexAndDraw( currentSelection - 1 );
              }
          else
              {
              // select last one
              iListBox->SetCurrentItemIndexAndDraw( itemsCount - 1 );
              }
          }
        }
    else if ( aType == EEventKeyDown && aKeyEvent.iScanCode == EStdKeyDevice3 )
        {
        SwitchRadioButtonL();
        }
   
    return result;
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::HandleListBoxEventL
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::HandleListBoxEventL( CEikListBox* /*aListBox*/, TListBoxEvent aEventType )
    {     
    if( AknLayoutUtils::PenEnabled() )  
        {
        if ( aEventType == EEventItemSingleClicked || aEventType == EEventEnterKeyPressed)
            {
            SwitchRadioButtonL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::SetIconsL
// Sets icons for the listbox.
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::SetIconsL()
    {
    CPbk2IconFactory* iconFactory = CPbk2IconFactory::NewLC();
    CArrayPtr<CGulIcon>* iconList = new (ELeave) CAknIconArray( 2 );
    CleanupStack::PushL( iconList );

    TPbk2IconId radioOnId = TPbk2IconId( TUid::Uid( KPbk2UID3 ), EPbk2qgn_prop_radiobutt_on );
    CGulIcon* radioOn = iconFactory->CreateIconL( radioOnId );
    CleanupStack::PushL( radioOn );
    iconList->AppendL( radioOn );
    CleanupStack::Pop( radioOn );
    
    TPbk2IconId radioOffId = TPbk2IconId( TUid::Uid( KPbk2UID3 ), EPbk2qgn_prop_radiobutt_off );
    CGulIcon* radioOff = iconFactory->CreateIconL( radioOffId );
    CleanupStack::PushL( radioOff );
    iconList->AppendL( radioOff );
    CleanupStack::Pop( radioOff );
    
    iListBox->ItemDrawer()->ColumnData()->SetIconArray( iconList );
    CleanupStack::Pop( iconList );
    CleanupStack::PopAndDestroy( iconFactory );
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::SetItemsL
// Sets items to the listbox.
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::SetItemsL()
    {    
    TInt conflictsCount = iConflictResolver->CountConflicts();
    CDesCArray* itemArray = new ( ELeave ) CDesCArrayFlat( 3 );

    for ( TInt i = 0; i < conflictsCount; i++ )
        {
        MPbk2MergeConflict& conflict = iConflictResolver->GetConflictAt( i );
        TInt type = conflict.GetConflictType();
        if ( type != EPbk2ConflictTypeImage )
            {
            HBufC* label = conflict.GetLabelLC();
            RBuf listItem;
            listItem.Create( KLabelRowFormat().Length() + label->Length() );
            listItem.Format( KLabelRowFormat, label );
            itemArray->AppendL( listItem );
            CleanupStack::PopAndDestroy( label );
            listItem.Close();
            
            HBufC* first = conflict.GetTextLC( EPbk2ConflictedFirst );
            listItem.Create( KRadioRowFormat().Length() + first->Length() );
            listItem.Format( KRadioRowFormat, &KSelectedRadioIconIndex, first );
            itemArray->AppendL( listItem );
            CleanupStack::PopAndDestroy( first );
            listItem.Close();
            
            HBufC* second = conflict.GetTextLC( EPbk2ConflictedSecond );
            listItem.Create( KRadioRowFormat().Length() + second->Length() );
            listItem.Format( KRadioRowFormat, &KDeselectedRadioIconIndex, second );
            itemArray->AppendL( listItem );
            CleanupStack::PopAndDestroy( second );
            listItem.Close();
            }
        
        TRadioControl radio;
        radio.iBeginIndex = IndexOfNewRadioControl();
        radio.iSelected = KFirstRadioButt;
        iRadioControls.Append( radio );
        
        }

    iListBox->Model()->SetItemTextArray( itemArray );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    
    iListBox->HandleItemAdditionL();
    
    TListItemProperties properties;
    properties.SetUnderlined( ETrue );
    TInt count = iRadioControls.Count();
    for ( TInt i= 0; i < count; i++ )
        {
        iListBox->ItemDrawer()->SetPropertiesL( iRadioControls[i].iBeginIndex , properties );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::RowType
// -----------------------------------------------------------------------------
//
TInt CPbk2MergeConflictsDlg::RowType( TInt aIndex )
    {
    TInt listboxItemsCount = iListBox->Model()->ItemTextArray()->MdcaCount();
    if ( aIndex >= listboxItemsCount || aIndex < 0 )
        {
        return KErrNotFound;
        }
    
    TInt count = iRadioControls.Count();
    for ( TInt i= 0; i < count; i++ )
        {
        if ( iRadioControls[i].iBeginIndex > aIndex )
            {
            break;
            }
        if ( iRadioControls[i].iBeginIndex == aIndex )
            {
            return ELabelRow;
            }
        }
    return ESelectionRow;
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::SwitchRadioButtonL
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::SwitchRadioButtonL()
    {
    TInt currentSelection = iListBox->CurrentItemIndex();
    TInt rowType = 0; 
    rowType = RowType( currentSelection );

    if ( rowType == ESelectionRow )
        {
        // do nothing if selected
        if ( IsRadioSelected( currentSelection ) )
            {
            return;
            }
        }
    else
        {
        return;
        }
    
    TRadioControl radio = { 0, 0 };
    TInt index = 0;
    TInt count = iRadioControls.Count();
    for (  index = 0; index < count; index++ )
        {
        if ( iRadioControls[index].iBeginIndex < currentSelection && 
             iRadioControls[index].iBeginIndex + KSelectionCount >= currentSelection )
            {
            radio = iRadioControls[index];
            break;
            }
        }
    
    CDesCArray* itemArray = static_cast<CDesC16ArrayFlat*>( iListBox->Model()->ItemTextArray() );
    // deselect item
    TInt selectedItem = radio.iBeginIndex + radio.iSelected;
    TPtrC oldText = itemArray->MdcaPoint( selectedItem );
    RBuf newText;
    newText.CreateL( oldText );
    newText.Replace( 0, KDeselectedRadioIconIndex().Length(), KDeselectedRadioIconIndex() );
    itemArray->Delete( selectedItem );
    CleanupClosePushL( newText );
    itemArray->InsertL( selectedItem, newText );
    CleanupStack::PopAndDestroy( &newText );
    
    // select new item
    oldText.Set( itemArray->MdcaPoint( currentSelection ) );
    newText.CreateL( oldText );
    newText.Replace( 0, KSelectedRadioIconIndex().Length(), KSelectedRadioIconIndex() );
    itemArray->Delete( currentSelection );
    CleanupClosePushL( newText );
    itemArray->InsertL( currentSelection, newText );
    CleanupStack::PopAndDestroy( &newText );
    
    iRadioControls[index].iSelected = currentSelection - radio.iBeginIndex;

    iListBox->DrawDeferred();
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::IsRadioSelected
// -----------------------------------------------------------------------------
//
TBool CPbk2MergeConflictsDlg::IsRadioSelected( TInt aIndex )
    {
    TInt count = iRadioControls.Count();
    for ( TInt i= 0; i < count; i++ )
        {
        if ( iRadioControls[i].iBeginIndex > aIndex )
            {
            break;
            }
        if ( iRadioControls[i].iBeginIndex + iRadioControls[i].iSelected == aIndex )
            {
            return ETrue;
            }
        }
    return EFalse; 
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::IndexOfNewRadioControl
// -----------------------------------------------------------------------------
//
TInt CPbk2MergeConflictsDlg::IndexOfNewRadioControl()
    {
    TInt count = iRadioControls.Count();
    if ( count )
        {
        return iRadioControls[count-1].iBeginIndex + KSelectionCount + 1;
        }
    else
        {
        return 0;
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::ResolveConflicts
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::ResolveConflicts()
    {
    TInt syncIndex = 0;
    TInt count = iConflictResolver->CountConflicts();
    for ( TInt i= 0; i < count; i++ )
        {
        MPbk2MergeConflict& conflict = iConflictResolver->GetConflictAt( i );
        TInt type = conflict.GetConflictType();
        if ( type != EPbk2ConflictTypeImage )
            {
            if ( iRadioControls[i + syncIndex].iSelected == KFirstRadioButt )
                {
                conflict.ResolveConflict( EPbk2ConflictedFirst );
                }
            else if ( iRadioControls[i + syncIndex].iSelected == KSecondRadioButt )
                {
                conflict.ResolveConflict( EPbk2ConflictedSecond );
                }
            }
        else
            {
            syncIndex--;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::SetNaviPaneL
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::SetNaviPaneL()
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    if ( statusPane && statusPane->PaneCapabilities( TUid::Uid( EEikStatusPaneUidNavi ) ).IsPresent() )
        {
        CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*>
            ( statusPane->ControlL ( TUid::Uid( EEikStatusPaneUidNavi ) ) );
            
        HBufC* naviLabel = StringLoader::LoadLC( R_QTN_PHOB_TITLE_SELECT_VALUE );

        iNaviDecorator = naviPane->CreateNavigationLabelL( *naviLabel );
        CleanupStack::PopAndDestroy( naviLabel );   
        naviPane->PushL( *iNaviDecorator );
        }
    }

// -----------------------------------------------------------------------------
// CPbk2MergeConflictsDlg::SetTitlePaneL
// -----------------------------------------------------------------------------
//
void CPbk2MergeConflictsDlg::SetTitlePaneL( TBool aCustom )
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
