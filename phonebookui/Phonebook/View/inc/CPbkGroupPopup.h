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
*     UI control for Phonebook's "Add to group".
*
*/


#ifndef __CPbkGroupPopup_H__
#define __CPbkGroupPopup_H__

//  INCLUDES
#include <e32base.h>        // CBase
#include <cntdef.h>         // TContactItemId
#include <cntviewbase.h>    // MContactViewObserver
#include <eikcmobs.h>       // MEikCommandObserver

//  FORWARD DECLARATIONS
class CEikColumnListBox;
class CPbkInputAbsorber;

// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Popup dialog which displays groups.
 */
NONSHARABLE_CLASS(CPbkGroupPopup) : 
        public CBase, 
        private MContactViewObserver,
        private MEikCommandObserver
    {
    public:  // interface
        /**
         * Parameters object for NewL.
         */
        struct TParams
            {
            /// Ref, input: a contact view containing the groups to display
            CContactViewBase* iGroupView;
            /// Own, input: Initial (before iGroupView is ready) popup softkeys
			/// resource id
            TInt iInitialSoftkeyResId;
            /// Own, input: Popup softkeys resource id
            TInt iSoftkeyResId;
            /// Ref, input: Popup prompt text
            const TDesC* iPrompt;
            /// Ref, input: empty text for the listbox
            const TDesC* iEmptyText;
            /// Own, input: listbox flags
            TInt iListBoxFlags;
            /// Own, output: Selected group's id
            TContactItemId iSelectedGroupId;

            /**
             * Constructor. Initializes mandatory fields with parameters and 
             * other fields to default values.
			 * @param aGroupView the view
			 * @param aInitialSoftkeyResId initial popup softkeys resource id
			 * @param aSoftKeyResId popup softkeys resource id
             */
            TParams
                (CContactViewBase& aGroupView, 
                TInt aInitialSoftkeyResId,
                TInt aSoftKeyResId);
            };

        /**
         * Creates a new instance of this class.
         *
         * @param aParams @see TParams.
         */
        static CPbkGroupPopup* NewL(TParams& aParams);

        /**
         * Destructor. Also makes ExecuteLD return like the dialog was
		 * cancelled.
         */
        ~CPbkGroupPopup();

        /**
         * Executes and deletes (also if a leave occurs) this dialog.
         *
         * @postcond    (result==0 && iParams.iSelectedGroupId==KNullContactId) ||
         *              (result!=0 && iParams.iSelectedGroupId!=KNullContactId)
         */
        TInt ExecuteLD();

        /**
         * Resets *aSelfPtr to NULL when this object is destroyed.
         *
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CPbkGroupPopup** aSelfPtr);

    private:  // from MContactViewObserver
	    void HandleContactViewEvent
            (const CContactViewBase& aView,
            const TContactViewEvent& aEvent);

    private:  // from MEikCommandObserver
    	void ProcessCommandL(TInt aCommandId);

    private:  // Implementation
        CPbkGroupPopup(TParams& aParams);
        void ConstructL();
        void WaitForViewEventL();
        void RunPopupL();
        void Close();
        void DoHandleContactViewEventL(const TContactViewEvent& aEvent);

    private:  // data
        /// Ref: parameters
        TParams& iParams;
        /// Own: return value from ExecuteLD
        TInt iResult;
        /// Own: event absorbing control
        CPbkInputAbsorber* iInputAbsorber;
        /// Own: listbox
        CEikColumnListBox* iListBox;
        /// Type of iPopupList
        class CPopupList;
        /// Own: popuplist that contains iListBox
        CPopupList* iPopupList;
        /// Ref: destructor sets referred TBool to ETrue
        TBool* iDestroyedPtr;
        /// Ref: referred pointer is set to NULL in destructor
        CPbkGroupPopup** iSelfPtr;
    };

#endif // __CPbkGroupPopup_H__

// End of File
