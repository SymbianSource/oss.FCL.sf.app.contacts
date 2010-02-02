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
* Description:  Phonebook 2 names list not ready state.
*
*/


#ifndef CPBK2NAMESLISTNOTREADYSTATE_H
#define CPBK2NAMESLISTNOTREADYSTATE_H

//  INCLUDES
#include <coecntrl.h>
#include "MPbk2NamesListState.h"

// FORWARD DECLARATIONS
class CPbk2ContactViewListBox;
class CAknSearchField;

// CLASS DECLARATIONß

/**
 * Phonebook 2 names list not ready state.
 */
NONSHARABLE_CLASS(CPbk2NamesListNotReadyState) :
        public CBase,
        public MPbk2NamesListState
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aParent   Parent control.
         * @param aListBox  Contact view listbox.
         * @param aFindBox  Contact view findbox.
         * @return  A new instance of this class.
         */
        static CPbk2NamesListNotReadyState* NewLC(
                const CCoeControl* aParent,
                CPbk2ContactViewListBox& aListBox,
                CAknSearchField* aFindBox );

        /**
         * Destructor.
         */
        ~CPbk2NamesListNotReadyState();

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

    private: // Implementation
        CPbk2NamesListNotReadyState(
                const CCoeControl& aParent,
                CPbk2ContactViewListBox& aListBox,
                CAknSearchField* aFindBox );

    private: // Data
        /// Ref: Parent coecontrol
        const CCoeControl& iParent;
        /// Ref: The list box
        CPbk2ContactViewListBox& iListBox;
        /// Ref: The find box
        CAknSearchField* iFindBox;
    };

#endif // CPBK2NAMESLISTNOTREADYSTATE_H

// End of File
