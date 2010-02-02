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
* Description:  Phonebook 2 empty contact data assigner.
*
*/


#ifndef CPBK2CONTACTEMPTYDATAASSIGNER_H
#define CPBK2CONTACTEMPTYDATAASSIGNER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactAssigner.h"
#include <Pbk2ServerAppIPC.h>

// FORWARD DECLARATIONS
class MPbk2ContactAssignerObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 empty contact data assigner.
 */
class CPbk2ContactEmptyDataAssigner : public CActive,
                                      public MPbk2ContactAssigner
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @return  A new instance of this class.
         */
        static CPbk2ContactEmptyDataAssigner* NewL(
                MPbk2ContactAssignerObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ContactEmptyDataAssigner();

    public: // From MPbk2ContactAssigner
        void AssignDataL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                const MVPbkFieldType* aFieldType,
                const HBufC* aDataBuffer );
        void AssignAttributeL(
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aContactField,
                TPbk2AttributeAssignData aAttributeAssignData );

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(
                TInt aError );

    private: // Implementation
        CPbk2ContactEmptyDataAssigner(
                MPbk2ContactAssignerObserver& aObserver );
        void IssueRequest();

    private: // Data
        /// Ref: Observer
        MPbk2ContactAssignerObserver& iObserver;
    };

#endif // CPBK2CONTACTEMPTYDATAASSIGNER_H

// End of File
