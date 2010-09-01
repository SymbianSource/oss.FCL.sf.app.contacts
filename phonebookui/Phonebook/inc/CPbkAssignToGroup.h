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
*      Assign multiple contact to a group process.
*
*/


#ifndef __CPbkAssignToGroup_H__
#define __CPbkAssignToGroup_H__

// INCLUDES
#include <MPbkBackgroundProcess.h>
#include <cntdef.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactGroup;

// CLASS DECLARATION

/**
 * Assign multiple contact to a group process.
 */
class CPbkAssignToGroup :
        public CBase,
        public MPbkBackgroundProcess
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine reference to a contact engine
         * @param aContacts contacts to assign
		 * @param aGroupId the group to assign to
         */
        IMPORT_C static CPbkAssignToGroup* NewLC
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts,
            TContactItemId aGroupId);

        /**
         * Destructor.
         */
        ~CPbkAssignToGroup();

    public:  // from MPbkBackgroundProcess
        void StepL();
        TInt TotalNumberOfSteps();
        TBool IsProcessDone() const;
        void ProcessFinished();
        TInt HandleStepError(TInt aError);
        void ProcessCanceled();

    private:  // Implementation
        CPbkAssignToGroup(CPbkContactEngine& aEngine, TContactItemId aGroupId);
        void ConstructL(const CContactIdArray& aContacts);
        TBool MoreContactsToAssign() const;
        void Cancel();

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: initial number of contacts to assign to a group
        TInt iTotalContactsToAssign;
        /// Own: ids of contacts to assign
        CContactIdArray* iContacts;
        /// Own: count of contacts succesfully assigned
        TInt iAssignedCount;
        /// Own: group to assign contacts
        TContactItemId iGroupId;
        /// Own: opened group for blocking
        CContactGroup* iGroup;
    };


#endif // __CPbkAssignToGroup_H__

// End of File
