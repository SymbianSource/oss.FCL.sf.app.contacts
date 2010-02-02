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
* Description: 
*      Play voice tag command object.
*
*/


#ifndef CPVTPLAYVOICETAGCMD_H
#define CPVTPLAYVOICETAGCMD_H

// INCLUDES
#include <e32base.h>
#include <mpbk2command.h>
#include <mvpbkcontactattributemanager.h>

// FORWARD DECLARATIONS
class MPbk2CommandObserver;
class CPvtVoiceDialHandler;
class MPbk2ContactUiControl;
class MVPbkStoreContact;
class MVPbkStoreContact;
class MVPbkContactOperationBase;

// CLASS DECLARATION

/**
 * Play voice tag command object.
 */
class CPvtPlayVoiceTagCmd : 
        public CActive,
        public MPbk2Command,
        public MVPbkMultiAttributePresenceObserver
    {
    public: // Constructors and destructor
        /**
         * Creates a new instance of this class.
         *
         * @param aUiControl    Contact UI control.
         * @return  A new instance of this class.
         */
        static CPvtPlayVoiceTagCmd* NewL(
                MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPvtPlayVoiceTagCmd();
        
    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError( TInt aError );

    public: // From MPbk2Command
        void ExecuteLD();
        void AddObserver( MPbk2CommandObserver& aObserver );
        void ResetUiControl(MPbk2ContactUiControl& aUiControl);
        
    public: // From MVPbkMultiAttributePresenceObserver
        void AttributePresenceOperationComplete(
            MVPbkContactOperationBase& aOperation, 
            MVPbkStoreContactFieldCollection* aResult);
            
        void AttributePresenceOperationFailed(
            MVPbkContactOperationBase& aOperation, TInt aError);

    private: // Implementation
        CPvtPlayVoiceTagCmd();
        void ConstructL( MPbk2ContactUiControl& aUiControl );
        void FindVoiceTagFieldsL(
                const MVPbkStoreContact& aContact );
        void PlayVoiceTagCmdL();
        void IssueRequest();
        void FinishProcess();
        void ShowNoVoiceTagNoteL();
        
    protected: // Data structures
        /// Process states
        enum TState
            {
            ECommandInitiliaze,
            EPlayVoiceTagCmd,
            ECommandFinished
            };        

    private: // Data
        /// Own: State of the command
        TState iState;
        /// Ref: Command observer
        MPbk2CommandObserver* iObserver;
        /// Ref: Store contact
        const MVPbkStoreContact* iContact;
        /// Own: Voice dial handler
        CPvtVoiceDialHandler* iVoiceDialHandler;
        /// Own: Focused field index
        TInt iFocusedFieldIndex;
        /// Own: Attrribute operation
        MVPbkContactOperationBase* iOperation;
        /// Own: Attribute operation's result
        MVPbkStoreContactFieldCollection* iResult;
    };

#endif // CPVTPLAYVOICETAGCMD_H

// End of File
