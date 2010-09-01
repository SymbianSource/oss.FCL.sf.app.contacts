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
* Description:  Phonebook 2 contact UI control proxy.
*
*/


#ifndef CPBK2CONTACTUICONTROLPROXY_H
#define CPBK2CONTACTUICONTROLPROXY_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ContactUiControl.h>
#include <MPbk2ContactUiControl2.h>

// CLASS DECLARATION

/**
 * Works as a "proxy" between real contact ui control and commands.
 */
class CPbk2ContactUiControlProxy : 	public CBase, 
									public MPbk2ContactUiControl,
									public MPbk2ContactUiControl2
	{
    public: // Construction and destruction
    
        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
    	CPbk2ContactUiControlProxy();
    	
        /**
         * Destructor.
         */
    	~CPbk2ContactUiControlProxy();
    	
	
    public: // Interace
    
        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
		void SetControl( MPbk2ContactUiControl* aControl );
	
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

    private: // Data
    
        /// Ref: Real contact ui control object.
		MPbk2ContactUiControl* iControl;
        /// Ref: Command in Execution
        MPbk2Command* iCommand;

	};


#endif // CPBK2CONTACTUICONTROLPROXY_H

// End of File
