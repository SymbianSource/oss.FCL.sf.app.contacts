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
* Description:  Phonebook 2 contact ringing tone assigner.
*
*/


#ifndef CPBK2CONTACTRINGINGTONEASSIGNER_H
#define CPBK2CONTACTRINGINGTONEASSIGNER_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ContactAssigner.h"
#include <Pbk2ServerAppIPC.h>

// FORWARD DECLARATIONS
class MPbk2ContactAssignerObserver;
class CPbk2DrmManager;

// CLASS DECLARATION

/**
 * Phonebook 2 contact ringing tone assigner.
 */
class CPbk2ContactRingingToneAssigner : public CActive,
                                        public MPbk2ContactAssigner
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver     Observer.
         * @return  A new instance of this class.
         */
        static CPbk2ContactRingingToneAssigner* NewL(
                MPbk2ContactAssignerObserver& aObserver );

        /**
         * Destructor.
         */
        ~CPbk2ContactRingingToneAssigner();

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
        CPbk2ContactRingingToneAssigner(
                MPbk2ContactAssignerObserver& aObserver );
        void ConstructL();
        void InsertDataL(
                MVPbkStoreContactField& aField,
                const HBufC& aDataBuffer );
        void IssueRequest();
        TInt PassesDrmCheckL(
                const HBufC* aDataBuffer,
                TBool& aIsProtected );
        TBool PassesSizeCheckL(
                const HBufC* aDataBuffer );
        void MaxToneFileSizeL(
                TInt& aMaxSizeKB ) const;
        TInt CheckFileSizeLimit(
                const TDesC& aFileName,
                TInt aSizeLimit );
        TInt CheckToneFileSize(
                const TDesC& aFile,
                TInt aSizeLimitKB );
        void ShowSizeErrorNoteL(
                TInt aSizeLimitKB ) const;
                
        /**
         * Process states.
         */
        enum TProcessState
            {            
            EStopping,
            EDrmProtected
            };                

    private: // Data
        /// Ref: Observer
        MPbk2ContactAssignerObserver& iObserver;
        /// Own: Index of the added field
        TInt iIndex;
        /// Own: Error code
        TInt iError;
        /// Own: DRM manager
        CPbk2DrmManager* iDrmManager;
        /// Own: Current state of process
        TProcessState iState;
    };

#endif // CPBK2CONTACTRINGINGTONEASSIGNER_H

// End of File
