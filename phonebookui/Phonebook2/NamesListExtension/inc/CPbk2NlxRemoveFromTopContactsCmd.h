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
* Description:  Phonebook 2 remove from top contacts command
*
*/


#ifndef CPBK2NLXREMOVEFROMTOPCONTACTSCMD_H
#define CPBK2NLXREMOVEFROMTOPCONTACTSCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>
#include <MVPbkOperationObserver.h>

#include "CPbk2DelayedWaitNote.h"

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CVPbkTopContactManager;
class MVPbkContactOperationBase;

// CLASS DECLARATION

/**
 * Phonebook 2 remove from top contacts command object.
 */
class CPbk2NlxRemoveFromTopContactsCmd : 
    public CActive,
    public MPbk2Command,
    public MVPbkOperationObserver,
    public MVPbkOperationErrorObserver,
    public MPbk2ProcessDecoratorObserver
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param Pbk2 Contact Ui control
         * @return  A new instance of this class.
         */
        static CPbk2NlxRemoveFromTopContactsCmd* NewL(
            MPbk2ContactUiControl& aUiControl );

        /**
         * Destructor.
         */
        ~CPbk2NlxRemoveFromTopContactsCmd();
    public: // From MVPbkOpeartionObserver
	    void VPbkOperationCompleted( MVPbkContactOperationBase* aOperation );
    public: // From MVPbkOpeartionErrorObserver	    
	    void VPbkOperationFailed(
	            MVPbkContactOperationBase* aOperation ,
	            TInt aError );
        
    public: //From MPbk2Command
        void ExecuteLD();
        void ResetUiControl( MPbk2ContactUiControl& aUiControl );
        void AddObserver( MPbk2CommandObserver& aObserver );

    private: //Construction
        CPbk2NlxRemoveFromTopContactsCmd( 
            MPbk2ContactUiControl& aUiControl );
        void ConstructL();
        
    private://From CActive
        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);
        
    private: // From MPbk2ProcessDecoratorObserver
        void ProcessDismissed( TInt aCancelCode );    
        
    private: // Implementation        
        void RemoveTopContactL();
        void ShowDelayedWaitNoteL();

    private: // Data
        //Ref
        MPbk2CommandObserver* iCommandObserver;
        //Ref
        MPbk2ContactUiControl *iUiControl;   
        //Own:
        MVPbkContactOperationBase* iVPbkContactOperationBase;
        //Own:        
        CVPbkTopContactManager* iVPbkTopContactManager;
        
        CPbk2DelayedWaitNote* iDelayedWaitNote;
    };

#endif // CPBK2NLXREMOVEFROMTOPCONTACTSCMD_H

// End of File
