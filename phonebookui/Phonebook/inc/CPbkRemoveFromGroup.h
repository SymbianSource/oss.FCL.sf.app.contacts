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
*      Removing multiple contact from a group process.
*
*/


#ifndef __CPbkRemoveFromGroup_H__
#define __CPbkRemoveFromGroup_H__

// INCLUDES
#include <MPbkBackgroundProcess.h>
#include <cntdef.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactGroup;


// CLASS DECLARATION

/**
 * Removing multiple contact from a group process.
 */
class CPbkRemoveFromGroup :
        public CBase,
        public MPbkBackgroundProcess
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine reference to a contact engine
         * @param aContacts contacts to delete
		 * @param aGroupId the group to delete from
         */
        IMPORT_C static CPbkRemoveFromGroup* NewLC
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts,
            TContactItemId aGroupId);

        /**
         * Destructor.
         */
        ~CPbkRemoveFromGroup();

    public:  // from MPbkBackgroundProcess
        void StepL();
        TInt TotalNumberOfSteps();
        TBool IsProcessDone() const;
        void ProcessFinished();
        TInt HandleStepError(TInt aError);
        void ProcessCanceled();

    private:  // Implementation
        CPbkRemoveFromGroup(CPbkContactEngine& aEngine, TContactItemId aGroupId);
        void ConstructL(const CContactIdArray& aContacts);
        TBool MoreContactsToRemove() const;
        void Cancel();

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: initial number of contacts to remove from a group
        TInt iTotalContactsToRemove;
        /// Own: ids of contacts to remove
        CContactIdArray* iContacts;
        /// Own: count of contacts succesfully removed
        TInt iRemovedCount;
        /// Own: group to remove contacts from
        TContactItemId iGroupId;
        /// Own: opened group for blocking
        CContactGroup* iGroup;
    };


#endif // __CPbkRemoveFromGroup_H__

// End of File
