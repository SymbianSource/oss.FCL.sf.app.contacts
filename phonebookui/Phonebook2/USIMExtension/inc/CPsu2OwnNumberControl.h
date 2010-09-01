/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 own numbers control.
*
*/


#ifndef CPSU2OWNNUMBERCONTROL_H
#define CPSU2OWNNUMBERCONTROL_H

// INCLUDES
#include <coecntrl.h>
#include <MVPbkContactViewObserver.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>

// FORWARD DECLARATIONS
class CPsu2OwnNumberListBox;
class MVPbkContactViewBase;
class MPbk2ContactNameFormatter;
class CAknDoubleGraphicStyleListBox;
class CCoeControl;

// CLASS DECLARATION

/**
 *  Phonebook 2 own numbers control.
 */
class CPsu2OwnNumberControl : public CCoeControl,
                              public MPbk2ContactUiControl,
                              public MPbk2ContactUiControl2
    {
    public:  // Constructors and destructor
        
        /**
         * Creates a new instance of this class.
         *
         * @param aView         The view from the extension framework.
         * @param aContainer    Container that owns the window.
         * @return  A new instance of this class.
         */
        static CPsu2OwnNumberControl* NewL(
                MVPbkContactViewBase& aView,
                CCoeControl* aContainer );
        
        /**
         * Destructor.
         */
        ~CPsu2OwnNumberControl();
        
    public: // From CCoeControl
        TKeyResponse OfferKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
        void SizeChanged();
    	TInt CountComponentControls() const;
	    CCoeControl* ComponentControl(
                TInt aIndex ) const;
        void FocusChanged(
                TDrawNow aDrawNow );
                
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
        void ProcessCommandL (
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
        CPsu2OwnNumberControl( 
                MVPbkContactViewBase& aView );
        void ConstructL(
                MVPbkContactViewBase& aView, 
                CCoeControl* aContainer );

    private: // Data
        /// Own: Listbox
        CAknDoubleGraphicStyleListBox* iListBox;
        /// Ref: All contacts view
        MVPbkContactViewBase* iBaseView;
    };

#endif // CPSU2OWNNUMBERCONTROL_H 
            
// End of File
