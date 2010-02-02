/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      Send message command object.
*	   Opens a message editor for writing and sending a message to a
*      contact.
*
*/


#ifndef __CPbkSendMultipleMessageCmd_H__
#define __CPbkSendMultipleMessageCmd_H__

// INCLUDES
#include <e32base.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>
#include <TPbkSendingParams.h>


// FORWARD DECLARATIONS
class CPbkContactEngine;
class CContactIdArray;
class TPbkContactItemField;
class CPbkAttachmentFile;


// CLASS DECLARATION

/**
 * Call command object.
 */
class CPbkSendMultipleMessageCmd :
        public CBase,
        public MPbkCommand,
        private MPbkProcessObserver
    {
    public: // Interface
		/**
         * Static constructor.        
         * @param aEngine, for retrieving contact info
         * @param aParams, sending params 
         * @param aContacts, array of contact ids where to send message to.
         * @param aFocusedField currently focused field. May be NULL if caller
         *                      doesn't have field-level focus. If field is 
         *                      applicable address field it is used directly
         *                      as the recipient address.
         * @param aUseDefaultDirectly   if ETrue and a default address is found
         *                              from the contact the default address is 
         *                              used without presenting address 
         *                              selection list.
         * @return a new send message command object.
         */
		static CPbkSendMultipleMessageCmd* NewL (
            CPbkContactEngine& aEngine,
            TPbkSendingParams aParams,
    		const CContactIdArray& aContacts,
    		const TPbkContactItemField* aFocusedField,
    		TBool aUseDefaultDirectly);

        /**
         * Destructor.
         */
        ~CPbkSendMultipleMessageCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        void AddObserver( MPbkCommandObserver& aObserver );
        
    public:  // from MPbkProcessObserver
        void ProcessFinished(MPbkBackgroundProcess& aProcess);

    private:  // Implementation
        CPbkSendMultipleMessageCmd (
            CPbkContactEngine& aEngine,
            TPbkSendingParams aParams,
    		const CContactIdArray& aContacts,
    		const TPbkContactItemField* aFocusedField,
    		TBool aUseDefaultDirectly);
        void ConstructL();
        
    private: //New functions    
        /**
         * Filter and display SendUI query               
         * @return TUid Uid of the selected sending service.
         */        
        TUid ShowWriteQueryL();

    private:  // Data
        // Ref: Pbk contact engine  
        CPbkContactEngine& iEngine;          
	    /// Sending params
	    TPbkSendingParams iParams;
        // Ref: Array of contact ids to be send
        const CContactIdArray& iContacts;
		/// Own: send mtm uid
	    TUid iMtmUid;
		/// Ref: the currently focused field
		const TPbkContactItemField* iFocusedField;
		/// Own: if true the possible default address is used directly
		TBool iUseDefaultDirectly;
		//Own: Postcard temporary attachment file
		CPbkAttachmentFile* iAttachmentFile;
		//Ref: Command Observer
		MPbkCommandObserver* iObserver;
    };


#endif // __CPbkSendMultipleMessageCmd_H__

// End of File
