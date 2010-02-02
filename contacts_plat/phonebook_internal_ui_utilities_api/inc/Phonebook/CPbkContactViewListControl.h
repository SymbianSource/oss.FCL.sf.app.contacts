/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*    Phonebook contact view list control.
*
*/


#ifndef __CPbkContactViewListControl_H__
#define __CPbkContactViewListControl_H__


//  INCLUDES
#include <cntviewbase.h>    // MContactViewObserver
#include <coecobs.h>        // MCoeControlObserver
#include <babitflags.h>     // TBitFlags

#include <PbkIconId.hrh>    // TPbkIconId
#include <MPbkContactDbObserver.h>
#include <CPbkContactListControlBase.h>

//  FORWARD DECLARATIONS
class CPbkContactFindView;
class CEikColumnListBox;
class CEikMenuPane;
class CAknSearchField;
class CPbkThumbnailPopup;
class CPbkViewState;
class MPbkContactEntryLoader;
class MPbkContactViewListControlObserver;
class MPbkContactViewListModel;
class TPbkContactViewListControlEvent;
class MPbkFindPrimitives;
class MPbkContactUiControlExtension;
class MPbkFetchDlgSelection;
/// Use for testing only!!!
class CPbkContactViewListControl_TestAccess;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook contact view list control. Used in all views and dialogs which
 * display contacts in a list box.
 */
