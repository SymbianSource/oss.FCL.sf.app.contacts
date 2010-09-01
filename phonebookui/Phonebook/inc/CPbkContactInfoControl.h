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
*     UI control for Phonebook's "Contact Info View".
*
*/


#ifndef __CPbkContactInfoControl_H__
#define __CPbkContactInfoControl_H__


//  INCLUDES
#include "CPbkContactDetailsBaseControl.h"
#include "PbkIconId.hrh"    // TPbkIconId
#include "MPbkClipListBoxText.h"
#include "MPbkFieldAnalyzer.h"

//  FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkContactEngine;
class TPbkContactItemField;
class CPbkFieldArray;
class CPbkViewState;
class CPbkThumbnailPopup;
class CEikListBox;
class MDesC16Array;
class CContactIdArray;

//  CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook "Contact Info View" UI control. 
 * Presents the fields of a contact as a list.
 */
class   CPbkContactInfoControl :
        public CPbkContactDetailsBaseControl,
        public MPbkClipListBoxText, 
        public MPbkFieldAnalyzer
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aResID        Id of a PBK_CONTACTINFO_CONTROL resource.
         * @param aParent       parent of this control.
         * @param aContact      Contact to display. This object does NOT take
         *                      ownership of aContact.
         * @param aEngine       Contact engine needed for MPbkFieldAnalyzer
         *                      functions. This object does NOT take
         *                      ownership of aContact.
         */
        IMPORT_C static CPbkContactInfoControl* NewL(
                TInt aResId,
                const CCoeControl& aParent, 
                CPbkContactItem* aContact,
                CPbkContactEngine* aEngine);

        /**
         * Destructor.
         */
        ~CPbkContactInfoControl();

    public:  // interface
        /** 
         * Updates this control to display aContact's details.
         * NOTE: This function does not redraw the control on screen.
         * NOTE2: this function is leave-safe; it quarantees that this 
         * control's state does not change if this function leaves.
         *
         * @param aContact  Contact to display.  This object does NOT take
         *                  ownership of aContact.
         */
        void UpdateL(CPbkContactItem* aContact);

        /**
         * Returns the contact item. 
         */
        CPbkContactItem& ContactItem();

        /**
         * Returns the currently selected field. 
         *
         * @return  currently selected field or NULL if no selection.
         */
        TPbkContactItemField* CurrentField() const;

        /**
         * Returns the currently selected field's index relative to all
         * fields viewed in the contact.
         *
         * @return  currently selected field's index or -1 if no selection.
         */
        IMPORT_C TInt CurrentFieldIndex() const;

        /**
         * Returns aField's position in the list, -1 if not found.
         */
        IMPORT_C TInt FieldPos(const TPbkContactItemField& aField);

		/**
		 * Return the field count of this control
		 */
		IMPORT_C TInt FieldCount() const;

        /**
         * Returns this view's current state.
         *
         * @return  this view's state. Caller is responsible of deleting 
         *          the returned object. 
         */
        IMPORT_C CPbkViewState* GetStateL() const;

        /**
         * Sames as GetStateL() above, but leaves the returned object on
         * the cleanup stack.
         */
        IMPORT_C CPbkViewState* GetStateLC() const;

        /**
         * Restores this view's state to aState. 
         * Note: doesn't care about aState->FocusedContactItem(), only restores
         * top field and focused field.
         */
        IMPORT_C void RestoreStateL(const CPbkViewState* aState);

        /**
         * Returns the listbox.
         */
        IMPORT_C CEikListBox& ListBox();

        /**
         * Hides the thumbnail window control is showing. 
         * Call ShowThumbnailL() or UpdateL() to restore the thumbnail.
         */
        void HideThumbnail();

        /**
         * Shows the thumbnail image.
         */
        void ShowThumbnailL();

    public: // from MPbkContactUiControl 
        TInt NumberOfItems() const;
        TBool ItemsMarked() const;
        const CContactIdArray& MarkedItemsL() const;
        TContactItemId FocusedContactIdL() const;
        const TPbkContactItemField* FocusedField() const;
        MObjectProvider* ObjectProvider();
        TInt FocusedFieldIndex() const;

    public:  // from MPbkClipListBoxText
        TBool ClipFromBeginning(TDes& aBuffer, TInt aItemIndex, TInt aSubCellNumber);

    public:  // from MPbkFieldAnalyzer
        TBool HasSpeedDialL(const TPbkContactItemField& aField) const;
        TBool HasVoiceTagL(const TPbkContactItemField& aField) const;

    private:  // from CCoeControl: standard compound control overrides
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
        void SizeChanged();
    	TInt CountComponentControls() const;
	    CCoeControl* ComponentControl(TInt aIndex) const;
        void HandleResourceChange(TInt aType);
        void DoHandleResourceChangeL(TInt aType);
        void FocusChanged( TDrawNow aDrawNow );

    private:  // Implementation
        CPbkContactInfoControl();

        void CheckRingToneFieldL( CPbkContactItem* aContact );

        /**
         * Second phase constructor, from CCoeControl. Initialises this object
         * from a PBK_CONTACTINFO_CONTROL resource.
         *
         * @param aReader   resource reader at the beginning of a 
         *                  PBK_CONTACTINFO_CONTROL resource.
         */
	    void ConstructFromResourceL(TResourceReader& aReader);
        
        class CItemDrawer;
        class CListBox;
        friend class CListBox;
        class CUpdateData;
        class CUpdateTestData;
        friend class CUpdateTestData;
        
    private:  // data
        /// Own: the list box control
        CListBox* iListBox;
        /// Own: flags
        TUint iFlags;
        /// Own: list box model
        MDesC16Array* iListBoxModel;
        /// Own: array of fields.
        CPbkFieldArray* iFields;
        /// Ref: Contact item to display
        CPbkContactItem* iContactItem;
        /// Own: Thumbnail handling
        CPbkThumbnailPopup* iThumbnailHandler;
        /// Own: default icon index
        TPbkIconId iDefaultIconId;
        /// Ref: Contact engine
        CPbkContactEngine* iContactEngine;
        /// Own: marked items array returned from MarkedItemsL()
        mutable CContactIdArray* iMarkedItemsArray;
        /// Own: is thumbnail visible
        TBool iShowThumbnail;
        ///Ref: voice tagged field;
        mutable TPbkContactItemField* iVoiceTagField;
        ///Own; voicetag fetch completed
        mutable TBool iVoiceTagFetchCompleted;
    };

#endif  // __CPbkContactInfoControl_H__

// End of File
