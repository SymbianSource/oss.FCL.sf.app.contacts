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
* Description:  Phonebook 2 memory entry defaults assign dialog.
*
*/


#ifndef CPBK2MEMORYENTRYDEFAULTSASSIGNDLG_H
#define CPBK2MEMORYENTRYDEFAULTSASSIGNDLG_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ClipListBoxText.h"
#include <VPbkFieldTypeSelectorFactory.h>

// FORWARD DECLARATIONS
class CAknPopupList;
class CPbk2IconArray;
class CVPbkContactManager;
class CEikFormattedCellListBox;
class CPbk2PresentationContactFieldCollection;
class MPbk2FieldPropertyArray;
class CPbk2ApplicationServices;
class CPbk2FieldListBoxModel;

// CLASS DECLARATION

/**
 * Phonebook 2 memory entry defaults assign dialog.
 * Responsible for presenting memory entry defaults assign dialog.
 */
NONSHARABLE_CLASS(CPbk2MemoryEntryDefaultsAssignDlg) :
        public CBase,
        public MPbk2ClipListBoxText
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2MemoryEntryDefaultsAssignDlg* NewL();

        /**
         * Destructor.
         */
        ~CPbk2MemoryEntryDefaultsAssignDlg();

    public: // Interface

        /**
         * Executes the defaults assign dialog.
         * @param aFields  Collection of presentation fields from
         *                 where to select the default.
         * @param aManager Contact manager.
         * @param aFieldProperties Field properties
         * @return         Field index selected as default.
         *                 KErrCancel, if user pressed Cancel.
         *                 If the returned index is bigger than
         *                 than the field count indicates, it means
         *                 that the user decided to remove the default
         *                 (last choice of the dialog is 'no default').
         */
        IMPORT_C TInt ExecuteLD(
                const CPbk2PresentationContactFieldCollection& aFields,
                const CVPbkContactManager& aManager,
                const MPbk2FieldPropertyArray& aFieldProperties,
                const VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector aSelectorID = VPbkFieldTypeSelectorFactory::EEmptySelector );


    private: // From MPbkClipListBoxText
        TBool ClipFromBeginning(
                TDes& aBuffer,
                TInt aItemIndex,
                TInt aSubCellNumber );

    private: // Implementation
        void ConstructL();
        CPbk2MemoryEntryDefaultsAssignDlg();
        void PreparePopupL(
                const CPbk2PresentationContactFieldCollection& aFields,
                const CVPbkContactManager& aManager,
                const MPbk2FieldPropertyArray& aFieldProperties );
        void CreateModelL(
                const CPbk2PresentationContactFieldCollection& aFields,
                const CVPbkContactManager& aManager,
                const MPbk2FieldPropertyArray& aFieldProperties,
                const CPbk2IconArray& aIconArray );

    private: // Data
        /// Own: Field selection listbox
        CEikFormattedCellListBox* iListBox;
        /// Own: Popup list for the listbox
        CAknPopupList* iPopupList;
        /// Ref: Referred TBool is set ETrue in destructor
        TBool* iDestroyedPtr;
        /// Own: Application Services
        CPbk2ApplicationServices* iAppServices;
        /// Own: Listbox model
        CPbk2FieldListBoxModel* iModel;
        // Selector Id
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector iSelectorId;
    };

#endif // CPBK2MEMORYENTRYDEFAULTSASSIGNDLG_H

// End of File
