/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phonebook 2 server app assign create empty
*              : property phase.
*
*/


#ifndef CPBK2SELECTCREATEEMPTYPROPERTYPHASE_H
#define CPBK2SELECTCREATEEMPTYPROPERTYPHASE_H

// INCLUDES
#include <e32base.h>
#include "MPbk2ServicePhase.h"
#include <TVPbkContactStoreUriPtr.h>

// FORWARD DECLARATIONS
class MPbk2ServicePhaseObserver;
class MPbk2SelectFieldProperty;
class MVPbkStoreContact;
class MVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 server app assign create empty property phase.
 * Responsible for implementing the select property service
 * phase in no data / empty data cases for create new entry service.
 */
class CPbk2SelectCreateEmptyPropertyPhase : public CActive,
                                            public MPbk2ServicePhase
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver             Observer.
         * @param aSavingStoreUri       Saving store uri
         * @param aSelectFieldProperty  Select field property.
         * @param aResult               Selection result.
         * @param aNoteFlags            Information note flags.
         * @return  A new instance of this class.
         */
        static CPbk2SelectCreateEmptyPropertyPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                TVPbkContactStoreUriPtr aSavingStoreUri,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );

        /**
         * Destructor.
         */
        ~CPbk2SelectCreateEmptyPropertyPhase();

    public: // From MPbk2ServicePhase
        void LaunchServicePhaseL();
        void CancelServicePhase();
        void RequestCancelL(
                TInt aExitCommandId );
        void AcceptDelayedL(
                const TDesC8& aContactLinkBuffer );
        MVPbkContactLinkArray* Results() const;
        MVPbkStoreContact* TakeStoreContact();
        TInt ExtraResultData() const;
        HBufC* FieldContent() const;

    private: // From CActive
        void RunL();
        TInt RunError(
                TInt aError );
        void DoCancel();

    private: // Implementation
        CPbk2SelectCreateEmptyPropertyPhase(
                MPbk2ServicePhaseObserver& aObserver,
                TVPbkContactStoreUriPtr aSavingStoreUri,
                MPbk2SelectFieldProperty*& aSelectFieldProperty,
                TInt& aResult,
                TUint& aNoteFlags );
        void IssueRequest();
        void CreateNewContactL();

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Saving store URI
        TVPbkContactStoreUriPtr iSavingStoreUri;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Ref: Select field property
        MPbk2SelectFieldProperty*& iSelectFieldProperty;
        /// Own: Selected field index
        TInt& iResult;
        /// Ref: Information note flags
        TUint& iNoteFlags;
    };

#endif // CPBK2SELECTCREATEEMPTYPROPERTYPHASE_H

// End of File
