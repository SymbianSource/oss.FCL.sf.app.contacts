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
* Description:  Phonebook 2 server app attribute address select phase.
*
*/

#ifndef CPBK2ATTRIBUTEADDRESSSELECTPHASE_H
#define CPBK2ATTRIBUTEADDRESSSELECTPHASE_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2KeyEventHandler.h>
#include <Pbk2ServerAppIPC.h>
#include <MVPbkContactStoreObserver.h>
#include "MPbk2ServicePhase.h"

// FORWARD DECLARATIONS
class CVPbkContactLinkArray;
class MPbk2DialogEliminator;
class MPbk2ServicePhaseObserver;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class CPbk2KeyEventDealer;
class CEikonEnv;
class MVPbkContactOperationBase;
class CVPbkContactLinkArray;
class CVPbkxSPContacts;
class MVPbkContactLinkArray;
// CLASS DECLARATION

/**
 * Phonebook 2 server app attribute address select phase.
 * Responsible for performing address select.
 */
class CPbk2AttributeAddressSelectPhase :
            public CBase,
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
         * @param aStoreContact     Contact to operate with,
         *                          ownership is taken.
         * @param aAttributeData    Attribute data.
         * @param aRskBack          Indicates whether the RSK should be Back.
         * @return  A new instance of this class.
         */
        static CPbk2AttributeAddressSelectPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                MVPbkContactLinkArray* aContactLinks,
                TPbk2AttributeAssignData aAttributeData,
                TBool aRskBack );

        /**
         * Destructor.
         */
        ~CPbk2AttributeAddressSelectPhase();

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

    private: // From MVPbkSingleContactOperationObserver
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
        CPbk2AttributeAddressSelectPhase(
                MPbk2ServicePhaseObserver& aObserver,
                TPbk2AttributeAssignData aAttributeData,
                TBool aRskBack );
        void ConstructL(
                MVPbkContactLinkArray* aContactLinks );
        void RetrieveContactL();
        void SelectAddressesL();
        MVPbkStoreContactField* CheckIfFieldAlreadySelectedL();
        TInt AddressSelectResourceId(
                TPbk2AttributeAssignData aAttributeData );
        TInt CorrectRSK(
                TInt aAddressSelectResourceId );
        void AppendResultL(
                const MVPbkStoreContactField* aField );
    

    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Results array
        CVPbkContactLinkArray* iResults;
        /// Own: Attribute data
        TPbk2AttributeAssignData iAttributeData;
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
        /// Own: Contact links to operate with
        CVPbkContactLinkArray* iContactLinks;
        /// Ref: Eikon enviroment
        CEikonEnv* iEikEnv;
    };

#endif // CPBK2ATTRIBUTEADDRESSSELECTPHASE_H

// End of File
