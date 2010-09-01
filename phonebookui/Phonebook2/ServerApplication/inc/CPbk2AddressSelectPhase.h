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
* Description:  Phonebook 2 server app address select phase.
*
*/


#ifndef CPBK2ADDRESSSELECTPHASE_H
#define CPBK2ADDRESSSELECTPHASE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2KeyEventHandler.h>
#include "MPbk2ServicePhase.h"
#include <AiwContactSelectionDataTypes.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2DialogEliminator;
class MPbk2ServicePhaseObserver;
class MVPbkContactOperationBase;
class RVPbkContactFieldDefaultPriorities;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class CPbk2KeyEventDealer;
class CEikonEnv;

// CLASS DECLARATION

/**
 * Phonebook 2 server app address select phase.
 * Responsible for performing address select.
 */
class CPbk2AddressSelectPhase : public CBase,
                                public MPbk2ServicePhase,
                                private MVPbkSingleContactOperationObserver,
                                private MPbk2KeyEventHandler,
                                public MVPbkContactStoreObserver
    {
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver         Observer.
         * @param aContactLinks     Contact links.
         * @param aPriorities       Address select default priorities.
         * @param aAddressSelectType       Address select type.
         * @param aRskBack          Indicates whether the RSK should be Back.
         * @return  A new instance of this class.
         */
        static CPbk2AddressSelectPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray& aContactLinks,
                RVPbkContactFieldDefaultPriorities& aPriorities,
                TAiwAddressSelectType aAddressSelectType,
                TBool aRskBack );

        /**
         * Destructor.
         */
        ~CPbk2AddressSelectPhase();

    public: // From MPbk2ServicePhase
        void LaunchServicePhaseL();
        void CancelServicePhase();
        void RequestCancelL(
                TInt aExitCommandId );
        void AcceptDelayedL(
                const TDesC8& aContactLinkBuffer );
        void DenyDelayedL(
                const TDesC8& aContactLinkBuffer );
        MVPbkContactLinkArray* Results() const;
        TInt ExtraResultData() const;
        MVPbkStoreContact* TakeStoreContact();
        HBufC* FieldContent() const;

    public: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // From MPbk2KeyEventHandler
        TBool Pbk2ProcessKeyEventL(
                const TKeyEvent& aKeyEvent,
                TEventCode aType );
                
    private: // From MVPbkContactStoreObserver
        void StoreReady(
                MVPbkContactStore& aContactStore );
        void StoreUnavailable(
                MVPbkContactStore& aContactStore,
                TInt aReason );
        void HandleStoreEventL(
                MVPbkContactStore& aContactStore,
                TVPbkContactStoreEvent aEvent );

    private: // Implementation
        CPbk2AddressSelectPhase(
                MPbk2ServicePhaseObserver& aObserver,
                RVPbkContactFieldDefaultPriorities& aPriorities,                
                TBool aRskBack );
        void ConstructL(
                MVPbkContactLinkArray& aContactLinks,
                TAiwAddressSelectType aAddressSelectType );
        void RetrieveContactL();
        void DoSelectAddressesL();
        TInt CorrectRSK(
                TInt aAddressSelectResourceId );
        MVPbkStoreContactField* CheckIfFieldAlreadySelectedLC();
        void AddFieldToResultsAndContinueL(
                MVPbkStoreContactField* aField );
        void AppendResultL(
                const MVPbkStoreContactField* aField );
        TInt AddressSelectResourceId(
                TAiwAddressSelectType aType );                

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Contact links
        CVPbkContactLinkArray* iContactLinks;
        /// Own: Results array
        CVPbkContactLinkArray* iResults;
        /// Ref: Address select default priorities
        RVPbkContactFieldDefaultPriorities& iPriorities;
        /// Own: Address select resource id
        TInt iResourceId;
        /// Own: Indicates whether the RSK should be Back
        TBool iRskBack;
        /// Ref: For address select dialog's exit handling
        MPbk2DialogEliminator* iAddressSelectEliminator;
        /// Own: Contact retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;
        /// Own: Store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Key event dealer
        CPbk2KeyEventDealer* iDealer;
        /// Own: Title resource id
        TInt iTitleResId;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikenv;
        ///Bool to denote whether the Current Obj has been deleted or not
		///Doesnt Own
        TBool *iThisPtrDestroyed;
    };

#endif // CPBK2ADDRESSSELECTPHASE_H

// End of File
