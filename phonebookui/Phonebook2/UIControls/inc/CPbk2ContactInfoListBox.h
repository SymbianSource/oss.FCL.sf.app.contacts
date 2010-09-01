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
* Description:  Phonebook 2 contact info list box.
*
*/


#ifndef CPBK2CONTACTINFOLISTBOX_H
#define CPBK2CONTACTINFOLISTBOX_H

//  INCLUDES
#include "MPbk2ClipListBoxText.h"
#include "CPbk2FieldAnalyzer.h"
#include <TPbk2IconId.h>
#include <aknlists.h>
#include <MPbk2PointerEventInspector.h>
#include <MVPbkContactAttributeManager.h>


// FORWARD DECLARATIONS
class CVPbkContactManager;
class CPbk2PresentationContactFieldCollection;
class CItemDrawer;
class CPbk2FieldListBoxModel;
class CPbk2ContactInfoDataSwapper;
class MPbk2FieldPropertyArray;
class CPbk2ContactFieldDynamicProperties;
class MVPbkContactOperationBase;

/**
 * Phonebook 2 contact info list box.
 */
NONSHARABLE_CLASS(CPbk2ContactInfoListBox) :
        public CAknFormDoubleGraphicStyleListBox,
        public MPbk2ClipListBoxText,
        public MPbk2PointerEventInspector,
        public MVPbkMultiAttributePresenceObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aContainer    The control container for this control.
         * @param aResourceId   The resource id from where to construct
         *                      this object from.
         * @param aManager      Virtual phonebook contact manager.
         * @return  A new instance of this class.
         */
        static CPbk2ContactInfoListBox* NewL(
                CCoeControl& aContainer,
                TInt aResourceId,
                CVPbkContactManager& aManager );

        /**
         * Destructor.
         */
        ~CPbk2ContactInfoListBox();

    public: // Interface

        /**
         * Prepares the data for updating, i.e. replacing the shown
         * contact with a new one.
         *
         * @param aDataSwapper          Utility class for data
         *                              storage and swapping.
         * @param aFieldCollection      Contact's fields.
         * @param aFieldProperties      Field properties.
         * @param aDynamicProperties    Dynamic field properties.
         */
        void PrepareForUpdateL(
                CPbk2ContactInfoDataSwapper& aDataSwapper,
                const CPbk2PresentationContactFieldCollection& aFieldCollection,
                MPbk2FieldPropertyArray& aFieldProperties,
                const CPbk2ContactFieldDynamicProperties* aDynamicProperties );

        /**
         * Updates list box content with the data got from the data swapper.
         *
         * @param aDataSwapper  Utility class for data storage and swapping.
         * @param aContact      Store contact.
         */
        void DoUpdateL(
                CPbk2ContactInfoDataSwapper& aDataSwapper,
                MVPbkStoreContact& aContact );

        /**
         * Returns the number of items in the list box currently.
         *
         * @return  Number of items in the listbox.
         */
        TInt NumberOfItems() const;

        /**
         * Returns list box selection status.
         *
         * @return  ETrue if there are items marked, EFalse otherwise.
         */
        TBool ContactsMarked() const;

        /**
         * Returns current item index.
         *
         * @return  Current item index.
         */
        TInt CurrentItemIndex() const;

        /**
         * Handles resource change events.
         * @see CCoeControl::HandleResourceChange
         *
         * @param aType Resource change type.
         */
        void DoHandleResourceChangeL(
                const TInt aType ) const;

        /**
         * Returns flags read from the resource file.
         *
         * @return  Flags from resource file (TResourceData.iFlags).
         */
        TUint Flags() const;

        /**
         * Sets the scroll event observer.
         */
        void SetScrollEventObserver(MEikScrollBarObserver* aObserver);

    public: // From CEikColumnListBox
        class CItemDrawer;
        CItemDrawer* ItemDrawer() const;
        void CreateItemDrawerL();
        void HandlePointerEventL(
                const TPointerEvent& aPointerEvent );
        void HandleScrollEventL( CEikScrollBar* aScrollBar, 
                                 TEikScrollEvent aEventType );

    private: // From MPbk2ClipListBoxText
        TBool ClipFromBeginning(
                TDes& aBuffer,
                TInt aItemIndex,
                TInt aSubCellNumber );

    public: // From MPbk2PointerEventInspector
        TBool FocusedItemPointed();
        TBool FocusableItemPointed();
        TBool SearchFieldPointed();

    private: // from MVPbkMultiAttributePresenceObserver
        void AttributePresenceOperationComplete(
            MVPbkContactOperationBase& aOperation, 
            MVPbkStoreContactFieldCollection* aResult);
        void AttributePresenceOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError);

    private: // Implementation
        CPbk2ContactInfoListBox(
                CVPbkContactManager& aManager );
        void ConstructL(
                CCoeControl& aContainer,
                TInt aResourceId );
        CPbk2FieldListBoxModel* CreateListBoxModelL(
                const CVPbkContactManager& aContactManager,
                const CPbk2PresentationContactFieldCollection& aFieldCollection,
                MPbk2FieldPropertyArray& aFieldProperties,
                const CPbk2ContactFieldDynamicProperties* aDynamicProperties );
        void DoCreateVoiceTagRetrieverL(MVPbkStoreContact& aContact);
        void DoRefreshListBoxContentL(
            MVPbkStoreContactFieldCollection* aResult);

    private: // Data loaded from resources
        struct TResourceData
            {
            /// Own: Flags
            TUint iFlags;
            /// Own: Text to use for unnamed entries
            HBufC* iUnnamedText;
            /// Own: Empty icon id
            TPbk2IconId iEmptyIconId;
            /// Own: Default icon id
            TPbk2IconId iDefaultIconId;
            /// Own: Listbox empty text in find state
            HBufC* iFindEmptyText;
            };

    private: // Data
        /// Own: Data loaded from resources
        TResourceData iResourceData;
        /// Own: List box model
        MDesC16Array* iModel;
        /// Own: Field analyzer
        CPbk2FieldAnalyzer* iFieldAnalyzer;
        /// Ref: Field collection
        const CPbk2PresentationContactFieldCollection* iFieldCollection;
        /// Ref: Virtual Phonebook contact manager
        CVPbkContactManager& iManager;
        /// Own: ETrue, if user tapped a focusable item in the list
        TBool iFocusableItemPointed;
        /// Own: ETrue, if user tapped a previously focused item in the list
        TBool iFocusedItemPointed;
        /// Own: Voice tag retrieval operation
        MVPbkContactOperationBase* iVoiceTagFindOperation;
        /// Ref: scroll event observer
        MEikScrollBarObserver* iScrollEventObserver;

    };

#endif // CPBK2CONTACTINFOLISTBOX_H

// End of File
