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
* Description:  Phonebook 2 contact UI control interface.
*
*/


#ifndef MPBK2CONTACTUICONTROL_H
#define MPBK2CONTACTUICONTROL_H

// INCLUDES
#include <e32std.h>

// FORWARD DECLATIONS
class CPbk2ViewState;
class MPbk2ContactLinkIterator;
class MPbk2Command;
class MVPbkBaseContact;
class MVPbkViewContact;
class MVPbkStoreContact;
class MVPbkBaseContactField;
class MVPbkContactBookmark;
class MVPbkContactLink;
class MVPbkContactLinkArray;
class MVPbkContactStore;
class CEikMenuPane;
class MPbk2UiControlCmdItem;
template <class MVPbkContactStore> class CArrayPtr;

//Use this UID to access contact store extension 2. 
//Used as a parameter to ContactUiControlExtension() method.
const TUid KMPbk2ContactUiControlExtension2Uid = { 2 };


// CLASS DECLARATION

/**
 * Phonebook 2 contact UI control interface.
 * The contact UI control can display three types of items:
 * A. Contacts
 * B. Contact fields
 * C. Command placeholders (the user can launch the command by clicking or
 *      opening the command list item. The command placeholders always appear
 *      at the top of the list.
 * At a certain moment the UI control can show both A and C in the list.
 */
