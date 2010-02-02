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
*      Multiple contacts removal from group command.
*
*/


#ifndef __CPbkRemoveFromGroupCmd_H__
#define __CPbkRemoveFromGroupCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <AknProgressDialog.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>


// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactIdArray;
class CPbkWaitNoteWrapperBase;
class CPbkRemoveFromGroup;
class CPbkContactViewListControl;

// CLASS DECLARATION

/**
 * Multiple contacts removal from group command.
 */
class CPbkRemoveFromGroupCmd :
        public CBase,
        public MPbkCommand,
        private MPbkProcessObserver        
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine reference to a contact engine
         * @param aContacts contacts to delete
		 * @param aUiControl the UI control to update
		 * @param aGroupId the group where to remove from
         * @return a new delete contacts command object
         */
        static CPbkRemoveFromGroupCmd* NewL
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts,
            CPbkContactViewListControl& aUiControl,
            TContactItemId aGroupId);

        /**
         * Sets aSelfPtr to NULL when this object is destroyed.
         * @precond !aSelfPtr || *aSelfPtr==this
         */
        void ResetWhenDestroyed(CPbkRemoveFromGroupCmd** aSelfPtr);

        /**
         * Destructor.
         */
        ~CPbkRemoveFromGroupCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        void AddObserver(MPbkCommandObserver& aObserver);

    public:  // from MPbkProcessObserver
        void ProcessFinished(MPbkBackgroundProcess& aProcess);

    private:  // Implementation
        CPbkRemoveFromGroupCmd(CPbkContactEngine& aEngine, CPbkContactViewListControl& aUiControl);
        void ConstructL(const CContactIdArray& aContacts, TContactItemId aGroupId);                
        void DestroySelf();

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Ref: UI control
        CPbkContactViewListControl& iUiControl;
        /// Own: set to ETrue in destructor entry
        TBool iDestroyed;
        /// Own: wait note wrapper
        CPbkWaitNoteWrapperBase* iWaitNoteWrapper;
        /// Own: contacts deletion process
        CPbkRemoveFromGroup* iRemoveFromGroupProcess;
        /// Own: contact to focus after deletion
        TInt iFocusIndex;
        /// Own: focused contact item
        TContactItemId iFocusedContact;
        /// Ref: pointer to pointer to reset in destructor
        CPbkRemoveFromGroupCmd** iSelfPtr;
        /// Ref: command observer
        MPbkCommandObserver* iCommandObserver;        
    };

#endif // __CPbkRemoveFromGroupCmd_H__

// End of File
