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
* Description:  Phonebook 2 add new contact field to a contact dialog.
*
*/


#ifndef CPBK2ADDITEMTOCONTACTDLG_H
#define CPBK2ADDITEMTOCONTACTDLG_H

// INCLUDES
#include <e32base.h>
#include <TPbk2AddItemDialogParams.h>
#include <MPbk2DialogEliminator.h>

// FORWARD DECLARATIONS
class CEikFormattedCellListBox;
class TPbk2AddItemWrapper;
class CAknPopupList;
class MPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Phonebook 2 add new contact field to a contact dialog.
 */
class CPbk2AddItemToContactDlg : public CBase,
                                 public MPbk2DialogEliminator
    {
    public: // Constructors and destructor

		/**
		 * Creates a new instance of this class.
		 *
		 * @return  A new instance of this class.
		 */
		IMPORT_C static CPbk2AddItemToContactDlg* NewL();
	
        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        IMPORT_C static CPbk2AddItemToContactDlg* NewL( MPbk2ApplicationServices* aAppServices );

        /**
         * Destructor.
         */
        ~CPbk2AddItemToContactDlg();

    public: // Interface

        /**
         * Executes the dialog.
         *
         * @param aAddItems     An array of items to show in the dialog.
         * @param aParams       Add item dialog parameters.
         * @return  Index of selected TPbk2AddItemWrapper in
         *          aAddItems array or KErrCancel if the dialog
         *          was canceled.
         */
        IMPORT_C TInt ExecuteLD(
                RArray<TPbk2AddItemWrapper>& aAddItems,
                const TPbk2AddItemDialogParams* aParams = NULL );

    public: // From MPbk2DialogEliminator
        void RequestExitL(
                TInt aCommandId );
        void ForceExit();
        void ResetWhenDestroyed(
                MPbk2DialogEliminator** aSelfPtr );

    private: // Implementation
        CPbk2AddItemToContactDlg( MPbk2ApplicationServices* aAppServices = NULL );
        void PreparePopupL(
                RArray<TPbk2AddItemWrapper>& aAddItems,
                const TPbk2AddItemDialogParams* aParams );
        TInt ExecutePopupL(
                const TPbk2AddItemDialogParams* aParams );

    private: // Data
        /// Own: To check is this destroyed
        TBool* iDestroyedPtr;
        /// Own: List box
        CEikFormattedCellListBox* iListBox;
        /// Own: Popup list
        CAknPopupList* iPopup;
        /// Ref: Set to NULL when this dialog is destroyed
        MPbk2DialogEliminator** iSelfPtr;
        // Ref: Pbk2 application services
        MPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2ADDITEMTOCONTACTDLG_H

// End of File
