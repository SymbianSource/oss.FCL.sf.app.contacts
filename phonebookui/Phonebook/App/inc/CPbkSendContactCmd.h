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
*      Contact(s) send command object.
*
*/


#ifndef __CPbkSendContactCmd_H__
#define __CPbkSendContactCmd_H__

// INCLUDES
#include <e32base.h>
#include <cntdef.h>
#include <MPbkCommand.h>
#include <MPbkBackgroundProcess.h>
#include <TPbkSendingParams.h>

// FORWARD DECLARATIONS
class CPbkContactItem;
class CPbkWaitNoteWrapperBase;  
class CPbkvCardConverter;
class CEikonEnv;
class CPbkContactEngine;
class CBCardEngine;
class TPbkContactItemField;

// CLASS DECLARATION


/**
 * Contact(s) send command object.
 */
class CPbkSendContactCmd :
        public CBase,
        public MPbkCommand,
        private MPbkProcessObserver
    {
    public: // Interface
        /**
         * Static constructor.
         * @param aParams, sending params
		 * @param aEngine reference to phonebook engine
		 * @param aBCardEng reference to business card engine         
         * @param aContact the contact to send
         * @param aField which field of the contact is focused, can be null
         * @return a new send contact command object.
         */
        static CPbkSendContactCmd* NewL
			(TPbkSendingParams aParams,
			CPbkContactEngine& aEngine, CBCardEngine& aBCardEng,
            TContactItemId aContactId,
            TPbkContactItemField* aField);

		/**
         * Static constructor.
         * @param aParams, sending params
		 * @param aEngine reference to phonebook engine
		 * @param aBCardEng reference to business card engine         
         * @param aContacts contacts to send
         * @return a new send contact command object.
         */
        static CPbkSendContactCmd* NewL
            (
            TPbkSendingParams aParams,
            CPbkContactEngine& aEngine, CBCardEngine& aBCardEng,
		    const CContactIdArray& aContacts);

        /**
         * Destructor.
         */
        ~CPbkSendContactCmd();

    public:  // from MPbkCommand
        void ExecuteLD();
        void AddObserver( MPbkCommandObserver& aObserver );
        
    private:  // from MPbkProcessObserver
        void ProcessFinished(MPbkBackgroundProcess& aProcess);

    private:  // Implementation
        CPbkSendContactCmd (
            TPbkSendingParams aParams,
            CPbkContactEngine& aEngine,
            TContactItemId aContactId, const CContactIdArray* aContacts,
            TPbkContactItemField* aField);
        void ConstructL(CBCardEngine& aBCardEng);
        void SendvCardsLD();
        static TInt SendvCardsLD(TAny* aThis);
        TBool AnyThumbnailsL() const;
        TBool HasThumbnail(CPbkContactItem& aItem) const;
        TBool IsSmsMtmL() const;
        TInt SelectionListL() const;
        void MapSelection(TInt& aSelection, TInt aShownMenu);
        TInt SelectSentDataL();
        
        /**
         * Filter and display SendUI query
         * @return TUid Uid of the selected sending service.
         */
        TUid ShowSendQueryL();

    private:  // Data
        /// Ref: contact engine
        CPbkContactEngine& iEngine;
        /// Own: the send command's service uid
		TUid iMtmUid;
		/// Sending  params
	    TPbkSendingParams iParams;
        /// Own: wait note wrapper
        CPbkWaitNoteWrapperBase* iWaitNoteWrapper;
		/// Own: vCard converter
        CPbkvCardConverter* iConverter;
		/// Ref: Eikon enviroment
		CEikonEnv* iEikEnv;
		/// Ref: Contact item id
		TContactItemId iContactId;
		/// Ref: Contact id array
		const CContactIdArray* iContacts;
        /// Ref: Focused contact field    
        TPbkContactItemField* iField;
		/// Own: vCard sender
        CIdle* iVcardSender;
        /// Own: set to ETrue in destructor entry
        TBool iUnderDestruction;
        /// Ref: Command observer
        MPbkCommandObserver* iObserver;
    };

#endif // __CPbkSendContactCmd_H__

// End of File
