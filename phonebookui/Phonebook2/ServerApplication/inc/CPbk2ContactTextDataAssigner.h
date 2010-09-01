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
* Description:  Phonebook 2 textual contact data assigner.
*
*/


#ifndef CPBK2CONTACTTEXTDATAASSIGNER_H
#define CPBK2CONTACTTEXTDATAASSIGNER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactAssigner.h"

// FORWARD DECLARATIONS
class MPbk2ContactAssignerObserver;

// CLASS DECLARATION

/**
 * Phonebook 2 textual contact data assigner.
 */
class CPbk2ContactTextDataAssigner : public CActive,
                                     public MPbk2ContactAssigner
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @return  A new instance of this class.
         */
        static CPbk2ContactTextDataAssigner* NewL(
                MPbk2ContactAssignerObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ContactTextDataAssigner();

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
        CPbk2ContactTextDataAssigner(
                MPbk2ContactAssignerObserver& aObserver );
        void InsertDataL(
                MVPbkStoreContactField& aField,
                const HBufC& aDataBuffer );
        void IssueRequest();

    private: // Data
        /// Ref: Observer
        MPbk2ContactAssignerObserver& iObserver;
        /// Own: Index of the added field
        TInt iIndex;
    };

#endif // CPBK2CONTACTTEXTDATAASSIGNER_H

// End of File
