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
* Description:  Phonebook 2 server app assign select create
*              : new property phase.
*
*/


#ifndef CPBK2SELECTCREATENEWPROPERTYPHASE_H
#define CPBK2SELECTCREATENEWPROPERTYPHASE_H

// INCLUDES
#include <e32base.h>
#include <MPbk2ExitCallback.h>
#include "MPbk2ServicePhase.h"
#include <TVPbkContactStoreUriPtr.h>

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2ServicePhaseObserver;
class MVPbkContactLinkArray;
class MPbk2SelectFieldProperty;
class MPbk2DialogEliminator;
class MVPbkStoreContact;

// CLASS DECLARATION

/**
 * Phonebook 2 server app assign select create new property phase.
 * Responsible for selecting single assign property.
 */
class CPbk2SelectCreateNewPropertyPhase :
            public CActive,
            public MPbk2ServicePhase,
            private MPbk2ExitCallback
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aSavingStoreUri       Saving store uri
         * @param aObserver             Observer.
         * @param aFilterBuffer         Address select filter buffer.
         * @param aSelectFieldProperty  Select field property.
         * @param aResult               Selection result.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static CPbk2SelectCreateNewPropertyPhase* NewL(
                TVPbkContactStoreUriPtr  aSavingStoreUri,
                MPbk2ServicePhaseObserver& aObserver,
                HBufC8* aFilterBuffer,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );

        /**
         * Destructor.
         */
        ~CPbk2SelectCreateNewPropertyPhase();

    public: // From MPbk2ServicePhase
        void LaunchServicePhaseL();
        void CancelServicePhase();
        void RequestCancelL(
                TInt aExitCommandId );
        void AcceptDelayedL(
                const TDesC8& aContactLinkBuffer );
        MVPbkContactLinkArray* Results() const;
        TInt ExtraResultData() const;
        MVPbkStoreContact* TakeStoreContact();
        HBufC* FieldContent() const;

    public: // From MPbk2ExitCallback
        TBool OkToExitL(
                TInt aCommandId );

    private: // From CActive
        void RunL();
        TInt RunError(
                TInt aError );
        void DoCancel();

    private: // Implementation
        CPbk2SelectCreateNewPropertyPhase(
                TVPbkContactStoreUriPtr  aSavingStoreUri,
                MPbk2ServicePhaseObserver& aObserver,
                HBufC8* aFilterBuffer,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );
        void SelectFieldL();
        void AppendResultL(
                const MVPbkStoreContact* aStoreContact );
        void IssueRequest();

    private: // Data
        /// Own: contact store uri ptr
        TVPbkContactStoreUriPtr iSavingStoreUri;
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: To check is this destroyed
        TBool* iDestroyedPtr;
        /// Ref: Select field dialog eliminator
        MPbk2DialogEliminator* iSelectFieldDialogEliminator;
        /// Ref: Address select filter buffer
        HBufC8* iFilterBuffer;
        /// Ref: Select field property
        MPbk2SelectFieldProperty*& iSelectFieldProperty;
        /// Own: Selected field index
        TInt& iResult;
        /// Ref: Information note flags
        TUint& iNoteFlags;        
    };

#endif // CPBK2SELECTCREATENEWPROPERTYPHASE_H

// End of File
