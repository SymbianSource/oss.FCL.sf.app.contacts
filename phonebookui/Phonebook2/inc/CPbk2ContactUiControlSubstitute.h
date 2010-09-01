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
* Description:  Phonebook 2 UI control substitute.
*
*/


#ifndef CPBK2CONTACTUICONTROLSUBSTITUTE_H
#define CPBK2CONTACTUICONTROLSUBSTITUTE_H

//  INCLUDES
#include <e32std.h>
#include <coecntrl.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>

// FORWARD DECLARATIONS
class MVPbkContactStore;
class MPbk2ContactUiControl;
class MVPbkBaseContactField;
class MVPbkStoreContact;
class CAknsBasicBackgroundControlContext;

// CLASS DECLARATION

/**
 * Phonebook 2 UI control substitute.
 * This is not a real UI control.
 */
class CPbk2ContactUiControlSubstitute :
        public CCoeControl,
        public MPbk2ContactUiControl,
        public MPbk2ContactUiControl2
    {
    public: // Constructors and destructor
        /**
         * Constructor.
         *
         * @return New instance of this class
         */
        IMPORT_C static CPbk2ContactUiControlSubstitute* NewL();

        /**
         * Destructor.
         */
        IMPORT_C ~CPbk2ContactUiControlSubstitute();

    public: // Interface

        /**
         * Sets the parent UI control returned by the ParentControl()
         * of the UI control interface.
         *
         * @param aParentUiControl  Parent UI control.
         */
        IMPORT_C void SetParentUiControl(
                MPbk2ContactUiControl& aParentUiControl );

        /**
         * Sets the selected store. The store is returned
         * from SelectedContactStoresL() of the UI control interface.
         *
         * @param aContactStore     Selected store.
         */
        IMPORT_C void SetSelectedStore(
                MVPbkContactStore& aContactStore );

        /**
         * Sets the focused store contact. Returned by
         * FocusedStoreContact() of the UI control interface.
         *
         * @param aContact      Focused store contact.
         */
        IMPORT_C void SetFocusedStoreContact(
                const MVPbkStoreContact& aContact );

        /**
         * Sets the focused contact field. Returned by
         * FocusedField() of the UI control interface.
         *
         * @param aContactField     Focused contact field.
         */
        IMPORT_C void SetFocusedField(
                const MVPbkBaseContactField& aContactField );

        /**
         * Sets the focused contact field index. Returned by
         * FocusedFieldIndex() of the UI control interface.
         *
         * @param aFieldIndex   Focused field index.
         */
        IMPORT_C void SetFocusedFieldIndex(
                TInt aFieldIndex );

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
        void AddCommandItemL(MPbk2UiControlCmdItem* aCommand, TInt aIndex);

    private: // Implementation
        CPbk2ContactUiControlSubstitute();
        void ConstructL();
        void Draw(
                const TRect& aRect ) const;

    private: // Data
        /// Ref: The selected contact store
        MVPbkContactStore* iContactStore;
        /// Ref: The parent control
        MPbk2ContactUiControl* iParentUiControl;
        /// Ref: The store contact
        const MVPbkStoreContact* iStoreContact;
        /// Own: The focused field index
        TInt iFocusedFieldIndex;
        /// Ref: The focused field
        const MVPbkBaseContactField* iContactField;
        /// Own: Background skin context
        CAknsBasicBackgroundControlContext* iBgSkinContext;
    };

#endif // CPBK2CONTACTUICONTROLSUBSTITUTE_H

// End of File
