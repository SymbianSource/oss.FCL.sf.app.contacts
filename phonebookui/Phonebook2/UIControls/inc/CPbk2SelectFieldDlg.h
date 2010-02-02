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
* Description:  Phonebook 2 field selection dialog.
*
*/


#ifndef CPBK2SELECTFIELDDLG_H
#define CPBK2SELECTFIELDDLG_H

// INCLUDES
#include <e32base.h>
#include <CVPbkFieldFilter.h>
#include <TPbk2IconId.h>
#include "CPbk2FieldListBoxModel.h"
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MVPbkStoreContactField;
class MVPbkStoreContactFieldCollection;
class CPbk2PresentationContactFieldCollection;
class MPbk2ControlKeyObserver;
class CVPbkContactManager;
class MPbk2FieldPropertyArray;
class CPbk2FieldListBoxModel;
class CPbk2PresenceIconInfo;
class CPbk2FieldAnalyzer;

// CLASS DECLARATION

/**
 * Phonebook 2 field selection dialog.
 */
NONSHARABLE_CLASS(CPbk2SelectFieldDlg) : 
        public CBase,
        public MVPbkContactStoreObserver
    {
    public: // Construction and destruction

        /**
         * Constructor.
         */
        CPbk2SelectFieldDlg();

        /**
         * Destructor.
         */
        ~CPbk2SelectFieldDlg();

    public: // From MVPbkContactStoreObserver
      void StoreReady(MVPbkContactStore& aContactStore);
      void StoreUnavailable(MVPbkContactStore& aContactStore, 
                      TInt aReason);
      void HandleStoreEventL(
                      MVPbkContactStore& aContactStore, 
                      TVPbkContactStoreEvent aStoreEvent);
      
    public: // Interface

        /**
         * Runs the dialog and returns selected field. Caller takes the
         * ownership of the field. Field is left to the cleanupstack
         * on return. NOTE: this object is deleted when this function
         * returns or leaves.
         *
         * @param aFieldCollection  Array of store contact fields.
         * @param aFieldCollectionArray  Array of store contact fields arrays.
         * @param aPresenceIconsArray  Presence icons array
         * @param aContactManager   Virtual Phonebook contact manager.
         * @param aFieldProperties  Phonebook 2 field properties.
         * @param aCbaResourceId    Softkey buttons for the control.
         * @param aCommMethod	    Communication method type selected
         * @param aHeading          Possible heading text for the control.
         * @param aFocusIndex       Field index to focus initially.
         *
         * @return  Returns the selected contact item field or NULL.
         * @see CPbk2SelectFieldDlg::TPbk2SelectFieldFlags
         */
        const MVPbkStoreContactField* ExecuteLCD(
                MVPbkStoreContactFieldCollection& aFieldCollection,
                RPointerArray<CVPbkFieldFilter>& aFieldCollectionArray,
                const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray,
                const CVPbkContactManager& aContactManager,
                const MPbk2FieldPropertyArray& aFieldProperties,
                TInt aCbaResourceId,
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommMethod,
                const TDesC& aHeading = KNullDesC,
                TInt aFocusIndex = KErrNotFound );

        /**
         * Registers aKeyObserver as a key event observer of this control.
         * Only one observer is supported and setting a new one removes the
         * previous setting.
         *
         * @param aKeyObserver  Phonebook 2 control key observer.
         */
        void SetObserver(
                MPbk2ControlKeyObserver* aKeyObserver );

        /**
         * Attempts to exit this dialog just like a soft button was pressed.
         * @see CAknPopupList::AttemptExitL(TBool).
         */
        void AttemptExitL(
                TBool aAccept );

        /**
         * Sets given pointer to NULL when this dialog is destroyed.
         *
         * @param aSelfPtr      The pointer to use.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(
                CPbk2SelectFieldDlg** aSelfPtr );

    private: // Implementation
        void CreateListBoxL(
                const CPbk2PresentationContactFieldCollection& aFieldCollection,
                const TArray<CPbk2PresenceIconInfo*>* aPresenceIconsArray,
                const CVPbkContactManager& aContactManager,
                const MPbk2FieldPropertyArray& aFieldProperties,
                TInt aCbaResourceId,
                const TDesC& aHeading,
                TInt aFocusIndex,
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aCommMethod);

    private: // Data
        /// Specialized list box class
        class CListBox;
        /// Own: List box inside the popup list
        CListBox* iListBox;
        /// Specialized popup list class
        class CPopupList;
        /// Own: Popup list query
        CPopupList* iPopupList;
        /// Ref: Key event observer
        MPbk2ControlKeyObserver* iKeyObserver;
        /// Ref: Set to ETrue in destructor
        TBool* iDestroyedPtr;
        /// Ref: Set to NULL when this dialog is destroyed.
        CPbk2SelectFieldDlg** iSelfPtr;
        /// Ref: list box model
        CPbk2FieldListBoxModel* iListBoxModel;
        /// Own: presentation fields for additional contacts
        RPointerArray<CPbk2PresentationContactFieldCollection>
            iFieldCollectionArray;
        /// Own: Array to keep presence icons positions in the common array
        RArray<CPbk2FieldListBoxModel::TPresenceIconPosInIconArray> iPresenceIconsPos;
        // Own: Field analyser
        CPbk2FieldAnalyzer* iFieldAnalyzer;
        /// Own: Current contact link
        MVPbkContactLink* iContactLink;
    };

#endif // CPBK2SELECTFIELDDLG_H

// End of File
