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
* Description:  Phonebook 2 contact info view control.
*
*/


#ifndef CPBK2CONTACTINFOCONTROL_H
#define CPBK2CONTACTINFOCONTROL_H

// INCLUDES
#include <coecntrl.h>
#include <coecobs.h>
#include "MPbk2ContactDetailsControl.h"
#include <Pbk2IconId.hrh>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2PointerEventInspector.h>
#include <eiksbobs.h>
#include <eiklbo.h>


// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MVPbkStoreContactField;
class CVPbkContactManager;
class MVPbkBaseContactField;
class CPbk2ContactInfoListBox;
class MPbk2ContactNameFormatter;
class CAknTitlePane;
class MPbk2ControlObserver;
class TPbk2ControlEvent;
class CPbk2ContactInfoDataSwapper;
class CPbk2PresentationContact;
class CPbk2PresentationContactFieldCollection;
class MPbk2FieldPropertyArray;
class MVPbkContactOperationBase;
class MVPbkContactLink;
class CPbk2ThumbnailLoader;
class MPbk2ContactNavigation;
class CPbk2StorePropertyArray;
class CPbk2StoreSpecificFieldPropertyArray;
class CPbk2ContactFieldDynamicProperties;
class MPbk2Command;


// CLASS DECLARATION

/**
 * Phonebook 2 contact info view control.
 * Presents the fields of a contact as a list.
 */
class CPbk2ContactInfoControl :
        public CCoeControl,
        public MPbk2ContactDetailsControl,
        public MCoeControlObserver,
        public MVPbkSingleContactOperationObserver,
        public MPbk2PointerEventInspector,
        public MEikScrollBarObserver,
        public MEikListBoxObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContainer        This control's container.
         * @param aManager          Virtual Phonebook contact manager.
         * @param aNameFormatter    Name formatter.
         * @param aFieldProperties  Field properties.
         * @param aStoreProperties  Store properties.
         * @param aNavigation       Optional contact navigator.
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2ContactInfoControl* NewL(
                const CCoeControl* aContainer,
                CVPbkContactManager& aManager,
                MPbk2ContactNameFormatter& aNameFormatter,
                MPbk2FieldPropertyArray& aFieldProperties,
                CPbk2StorePropertyArray& aStoreProperties,
                MPbk2ContactNavigation* aNavigation );

        /**
         * Destructor.
         */
        ~CPbk2ContactInfoControl();

    public: // Interface
        /**
         * Adds an observer to this control.
         *
         * @param aObserver The observer to add.
         */
        IMPORT_C void AddObserverL(
                MPbk2ControlObserver& aObserver );

        /**
         * Removes an observer from this control.
         *
         * @param aObserver The observer to remove.
         */
        IMPORT_C void RemoveObserver(
                MPbk2ControlObserver& aObserver );

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
        void ShowThumbnail();
        void HideThumbnail();
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
        void AddCommandItemL(MPbk2UiControlCmdItem* /*aCommand*/, TInt /*aIndex*/);
        
    
    public: // From MPbk2ContactDetailsControl
        void UpdateL(
                MVPbkStoreContact* aContact );
        TInt FieldCount() const;
        TInt FieldPos(
                const MVPbkStoreContactField& aField ) const;

    private: // From CCoeControl
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(
                TInt aIndex ) const;
        void FocusChanged(
                TDrawNow aDrawNow );
        void HandleResourceChange(
                TInt aType );
        void HandlePointerEventL( const TPointerEvent& aPointerEvent );

    private: // From MCoeControlObserver
        void HandleControlEventL(
                CCoeControl* aControl,
                TCoeEvent aEventType );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    public: // MPbk2PointerEventInspector
        TBool FocusedItemPointed();
        TBool FocusableItemPointed();
        TBool SearchFieldPointed();

    public: // from MEikScrollBarObserver
         void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

    private: // from MEikListboxObserver
        void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );

    private: // Implementation
        CPbk2ContactInfoControl(
                CVPbkContactManager& aManager,
                MPbk2ContactNameFormatter& aNameFormatter,
                MPbk2FieldPropertyArray& aFieldProperties,
                CPbk2StorePropertyArray& aStoreProperties,
                MPbk2ContactNavigation* aNavigator );
        void ConstructL( const CCoeControl* aContainer );
        void StorePanesL(
                CPbk2ContactInfoDataSwapper& aSwapper,
                const CPbk2PresentationContact* aContact );
        void SendEventToObserversL(
                const TPbk2ControlEvent& aEvent );
        CPbk2ContactFieldDynamicProperties* CreateDynamicFieldPropertiesLC(
                CPbk2PresentationContact& aContact );
        void DoHandleResourceChangeL(
                TInt aType );
        void RetrieveContactL( const MVPbkContactLink& aContactLink );

    private: // Data
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Name formatter
        MPbk2ContactNameFormatter& iNameFormatter;
        /// Ref: Contact navigator
        MPbk2ContactNavigation* iNavigator;
        /// Ref: Field property array
        MPbk2FieldPropertyArray& iFieldProperties;
        /// Ref: Store properties for store specific field properties
        CPbk2StorePropertyArray& iStoreProperties;
        /// Own: List box
        CPbk2ContactInfoListBox* iListBox;
        /// Own: Flags
        TUint iFlags;
        /// Ref: Array of fields
        CPbk2PresentationContactFieldCollection* iFields;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Presentation contact to display
        CPbk2PresentationContact* iContact;
        /// Own: Observers of this object
        RPointerArray<MPbk2ControlObserver> iObservers;
        /// Own: Contact retrieve process
        MVPbkContactOperationBase* iRetriever;
        /// Own: Contact link, needed in contact retrieve process
        MVPbkContactLink* iContactLink;
        /// Own: Thumbnail handling
        CPbk2ThumbnailLoader* iThumbnailLoader;
        /// Own: Store specific field properties
        CPbk2StoreSpecificFieldPropertyArray* iSpecificFieldProperties;
        /// Own: Focused field index
        TInt iFocusedFieldIndex;
        /// Ref: command in execution
        MPbk2Command* iCommand;
        /// Ref: Title pane
        CAknTitlePane* iTitlePane;
        /// flag whether pointer moved or not for the thumbnail
        TBool iDidMove;
        
    };

#endif // CPBK2CONTACTINFOCONTROL_H

// End of File