class MPbk2ContactUiControl
    {
    public: // Interface

        /**
         * Destructor.
         */
        virtual ~MPbk2ContactUiControl()
            {}

        /**
         * Returns the parent control of this control or NULL
         * if this is the control at the bottom.
         * Ownership is not given.
         *
         * @return  Parent control or NULL.
         */
        virtual MPbk2ContactUiControl* ParentControl() const = 0;

        /**
         * Returns the number of contacts in the UI control.
         *
         * @return  Number of contacts.
         */
        virtual TInt NumberOfContacts() const = 0;

        /**
         * Returns the currently focused contact.
         *
         * @return  Currently focused contact.
         */
        virtual const MVPbkBaseContact* FocusedContactL() const = 0;

        /**
         * Returns the currently focused contact as a view contact.
         * If the UI control does not have an access to view contacts
         * returns NULL.
         *
         * @return  Currently focused contact as a view contact or NULL.
         */
        virtual const MVPbkViewContact* FocusedViewContactL() const = 0;

        /**
         * Returns the currently focused contact as a store contact.
         * If the UI control does not have an access to store contacts
         * returns NULL.
         *
         * @return  Currently focused contact as a store contact or NULL.
         */
        virtual const MVPbkStoreContact* FocusedStoreContact() const = 0;

        /**
         * Sets the focused contact.
         * Doesn't do anything if the contact does not exist in the control.
         *
         * @param aContact  Contact to focus.
         */
        virtual void SetFocusedContactL(
                const MVPbkBaseContact& aContact ) = 0;
        
        /**
         * Sets the focused contact.
         * Doesn't do anything if the contact does not exist in the control.
         *
         * @param aContactBookmark  Contact to focus.
         */
        virtual void SetFocusedContactL(
                const MVPbkContactBookmark& aContactBookmark ) = 0;
        
        /**
         * Sets the focused contact.
         * Doesn't do anything if the contact does not exist in the control.
         *
         * @param aContactLink  Contact to focus.
         */
        virtual void SetFocusedContactL(
                const MVPbkContactLink& aContactLink ) = 0;

        /**
         * Returns the currently focused contact index or KErrNotFound if
         * this control does not support contact level focus.
         *
         * @return  Currently focused contact index.
         */
        virtual TInt FocusedContactIndex() const = 0;

        /**
         * Sets focused contact index, if negative the top
         * contact is focused.
         *
         * @param aIndex    The index of the contact to focus.
         */
        virtual void SetFocusedContactIndexL(
                TInt aIndex ) = 0;

        /**
         * Returns the number of contact fields in the UI control.
         *
         * @return  Number of contact fields or KErrNotSupported 
         *          if fields are not supported
         */
        virtual TInt NumberOfContactFields() const = 0;

        /**
         * Returns the currently focused contact field or NULL if
         * this control does not support field level focus.
         *
         * @return  Currently focused contact field.
         */
        virtual const MVPbkBaseContactField* FocusedField() const = 0;

        /**
         * Returns the currently focused contact field index or
         * KErrNotFound if this control does not support field level focus.
         *
         * @return  Currently focused contact field index.
         */
        virtual TInt FocusedFieldIndex() const = 0;

        /**
         * Sets focused field index, if negative the top field is focused.
         *
         * @param aIndex    The index of the field to focus.
         */
        virtual void SetFocusedFieldIndex(
                TInt aIndex ) = 0;

        /**
         * Returns ETrue if there are marked contacts.
         *
         * @return  ETrue if there are marked contacts, EFalse otherwise.
         */
        virtual TBool ContactsMarked() const = 0;

        /**
         * Returns an array of currently selected contacts.
         * If there are no selected contacts, returns NULL.
         * NOTE! When having lots of contacts marked, this function
         *       might cause OOM situation. Use SelectedContactsIteratorL()
         *       instead.
         *
         * @return  Selected contacts in a link array.
         *          Caller takes the ownership.
         */
        virtual MVPbkContactLinkArray* SelectedContactsL() const = 0;

        /**
         * Returns an array of currently selected contacts.
         * If there are no selected contacts, returns the focused contact.
         * If there are no contacts, returns NULL.
         * NOTE! When having lots of contacts marked, this function
         *       might cause OOM situation. Use SelectedContactsIteratorL()
         *       instead.
         *
         * @return  Selected contacts, or focused contact, in a link array.
         *          Caller takes the ownership.
         */
        virtual MVPbkContactLinkArray* SelectedContactsOrFocusedContactL() const = 0;

        /**
         * Returns an iterator for currently selected contacts.
         * Client gets the ownership of the iterator.
         * If there are no selected contacts, returns NULL.
         *
         * @return  Iterator for selected contact links.
         */
        virtual MPbk2ContactLinkIterator* SelectedContactsIteratorL() const = 0;

        /**
         * Returns an array of selected contact stores if the control is
         * a store list control. Other controls return NULL.
         *
         * @return  Array of stores or NULL. Client gets the ownership
         *          of the array but does not own the stores inside
         *          the array.
         */
        virtual CArrayPtr<MVPbkContactStore>* SelectedContactStoresL() const = 0;

        /**
         * Clears all marked contacts.
         */
        virtual void ClearMarks() = 0;

        /**
         * Selects or deselects a contact in the control.
         * If the control doesn't support this functionality then it
         * doesn't do anything.
         *
         * @param aIndex        Index of the contact to set
         *                      the selection status.
         * @param aSelected     If ETrue the contact at the given index
         *                      is set to selected, otherwise the contact
         *                      will be deselected.
         */
        virtual void SetSelectedContactL(
                TInt aIndex,
                TBool aSelected ) = 0;

        /**
         * Selects or deselects a contact in the control.
         * If the control doesn't support this functionality then it
         * doesn't do anything.
         *
         * @param aContactBookmark  Identifies the selected contact.
         *                          If contact is not found from the control
         *                          then nothing is done.
         * @param aSelected         If ETrue the contact at the given index
         *                          is set to selected, otherwise the contact
         *                          will be deselected.
         */
        virtual void SetSelectedContactL(
                const MVPbkContactBookmark& aContactBookmark,
                TBool aSelected ) = 0;
        
        /**
         * Selects or deselects a contact in the control.
         * If the control doesn't support this functionality then it
         * doesn't do anything.
         *
         * @param aContactLink  Identifies the selected contact.
         *                      If contact is not found from the control
         *                      then nothing is done.
         * @param aSelected     If ETrue the contact at the given index
         *                      is set to selected, otherwise the contact
         *                      will be deselected.
         */
        virtual void SetSelectedContactL(
                const MVPbkContactLink& aContactLink,
                TBool aSelected ) = 0;
        
        /**
         * Updates the control specific menu items. Call from
         * control owner's DynInitMenuPaneL.
         *
         * @param aResourceId   The resource id of the menu pane.
         * @param aMenuPane     The menu pane.
         * @see AknSelectionService::HandleMarkableListDynInitMenuPane
         */
        virtual void DynInitMenuPaneL(
                TInt aResourceId,
                CEikMenuPane* aMenuPane ) const = 0;

        /**
         * Offers the command to the control.
         *
         * @param aCommandId    The id of the selected command.
         * @see AknSelectionService::HandleMarkableListProcessCommandL
         */
        virtual void ProcessCommandL(
                TInt aCommandId ) const = 0;

        /**
         * Updates control to the state it should be in after a command
         * execution.
         *
         * @see AknSelectionService::HandleMarkableListUpdateAfterCommandExecution
         */
        virtual void UpdateAfterCommandExecution() = 0;

        /**
         * Gets control's menu filtering flags.
         *
         * @return  Menu filtering flags.
         */
        virtual TInt GetMenuFilteringFlagsL() const = 0;

        /**
         * Gets the control state.
         *
         * @return  Control state.
         */
        virtual CPbk2ViewState* ControlStateL() const = 0;

        /**
         * Restores the control state to given state, except the
         * focused contact.
         *
         * @param aState    The state to restore.
         */
        virtual void RestoreControlStateL(
                CPbk2ViewState* aState ) = 0;

        /**
         * Gets the text in the find box. Returns KNullDesC if not
         * applicable or there is no text in the box.
         *
         * @return  Find box text or KNullDesC.
         */
        virtual const TDesC& FindTextL() = 0;

        /**
         * Resets any strings in the find box.
         */
        virtual void ResetFindL() = 0;

        /**
         * Hides the thumbnail image.
         */
        virtual void HideThumbnail() = 0;

        /**
         * Shows the thumbnail image.
         */
        virtual void ShowThumbnail() = 0;

        /**
         * Blanks/unblanks the control. Use when
         * there are massive updates to the databases to avoid flickering.
         *
         * @param aBlank    ETrue blanks the control, EFalse unblanks it.
         */
        virtual void SetBlank(
                TBool aBlank ) = 0;

        /**
         * Registers a command to the UI control. Does not pass ownership.
         *
         * @param aCommand  Command that is registered to the UI control.
         */
        virtual void RegisterCommand(
                MPbk2Command* aCommand ) = 0;

        /**
         * Sets a text for a control.
         *
         * @param aText     New text for the control
         */
        virtual void SetTextL(
                const TDesC& aText ) = 0;

        /**
         * Returns an extension point for this interface or NULL.
         *
         * @param aExtensionUid     Extension UID.
         * @return  Extension point.
         */
        virtual TAny* ContactUiControlExtension(
                TUid /*aExtensionUid*/ )
            {
            return NULL;
            }
    };

#endif // MPBK2CONTACTUICONTROL_H

// End of File
