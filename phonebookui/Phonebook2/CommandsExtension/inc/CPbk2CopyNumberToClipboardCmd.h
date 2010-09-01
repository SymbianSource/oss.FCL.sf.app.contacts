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
* Description:  A command for clipboard copy
*
*/


#ifndef CPBK2COPYNUMBERTOCLIPBOARDCMD_H
#define CPBK2COPYNUMBERTOCLIPBOARDCMD_H

// INCLUDES
#include <e32base.h>

#include <MPbk2Command.h>
#include <MVPbkSingleContactOperationObserver.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MPbk2CommandObserver;
class MVPbkStoreContactField;
class CPbk2ApplicationServices;

// CLASS DECLARATIONS

NONSHARABLE_CLASS(CPbk2CopyNumberToClipboardCmd) :
        public CBase,
        public MPbk2Command,
        private MVPbkSingleContactOperationObserver
    {
    public: 
        static CPbk2CopyNumberToClipboardCmd* NewL(MPbk2ContactUiControl& aUiControl);
        
        ~CPbk2CopyNumberToClipboardCmd();
        
    public:  // From MPbk2Command
        void ExecuteLD();
        void AddObserver(MPbk2CommandObserver& aObserver);
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);

    private: // From MVPbkSingleContactOperationObserver
        void VPbkSingleContactOperationComplete(
                MVPbkContactOperationBase& aOperation,
                MVPbkStoreContact* aContact);
        void VPbkSingleContactOperationFailed(
                MVPbkContactOperationBase& aOperation, 
                TInt aError);

    private: // implementation
        CPbk2CopyNumberToClipboardCmd(
            MPbk2ContactUiControl& aUiControl);
        void ConstructL();
        void CopyNumberToClipboardL();
        MVPbkStoreContactField* FocusedFieldLC
            ( MVPbkStoreContact& aStoreContact );
                
    private: // Data
        /// Ref: ui control
        MPbk2ContactUiControl* iUiControl;
        /// Own: retrieve operation
        MVPbkContactOperationBase* iRetrieveOperation;   
        /// Ref: command observer
        MPbk2CommandObserver* iObserver;     
        /// Own: store contact
        MVPbkStoreContact* iStoreContact;
        /// Own: Application Services pointer
        CPbk2ApplicationServices* iAppServices;
    };

#endif // CPBK2COPYNUMBERTOCLIPBOARDCMD_H   
// End of file
