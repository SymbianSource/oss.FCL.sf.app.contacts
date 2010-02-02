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
* Description:  Phonebook 2 call AIW interest item.
*
*/

#ifndef CPBK2AIWINTERESTITEMCALL_H
#define CPBK2AIWINTERESTITEMCALL_H

// INCLUDES
#include <e32base.h>
#include "CPbk2AiwInterestItemBase.h"
#include "MPbk2AiwInterestItem.h"
#include <MVPbkSingleContactOperationObserver.h>
#include "MPbk2StoreStateCheckerObserver.h"
#include <MVPbkOperationObserver.h>

// FORWARD DECLARATIONS
class CAiwServiceHandler;
class MPbk2Command;
class CVPbkContactManager;
class MVPbkContactLink;
class MVPbkContactOperationBase;
class MVPbkStoreContact;
class CPbk2FieldPropertyArray;
class MVPbkStoreContactField;
class CPbk2CallTypeSelector;
class CPbk2StoreStateChecker;
class CPbk2ApplicationServices;
class MVPbkContactOperationBase;
class CVPbkContactLinkArray;
class CVPbkxSPContacts;
class MPbk2AppUi;
class MVPbkContactLinkArray;

// CLASS DECLARATION

/**
 * Phonebook 2 call AIW interest item.
 * Responsible for call creation and menu pane filtering.
 */
NONSHARABLE_CLASS(CPbk2AiwInterestItemCall) :
        public CPbk2AiwInterestItemBase,
        public MVPbkSingleContactOperationObserver,
        private MPbk2StoreStateCheckerObserver,
        public MVPbkOperationErrorObserver,
        public MVPbkOperationResultObserver<MVPbkContactLinkArray*>
    {
    public:  // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @params aServiceHandler  AIW service handler.
         * @return  A new instance of this class.
         */
        static CPbk2AiwInterestItemCall* NewL(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler );

        /**
         * Destructor.
         */
        ~CPbk2AiwInterestItemCall();

    public: // From MPbk2AiwInterestItem
        TBool DynInitMenuPaneL(
                const TInt aResourceId,
                CEikMenuPane& aMenuPane,
                const MPbk2ContactUiControl& aControl );
        TBool HandleCommandL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                TInt aServiceCommandId );

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact );
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation,
                TInt aError );

    private: // MPbk2StoreStateCheckerObserver
        void StoreState(
                MPbk2StoreStateCheckerObserver::TState aState );
                
    private: // From CActive
        void DoCancel();
        void RunL();
        TInt RunError(
                TInt aError );

    private: // Data structures
        enum TNextAction
            {
            ERetrieveStoreContact,
            ERetrieveXSPContact,
            EExecuteCall,
            EShowUnavailableNote,
            };

    private: // Implementation
        CPbk2AiwInterestItemCall(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler );
        void ConstructL();
        void ExecuteCommandL(
                MVPbkStoreContact*& aStoreContact );
        MPbk2Command* CreateCallCmdObjectL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                MVPbkStoreContact*& aStoreContact );
        MPbk2Command* CreatePocCmdObjectL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                MVPbkStoreContact& aStoreContact,
                MVPbkStoreContactField* aStoreContactField );
        MPbk2Command* CreatePrependCmdObjectL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                MVPbkStoreContact*& aStoreContact );                
        void RetrieveContactL();
        void IssueNextAction(
                TNextAction aState );
        MVPbkStoreContactField* FocusedFieldLC(
                MVPbkStoreContact& aStoreContact,
                TInt aFocusedFieldIndex );
        void AssignFocusedFieldL(
                MPbk2ContactUiControl& aControl,
                MVPbkStoreContact& aStoreContact );
        void VideoCallMenuVisibilityL(
                CEikMenuPane& aMenuPane ) const;
        void FilterCallMenuL(
                const TInt aResourceId,
                CEikMenuPane& aMenuPane,
                const MPbk2ContactUiControl& aControl );
        void FilterContextMenuL(
                const TInt aResourceId,
                CEikMenuPane& aMenuPane,
                const MPbk2ContactUiControl& aControl );
        void FetchxSPLinksL();
    
    public: // From MVPbkOperationErrorObserver
        void VPbkOperationFailed(
                MVPbkContactOperationBase* aOperation,
                TInt aError );
        
    public: // From MVPbkOperationResultObserver<MVPbkContactLinkArray*>
        void VPbkOperationResultCompleted(
                MVPbkContactOperationBase* aOperation,
                MVPbkContactLinkArray* aArray );

    private: // Data
        /// Own: Contact link
        MVPbkContactLink* iContactLink;
        /// Own: Retrieve contact operation
        MVPbkContactOperationBase* iRetriever;
        /// Own: Retrieved store contact
        MVPbkStoreContact* iStoreContact;
        /// Ref: Field property array
        CPbk2FieldPropertyArray* iFieldPropertyArray;
        /// Own: Menu command id
        TInt iMenuCommandId;
        /// Own: Call type selector
        CPbk2CallTypeSelector* iSelector;
        /// Ref: Contact UI control
        MPbk2ContactUiControl* iControl;
        /// Own: Focused field
        MVPbkStoreContactField* iFocusedField;
        /// Own: Check store state
        CPbk2StoreStateChecker* iStoreStateChecker;
        /// Own: Next action
        TNextAction iState;
        /// Own: 
        CPbk2ApplicationServices* iAppServices;
        /// Ref: Pbk2 AppUi
        MPbk2AppUi* iAppUi;
        /// Own: xSP stores interface
        CVPbkxSPContacts* ixSPManager;
        /// Own: xSP contacts links array
        CVPbkContactLinkArray* ixSPContactLinksArray;
        /// Own: xSP store contacts array
        RPointerArray<MVPbkStoreContact> ixSPStoreContactsArray;
        
    };

#endif // CPBK2AIWINTERESTITEMCALL_H

// End of File
