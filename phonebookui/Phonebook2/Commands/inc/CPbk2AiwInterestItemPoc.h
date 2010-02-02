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
* Description:  Phonebook 2 PoC AIW interest item.
*
*/


#ifndef CPBK2AIWINTERESTITEMPOC_H
#define CPBK2AIWINTERESTITEMPOC_H

//  INCLUDES
#include <e32base.h>
#include "CPbk2AiwInterestItemBase.h"
#include <MVPbkSingleContactOperationObserver.h>
#include "MPbk2StoreStateCheckerObserver.h"

// FORWARD DECLARATIONS
class CAiwServiceHandler;
class MPbk2Command;
class MVPbkContactLink;
class CPbk2CallTypeSelector;
class CPbk2FieldPropertyArray;
class MPbk2ContactUiControl;
class CPbk2StoreStateChecker;

// CLASS DECLARATION

/**
 * Phonebook 2 PoC AIW interest item.
 */
NONSHARABLE_CLASS(CPbk2AiwInterestItemPoc) :
        public CPbk2AiwInterestItemBase,
        public MVPbkSingleContactOperationObserver,
        private MPbk2StoreStateCheckerObserver
    {
    public: // Constructors and destructor

        /**
         * Creates a new instance of this class.
         *
         * @param aInterestId       Interest id.
         * @params aServiceHandler  AIW service handler.
         * @return  A new instance of this class.
         */
        static CPbk2AiwInterestItemPoc* NewL(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler );

        /**
         * Destructor.
         */
        ~CPbk2AiwInterestItemPoc();

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

    private: // Data structures
        enum TNextAction
            {
            ERetrieveContact,
            EExecutePoc,
            EShowUnavailableNote,
            };

    private:  // Implementation
        CPbk2AiwInterestItemPoc(
                TInt aInterestId,
                CAiwServiceHandler& aServiceHandler );
        void ConstructL();
        void ExecuteCommandL(
                MVPbkStoreContact& aStoreContact );
        void RetrieveContactL();
        void IssueNextAction(
                TNextAction aState );
        void DoInitMenuPaneL(
                const TInt aResourceId,
                CEikMenuPane& aMenuPane,
                const MPbk2ContactUiControl& aControl,
                TUint aPreferredMenu = 0 ) const;
        MPbk2Command* CreatePocCmdObjectL(
                const TInt aMenuCommandId,
                MPbk2ContactUiControl& aControl,
                MVPbkStoreContact& aStoreContact );

    private: // Data
        /// Own: Contact link
        MVPbkContactLink* iContactLink;
        /// Own: Retrieve contact operation
        MVPbkContactOperationBase* iRetriever;
        /// Own: Retrieved store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Call type selector
        CPbk2CallTypeSelector* iSelector;
        /// Ref: Field property array
        CPbk2FieldPropertyArray* iFieldPropertyArray;
        /// Own: Index of the focused field
        TInt iFocusedFieldIndex;
        /// Own: Menu command id
        TInt iMenuCommandId;
        /// Ref: Contact UI control
        MPbk2ContactUiControl* iControl;
        /// Own: Menu filtering flags
        TUint iFlags;
        /// Own: Check store state
        CPbk2StoreStateChecker* iStoreStateChecker;
        /// Own: Next action
        TNextAction iState;
    };

#endif // CPBK2AIWINTERESTITEMPOC_H

// End of File
