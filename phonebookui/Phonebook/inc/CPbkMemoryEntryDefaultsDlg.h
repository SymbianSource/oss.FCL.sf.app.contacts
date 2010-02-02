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
*     Phonebook Memory entry Defaults dialog.
*
*/


#ifndef __CPbkMemoryEntryDefaultsDlg_H__
#define __CPbkMemoryEntryDefaultsDlg_H__

//  INCLUDES
#include <cntdef.h>     // TContactItemId
#include <aknPopup.h>   // CAknPopupList

// FORWARD DECLARATIONS
class CEikColumnListBox;
class CPbkContactEngine;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebook Memory entry Defaults dialog.
 * This is a custom popup for phonebook applications default settings.
 */
class CPbkMemoryEntryDefaultsDlg : public CAknPopupList
    {
    public:
        /**
         * Creates a new instance of this class.
         * @param aContactId contact item to which defaults are chosen.
		 * @param aEngine reference to phonebook engine
         */
        IMPORT_C static CPbkMemoryEntryDefaultsDlg* NewL
			(TContactItemId aContactId, CPbkContactEngine& aEngine);

        /**
         * Run the dialog.
         */
        IMPORT_C void ExecuteLD();
    
        /**
         * Destructor.
         * Also cancels and dismisses this popup list, in other words makes
         * ExecuteLD return as if cancel was pressed.
         */
        ~CPbkMemoryEntryDefaultsDlg();

    private: // from CAknPopupList
      	void ProcessCommandL(TInt aCommandId);
	    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

    private: // Implementation
        CPbkMemoryEntryDefaultsDlg(CPbkContactEngine& aEngine, TContactItemId aContactId);
        void ConstructL();
        void CreateLinesL() const;
        TBool DefaultsAssignPopupL(TInt aIndex);
        class CPbkMemoryEntryDefaultAssignDlg;
        TBool IsSupported(TInt aDefaults) const;
        TInt FixIndex(TInt aIndex) const;
        
    private: // data members
        /// Own: contact item id
        TContactItemId iContactId;
        /// Ref: contact engine
        CPbkContactEngine& iEngine;
        /// Own: default selection listbox
        CEikColumnListBox* iListBox;
        /// Own: Default field selection dialog
        CPbkMemoryEntryDefaultAssignDlg* iDefaultAssignDlg;
        /// Ref: Referred TBool is set ETrue in destructor
        TBool* iDestroyedPtr;
    };

#endif // __CPbkMemoryEntryDefaultsDlg_H__
            
// End of File
