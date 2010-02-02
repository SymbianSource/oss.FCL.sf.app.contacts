/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Remove speed dial command object.
*
*/


#ifndef CPBK2REMOVESPEEDDIALCMD_H
#define CPBK2REMOVESPEEDDIALCMD_H

// INCLUDES
#include <e32base.h>
#include <MVPbkSingleContactOperationObserver.h>
#include <MVPbkContactAttributeManager.h>
#include <MVPbkContactObserver.h>
#include <MVPbkContactStoreObserver.h>
#include "MPbk2Command.h"

// FORWARD DECLARATIONS
class CAknQueryDialog;
class MPbk2ContactUiControl;
class MPbk2CommandObserver;
class MVPbkContactOperationBase;
class CPbk2ApplicationServices;

// CLASS DECLARATION

/**
 * Remove speed dial command.
 */
NONSHARABLE_CLASS(CPbk2RemoveSpeedDialCmd) : 
        public CBase,
        public MPbk2Command,
        public MVPbkContactStoreObserver,
        public MVPbkSingleContactOperationObserver,
        public MVPbkContactObserver,
        public MVPbkSetAttributeObserver
    {
    public: // Interface
        /**
         * Creates a new instance of this class.
         * @param aParams command parameters
         * @return a newly created command object
         */
        static CPbk2RemoveSpeedDialCmd* NewL(MPbk2ContactUiControl& aUiControl);

        ~CPbk2RemoveSpeedDialCmd();

    public:  // From MPbk2Command
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver);
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    public: // From MVPbkContactStoreObserver
    	void StoreReady(MVPbkContactStore& aContactStore);
    	void StoreUnavailable(MVPbkContactStore& aContactStore, 
    	                TInt aReason);
    	void HandleStoreEventL(
    	                MVPbkContactStore& aContactStore, 
    	                TVPbkContactStoreEvent aStoreEvent);
            
    public: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);

    public: // From MVPbkSetAttributeObserver
        void AttributeOperationComplete(
            MVPbkContactOperationBase& aOperation);
        void AttributeOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError);
            
    public: // From MVPbkContactObserver
        void ContactOperationCompleted(TContactOpResult aResult);
        void ContactOperationFailed
            (TContactOp aOpCode, TInt aErrorCode, TBool aErrorNotified);    

    private:  // Implementation
        CPbk2RemoveSpeedDialCmd(MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void FinishCommand(TInt aError);
        TInt ToStoreFieldIndexL(
		        TInt aPresIndex,
		        const MVPbkStoreContact* aStoreContact );
        TBool HasSpeedDialL();
        
    private:  // Data
        /// Ref: Ui control
        MPbk2ContactUiControl* iUiControl;
        /// Ref: command observer
        MPbk2CommandObserver* iCommandObserver;
        /// Own: remove / retrieve operation
        MVPbkContactOperationBase* iOperation;
        /// Own: Store Contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
        /// Own: Current contact link
        MVPbkContactLink* iContactLink;
    };


#endif // CPBK2REMOVESPEEDDIALCMD_H

// End of File
