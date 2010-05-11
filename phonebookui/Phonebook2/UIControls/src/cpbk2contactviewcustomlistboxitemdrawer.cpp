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
* Description: PhoneBook2 custom control. Code has been modified to
*       suit PhoneBook2 requirements. See CColumnListBoxItemDrawer
*       in EIKCLB.CPP
*       Ensure that this piece of code is in sync with Avkon EIKCLB.CPP(CColumnListBoxItemDrawer)
*
*/


#include <barsread.h>
#include <gdi.h>
#include <gulicon.h>
#include <gulutil.h>

#include <eikenv.h>
#include <eiklbi.h>
#include <eiktxlbm.h>
#include <eikedwin.h>
#include <eiklbv.h>
#include <AknUtils.h>
#include <eiksfont.h>
#include <AknsDrawUtils.h>
#include <AknsControlContext.h>
#include <aknlists.h>
#include <eikpanic.h>
#include <eikcoctlpanic.h>
#include <aknlayoutscalable_avkon.cdl.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#endif //RD_UI_TRANSITION_EFFECTS_LIST
#include <AknTasHook.h>

//Start of Code Added for PhoneBook2
//Added for PhoneBook2
#include "cpbk2contactviewcustomlistboxdata.h"
#include "cpbk2contactviewcustomlistboxitemdrawer.h"
//End of Code Added for PhoneBook2

//Note:
//****
//PBK2_AVKON_TOUCH_MARKINGMODE_CHANGES 
//Define this Flag once Pbk2 decides to implement the MarkingMode changes that has been implemented for Avkon Lists

//
//    Class CPbk2ContactViewCustomListBoxItemDrawer
//

CPbk2ContactViewCustomListBoxItemDrawer::CPbk2ContactViewCustomListBoxItemDrawer() {}
CPbk2ContactViewCustomListBoxItemDrawer::CPbk2ContactViewCustomListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel,
                                                            const CFont* aFont,
                                                            CPbk2ContactViewCustomListBoxData* aColumnData )
    : CTextListItemDrawer(aTextListBoxModel, aFont)
    {    
    SetData(aColumnData);
    }

CPbk2ContactViewCustomListBoxItemDrawer::~CPbk2ContactViewCustomListBoxItemDrawer()
    {
    delete iPropertyArray;
    }

TSize CPbk2ContactViewCustomListBoxItemDrawer::MinimumCellSize() const
    {
    CPbk2ContactViewCustomListBoxData* data=ColumnData();
    const TInt columns=data->LastColumn();
    if (columns==-1)
        {
        return CTextListItemDrawer::MinimumCellSize();
        }
    TInt width=0;
    TInt height=0;
    TBool graphicsColumn=EFalse;
    for (TInt ii=0;ii<=columns;ii++)
        {
        width+=data->ColumnWidthPixel(ii);
        if (data->ColumnIsGraphics(ii))
            {
            graphicsColumn=ETrue;
            }
        else
            {
            const CFont* font=data->ColumnFont(ii);
            if(font)
                {
                height=Max(height,font->HeightInPixels());
                }
            else
                {
                height=Max(height, iFont->HeightInPixels());
                }
            }
        }
    if (graphicsColumn)
        {
        CArrayPtr<CGulIcon>* iconArray=data->IconArray();
        if (iconArray) 
            {
            const TInt count=iconArray->Count();
            for (TInt jj=0;jj<count;jj++)
                height=Max(height,(*iconArray)[jj]->Bitmap()->SizeInPixels().iHeight);
            }
        }
    height = Max (iData->FontBoundValues().iHeightInPixels, height);
    height+=VerticalInterItemGap(); 
    return TSize(width,height);
    }

TInt CPbk2ContactViewCustomListBoxItemDrawer::ItemWidthInPixels(TInt /*aItemIndex*/) const
    {
    CPbk2ContactViewCustomListBoxData* data=ColumnData();
    const TInt columns=data->LastColumn();
    TInt itemWidth = 0;
    if (iDrawMark)
        {
        itemWidth += (iMarkColumnWidth + iMarkGutter);
        }
    for (TInt ii=0;ii<=columns;ii++)
        {
        if (!ColumnData()->ColumnIsOptional(ii))
            {
            itemWidth+=data->ColumnWidthPixel(ii);
            }
        }
    return itemWidth;
    }

void CPbk2ContactViewCustomListBoxItemDrawer::SetItemCellSize(
    const TSize& aSizeInPixels)
    {
    CTextListItemDrawer::SetItemCellSize( aSizeInPixels );

    // Data needs the cell size to create/reconfigure highlight animations
    CPbk2ContactViewCustomListBoxData* data = ColumnData();
    data->SetItemCellSize( iItemCellSize );
    }

#ifndef RD_TOUCH2
void CPbk2ContactViewCustomListBoxItemDrawer::DrawCurrentItemRect(const TRect& aRect) const
//
//    Draw the item background
//
    {
    iGc->SetClippingRect(iViewRect);
    iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
    iGc->SetPenColor(iHighlightedBackColor);    // KDefaultLbxHighlightRectColor
    iGc->DrawRect(aRect);
    iGc->CancelClippingRect();
    }
#endif // !RD_TOUCH2

/**
* Returns a pointer to the column data. Does not imply transfer of ownership.
*
* @since ER5U
*/
CPbk2ContactViewCustomListBoxData* CPbk2ContactViewCustomListBoxItemDrawer::ColumnData() const
    {
    return STATIC_CAST(CPbk2ContactViewCustomListBoxData*,iData);
    }

