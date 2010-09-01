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
* Description:  Phonebook 2 names list ready state.
*
*/


#ifndef CPBK2NAMESLISTREADYSTATE_H
#define CPBK2NAMESLISTREADYSTATE_H

//  INCLUDES
#include <e32base.h>
#include <coecntrl.h>
#include <aknsfld.h>
#include "MPbk2NamesListState.h"
#include "MPbk2FindDelayObserver.h"
#include "RPbk2ContactBookmarkCollection.h"
#include <MPbk2UiControlCmdItem.h>
#include <MPbk2CmdItemVisibilityObserver.h>

// FORWARD DECLARATIONS
class CPbk2ContactViewListBox;
class MVPbkContactViewBase;
class MVPbkContactLinkArray;
class MVPbkContactBookmark;
class CAknSearchField;
class CPbk2ThumbnailLoader;
class MPbk2UiControlEventSender;
class TPointerEvent;
class CPbk2FindDelay;
class MPbk2Command;
class MPbk2ContactNameFormatter;
class CPbk2FindStringSplitter;
class CVPbkContactFindPolicy;
class CPbk2AdaptiveSearchGridFiller;
class CPbk2PredictiveSearchFilter;
class CVPbkContactLinkArray;
// CLASS DECLARATION

/**
 * Phonebook 2 names list ready state.
 * Responsible for:
 * - Implements names list control's ready state
 */
