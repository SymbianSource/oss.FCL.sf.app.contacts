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
* Description:  Phonebook 2 thumbnail popup.
*
*/


#ifndef CPBK2THUMBNAILPOPUP_H
#define CPBK2THUMBNAILPOPUP_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ImageOperationObservers.h>
#include <eiklbx.h>

// FORWARD DECLARATIONS
class MPbk2ThumbnailPopupControl;
class CPbk2ImageManager;
class CPbk2ThumbnailScaler;
class CPbk2ThumbnailScalerObserver;
class CVPbkContactManager;
class MVPbkBaseContact;
class MVPbkFieldType;
class TPbk2ImageManagerParams;

// CLASS DECLARATION

/**
 * Phonebook 2 thumbnail location.
 */
enum TPbk2ThumbnailLocation
    {
    // Thumbnail is placed either upper or lower corner of
    // the listbox control
    ELocationUpDown = 0,
    // Thumbnail hangs either under or above the focus
    ELocationHanging
    };

/**
 * Phonebook 2 thumbnail popup observer.
 */
class MPbk2ThumbnailPopupObserver
    {
    public: // Interface

        /**
         * Called when loading of the popup thumbnail has completed
         * succesfully.
         */
        virtual void PopupLoadComplete() = 0;

        /**
         * Called when loading of the popup thumbnail failed.
         *
         * @param aError    Error code.
         */
        virtual void PopupLoadFailed(
                TInt aError ) = 0;

    protected: // Disabled functions
        ~MPbk2ThumbnailPopupObserver()
            {}
    };

/**
 * Phonebook 2 thumbnail popup.
 *
 * Loads a thumbnail image attached to Phonebook's contact
 * and displays it in a popup control.
 */
class CPbk2ThumbnailPopup : public CBase,
                            private MPbk2ImageGetObserver
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContactManager   Virtual Phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2ThumbnailPopup* NewL(
                CVPbkContactManager& aContactManager );

        /**
         * Destructor.
         * Hides any thumbnail popup visible, cancels any Load() in
         * progress and destroys this object.
         */
        ~CPbk2ThumbnailPopup();

    public: // Interface

        /**
         * Loads contact's thumbnail image (if any) and displays it
         * in a popup control. If a previous thumbnail is visible
         * when this function is called it is immediately hidden.
         * The loading and displaying of the thumbnail image is done
         * asynchronously, so the thumbnail is not immediately visible
         * when this function returns.
         *
         * This function ignores all errors that might occur when loading
         * and displaying the thumbnail image. If there are errors the
         * thumbnail is simply not shown. No error note is displayed
         * to the user.
         *
         * @param aContact      Contact whose thumbnail to show.
         * @param aObserver     Observer of the thumbnail popup.
         * @param aListBox      If given, thumbnail position is depended on
         *                      list box focus. If NULL, thumbnail position
         *                      is fixed.
         * @param aFieldType    Type of the field from where to load
         *                      the thumbnail, if NULL the default thumbnail
         *                      field is used.
         */
        void Load(
                MVPbkBaseContact& aContact,
                MPbk2ThumbnailPopupObserver& aObserver,
                const CEikListBox* aListBox = NULL,
                const MVPbkFieldType* aFieldType = NULL );

        /**
         * Hides the thumbnail popup control and cancels any Load() in
         * progress.
         */
        void HideAndCancel();

        /**
         * Shows the given bitmap in a popup control.
         * Note that the bitmap has to be scaled down before calling this
         * function.
         *
         * @param aBitmap   Bitmap to show in a popup control.
         */
        void ShowPopupBitmap(
                CFbsBitmap* aBitmap );

        /**
         * Refreshes an already loaded thumbnail.
         */
        void Refresh();

        /**
         * Sets the location where thumbnail is drawn
         *
         * @param aThumbnailLocation    Thumbnail location.
         */
        void SetThumbnailLocation(
                TPbk2ThumbnailLocation aThumbnailLocation );

    private: // From MPbk2ImageGetObserver
        void Pbk2ImageGetComplete(
                MPbk2ImageOperation& aOperation,
                CFbsBitmap* aBitmap );
        void Pbk2ImageGetFailed(
                MPbk2ImageOperation& aOperation,
                TInt aError );

    private: // Implementation
        CPbk2ThumbnailPopup(
                CVPbkContactManager& aContactManager );
        void ConstructL();
        void DoLoadL(
                MVPbkBaseContact& aContact,
                const MVPbkFieldType* aFieldType );
        void HideControl();
        void GetThumbnailL(
                MVPbkBaseContact& aContact,
                const MVPbkFieldType& aFieldType );
        void ReadFieldTypeL();
        void LoadThumbnailBackgL();
        void SetupImageParams(
                TPbk2ImageManagerParams& aParams );

    private: // Data
        /// Own: Thumbnail manager
        CPbk2ImageManager* iManager;
        /// Own: Bitmap popup control
        MPbk2ThumbnailPopupControl* iThumbnailPopupControl;
        /// Own: Asynchronous thumbnail operation
        MPbk2ImageOperation* iThumbOperation;
        /// Own: Thumbnail scaler
        CPbk2ThumbnailScaler* iThumbnailScaler;
        /// Own: Scaler observer
        CPbk2ThumbnailScalerObserver* iScalerObserver;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iContactManager;
        /// Ref: Observer for the load function
        MPbk2ThumbnailPopupObserver* iLoadObserver;
        /// Ref: Field type
        const MVPbkFieldType* iFieldType;

        /// Not Own: 
        CFbsBitmap* iBitmap;

        
        /// Own: Thumbnail background shadow
        CFbsBitmap* iThumbnailBackg;
        /// Own: Thumbnail background shadow mask
        CFbsBitmap* iThumbnailBackgMask;
        /// Ref: Listbox for finding out where the focus is
        const CEikListBox* iListBox;
        /// Own: Thumbnail location
        TPbk2ThumbnailLocation iThumbnailLocation;
    };

#endif // CPBK2THUMBNAILPOPUP_H

// End of File
