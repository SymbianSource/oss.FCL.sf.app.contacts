/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Adapter between Pbk2 commands and command users.
*
*/


#ifndef TPBK2UICONTROLEVENTADAPTER_H
#define TPBK2UICONTROLEVENTADAPTER_H

// INCLUDES
#include <e32base.h>

#include <mpbk2contactuicontrol.h>
#include <mpbk2uicontroladapterobserver.h>
#include <mvpbkstorecontact.h>
#include <mvpbkbasecontact.h>

// FORWARD DECLARATIONS
class MVPbkStoreContact;
class MPbk2UiControlAdapterObserver;


// CLASS DECLARATIONS
/**
 * Phonebook 2 checks store state
 */
NONSHARABLE_CLASS( TPbk2UiControlEventAdapter ):  
        public MPbk2ContactUiControl
    {     
    public: // Interface

            
        void SetContact( MVPbkStoreContact& aStoreContact );

        void SetObserver( MPbk2UiControlAdapterObserver& aObserver );

    private: // From MPbk2ContactUiControl

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

        TInt CommandItemCount() const;

        const MPbk2UiControlCmdItem& CommandItemAt( TInt aIndex ) const;

        const MPbk2UiControlCmdItem* FocusedCommandItem() const;

        void DeleteCommandItemL( TInt aIndex );

        void AddCommandItemL(MPbk2UiControlCmdItem* aCommand, TInt aIndex);

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

    private: // Data
        MVPbkStoreContact* iStoreContact; // Not owned
        MPbk2UiControlAdapterObserver* iObserver; // Not owned
    };

// INLINE IMPLEMENTATION
#include "tpbk2uicontroleventadapter.inl"

#endif // TPBK2UICONTROLEVENTADAPTER_H

// End of File
