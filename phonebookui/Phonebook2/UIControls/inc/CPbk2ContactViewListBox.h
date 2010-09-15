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
* Description:  Phonebook 2 contact view list box.
*
*/


#ifndef CPBK2CONTACTVIEWLISTBOX_H
#define CPBK2CONTACTVIEWLISTBOX_H

//  INCLUDES
#include <aknlists.h>
#include <TPbk2IconId.h>
#include "MPbk2ClipListBoxText.h"
#include <MPbk2PointerEventInspector.h>
#include <MPbk2UiControlCmdItem.h>
#include <coemain.h>
#include "CPbk2ContactViewListBoxModel.h"


// FORWARD DECLARATIONS
class CPbk2ContactViewListBoxModel;
class CVPbkContactManager;
class MVPbkContactViewBase;
class MPbk2ContactNameFormatter;
class CPbk2StorePropertyArray;
class MPbk2ContactUiControlExtension;
class CPbk2ListboxModelCmdDecorator;
class CPbk2ContactViewCustomListBoxData;
class CPbk2PredictiveSearchFilter;
class CCoeControl;

/**
 * Phonebook 2 contact view list box.
 */
NONSHARABLE_CLASS(CPbk2ContactViewListBox) :
            public CAknSingleGraphicStyleListBox,
            public MPbk2ClipListBoxText,
            public MPbk2PointerEventInspector,
            private MCoeForegroundObserver
    {
    
    public:        
        /** CreateItemDrawerL()
         *
         * Chooses which itemdrawer to use.
         */
        void CreateItemDrawerL();

    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContainer        The control container for this control.
         * @param aResourceReader   Resource reader used to construct
         *                          this object.
         * @param aManager          Reference to contact manager.
         * @param aView             Reference to contact view.
         * @param aNameFormatter    Reference to the name formatter.
         * @param aStoreProperties  Store property array.
         * @param aUiExtension      UI extension.
         * @param aSearchFilter PBk2 Search Filter
         * @return  A new instance of this class.
         */
        static CPbk2ContactViewListBox* NewL(
                CCoeControl& aContainer,
                TResourceReader& aResourceReader,
                const CVPbkContactManager& aManager,
                MVPbkContactViewBase& aView,
                MPbk2ContactNameFormatter& aNameFormatter,
                CPbk2StorePropertyArray& aStoreProperties,
                MPbk2ContactUiControlExtension* aUiExtension,
                CPbk2PredictiveSearchFilter &aSearchFilter );

        /**
         * Destructor.
         */
        ~CPbk2ContactViewListBox();

    public: // Interface

        /**
         * Returns the number of items in the list box currently.
         *
         * @return  Number of items in the listbox.
         */
        TInt NumberOfItems() const;

        /**
         * Returns list box selection status.
         *
         * @return  ETrue if there are items marked, EFalse otherwise.
         */
        TBool ContactsMarked() const;

        /**
         * Handles item addition / deletion etc 
         * events by updating list box data and the focus.
         *
         * @param aEvent contact view event 
         *               @see MPbk2NamesListState::HandleContactViewEventL
         * @param aIndex listbox index
         */
        void HandleEventL(
                TInt aEvent,
                TInt aIndex );

        /**
         * Fixes the top item index of the list box.
         */
        void FixTopItemIndex();

        /**
         * Checks that there is focus in the list.
         */
        void CheckFocusIndex();

        /**
         * Handles pointer event.
         *
         * @see CAknSingleGraphicStyleListBox::HandlePointerEventL
         */
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );

        /**
         * Draws the listbox.
         * 
         * @see CAknSingleGraphicStyleListBox::Draw
         */
        void Draw( const TRect& aRect ) const;
        
        /**
         * Returns Phonebook 2 control flags defined in Pbk2UIControls.hrh.
         *
         * @return  Control flags.
         */
        TUint Flags() const;

        /**
         * Returns ETrue if the list supports selection.
         *
         * @return  ETrue if the list supports selection.
         */
        TBool IsSelectionListBox();

        /**
         * Sets the scroll event observer.
         */
        void SetScrollEventObserver(MEikScrollBarObserver* aObserver);

        /**
 	     * Sets the commands that should appear at the top of the list.
 	     * @param aCommands An array of commands. NULL may be given to
 	     * 		  indicate that no commands are to be shown. 
         */
        void SetListCommands( const RPointerArray<MPbk2UiControlCmdItem>* aCommands );

        /**
         * Handle showing of popupcharacter when user scrolls name list 
         * using scroll bar.
         *
         * @param CWindowGc.
         * @param TRect.
         */
        void HandlePopupCharacter( CWindowGc* aGc,
                                   const TRect& aRectOfListBoxItem ) const;

        /**
         * Set the state of the Marking mode 
         */
        inline void SetMarkingModeState( TBool aActived )
            {
            iMarkingModeOn = aActived;
            }

        /**
         * Reset the value of iShowPopupChar, i.e. set its value to EFalse.
         * As for iShowPopupChar, ETrue indicates showing the scrollbar popup char;
         * EFalse indicates the opposite. 
         */
        inline void ResetShowPopupChar()
            {
            iShowPopupChar = EFalse;
            }

    public: // From MPbk2ClipListBoxText
        TBool ClipFromBeginning(
                TDes& aBuffer,
                TInt aItemIndex,
                TInt aSubCellNumber );

    public: // From MPbk2PointerEventInspector
        TBool FocusedItemPointed();
        TBool FocusableItemPointed();
        TBool SearchFieldPointed();

    public: // From CEikListBox
        void HandleScrollEventL( CEikScrollBar* aScrollBar,
                                 TEikScrollEvent aEventType );

    private: // MCoeForegroundObserver
        void HandleGainingForeground();
        void HandleLosingForeground();

    protected: // Implementation
        CPbk2ContactViewListBox(
                CCoeControl& aContainer,
                MPbk2ContactUiControlExtension* aUiExtension,
                CPbk2PredictiveSearchFilter &aSearchFilter );
        void ConstructL(
                CCoeControl& aContainer,
                TResourceReader& aResourceReader,
                const CVPbkContactManager& aManager,
                MVPbkContactViewBase& aView,
                MPbk2ContactNameFormatter& aNameFormatter,
                CPbk2StorePropertyArray& aStoreProperties );
        void CreateListBoxModelL(
                const CVPbkContactManager& aManager,
                MVPbkContactViewBase& aView,
                MPbk2ContactNameFormatter& aNameFormatter,
                CPbk2StorePropertyArray& aStoreProperties );
        
        
        virtual CPbk2ContactViewListBoxModel* DoCreateModelL( 
        		CPbk2ContactViewListBoxModel::TParams params );
        
        virtual CPbk2ListboxModelCmdDecorator* DoCreateDecoratorL( 
        		const CPbk2IconArray& aIconArray,
        		TPbk2IconId aEmptyIconId,
        		TPbk2IconId aDefaultIconId );

        TBool IsContact( TInt aIndex ) const;
        
    private:
        void RecalcWidthInMarkingMode( TInt& aWidth, const CFont& aFont, TChar aChar );
        
    private: // Data structures
        /// Data loaded from resources
        struct TResourceData
            {
            /// Own: Flags
            TUint iFlags;
            /// Own: Text to use for unnamed entries
            HBufC* iUnnamedText;
            /// Own: Empty icon id
            TPbk2IconId iEmptyIconId;
            /// Own: Default icon id
            TPbk2IconId iDefaultIconId;
            };

    protected:	//data
    	/// Own: List box model
    	CPbk2ContactViewListBoxModel* iModel;
    	/// Own: Data loaded from resources
        TResourceData iResourceData;
        /// Own: Indexes of the changed list items
        CArrayFixFlat<TInt> iChangedIndexes;
        
        /// Ref: UI extension
        MPbk2ContactUiControlExtension* iUiExtension;
        /// Own: Pointer movement indicator
        TBool iFocusedContactPointed;
        /// Own: Pointer movement indicator
        TBool iFocusableContactPointed;
        /// Ref: scroll event observer
        MEikScrollBarObserver* iScrollEventObserver;
        /// Own: Wraps the listbox model
        CPbk2ListboxModelCmdDecorator* iListboxModelDecorator;
        /// For handling of scroll bar popupcharacter
        TBool iShowPopupChar;
        /// Doesnt Own - Ownership Transferred to CPbk2ContactViewCustomListBoxItemDrawer
        CPbk2ContactViewCustomListBoxData* iColumnData;
        ///Ref : Search Pane Filter
        CPbk2PredictiveSearchFilter &iSearchFilter;
        // ref: used fro checking command item count
        CCoeControl& iContainer;
        // Flag to indicate whether Marking mode is active
        TBool iMarkingModeOn;
        
    };

#endif // CPBK2CONTACTVIEWLISTBOX_H

// End of File