void CPbk2ContactViewCustomListBoxItemDrawer::DrawItemMark(TBool /*aItemIsSelected*/, TBool /*aViewIsDimmed*/, const TPoint& /*aMarkPos*/) const
    {
    // not used in S60
    }

void CPbk2ContactViewCustomListBoxItemDrawer::DrawItemText( TInt aItemIndex,
                                             const TRect& aItemTextRect,
                                             TBool aItemIsCurrent,
                                             TBool /*aViewIsEmphasized*/,
                                             TBool aItemIsSelected ) const
//
//    Draw the items text
//
    {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
    if ( transApi )
        {
        transApi->StartDrawing( MAknListBoxTfxInternal::EListNotSpecified );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    iGc->SetPenColor(iTextColor);
    iGc->SetBrushColor(iBackColor);
    TPtrC temp=iModel->ItemText(aItemIndex);
    SetupGc(aItemIndex);
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        transApi->StopDrawing();
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    TBufC<256> target(KNullDesC);
    // SERIES60 ITEM MARKS! SAPLAF (the same code is in eikfrlb.cpp and eikclb.cpp)
    TPtrC repl;
    TInt pos = -1;
    
    TBool removeicon = (!aItemIsSelected && !ItemMarkReverse()) || (aItemIsSelected && ItemMarkReverse()); 
#ifdef PBK2_AVKON_TOUCH_MARKINGMODE_CHANGES
    
    if ( Flags() & CListItemDrawer::EMarkingModeEnabled )
        {
        removeicon = EFalse;
        }
#endif // PBK2_AVKON_TOUCH_MARKINGMODE_CHANGES
    if ( Flags() & EDrawMarkSelection && ItemMarkPosition() != -1 && removeicon)
        {
        repl.Set( ItemMarkReplacement() );
        pos = ItemMarkPosition();
        }

    // Try to allocate buffer dynamically. If out of memory, use the fixed size stack buffer.
    // (+1 is for the last column separator)
    TInt size = temp.Length() + repl.Length() + 1;
    if ( pos >= 0 )
        {
        size += pos; // space for other column separators
        }

    HBufC* buffer = NULL;
    if ( size > 256 )
        {
        buffer = HBufC::New( size );
        }

    TPtr des = ( buffer ? buffer->Des() : target.Des() );
    // Note that ReplaceColumn does not update correct length in variable 'des',
    // because it is not a reference parameter :(
    AknLAFUtils::ReplaceColumn(des, &temp, &repl, '\t', pos);
    des.Set( buffer ? buffer->Des() : target.Des() );

    // END OF ITEM MARKS! SAPLAF
 
    CPbk2ContactViewCustomListBoxData::TColors colors;
    colors.iText=iTextColor;
    colors.iBack=iBackColor;
    colors.iHighlightedText=iHighlightedTextColor;
    colors.iHighlightedBack=iHighlightedBackColor;

    if (ColumnData()->CurrentMarqueeItemIndex() != aItemIndex && aItemIsCurrent)
        {
        ColumnData()->ResetMarquee();
        ColumnData()->SetCurrentMarqueeItemIndex(aItemIndex);
        }
    ColumnData()->SetCurrentItemIndex(aItemIndex);
    ColumnData()->Draw(Properties(aItemIndex), *iGc,&des,aItemTextRect,(aItemIsCurrent /*|| aViewIsEmphasized*/),colors, aItemIndex);
    
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        transApi->StartDrawing( MAknListBoxTfxInternal::EListNotSpecified );
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    iGc->DiscardFont();
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    if ( transApi )
        {
        transApi->StopDrawing();
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
    // end of SERIES60 LAF code.

    delete buffer;
    }

TAny* CPbk2ContactViewCustomListBoxItemDrawer::Reserved_1()
    {
    return NULL;
    }


void CPbk2ContactViewCustomListBoxItemDrawer::ClearAllPropertiesL()
    {
    delete iPropertyArray;
    iPropertyArray = NULL;
    iPropertyArray = new (ELeave) CArrayFixFlat<SListProperties>(2);    
    }

void CPbk2ContactViewCustomListBoxItemDrawer::SetPropertiesL(TInt aItemIndex, TListItemProperties aProperty)
    {
    if (!iPropertyArray) ClearAllPropertiesL();
    TInt index;
    TKeyArrayFix key(0,ECmpTInt);
    SListProperties prop;
    prop.iItem = aItemIndex;
    
    if (iPropertyArray->FindIsq(prop, key, index))
        {
        iPropertyArray->InsertIsqL(prop, key);
        iPropertyArray->FindIsq(prop, key, index);
        }
    iPropertyArray->At(index).iProperties = aProperty;
    }


TListItemProperties CPbk2ContactViewCustomListBoxItemDrawer::Properties(TInt aItemIndex) const
    {
    if (!iPropertyArray) return CTextListItemDrawer::Properties(aItemIndex);
    CAknListBoxFilterItems *filter = STATIC_CAST(CAknFilteredTextListBoxModel*,iModel)->Filter();
    if (filter)
        {
        aItemIndex = filter->FilteredItemIndex(aItemIndex);
        }
    TKeyArrayFix key(0,ECmpTInt);
    SListProperties prop;
    prop.iItem = aItemIndex;
    TInt index;
    if (iPropertyArray->FindIsq(prop, key, index)) return CTextListItemDrawer::Properties(aItemIndex);
    return iPropertyArray->At(index).iProperties;
    }

void CPbk2ContactViewCustomListBoxItemDrawer::CPbk2ContactViewCustomListBoxItemDrawer_Reserved()
    {
    }

// End of File
