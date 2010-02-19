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
* Description: CCA customized control. Code has been modified to
*       suit CCA requirements. See CFormattedCellListBoxItemDrawer
*       in eikfrlb.cpp
*       Ensure that this piece of code is in sync with Avkon eikfrlb.cpp(CFormattedCellListBoxItemDrawer) 
*
*/

#include <eikfrlb.h>
#include <barsread.h>
#include <gulicon.h>
#include <AknUtils.h>
#include <AknsControlContext.h>
#include <AknBidiTextUtils.h>
#include <aknlists.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <AknFontId.h>

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
#include <aknlistloadertfx.h>
#include <aknlistboxtfxinternal.h>
#include <aknlistboxtfx.h>
#endif //RD_UI_TRANSITION_EFFECTS_LIST

#ifdef RD_UI_TRANSITION_EFFECTS_POPUPS
#include <akntransitionutils.h>
#endif

_LIT(KMarkReplacementString, "%S");

#include "ccappcommlaunchercustomlistboxdata.h"
#include "ccappcommlaunchercustomlistboxitemdrawer.h"

CCCAppCommLauncherCustomListBoxItemDrawer::CCCAppCommLauncherCustomListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel, 
                                 const CFont* aFont, 
                                 CCCAppCommLauncherCustomListBoxData* aFormattedCellData)
						         : CTextListItemDrawer(aTextListBoxModel, aFont)
    {
    SetData(aFormattedCellData);
    }


CCCAppCommLauncherCustomListBoxItemDrawer::~CCCAppCommLauncherCustomListBoxItemDrawer()
    {   
    delete iPropertyArray;
    }

CCCAppCommLauncherCustomListBoxData* CCCAppCommLauncherCustomListBoxItemDrawer::FormattedCellData() const
    {
    return STATIC_CAST(CCCAppCommLauncherCustomListBoxData*,iData);
    }

CCCAppCommLauncherCustomListBoxData* CCCAppCommLauncherCustomListBoxItemDrawer::ColumnData() const
    {
    return STATIC_CAST(CCCAppCommLauncherCustomListBoxData*,iData);
    }

void CCCAppCommLauncherCustomListBoxItemDrawer::DrawEmptyItem( TInt /*aItemIndex*/,
                                               TPoint aItemRectPos,
                                               TBool /*aViewIsDimmed*/ ) const
   {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
   MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
   if ( transApi )
       {
       transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
       }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
   TRect r( aItemRectPos, iItemCellSize );
   CCoeControl* control = FormattedCellData()->Control();
   
    const MCoeControlBackground* backgroundDrawer = control->FindBackground();
   if ( control )
       {
       MAknsControlContext *cc = AknsDrawUtils::ControlContext( control );

       if ( !cc )
           {
           cc = FormattedCellData()->SkinBackgroundContext();
           }
        if ( backgroundDrawer )
            {
            backgroundDrawer->Draw( *iGc, *control, r );
            }
        else if ( CAknEnv::Static()->TransparencyEnabled() )
           {
           AknsDrawUtils::Background( AknsUtils::SkinInstance(), cc, control, *iGc, r,
                                  KAknsDrawParamNoClearUnderImage );
           }
       else
           {
           AknsDrawUtils::Background( AknsUtils::SkinInstance(), cc, control, *iGc, r,
                                  KAknsDrawParamNoClearUnderImage |
                                  KAknsDrawParamBottomLevelRGBOnly );
           }
       }
   else
       {
       iGc->Clear( r );
       }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
   if ( transApi )
       {
       transApi->StopDrawing();
       }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
   }

struct TLafTable;
const TLafTable &BgListPaneTable();


