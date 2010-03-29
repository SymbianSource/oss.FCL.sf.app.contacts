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
* Description: CCA custom control. Code has been modified to
*       suit CCA requirements. See CFormattedCellListBoxItemDrawer
*       in eikfrlb.cpp
*
*/

#ifndef CCAPPCOMMLAUNCHERCUSTOMLISTBOXITEMDRAWER_H_
#define CCAPPCOMMLAUNCHERCUSTOMLISTBOXITEMDRAWER_H_

#include <eikclb.h>
#include <eikfrlb.h>
#include <eikslb.h>
#include <AknGrid.h>
#include <AknUtils.h>
#include <eiklbi.h>


#include <eiklbv.h>
#include <eiktxlbm.h>
#include <eiktxlbx.h>



class TAknTextLineLayout;
class MAknsSkinInstance;
class MAknsControlContext;

class CCCAppCommLauncherCustomListBoxData;

class CCCAppCommLauncherCustomListBoxItemDrawer : public CTextListItemDrawer
	{
public:

    /**
     * C++ default constructor.
     * 
     * @param aTextListBoxModel The list box model class.
     * @param aFont Font to be used in listbox.
     * @param aFormattedCellData Pointer to the list layout drawer class.
     */
	CCCAppCommLauncherCustomListBoxItemDrawer(
            MTextListBoxModel* aTextListBoxModel, 
            const CFont* aFont, 
            CCCAppCommLauncherCustomListBoxData* aFormattedCellData);
	
    /**
     * Destructor.
     */
    ~CCCAppCommLauncherCustomListBoxItemDrawer();

    /**
     * Gets the list layout drawer.
     * 
     * @return Pointer to the list layout drawer object.
     */
	CCCAppCommLauncherCustomListBoxData* FormattedCellData() const;

    /**
     * Gets the list layout drawer.
     * 
     * @return Pointer to the list layout drawer object.
     */
	CCCAppCommLauncherCustomListBoxData* ColumnData() const; 
    // for compability with columnlistbox itemdrawer

    /**
     * Draws an empty item to the list box.
     * 
     * @param aItemIndex Not used.
     * @param aItemRectPos Position for the list box item rectangle.
     * @param aViewIsDimmed Not used.
     */
    virtual void DrawEmptyItem(TInt aItemIndex, TPoint aItemRectPos, 
                                        TBool aViewIsDimmed) const;

    /**
     * This function sets top item index.
     * 
     * @param aTop Item index value.
     */
	void SetTopItemIndex(TInt aTop);

    /**
     * List item index and color and style properties.
     */
	struct SListProperties 
	    {
        /** List box item index. */
        TInt iItem; // must be first

        /** Colour and style properties for the list item. */
        TListItemProperties iProperties;
	    };

    /**
     * Clears all properties.
     */
	void ClearAllPropertiesL();

    /** 
     * The SetProperties is convenience method for normal cases.
     * NOTE that it does NOT work correctly if you can insert/delete
     * items from the list -- You need to update all properties if
     * any changes to the item text array happen! Often it is convenient
     * to override the @c Properties() method. Further, use this only
     * for small lists.
     *
     * It has been implemented as sorted list of (index,Property) pairs.
     * 
     * If you're using find, aItemIndex for this method is the same as you give
     * in @c MdcaPoint() points the default @c Properties() does conversion 
     * between indexes.
     * 
     * @param aItemIndex List box item index.
     * @param properties Colour and style properties for the list box item.
     */
    void SetPropertiesL(TInt aItemIndex, TListItemProperties properties);

    /**
     * Function for getting properties of a certain list box item.
     * 
     * @param aItemIndex The listbox item index.
     * @return Colour and style properties of wanted
     *         list box item.
     */
	virtual TListItemProperties Properties(TInt aItemIndex) const;

    /**
     * Gets list box item cell size.
     * 
     * @return The list box item cell size.
     */
	TSize LafItemSize() { return iItemCellSize; }

public: // from CTextListItemDrawer

    /**
     * From @c CTextListItemDrawer.
     *
     * Draw an item in a list box.
     * 
     * @param aItemIndex The index into the model's item array of the item 
     *        to draw.
     * @param aItemTextRect The item’s text rectangle. 
     * @param aItemIsCurrent @c ETrue if the item is current. @c EFalse 
     *        otherwise.
     * @param aViewIsEmphasized @c ETrue if the view is emphasized. @c EFalse
     *        otherwise.
     * @param aItemIsSelected @c ETrue if the item is selected. @c EFalse 
     *        otherwise.
     */
	void DrawItemText(TInt aItemIndex,const  TRect& aItemTextRect,
                               TBool aItemIsCurrent, TBool aViewIsEmphasized, 
                               TBool aItemIsSelected) const;

    /**
     * From @c CTextListItemDrawer.
     *
     * Draws an item tick mark. 
     *
     * A tick mark is used in multiple selection lists to indicate the item is
     * selected.
     *
     * This function is invoked by the framework to draw an item mark if 
     * @c iDrawMark is set. The mark should be drawn into this list item 
     * drawer's graphics context; by default, @c DrawItemMark() draws a tick 
     * using the standard symbol font.
     * 
     * This function does not need to be re-implemented for derived classes 
     * which do not draw item marks.
     * 
     * @param aItemIsSelected Whether the item to draw for is selected.
     * @param aViewIsDimmed Whether the item is dimmed. 
     * @param aMarkPos Position of the mark.
     */
	void DrawItemMark(TBool aItemIsSelected, TBool aViewIsDimmed, 
                               const TPoint& aMarkPos) const;

public: // from CListItemDrawer

    /**
     * From @c CListItemDrawer.
     *
     * Gets the minimum size of a cell based on the font and the number of 
     * characters in the cell.
     * 
     * @return The minimum size for a cell in pixels.
     */
	TSize MinimumCellSize() const;

    /**
     * From @c CListItemDrawer.
     *
     * Gets the width in pixels of the specified item.
     * 
     * @param aItemIndex The index of the item for which the width is obtained.
     * @return The specified item's width in pixels.
     */
	TInt ItemWidthInPixels(TInt aItemIndex) const; 

    /**
     * From @c CListItemDrawer.
     *
     * Sets the item cell size.
     *
     * The cell size is the on-screen size of the entire item, including its 
     * text and its item mark.
     * 
     * @param aSizeInPixels New size for the item cell.
     */
    void SetItemCellSize(const TSize& aSizeInPixels);

protected:
    
    /**
     * Draws current item rectangle.
     * 
     * @param aRect Current item rectangle.
     */
	void DrawCurrentItemRect(const TRect& aRect) const;

    /**
     * @c WordWrapListItem is used with @c CAknDouble* style listboxes.
     * If listbox does not have 2nd line defined, @c WordWrapListItem
     * wraps 1st line to 2nd line, if text does not fit to 1st line.
     * If listbox does have 2nd line defined, this method truncates
     * both lines.
     *
     * @internal
     * @param aTarget Where to place wrapped item string
     * @param aItemString Text to be wrapped
     * @param aFirstIndex index of 1st subcell to wrap
     * @param aSecondIndex index of 2nd subcell
     */
    void WordWrapListItem( TPtr& aTarget, 
                           const TDesC &aItemString, 
                           TInt aFirstIndex, 
                           TInt aSecondIndex,
                           TInt aItemIndex ) const;
    /**
    * @internal
    */
    void DrawBackgroundAndSeparatorLines( const TRect& aItemTextRect ) const;
    /**
     *  Draw separator line 
     */
    void DrawSeparator( CGraphicsContext& aGc, const TRect& aRect, const TRgb& aColor ) const;
                        

private:

	void CCCAppCommLauncherCustomListBoxItemDrawer_Reserved();

protected:

    /**
     * Top item index.
     */
	TInt iTopItemIndex;

    /**
     * Array for list box item's properties.
     * Own.
     */
	CArrayFix<SListProperties>* iPropertyArray;

private:

	TInt iSpare;
	};



#endif /* CCAPPCOMMLAUNCHERCUSTOMLISTBOXITEMDRAWER_H_ */
