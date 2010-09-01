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
*     Phonebook Info view UI control.
*
*/


#ifndef __CPbkPhonebookInfoDlg_H__
#define __CPbkPhonebookInfoDlg_H__

//  INCLUDES
#include <eikdialg.h>    // CEikDialog
#include <cntviewbase.h> // MContactViewObserver

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CAknPopupList;
class CAknSingleHeadingPopupMenuStyleListBox;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * "Phonebook Info View" UI control.
 */
class CPbkPhonebookInfoDlg : 
        public CBase, 
        private MContactViewObserver
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class.
		 * @param aEngine reference to Phonebook engine
         */
        IMPORT_C static CPbkPhonebookInfoDlg* NewL(
                CPbkContactEngine& aEngine);
        
        /**
         * Destructor.
         */
        ~CPbkPhonebookInfoDlg();

    public: // interface
        /**
         * Executes info note dialog.
         */
        IMPORT_C void ExecuteLD();

    private:  // from MContactViewObserver
	    void HandleContactViewEvent
            (const CContactViewBase& aView,const TContactViewEvent& aEvent);
    
    private: // implementation
        CPbkPhonebookInfoDlg(CPbkContactEngine& aEngine);
        void ConstructL();
	    void HandleContactViewEventL
            (const CContactViewBase& aView, const TContactViewEvent& aEvent);
		void SetTitleL(const TDesC &aTitle);
        MDesCArray* ConstructDataArrayL();
        HBufC* ContactsEntryLC();
        HBufC* GroupsEntryLC();
        HBufC* UsedMemoryEntryLC();
        HBufC* FreeMemoryEntryLC();
        void UpdateDataL();

    private: // Data members
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: all contacts view
        CContactViewBase* iAllContactsView;
        /// Ref: all groups view
        CContactViewBase* iAllGroupsView;
        /// Own: Popup list that is the actual dialog that is shown.
		CAknPopupList* iPopupList;
		/// Own: Listbox that is given to CAknPopupList constructor.
		CAknSingleHeadingPopupMenuStyleListBox* iListBox;
        /// Ref: Keeps track if destructor is already called.
		TBool* iDestroyed;
        /// Own: keeps track of contacts view readiness
        TBool iContactsViewReady;
        /// Own: keeps track of groups view readiness
        TBool iGroupsViewReady;
    };

#endif // __CPbkPhonebookInfoDlg_H__
                   
// End of File
