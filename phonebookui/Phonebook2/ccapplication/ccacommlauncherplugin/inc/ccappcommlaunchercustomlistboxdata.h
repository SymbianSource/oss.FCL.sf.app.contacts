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
*              suit CCA requirements. See CFormattedCellListBoxData
*              in EIKFRLBD.H. 
*              Ensure that this piece of code is in sync with Avkon EIKFRLBD.H (CFormattedCellListBoxData)
*
*/

#ifndef CCAPPCOMMLAUNCHERCUSTOMLISTBOXDATA_H_
#define CCAPPCOMMLAUNCHERCUSTOMLISTBOXDATA_H_

#include <e32base.h>
#include <gdi.h>

#include <eiklbd.h>
#include <eikfrlb.h>
#include <avkon.hrh>
#include <AknsDrawUtils.h> // for TAknsQsnTextColorsIndex
#include <babitflags.h>    // for TBitFlags32

class CWindowGc;
class CGulIcon;
class CCCAppCommLauncherCustomListBoxDataExtension;
class MAknsControlContext;
class TAknsItemID;
class CAknLayoutData;
class CAknsEffectAnim;
class TAknTextLineLayout;
class TAknWindowLineLayout;
class TAknTextComponentLayout;
class TAknWindowComponentLayout;

class CCCAppCommLauncherCustomListBoxData : public CListBoxData
    {
    friend class CCCAppCommLauncherCustomListBoxDataExtension;

public:
    
    //INNER CLASS DECLARATION
    /*
     * A container class for storage of colour values.                           
     * Used by @c CFormattedCellListBoxData.
     */
    class TColors
        {
    
    public:
    
        /**
         * C++ default constructor.
         */
         TColors();

    public:
    
        /** 
         * The text colour.
         */
        TRgb iText;

        /** 
         * The background colour.
         */
        TRgb iBack;

        /** 
         * The highlighted text colour.
         */
        TRgb iHighlightedText;

        /** 
         * The highlighted background colour.
         */
        TRgb iHighlightedBack;
        
        /** 
         * The separator line colour.
         *
         * @deprecated The separator line colour is no longer used because the
         *             separator lines are skinned.
         */
        TRgb iRightSeparatorColor;
        };

public:

    /**
     * Two-phased constructor.
     *
     * @return Pointer to a new formatted cell listbox data object.
     */
     static CCCAppCommLauncherCustomListBoxData* NewL();

    /**
     * Destructor.
     */
     ~CCCAppCommLauncherCustomListBoxData();

public:

    /**
     * Gets colours attached to the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Text and background colors.
     */
     const TColors& SubCellColors(TInt aSubCellIndex) const;
    
    /**
     * Sets colours attached to the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aColors Text and background colors.
     */
     void SetSubCellColorsL(TInt aSubCellIndex, const TColors &aColors);

    /**
     * Gets baseline position from top of the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Baseline y-position.
     */
     TInt SubCellBaselinePos(TInt aSubCellIndex) const;
    
    /**
     * Sets baseline position from top of the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aPos Position which is set for the item.
     */
     void SetSubCellBaselinePosL(TInt aSubCellIndex, TInt aPos);

    /**
     * Gets cell size in pixels.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Cell size.
     */
     TSize SubCellSize(TInt aSubCellIndex) const;
    
    /**
     * Sets cell size in pixels.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aSize Cell size.
     */
     void SetSubCellSizeL(TInt aSubCellIndex, TSize aSize);

    /**
     * Gets cell position inside an item.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Cell position.
     */
     TPoint SubCellPosition(TInt aSubCellIndex) const;
    
    /**
     * Sets cell position inside an item.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aPosition New position for cell.
     */
     void SetSubCellPositionL(TInt aSubCellIndex, TPoint aPosition);

    /**
     * Determines whether this cell can overlap with one of the previous cells.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return @c ETrue if cell overlaps another one, @c EFalse if cell allocates 
     *         area from other cells.
     */
     TBool SubCellIsTransparent(TInt aSubCellIndex) const;

    /**
     * Modifies whether the cell can overlap with one of the previous cells.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aIsTransparent @c ETrue if transparent, @c EFalse if not.
     */
     void SetTransparentSubCellL(TInt aSubCellIndex, 
                                         TBool aIsTransparent);

    /**
     * Checks whether drawing algorithm always draws the cell or whether empty 
     * string in list item can disable the drawing of the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return @c ETrue when empty string disables cell, 
     *         @c EFalse if always drawn.
     */
     TBool SubCellIsNotAlwaysDrawn(TInt aSubCellIndex) const;

    /**
     * Sets whether drawing algorithm always draws the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aIsNotAlwaysDrawn If @c ETrue, empty string disables cell.
     */
     void SetNotAlwaysDrawnSubCellL(TInt aSubCellIndex,
                                            TBool aIsNotAlwaysDrawn);

    /**
     * Gets margins used for the content inside the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Margins in pixels.
     */
     TMargins SubCellMargins(TInt aSubCellIndex) const;

    /**
     * Sets margins for the content inside the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aMargins Margins for the content inside the cell.
     */
     void SetSubCellMarginsL(TInt aSubCellIndex, TMargins aMargins);

    /**
     * Gets font attached to the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Handle to the attached font.
     */
     const CFont* SubCellFont(TInt aSubCellIndex) const;

    /**
     * Sets the font attached to the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aFont Font to attach to the cell.
     */
     void SetSubCellFontL(TInt aSubCellIndex, const CFont* aFont);

    /**
     * Gets the alignment attached to the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Alignment of the text.
     */
     CGraphicsContext::TTextAlign SubCellAlignment(TInt aSubCellIndex)
        const;

    /**
     * Sets the alignment attached to the cell.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aAlign Alignment that is attached to the cell
     */
     void SetSubCellAlignmentL(TInt aSubCellIndex,
                                       CGraphicsContext::TTextAlign aAlign);

    /**
     * Checks whetherdrawing algorithm considers the text
     * string in list item as unicode text string displayable on the screen,
     * or as string representation of an index to icon array.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return @c ETrue if graphics cell, @c EFalse if text cell.
     */
     TBool SubCellIsGraphics(TInt aSubCellIndex) const;

    /**
     * Sets whether drawing algorithm considers the text string in list item as
     * unicode text string displayable on the screen, or as string 
     * representation of an index to icon array.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aIsGraphics @c ETrue if graphics cell, @c EFalse if text cell.
     */
     void SetGraphicsSubCellL(TInt aSubCellIndex, TBool aIsGraphics);

    /**
     * Checks whether vertical line is drawn to the right side of the cell.
     *
     * @param aColumn Index number of the subcell.
     * @return Style of pen used to draw a line for the right side of the cell.
     */
     CGraphicsContext::TPenStyle 
        SubCellRightSeparatorStyle(TInt aColumn) const;

    /**
     * Sets whether vertical line is drawn to the right side of the cell.
     *
     * @param aSubCell Index number of the subcell.
     * @param aStyle Style of a pen used to draw a line for the right side of 
     *        the cell.
     */
     void SetSubCellRightSeparatorStyleL(TInt aSubCell,
                                            CGraphicsContext::TPenStyle aStyle);

    /**
     * Gets the amount of pixels text margins is grown if it is truncated. 
     * This is usually very small number used to make the layout look more 
     * balanced when text is truncated. (the glyph used for truncation is 
     * almost empty and on narrow text elements, it looks odd if the truncated
     * text element is exactly same size as non-truncated version.)
     *
     * @param aSubCellIndex Index number of the subcell.
     * @return Amount of pixels that are used for growing the text margins in 
     *         truncation.
     */
     TInt SubCellTextClipGap(TInt aSubCellIndex) const;

    /**
     * Sets the amount of pixels truncation grows a text element.
     *
     * @param aSubCellIndex Index number of the subcell.
     * @param aGap The amount of pixels that are required for growing the text 
     *        element when it is truncated. 
     */
     void SetSubCellTextClipGapL(TInt aSubCellIndex, TInt aGap);

    /**
     * SkinEnabled checks whether skins are enabled on this list.
     *
     * @since Symbian 2.0 
     * @return @c ETrue if enabled, @c EFalse if disabled.
     */
     TBool SkinEnabled() const;

    /**
     * Sets whether skins are enabled on this list.
     * The default value is determined from global setting from @c CAknAppUi.
     *
     * @since Symbian 2.0
     * @param aEnabled If @c ETrue skins are set enabled. 
     *        If @c EFalse skins are set disabled.
     */
     void SetSkinEnabledL(TBool aEnabled);

    /**
     * Checks whether the cell is used for numbers.
     *
     * @since Symbian 2.0
     * @param aSubCellIndex Index number of the subcell.
     * @return @c ETrue if enabled, @c EFalse if disabled.
     */
     TBool SubCellIsNumberCell(TInt aSubCellIndex) const;

    /**
     * Sets whether the cell is used for numbers.
     *
     * @since Symbian 2.0
     * @param aSubCellIndex Index number of the subcell.
     * @param aIsNumberCell New value for the flag.
     */
     void SetNumberCellL(TInt aSubCellIndex, TBool aIsNumberCell);

    /**
     * Gets the icon array used when drawing bitmaps for the listbox items.
     */
     CArrayPtr<CGulIcon>* IconArray() const;

    /**
     * Sets the icon array used when drawing bitmaps.
     * The array can be modified (no need to call this method when modifying),
     * even in @c MdcaPoint() or @c MdcaCount() methods of @c MDesCArray class.
     * See also @c CTextListBoxModel::SetItemTextArray().
     * 
     * The most common way to crash a listbox is by forgetting to call this 
     * method, or the list item string for graphics cell is not an index of 
     * a bitmap.
     *
     * @param aArray Pointer to the icon array.
     */
     void SetIconArrayL(CArrayPtr<CGulIcon>* aArray);

    /**
     * SetIconArray sets the icon array used when drawing bitmaps.
     * The array can be modified (no need to call this method when modifying),
     * even in @c MdcaPoint() or @c MdcaCount() methods of @c MDesCArray class.
     * See also @c CTextListBoxModel::SetItemTextArray().
     * 
     * The most common way to crash a listbox is by forgetting to call this
     * method, or the list item string for graphics cell is not an index of
     * a bitmap.
     *
     * @param aArray Pointer to the icon array.
     */
     void SetIconArray(CArrayPtr<CGulIcon>* aArray);

    /**
     * Font access to the font used in subcell.
     *
     * @param aItemProperties Not used in code.
     * @param aSubCellIndex Index number of the subcell.
     * @return Font used in subcell.
     */
     CFont* Font(const TListItemProperties& aItemProperties,
                         TInt aSubCellIndex) const;
    
    /**
     * From @c CListBoxData.
     *
     * @c Draw() is the most important method in 
     * @c CEikFormattedCellListBoxData.
     * This is the only method that needs to be implemented in this class and
     * it draws the whole list item.
     *
     * @param aProperties Properties attached to the list item.
     * @param aGc Graphics Context used for drawing the list item.
     * @param aText A text string describing the list item cells.
     * @param aRect The area of the screen for the whole list item.
     * @param aHighlight Whether the list item is selected.
     * @param aColors The colours used for drawing the item.
     */
     virtual void Draw(TListItemProperties aProperties,
                               CWindowGc& aGc,
                               const TDesC* aText,
                               const TRect& aRect,
                               TBool aHighlight,
                               const TColors& aColors) const;

    /**
     * Indicates if sub cell is using its own colors rather than common colors.
     *
     * @param aSubCell Index number of the subcell.
     * @return Boolean indicating whether own colors enabled.
     *         @c ETrue enabled, @c EFalse disabled.
     */
     TBool UseSubCellColors(TInt aSubCell) const;

    /**
     * Ability for a skin subsystem to access Mop chain through current 
     * listbox control.
     *
     * @since Symbian 2.0
     * @return Current listbox instance or NULL.
     */
     CCoeControl* Control() const;
    
    /**
     * Ability for a skin subsystem to access mop chain through current listbox
     * control.
     *
     * @since Symbian 2.0
     * @param aControlOrNull Current listbox instance or NULL. Cannot be other
     *        control than the current listbox!
     */
     void SetControl(CCoeControl* aControlOrNull);
    
    /**
     * Access to the background context of the listbox.
     *
     * @since Symbian 2.0
     * @return Skin background context.
     */
     MAknsControlContext* SkinBackgroundContext() const;
    /**
     * Modify properties of the skin context (background).
     *
     * @since Symbian 2.0
     * @param aId Skin background identifier.
     * @param aRect Skin background rectangle.
     */
     void SetBackgroundSkinStyle(const TAknsItemID *aId,
                                         const TRect &aRect);
    
    /**
     * Modify properties of the skin context (AB-column).
     *
     * @since Symbian 2.0
     * @param aId Skin background identifier.
     * @param aTileRect Skin background area from LAF.
     */
     void SetSkinStyle(const TAknsItemID* aId, const TRect& aTileRect);
    
    /**
     * Modify position of the parent for list default context.
     *
     * @since Symbian 2.0
     * @param aPos The new position.
     */
     void SetSkinParentPos(const TPoint& aPos);
   
    /**
     * Modify properties of the skin context (list ending)*
     
     * @since Symbian 2.0
     * @param listEndId List ending skin background identifier.
     * @param aRect List ending background area from LAF.
     */
     void SetListEndSkinStyle(const TAknsItemID *listEndId,
                                      const TRect &aRect);

    /**
     * Gets RespectFocus flag.
     *
     * @return RespectFocus flag. @c ETrue if list item highlights are only 
     *         shown  when the list is has been focused on.
     */
     TBool RespectFocus() const;
    
    /**
     * Sets RespectFocus flag.
     *
     * @param aBool Value of RespectFocus flag. @c ETrue means that the 
     *        list item highlight is only shown if list is focused on.
     */
     void SetRespectFocus(TBool aBool);

    /**
     * Set list higlight skin style.
     *
     * @since Symbian 2.0
     * @param aFrameId The id for the frame.
     * @param aFrameCenterId The id for the frame center.
     */
     void SetSkinHighlightFrame(const TAknsItemID* aFrameId,
                                        const TAknsItemID* aFrameCenterId);
    /**
     * Set popup skin style.
     *
     * @since Symbian 2.0
     * @param aFrameId The id for the frame.
     * @param aFrameCenterId The id for the frame center.
     */
     void SetSkinPopupFrame(const TAknsItemID* aFrameId, 
                                    const TAknsItemID* aFrameCenterId);
    
    /**
     * Set popup skin frame position.
     *
     * @since Symbian 2.0
     * @param aOuterRect Outer position of the frame.
     * @param aInnerRect Inner position of the frame.
     */
     void SetSkinPopupFramePosition(const TRect& aOuterRect,
                                            const TRect& aInnerRect);

    /**
     * Enable or disable logical to visual reordering in listbox data text
     * drawing. By default, it is enabled. This has any effect only when
     * bidirectional text is rendered.
     *
     * If you convert text to visual order prior to passing it to the listbox
     * (for example by wrapping text to lines with methods in
     * @c AknBidiTextUtils), then you should disable conversion in listbox by
     * calling this method.
     *
     * @since Symbian 2.0
     * @param aUseConversion Enable (@c ETrue) or disable 
     *        (@c EFalse) conversion.
     */
     void UseLogicalToVisualConversion( TBool aUseConversion );

    /**
     * Enables or disables marquee.
     *
     * @param aEnable @c ETrue if On, @c EFalse if Off.
     */
     void EnableMarqueeL(TBool aEnable);
    
    /**
     * Determines if marquee is on.
     *
     * @return Flag indicating if marquee is on (@c ETrue on, @c EFalse off)  
     */
#ifdef __WINS__
     const TBool IsMarqueeOn();
#else
     TBool IsMarqueeOn();
#endif // __WINS__
    /**
     * Function that is called when the owning control has gained the focus.
     * Causes that animation is continued because the control is visible and
     * focused.
     */
    void FocusGained();

    /**
     * Function that is called when the owning control has lost the focus.
     * Causes the animation to pause, even if the control is partially
     * visible.
     */
    void FocusLost();
    
    /**
     * Handles the skin change situation. Causes change of animation.
     *
     * @param aType Type of resource change.
     */
    void HandleResourceChange( TInt aType );
    
    /**
     * Gets animation used for drawing the list highlight.
     * May return NULL, in which case the normal skinned highlight should be
     * drawn. This pointer is valid only during the draw -> you should not
     * store it locally outside draw scope.
     *
     * @return Animation used for drawing the list highlight.
     */
     const CAknsEffectAnim* HighlightAnim() const;

    /**
     * This method should be called before drawing animated highlight. It will
     * sync the animation with the background under the current highlight (only
     * when current item index has changed).
     */
     void AboutToDrawHighlightAnim() const;

    /**
     * Sets the observer for overriding default highlight input layer drawing
     * functionality. Note that you should still call 
     * @c AboutToDrawHighlightAnim (it will call the observer if it is set).
     *
     * @param aDrawer Highlight animation drawer.
     */
     void SetHighlightAnimBackgroundDrawer( 
                                       MListBoxAnimBackgroundDrawer* aDrawer );

    /**
     * Changes item cell size and resizes highlight animation. Called by list
     * when item cell size changes. If you are using non-standard highlight 
     * sizes make sure to sync the list item cell size.
     *
     * @param aSize New cell size.
     */
     void SetItemCellSize( const TSize& aSize );

    /**
    * @return ETrue if highlight animation exists, EFalse otherwise.
    *
    * @since 3.1
    */
     TBool HasHighlightAnim() const;
    /**
    * The preferred method for drawing highlight animation in derived classes.
    * Before calling this method you should check that highlight animation
    * exists by calling HasHighlightAnim. In case of draw failure you should
    * default to normal skinned highlight drawing. For example:
    *
    * TBool drawOk = EFalse;
    *
    * if( HasHighlightAnim() )
    *     {
    *     drawOk = DrawHighlightAnim( gc, rect );
    *     }
    *
    * if( !drawOk )
    *     {
    *     ...do normal skinned draw
    *     }
    *
    * @param aGc   Graphics context for blitting animation frame
    * @param aRect Target rect for blitting animation frame
    * @return The draw status, ETrue if drawing was ok, EFalse otherwise.
    *
    * @since 3.1
    */
     TBool DrawHighlightAnim( CBitmapContext& aGc, const TRect& aRect ) const;
    /**
    * Creates pictograph interface.
    * Only effective in Japanese variant.
    * Called by the listbox.
    */
    void CreatePictographInterfaceL();

    /**
     * Creates marquee control.
     * Called by the listbox.
     */
    void CreateMarqueeControlL();

    /**
     * Resets the marquee animation data.
     */
    void ResetMarquee();

    /**
     * Gets the current list item index that is / was drawn by marquee.
     *
     * @return Index of the item that is / was drawn by marquee.
     */
    TInt CurrentMarqueeItemIndex();
    
    /**
     * Sets the current list item index that was drawn by marquee.
     *
     * @param aIndex Index of list item.
     */
    void SetCurrentMarqueeItemIndex(TInt aIndex);

    /**
     * Sets the current list item index (not for marquee). Since Symbian 3.0 
     * causes highlight animation to change its background (only if the current
     * item index changes). Don't call this method if the index is not the 
     * current item index.
     *
     * @param aIndex Index number of list item.
     */
    void SetCurrentItemIndex(TInt aIndex);

    // previous method was used both as current item index AND as currently
    // drawn item index. Not really good idea.
    void SetCurrentlyDrawnItemIndex( TInt aIndex );

    /**
     * Sets the font attached to the cell for each row separately.
     *
     * @param aRowIndex Row, which subcells are to be changed.
     * @param aSubCellIndex Index number of the subcell.
     * @param aFont Font to be used.
     */
     void SetSubCellFontForRowL(TInt aRowIndex, 
                                        TInt aSubCellIndex, 
                                        const CFont* aFont);

    /**
     * Gets font attached to the row and cell.
     *
     * @param aRow Index number of the row.
     * @param aSubCellIndex Index number of the subcell.
     * @return Font handle.
     */
     const CFont* RowAndSubCellFont(TInt aRow, 
                                            TInt aSubCellIndex) const;

    /**
     * Enables or disables transparency effect i.e. does the listbox draw its
     * own background or not.
     *
     * @param aDrawBackground @c EFalse enables transparency, @c ETrue disables.
     */ 
     void SetDrawBackground(const TBool aDrawBackground);

    /**
     * Checks if the list is transparent and drawing the background 
     * that would fall behind it.
     * 
     * @return @c EFalse if transparent, @c ETrue otherwise.
     */ 
    TBool IsBackgroundDrawingEnabled() const;

    /**
     * Enables or disables drawing background after scrollbar.
     *
     * @param aDrawBackground Default is @c ETrue which means that Scrollbar
     *        is drawn first.
     */ 
    void SetDrawScrollbarBackground(const TBool aDrawBackground);

    /**
     * Is the listbox drawing the background behind the scrollbar or not.
     *
     * @return @c ETrue if background is drawn after scrollbar, @c EFalse
     *         otherwise. 
     */ 
    TBool IsScrollbarBackgroundDrawingEnabled() const;

    /**
     * Sets position of listbox separator line.
     *
     * @param aPosition Position in which separator line is placed.
     */
     void SetSeparatorLinePosition(TAknSeparatorLinePosition aPosition);

    /**
     * Gets separator line position.
     *
     * @return Position of separator line.
     */
     TAknSeparatorLinePosition SeparatorLinePosition() const;

    /**
     *
     * @deprecated
     *
     * Gets layout data needed for separator lines drawing.
     *
     * @return Layout data.
     */
     CAknLayoutData* LayoutData() const;

    /**
     *
     * @deprecated
     *
     * Gets LayOutInit flag used for drawing separator lines in lists.
     *
     * @internal
     * @return LayOutInit flag.
     */
     TBool LayoutInit() const;
    
    /**
     *
     * @deprecated
     *
     * Sets LayOutInit flag used for drawing separator lines in lists.
     *
     * @internal
     * @param aValue LayOutInit flag value.
     */
     void SetLayoutInit(TBool aValue);

    /**
     * Enables or disables underlining per text subcell only if 
     * @c TListItemProperties also has underlining enabled. If this method is
     * not called and @c TListItemProperties has underlining enabled, drawing
     * will revert to old style where only first text subcell will be underlined.
     *
     * @since S60 3.1
     * @param aUnderlinedCells Bitmask of cells to be underlined.
     */    
     void SetSubcellUnderlined( TBitFlags32 aUnderlinedCells );

    /**
    *
    * enables certain highlight skinning for
    * radibutton / checkbox set style lists
    *
    * should be used only by avkon internally
    *
    * @since S60 3.1
    * 
    */

    void UseHighlightIconSwapping( TBool aUse );
    TBool UseHighlightIconSwapping() const;

    // next methods are only used inside the drawing algorithm.
    // The set methods are also const because they do not change observable behavior of the system.
    TSize SubCellRealSize(TInt aSubCellIndex) const;
    void SetSubCellRealSize(TInt aSubCellIndex, TSize aRealSize) const;
    //
    TSize SubCellRealTextSize(TInt aSubCellIndex) const;
    void SetSubCellRealTextSize(TInt aSubCellIndex, TSize aRealSize) const;
    

    struct SSubCell
        {
        SSubCell() { }
        TInt iSubCell; // Must be first entry
        
        TInt iWidth;
        TMargins iMargin;
        const CFont* iBaseFont;
        TInt iActualFontIndex;
        TBool iGraphics;
        TBool iTransparent;
        TBool iNotAlwaysDrawn;
        CGraphicsContext::TTextAlign iAlign;
        CGraphicsContext::TPenStyle iRightSeparatorStyle;
        
        TPoint iPosition;
        TInt iBaseline;
        TSize iSize;

        TBool iUseSubCellColors; // this is set if someone calls SetSubCellColorsL()
        TColors iColors;
        TInt iTextClipGap;

        TBool iNumberCell;
        // the next ones change during drawing algorithm
        __MUTABLE TSize iRealSize;
        __MUTABLE TSize iRealTextSize;
        };

protected:

    /**
     * C++ default constructor.
     */
	CCCAppCommLauncherCustomListBoxData();

    /**
     * Second phase constructor. Highlight animation will be 
     * created by default (if it is provided by the skin).
     */
     void ConstructLD();

    /**
     * Second phase constructor for subclasses that want to override highlight
     * animation creation.
     *
     * @param aAnimationIID Skin ItemID of the constructed animation. Passing
     *        @c KAknsIIDNone will disable highlight animation.
     */
     void ConstructLD(const TAknsItemID& aAnimationIID);

    /**
     * Main drawing algorithm used for drawing S60 list item. 
     * @c Draw() method should call this method after clearing the list item
     * area and drawing the highlight.
     *
     * @param aProperties Properties attached to the list item.
     * @param aGc Graphics Context used for drawing the list item.
     * @param aText A text string describing the list item cells.
     * @param aRect The area of the screen for the whole list item.
     * @param aHighlight Whether the list item is selected.
     * @param aColors The colors used for drawing the item.
     * @panic EAknPanicFormattedCellListInvalidBitmapIndex The defined bitmap
     *        index is invalid.
     * @panic EAknPanicOutOfRange The defined index is out of the range. 
     */
     void DrawFormatted(TListItemProperties aProperties,
                                CWindowGc& aGc,
                                const TDesC* aText,
                                const TRect& aRect,
                                TBool aHighlight,
                                const TColors& aColors) const;

    /**
    * Draws list items.
    *
    * @param aProperties Properties attached to the list item.
    * @param aGc Graphics Context used for drawing the list item.
    * @param aText A text string describing the list item cells.
    * @param aRect The area of the screen for the whole list item.
    * @param aHighlight Whether the list item is selected.
    * @param aColors The colors used for drawing the item.
    * @internal
    */
    void DrawFormattedSimple( TListItemProperties& aProperties,
                              CWindowGc& aGc,
                              const TDesC* aText,
                              const TRect& aRect,
                              TBool aHighlight,
                              const TColors& aColors ) const;
private:
    /**
    * @internal
    */
    void DrawFormattedOld( TListItemProperties& aProperties,
                           CWindowGc& aGc,
                           const TDesC* aText,
                           const TRect& aItemRect,
                           TBool aHighlight,
                           const TColors& aColors ) const;

    /**
    * @internal helper
    */
    void BitBltColored( CWindowGc&      aGc,
                        TBool           aHighlight,
                        const CGulIcon* aIcon,
                        TInt            aSubcell,
                        TBool           aColorIcon,
                        const TRect&    aGraphicRect ) const;


public:
    /**
     * Customizes marquee default behavior. 
     * Use this before enabling marquee ( @c EnableMarqueeL() ), otherwise 
     * marquee behavior will change during animation.
     *
     * @since S60 2.8
     * @param aLoops Max number of loops to be executed (default 1).
     * @param aScrollAmount The amount of pixels scrolled per frame (default 6).
     * @param aScrollDelay The delay between each loop
     *                     (in microseconds, default 1000000 microseconds).
     * @param aInterval The interval between frame updates after
     *                  the initial delay, in microseconds.
     */
     void SetMarqueeParams(const TInt aLoops,
                                   const TInt aScrollAmount, 
                                   const TInt aScrollDelay, 
                                   const TInt aInterval);


    /**
    * @internal helper
    */
    void SetClippedByWrap( TUint32 aClippedCells, TBool aUseClippedByWrap );
            
    /**
    * Tells whether current item has clipped text cells
    * @since S60 3.2
    * @return bitfield of clipped subcells
    */
     TUint32 CurrentItemTextWasClipped() const;

protected: // non-exported!

    /**
     * Gets list box extension, which is used for adding more funtionality to
     * list boxes without destroying compatibility with previous versions. 
     *
     * @return Pointer to the list box extension.
     */
	CCCAppCommLauncherCustomListBoxDataExtension* Extension();

    /**
     * Gets list box extension, which is used for adding more funtionality to
     * list boxes without destroying compatibility with previous versions. 
     *
     * @return Pointer to the list box extension.
     */
	CCCAppCommLauncherCustomListBoxDataExtension* Extension() const;

public:
    /**
     * Renders a default animated highlight.
     *
     * @param aGc Graphics context used for drawing.
     * @param aItemRect the area of the screen for drawing.
     * @param aHighlight If @c ETrue highlight is rendered
     *        if @c EFalse, nothing is done.
     */
    void DrawDefaultHighlight(CWindowGc &aGc,
                              const TRect &aItemRect,
                              TBool aHighlight) const;

    /**
     * Renders a settings animated highlight.
     *
     * @param aGc Graphics context used for drawing.
     * @param aItemRect the area of the screen for drawing.
     * @param aHighlight If @c ETrue highlight is rendered 
     *        if @c EFalse, nothing is done.
     */
    void DrawSettingHighlight(CWindowGc &aGc,
                              const TRect &aItemRect,
                              TBool aHighlight) const;

    /**
     * Renders a popup animated highlight.
     *
     * @param aGc Graphics context used for drawing.
     * @param aItemRect the area of the screen for drawing.
     * @param aHighlight If @c ETrue highlight is rendered 
     *        if @c EFalse, nothing is done.
     */
    void DrawPopupHighlight(CWindowGc &aGc,
                            const TRect &aItemRect,
                            TBool aHighlight) const;

    /**
     * Draws a popup frame.
     *
     * @param aGc Graphics context used for drawing.
     */
    void DrawPopupFrame(CWindowGc &aGc) const;

    /**
     * Disables logical-to-visual conversion in subcells between indexes.
     *
     * @param aFirstIndex First subcell not to use logical-to-visual conversion.
     * @param aSecondIndex Last subcell not to use logical-to-visual conversion.
     */
    void SetWordWrappedSubcellIndices( TInt aFirstIndex, TInt aSecondIndex );

    /**
     * Sets the size of the subcell's icon.
     *
     * @internal
     * @param aIndex Subcell to modify.
     * @param aSize New size for the icon.
     */
     void SetSubCellIconSize(TInt aIndex, TSize aSize);

    /**
     * Gets the size of the subcell's icon.
     *
     * @internal
     * @param aIndex Subcell to query.
     * @return Size of the subcell's icon.
     */
    TSize GetSubCellIconSize(TInt aIndex);

    /**
     * Create and set the defaul skin context. Needed by the dynamic skin support.
     *
     * @internal
     * @panic EAknPanicNullPointer Panics if @c iExtension has not been defined.
     */
    void SetupSkinContextL();

    /**
     * Sets a text color if enhanced skin support is available.
     *
     * @internal
     * @param aIndex Index Number of text color in @c TAknsQsnTextColorsIndex.
     * @see @c TAknsQsnTextColorsIndex.
     */
    void SetESSTextColor( TAknsQsnTextColorsIndex aIndex );

    /**
     * Set a highlighted text color if enhanced skin support is available.
     *
     * @internal
     * @param aIndex Index Number of text color in @c TAknsQsnTextColorsIndex.
     * @see @c TAknsQsnTextColorsIndex.
     */
    void SetESSHighlightedTextColor( TAknsQsnTextColorsIndex aIndex );

    /**
     * Set window line layout for the defined sub cell.
     *
     * @internal
     * @param aSubCell Target cell for which the layout is changed.
     * @param aGraphicLayout The new layout.
     */
    void SetGraphicSubCellL(TInt aSubCell, const TAknWindowLineLayout& aGraphicLayout);
    
    /**
     * Support for new layouts.
     *
     * Set text line layout for the defined sub cell.
     *
     * @internal
     * @param aSubCell Target cell.
     * @param aTextLayout The new layout. 
     */
    void SetTextSubCellL(TInt aSubCell,const TAknTextLineLayout& aTextLayout);


    /**
     * Support for new layouts.
     *
     * @internal
     * Changes setting of a graphical sub cell which is contained by the 
     * defined text sub cell. 
     *
     * @param aSubCell Target text sub cell.
     * @param aTextLayout New text layout. 
     * @param aAffectedSubCell The graphical sub cell affected by the change. 
     */
    
    TInt CorrectBaseline(TInt aParentHeight, TInt aBaseline, TInt aFontId);
    
    TBool IsParentRelative(TInt aVal);
    
    void SetConditionalSubCellL(TInt aSubCell,
                                const TAknTextLineLayout& aTextLayout,
                                TInt aAffectedSubCell);

    /**
    * Checks if the list box uses scalable layouts.
    *
    * @internal
    */
    TBool UsesScalableLayoutData() const;
    /**
    * @internal. Do NOT use.
    */
    void  UseScalableLayoutData( TBool aUse );
        
    /**
    * @internal
    */
    void SetStretchableGraphicSubCellL(TInt aSubCell, 
                                       const TAknWindowComponentLayout& aNormalLayout, 
                                       const TAknWindowComponentLayout& aStretchedLayout);
    /**
    * @internal
    */
    void SetStretchableTextSubCellL(TInt aSubCell,                                      
                                       const TAknTextComponentLayout& aNormalLayout, 
                                       const TAknTextComponentLayout& aStretchedLayout);
    /**
    * @internal
    */
    void SetStretchableConditionalSubCellL(TInt aSubCell,
                                           const TAknTextComponentLayout& aNormalLayout,
                                           const TAknTextComponentLayout& aStretchedLayout,
                                           TInt aNormalSubCell,
                                           TInt aStretchedSubCell );
    /**
    * @internal
    */
    void ResetSLSubCellArray();                                           

    /**
    * @internal
    */
    void EnableStretching(const TBool aEnabled);
    /**
    * @internal
    */
    TBool StretchingEnabled() const;
    /**
    * @internal
    */
    void HideSecondRow(const TBool aHide);
    /**
    * @internal
    */
    TBool SecondRowHidden() const;
    /**
    * @internal
    */
    void SubCellsMightIntersect( const TBool aMightIntersect );

    // Drop shadows can be toggled on/off in
    // "transparent" lists (eg. transparent camera setting page).
    // ETrue = on, EFalse = off
    /**
    * @internal
    */
    void ToggleDropShadows( const TBool aEnable );

    // changes tactile feedback behaviour
    // ETrue  - feedback for any item
    // EFalse - feedback for focused item only
    /**
    * @internal
    */
    void SetFeedbackForAllItems( TBool aFeedbackForAllItems );
    // gets tactile feedback behaviour
    // ETrue  - feedback for any item
    // EFalse - feedback for focused item only
    /**
    * @internal
    */
    TBool FeedbackForAllItems();

    /**
    * Returns ETrue if kinetic scrolling is enabled.
    * @internal
    * @since S60 5.0
    * @return ETrue if kinetic scrolling feature is enabled.
    */
    TBool KineticScrollingEnabled() const;
    
    /**
    * Returns instance of CEikListBox.
    * @internal
    * @since S60 5.0
    * @return Instance of CEikListBox.
    */
    CEikListBox* ListBox() const;

    
private:
    void DoConstructL( const TAknsItemID& aAnimationIID );

    const TColors &Colors(TInt aItemIndex, TInt aSubCell) const;
    TInt LastSubCell() const;
    void AddSubCellL(TInt aSubCell);
    SSubCell& At(TInt aArrayIndex);
    const SSubCell& At(TInt aArrayIndex) const;
    TInt FindSubCellIndex(TInt& aArrayIndex,TInt aSubCell) const;
    void FindSubCellIndexOrAddL(TInt& aArrayIndex,TInt aSubCell);
    TInt AddActualFontL(const CFont* aBaseFont);
    void SetUnderlineStyle( TListItemProperties aProperties, CWindowGc& aGc, TInt aSubCell ) const;
    void CheckIfSubCellsIntersect( TAknTextLineLayout* aLayouts, TBool* aResults, const TDesC& aText, const TRect& aItemRect ) const;

private:
    CArrayFix<SSubCell>* iSubCellArray;
    CArrayPtr<CGulIcon>* iIconArray;
    friend class CCCAppCommLauncherCustomListBoxItemDrawer; // this is for "lastsubcell" -access.
    TColors defaultcolors;
    CCCAppCommLauncherCustomListBoxDataExtension *iExtension; 
    };


#endif /* CCAPPCOMMLAUNCHERCUSTOMLISTBOXDATA_H_ */
