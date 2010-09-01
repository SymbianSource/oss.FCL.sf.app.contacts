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
*      Multiple contact deletion process.
*
*/


#ifndef __CPbkDeleteContacts_H__
#define __CPbkDeleteContacts_H__

// INCLUDES
#include <MPbkBackgroundProcess.h>
#include <e32base.h>
#include <shareddataclient.h>   // RSharedDataClient

// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactIdArray;
class RSharedDataClient;
// CLASS DECLARATION

/**
 * Multiple contact deletion process.
 */
class CPbkDeleteContacts :
        public CBase,
        public MPbkBackgroundProcess
    {
    public: // Interface
        /**
         * OBSOLETE
         * Leaves with KErrNotSupported
         */
        IMPORT_C static CPbkDeleteContacts* NewLC
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts);

        /**         
         * Creates a new instance of this class.
         *
         * @param aEngine   reference to a contact engine.
         * @param aContacts contacts to delete.
         * @param aSharedDataClient shared data client
         */
        static CPbkDeleteContacts* NewLC
            (CPbkContactEngine& aEngine,
            const CContactIdArray& aContacts,
            RSharedDataClient& aSharedDataClient);

        /**
         * Destructor.
         */
        ~CPbkDeleteContacts();

        /**
         * Returns the number of contacts deleted.
         */
        TInt DeletedCount() const;

    public:  // from MPbkBackgroundProcess
        void StepL();
        TInt TotalNumberOfSteps();
        TBool IsProcessDone() const;
        void ProcessFinished();
        TInt HandleStepError(TInt aError);
        void ProcessCanceled();

    private:  // Implementation
        CPbkDeleteContacts(CPbkContactEngine& aEngine, RSharedDataClient& aSharedDataClient);
        void ConstructL(const CContactIdArray& aContacts);
        TBool MoreContactsToDelete() const;
        void Cancel();
        void CompressIfRequired();
        TInt64 FreeSpaceOnDbDrive() const;

    private:  // Data
        /// Ref: Phonebook engine
        CPbkContactEngine& iEngine;
        /// Own: initial number of contacts to delete
        TInt iTotalContactsToDelete;
        /// Own: ids of contacts to delete
        CContactIdArray* iContacts;
        /// Own: count of contacts succesfully deleted
        TInt iDeletedCount;
        /// Own: maximum free space required to delete a contact from the DB
        TInt iFreeSpaceRequiredToDelete;
        /// Own: contact database drive
        TInt iDbDrive;
        /// Ref: shared data client
        RSharedDataClient& iSharedDataClient;
    };


#endif // __CPbkDeleteContacts_H__

// End of File
