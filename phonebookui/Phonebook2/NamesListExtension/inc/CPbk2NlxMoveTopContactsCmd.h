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
* Description:  Phonebook 2 Organize top contacts command
*
*/


#ifndef CPbk2NlxMoveTopContactsCmd_H
#define CPbk2NlxMoveTopContactsCmd_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <CVPbkTopContactManager.h>
#include <MVPbkOperationObserver.h>
#include <MPbk2ProcessDecorator.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class MVPbkContactOperationBase;
class CPbk2NlxReorderingModel;
class MVPbkContactViewBase;
class MVPbkContactLink;
// CLASS DECLARATION

/**
 * Phonebook 2 move top contacts command object.
 */
class CPbk2NlxMoveTopContactsCmd : 
    public CActive,
    public MPbk2Command,
    private MVPbkOperationObserver,
    private MVPbkOperationErrorObserver,
    private MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @return  A new instance of this class.
         */
        static CPbk2NlxMoveTopContactsCmd* NewL( 
            MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2NlxMoveTopContactsCmd();
        
    public: //From MPbk2Command
        void ExecuteLD();
        void ResetUiControl( MPbk2ContactUiControl& aUiControl );
        void AddObserver( MPbk2CommandObserver& aObserver );

    private: // From MVPbkOperationObserver
        void VPbkOperationCompleted(MVPbkContactOperationBase*);
    private: // From MVPbkOperationErrorObserver        
        void VPbkOperationFailed( MVPbkContactOperationBase*,TInt aError );
        
    private: //Construction
        CPbk2NlxMoveTopContactsCmd( 
            MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        
    private: //From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);
        
    public: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed(
                TInt aCancelCode );    
        
    private:
        /// Command states
        enum TState
            {
            EShowMoveDlg,
            ESavingTopContacts,
            ECompleteCommand
            };
        
    private: //implementation
    	void ShowDlgL();
    	void FinishCommand(TInt aError);
    	void IssueRequest(TState aNextState);
        void ShowWaitNoteL();
        
    private: // Data
        //Ref: Command observer
        MPbk2CommandObserver* iCommandObserver;
        //Ref: Current ui control
        MPbk2ContactUiControl *iUiControl;        
        //Own: Provides top contacts services       
        CVPbkTopContactManager* iVPbkTopContactManager;
        //Own: listbox model that supports reordering
        //		uses TopContactManager to handle top contacts
        CPbk2NlxReorderingModel* iReorderingModel;
        /// Own: State of the command
        TState iState;
        /// Own: Top contacts view
        MVPbkContactViewBase* iView;
        /// Own: Top contact manager oepration
        MVPbkContactOperationBase* iTopContactOperation;
        /// Own: focused contact from move dialog, used to keep the same contact
        ///      focused when back to the main namelist view
        const MVPbkContactLink* iFocusedContact;
        /// Own: Decorator for the wait note process
        MPbk2ProcessDecorator* iDecorator;
    };

#endif // CPbk2NlxMoveTopContactsCmd_H

// End of File
