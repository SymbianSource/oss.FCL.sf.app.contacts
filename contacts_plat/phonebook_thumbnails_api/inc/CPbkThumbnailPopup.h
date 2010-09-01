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
*  		Loads a thumbnail image attached to Phonebook's contact and displays it in a
*  		popup control on top of the context pane.
*
*/


#ifndef __CPbkThumbnailPopup_H__
#define __CPbkThumbnailPopup_H__

//  INCLUDES
#include <e32base.h> // CBase
#include "MPbkThumbnailOperationObservers.h"
#include <cntdef.h>  // TContactItemId
#include <eiklbx.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkContactItem;
class MPbkThumbnailPopupControl;
class CPbkThumbnailManager;
class CPbkThumbnailScaler;
class CPbkThumbnailScalerObserver;
class CPbkSwapContextPaneIcon;
/// Use for testing only!!!
class CPbkThumbnailPopup_TestAccess;


// CLASS DECLARATION

/**
 * Loads a thumbnail image attached to Phonebook's contact and displays it in a
 * popup control on top of the context pane.
 */
class CPbkThumbnailPopup :
		public CBase,
        private MPbkThumbnailGetObserver
    {
    public:  // Constructors and destructor
        /**
         * Constructor.
         * @param aEngine Phonebook engine.
         */
        IMPORT_C static CPbkThumbnailPopup* NewL(CPbkContactEngine& aEngine);

        /**
         * Destructor. Hides any thumbnail popup visible, cancels any Load() in
         * progress and destroys this object.
         */
        ~CPbkThumbnailPopup();
       
    private:
        /**
        * By default Symbian 2nd phase constructor is private.        
        */
        void ConstructL();

    public: // Interface
        /**
         * Loads contact item's thumbnail image (if any) and displays it on top
         * of the context pane in a popup control. If a previous thumbnail is
         * visible when this function is called it is immediately hidden. The
         * loading and displaying of the thumbnail image is done asynchronously,
         * so the thumbnail is not immediately visible when this function
         * returns.
         *
         * This function ignores all errors that might occur when loading and
         * displaying the thumbnail image. If there are errors the thumbnail is
         * simply not shown. No error note is displayed to the user.
         *
         * NOTE: Prefer this function instead of Load(TContactItemId) if you
         * already have the CPbkContactItem object.
         *
         * @param aContactItem  contact whose thumbnail to show.
         * @param aDelay        if true starts reading the contact thumbnail
         *                      after a small delay. Used by Phonebook's names
         *                      list view to keep focus movement smooth.
         * deprecated, aDelay not used anymore
         */
        IMPORT_C void Load(const CPbkContactItem& aContactItem, TBool aDelay=EFalse);

        /**
         * Like Load(const CPbkContactItem&,TBool) but reads the contact from
         * database.
         *
         * @param aContactId    database id of the contact whose thumbnail to
         *                      show.
         * deprecated, aDelay not used anymore
         * @see Load(const CPbkContactItem&,TBool)
         */
        IMPORT_C void Load(TContactItemId aContactId, TBool aDelay=EFalse);

        /**
         * Loads contact item's thumbnail image (if any) and displays it on 
         * listbox near focus if landscape mode is enabled. If a previous thumbnail is
         * visible when this function is called it is immediately hidden. The
         * loading and displaying of the thumbnail image is done asynchronously,
         * so the thumbnail is not immediately visible when this function
         * returns.
         *
         * This function ignores all errors that might occur when loading and
         * displaying the thumbnail image. If there are errors the thumbnail is
         * simply not shown. No error note is displayed to the user.
         *
         * NOTE: Prefer this function instead of Load(TContactItemId) if you
         * already have the CPbkContactItem object.
         *
         * @param aContactItem  contact whose thumbnail to show.
         * @param aListBox      ListBox where thumbnail is drawn
         */
        void Load(const CPbkContactItem& aContactItem, CEikListBox* aListBox );

        /**
         * Like Load(const CPbkContactItem&,CEikListBox*) but reads the contact from
         * database.
         *
         * @param aContactId    database id of the contact whose thumbnail to
         *                      show.
         * @param aListBox      ListBox where thumbnail is drawn
         * @see Load(const CPbkContactItem&,CEikListBox*)
         */
        void Load(TContactItemId aContactId, CEikListBox* aListBox);

        /**
         * Refresh thumbnail
         */
        void Refresh();

        /**
         * Hides the thumbnail popup control and cancels any Load() in
         * progress.
         */
        IMPORT_C void CancelLoading();

        /**
         * Returns the Id of the currently displayed contact.
         */
        TContactItemId ContactId() const;

        /**
         * Shows the given bitmap in popup control.
         * Note that the bitmap has to be scaled down before calling this
         * function.
         * @param aBitmap Bitmap to show in popup control.
         */
        void ShowPopupBitmap(CFbsBitmap* aBitmap);

    private: // from MPbkThumbnailGetObserver
        void PbkThumbnailGetComplete(MPbkThumbnailOperation& aOperation, CFbsBitmap* aBitmap);
        void PbkThumbnailGetFailed(MPbkThumbnailOperation& aOperation, TInt aError);

    private:  // Implementation
        CPbkThumbnailPopup(CPbkContactEngine& aEngine);        
        void DoLoadL(TContactItemId aContactId);
        void DoLoadL(const CPbkContactItem& aContactItem);        
        void ShowControl();
        void HideControl();
        void LoadContactL();
        void GetThumbnailL(const CPbkContactItem& aItem);
        void LoadThumbnailBackgL();
        
        __DECLARE_TEST;

        /// Use for testing only!!!
        friend class CPbkThumbnailPopup_TestAccess;

    private:    // Data
        ///Own: 
        CPbkSwapContextPaneIcon* iSwapContextIcon;
        /// Ref: phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: contact item
        const CPbkContactItem* iContactItem;
        /// Own: opened contact item
        CPbkContactItem* iLoadedItem;
        /// Own: id of contact to load to iLoadedItem
        TContactItemId iLoadContactId;        
        /// Own: thumbnail manager
        CPbkThumbnailManager* iManager;
        /// Own: Bitmap popup control
        MPbkThumbnailPopupControl* iThumbnailPopupControl;
        /// Own: asyncronous thumbnail operation
        MPbkThumbnailOperation* iThumbOperation;
        /// Own: Thumbnail scaler
        CPbkThumbnailScaler* iThumbnailScaler;
        /// Own: Scaler observer
        CPbkThumbnailScalerObserver* iScalerObserver;
        /// Own: Thumbnail background shadow
        CFbsBitmap* iThumbnailBackg;
        /// Own: Thumbnail background shadow mask
        CFbsBitmap* iThumbnailBackgMask;
        /// Ref: For passing this to slimcontrol so that thumbnail
        /// is drawn correctly in listbox
        CEikListBox* iListBox;
        /// Own: To check that is screen orientation changed
        TBool iLandscape;
    };

#endif // __CPbkThumbnailPopup_H__

// End of File