void CCCAppCommLauncherCustomListBoxItemDrawer::DrawItemText( TInt aItemIndex,
                                                   const TRect& aItemTextRect,
                                                   TBool aItemIsCurrent,
                                                   TBool aViewIsEmphasized, 
                                                   TBool aItemIsSelected) const
   {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
   MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
   if ( transApi )
       {
       transApi->StartDrawing( MAknListBoxTfxInternal::EListNotSpecified );
       }
#endif //RD_UI_TRANSITION_EFFECTS_LIST
   iGc->SetPenColor(iTextColor);
   iGc->SetBrushColor(iBackColor);

   TPtrC temp=iModel->ItemText(aItemIndex);

   SetupGc(aItemIndex);
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
   if ( transApi )
       {
       transApi->StopDrawing();
       }
#endif //RD_UI_TRANSITION_EFFECTS_LIST

   TBool removeicon = (!aItemIsSelected && !ItemMarkReverse()) || (aItemIsSelected && ItemMarkReverse());

   CCCAppCommLauncherCustomListBoxData::TColors colors;
   colors.iText=iTextColor;
   colors.iBack=iBackColor;
   colors.iHighlightedText=iHighlightedTextColor;
   colors.iHighlightedBack=iHighlightedBackColor;
   
   DrawBackgroundAndSeparatorLines( aItemTextRect );

   // Draw separator line except last item 
   if ( aItemIndex < iModel->NumberOfItems() - 1 )
	   {
	   DrawSeparator( *iGc, aItemTextRect, iTextColor);
	   }
   
   TBool highlightShown = ETrue;
   
   if (FormattedCellData()->RespectFocus() && !aViewIsEmphasized)
       {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
       if ( transApi )
           {
           transApi->Remove( MAknListBoxTfxInternal::EListHighlight );
           }
#endif //RD_UI_TRANSITION_EFFECTS_LIST

       highlightShown = EFalse;
       }
       
   if (FormattedCellData()->IsMarqueeOn() && FormattedCellData()->CurrentMarqueeItemIndex() != aItemIndex && aItemIsCurrent)
       {
       FormattedCellData()->ResetMarquee();
       FormattedCellData()->SetCurrentMarqueeItemIndex(aItemIndex);
       }
   
   if ( aItemIsCurrent )
       {
       FormattedCellData()->SetCurrentItemIndex( aItemIndex );
       }
       
   FormattedCellData()->SetCurrentlyDrawnItemIndex( aItemIndex );
   
   // drawing with mark icon
   if ( Flags() & EDrawMarkSelection && ItemMarkPosition() != -1 && removeicon)
       {
       // Try to allocate buffer dynamically. If out of memory, just use normal drawing
       // without mark icon.
       // (+2 is for the possible 2 additional column separators)
       TPtrC repl;
       repl.Set( ItemMarkReplacement() );        
       TInt size = temp.Length() + repl.Length() + 2;
       //TBufC<KMaxTotalDataLength> target(KNullDesC);
       HBufC* buffer = HBufC::New( size );

       if( buffer )
           {
           TPtr des = buffer->Des();
           TInt markPos = ItemMarkPosition(); // -1 if not set
           TInt startPos(0);
           TInt endPos(0);
           
           while( endPos < temp.Length() && markPos >= 0 )
               {
               if( temp[endPos] == '\t' )
                   {
                   markPos--;
                   if( markPos == 0 )
                       {
                       startPos = endPos + 1; // +1 for column separator
                       }
                   }
               endPos++;
               }

           if( markPos > 0 ) // mark icon will go somewhere after item string...
               {
               startPos = temp.Length();
               endPos = temp.Length();
               des.Append( temp.Left( startPos ) ); // first part of string
               des.Append( '\t' ); // column separator before mark icon was missing                
               }
           else
               {
               des.Append( temp.Left( startPos ) ); // first part of string
               }

           TInt replace = repl.FindF(KMarkReplacementString);
           if (replace != KErrNotFound) // moving subcell
               {
               des.Append( repl.Left( replace ) );
               // now we have first part of string + mark subcell
               // then add 1 moved subcell
               des.Append( temp.Mid( startPos, endPos-startPos ) );

               // skip second moved subcell and add rest of the string
               while( endPos < temp.Length() )
                   {
                   if( temp[endPos] == '\t' )
                       {
                       break;
                       }
                   endPos++;
                   }
               if( endPos < temp.Length() ) // this cuts off '\t'
                   {
                   endPos++;
                   }
               des.Append( temp.Mid( endPos ) );
               
               //is this needed - currently not used in S60?
               //buffer.Append(aReplacement->Mid(replace+2)); // 2 == length of %s
               }
           else // just replacement
               {
               des.Append( repl ); // no '%s' in replacement string
               des.Append( '\t' );
               des.Append( temp.Mid( endPos ) );
               }

           des.Set( buffer->Des() );
           FormattedCellData()->Draw( Properties(aItemIndex),
                                      *iGc,
                                      &des,
                                      aItemTextRect,
                                      aItemIsCurrent && highlightShown,
                                      colors );
           delete buffer;
           return;
           }
       }

   // normal drawing without mark icon
   FormattedCellData()->Draw( Properties(aItemIndex),
                              *iGc,
                              &temp,
                              aItemTextRect,
                              aItemIsCurrent && highlightShown,
                              colors );
   }

