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
*      Assign ringing tone to group members process.
*
*/


#ifndef __CPbkSetToneToGroup_H__
#define __CPbkSetToneToGroup_H__

// INCLUDES
#include <e32base.h>
#include <MPbkBackgroundProcess.h>
#include <cntdef.h>

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactIdArray;
class CPbkContactItem;

// CLASS DECLARATION

/**
 * Assign ringing tone to group members process.
 */
class CPbkSetToneToGroup :
        public CBase,
        public MPbkBackgroundProcess
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         *
         * @param aEngine   reference to a contact engine
         * @param aContacts contacts to delete
		 * @param aRingingToneName ringing tone file name
         */
        IMPORT_C static CPbkSetToneToGroup* NewLC
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts,
            const TDesC& aRingingToneName);

        /**
         * Destructor.
         */
        ~CPbkSetToneToGroup();

        /**
         * Returns the ringing tone file name.
         */
        IMPORT_C const TDesC& RingingToneFile() const;

    public:  // from MPbkBackgroundProcess
        void StepL();
        TInt TotalNumberOfSteps();
        TBool IsProcessDone() const;
        void ProcessFinished();
        TInt HandleStepError(TInt aError);
        void ProcessCanceled();

    private:  // Implementation
        CPbkSetToneToGroup(CPbkContactEngine& aEngine);
        void ConstructL(const CContactIdArray& aContacts, const TDesC& aRingingToneName);
        TBool MoreContactsToSet() const;
        void Cancel();
        void SetRingingToneL(CPbkContactItem& aItem);

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: initial number of contacts to assign a tone
        TInt iTotalContactsToSet;
        /// Own: ids of contacts to set ringing tone to
        CContactIdArray* iContacts;
        /// Own: count of contacts succesfully set
        TInt iSetCount;
        /// Own: ringing tone file name
        HBufC* iRingingToneFile;
    };

#endif // __CPbkSetToneToGroup_H__

// End of File
