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
* Description: Command for opening MyCard.
*
*/


#ifndef CPBK2OPENMYCARDCMD_H
#define CPBK2OPENMYCARDCMD_H

// INCLUDES
#include <e32base.h>
#include <MPbk2Command.h>

// FORWARD DECLARATIONS
class MPbk2ContactUiControl;
class CPbk2FieldPropertyArray;
class CPbk2StoreSpecificFieldPropertyArray;
class CPbk2PresentationContact;
class CPbk2MyCard;
class MVPbkContactLink;

//Cca
class MCCAParameter;
class MCCAConnection;

// CLASS DECLARATION

/**
 *
 */
class CPbk2OpenMyCardCmd : public CActive,
                           public MPbk2Command
    {
    public: // Construction and destruction

        /**
         * Creates a new instance of this class.
         *
         * @param a reference to a pointer to CCA client connection object
         * @param Pbk2 Contact Ui control
         * @return  A new instance of this class.
         */
        static CPbk2OpenMyCardCmd* NewL(
            MCCAConnection*& aCCAConnection,
            CPbk2MyCard* aMyCard,
            MPbk2ContactUiControl* aUiControl = NULL );

        /**
         * Destructor.
         */
        ~CPbk2OpenMyCardCmd();

    public: // Interface
        void SetViewUid( TUid aViewUid );
        
    public: //From MPbk2Command
        void ExecuteLD();
        void ResetUiControl( MPbk2ContactUiControl& aUiControl );
        void AddObserver( MPbk2CommandObserver& aObserver );

    private: //Construction
        CPbk2OpenMyCardCmd(
            MPbk2ContactUiControl* aUiControl,
            CPbk2MyCard* aMyCard,
            MCCAConnection*& aCCAConnection );
        void ConstructL();

    private: // Implementation

        /**
         * Set contact launch data (launch mycard from link)
         */
        void SetContactDataL( MCCAParameter& aParam, MVPbkContactLink* aLink );
        
        /**
         * Set contact launch data (launch mycard from data model)
         * If aContact is not provided then empty model will be delivered.
         * Empty model means that mycard does not exist and needs to be created.
         */
        void SetContactDataL( MCCAParameter& aParam, CPbk2PresentationContact* aContact = NULL );

        void LaunchCcaL(); // Sync
		void IssueRequest();
		
		/**
		 * Create presentation contact from mycard.
		 */
		CPbk2PresentationContact* PresentationContactL();

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private: // Data structures
        /// Process states
        enum TProcessState
            {
            ELaunching,
            ERunning
            };

    private: // Data

        //Ref, not owned
        MPbk2CommandObserver* iCommandObserver;
        //Ref, not owned
        MPbk2ContactUiControl *iUiControl;
        /// Ref to ptr: for CCA, not owned
        MCCAConnection*& iConnectionRef;
        // Own: The internal state of the command
        TProcessState iState;
        /// Requested view uid for CCA 
        TUid iViewUid;
        /// Not own. My card instance
        CPbk2MyCard* iMyCard;
        /// Own. Field properties for presentation contact
        CPbk2FieldPropertyArray* iFieldProperties;
        /// Own. Field properties for presentation contact
        CPbk2StoreSpecificFieldPropertyArray* iSpecificFieldProperties;
        /// Own. Presentation contact of mycard
        CPbk2PresentationContact* iPresentationContact;
    };

#endif // CPBK2OPENMYCARDCMD_H

// End of File
