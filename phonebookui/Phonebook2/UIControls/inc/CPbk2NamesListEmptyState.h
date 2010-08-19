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
* Description:  Phonebook 2 names list empty state.
*
*/


#ifndef CPBK2NAMESLISTEMPTYSTATE_H
#define CPBK2NAMESLISTEMPTYSTATE_H

//  INCLUDES
#include <e32base.h>
#include "MPbk2NamesListState.h"
#include <MPbk2CmdItemVisibilityObserver.h>

// FORWARD DECLARATIONS
class CPbk2ContactViewListBox;
class CAknSearchField;
class CPbk2ThumbnailLoader;

// CLASS DECLARATION

/**
 * Phonebook 2 names list empty state.
 * Responsible for:
 * - implements names list control empty state
 */
NONSHARABLE_CLASS(CPbk2NamesListEmptyState) :
        public CBase,
        public MPbk2NamesListState,
        public MPbk2CmdItemVisibilityObserver 
    {
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aParent           Parent control.
         * @param aListBox          Reference to the list box.
         * @param aFindBox          Reference to the find box.
         * @param aThumbnailLoader  Thumbnail loader.
         * @return  A new instance of this class.
         */
        static CPbk2NamesListEmptyState* NewLC(
                const CCoeControl* aParent,
                CPbk2ContactViewListBox& aListBox,
                CAknSearchField* aFindBox,
                CPbk2ThumbnailLoader& aThumbnailLoader,
                RPointerArray<MPbk2UiControlCmdItem>& aCommandItems );  

        /**
         * Destructor.
         */
        ~CPbk2NamesListEmptyState();

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
        void HandleCommandEventL(TInt aEvent, TInt aListboxIndex);
        CCoeControl& CoeControl();
        void HandleControlEventL( CCoeControl* aControl,
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
                MPbk2Command* aCommand);
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

    private: // Implementation
        void CmdItemVisibilityChanged( TInt aCmdItemId, TBool aVisible );  

        CPbk2NamesListEmptyState(
                const CCoeControl* aParent,
                CPbk2ContactViewListBox& aListBox,
                CAknSearchField* aFindBox,
                CPbk2ThumbnailLoader& aThumbnailLoader,
                RPointerArray<MPbk2UiControlCmdItem>& aCommandItems );	
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

    private: // Data
        /// Ref: Parent coecontrol
        const CCoeControl& iParent;
        /// Ref: The list box
        CPbk2ContactViewListBox& iListBox;
        /// Ref: The find box control
        CAknSearchField* iFindBox;
        /// Ref: Thumbnail loader
        CPbk2ThumbnailLoader& iThumbnailLoader;
        /// Ref: Command items
        RPointerArray<MPbk2UiControlCmdItem>& iCommandItems;          
    };

#endif // CPBK2NAMESLISTEMPTYSTATE_H

// End of File
