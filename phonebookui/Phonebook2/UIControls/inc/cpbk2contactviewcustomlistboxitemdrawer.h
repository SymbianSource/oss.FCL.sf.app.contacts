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
*
*/


#if !defined(__CPbk2ContactViewCustomListBoxItemDrawer_H__)
#define __CPbk2ContactViewCustomListBoxItemDrawer_H__


#include <eiklbi.h>
#include <eiktxlbx.h>
#include <eiklbed.h>

class CPbk2ContactViewCustomListBoxData;

/**
 * Draws the contents of list box items.
 * Ensure that this piece of code is in sync with Avkon EIKCLB.CPP(CColumnListBoxItemDrawer)
 * @since 5.0
 */
class CPbk2ContactViewCustomListBoxItemDrawer : public CTextListItemDrawer
    {

public:
    /* C++ default constructor.
    *
    *  @since 3.2
    */
            
    CPbk2ContactViewCustomListBoxItemDrawer();
            
    /**
     * C++ default constructor.
     *
     * Initialises the list box item drawer with the specified model, 
     * font and data.
     *
     * @param aTextListBoxModel The model whose items will be drawn. 
     *        This provides the item text that is drawn using 
     *        @c DrawItemText().
     * @param aFont The font in which the items will be drawn. 
     * @param aColumnData The column list box data. This comprises: 
     *        the column widths, gaps, fonts, aligments and bitmaps.
     */

    CPbk2ContactViewCustomListBoxItemDrawer(MTextListBoxModel* aTextListBoxModel, 
                                      const CFont* aFont, 
                                      CPbk2ContactViewCustomListBoxData* aColumnData );
 
    /**
     * Destructor.
     */
    ~CPbk2ContactViewCustomListBoxItemDrawer();

    /**
     * Gets a pointer to the column list box data as set during construction.
     * Does not transfer ownership.
     *
     * @return The column list box data.
     */
    CPbk2ContactViewCustomListBoxData* ColumnData() const;

public:


    /** 
     * The structure holding given item properties. 
     *
     * Used for storing information given through @c SetProperties() method.
     */
    struct SListProperties 
        {
        /** Item identifier. */
        TInt iItem; // must be first

        /** Properties of the item. */
        TListItemProperties iProperties;
        };

    /**
     * Clears all properties from the @c CPbk2ContactViewCustomListBoxItemDrawer.
     */
    void ClearAllPropertiesL();
    
   
    /**
     * Set the property for an item with @c aItemIndex 
     *
     * @param aItemIndex Item index
     * @param aProperty The new property setting
     * The SetProperties is convinience method for normal cases.
     * NOTE that it does NOT work correctly if you can insert/delete
     * items from the list -- You need to update all properties if
     * any changes to the item text array happen! Often it is convinient
     * to override the Properties() method. Further, use this only
     * for small lists as it causes O(n) time if you need to set
     * all properties.
     */
    void SetPropertiesL(TInt aItemIndex, 
                                 TListItemProperties aProperty);

    /**
     * Get properties for an item. 
     *
     * @param aItemIndex Item's index for identifying.
     * @return Wanted property set.
     */
    virtual TListItemProperties Properties(TInt aItemIndex) const;

public: // from CTextListItemDrawer

    /**
     * From @c CTextListItemDrawer.
     * 
     * Draw item text.
     *
     * @param aItemIndex Index of the item to draw. 
     * @param aItemTextRect Area to draw into. 
     * @param aItemIsCurrent @c ETrue if the item is current. 
     * @param aViewIsEmphasized @c ETrue if the view is emphasised.
     * @param aItemIsSelected @c ETrue if the item is selected.
     */
    void DrawItemText(TInt aItemIndex,
                               const TRect& aItemTextRect,
                               TBool aItemIsCurrent,
                               TBool aViewIsEmphasized, 
                               TBool aItemIsSelected) const;
    // the next one is for avkon

    /**
     * Not used in S60.
     */
    void DrawItemMark(TBool aItemIsSelected, 
                               TBool aViewIsDimmed, 
                               const TPoint& aMarkPos) const;

public: // from CListItemDrawer

    /**
     * From @c CListItemDrawer.
     *
     * Gets the minimum cell size.
     *
     * @return The minimum size for a cell. This is 
     *         TSize(0, 0) in @c CListItemDrawer.
     */
    TSize MinimumCellSize() const;
    
    /**
     * From @c CListItemDrawer.
     *
     * Gets the width of the specified item.
     *
     * @param aItemIndex The index of the item whose width is to be returned. 
     * @return The width of the item. This is 0 in @c CListItemDrawer
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
    
    /**
     * Get Item Cell Size.
     *
     * @return Cell size of an item.
     */
    TSize LafItemSize() { return iItemCellSize; }

private: // from CListItemDrawer

    TAny* Reserved_1();

private: // defining reserved method.

    void CPbk2ContactViewCustomListBoxItemDrawer_Reserved();

protected:

    /**
     * Draw the item background.
     *
     * Internal member, not intended for use.
     *
     * @param aRect Drawing area. 
     */
    void DrawCurrentItemRect(const TRect& aRect) const;
    
    /** 
     * Array of properties.
     */
    CArrayFix<SListProperties>* iPropertyArray;
    
    /** 
     * Not used. 
     */ 
    TInt iSpare;
    
    };




#endif // __CPbk2ContactViewCustomListBoxItemDrawer_H__

// End of File