NONSHARABLE_CLASS(CPbk2NamesListReadyState) :
        public CBase,
        public MPbk2NamesListState,
        public MPbk2FindDelayObserver,
        public MAdaptiveSearchTextObserver,
        public MPbk2CmdItemVisibilityObserver
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aParent               Parent control.
         * @param aListBox              Reference to the list box.
         * @param aViewStack            A view stack for findbox filtering
         * @param aView                 The contact view.
         * @param aFindBox              Reference to find box.
         * @param aThumbnailLoader      Thumbnail loader.
         * @param aEventSender          UI control event sender.
         * @param aNameFormatter        Phonebook 2 contact name formatter interface
         * @param aCommandItems         Array of UI control Command item interface
         * @param aSearchFilter         PBk2 Search Filter
         * @return  A new instance of this class.
         */
        static CPbk2NamesListReadyState* NewLC(
                const CCoeControl* aParent,
                CPbk2ContactViewListBox& aListBox,
                MPbk2FilteredViewStack& aViewStack,
                CAknSearchField* aFindBox,
                CPbk2ThumbnailLoader& aThumbnailLoader,
                MPbk2UiControlEventSender& aEventSender,
                MPbk2ContactNameFormatter& aNameFormatter,
                RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
                CPbk2PredictiveSearchFilter& aSearchFilter );

        /**
         * Destructor.
         */
        ~CPbk2NamesListReadyState();

    public: // From MPbk2NamesListState
        void ActivateStateL();
        void DeactivateState();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(
                TInt aIndex ) const;
        void SizeChanged();
        void MakeComponentsVisible(
                TBool aVisible );
        void ActivateL();
        void Draw(
                const TRect& aRect,
                CWindowGc& aGc ) const;
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        TInt NamesListState() const;
        void HandleContactViewEventL(
                TInt aEvent,
                TInt aIndex );
        void HandleCommandEventL(TInt aEvent, TInt aCmdIndex);
        void HandleCommandItemEventL(TInt aEvent, TInt aListboxIndex);        
        CCoeControl& CoeControl();
        void HandleControlEventL(
                CCoeControl* aControl,
                MCoeControlObserver::TCoeEvent aEventType, TInt aParam );
        const MVPbkViewContact* TopContactL();

    public: // From MPbk2ContactUiControl
        MPbk2ContactUiControl* ParentControl() const;
        TInt NumberOfContacts() const;
        const MVPbkBaseContact* FocusedContactL() const;
        const MVPbkViewContact* FocusedViewContactL() const;
        const MVPbkStoreContact* FocusedStoreContact() const;
        void SetFocusedContactL(
                const MVPbkBaseContact& aContact );
        void SetFocusedContactL(
                const MVPbkContactBookmark& aContactBookmark );
        void SetFocusedContactL(
                const MVPbkContactLink& aContactLink );
        TInt FocusedContactIndex() const;
        void SetFocusedContactIndexL(
                TInt aIndex );
        TInt NumberOfContactFields() const;
        const MVPbkBaseContactField* FocusedField() const;
        TInt FocusedFieldIndex() const;
        void SetFocusedFieldIndex(
                TInt aIndex );
        TBool ContactsMarked() const;
        MVPbkContactLinkArray* SelectedContactsL() const;
        MVPbkContactLinkArray* SelectedContactsOrFocusedContactL() const;
        MPbk2ContactLinkIterator* SelectedContactsIteratorL() const;
        CArrayPtr<MVPbkContactStore>* SelectedContactStoresL() const;
        void ClearMarks();
        void SetSelectedContactL(
                TInt aIndex,
                TBool aSelected );
        void SetSelectedContactL(
                const MVPbkContactBookmark& aContactBookmark,
                TBool aSelected );
        void SetSelectedContactL(
                const MVPbkContactLink& aContactLink,
                TBool aSelected );
        void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane ) const;
        void ProcessCommandL(
                TInt aCommandId ) const;
        void UpdateAfterCommandExecution();
        TInt GetMenuFilteringFlagsL() const;
        CPbk2ViewState* ControlStateL() const;
        void RestoreControlStateL(
                CPbk2ViewState* aState );
        const TDesC& FindTextL();
        void ResetFindL();
        void HideThumbnail();
        void ShowThumbnail();
        void SetBlank(
                TBool aBlank );
        void RegisterCommand(
                MPbk2Command* aCommand );
        void SetTextL(
                const TDesC& aText );
        TAny* ContactUiControlExtension(TUid aExtensionUid );        
    
    public: // From MPbk2ContactUiControl2
        TInt CommandItemCount() const;
        const MPbk2UiControlCmdItem& CommandItemAt( TInt aIndex ) const;
        const MPbk2UiControlCmdItem* FocusedCommandItem() const;
        void DeleteCommandItemL( TInt aIndex );
        void AddCommandItemL(MPbk2UiControlCmdItem* aCommand, TInt aIndex);
    
    public: // From MPbk2PointerEventInspector
        TBool FocusedItemPointed();
        TBool FocusableItemPointed();
        TBool SearchFieldPointed();

    public: // From MPbk2FilteredViewStackObserver
        void TopViewChangedL( MVPbkContactViewBase& aOldView );
        void TopViewUpdatedL();
        void BaseViewChangedL();
        void ViewStackError( TInt aError );
        void ContactAddedToBaseView(
                MVPbkContactViewBase& aBaseView, TInt aIndex,
                const MVPbkContactLink& aContactLink );

    public: // From MPbk2FindDelayObserver
        void FindDelayComplete();
        
    public: // From MAdaptiveSearchTextObserver
        void AdaptiveSearchTextChanged( CAknSearchField* aSearchField ); 


    private: // From MPbk2AddFavoritesVisibilityObserver
        void CmdItemVisibilityChanged( TInt aCmdItemId, TBool aVisible );

    private: // Implementation
        CPbk2NamesListReadyState(const CCoeControl& aParent,
                CPbk2ContactViewListBox& aListBox,
                MPbk2FilteredViewStack& aViewStack,
                CAknSearchField* aFindBox,
                CPbk2ThumbnailLoader& aThumbnailLoader,
                MPbk2UiControlEventSender& aEventSender,
                MPbk2ContactNameFormatter& aNameFormatter,
                RPointerArray<MPbk2UiControlCmdItem>& aCommandItems,
                CPbk2PredictiveSearchFilter& aSearchFilter );
        void ConstructL();
        void SetFocusedContactL( TInt aIndex,
                const MVPbkBaseContact* aContact  );
        TBool RestoreMarkedContactsL(
                const MVPbkContactLinkArray* aMarkedContacts );
        void RestoreMarkedItemsL(
            const MVPbkContactBookmarkCollection& aMarkedItems );
        void DisableRedrawEnablePushL();
        void HandleFocusChangeL();
        void UpdateFindResultL();
        void UpdateFindBoxL();
        MVPbkContactBookmarkCollection& SelectedItemsL(
            MVPbkContactViewBase& aCurrentView );
        TBool HasSelectedItemsChangedL();
        void SetCurrentItemIndexAndDrawL();
        void UpdateAdaptiveSearchGridL( TBool aClearCache );
        TBool IsContactAtListboxIndex( TInt aListboxIndex ) const;
        void AllowCommandsToShowThemselves( TBool aVisible );  
        void UpdateCommandEnabled(TInt aCommandId, TBool aEnabled);
        void UnmarkCommands() const;
        TBool IsCommandEnabled( TInt aCommandId) const;
        TInt FindCommand(TInt aCommandId) const;
        TInt CalculateListboxIndex(TInt aCommandIndex) const;                
        void UnsubscribeCmdItemsVisibility() const;
        void SubscribeCmdItemsVisibility();
        void HandleListboxEventL(TInt aEvent, TInt aListboxIndex, TBool aMantainFocus);
        TInt EnabledCommandCount() const; 

        /*
         * If wanted contact link is my card, function checks command item extension
         * and fetches my card link. If the link exist, it is appended to the array
         */
        void AppendMyCardLinkIfExistL( CVPbkContactLinkArray& aArray ) const;
        
        CEikMenuBar* DialogMenuBar(CAknAppUi* aAppUi);
        CEikMenuBar* ViewMenuBar(CAknAppUi* aAppUi);
        CEikMenuBar* MenuBar(CAknAppUi* aAppUi);
        
    private: // Data
        /// Own: Buffer for text retrieved from iFindBox
        HBufC* iFindTextBuf;
        /// Own: find start typing delay
        CPbk2FindDelay* iFindDelay;
        /// Ref: Parent coecontrol
        const CCoeControl& iParent;
        /// Ref: The list box
        CPbk2ContactViewListBox& iListBox;
        /// Ref: The control view stack that also the contact view.
        MPbk2FilteredViewStack& iViewStack;
        /// Ref: Find box control
        CAknSearchField* iFindBox;
        /// Ref: Thumbnail loader
        CPbk2ThumbnailLoader& iThumbnailLoader;
        /// Own: Is this state visible
        TBool iVisible;
        /// Ref: UI control event sender
        MPbk2UiControlEventSender& iEventSender;
        /// Own: Search field indicator
        TBool iSearchFieldPointed;
        /// Own: Collection of selected/marked items
        RPbk2ContactBookmarkCollection iSelectedItems;
        /// Ref: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Own: Array of search strings
        MDesCArray* iStringArray;
        /// Own: String splitter
        CPbk2FindStringSplitter* iStringSplitter;
        /// Own: Contact find policy
        CVPbkContactFindPolicy* iFindPolicy;
        /// Own: User to create keymaps for adaptive search grid.
        CPbk2AdaptiveSearchGridFiller *iAdaptiveSearchGridFiller;
        /// Ref: Command items
        RPointerArray<MPbk2UiControlCmdItem>& iCommandItems;
        /// Iterator indexes.
        mutable RArray<TInt> iIteratorIndexes;
        /// Ref :Search Pane Filter
        CPbk2PredictiveSearchFilter& iSearchFilter;
    };

#endif // CPBK2NAMESLISTREADYSTATE_H

// End of File
