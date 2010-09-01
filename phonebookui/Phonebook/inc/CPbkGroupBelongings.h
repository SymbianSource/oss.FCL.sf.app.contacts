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
*     UI control for Phonebook's "Group belongings".
*
*/


#ifndef __CPbkGroupBelongings_H__
#define __CPbkGroupBelongings_H__

//  INCLUDES
#include <e32base.h>
#include <cntdef.h>         // TContactItemId
#include <cntviewbase.h>    // MContactViewObserver
#include <MPbkContactSelector.h>

//  FORWARD DECLARATIONS
class CPbkContactEngine;
class CPbkGroupPopup;
class CPbkContactSubView;


// CLASS DECLARATION

/**
 * @internal Only Phonebook internal use supported!
 *
 * Phonebooks Group belongings dialog.
 */
class CPbkGroupBelongings : 
        public CBase, 
        private MContactViewObserver, 
        private MPbkContactSelector
    {
    public:  // Interface
        /**
         * Creates a new instance of this class.
         */
        IMPORT_C static CPbkGroupBelongings* NewL();

        /**
         * Executes and deletes (also if a leave occurs) this dialog.
         *
         * @param aEngine       phonebook engine
         * @param aContactId    group belongings are listed for this contact
         */
        IMPORT_C void ExecuteLD(CPbkContactEngine& aEngine,
			TContactItemId aContactId);

        /**
         * Destructor. Also cancels open dialog and makes ExecuteLD() return.
         */
        ~CPbkGroupBelongings();

    private:  // from MContactViewObserver
	    void HandleContactViewEvent
            (const CContactViewBase& aView,const TContactViewEvent& aEvent);

    private:  // from MPbkContactSelector
        TBool IsContactIncluded(TContactItemId aId);

    private:  // Implementation
        CPbkGroupBelongings();
        void CreateGroupSubViewL
            (CPbkContactEngine& aEngine, TContactItemId aContactId);
        void RunPopupL();

    private:  // data
        /// Own: group list popup
        CPbkGroupPopup* iGroupPopup;
        /// Own: groups list
        CContactIdArray* iGroups;
        /// Own: contact id
        TContactItemId iContactId;
        /// Own: groups sub view
        CPbkContactSubView* iGroupView;
        /// Ref: destructor sets referred TBool to ETrue
        TBool* iDestroyedPtr;
    };


#endif // __CPbkGroupBelongings_H__

// End of File
