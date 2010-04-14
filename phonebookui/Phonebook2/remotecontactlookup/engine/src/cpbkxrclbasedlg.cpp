/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Definition of the class CPbkxRclBaseDlg.
*
*/


#include "emailtrace.h"
#include <AknsListBoxBackgroundControlContext.h>
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <eikfrlb.h>
#include <eikfrlbd.h>
#include <eikclb.h>
#include <eikclbd.h>

#include "cpbkxrclbasedlg.h"
#include "cpbkxrclsearchresultlistbox.h"
#include "cpbkxrclresultinfolistbox.h"
#include "cpbkxrclsearchresultlistbox.h"
#include "cpbkxrclresultinfolistbox.h"
#include "engine.hrh"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CPbkxRclBaseDlg::CPbkxRclBaseDlg
// ---------------------------------------------------------------------------
//
CPbkxRclBaseDlg::CPbkxRclBaseDlg(
    TInt& aIndex,
    CDesCArray* aArray,
    MEikCommandObserver* aCommand,
    TBool aColumnListBox ) :
    CAknSelectionListDialog( aIndex, aArray, aCommand ),
    iListItemId( KAknsIIDQgnFsGrafContactsContent ),
    iListBottomId( KAknsIIDQsnBgColumnA ),
    iHighlightId( KAknsIIDQgnFsList ),
    iHighlightCenterId( KAknsIIDQgnFsListCenter ),
    iHighlightTextColorId( KAknsIIDFsHighlightColors ),
    iTextColorId( KAknsIIDFsTextColors ),
    iColumnListBox( aColumnListBox )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclBaseDlg::ConstructL
// ---------------------------------------------------------------------------
//
void CPbkxRclBaseDlg::ConstructL( TInt aResourceId )
    {
    FUNC_LOG;
    CAknSelectionListDialog::ConstructL( aResourceId );
    }

// ---------------------------------------------------------------------------
// CPbkxRclBaseDlg::~CPbkxRclBaseDlg
// ---------------------------------------------------------------------------
//
CPbkxRclBaseDlg::~CPbkxRclBaseDlg()
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// CPbkxRclBaseDlg::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CPbkxRclBaseDlg::HandleResourceChange( TInt aType )
    {
    FUNC_LOG;
    CEikDialog::HandleResourceChange( aType );
    if ( aType == KAknsMessageSkinChange ) 
        {
        UpdateGraphics();
        }
    else if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        
        // for some reason formatted cell listbox loses settings
        // when layout is switched
        if ( !iColumnListBox )
            {
            UpdateGraphics();
            }
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclBaseDlg::IsAcceptableListBoxType
// ---------------------------------------------------------------------------
//
TBool CPbkxRclBaseDlg::IsAcceptableListBoxType(
    TInt aControlType,
    TBool& aIsFormattedCellList ) const
    {
    FUNC_LOG;
    if ( aControlType == ERclSearchResultListBox )
        {
        aIsFormattedCellList = EFalse;
        return ETrue;
        }
    else if ( aControlType == ERclResultInfoListBox )
        {
        aIsFormattedCellList = ETrue;
        return ETrue;
        }
    else
        {
        return CAknSelectionListDialog::IsAcceptableListBoxType(
            aControlType,
            aIsFormattedCellList );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclBaseDlg::CreateCustomControlL
// ---------------------------------------------------------------------------
//
SEikControlInfo CPbkxRclBaseDlg::CreateCustomControlL( TInt aControlType )
    {
    FUNC_LOG;
    if ( aControlType == ERclSearchResultListBox )
        {
        CCoeControl* control = new ( ELeave ) CPbkxRclSearchResultListBox();
        SEikControlInfo info = { control, 0, 0 };
        return info;
        }
    else if ( aControlType == ERclResultInfoListBox )
        {
        CCoeControl* control = new ( ELeave ) CPbkxRclResultInfoListBox();
        SEikControlInfo info = { control, 0, 0 };
        return info;
        }
    else
        {
        return CAknSelectionListDialog::CreateCustomControlL( aControlType );
        }
    }

// ---------------------------------------------------------------------------
// CPbkxRclBaseDlg::UpdateGraphics
// ---------------------------------------------------------------------------
//
void CPbkxRclBaseDlg::UpdateGraphics()
    {
	FUNC_LOG;
 
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    
    CAknsListBoxBackgroundControlContext* bgContext = NULL;
    CFormattedCellListBoxItemDrawer* fItemDrawer = NULL;
    CColumnListBoxItemDrawer* cItemDrawer = NULL;
    
    // set first to this value, then change if those are not found
    iHighlightId = KAknsIIDQgnFsList;
    iHighlightCenterId = KAknsIIDQgnFsListCenter;

    if ( iColumnListBox )
        {
        CEikColumnListBox* listBox = 
            static_cast<CEikColumnListBox*>( ListBox() );
        cItemDrawer = listBox->ItemDrawer();
        bgContext = 
            static_cast<CAknsListBoxBackgroundControlContext*>( 
                cItemDrawer->ColumnData()->SkinBackgroundContext() );
        }
    else
        {
        CEikFormattedCellListBox* listBox = 
            static_cast<CEikFormattedCellListBox*>( ListBox() );
        fItemDrawer = listBox->ItemDrawer();
        bgContext = 
            static_cast<CAknsListBoxBackgroundControlContext*>( 
                fItemDrawer->FormattedCellData()->SkinBackgroundContext() );
        }
    
    if ( skin->GetCachedItemData( iListItemId ) )
        {
        bgContext->SetBitmap( iListItemId );
        bgContext->SetTiledBitmap( iListItemId );
        }
    
    if ( skin->GetCachedItemData( iListBottomId ) )
        {
        bgContext->SetBottomBitmap( iListBottomId );
        }

    if ( skin->GetCachedItemData( iHighlightId ) &&
         skin->GetCachedItemData( iHighlightCenterId ) )
        {
        if ( iColumnListBox )
            {
            cItemDrawer->ColumnData()->SetSkinHighlightFrame(
                &iHighlightId,
                &iHighlightCenterId );
            }
        else
            {
            fItemDrawer->FormattedCellData()->SetSkinHighlightFrame(
                &iHighlightId, 
                &iHighlightCenterId );
            }
        }
    else
        {
        // put defaults if fs specific items are not found
        iHighlightId = KAknsIIDQsnFrList;
        iHighlightCenterId = KAknsIIDQsnFrListCenter;
        if ( iColumnListBox )
            {
            cItemDrawer->ColumnData()->SetSkinHighlightFrame(
                &iHighlightId,
                &iHighlightCenterId );
            }
        else
            {
            fItemDrawer->FormattedCellData()->SetSkinHighlightFrame(
                &iHighlightId, 
                &iHighlightCenterId );
            }
        }

    }

