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


#ifndef CPBK2COMMADDRESSSELECTPHASE_H
#define CPBK2COMMADDRESSSELECTPHASE_H

// INCLUDES
#include <badesca.h>
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MPbk2KeyEventHandler.h>
#include "MPbk2ServicePhase.h"
#include <VPbkFieldTypeSelectorFactory.h>
#include <MVPbkOperationObserver.h>
#include <mcontactpresenceobs.h>
#include <MVPbkContactStoreObserver.h>

// FORWARD DECLARATIONS
class MVPbkContactLink;
class CVPbkContactLinkArray;
class MPbk2DialogEliminator;
class MPbk2ServicePhaseObserver;
class MVPbkContactOperationBase;
class RVPbkContactFieldDefaultPriorities;
class MVPbkStoreContact;
class MVPbkStoreContactField;
class CPbk2KeyEventDealer;
class CEikonEnv;
class CVPbkxSPContacts;
class MContactPresence;
class CPbk2PresenceIconInfo;

// CLASS DECLARATION

/**
 * Phonebook 2 server app address select phase.
 * Responsible for performing address select.
 */
class CPbk2CommAddressSelectPhase : public CBase,
                    public MPbk2ServicePhase,
                    private MVPbkSingleContactOperationObserver,
                    private MPbk2KeyEventHandler,
                    public MVPbkOperationErrorObserver,
                    public MVPbkOperationResultObserver<MVPbkContactLinkArray*>,
                    public MContactPresenceObs,
                    public MVPbkContactStoreObserver
    {
    private:
        
        enum TState
            {
            EMainContactRetrieving,
            ExSPLinksRetrieving,
            ExSPContactsRetrieving,
            EWaitForPresenceIcons,
            EDialogWaitsUserInput
            };
     
    public: // Construction

        /**
         * Creates a new instance of this class.
         *
         * @param aObserver          Observer.
         * @param aContactLink       Contact link.
         * @param aPriorities        Address select default priorities.
         * @param aFieldTypeSelector Field type selector.
         * @param aCommSelector      Enum value for field type selector.
         * @param aRskBack           Indicates whether the RSK should be Back.
         * @return  A new instance of this class.
         */
        static CPbk2CommAddressSelectPhase* NewL(
                MPbk2ServicePhaseObserver& aObserver,
                const MVPbkContactLink& aContactLink,
                RVPbkContactFieldDefaultPriorities& aPriorities,
                CVPbkFieldTypeSelector& aFieldTypeSelector,
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
                    aCommSelector,
                TBool aRskBack );

        /**
         * Destructor.
         */
        ~CPbk2CommAddressSelectPhase();

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
                
    public: // From MVPbkOperationErrorObserver
        void VPbkOperationFailed( MVPbkContactOperationBase* aOperation,
            TInt aError );
    
    public: // From MVPbkOperationResultObserver
        void VPbkOperationResultCompleted(
            MVPbkContactOperationBase* aOperation,
            MVPbkContactLinkArray* aArray );
    
    public: //From MContactPresenceObs
    
        void ReceiveIconInfoL(
            const TDesC8& aPackedLink,
            const TDesC8& aBrandId,
            const TDesC8& aElementId );

        void ReceiveIconFileL(
            const TDesC8& aBrandId,
            const TDesC8& aElementId,
            CFbsBitmap* aBrandedBitmap,
            CFbsBitmap* aMask );
              
        void PresenceSubscribeError(
            const TDesC8& aPackedLink,
            TInt aStatus );

        void ErrorOccured(
            TInt aOpId,
            TInt aStatus );
        
        void ReceiveIconInfosL(
            const TDesC8& aPackedLink,
            RPointerArray <MContactPresenceInfo>& aInfoArray,
            TInt aOpId ); 
        
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
        CPbk2CommAddressSelectPhase(
                MPbk2ServicePhaseObserver& aObserver,
                RVPbkContactFieldDefaultPriorities& aPriorities,                
                TBool aRskBack,
                CVPbkFieldTypeSelector& aFieldTypeSelector,
                VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector
                    aCommSelector );
        void ConstructL(
                const MVPbkContactLink& aContactLink );
        void RetrieveContactL();
        void DoSelectAddressesL();
        TInt CorrectRSK(
                TInt aAddressSelectResourceId );
        void AppendResultL(
                const MVPbkStoreContactField* aField );
        void GetImppFieldDataL(CDesC16ArrayFlat & aXspIdArray, 
                MVPbkStoreContact& aContact );
        void FilterXspContactsL();
        TBool IsMatchL( MVPbkStoreContact& aXspContact, 
                MVPbkStoreContact& aStoreContact );
    
    private: // Data
        /// Ref: Observer
        MPbk2ServicePhaseObserver& iObserver;
        /// Own: Contact links
        MVPbkContactLink* iContactLink;
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
        /// Ref: Field type selector
        CVPbkFieldTypeSelector& iFieldTypeSelector;
        /// Own: xSP stores interface
        CVPbkxSPContacts* ixSPManager;
        /// Own: xSP contact retrieve operation
        MVPbkContactOperationBase* ixSPContactOperation;
        /// Own: xSP contacts array
        CVPbkContactLinkArray* ixSPContactsArray;
        /// Own: xSP store contacts array
        RPointerArray<MVPbkStoreContact> ixSPStoreContactsArray;
        /// Own: state
        TState iState;
        /// Own: content of the selected field
        HBufC* iFieldContent;
        /// Own: contact presence manager
        MContactPresence* iContactPresence;
        /// Own: array of presence icons
        RPointerArray<CPbk2PresenceIconInfo> iPresenceIconArray;
        /// Own: ETrue, if presence icon has been retrieved
        TBool iPresenceIconsRetrieved;
        /// Own: ETrue, if contact has been retrieved
        TBool iContactRetrieved;
        /// Own: Communication method
        VPbkFieldTypeSelectorFactory::TVPbkContactActionTypeSelector iCommMethod;
    };

#endif // CPBK2COMMADDRESSSELECTPHASE_H

// End of File