class CPbkContactViewListControl :
		public CPbkContactListControlBase,
		private MCoeControlObserver,
		private MContactViewObserver
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         * @param aEngine phonebook engine instance.
         * @param aView the contact view this list control displays.
         * @param aResourceId if of a PBK_CONTACTVIEWLISTCONTROL resource to
         *        use for intialisation.
         * @param aParent container for this control. If NULL this control
         *        creates its own window.         
         */
        IMPORT_C static CPbkContactViewListControl* NewL
            (CPbkContactEngine& aEngine,
            CContactViewBase& aView,
            TInt aResourceId,
            const CCoeControl* aParent);

        /**
         * C++ default constructor.
         */
        IMPORT_C CPbkContactViewListControl();

        /**
         * Second phase constructor, from CCoeControl. Initialises this object
         * from a PBK_ENTRYLIST_CONTROL resource struct using aReader.
		 * Should be called before ConstructL().
         */
	    void ConstructFromResourceL(TResourceReader& aReader);

        /**
         * "Third phase" constructor. Should be called after 
         * ConstructFromResourceL().
         * @param aEngine   Phonebook engine instance.
         * @param aView     the contact view this list control displays.
         */
        IMPORT_C void ConstructL
            (CPbkContactEngine& aEngine, CContactViewBase& aView);

		/**
         * Destructor.
         */ 
        ~CPbkContactViewListControl();

    public:  // New functions

        /**
         * Returns the Contact id at listbox index aIndex.
         */
        IMPORT_C TContactItemId ContactIdAtL(TInt aIndex) const;

        /**
         * Returns the listbox index of entry having contact id aId, 
         * -1 if not found.
         */
        IMPORT_C TInt FindContactIdL(TContactItemId aId) const; 

        /**
         * Returns the currently focused item's index.         
         */
        IMPORT_C TInt CurrentItemIndex() const;

        /**
         * Sets the currently focused item's index to aIndex.
         */
        IMPORT_C void SetCurrentItemIndex(TInt aIndex);

        /**
         * Sets the currently focused item's index to aIndex and updates
         * the control on the display.
         */
        IMPORT_C void SetCurrentItemIndexAndDraw(TInt aIndex);

        /**
         * Handles aCommandId if it is a markable list box command.
         * @see AknSelectionService::HandleMarkableListProcessCommandL.
         */
        IMPORT_C void HandleMarkableListProcessCommandL(TInt aCommandId);

        /**
         * Updates a menu pane containing markable list commands.
         * @see AknSelectionService::HandleMarkableListDynInitMenuPane
         */
        IMPORT_C void HandleMarkableListDynInitMenuPane
            (TInt aResourceId, CEikMenuPane* aMenu);

		/**
         * Updates listbox state after command execution.
         * @see AknSelectionService::HandleMarkableListUpdateAfterCommandExecution.
         */
        IMPORT_C void HandleMarkableListUpdateAfterCommandExecution();

        /**
         * Returns the text currenlty in find box. KNullDesC if the find box
         * is not visible.
         */
        IMPORT_C const TDesC& FindTextL() const;

        /**
         * Resets any string in the find box.
         */
        IMPORT_C void ResetFindL();

        /**
         * Returns ETrue if item at aIndex is currently marked.
         */
        IMPORT_C TBool ItemMarked(TInt aIndex) const;

        /**
         * Returns index of next unmarked item starting from focused item.
         */
        IMPORT_C TInt NextUnmarkedIndexFromFocus() const;

        /**
         * Marks or unmarks a contact item in the list.
         *
         * @param aContactId Id of Contact item to mark or unmark.
         * @param aMark ETrue: set the mark, EFalse: remove the mark.
         */
        IMPORT_C void MarkItemL(TContactItemId aContactId, TBool aMark);

        /**
         * Umarks all marked items.
         */
        IMPORT_C void ClearMarks();

        /**
         * Creates a new CPbkViewState and stores this control's 
         * state to it.
         * @param aSaveMarks if true saves also marked entries to returned
         *        view state object.
         * @return this control's saved state. Caller is responsible for
         *         deleting the returned object.
         * @see RestoreState
         */
        IMPORT_C CPbkViewState* GetStateL(TBool aSaveMarks=ETrue) const;

        /**
         * Same as GetStateL, but leaves the object on the cleanup stack.
         * @see GetStateL
         */
        IMPORT_C CPbkViewState* GetStateLC(TBool aSaveMarks=ETrue) const;

        /**
         * Restores this control to a previously saved state.
         * @param aState the state where to restore this control. 
         *        If NULL the function does nothing.
         * @see GetState
         */
        IMPORT_C void RestoreStateL(const CPbkViewState* aState);
        
		/**
         * Shows the thumbnail image if the currently focused contact has
         * one.
         */
        IMPORT_C void ShowThumbnailL();

        /**
         * Hides thumbnail window if visible. Call UpdateL() to restore
         * thumbnail.
         */
        IMPORT_C void HideThumbnail();

        /**
         * Adds an observer aObserver to this control.
         */
        IMPORT_C void AddObserverL
            (MPbkContactViewListControlObserver& aObserver);

		/**
         * Removes an observer aObserver from this control.
         */
        IMPORT_C void RemoveObserver
            (MPbkContactViewListControlObserver& aObserver);

		/**
         * Returns ETrue when this control is ready to use.
         */
        IMPORT_C TBool IsReady() const;

        /**
         * Disables/enables listbox control redrawing.
         * @see CListBoxView::SetDisableRedraw(TBool)
         */
	    void SetDisableRedraw(TBool aDisableRedraw);

        /**
         * Disables listbox redrawing and pushes a cleanup item on the cleanup 
         * stack which enables redrawing again when PopAndDestroy()'ed.
         * @see SetDisableRedraw(TBool)
         */
	    void DisableRedrawEnablePushL();

        /**
         * Blanks/unblanks this control. Use when there are massive updates
         * to the database to avoid flickering.
		 * @param aBlank ETrue blanks the control, EFalse unblanks it
         */
        IMPORT_C void SetBlank(TBool aBlank);

        /**
         * Set contact entry loader to aContactEntryLoader.
         */
        IMPORT_C void SetEntryLoader
            (MPbkContactEntryLoader& aContactEntryLoader);
        
        /**
         * Set selection accepter. If accepter is set, all item selections
         * are first checked from accepter. It can then prevent the
         * selection from happening.
         * @param aAccepter Pointer to the accepter
         */
        void SetSelectionAccepter(MPbkFetchDlgSelection* aAccepter);

        /**
        * Deletes this control thumbnail
        */
        IMPORT_C void DeleteThumbnail();
        
        /**
        * Calls Listbox's corresponding method. 
        * @param aEnable If EFalse, removes the listbox's own observer        
        */
        void EnableMSKObserver( TBool aEnable );
        
        /**
        * Returns the count of items on a list.
        * @return item count
        */
        TInt ItemCount();
        
	public: // From MPbkContactUiControl
        /**
         * Returns the number of entries in the list box.
         * NOTE: if find is active (there is text in the find box) this
         * function returns number of currently visible items only!
         */
        TInt NumberOfItems() const;
        TBool ItemsMarked() const;
        const CContactIdArray& MarkedItemsL() const;
        TContactItemId FocusedContactIdL() const;
        const TPbkContactItemField* FocusedField() const;
        MObjectProvider* ObjectProvider();

    public:  // from CCoeControl
        TKeyResponse OfferKeyEventL
            (const TKeyEvent& aKeyEvent,TEventCode aType);
        void HandlePointerEventL(const TPointerEvent& aPointerEvent);            
	    void MakeVisible(TBool aVisible);

    public: // from MPbkContactUiControlUpdate
        void UpdateContact(TContactItemId aContactId);

    private:  // from CCoeControl
	    void ConstructFromResourceL(TInt aResourceId);
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
	    void FocusChanged(TDrawNow aDrawNow);
        void SizeChanged();
        void Draw(const TRect& aRect) const;
        void HandleResourceChange(TInt aType);
        void DoHandleResourceChangeL(TInt aType);        

    private:  // from MCoeControlObserver
        void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

    private:  // from MContactViewObserver
	    void HandleContactViewEvent
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);
            
    private:  // Implementation
        void ConstructL
            (CPbkContactEngine& aEngine, CContactViewBase& aView,
            TInt aResourceId, const CCoeControl* aParent);
        CEikColumnListBox& ListBox() const;
        MPbkContactViewListModel& Model() const;
        void FixTopItemIndex();
        void Redraw();
        void MakeComponentsVisible(TBool aVisible); 
        void HandleItemAdditionL(TInt aIndex);
        void HandleItemRemovalL(TInt aIndex);
        void UpdateFindBoxL();
        void EnableFindBoxL();
        void DisableFindBoxL();
        TBool UpdateFindResultL();
        void SetFindEmptyTextL();
        void RemoveFindEmptyTextL();
	    void DoHandleContactViewEventL
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);
        static TInt NoEventsInLastPeriod(TAny* aPtr);
        void SendEventToObserversL
            (const TPbkContactViewListControlEvent& aEvent);
        void ShowThumbnail(TContactItemId aContactId);
        void HandleFocusChange();
        void HandleFocusChangeL();
        TBool RestoreMarkedItemsL(const CContactIdArray* aMarkedContactIds);
        TBool PostCond_Constructor();
        TBool PostCond_ConstructFromResource();
        void CreateControlExtensionL(CPbkContactEngine& aEngine);
        void RefreshIconArrayL();

        /// State flags
        enum TStateFlags
            {
            /// Set when this control is ready to use
            EReady,
            /// Set when this control should draw only blank
            EBlank,
            };

        /// Data loaded from resources
        struct TResourceData
            {
            /// Own: flags
            TUint iFlags;
            /// Own: text to use for unnamed entries
            HBufC* iUnnamedText;
            /// Own: empty icon id
            TPbkIconId iEmptyIconId;
            /// Own: default icon id
            TPbkIconId iDefaultIconId;
            /// Own: listbox empty text in find state
            HBufC* iFindEmptyText;
            };

        /// Use for testing only!!!
        friend class CPbkContactViewListControl_TestAccess;

    private:  // data
        /// Own: Listbox control
        CEikColumnListBox* iListBox;
        /// Own: find box control
        CAknSearchField* iFindBox;
        /// Own: Thumbnail popup
        CPbkThumbnailPopup* iThumbnailPopup;
        /// Own: state flags
        TBitFlags iStateFlags;
        /// Own: the contact view this list control is displaying
        CContactViewBase* iBaseView;
        /// Own: wrapper view over iBaseView implementing find functionality
        CPbkContactFindView* iView;
        /// Own: find primitives implementation for iView
        MPbkFindPrimitives* iFindPrimitives;
        /// Own: Buffer for text retrieved from iFindBox
        mutable HBufC* iFindTextBuf;
        /// Own: marked items array returned from MarkedItemsL()
        mutable CContactIdArray* iMarkedItemsArray;
        /// Own: contact item id where focus was the last time
        TContactItemId iLastFocusId;
        /// Own: observers of this object
        RPointerArray<MPbkContactViewListControlObserver> iObservers;
        /// Own: standard listbox empty text
        HBufC* iOriginalEmptyText;
        /// Own: data loaded from resources
        TResourceData iResourceData;
        /// Own: indexes of the changed list items
        CArrayFixFlat<TInt> iChangedIndexes;
        /// Own: for setting and releasing contact ui control
        MPbkContactUiControlExtension* iControlExtension;
        /// Not own: for asking wheter a contact item can be selected 
        MPbkFetchDlgSelection* iSelectionAccepter;
        /// Ref: Contact Engine
        CPbkContactEngine* iEngine;
        /// Own: Contact item index preceding the latest stylus tap
        TInt iPrevIndex;        
    };


#endif // __CPbkContactViewListControl_H__
            
// End of File