TSize CCCAppCommLauncherCustomListBoxItemDrawer::MinimumCellSize() const 
   {
   CCCAppCommLauncherCustomListBoxData* data=FormattedCellData();
   const TInt cells=data->LastSubCell();
   if (cells==-1)
       return CTextListItemDrawer::MinimumCellSize();
   TInt width=0;
   TInt height=0;
   for(TInt ii=0;ii<cells;ii++) 
       {
       TPoint endpos( data->SubCellPosition(ii) + data->SubCellSize(ii) );
       if (endpos.iX > width) width = endpos.iX;
       if (endpos.iY > height) height = endpos.iY;
       }
   height+=VerticalInterItemGap();
   return TSize(width,height);
   }


void CCCAppCommLauncherCustomListBoxItemDrawer::DrawItemMark(TBool /*aItemIsSelected*/, TBool /*aViewIsDimmed*/, const TPoint& /*aMarkPos*/) const
   {
   }


TInt CCCAppCommLauncherCustomListBoxItemDrawer::ItemWidthInPixels(TInt) const
   {
   TInt itemWidth = MinimumCellSize().iWidth;
   
   if (iDrawMark)
       itemWidth += (iMarkColumnWidth + iMarkGutter);
   return itemWidth;
   }

void CCCAppCommLauncherCustomListBoxItemDrawer::SetItemCellSize(
   const TSize& aSizeInPixels )
   {
   CTextListItemDrawer::SetItemCellSize( aSizeInPixels );

   // Data needs the cell size to create/reconfigure highlight animations
   FormattedCellData()->SetItemCellSize( iItemCellSize );
   }

void CCCAppCommLauncherCustomListBoxItemDrawer::SetTopItemIndex(TInt aTop)
   {
   iTopItemIndex = aTop;
   }

void CCCAppCommLauncherCustomListBoxItemDrawer::DrawCurrentItemRect(const TRect& aRect) const
   {
   iGc->SetClippingRect(iViewRect);
   iGc->SetBrushStyle(CGraphicsContext::ENullBrush);
   iGc->SetPenColor(iHighlightedBackColor);
   iGc->DrawRect(aRect);
   iGc->CancelClippingRect();
   }

void CCCAppCommLauncherCustomListBoxItemDrawer::ClearAllPropertiesL()
    {
    delete iPropertyArray;
    iPropertyArray = NULL;
    iPropertyArray = new (ELeave) CArrayFixFlat<SListProperties>(2);    
    }

void CCCAppCommLauncherCustomListBoxItemDrawer::SetPropertiesL(TInt aItemIndex, TListItemProperties aProperty)
    {
    if (!iPropertyArray) ClearAllPropertiesL();
    TInt index;
    TKeyArrayFix key(0,ECmpTInt);
    SListProperties prop;
    prop.iItem = aItemIndex;
    TInt error = iPropertyArray->FindIsq(prop, key, index);
    if (error)
    { // not found, error is nonzero.
    iPropertyArray->InsertIsqL(prop, key);
    iPropertyArray->FindIsq(prop, key, index);
    }
    iPropertyArray->At(index).iProperties = aProperty;
    }

TListItemProperties CCCAppCommLauncherCustomListBoxItemDrawer::Properties(TInt aItemIndex) const
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
    TInt error = iPropertyArray->FindIsq(prop, key, index);
    if (error) return CTextListItemDrawer::Properties(aItemIndex);
    return iPropertyArray->At(index).iProperties;
    }



void CCCAppCommLauncherCustomListBoxItemDrawer::CCCAppCommLauncherCustomListBoxItemDrawer_Reserved()
    {
    }

void CCCAppCommLauncherCustomListBoxItemDrawer::DrawBackgroundAndSeparatorLines( const TRect& aItemTextRect ) const
    {
    MAknsSkinInstance *skin = AknsUtils::SkinInstance();
    CCoeControl* control = FormattedCellData()->Control();
    MAknsControlContext *cc = AknsDrawUtils::ControlContext( control );

    if ( !cc )
        {
        cc = FormattedCellData()->SkinBackgroundContext();
        }

#ifdef RD_UI_TRANSITION_EFFECTS_LIST
    MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
    
    if ( transApi && !transApi->EffectsDisabled() )
        {
        MAknListBoxTfx* tfxApi = CAknListLoader::TfxApi( iGc );

        if ( tfxApi )
            {
            tfxApi->EnableEffects( FormattedCellData()->IsBackgroundDrawingEnabled() );
            }
        }
#endif // RD_UI_TRANSITION_EFFECTS_LIST

    // background
    if ( FormattedCellData()->IsBackgroundDrawingEnabled() )
        {
#ifdef RD_UI_TRANSITION_EFFECTS_LIST
        MAknListBoxTfxInternal* transApi = CAknListLoader::TfxApiInternal( iGc );
        if ( transApi )
            {
            transApi->StartDrawing( MAknListBoxTfxInternal::EListView );
            }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
        TBool bgDrawn( EFalse );
        if ( control )
            {
            const MCoeControlBackground* backgroundDrawer =
                control->FindBackground();
            if ( backgroundDrawer )
                {
                backgroundDrawer->Draw( *iGc, *control, aItemTextRect );
                bgDrawn = ETrue;
                }
            else if ( CAknEnv::Static()->TransparencyEnabled() )
                {
                bgDrawn = AknsDrawUtils::Background(
                    skin, cc, control, *iGc, aItemTextRect,
                    KAknsDrawParamNoClearUnderImage );
                }
            else
                {
                bgDrawn = AknsDrawUtils::Background(
                    skin, cc, control, *iGc, aItemTextRect,
                    KAknsDrawParamNoClearUnderImage | 
                    KAknsDrawParamBottomLevelRGBOnly );
                }
            }
        if ( !bgDrawn )
            {
            iGc->Clear( aItemTextRect );
            }
#ifdef RD_UI_TRANSITION_EFFECTS_LIST  
        if ( transApi )
            {
            transApi->StopDrawing();
            }
#endif // RD_UI_TRANSITION_EFFECTS_LIST
        }
    }

void CCCAppCommLauncherCustomListBoxItemDrawer::DrawSeparator( CGraphicsContext& aGc, const TRect& aRect, const TRgb& aColor ) const
	{
	aGc.SetBrushStyle( CGraphicsContext::ENullBrush );
	aGc.SetPenStyle( CGraphicsContext::ESolidPen );
	
	TRgb color( aColor );
	color.SetAlpha( 32 );
	aGc.SetPenColor( color );
	
	TRect lineRect( aRect );
	TInt gap = AknLayoutScalable_Avkon::listscroll_gen_pane( 0 ).LayoutLine().it; 
	lineRect.Shrink( gap, 0 );
	lineRect.Move( 0, -1 );
	
	aGc.DrawLine( TPoint( lineRect.iTl.iX, lineRect.iBr.iY ), 
	TPoint( lineRect.iBr.iX, lineRect.iBr.iY ) );
	}
// End of File
